// CPU-only capture benchmark. Tests inspect the captured vertex stream without
// creating a graphics context; the production Scene interface stays unchanged.
#include "Animation.hpp"
#include "ExeTables.hpp"
#include "Renderer.hpp"
#include "TextRaster.hpp"
#include <array>
#include <list>
#define private public
#include "Scene.hpp"
#undef private
#include "Scene.cpp"
#include "game/GameState.hpp"
#include "game/StageFlow.hpp"
#include "wasm/Api.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <new>
using namespace th12host;
namespace {
bool counting=false;size_t allocations=0;u64 digest=1469598103934665603ull;
void require(bool ok,const std::string& message){if(!ok){std::fprintf(stderr,"%s\n",message.c_str());std::exit(1);}}
template<class T> void hash(T value){const auto* p=reinterpret_cast<const u8*>(&value);for(size_t i=0;i<sizeof value;++i){digest^=p[i];digest*=1099511628211ull;}}
std::filesystem::path assets;
std::vector<u8> bytes(const std::string& name){std::ifstream f(assets/name,std::ios::binary);require(bool(f),"missing "+name);return {std::istreambuf_iterator<char>(f),{}};}
void load(int stage,Banks& banks,Scene& scene){
    const auto suffix=std::string(stage<10?"0":"")+std::to_string(stage);th12_select_stage(stage);
    for(const auto& name:std::vector<std::string>{"default.ecl","stage"+suffix+".ecl"}){auto b=bytes(name);require(th12_load_ecl(b.data(),int(b.size())),name);}
    if(stage==7){auto b=bytes("stage07boss.ecl");require(th12_load_ecl(b.data(),int(b.size())),"Extra ECL");}
    auto b=bytes("stage"+suffix+".std");require(th12_load_std(b.data(),int(b.size())),"STD");StdGeometry g;std::string error;require(g.parse(b,error),error);scene.set_geometry(stage,std::move(g));
    b=bytes("stage"+suffix+".anm");require(th12_load_anm(10,b.data(),int(b.size())),"stage ANM");
    auto file=std::make_shared<AnmFile>();require(parse_anm("stage"+suffix,b,*file,error,false),error);for(auto& e:file->entries)e.texture=1;banks.slots[10]=file;
    th12_start(2,0,stage==7?4:1,12345);
}
void hash_background(const Scene& scene){
    hash(u32(scene.triangles.size()));
    for(const auto& t:scene.triangles){
        hash(t.layer);hash(t.entry);hash(t.blend);hash(t.point);hash(t.depth);
        for(const auto& v:t.v)for(float n:{v.x,v.y,v.u,v.v,v.r,v.g,v.b,v.a,v.depth,v.fog})hash(n);
    }
    hash(scene.fog.r);hash(scene.fog.g);hash(scene.fog.b);for(float n:scene.shake)hash(n);
}
}
void* operator new(std::size_t n){if(counting)++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
int main(int argc,char** argv){
    require(argc==2,"pass extracted TH12 assets directory");assets=argv[1];Renderer gpu;TextRaster text;Banks banks;ExeTables exe;Scene scene(gpu,text,banks,exe);
    double total_ms=0;size_t total_allocations=0,total_triangles=0;int captures=0;
    for(int stage=1;stage<=7;++stage){
        load(stage,banks,scene);
        for(int f=0;f<=600;++f){
            if(f)th12_tick(0,0);
            if(f==0||f==1||f==30||f==120||f==240||f==600){scene.capture_background();hash_background(scene);total_triangles+=scene.triangles.size();}
        }
        scene.capture_background();allocations=0;counting=true;const auto start=std::chrono::steady_clock::now();
        for(int i=0;i<240;++i)scene.capture_background();
        total_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();counting=false;total_allocations+=allocations;captures+=240;
        // Additional camera offsets cross near/far clipping boundaries with
        // the same vertex-order/hash comparison as the moving retail cameras.
        const auto saved=th12::stageState.camera;
        for(float offset:{-1800.f,-30.f,0.f,30.f,1800.f}){th12::stageState.camera=saved;th12::stageState.camera.offset[2]+=offset;scene.capture_background();hash_background(scene);}
    }
    require(total_triangles>0,"retail backgrounds produced no triangles");
    std::printf("{\"stages\":7,\"traceTriangles\":%zu,\"outputHash\":\"%016llx\",\"captures\":%d,\"captureMs\":%.3f,\"allocations\":%zu}\n",total_triangles,(unsigned long long)digest,captures,total_ms,total_allocations);
}
