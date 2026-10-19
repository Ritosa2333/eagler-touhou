#pragma once
#include "../../cpp/game/BulletDraw.hpp"
namespace bullet_draw_test {
using namespace th09;
struct Fixture:BulletDrawServices {
    Rng random;AnmExecutor executor{random};AnmLoaded file;BulletManager bullets;BulletVisuals visuals{file,executor};LaserManager lasers{file,executor};
    std::vector<i32> order;std::vector<AnmVm> animations;
    void begin_field()override{order.push_back(-1);}
    void upper_effects()override{order.push_back(-2);}
    void draw_animation(AnmVm& vm)override{
        const auto p=reinterpret_cast<uintptr_t>(&vm),first=reinterpret_cast<uintptr_t>(visuals.instances.data());
        if(p>=first&&p<first+visuals.instances.size()*5*sizeof(AnmVm))order.push_back(i32((p-first)/sizeof(AnmVm)));
        else{const auto base=reinterpret_cast<uintptr_t>(lasers.pool.data()),offset=p-base;order.push_back(10000+i32(offset/sizeof(Laser))*2+i32(offset%sizeof(Laser))/sizeof(AnmVm));}
        animations.push_back(vm);
    }
    void draw(){order.clear();animations.clear();BulletDraw::draw(bullets,visuals,lasers,*this);}
};
}
