#pragma once
#include "Types.hpp"
#include <array>
namespace th09 {
struct Bounds {Vec3 minimum,maximum;};
struct MovementSpeeds {float normal=0,focused=0,diagonal=0,focused_diagonal=0;};
struct PlayfieldLimits {Vec2 origin,extent;};
struct PlayerMotion;
struct MotionSource {virtual ~MotionSource()=default;virtual bool movement(const PlayerMotion&,float speed,float rate,float& x,float& y)=0;};
struct MotionEffects {
    virtual ~MotionEffects()=default;
    virtual void begin_focus(const Vec3&,u32 player,u32 character)=0;
    virtual void end_focus()=0;
    virtual void animation_interrupt(u32 label)=0;
    virtual void update_options()=0;
    virtual bool movement(float,float,float&,float&){return false;}
};
struct PlayerMotion {
    i32 health=10;
    u32 player=0,character=0,flags=0,direction=0;
    bool focus_effects=false;
    Vec3 position{},hit_extent{},graze_extent{},item_extent{};
    Vec2 base_scale{1,1},effect_scale{1,1},velocity{},step{};
    float angle=0;
    Bounds hit_bounds{},graze_bounds{},item_bounds{};
    std::array<Vec3,16> history{};
    void update(u16 keys,bool automatic_focus,u16 fire_frames,const MovementSpeeds&,const PlayfieldLimits&,float rate,MotionEffects&);
};
}
