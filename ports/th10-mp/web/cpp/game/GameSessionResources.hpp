#pragma once
#include "GameSession.hpp"
namespace th10 {
// Objects are created and destroyed in this order by the original game loader.
enum class SessionObject : u32 { Replay,Stage,PreviousStage,Gui,Player,Bullets,Items,Lasers,Results,TextOverlay,ScorePopups,Enemies,Effects,Bomb,Spell,Count };
struct GameSessionResourceEnvironment {
    GameEconomy* game;
#ifdef TH_ENABLE_THPRAC
    // Live TH10 tracker counters, reset on the original th10_enter (0x41798C).
    PracticeState* practice=nullptr;
#endif
    ScoreData** scores;
    GameSession** current;
    GameSystemCallbacks** objects[static_cast<u32>(SessionObject::Count)];
    const StageConfiguration** current_stage;
    const u8* configuration;
    const i32 *new_game,*practice_lives;
    const u32 *engine_flags,*display_flags;
    const i32 *pending_screen,*drawing_resource,*updating_resource,*pending_upload;
    i32 *loader_stop_requested,*loader_running,*menu_state;
    u32 *background_color,*loading_animation;
    double *recorded_time,*rendered_time;
    float* rate;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback,loader_callback;
    virtual GameSession* allocate_session()=0;
    virtual void release_session(GameSession* session)=0;
    virtual void evict_graphics_resources()=0;
    virtual void begin_loading(CallbackToken callback)=0;
    virtual void sleep(u32 milliseconds)=0;
    virtual bool create_object(SessionObject kind,i32 replay_mode)=0;
    virtual void destroy_object(SessionObject kind,GameSystemCallbacks* object)=0;
    virtual void prepare_replay()=0;
    virtual void reload_gui()=0;
    virtual void discard_gui_stage()=0;
    virtual void clear_enemies()=0;
    virtual void stop_music()=0;
    virtual void load_music(i32 slot,const char* name)=0;
    virtual void music_command(i32 command)=0;
    virtual void finish_loading(bool success)=0;
    virtual void save_score()=0;
    virtual u32 create_loading_animation(const Vec3& position)=0;
    GameSystemCallbacks*& object(SessionObject kind){return *objects[static_cast<u32>(kind)];}
};
struct GameSessionResources {
    GameSession& session;
    GameSessionResourceEnvironment& environment;
    i32 load();
    struct Progress {u32 phase=0;};
    // Returns 1 while work remains, 0 on success, -1 on failure. Game updates
    // must remain suspended until this preserves the original load barrier.
    i32 load_step(Progress& progress);
    i32 fail();
    void shutdown();
    void show_loading(float x,float y);
    static GameSession* create(i32 replay_mode,GameSessionResourceEnvironment& environment);
};
void reset_enemy_stage(EnemyManager& manager,const float* rate) noexcept;
void set_item_value(GameEconomy& game,i32 points,const float* rate) noexcept;
void increment_continue_count(GameEconomy& game) noexcept;
}
