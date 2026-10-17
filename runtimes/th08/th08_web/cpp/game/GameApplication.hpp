#pragma once
#include "GameplayScene.hpp"
#include "TitleScene.hpp"
#include "ResultScene.hpp"
#include "MusicRoom.hpp"
#include "Ending.hpp"
#include "LoadingScreen.hpp"
#include "FrameStatistics.hpp"
#include "SupervisorFrame.hpp"
#include "ConfigurationFile.hpp"
#include "ReplayExport.hpp"
#include "ScoreReport.hpp"
namespace th08 {
// Files, calendar, input and device operations are host boundaries. Scene
// selection, record handling, loading gates and ordered game jobs remain here.
struct ApplicationPlatform:PlayerScenePlatform,TextWriter,AsciiOverlay,FrameClock {
    virtual void begin_motion(i32,bool,bool,bool){}
    virtual bool load_motion(const u8*,u32){return true;}
    virtual i32 replay_touch_points(ReplayTouchPoint*,i32){return 0;}
    virtual bool cheat_movement_used()const{return false;}
    virtual std::vector<u8> read_prefix(const char*,u32 size)=0;
    virtual bool write(const char*,const u8*,u32)=0;
    virtual std::vector<std::string> user_replays()=0;
    virtual void calendar(char date[6],char timestamp[20])=0;
    virtual u16 poll_input()=0;
    virtual void begin_frame()=0;
    virtual bool present()=0;
    virtual void reset_device()=0;
    virtual void discard_graphics()=0;
    virtual bool load_surface(i32,const char*)=0;
    virtual void release_surface(i32)=0;
    virtual bool has_surface(i32)=0;
    virtual void draw_surface(i32,i32 x=0,i32 y=0)=0;
    virtual void capture_screen(i32)=0;
    virtual bool capture_pending()=0;
    virtual bool capture_arcade(const AnmLoadedSprite&)=0;
    virtual bool capture_texture(const TextureCaptureRequest&)=0;
    virtual bool play_music(i32 slot,i32 song)=0;
    virtual void load_music(i32 slot,const char*)=0;
    virtual void play_audio(const char*,i32 song)=0;
    virtual void stop_audio()=0;
    virtual void fade_music(float)=0;
    virtual void menu_music(MenuMusic,float)=0;
    virtual void midi_reset()=0;
    virtual void start_bgm()=0;
    virtual void process_sounds()=0;
    virtual void update_audio_fades()=0;
    virtual void apply_volume(const GameConfiguration&)=0;
    virtual void replay_error()=0;
};
class GameApplication {
    ApplicationPlatform& platform;
    struct GameIo:GameplayPlatform {
        GameApplication& a;explicit GameIo(GameApplication& a):a(a){}
        bool player_motion(const PlayerMovementState& s,float speed,const FrameTiming& timing,float& x,float& y)override{return a.platform.player_motion(s,speed,timing,x,y);}
        void begin_motion(i32 stage,bool initial,bool replay,bool record)override{a.platform.begin_motion(stage,initial,replay,record);}
        bool load_motion(const u8* data,u32 size)override{return a.platform.load_motion(data,size);}
        i32 replay_touch_points(ReplayTouchPoint* points,i32 capacity)override{return a.platform.replay_touch_points(points,capacity);}
        std::vector<u8> read(const char* p)override{return a.platform.read(p);}
        void sound(i32 i,i32 m,float p,bool pan)override{a.platform.sound(i,m,p,pan);}
        bool draw(AnmVm& v,TextAlignment t,u32 c,u32 o,const char* s)override{return a.platform.draw(v,t,c,o,s);}
        void begin(bool f)override{a.platform.begin(f);}void rectangle(const OverlayRect& r,u32 c)override{a.platform.rectangle(r,c);}
        bool play_music(i32 i,i32 s)override{return a.platform.play_music(i,s);}
        void play_audio(const char* p,i32 i)override{a.platform.play_audio(p,i);}
        void stop_audio()override{a.platform.stop_audio();}void fade_music(float s)override{a.platform.fade_music(s);}
        bool capture_arcade(const AnmLoadedSprite& s)override{return a.platform.capture_arcade(s);}
        bool capture_texture(const TextureCaptureRequest& r)override{return a.platform.capture_texture(r);}
        void menu_music(MenuMusic m,float s)override{a.platform.menu_music(m,s);}
        void release_loading_surface()override{a.loading.phase=0;a.platform.release_surface(8);}
        void save_score()override{a.failed|=!a.save_score();}
        u32 milliseconds()override{return a.platform.milliseconds();}
        std::vector<u8> read_score()override{return a.platform.read("score.dat");}
        void preload_music(i32 i,const char* p)override{a.platform.load_music(i,p);}
    } game_io{*this};
    struct TitleIo:TitlePlatform {
        GameApplication& a;explicit TitleIo(GameApplication& a):a(a){}
        std::vector<u8> read_asset(const char* p)override{return a.platform.read(p);}
        std::vector<u8> read_score()override{return a.platform.read("score.dat");}
        i32 load_surface(i32 i,const char* p)override{return a.platform.load_surface(i,p)?0:-1;}
        void sound(i32 i,i32 pan)override{a.platform.sound(i,pan,0,false);}
        void process_sounds()override{a.platform.process_sounds();}
        void play_music(i32 i,i32 s)override{a.platform.play_music(i,s);}
        void load_music(i32 i,const char* p)override{a.platform.load_music(i,p);}
        void stop_audio()override{a.platform.stop_audio();}void midi_reset()override{a.platform.midi_reset();}
        void apply_volume()override{a.platform.apply_volume(a.session.display_config);}
        std::vector<u8> read_replay(const char* p)override{return a.platform.read(p);}
        std::vector<std::string> list_user_replays()override{return a.platform.user_replays();}
        void replay_error()override{a.platform.replay_error();}
        void background()override{a.platform.draw_surface(0);}
        void load_image(const char* p)override{a.failed|=!a.platform.load_surface(0,p);}
        void release_image()override{a.platform.release_surface(0);}
        void begin_frame()override{a.platform.begin_frame();}
        u16 poll_input()override{return a.platform.poll_input();}
        bool present()override{return a.platform.present();}
        void reset_device()override{a.platform.reset_device();}
        void confirm_sound()override{a.platform.sound(10,0,0,false);}
        void discard_graphics()override{a.platform.discard_graphics();}
        void show_loading(bool c)override{a.show_loading({500,440,0},c);}
        void start_transition_effect()override{a.start_effect();}
        void fade_loading()override{a.fade_loading();}
        bool capture_pending()override{return a.platform.capture_pending();}
    } title_io{*this};
    struct ResultIo:ResultPlatform {
        GameApplication& a;explicit ResultIo(GameApplication& a):a(a){}
        std::vector<u8> read_asset(const char* p)override{return a.platform.read(p);}
        std::vector<u8> read_score()override{return a.platform.read("score.dat");}
        bool write_score(const u8* b,u32 n)override{return a.platform.write("score.dat",b,n);}
        bool load_result_background()override{return a.platform.load_surface(0,"result/result.jpg");}
        void release_result_background()override{a.platform.release_surface(0);}
        void sound(i32 i,i32 pan)override{a.platform.sound(i,pan,0,false);}
        void process_sounds()override{a.platform.process_sounds();}
        void export_records()override{a.export_records();}
        void format_date(char* d)override{char stamp[20]{};a.platform.calendar(d,stamp);}
        std::vector<u8> read_replay(i32 slot)override;
        void save_replay(i32 slot,const char* n)override{a.save_replay(slot+1,n);}
        u32 now_ms()override{return a.platform.milliseconds();}
        void draw_background()override{a.platform.draw_surface(0);}
        void show_loading(const Vec3& p)override{a.show_loading(p,false);}
    } result_io{*this};
    struct MusicIo:MusicRoomActions {
        GameApplication& a;explicit MusicIo(GameApplication& a):a(a){}
        bool load(MusicRoom&)override;void release()override;
        void background()override{a.platform.draw_surface(0);}
        void start_bgm()override{a.platform.start_bgm();}
        void play_audio(const char* p)override{a.platform.play_audio(p,0);}
        void capture_loading(const Vec3& p)override{a.show_loading(p,true);}
        void fade_music(float s)override{a.platform.fade_music(s);}
    } music_io{*this};
    struct EndingIo:EndingActions {
        GameApplication& a;explicit EndingIo(GameApplication& a):a(a){}
        std::vector<u8> read_ending(const char* p)override{return a.platform.read(p);}
        bool load_background(const char* p)override{return a.platform.load_surface(0,p);}
        void draw_background(i32 x,i32 y)override{a.platform.draw_surface(0,x,y);}
        void present()override{a.platform.present();}
        void release_background()override{a.platform.release_surface(0);}
        void music(const char* p)override{a.platform.play_audio(p,0);}
        void fade_music(float s)override{a.platform.fade_music(s);}
        void finished()override{if(!a.stopping)a.supervisor.state.target=i32(Scene::GameResults);}
    } ending_io{*this};
    struct LoadingIo:LoadingActions {
        GameApplication& a;explicit LoadingIo(GameApplication& a):a(a){}
        void capture_screen()override{a.platform.capture_screen(8);}
        bool has_capture()override{return a.platform.has_surface(8);}
        void draw_capture()override{a.platform.draw_surface(8);}
        void release_capture()override{a.platform.release_surface(8);}
        void draw_text()override{a.ascii.draw_strings(a.ascii_context);}
    } loading_io{*this};
    struct SupervisorIo:SupervisorActions {
        GameApplication& a;explicit SupervisorIo(GameApplication& a):a(a){}
        void animate_loading()override{a.loading.update();}
        bool service_animations()override{return a.library.service()&&(!a.title.active()||a.title.service());}
        void update_audio_fades()override{a.platform.update_audio_fades();}
        u16 poll_input()override{return a.platform.poll_input();}
        void discard_graphics()override{a.platform.discard_graphics();}
        bool title(bool replay_finished)override{return a.enter_title(replay_finished);}
        bool game()override{return a.enter_game();}
        bool results(bool from_game)override{return a.enter_results(from_game);}
        bool music()override{return a.enter_music();}
        bool ending()override{return a.enter_ending();}
        void cut_game()override{a.leave_game();}
        void save_replay()override{a.game.recording.reset();}
        void advance_stage()override{a.game.control.advance_stage();a.supervisor.state.stage=a.game.globals.stage;}
        bool version_valid()override{return !a.version.empty();}
    } supervisor_io{*this};
    ChainElement supervisor_job,ascii_calc,ascii_draw,background_draw,loading_draw,fps_draw;
    ScreenEffectState* transition_effect=nullptr;std::vector<u8> version;
    Vec2 presentation_shake{};bool presentation_shake_valid=false;
    bool initialized=false,running=false,failed=false,stopping=false,game_attached=false,loading_gate=false,loading_hidden=false;
    FrameTiming timing;ResultContext last_game;
    AnmLoaded* load_animation(i32,const char*);bool enter_title(bool);bool enter_game();void leave_game();
    bool enter_results(bool);bool enter_music();bool enter_ending();
    void bind_jobs();JobResult update_supervisor();void publish_scene();void synchronize();void finish_loading();
    void start_effect();void finish_effect();void show_loading(const Vec3&,bool);void fade_loading();
    ResultContext result_context()const;void save_replay(i32,const char*);void export_records();
public:
    GameplaySession session;TextureStore textures;AnmLibrary library;AnmRenderer renderer;Chain chain;AnmExecutor animations;
    AsciiManager ascii;AsciiContext ascii_context;ScreenEffects screen;LoadingScreen loading;FrameStatistics statistics;
    SupervisorFrame supervisor;TitleScene title;GameplayScene game;ResultScene results;MusicRoom music;Ending ending;
    GameApplication(ApplicationPlatform&,SpriteBackend&);
    ~GameApplication(){shutdown();}
    bool initialize(u32 performance_frequency=0);
    bool update();bool draw(float presentation_alpha=1.0f,bool presentation_active=false,bool presentation_only=false,bool world_interpolate=true);
    void shutdown();bool save_score();
    bool finalize_replay(i32 slot,const char* name){if(!game.recording.ready()||(game.globals.game_flags&8)||slot<1||slot>15||!name)return false;last_game=result_context();save_replay(slot,name);return !invalid();}
    bool active()const{return running;}
    bool invalid()const{return failed||animations.invalid||title.invalid()||results.invalid()||(game_attached&&game.invalid());}
    bool in_game()const{return game_attached;}
    bool loading_game()const{return loading_gate;}
    void close(){supervisor.state.close_requested=true;}
};
}
