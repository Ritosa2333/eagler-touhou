#include "animation.hpp"
#include <cstring>
namespace th20::source::sprite {
void construct_animation_base(AnimationBase& a) noexcept {
    // Every scalar/vector/interpolator constructor called by 0x448d70 writes
    // zero. Its only unwritten bytes are the 2-byte gap after field +0x440.
    std::memset(&a,0,0x442);
    std::memset(reinterpret_cast<unsigned char*>(&a)+0x444,0,sizeof(a)-0x444);
}
void construct_animation(Animation& a) noexcept {
    construct_animation_base(a.base);
    a.handle=a.index=0;a.timer_4c8={};a.timer_4d8={};a.field_4e8=0;
    for(auto& link:a.links)link={&a,nullptr,nullptr,nullptr,nullptr};
    for(auto& word:a.fields_550)word=0;
    a.field_578=a.field_579=0;
    a.matrix_57c={};a.vector_5bc={};a.field_5c8=a.field_5cc=0;
    a.vector_5d0={};a.field_5dc=a.field_5e0=0;
}
void construct_pooled_animation(PooledAnimation& a) noexcept {
    construct_animation(a.animation);a.free_link={};a.active=0;a.index=0;
}
void reset_animation_state(Animation& a) noexcept {
    auto& b=a.base;
    b.vector_50=b.vector_58=b.vector_68={1.0f,1.0f};
    b.field_490=0xffffffffu;
    b.matrix_3b8={{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
    for(auto& word:b.flags)word=0;
    b.flags[0]|=0x10000u;b.flags[1]|=1u;b.flags[2]=(b.flags[2]&~3u)|1u;
    th20::recovered::timer_reset(a.timer_4c8);th20::recovered::timer_reset(a.timer_4d8);
    // Raw words at +0x478/+0x47c are floats 1 and pi; +0x480 is integer 65536.
    b.fields_444[13]=0x3f800000u;b.fields_444[14]=0x40490fdbu;b.fields_444[15]=0x10000u;
    a.fields_550[2]=a.fields_550[3]=0;
    b.vector_2c=b.vector_484=b.vector_38=b.vector_44={};
    b.vector_60=b.vector_70={};b.field_78=b.field_7c=0;b.vector_80={};
    b.interpolation_8c.duration=b.interpolation_e0.duration=b.interpolation_134.duration=0;
    b.interpolation_160.duration=b.interpolation_1b4.duration=0;
    b.interpolation_1e0.duration=b.interpolation_220.duration=b.interpolation_260.duration=0;
    b.interpolation_2a0.duration=b.interpolation_2f4.duration=b.interpolation_320.duration=b.interpolation_34c.duration=0;
    a.fields_550[8]=0; // +0x570
}
void clear_animation_suffix(Animation& a) noexcept { a.field_5dc=a.field_5e0=0; }
}
