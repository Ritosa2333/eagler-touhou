#pragma once
#include "AttackAreas.hpp"
#include "GameInput.hpp"
#include "ShotResource.hpp"
namespace th09 {
struct ShotControlState {
    float charge=0,available=100;
    Timer full_charge{0,0,0},charged_fire{0,0,0},normal_fire{0,0,0},cooldown{0,0,0},charging_time{0,0,0},protection{0,0,0};
    Timer shock{0,0,0};u32 flags=0;i32 player_state=0;
};
struct ShotControlActions {
    virtual ~ShotControlActions()=default;
    virtual void play_sound(i32 id,i32 pan)=0;
    virtual void begin_charge()=0;
    virtual void end_charge()=0;
    virtual bool opponent_boss_available()=0;
    virtual void attack(i32 level,const std::string& spell_name)=0;
    virtual void effect(i32 id,const Vec3&)=0;
    virtual void fire(u32 set,i32 frame)=0;
};
class ShotControl {
    ShotControlState& state;AttackAreas& areas;ShotControlActions& actions;
    void protect(i32 level,const Vec3&);
    void release(const ShotResource&,const Vec3&,const FrameTiming&);
public:
    ShotControl(ShotControlState& s,AttackAreas& a,ShotControlActions& out):state(s),areas(a),actions(out){}
    void update(const GameInput&,bool automatic_focus,i32 side,const ShotResource&,const Vec3&,const FrameTiming&);
    bool bomb(const GameInput&,bool scene_locked,const ShotResource&,const Vec3&);
    bool automatic_defence(const ShotResource&,const Vec3&);
};
}
