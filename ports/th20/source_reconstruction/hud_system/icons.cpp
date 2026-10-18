#include "hud.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include <stdexcept>
namespace th20::source::hud {
namespace {
void icons(sprite::Animation* const* entries,int full,int fragments,int maximum){
    if(!entries[0])return;
    const sprite::Vec3 position{recovered::mul32(recovered::int_float(recovered::signed_bits(7u-static_cast<std::uint32_t>(maximum))),28),0,0};
    for(unsigned i=0;i<7;++i)entries[i]->vector_5bc=position;
    unsigned i=0;auto at=[&](unsigned index)->sprite::Animation&{if(index>=7)throw std::out_of_range("FrontInf icon index exceeds original seven entries");return *entries[index];};
    for(;static_cast<int>(i)<full;++i)sprite::set_animation_interrupt(at(i),2);
    if(static_cast<int>(i)<maximum){if(fragments<0||fragments>=3)throw std::out_of_range("FrontInf fragment table outside original three values");sprite::set_animation_interrupt(at(i),fragments+7);++i;}
    for(;static_cast<int>(i)<maximum;++i)sprite::set_animation_interrupt(at(i),3);
    for(;i<7;++i)sprite::set_animation_interrupt(at(i),5);
}
}
void set_lives(FrontInf& owner,int full,int fragments,int maximum){icons(owner.life_animations,full,fragments,maximum);}
void set_bombs(FrontInf& owner,int full,int fragments,int maximum){icons(owner.bomb_animations,full,fragments,maximum);}
}
