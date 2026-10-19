#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
struct RingGeometry { //45d610,3c0 bytes
    sprite::Vec2 positions[60]{};
    std::uint32_t colors[60]{};
    float radii[60]{};
    void initialize(std::uint32_t,bool filled); //45dbf0/45ec10
    void update(float radius,bool filled); //45d780/45e800
    void draw(const sprite::Vec3&,bool filled); //45dae0/45eac0
};
static_assert(sizeof(RingGeometry)==0x3c0);
template<std::size_t Count> class RingEffect final:public AttachedCallback { //45d5b0/45e6d0
public:
    RingGeometry rings[Count];
    std::uint32_t field_after_rings;
    recovered::Timer age;
    std::int32_t view_index;
    explicit RingEffect(sprite::Animation&);
    int initialize(const Parameters&,int); //45db30/45eb10
    std::int32_t update() override; //45d6f0/45e770
    void draw() override; //45da40/45ea20
    void retire() override {} //412540
    void interrupt(std::int32_t) override {} //414b50
};
using FilledRing=RingEffect<1>;using TripleRing=RingEffect<3>;
static_assert(sizeof(FilledRing)==0x3e0&&sizeof(TripleRing)==0xb60);
extern template class RingEffect<1>;extern template class RingEffect<3>;
int draw_triangle_fan(sprite::Controller&,int,const sprite::Vec3&,const sprite::Vec2*,const std::uint32_t*); //43cb70
}
