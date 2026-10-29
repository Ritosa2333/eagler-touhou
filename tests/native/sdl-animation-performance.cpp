// Same program runs against the before/after sampler. Retail ANMs are optional
// local inputs; only hashes and timings are written to the validation receipt.
#include "Animation.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <new>
using namespace th12host;
namespace {
bool counting=false;size_t allocations=0;
u64 digest=1469598103934665603ull;
void require(bool ok,const char* message){if(!ok){std::fprintf(stderr,"%s\n",message);std::exit(1);}}
template<class T> void hash(T value){const auto* p=reinterpret_cast<const u8*>(&value);for(size_t i=0;i<sizeof value;++i){digest^=p[i];digest*=1099511628211ull;}}
void hash_frame(const Track::Frame& f){
    const auto& a=f.a;
    for(i32 n:{a.bank,a.sprite,a.script,a.layer,a.mode,a.anchor_x,a.anchor_y,a.blend,a.geometry_count})hash(n);
    for(float n:{a.x,a.y,a.z,a.rx,a.ry,a.rotation,a.sx,a.sy,a.u,a.v,a.uv_scale_x,a.uv_scale_y,a.geometry_radius,a.geometry_width,
        a.r,a.g,a.b,a.alpha,a.r2,a.g2,a.b2,a.alpha2,a.native_width,a.native_height})hash(n);
    for(bool n:{a.has_primary,a.visible,a.point,a.rotate})hash(n);
    for(float n:a.primary)hash(n);
    hash(u32(f.children.size()));for(const auto& c:f.children)hash_frame(c);
}
double milliseconds(std::chrono::steady_clock::time_point start){return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();}
std::shared_ptr<AnmFile> fixture(){
    auto file=std::make_shared<AnmFile>();file->scripts.resize(6);
    const auto command=[](u16 op,std::vector<i32> ints={},std::vector<float> floats={}){AnmCommand c;c.op=op;c.ints=std::move(ints);c.floats=std::move(floats);return c;};
    file->scripts[0].commands={command(3,{1}),command(63),command(64,{2}),command(3,{8}),command(63)}; // stopped, settled, interruptible
    file->scripts[1].commands={command(3,{2}),command(54,{}, {0.125f,-0.25f}),command(63)}; // stopped, moving
    file->scripts[2].commands={command(3,{3}),command(55,{0,100}),command(63)}; // stopped, still interpolating
    file->scripts[3].commands={command(3,{4}),command(88,{1}),command(63)}; // stopped, moving child
    file->scripts[4].commands={command(3,{5}),command(2)}; // ended, visible
    file->scripts[5].commands={command(87,{1}),command(40,{0,100}),command(63)}; // count bindings via display RNG
    return file;
}
void verify(std::shared_ptr<const AnmFile> file,i32 bank,i32 script,const SampleOptions& o){
    AnimationSampler stepped,jumped;stepped.files=jumped.files=[file](i32){return file;};
    u32 r0=7,r1=7;stepped.display_random32=[&]{return r0=r0*1664525+1013904223;};jumped.display_random32=[&]{return r1=r1*1664525+1013904223;};
    std::unique_ptr<Track> reference;
    for(i32 age:{0,1,3,15,59,119,17,18,120}){
        if(!reference||age<i32(reference->tick_count())-1)reference=std::make_unique<Track>(stepped,file,bank,script,std::nullopt,o);
        while(reference->tick_count()<=u32(age))reference->tick();
        const auto* slow=reference->frame();
        const auto* fast=jumped.sample(bank,script,age,{},o);
        const auto prior=digest;hash_frame(*slow);const auto expected=digest;digest=prior;hash_frame(*fast);
        require(expected==digest&&r0==r1,"ANM sparse/rewind output or display RNG differs");
    }
}
}
void* operator new(std::size_t n){if(counting)++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
int main(int argc,char** argv){
    auto file=fixture();
    for(i32 script=0;script<5;++script)verify(file,0,script,{});
    // Interrupts after a large age skip still run from the saved stopped clock.
    AnimationSampler owner;Track moving(owner,file,0,1,{},{});moving.sample(2000);require(moving.state.sx==251.125f,"stopped velocity was skipped");
    Track fading(owner,file,0,2,{},{});fading.sample(2000);require(fading.state.alpha==0,"stopped interpolation was skipped");
    Track edited(owner,file,0,0,{},{});edited.sample(10000);edited.state.x=37;require(edited.sample(20000).a.x==37,"public state edit was skipped");
    require(edited.interrupt(2),"late stopped interrupt missing");require(edited.sample(20010).a.sprite==8&&edited.tick_count()==20011,"late interrupt was skipped");
    Track wrapping(owner,file,0,0,{},{});wrapping.state.u=5;wrapping.state.v=-4;
    require(wrapping.sample(10000).a.u==0&&wrapping.frame()->a.v==0,"stopped UV normalization was skipped");
    AnimationSampler cached;cached.files=[file](i32){return file;};SampleOptions option;option.entity=91;u32 bindings=0;cached.display_random32=[&]{return ++bindings;};
    cached.begin_frame();cached.sample(0,5,4,{},option);cached.sample(0,5,4,{},option);cached.end_frame();require(bindings==1,"duplicate sample rebound a live track");
    cached.begin_frame();cached.end_frame(); // no draw: evict
    cached.begin_frame();cached.sample(0,5,4,{},option);cached.end_frame();require(bindings==2,"unused track was retained");
    cached.reset();cached.begin_frame();cached.sample(0,5,4,{},option);cached.end_frame();require(bindings==3,"reset retained an old track");
    size_t files=0,scripts=0;
    if(argc>1){
        std::vector<std::filesystem::path> names;for(const auto& e:std::filesystem::directory_iterator(argv[1]))if(e.path().extension()==".anm")names.push_back(e.path());std::sort(names.begin(),names.end());
        for(const auto& name:names){
            std::ifstream in(name,std::ios::binary);std::vector<u8> bytes(std::istreambuf_iterator<char>(in),{});auto retail=std::make_shared<AnmFile>();std::string error;
            require(parse_anm(name.stem().string(),bytes,*retail,error,false),error.c_str());++files;
            for(i32 id=0;id<i32(retail->scripts.size());++id){++scripts;verify(retail,0,id,{});SampleOptions interrupted;interrupted.seed=19;interrupted.interrupts={2,7,3};verify(retail,1,id,interrupted);}
        }
    }
    AnimationSampler steady;steady.files=[file](i32){return file;};
    const auto draw=[&](int age){steady.begin_frame();for(u32 i=1;i<=512;++i){SampleOptions o;o.entity=i;steady.sample(0,0,age,{},o);}steady.end_frame();};
    draw(0);allocations=0;counting=true;auto start=std::chrono::steady_clock::now();
    for(int age=1;age<=600;++age)draw(age);
    const double steady_ms=milliseconds(start);counting=false;const size_t steady_allocations=allocations;
    AnimationSampler chained;chained.files=[file](i32){return file;};
    const auto draw_chained=[&](int age){chained.begin_frame();for(u32 i=1;i<=512;++i){SampleOptions o;o.entity=i;o.interrupts={2,7,3};chained.sample(0,0,age,{},o);}chained.end_frame();};
    draw_chained(0);allocations=0;counting=true;start=std::chrono::steady_clock::now();
    for(int age=1;age<=600;++age)draw_chained(age);
    const double chained_ms=milliseconds(start);counting=false;const auto chained_allocations=allocations;
    // Long interrupt sequences keep their tail in both options and cache keys.
    SampleOptions long_a,long_b;long_a.interrupts={2,7,3,8,4,9};long_b.interrupts={2,7,3,8,4,10};
    require(!(long_a.interrupts==long_b.interrupts),"interrupt tails were truncated");
    auto copied=long_a.interrupts;require(copied==long_a.interrupts,"long interrupt copy differs");
    copied=copied;require(copied==long_a.interrupts,"interrupt self assignment differs");
    auto moved=std::move(copied);require(moved==long_a.interrupts,"long interrupt move differs");
    moved={2,7,3};auto short_copy=moved;require(moved==short_copy&&moved.size()==3,"short interrupt copy differs");
    moved={};require(moved.empty(),"interrupt empty assignment differs");
    require(chained.sample(0,0,1,{},long_a)!=chained.sample(0,0,1,{},long_b),"distinct long interrupt keys were aliased");
    start=std::chrono::steady_clock::now();
    for(int i=0;i<60;++i){Track late(owner,file,0,0,{},{});late.sample(10000);require(late.tick_count()==10001,"settled age differs");hash_frame(*late.frame());}
    const double late_ms=milliseconds(start);
    std::printf("{\"files\":%zu,\"scripts\":%zu,\"outputHash\":\"%016llx\",\"steadyFrames\":600,\"steadyTracks\":512,\"steadyMs\":%.3f,\"steadyAllocations\":%zu,\"lateBindings\":60,\"lateAge\":10000,\"lateMs\":%.3f,\"chainedMs\":%.3f,\"chainedAllocations\":%zu}\n",files,scripts,(unsigned long long)digest,steady_ms,steady_allocations,late_ms,chained_ms,chained_allocations);
}
