// Test-only original CPU oracle; the game entry point is never executed.
#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../../sprite_renderer/named_spawn.hpp"
#include "../../sprite_renderer/pool.hpp"
#include "../../sprite_renderer/binding.hpp"
#include "../../sprite_renderer/anm_vm.hpp"
#include "../../core_scheduler/scheduler.hpp"
namespace s=th20::source::sprite;
namespace q=th20::source::scheduler;
namespace {
s::Controller* controller=nullptr;float clock_value=1;
template<class R,class... A> R cpu(std::uint32_t va,void* self,A... args) {using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
[[noreturn]] void unexpected(const char* name) {throw std::runtime_error(std::string("Unexercised test dependency: ")+name);}
}
namespace th20::source::sprite::anm_environment {
float& clock_scale(){return clock_value;}const float* timer_rate(){return &clock_value;}
AnmInstruction* script(Animation& a){return script_start(*controller,a);}bool gameplay_frozen(){return false;}
std::uint32_t random_next(){unexpected("random_next");}std::uint32_t random_bounded(std::uint32_t){unexpected("random_bounded");}
float random_unit(){unexpected("random_unit");}float random_signed_unit(){unexpected("random_signed_unit");}float camera_component(std::int32_t){unexpected("camera_component");}
void assign_sprite(Animation&,std::int32_t){unexpected("assign_sprite");}void set_layer(Animation& a,std::int32_t layer){set_animation_layer(a,layer);}
void add_camera_offset(Vec3&){unexpected("add_camera_offset");}void calculate_corners(Animation&,Vec3(&)[4]){unexpected("calculate_corners");}
float screen_scale(){return 1.f;}std::int32_t screen_offset(unsigned,unsigned){return 0;}
void* allocate_geometry(std::uint32_t){unexpected("allocate_geometry");}
std::uint32_t spawn_child(Animation&,std::int32_t,std::uint32_t){unexpected("spawn_child");}
std::uint32_t spawn_detached(Animation&,std::int32_t,std::uint32_t){unexpected("spawn_detached");}
Animation& lookup_animation(std::uint32_t){unexpected("lookup_animation");}void spawn_effect(Animation&,std::int32_t){unexpected("spawn_effect");}
}
int wmain(int argc,wchar_t** argv) {try {
    if(argc!=3)throw std::runtime_error("Usage: th20_named_spawn_cpu_compare ORIGINAL.exe OUTPUT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");
    const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
    controller=static_cast<s::Controller*>(VirtualAlloc(nullptr,sizeof(s::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!controller)throw std::bad_alloc();auto& c=*controller;
    *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=controller;*reinterpret_cast<void**>(mapped_image_base+0x1ba828)=nullptr;
    *reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=reinterpret_cast<float*>(mapped_image_base+0x1aefe4);*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=1;
    for(const auto& item:pe.imports)if(item.name=="GetCurrentThreadId")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
    for(unsigned index:{1u,9u}){auto* lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240+index*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;}
    std::mt19937 rng(0x450cb0);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,const void* a,const void* b,std::size_t size) {if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<20){std::ostringstream text;text<<name;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){text<<" +0x"<<std::hex<<i;break;}failures.push_back(text.str());}};
    struct Snapshot {std::vector<std::uint8_t> bytes;std::uint32_t generation; s::AnimationLink free;
        Snapshot(s::Controller& c):bytes(0x48+16*sizeof(s::PooledAnimation)),generation(c.field_7d40e88),free(c.free_sentinel){std::memcpy(bytes.data(),c.lists,bytes.size());}
        void restore(s::Controller& c)const {std::memcpy(c.lists,bytes.data(),bytes.size());c.field_7d40e88=generation;c.free_sentinel=free;}
    };
    for(unsigned test=0;test<4096;++test) {
        for(auto& list:c.lists)s::initialize_animation_list(list);c.free_sentinel={};c.field_7d40e88=rng();c.field_6c4=test%7==0?1:0;
        for(unsigned i=0;i<16;++i){std::memset(&c.pool[i],0,sizeof(c.pool[i]));s::construct_pooled_animation(c.pool[i]);c.pool[i].index=i;c.pool[i].animation.index=i;s::initialize_animation_link(c.pool[i].free_link,&c.pool[i].animation);}
        // The original accepts group1 at +710. Preserve that addressing while
        // leaving pool item0 unavailable so its storage can hold those heads.
        auto* extra=reinterpret_cast<s::AnimationList*>(&c.pool[0]);for(unsigned i=0;i<3;++i)s::initialize_animation_list(extra[i]);
        for(unsigned i=1;i<8;++i)q::insert_after(reinterpret_cast<q::Link&>(c.free_sentinel),reinterpret_cast<q::Link&>(c.pool[i].free_link));
        s::AnmInstruction instruction{2,8,0,0};s::AnmInstruction* scripts[]{&instruction};s::Animation prototype{};s::construct_animation(prototype);s::reset_animation_state(prototype);
        prototype.base.fields_10_28[6]=0;prototype.base.flags[1]=rng()&0x400200u;prototype.base.flags[2]=rng()&0x1ffffffu;
        prototype.base.fields_10_28[1]=rng()%64;prototype.vector_5bc={.25f,1.7f,-4.1f};prototype.base.vector_2c={2.8f,4.9f,-2.1f};
        s::AnimationFile file{};file.templates=&prototype;file.scripts=scripts;file.fields_5c[3]=rng();c.files[0]=&file;
        // Original MSVC pmr strings have the same validated layout; exercise
        // both short and allocated stems, plus no-name and mismatched paths.
        file.stem=test%2?"sig":"a_very_long_animation_stem_name";
        const char* name=test%3==0?nullptr:(test%3==1?file.stem.c_str():"different");
        const std::int32_t layer=static_cast<std::int32_t>(rng()%80)-10;const std::uint32_t flags=test%32;
        const s::Vec3 position{float(int(rng()%100000)-50000)/79.f,float(int(rng()%100000)-50000)/31.f,-3.7f};const auto* location=test%2?&position:nullptr;
        const float rotation=float(int(rng()%1000)-500)/31.f;const Snapshot before(c);const auto counter=file.fields_5c[3];
        std::uint32_t expected=0xdeadbeefu;s::Animation* expected_animation=nullptr;const bool wrapper=test%11==0;
        FloatingEnvironment::prepare();if(wrapper)cpu<void>(0x450c70,&file,&expected,name,0,layer,&expected_animation);else cpu<void>(0x450cb0,&file,&expected,name,0,location,rotation,layer,flags,&expected_animation);
        const Snapshot after(c);const auto expected_counter=file.fields_5c[3];before.restore(c);file.fields_5c[3]=counter;
        std::uint32_t actual=0xdeadbeefu;s::Animation* actual_animation=nullptr;FloatingEnvironment::prepare();
        if(wrapper)actual=s::spawn_named_animation(c,file,name,0,layer,&actual_animation);else s::spawn_named_animation(c,file,actual,name,0,location,rotation,layer,flags,&actual_animation);
        const Snapshot source(c);check("object_and_lists",after.bytes.data(),source.bytes.data(),after.bytes.size());check("generation",&after.generation,&source.generation,4);check("free_list",&after.free,&source.free,sizeof(after.free));
        check("handle",&expected,&actual,4);check("animation_pointer",&expected_animation,&actual_animation,4);check("file_counter",&expected_counter,&file.fields_5c[3],4);
    }
    std::ofstream report(argv[2]);report<<"{\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"cases\": 4096,\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"full_game_equivalence\": false,\n  \"failures\": [";
    for(unsigned i=0;i<failures.size();++i)report<<(i?", ":"")<<'"'<<failures[i]<<'"';report<<"]\n}\n";
    std::cout<<passed<<" passed, "<<failed<<" failed\n";VirtualFree(controller,0,MEM_RELEASE);return failed?1:0;
}catch(const std::exception& ex){std::cerr<<ex.what()<<'\n';return 2;}}
