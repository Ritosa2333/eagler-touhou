#pragma once
#include "PlayerMotion.hpp"
#include "PlayerCollision.hpp"
#include "ItemManager.hpp"
namespace th11 {
struct ReplayStageState;
struct PlayerFrameInput {
    PlayerMotionInput movement;
    bool special_available=true,special_active=false,shooting_blocked=false;
    bool replay=false,hit_sound=true;
};
struct PlayerSpellState {u32 flags=0;i32 elapsed=0,bonus=0;};
struct PlayerBounds {Vec3 minimum,maximum;};
struct PlayerFrameState {
    i32 life_state=0;
    Timer state_timer,transition_timer,invincibility;
#if PORT_TITLE == 14
    // Retail +6b4: separate from the focus/movement clock at +6a0.
    Timer th14_shooting_age;
#endif
    Vec3 death_position,hit_half,pickup_size,focused_half;
    PlayerBounds hit,pickup,focused_attract,unfocused_attract;
    i32 minimum_point_value=0;
};
struct PlayerFrameWorld:ShotWorld {
    virtual bool player_sound(i32){return false;}
#if PORT_TITLE == 14 || PORT_TITLE == 16
    virtual bool player_effect(Vec3,i32){return false;}
#endif
#if PORT_TITLE == 14
    virtual u32 player_focus(){return 0;}
    virtual bool bombs_changed(i32,i32){return false;}
#endif
#if PORT_TITLE == 16
    virtual u32 player_movement_effect(i32){return 0;}
    virtual bool th16_player_death_effect(Vec3){return false;}
    virtual bool bombs_changed(i32,i32){return false;}
#endif
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
#if defined(PORT_TITLE) && PORT_TITLE == 14
bool player_th14_death_drops(GameEconomy&,Vec3,PlayerFrameWorld&);
struct PlayerTh14Scale {
    u32 flags=0;FloatInterpolator interpolation;float factor=1; i32 auxiliary=0,recall=0;
    bool request(float,const float*)noexcept;
    float update(i32 frame,const float*)noexcept;
};
void player_th14_scale_bounds(PlayerFrameState&,Vec3,float)noexcept;
void player_th14_blink(AnmVm&,const PlayerFrameState&)noexcept;
bool player_th14_tick_clocks(PlayerFrameState&,const PlayerFrameInput&,u32 scale_flags)noexcept;
#endif
// Original per-frame player orchestration (431070), death (4327d0) and
// hit transition (432a90). World callbacks connect the live game managers.
class PlayerFrame:private PlayerMotionWorld {
public:
    PlayerFrame(ShtResource&,AnmResource&,AnmResource&,AnmManager&,GameEconomy&,PlayerFrameWorld&,u16 player_file=7,u16 bullet_file=0);
    PlayerMotion motion;ShotManager shots;PlayerFrameState state;
    PlayerFrameInput input;PlayerSpellState spell;i32 last_error=0;
#if PORT_TITLE == 16
    ShtResource* th16_season_shots=nullptr;
    AnmResource* th16_season_animation=nullptr;
    bool rebuild_th16_options(){return rebuild_options();}
#endif
#if PORT_TITLE == 14
    PlayerTh14Scale th14_scale;
    bool set_th14_scale(float);
    bool rebuild_th14_options(){return rebuild_options();}
#endif
    bool initialize();bool update();bool hit();bool die();bool draw(AnmRenderer&);
    bool start_stage();
    bool restore_replay_position(const ReplayStageState&);
    PlayerCollision collision()const noexcept;
    void copy_item_state(ItemPlayer&)const noexcept;
private:
    ShtResource& resource;AnmResource& bullet_resource;AnmManager& animations;GameEconomy& economy;PlayerFrameWorld& world;u16 bullet_file;
    bool sound(i32 id)override{return world.player_sound(id);}
    bool attract_items()override{return world.attract_items();}
#if PORT_TITLE == 14
    u32 create_focus()override{return world.player_focus();}
#endif
#if PORT_TITLE == 16
    u32 create_movement_effect(i32 script)override{return world.player_movement_effect(script);}
#endif
    bool rebuild_options()override;
    bool start_special();
    bool death_frame();bool active_frame();bool cancel_all(bool);void fail_spell();void update_bounds();
};
}
