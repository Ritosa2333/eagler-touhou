#pragma once
#include "effect.hpp"
namespace th20::source::effects {
struct AttachedCallback: sprite::AnimationCallback { //446b60,8 bytes
    sprite::Animation* animation;
    explicit AttachedCallback(sprite::Animation& a):animation(&a){a.fields_550[6]=reinterpret_cast<std::uint32_t>(this);}
};
static_assert(sizeof(AttachedCallback)==8);
class RoundedPanel final:public AttachedCallback { //461000,table56efa0
public:
    sprite::Vec3 target_position,dimensions;
    std::uint32_t color,field_24;
    recovered::Timer age,remaining;
    explicit RoundedPanel(sprite::Animation&);
    int initialize(const Parameters&,std::int32_t view_index); //461310, second stack argument unused but RET8
    std::int32_t update() override; //4610d0
    void draw() override; //461140
    void retire() override {} //412540, original literal return0 with no stores
    void interrupt(std::int32_t value) override {(void)on_interrupt(value);}
    int on_interrupt(std::int32_t); //4613d0
};
static_assert(offsetof(RoundedPanel,target_position)==8 && offsetof(RoundedPanel,age)==0x28 && sizeof(RoundedPanel)==0x48);
}
