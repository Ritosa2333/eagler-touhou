#pragma once
#include "../../cpp/game/EnemyDraw.hpp"
namespace enemy_draw_test {
using namespace th09;
struct Fixture:EnemyDrawServices {
    EclVm value;AnmLoadedSprite sprite;std::vector<i32> order;std::vector<AnmVm> animations;std::vector<EnemyTrailVertex> vertices;
    void draw_animation(AnmVm& vm)override{order.push_back(i32(&vm-value.animation.layers));animations.push_back(vm);}
    void draw_strip(AnmVm&,const EnemyTrailVertex* data,u32 count)override{order.push_back(i32(count)+1000);vertices.assign(data,data+count);}
    void prepare(){for(auto& vm:value.animation.layers)vm.loadedSprite=&sprite;}
    void draw(){order.clear();animations.clear();vertices.clear();EnemyDraw::enemy(value,*this);}
};
}
