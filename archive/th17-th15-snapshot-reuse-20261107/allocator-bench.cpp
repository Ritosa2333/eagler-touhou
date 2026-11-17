#include "FrameImage.hpp"
#include "AnmRegistry.hpp"
#include "BulletManager.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <new>
using namespace th17;
static bool counting=false;
static std::size_t allocations=0,allocated=0;
void* operator new(std::size_t bytes){
    if(counting){++allocations;allocated+=bytes;}
    if(auto* result=std::malloc(bytes?bytes:1))return result;
    std::abort();
}
void operator delete(void* value)noexcept{std::free(value);}
void operator delete(void* value,std::size_t)noexcept{std::free(value);}
struct Host final:BulletManagerHost,BulletFrameHost {
    BulletFrameHost& bullet(u32)override{return *this;}
    void prepare(u32,BulletFrameContext&)override{}
    void play(i32)override{}
    bool interrupt(i32)override{return true;}
    bool bind_appearance(i32,i32)override{return true;}
    bool spawn_animation()override{return true;}
    bool emit_children(const BulletShooter&)override{return true;}
    bool cancel_bullet(i32)override{return true;}
    i32 contact(BulletState&,bool)override{return 0;}
    bool animation_finished(bool)override{return false;}
    void cancellation_effect(i32,const Vec3&,const Vec3&)override{}
    void recycle()override{}
};
int main(){
    Host host;
    Rng random;
    BulletManager bullets(host);
    bullets.random=&random;
    BulletShooter shot;
    shot.rows=1;shot.count=108;shot.speed=1;
    if(!bullets.emit(shot,0))return 1;
    AnmRegistry registry;
    std::array<AnmVm*,256> nodes{};
    for(unsigned i=0;i<nodes.size();i++){
        nodes[i]=registry.allocate();
        if(!registry.submit(*nodes[i],(i&1)*4))return 2;
        nodes[i]->interpolators.mask_valid=true;
        if(i%7==0){
            nodes[i]->interpolators.invalidate();
            nodes[i]->interpolators.position.begin(20,0,{0,0,0},{2,1,0});
        }
        if(i>=8&&i%5==0&&!registry.attach(*nodes[i],*nodes[i%8]))return 3;
    }
    multiplayer::FrameImage::BufferPool buffers;
    std::array<multiplayer::FrameImage,13> history;
    for(auto& image:history)image.use_buffer_pool(buffers);
    auto frame=[&](unsigned n){
        nodes[0]->visual.translation.x=float(n);
        auto& target=history[n%13];
        bullets.save_rollback(target);
        if(!registry.save_rollback(target))std::abort();
        if(n%37==0&&(!target.restore()||nodes[0]->visual.translation.x!=float(n)))std::abort();
        history[(n+13-5)%13].clear();
    };
    for(unsigned n=0;n<130;n++)frame(n);
    const auto begin=std::chrono::steady_clock::now();
    counting=true;
    for(unsigned n=130;n<6130;n++)frame(n);
    counting=false;
    const auto milliseconds=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();
    std::printf("{\"frames\":6000,\"nodes\":256,\"bullets\":108,\"allocations\":%zu,\"allocatedBytes\":%zu,\"ms\":%.3f}\n",allocations,allocated,milliseconds);
}
