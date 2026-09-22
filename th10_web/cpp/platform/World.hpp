#pragma once
#include "Hud.hpp"
#include "Backgrounds.hpp"
#include "ReplayFiles.hpp"
#include "../game/GameSessionResources.hpp"
#include "../game/BulletEmitter.hpp"
#include "../game/LaserManager.hpp"
#include "../game/EclProgram.hpp"
#include "../../../portable/input/MotionTrack.hpp"
#include <map>
namespace th10::browser {
// The gameplay owner persists across sessions, including transitions that keep
// the player/replay/projectile pools or hand a completed replay to the menus.
struct World final:HudActions,CallbackReceiver {
    GameState& state;AnimationEngine& engine;Common& common;Fonts& fonts;Input& input;Audio& audio;Scores& scores;ScreenEffects& effects;
    GameActors actors;Backgrounds backgrounds;UpdateChain* chain;Hud* hud=nullptr;
    PlayerProfile* cached_profile=nullptr;ReplayDocument replay_files;MemoryPool replay_memory;
    ReplayWriter replay_writer;ReplayCalendar& calendar;
    touhou::input::MotionTrack& motion=state.motion;
    struct Preview {ReplayDocument document;Preview* next;Preview(FileSystem& files,u32 flags,Preview* next):document(files,flags),next(next){}};
    Preview* previews=nullptr;
    i32 new_game=0,loader_stop=0,loader_running=0,resource_drawing=-1,resource_updating=-1,pending_upload=0;
    float measured_fps=60;bool loading=false;bool always_hitbox=false;i32 error=0;
    struct PlayerPresentation {Vec3 position{};i32 state=0;bool valid=false;} player_presentation;
    struct BulletPresentation {Vec3 position{};float angle=0;i32 id=0;u16 state=0;bool active=false;} bullet_presentation[2000]{};
    struct ItemPresentation {Vec3 position{};i32 age=0,state=0,kind=0;bool active=false;} item_regular_presentation[150]{},item_faith_presentation[2048]{};
    struct LaserPresentation {Vec3 position{};float angle=0,length=0,width=0;u32 id=0;i32 state=0;u32 kind=0;};
    std::map<const EnemyLaser*,LaserPresentation> laser_presentation;
    struct PopupPresentation {Vec3 position{};float elapsed=0;i32 timer=-2;u8 active=0,length=0;};
    PopupPresentation popup_presentation[723]{};
    GameSessionResources::Progress loading_progress;
    World(GameState&,AnimationEngine&,Common&,Fonts&,Input&,Audio&,Scores&,ScreenEffects&);
    ~World();
    bool start(i32 mode);void advance_loading();void stop_session();void shutdown();
    void advance_loading_step();
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    bool create_object(SessionObject,i32);void destroy_object(SessionObject,GameSystemCallbacks*);
    void clear_for_dialogue() override;void show_clear_results() override;
    void select_screen(i32);void sound(i32);void sound(i32,float);
    AudioGame music();u32 animation(AnmFile&,i32,u32 tag=15);
    void effect(AnmFile&,i32,const Vec3&);void rectangle(const ScreenRect&,u32);
    void text(AnmVm&,u32,const char*,TextAlignment,const u32* words=nullptr,u32 count=0);
    void queue_text(const Vec3&,const char*,const u32*,u32);
    void* read_file(const char*);void fail();
    bool create_player();void destroy_player(Player*);void activate_player();void configure_player();
    i32 update_player();i32 draw_player();void hit_player();
    i32 player_damage(const Vec3&,const Vec2&);i32 collide_player(const Vec3&,const Vec2&);i32 collide_player_laser(const Vec3&,float,float,float);
    bool create_bullets();void destroy_bullets(EnemyBulletManager*);void clear_bullets();
    i32 update_bullets();i32 draw_bullets();void fire(const BulletEmitter&);
    void cancel_bullets(bool);void cancel_bullet_circle(const Vec3&,float,bool,bool);
    i32 cancel_bullet_rectangle(i32);void cancel_bullet(EnemyBullet&);
    bool create_lasers();void destroy_lasers(LaserManager*);void clear_lasers();
    i32 update_lasers();i32 draw_lasers();i32 create_laser(i32,const void*);
    i32 cancel_lasers(i32);i32 cancel_laser_circle(const Vec3&,float,i32);i32 cancel_laser_rectangle(const Vec3&,const Vec3&,i32);
    void destroy_laser(EnemyLaser&);
    bool create_items();void destroy_items(ItemManager*);i32 update_items();i32 draw_items();
    i32 spawn_item(const Vec3&,i32,u32,float,float);i32 convert_power();
    bool create_bomb();void destroy_bomb(Bomb*);i32 start_bomb();i32 update_bomb();i32 bomb_damage(const Vec3&);
    bool create_effects();void destroy_effects(GameEffects*);
    bool create_enemies();void destroy_enemies(EnemyManager*);void clear_enemies(bool all);i32 update_enemies();
    Enemy* spawn_enemy(const char*,const EnemySpawnParameters&);void destroy_enemy(Enemy&);i32 update_enemy(Enemy&);
    i32 enemy_command(EnemyState&,EclContext&,EclGlobals&);
    bool create_spell();void destroy_spell(SpellCard*);i32 update_spell();i32 draw_spell(bool foreground);
    void start_spell(i32,i32,const char*,i32);void finish_spell();
    bool create_replay(i32,const char*);void destroy_replay(Replay*);void prepare_replay();void activate_replay();
    i32 update_replay();i32 replay_frame_action();i32 draw_replay();void finish_replay(i32);
    Replay* preview(const char*);void release_replay(Replay*);void save_replay(const char*,const char*);
    bool create_results();void destroy_results(Results*);void show_results(bool);i32 update_results();i32 draw_results();
    bool create_popups();void destroy_popups(ScorePopups*);i32 update_popups();i32 draw_popups();void popup(const Vec3&,i32,u32);
    bool create_hints();void destroy_hints(StageHints*);i32 update_hints();void record_hint(const char*,const Vec3&,bool caution);
    i32 update_session();void activate_session();
    ResultsEnvironment* results_adapter=nullptr;
    ResultsEnvironment& results_services();void release_results_services();
};
}
