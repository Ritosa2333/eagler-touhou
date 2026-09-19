#pragma once
#include "EffectState.hpp"
#include "PlayerMovement.hpp"
namespace th08 {
struct PlayerLifeState {
    i8 state=0;u8 deathbomb=0,auto_bomb=0,reserved=0;Timer timer;
    i32 predead_count=0,clear_frames=0;EffectState *invincible_effect=nullptr,*predead_effect=nullptr;
};
struct PlayerLifeContext {
    u32 game_flags=0,hud_flags=0;i32 pause=0,bombs=0,lives=0,power=0,time_orbs=0,last_spell_requirement=0;
    Vec2 extent{384,448};u16 replay_flags=0;u8 character=0,focused=0,time_spell=0,game_over=0,miss_control=0,cheats=0;
    i16 gauge=0,padding=0;
};
struct PlayerLifeActions {
    virtual ~PlayerLifeActions()=default;
    virtual void update_integrity()=0;
    virtual void dissolve()=0;
    virtual void effect(i32 type,const Vec3& position,i32 count,u32 color)=0;
    virtual EffectState* fixed_effect(i32 type,const Vec3& position,i32 slot,u32 color)=0;
    virtual void sound(i32 index,float x)=0;
    virtual void cancel_item_homing()=0;
    virtual void cancel_rectangle(const Vec3& position,float width,float height,i32 value,i32 lifetime)=0;
    virtual void reset_screen_color()=0;
    virtual void fail_spell()=0;
    virtual void add_deaths(i32 amount)=0;
    // Value actions update the context before subsequent reads in this phase.
    virtual void add_time_orbs(i32 amount)=0;
    virtual void set_power(i32 value)=0;
    virtual void add_power(i32 amount)=0;
    virtual void set_bombs(i32 value)=0;
    virtual void add_lives(i32 amount)=0;
    virtual void subtract_rank(i32 amount)=0;
    virtual void item(i32 type,const Vec3& position,i32 mode)=0;
    virtual void animation(i32 script)=0;
};
// TH08 1.00d 0044ab40, 0044cbf0, 0044d180 and 0044d2c0.
class PlayerLife {
public:
    PlayerLife(PlayerLifeState& state,PlayerLifeContext& context,PlayerMovementState& movement,AnmVm& animation,PlayerLifeActions& actions)
        :state(state),context(context),movement(movement),animation(animation),actions(actions){}
    void die();
    bool resolve_death(const ShotProfile& profile);
    void respawn(const ShotProfile& profile);
    void update_invincibility(const FrameTiming& timing);
private:
    PlayerLifeState& state;PlayerLifeContext& context;PlayerMovementState& movement;AnmVm& animation;PlayerLifeActions& actions;
};
}
