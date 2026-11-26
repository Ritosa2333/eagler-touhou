// 原61D854..61D8BB，白幕绘制完成后重读并递增结局淡入计数。
#pragma once
#include "core/shared/ObjectRecord.hpp"
namespace kaikeidou {
struct EndingFadePresentationServices {
    virtual ~EndingFadePresentationServices()=default;
    virtual void blend(std::uint32_t mode,std::uint32_t alpha)=0;
    virtual std::uint32_t color(std::uint32_t r,std::uint32_t g,std::uint32_t b)=0;
    virtual void rectangle(std::uint32_t left,std::uint32_t top,std::uint32_t right,std::uint32_t bottom,std::uint32_t color,std::uint32_t filled)=0;
};
inline void present_ending_fade(ObjectRecord<420>& ending,EndingFadePresentationServices& services){
    const auto fade=ending.word(0x20);
    if(fade==0||fade>=0x80000000u)return;
    services.blend(3,fade);
    const auto color=services.color(255,255,255);
    services.rectangle(0,0,640,480,color,1);
    services.blend(0,255);
    ending.word(0x20,ending.word(0x20)+1u);
}
}
