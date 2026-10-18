#pragma once
#include "rounded_panel.hpp"
namespace th20::source::effects {
// EffectShortLineEffectInf (465df0,20 samples) and EffectShortLineEffect2Inf
// (4663e0,30 samples) have the same member sequence, with differently sized arrays.
template<std::size_t Count> class ShortLine:public AttachedCallback {
public:
    sprite::Vec2 positions[Count];
    std::uint32_t colors[Count];
    float angle;
    recovered::Timer age;
    explicit ShortLine(sprite::Animation&);
    int initialize(const Parameters&,std::int32_t); //466140/466730, RET8
    std::int32_t update() override; //465e90/466480
    void draw() override; //4660a0/466690
    void retire() override {} //412540, actual empty base callback
    void interrupt(std::int32_t) override {} //414b50, actual empty base callback
};
using ShortLine20=ShortLine<20>;
using ShortLine30=ShortLine<30>;
static_assert(sizeof(ShortLine20)==0x10c && offsetof(ShortLine20,age)==0xfc);
static_assert(sizeof(ShortLine30)==0x184 && offsetof(ShortLine30,age)==0x174);
extern template class ShortLine<20>;
extern template class ShortLine<30>;
extern template class ShortLine<64>;
class LongLine final:public ShortLine<64> { //45ded0, EffectLineInf, vtable56ed1c
public:
    std::int32_t view_index;
    explicit LongLine(sprite::Animation& a):ShortLine<64>(a),view_index(0){}
    int initialize(const Parameters&,std::int32_t); //45e3a0 orange, alpha interpolation
    int initialize_gray(const Parameters&,std::int32_t); //45e240, unused second argument
};
static_assert(sizeof(LongLine)==0x320 && offsetof(LongLine,view_index)==0x31c);
//43d1a0, count is the number of vertices and produces count-1 line segments.
int draw_polyline(sprite::Controller&,std::int32_t,const sprite::Vec3&,const sprite::Vec2*,const std::uint32_t*);
}
