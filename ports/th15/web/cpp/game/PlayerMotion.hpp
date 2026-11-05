#pragma once
#include "ShtResource.hpp"
#include "Timer.hpp"
#include "PlayerTouch.hpp"
#include <functional>
#include <string>
namespace th15 {
struct PlayerFixedPosition {i32 x=0,y=0;};
struct PlayerOption {
    i32 active=0;PlayerFixedPosition target{},position{},normal_offset{},focus_offset{};i32 snap=0,index=0;
    std::function<bool(PlayerOption&)> movement_callback;
};
struct PlayerMotionVisuals {
    virtual ~PlayerMotionVisuals()=default;
    virtual bool focus_begin(bool enabled,float scale)=0;
    virtual bool pose(i32 script)=0;
    virtual void focus_position(const Vec3&)=0;
    virtual void option_position(u32,const Vec3&)=0;
    virtual bool option_remove(u32)=0;
    virtual bool barrier_position(const Vec3&)=0;
    virtual bool barrier_remove()=0;
};
class PlayerMotion {
public:
    PlayerTouch touch;
    bool flip_vertical_step=false;
    PlayerFixedPosition position_fixed{},last_step{},step{};Vec3 position{},velocity{},last_direction{};
    i32 normal_speed=0,focus_speed=0,normal_diagonal_speed=0,focus_diagonal_speed=0;
    i32 direction=0,focus=0,input_frame=0,option_follow_percentage=30,collapse_frame=0,option_count=0;
    u32 behavior_flags=0;float enlargement=1,movement_scale=1;Vec3 external_velocity{};Timer barrier_timer;
    std::array<PlayerOption,8> options;std::string error;
    void configure(const ShtHeader&)noexcept;
    void snap_options()noexcept{for(auto& option:options)option.snap=1;}
    bool update(u32 input,bool focus_allowed,float rate,PlayerMotionVisuals&);
};
}
