#pragma once
#include "PlayerScene.hpp"
#include "BackgroundFlow.hpp"
#include "BulletFlow.hpp"
#include "EnemyFlow.hpp"
#include "GuiFlow.hpp"
#include "SpellFlow.hpp"
#include "EnemyNameAtlas.hpp"
#include "GameplayControl.hpp"
#include "PlayTime.hpp"
#include "GameplayStart.hpp"
#include "ReplayPlayback.hpp"
#include "ReplayRecording.hpp"
namespace th08 {
struct ReplayTouchPoint {float x=0,y=0;};
struct GameplayPlatform:PlayerScenePlatform,TextWriter,AsciiOverlay {
    virtual bool play_music(i32 index,i32 song)=0;
    virtual void play_audio(const char* path,i32 song)=0;
    virtual void stop_audio()=0;
    virtual void fade_music(float seconds)=0;
    virtual bool capture_arcade(const AnmLoadedSprite& sprite)=0;
    virtual bool capture_texture(const TextureCaptureRequest& request)=0;
    virtual void menu_music(MenuMusic action,float seconds)=0;
    virtual void release_loading_surface()=0;
    virtual void save_score()=0;
    virtual u32 milliseconds()=0;
    virtual std::vector<u8> read_score()=0;
    virtual void preload_music(i32 slot,const char* path)=0;
    virtual void begin_motion(i32,bool,bool,bool){}
    virtual bool load_motion(const u8*,u32){return true;}
    virtual i32 replay_touch_points(ReplayTouchPoint*,i32){return 0;}
};
// Actual gameplay owners, independent of the executable and the early
// GameRuntime prototype. The supplied platform implements resource/device I/O.
class GameplayScene:private BackgroundResources,private EnemyResources,private GuiResources,private SpellResources,private EffectResources,
                    private EnemySystemActions,private BulletSystemAudio,private SpellPresentationActions,private BackgroundActions,private BackgroundDrawActions,private DialogueActions,private EclLiveValues,private MenuActions,private GameplayStartActions {
    GameplaySession& session;TextureStore& textures;AnmLibrary& library;AnmRenderer& renderer;GameplayPlatform& platform;
    Chain owned_chain;Chain& chain;AnmExecutor animations;
    ChainElement player_calc,player_high,player_low,ascii_calc,ascii_high,ascii_low,control_calc,control_draw,replay_calc,replay_after,record_calc,replay_bookkeeping;
    struct ControlActions:GameplayControlActions {
        GameplayScene& scene;explicit ControlActions(GameplayScene& s):scene(s){}
        bool replay_stage(i32 stage)override;
        void update_enemy_name()override;void release_loading_surface()override;
        void play_music(i32 slot,i32 song)override;void pause_audio()override;
        void sound(i32 index)override;void update_game_time()override;
        void capture_arcade()override;void demo_fade()override;
    } control_actions{*this};
    struct Boundary {GameplayScene* scene=nullptr;i32 phase=0;ChainElement job;};
    Boundary boundaries[6];
    bool loaded=false,failed=false,playing_replay=false,recording_game=false;u32 owned_resources=0;GameplayLoad request;u16 previous_input=0;ReplayFrameState debug_frame;
    AnmLoaded* load(i32,const char*)override;AnmLoaded* get(i32)override;void release(i32)override;
    std::vector<u8> stage(const char* path)override{return platform.read(path);}
    std::vector<u8> ecl(const char* path)override{return platform.read(path);}
    std::vector<u8> message(const char* path)override{return platform.read(path);}
    void sound(i32 index,i32 mode)override{platform.sound(index,mode,0,false);}
    void sound(i32 index)override{sound(index,0);}
    void sound(i32 index,float position,bool panned)override{platform.sound(index,0,position,panned);}
    void panned_sound(i32 index,float x)override{platform.sound(index,0,x,true);}
    void bonus(i32 value)override{gui.show_bonus(value);}
    bool clock(i32 action)override{return gui.clock(action);}
    void message(i32 entry)override;
    AnmVm* moon()override;
    void move_effects(const Vec3& offset)override{effect_system.shift_glows(offset);}
    void sparkle(const Vec3& position)override{effect_system.spawn(62,position,1,0x20ffffff);}
    bool integrity_failed()override{return session.values.tampered();}
    bool stage_finished()override{return gui.finished();}
    void moon(AnmVm&)override;
    void effects()override; // BackgroundDrawActions
    void copy_enemy_name(i32 index)override;
    void clear_bullets()override{bullets.clear(1);}
    void despawn_enemies()override;
    void collect_items()override{items.collect_all();}
    bool play_music(i32 index,i32 song)override{return practice_bgm_filter(0,song)||platform.play_music(index,song);}
    void play_audio(const char* path,i32 song)override{if(!practice_bgm_filter(0,song))platform.play_audio(path,song);}
    void stop_audio()override{if(!practice_bgm_filter(1,0))platform.stop_audio();}
    void fade_music(float seconds)override{if(!practice_bgm_filter(1,0))platform.fade_music(seconds);}
    void fade_screen(i32 frames,u32 color,i32 priority)override{screen.create(ScreenEffectType::FadeOut,frames,i32(color),0,0,priority);}
    void capture_arcade(const AnmLoadedSprite& sprite)override{failed|=!platform.capture_arcade(sprite);}
    void capture_arcade()override{failed|=!gui.capture();}
    bool capture(const TextureCaptureRequest& r)override{return platform.capture_texture(r);}
    void music(MenuMusic m,float seconds)override{if(practice_bgm_filter(m==MenuMusic::Pause||m==MenuMusic::PartialFadeOut?2:m==MenuMusic::Stop?1:3,0))return;platform.menu_music(m,seconds);}
    void save_score()override{platform.save_score();}
    std::vector<u8> read_score()override{return platform.read_score();}
    void preload_music(i32 slot,const char* path)override{platform.preload_music(slot,path);}
    u32 now()override{return platform.milliseconds();}
    void update_game_time()override{accumulate_play_time(session.statistics.game_time,menus.context.system_time,now());}
    void synchronize();void publish_dialogue();JobResult boundary(i32 phase);
    // th08_everlasting_bgm (upstream ElBgmTest): returns true to swallow a
    // music command. command: 0=play 1=stop/fade 2=pause 3=resume.
    bool practice_bgm_filter(i32 command,i32 song);
    JobResult update_player();JobResult update_ascii();JobResult update_control();JobResult draw_ascii();
    JobResult update_replay();JobResult finish_replay_frame();
    JobResult update_recording();JobResult sample_replay_frame();void publish_input(const ReplayInputState&);
    void bind_jobs();
    i32 pending_time()const override{return globals.spell_time_items;}
    i32 uncollected_time_items()const override{return items.time_orb_count();}
    AsciiManager owned_ascii;
public:
    AsciiManager& ascii;AsciiContext ascii_context;
    EclGlobals globals;EclProgram program;
    GuiState hud;GuiImplState display;DialogueContext dialogue_context;GuiContext gui_context;
    BackgroundState background;BackgroundContext background_context;
    PlayerSimulationState player_state;ShotResource shots[2];
    PlayerScene player_services;PlayerSimulation player;
    ScreenEffects screen;EffectPoolState effect_pool;EffectEnvironment environment{};
    EffectSystem effect_system;ItemSystem items;EclExecutor executor;BulletManagerState projectile_pool;
    EnemySystem enemies;BulletSystem bullets;
    SpellPresentation presentation;SpellSystem spells;SpellDrawing spell_drawing;
    GuiController gui;Dialogue dialogue;BackgroundScript background_script;BackgroundView background_view;SpellBackground spell_background;
    EnemyNameAtlas name_atlas;
    i32 screen_counter=0;EnemyNativeScene native_scene;PlayerSceneWorld player_world;
    BackgroundFlow background_flow;BulletFlow bullet_flow;EnemyFlow enemy_flow;EffectFlow effect_flow;GuiFlow gui_flow;SpellFlow spell_flow;
    UiMenus menus;GameplayControl control;ReplayPlayback playback;ReplayRecording recording;u32 replay_stage_mask=0;
    bool time_stopped=false,paused=false,retrying=false,always_hitbox=false;
    GameplayScene(GameplaySession&,TextureStore&,AnmLibrary&,AnmRenderer&,GameplayPlatform&,Chain* shared_chain=nullptr,AsciiManager* shared_ascii=nullptr);
    ~GameplayScene();
    bool load(const GameplayLoad&,bool initialize_values=false);void unload(bool keep_resources=false,bool release_resources=true);
    bool start(const GameplayLoad& wanted){return load(wanted,true);}
    bool load_replay(const u8* data,u32 size){if(loaded)return false;return playback.load(data,size)&&platform.load_motion(data,size);}
    void play_practice_music(i32 slot,i32 song){play_music(slot,song);}
    bool update(u16 buttons,float rate=1,bool force_unit=false);bool draw();
    // Application-owned chains run the same jobs alongside the supervisor,
    // loading display and FPS counter, preserving their original priorities.
    bool prepare_frame(u16 buttons,float rate=1,bool force_unit=false);
    bool ready()const{return loaded&&!invalid();}
    u32 faults()const{return u32(failed)|(u32(animations.invalid)<<1)|(u32(player_services.invalid())<<2)|(u32(player.invalid())<<3)|(u32(enemies.invalid())<<4)|(u32(effect_system.invalid)<<5)|(u32(items.invalid())<<6)|(u32(bullets.invalid())<<7)|(u32(background_script.invalid)<<8);}
    bool invalid()const{return failed||animations.invalid||player_services.invalid()||player.invalid()||enemies.invalid()||effect_system.invalid||items.invalid()||bullets.invalid()||background_script.invalid;}
};
}
