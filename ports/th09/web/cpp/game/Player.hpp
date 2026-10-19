#pragma once
#include "PlayerLife.hpp"
#include "PlayerItems.hpp"
#include "PlayerShots.hpp"
#include "CpuPlayer.hpp"
#include "Combo.hpp"
#include "ChargeGauge.hpp"
namespace th09 {
namespace multiplayer { class WorldState; }
struct PlayerActions:PlayerShotActions,ComboActions,CpuActions {
    virtual void begin_charge()=0;
    virtual void end_charge()=0;
    virtual bool opponent_boss_available()=0;
    virtual void charged_attack(i32 level,const std::string& name)=0;
    virtual void protection_effect(i32,const Vec3&)=0;
    virtual void opponent_wins(i32)=0;
    virtual void slotted_effect(i32,const Vec3&,i32 slot,u32 color)=0;
    virtual void critical_health()=0;
    virtual void damage_flash(i32 side,i32 frames,u32 color)=0;
    virtual void begin_focus(const Vec3&,u32 side,u32 character)=0;
    virtual void end_focus()=0;
    virtual void shield_position(const Vec3&)=0;
    virtual void remove_shield()=0;
    virtual void charge_level(i32)=0;
    virtual void body_interrupt(AnmVm&,i32)=0;
    virtual void item_animation(AnmVm&,i32)=0;
    virtual void draw_body(AnmVm&)=0;
    virtual void draw_item(AnmVm&)=0;
    virtual TransferParameters* delayed_transfer(i32,const Vec3&,const Vec3&,float)=0;
    virtual void critical_opponent_time(i32)=0;
};
struct PlayerFrameContext {
    FrameTiming timing;PlayfieldLimits limits;PlayfieldGeometry geometry[2];CpuContext computer;
    u32 game_flags=0,field_flags=0;bool dialogue=false,automatic_focus=false,extra_mode=false,hide_players=false;
    i32 scene_state=0,ending_frames=0,opponent_level=0,opponent_survival_time=0;
};
// Owns player state and its named components. The platform sees only PlayerActions;
// original addresses and machine memory are not part of this game interface.
class Player:private PlayerLifeActions,private MotionEffects,private PlayerItemActions {
    friend class multiplayer::WorldState;
    EclWorldState& world;PlayerActions& actions;PlayerFrameContext frame;
    Combo combo_system(){return Combo(world,actions,frame.timing,frame.geometry[motion.player],frame.geometry[1-motion.player].width,i32(motion.player));}
    void synchronize_shots();
    void play_sound(i32 id,i32 pan)override{actions.play_sound(id,pan);}
    void play_positioned_sound(i32 id,float x)override{actions.play_positioned_sound(id,x);}
    void begin_charge()override{actions.begin_charge();}
    void end_charge()override{actions.end_charge();}
    bool opponent_boss_available()override{return actions.opponent_boss_available();}
    void attack(i32 level,const std::string& name)override{actions.charged_attack(level,name);}
    void effect(i32 id,const Vec3& p)override{actions.protection_effect(id,p);}
    void fire(u32 set,i32 time)override{synchronize_shots();shots.fire(resource,set,time);motion.base_scale=shots.player_scale;}
    void opponent_wins(i32 side)override{actions.opponent_wins(side);}
    void slotted_effect(i32 id,const Vec3& p,i32 slot,u32 color)override{actions.slotted_effect(id,p,slot,color);}
    void critical_health()override{actions.critical_health();}
    void damage_flash(i32 side,i32 n,u32 color)override{actions.damage_flash(side,n,color);}
    void begin_focus(const Vec3& p,u32 side,u32 character)override{actions.begin_focus(p,side,character);}
    void end_focus()override{actions.end_focus();}
    void remove_shield()override{actions.remove_shield();}
    void flush_combo()override{combo_system().flush(combo_state);}
    void reset_ai()override{cpu.reset_damage();}
    void animation_interrupt(u32 label)override{actions.body_interrupt(body,i32(label));}
    void update_options()override{}
    bool movement(float speed,float rate,float& x,float& y)override{return motion_source&&life.input_controller==0&&motion_source->movement(motion,speed,rate,x,y);}
    void start_animation(AnmVm& vm,i32 script)override{actions.item_animation(vm,script);}
    void draw_animation(AnmVm& vm)override{actions.draw_item(vm);}
    TransferParameters* create_transfer(i32 type,const Vec3& p,const Vec3& dest,float delay)override{return actions.delayed_transfer(type,p,dest,delay);}
    void combo(const Vec3& p,i32 normal,i32 spirit,i32 character,i32 score)override{combo_system().add(combo_state,p,normal,spirit,character,score);}
public:
    MotionSource* motion_source=nullptr;PlayerMotion motion;ShotControlState control;AnmVm body{};ShotResource resource;GameInput input;
    PlayerShots shots;PlayerItems items;PlayerHazards hazards;CpuState cpu;ComboState combo_state;PlayerLife life;
    Vec3 secondary_target{-999,-999,0};i32 target_priority=0,attack_levels[2]{1,1};bool initialized=false;
    Player(EclWorldState& w,PlayerActions& a):world(w),actions(a),shots(a),items(w.random,*this),life(motion,control,body,shots.areas,w.random,*this){}
    bool initialize(const u8* shot_data,u32 shot_size,u32 side,u32 character,i32 controller);
    void reset_round(i32 health,i32 difficulty,i32 round,float playfield_height=448);
    void update(const PlayerFrameContext&,DamageRules&);
    void draw(bool fading=false);
    void charge(float amount)override;
    bool add_combo(const Vec3& p,i32 normal,i32 spirit,i32 character,i32 score){return combo_system().add(combo_state,p,normal,spirit,character,score);}
};
}
