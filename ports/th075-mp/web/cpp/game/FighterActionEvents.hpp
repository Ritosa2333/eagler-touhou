#pragma once
#include "FighterActor.hpp"
namespace th075 {
enum class FighterEffectSpace:u8 {Owned,Shared};
struct FighterActionEvents {
    virtual ~FighterActionEvents()=default;
    virtual void sound(i16)=0;
    virtual void system_sound(i16 id){sound(id);}
    virtual void effect(FighterEffectSpace,i16,float x,float y,i8 facing,u8 layer,const std::array<float,4>& parameters,u8 contactDelay=0,const FrameVisual* visual=nullptr,u8 alternateDraw=0,u8 shadow=0)=0;
    virtual void shake(float amplitude)=0;
    virtual u32& fade_color()=0;
    virtual u8& fade_alpha()=0;
    virtual std::array<u8,2>& hud_alpha()=0;
    virtual u8& completed_stages()=0;
    virtual u8& route()=0;
    virtual CameraMotion& camera()=0;
};
}
