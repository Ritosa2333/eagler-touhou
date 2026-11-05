#pragma once
#include "ShotControl.hpp"
#include "PlayerHazards.hpp"
#include "AnmLayout.hpp"
namespace th09 {
struct DamageRules {i32 damage=1,extra_damage=0,rank_charge=0;bool blocked=false;};
struct PlayerLifeActions:ShotControlActions {
    virtual void play_positioned_sound(i32,float)=0;
    virtual void opponent_wins(i32)=0;
    virtual void slotted_effect(i32,const Vec3&,i32 slot,u32 color)=0;
    virtual void critical_health()=0;
    virtual void damage_flash(i32 side,i32 duration,u32 color)=0;
    virtual void end_focus()=0;
    virtual void remove_shield()=0;
    virtual void flush_combo()=0;
    virtual void reset_ai()=0;
    virtual void charge(float)=0;
};
class PlayerLife {
    PlayerMotion& motion;ShotControlState& control;AnmVm& body;AttackAreas& areas;Rng& random;PlayerLifeActions& actions;
public:
    i32 input_controller=0,display_frames=0,hidden=0;float knockback_z=0;
    bool shield_active=false;Vec3 shield_position;
    PlayerLife(PlayerMotion& m,ShotControlState& c,AnmVm& b,AttackAreas& a,Rng& r,PlayerLifeActions& out):motion(m),control(c),body(b),areas(a),random(r),actions(out){}
    void damage(DamageRules&,const ShotResource&);
    bool collide(PlayerHazards&,float hit_radius,DamageRules&,const ShotResource&);
    void recover(const PlayfieldLimits&);
    void enter();
    void update_timers(const FrameTiming&);
};
}
