#pragma once
#include "PlayerMotion.hpp"
#include "PlayerCollision.hpp"
#include "ItemManager.hpp"
namespace th11 {
struct PracticeState;
struct ReplayStageState;
struct PlayerFrameInput {
    PlayerMotionInput movement;
    bool special_available=true,special_active=false,shooting_blocked=false;
    bool replay=false,hit_sound=true;
#ifdef TH11_MULTIPLAYER
    bool challenge=false;
#endif
};
struct PlayerSpellState {u32 flags=0;i32 elapsed=0,bonus=0;};
struct PlayerBounds {Vec3 minimum,maximum;};
struct PlayerFrameState {
    i32 life_state=0;
    Timer state_timer,transition_timer,invincibility;
    Vec3 death_position,hit_half,pickup_size,focused_half;
    PlayerBounds hit,pickup,focused_attract,unfocused_attract;
    i32 minimum_point_value=0;
};
struct PlayerFrameWorld:ShotWorld {
    virtual bool player_sound(i32){return false;}
    virtual bool attract_items(){return false;}
    virtual bool cancel_bullets(const Vec3*,float,bool){return false;}
    virtual bool cancel_lasers(const Vec3*,float,bool,bool){return false;}
    virtual bool start_bomb(){return false;}
    virtual bool spawn_item(i32,Vec3,u32,float,float){return false;}
    virtual bool display_lives(i32,i32){return false;}
    virtual bool record_death(){return false;}
    virtual bool enemy_death(){return false;}
    virtual bool game_over(bool){return false;}
};
// Original per-frame player orchestration (431070), death (4327d0) and
// hit transition (432a90). World callbacks connect the live game managers.
class PlayerFrame:private PlayerMotionWorld {
public:
    PracticeState* practice=nullptr;
    PlayerFrame(ShtResource&,AnmResource&,AnmResource&,AnmManager&,GameEconomy&,PlayerFrameWorld&,u16 player_file=7,u16 bullet_file=0);
    PlayerMotion motion;ShotManager shots;PlayerFrameState state;
    PlayerFrameInput input;PlayerSpellState spell;i32 last_error=0;
    bool initialize();bool update();bool hit();bool die();bool draw(AnmRenderer&);
    bool start_stage();
    bool restore_replay_position(const ReplayStageState&);
    PlayerCollision collision()const noexcept;
    void copy_item_state(ItemPlayer&)const noexcept;
private:
    ShtResource& resource;AnmResource& bullet_resource;AnmManager& animations;GameEconomy& economy;PlayerFrameWorld& world;u16 bullet_file;
    bool sound(i32 id)override{return world.player_sound(id);}
    bool attract_items()override{return world.attract_items();}
    bool rebuild_options()override;
    bool death_frame();bool active_frame();bool cancel_all(bool);void fail_spell();void update_bounds();
    bool practice_power_locked()const;
};
}
