#pragma once
#include "Stage.hpp"
#include "Item.hpp"
#include "ScoreData.hpp"
#include "Replay.hpp"
#include "Gui.hpp"
#include "EnemyManager.hpp"
#include "GameProgression.hpp"
namespace th10 {
struct PracticeState;
// Common first four words of the managers participating in the update chain.
struct GameSystemCallbacks {u32 flags,state;UpdateChainEntry *update,*draw;};
struct GameSessionEnvironment {
#ifdef TH_ENABLE_THPRAC
    const PracticeState* practice=nullptr;
#endif
    GameEconomy* game;
    ScoreData** scores;
    Replay** replay;
    Stage **stage,**previous_stage;
    ItemManager** items;
    Gui** gui;
    EnemyManager** enemies;
    AnmManager** animations;
    GameSystemCallbacks** systems[11];
    UpdateChainEntry** spell_foreground;
    u32 *loading_animation,*intro_animation;
    const u32 *held,*engine_flags,*display_flags;
    i32* pending_screen;
    float* rate;
    const StageConfiguration** current_stage;
    virtual void restart_stage()=0;
    virtual void fade_previous_stage()=0;
    virtual void fade_in_stage()=0;
    virtual void hide_screen(i32 frames)=0;
    virtual void stop_loader()=0;
    virtual void clear_bullets()=0;
    virtual void activate_player()=0;
    virtual void clear_enemies()=0;
    virtual void clear_lasers()=0;
    virtual void activate_replay()=0;
    virtual void spawn_stage_controller()=0;
    virtual void activate_gui()=0;
    virtual void configure_player()=0;
    virtual void play_music(i32 track,bool stage_start)=0;
    virtual void music_command(i32 command)=0;
    virtual void delete_stage(Stage* stage)=0;
    virtual void update_score_display()=0;
};
struct GameSession {
    u32 flags,stage_identifier;
    UpdateChainEntry *update_entry,*draw_entry;
    Timer elapsed;u32 timer_flags;
    u8 configuration[52];
    u32 session_flags;
    i32 replay_mode;
    void reset_timer(float* rate) noexcept;
    void activate_objects(GameSessionEnvironment& environment);
    i32 update(GameSessionEnvironment& environment);
    i32 draw(AnmManager& animations) const noexcept;
};
static_assert(sizeof(GameSession)==0x60 && offsetof(GameSession,session_flags)==0x58);
void decay_faith(GameEconomy& game) noexcept;
}
