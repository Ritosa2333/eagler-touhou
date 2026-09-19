#pragma once
#include "../game/GameApplication.hpp"
#include "../game/AnmText.hpp"
#include <map>
#include "ResourceManager.hpp"
#include "../../../portable/input/MotionTrack.hpp"
namespace th08 {
struct BrowserTexture {u32 handle,width,height,format,pitch,data,size,revision;};
class GameAudioManager;
class BrowserRuntime:public ApplicationPlatform {
    struct Graphics:SpriteBackend {
        BrowserRuntime& r;explicit Graphics(BrowserRuntime& r):r(r){}
#ifdef TH_NATIVE_PLATFORM
        PipelineState& pipeline()override;
#else
        void stage_state(u32,u32)override;void render_state(u32,u32)override;
#endif
        void bind_texture(u32)override;void destination_blend(BlendParameter)override;void write_depth(bool)override;
        void triangles(const SpriteVertex*,u32)override;void transform(MatrixParameter,const Matrix4&)override;
        void set_viewport(const Viewport&)override;void texture_factor(u32)override;
        void vertex_format(VertexFormat)override{};void draw(Primitive,VertexFormat,const void*,u32)override;
        void clear_target(u32,u32,float,u32)override;
    } graphics{*this};
    ResourceManager resources_;std::map<std::string,TexturePixels> images;
#ifdef TH_NATIVE_PLATFORM
    std::map<std::string,u64> image_use;u64 cache_clock=0;
    std::vector<u16> warm_glyphs;
#endif
    std::map<i32,u32> surfaces;
    BrowserTexture texture_result{};bool captured=false,prepared=false;
    InputController input;ControllerSnapshot pad;u8 keys[256]{};
    u32 back=0;JapaneseFonts fonts;std::unique_ptr<AnmText> text;
    std::unique_ptr<GameAudioManager> audio;
    void flush();void readback(u32);
    bool capture(u32,const TextureRect&,const TextureRect&,bool triangle);
    struct PendingCapture {u32 target=0;TextureRect source{},destination{};bool triangle=false;} pending_capture;
    bool capture_failed=false;void finish_capture();
public:
    GameApplication app;
    touhou::input::MotionTrack motion;
    bool player_motion(const PlayerMovementState&,float,const FrameTiming&,float&,float&)override;
    void begin_motion(i32 stage,bool initial,bool replay,bool record)override{motion.begin(stage,initial,replay,record);}
    bool load_motion(const u8* data,u32 size)override{return motion.load(data,size,8);}
    i32 replay_touch_points(ReplayTouchPoint*,i32)override;
    BrowserRuntime();~BrowserRuntime();
    static std::string path(const char*);
    bool put(const char*,const u8*,u32);bool put_archive(const u8*,u32);bool put_font(i32,const u8*,u32);
    bool mount_archive(std::unique_ptr<ArchiveSource> source){return !prepared&&resources_.mount_archive(std::move(source));}
    bool put_image(const char*,u32,u32,const u8*,u32);bool initialize();bool step(bool render=true);
    const std::vector<ArchiveEntry>& resources()const{return resources_.contents();}
    bool native_fonts();
    u32 native_font_steps();bool native_font_step(u32);
    const std::vector<u8>& file(const char*);const BrowserTexture* texture(u32);u32 backbuffer()const{return back;}
    i32 status(i32)const;
    bool audio_tick(u32 now);
    u8* keyboard_state(){return keys;}
    void controller_state(i32 x,i32 y,const u8* b,u32 n,bool available){pad={};pad.x=x;pad.y=y;pad.available=available;if(b)std::memcpy(pad.buttons,b,std::min<u32>(128,n));InputController::bindings(pad,app.title.context.controller_state);}
    std::vector<u8> read(const char*)override;std::vector<u8> read_prefix(const char*,u32)override;
    bool write(const char*,const u8*,u32)override;std::vector<std::string> user_replays()override;
    void calendar(char[6],char[20])override;u32 milliseconds()override;u64 performance_counter()override;
    u16 poll_input()override;void begin_frame()override;bool present()override;void reset_device()override;void discard_graphics()override;
    bool load_surface(i32,const char*)override;void release_surface(i32)override;bool has_surface(i32)override;
    void draw_surface(i32,i32,i32)override;void capture_screen(i32)override;bool capture_pending()override{return captured;}
    bool capture_arcade(const AnmLoadedSprite&)override;bool capture_texture(const TextureCaptureRequest&)override;
    void sound(i32,i32,float,bool)override;bool draw(AnmVm&,TextAlignment,u32,u32,const char*)override;
    void begin(bool)override;void rectangle(const OverlayRect&,u32)override;
    bool play_music(i32,i32)override;void load_music(i32,const char*)override;void play_audio(const char*,i32)override;
    void stop_audio()override;void fade_music(float)override;void menu_music(MenuMusic,float)override;
    void midi_reset()override;void start_bgm()override;void process_sounds()override;void update_audio_fades()override;
    void apply_volume(const GameConfiguration&)override;void replay_error()override;

};
}
