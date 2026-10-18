#pragma once
#include "background-fixture.hpp"
#include "../../cpp/game/StageModelRenderer.hpp"
namespace stage_renderer_test {
using namespace th09;
struct Fixture:background_test::Fixture,StageDrawServices {
    std::vector<AnmLoadedSprite> sprites;std::vector<std::array<i32,2>> draws;std::vector<AnmVm> drawn_vms;std::vector<SpriteVertex> quads;
    void begin_models(const Camera&)override{}
    void fog_enabled(bool enabled)override{draws.push_back({0,i32(enabled)});}
    void world_sprite(AnmVm& vm)override{draws.push_back({1,index(vm)});drawn_vms.push_back(vm);}
    void screen_sprite(AnmVm& vm)override{draws.push_back({2,index(vm)});drawn_vms.push_back(vm);}
    void screen_quad(AnmVm& vm,const SpriteVertex* p)override{draws.push_back({3,index(vm)});drawn_vms.push_back(vm);for(u32 n=0;n<4;++n){auto v=p[n];if(!(v.color>>24))v.color=0;quads.push_back(v);}}
    void prepare(){sprites.resize(bg.primitives.size());for(u32 n=0;n<sprites.size();++n)bg.primitives[n].loadedSprite=&sprites[n];}
    void draw(i32 layer){draws.clear();drawn_vms.clear();quads.clear();StageModelRenderer::draw(bg,layer,*this);}
};
}
