#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
struct BurstRingGeometry { //45f3c0
    sprite::Vec2 positions[61]{};
    std::uint32_t colors[61]{};
    float radii[62]{};
    void initialize(std::uint32_t); //45fc90
    void update(float); //45f670
};
static_assert(sizeof(BurstRingGeometry)==0x3d4&&offsetof(BurstRingGeometry,radii)==0x2dc);
class BurstRings final:public AttachedCallback { //45f340, vtable56ee50
public:
    BurstRingGeometry rings[3];
    std::uint32_t field_b84;
    recovered::Timer age;
    std::int32_t view_index;
    sprite::Vec3 origin;
    std::uint8_t follow_animation,padding_ba9[3];
    explicit BurstRings(sprite::Animation&);
    int initialize(const Parameters&,int); //45fb60
    std::int32_t update() override; //45f5e0
    void draw() override; //45f9b0
    void retire() override {} //412540
    void interrupt(std::int32_t) override {} //414b50
};
static_assert(sizeof(BurstRings)==0xbac&&offsetof(BurstRings,age)==0xb88);
int draw_thick_polyline(sprite::Controller&,int,const sprite::Vec3&,const sprite::Vec2*,const std::uint32_t*,float,bool); //43d370
}
