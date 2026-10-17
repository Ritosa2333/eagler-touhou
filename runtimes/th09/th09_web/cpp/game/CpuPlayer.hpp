#pragma once
#include "PlayerHazards.hpp"
#include "GameInput.hpp"
#include "ShotControl.hpp"
namespace th09 {
struct CpuState {
    Timer frame{0,0,0};i32 charging=0;
    i32 directions[8]{},evasive_directions[8]{};
    i32 hold_direction=0,direction=0;float charge_goal=100;
    Timer survival{0,0,0},mistakes{0,0,0};u32 flags=0;
    void reset_damage(){survival.reset();mistakes.reset();flags&=~3u;}
};
struct CpuContext {
    i32 side=0;
    HazardPlayer player;MovementSpeeds speeds;Vec2 base_scale{1,1},effect_scale{1,1};PlayfieldLimits limits;
    i32 health=10,level=0,opponent_pending=0,spirit_count=0,normal_enemies=0,attack_areas=0;
    u32 field_flags=0;bool scene_locked=false,dialogue=false,extra_mode=false;
    Vec3 item_target{-1000,0,0},priority_target{},first_target{};bool has_priority=false,has_first=false;u32 target_flags=0;
    i32 extra_damage=0;
};
struct CpuActions {
    virtual ~CpuActions()=default;
    virtual void opponent_survival_display(i32 frames)=0;
    virtual void opponent_survival_expired()=0;
};
class CpuPlayer {
    CpuState& state;Rng& random;PlayerHazards& hazards;CpuActions& actions;
    static Vec2 velocity(i32 direction,bool focus,const CpuContext&);
public:
    CpuPlayer(CpuState& s,Rng& r,PlayerHazards& h,CpuActions& a):state(s),random(r),hazards(h),actions(a){}
    void update(GameInput&,CpuContext&,ShotControlState&,const FrameTiming&);
    static i32 survival_seconds(i32 level);
};
}
