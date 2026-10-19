#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
class SpiralTrail:public AttachedCallback { //460180 / vtable56eec0
public:
    sprite::Vec2 positions[80]; //08
    std::uint32_t colors[80]; //288
    float angle,turn; //3c8,3cc
    recovered::Timer age; //3d0
    std::int32_t view_index; //3e0
    explicit SpiralTrail(sprite::Animation&);
    int initialize(const Parameters&,int); //460620
    std::int32_t update() override; //460240
    void draw() override; //460570
    void retire() override {} //412540
    void interrupt(std::int32_t) override {} //414b50
};
static_assert(sizeof(SpiralTrail)==0x3e4&&offsetof(SpiralTrail,colors)==0x288&&offsetof(SpiralTrail,age)==0x3d0);
class ReverseSpiralTrail final:public SpiralTrail { //460910 /vtable56ef30
public:
    sprite::Vec3 origin; //3e4
    std::uint8_t follow_player,padding_3f1[3];
    explicit ReverseSpiralTrail(sprite::Animation&);
    int initialize(const Parameters&,int); //460b60
    std::int32_t update() override; //4609e0
};
static_assert(sizeof(ReverseSpiralTrail)==0x3f4&&offsetof(ReverseSpiralTrail,follow_player)==0x3f0);
}
