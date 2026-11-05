#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
struct RadialTrail { //4669d0
    sprite::Vec2 positions[30]{};
    std::uint32_t colors[30]{};
    void initialize(float angle,float angular_step,float length,std::uint32_t color); //466e20,4th stack arg provenRET10
    void draw(const sprite::Vec3&,int start,int count); //466c10
};
static_assert(sizeof(RadialTrail)==0x168);
class RadialTrails final:public AttachedCallback { //466990 /vtable56f20c
public:
    RadialTrail trails[32];
    recovered::Timer age;
    explicit RadialTrails(sprite::Animation&);
    int initialize(const Parameters&,int view); //466c70, RET8 (view unused)
    std::int32_t update() override; //466a90
    void draw() override; //466ae0
    void retire() override {} //412540
    void interrupt(std::int32_t) override {} //414b50
};
static_assert(sizeof(RadialTrails)==0x2d18&&offsetof(RadialTrails,age)==0x2d08);
}
