#include "WorldCheckpoint.hpp"
#include "ResourceCrypt.hpp"
#include "Lzss.hpp"
#include "../game/VsCardSelection.hpp"
#include "../game/VsMatch.hpp"
#include "../game/VsScene.hpp"
#include "VsRuleData.hpp"
#include "CardEffects.hpp"
#include <sstream>
#include "EnemyVisual.hpp"
#include "LaserVisual.hpp"
#include "../game/BulletVisual.hpp"
#include "../game/CpuControl.hpp"
#include "../game/CpuObstacle.hpp"
#include "../game/CpuStory.hpp"
#include "../game/ReflectedBullet.hpp"
#include "../game/BulletSprites.hpp"
#include "../game/PlayerShotTable.hpp"
#include "../game/ChargeAttack.hpp"
#include "../game/ReimuSkill.hpp"
#include "../game/MarisaSkill.hpp"
#include "../game/FixedStep.hpp"
#include "../game/FrameRate.hpp"
#include "../game/TitleWave.hpp"
#include "../game/ShotCollision.hpp"
#include "../game/SoundTable.hpp"
// th19 SDL3 application: title screen + main menu, rendered through the shared
// portable SDL3 renderer via th19's own GraphicsDevice.
//
// Reverse-engineered (memory/2026-10-08.md):
//   - th19 renders at 1280x960 (scale 2.0); title font = ascii1280.anm.
//   - Menu text (font type 7): entry#3 = white glyphs, entry#4 = black shadow,
//     glyph index = char - 0x20, proportional 45px-tall glyphs, advance = width,
//     shadow offset = 4px (DAT_0062eeb0 * DAT_005aa280 = 2.0 * 2.0).
//   - Menu layout (640x480 logical x2): X=64, Y0=500, dy=50.
//   - Title animation roots: script 11 (TITLE ROOT) + script 87 (MENU ROOT).
#include "GraphicsDevice.hpp"
#include "FieldClip.hpp"
#include "../game/AnmResource.hpp"
#include "../game/AnmVm.hpp"
#include "../game/AnmGeometry.hpp"
#include "../game/AutoBarrier.hpp"
#include "../game/PlayerDamage.hpp"
#include "../game/Archive.hpp"
#include "../game/EclProgram.hpp"
#include "../game/EclVm.hpp"
#include "../game/GameWorld.hpp"
#include "../game/AttackWarning.hpp"
#include "../game/StoryData.hpp"
#include "../game/VsStoryData.hpp"
#include "../game/StoryPlayback.hpp"
#include "../game/DialogueBubble.hpp"
#include "../game/PauseAnimation.hpp"
#include "../game/StdScene.hpp"
#include "../game/WorldStrip.hpp"
#include "../game/BombHud.hpp"
#include "../game/PlayerBomb.hpp"
#include "../game/ChargeGauge.hpp"
#include "../game/ChargePoint.hpp"
#include <SDL3/SDL.h>
#include <eagler/input/BrowserGamepad.hpp>
#include <eagler/audio/WebSfx.hpp>
#include "TouchController.hpp"
#include <SDL3/SDL_audio.h>
#include <emscripten.h>
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#include "third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "third_party/miniaudio.h"
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <cstring>
#include <cmath>

namespace {
using th19::u8; using th19::u16; using th19::u32; using th19::i32;
using th19::sdl::GraphicsDevice;
using th19::sdl::SpriteVertex;
using th19::sdl::TextureAllocation;
using th19::sdl::Viewport;
using namespace touhou::graphics;
#include "music_names.h"
#include "native_music_text.h"
#include "trophy_text.h"
#include "RouteNotices.hpp"
#include "notice_text.h"
#include "ending_text.h"
#include "staff_events.h"
#include "ending_events.h"
#include "spellcard_names.h"
#include "native_spell_text.h"
#include "ability_names.h"
#include "ability_text.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

GraphicsDevice gfx;
th19::FrameRate g_present_fps;
int g_win_stars_drawn[2]{},g_fps_glyphs=0;
struct FpsGlyph { TextureAllocation texture{};th19::AnmSprite sprite{}; };
std::array<FpsGlyph,128> g_fps_font;
std::vector<u8> g_title_bytes, g_ascii_bytes, g_sig_bytes;
std::vector<th19::AnmEntry> g_title, g_ascii, g_sig;
std::vector<u8> g_front_bytes, g_story_text_bytes;
std::vector<th19::AnmEntry> g_story_text;
std::unordered_map<i32,th19::AnmScriptLoc> g_story_text_scripts;
std::vector<th19::AnmEntry> g_front;
std::vector<u8> g_plf_bytes[19];
std::vector<th19::AnmEntry> g_plf[19];

// ---- 说明书（help.anm）----
std::vector<u8> g_help_bytes;
std::vector<th19::AnmEntry> g_help;
std::map<i32, TextureAllocation> g_help_tex;
std::map<u32, std::pair<i32,i32>> g_help_sprites;
std::unordered_map<i32, th19::AnmScriptLoc> g_help_scripts;
std::vector<th19::AnmVm> g_help_vms;
std::map<i32, TextureAllocation> g_help_page_tex;
i32 g_help_sel = 0;
i32 g_help_mode = 0;
i32 g_help_page = 0;
// 详情页动画：0=无 1=淡入 2=向上滑出(下一页) 3=向下滑出(上一页/返回)
i32 g_help_anim = 0;
i32 g_help_anim_frame = 0;
i32 g_help_anim_next = 0;   // 动画结束动作：1=下一页 2=上一页 3=返回菜单

// eagler-host 协议状态（在 loop 之前定义，供 loop 读取）
static bool g_loop_started = false;
static bool g_loop_paused = false;
static th19::FixedStep g_fixed_step;
static bool g_skip_present=false;

// Geometry has no simulation side effects. Hidden replay ticks still update
// every ANM VM, but need no temporary vertex buffers.
template<class... Args> auto presentation_geometry(Args&&... args){
 using Geometry=decltype(th19::anmGeometry(std::forward<Args>(args)...));
 if(g_skip_present)return Geometry{};
 return th19::anmGeometry(std::forward<Args>(args)...);
}

static u8 g_injected_keys[512] = {};
static bool g_touch_keys[512]{};
static touhou::input::TouchController g_touch_input;
static int g_touch_side=0;
static bool g_network_active=false,g_network_finished=false;
static u32 g_network_frame=0;
static bool g_network_unlimited[2]{};
static float g_network_motion_x[2]{},g_network_motion_y[2]{};
const bool* game_keyboard_state(){
    static const bool neutral[512]{};
    static bool mapped[512]{};
    if(g_network_active)return neutral;
    const bool* physical=SDL_GetKeyboardState(nullptr);
    for(unsigned i=0;i<512;++i)mapped[i]=physical[i]||g_touch_keys[i];
    const int bits=eagler::input::BrowserGamepadState();
    if(bits>=0){
        std::array<unsigned char,256> virtualKeys{};
        eagler::input::ApplyGamepadVirtualKeys(virtualKeys,bits);
        const int vk[]={90,88,16,27,38,40,37,39,17,13,67,82};
        const int scans[]={SDL_SCANCODE_Z,SDL_SCANCODE_X,SDL_SCANCODE_LSHIFT,SDL_SCANCODE_ESCAPE,SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT,SDL_SCANCODE_LCTRL,SDL_SCANCODE_RETURN,SDL_SCANCODE_C,SDL_SCANCODE_R};
        for(unsigned n=0;n<12;++n)mapped[scans[n]]=mapped[scans[n]]||virtualKeys[vk[n]];
    }else{
        int count=0;SDL_JoystickID* ids=SDL_GetGamepads(&count);
        if(count>0){SDL_Gamepad* pad=SDL_OpenGamepad(ids[0]);if(pad){
            const float x=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTX)/32767.f,y=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTY)/32767.f;
            mapped[SDL_SCANCODE_LEFT]|=x<-.25f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_LEFT);
            mapped[SDL_SCANCODE_RIGHT]|=x>.25f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
            mapped[SDL_SCANCODE_UP]|=y<-.25f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_UP);
            mapped[SDL_SCANCODE_DOWN]|=y>.25f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_DOWN);
            mapped[SDL_SCANCODE_Z]|=SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_SOUTH);
            mapped[SDL_SCANCODE_X]|=SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_WEST);
            mapped[SDL_SCANCODE_C]|=SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_EAST);
            mapped[SDL_SCANCODE_LSHIFT]|=SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
            mapped[SDL_SCANCODE_ESCAPE]|=SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_START);
            SDL_CloseGamepad(pad);
        }}SDL_free(ids);
    }
    return mapped;
}
static i32 g_status[38] = {};   // First eight fields are the stable host status ABI.

// eagler-host：资源从 /th19.dat（THA1 归档）读取；standalone 开发时回退到预载文件。
// Archive 持有 g_dat_bytes 指针，必须保持存活。
static std::vector<u8> g_dat_bytes;
static th19::Archive g_dat;
static bool g_dat_open = false;
bool read_asset(const char* name, std::vector<u8>& out);

// ---- 音频：miniaudio 混音引擎 + SDL 流输出（效仿 th09 AudioDevice）----
// Native game IDs are remapped through 0x5c4e00, not filename-table indices.
struct AudioVoice {
    ma_decoder decoder{};
    ma_sound sound{};
    bool decoded = false, attached = false, immediate = false;
    std::vector<float> pcm;
    float gain=1;
    std::vector<u8> data;   // 内存解码时保持字节存活
    ~AudioVoice() { if(immediate)eagler::audio::web_sfx::erase(this); if (attached) ma_sound_uninit(&sound); if (decoded) ma_decoder_uninit(&decoder); }
};
struct AudioEngine {
    ma_engine engine{};
    SDL_AudioStream* stream = nullptr;
    bool engine_ready = false, ready = false, refill = true;
    std::map<i32, std::unique_ptr<AudioVoice>> sfx;
    std::unique_ptr<AudioVoice> music;
    float music_volume=1.f,sfx_volume=.8f;
    std::string current_music;
    bool attach(AudioVoice& v) {
        if (ma_sound_init_from_data_source(&engine, &v.decoder, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &v.sound) != MA_SUCCESS)
            return false;
        v.attached = true; return true;
    }
    bool load_sfx(i32 id, const char* path) {
        auto v = std::make_unique<AudioVoice>();
        if (!read_asset(path, v->data)) return false;
        ma_decoder_config cfg = ma_decoder_config_init(ma_format_f32, 2, 44100);
        if (ma_decoder_init_memory(v->data.data(), v->data.size(), &cfg, &v->decoder) != MA_SUCCESS) return false;
        v->decoded = true;
        ma_uint64 frames=0,read=0;
        if(ma_decoder_get_length_in_pcm_frames(&v->decoder,&frames)==MA_SUCCESS&&frames&&frames<44100*60){
            v->pcm.resize(std::size_t(frames)*2);
            if(ma_decoder_read_pcm_frames(&v->decoder,v->pcm.data(),frames,&read)==MA_SUCCESS&&read)
                v->immediate=eagler::audio::web_sfx::upload(v.get(),v->pcm.data(),unsigned(read),2,44100);
            ma_decoder_seek_to_pcm_frame(&v->decoder,0);
        }
        if (!v->immediate&&!attach(*v)) return false;
        v->gain=std::pow(10.f,th19::SOUNDS[id].attenuation/2000.f);
        if(v->immediate)eagler::audio::web_sfx::volume(v.get(),sfx_volume*v->gain);
        else ma_sound_set_volume(&v->sound,sfx_volume*v->gain);
        sfx[id] = std::move(v);
        return true;
    }
    void play_sfx(i32 id) {
        auto it = sfx.find(id);
        if (it == sfx.end()) return;
        if(it->second->immediate){eagler::audio::web_sfx::play(it->second.get());return;}
        ma_sound_seek_to_pcm_frame(&it->second->sound, 0);
        ma_sound_start(&it->second->sound);
    }
    // 播放音乐（ogg 或 wav），循环点 intro/total 为解码后 PCM 字节偏移，align=帧字节数
    bool play_music(const char* base, u32 intro, u32 total, u32 align) {
        if (!ready) { EM_ASM({console.error('[th19-audio] play_music: audio not ready');}); return false; }
        if (music) { ma_sound_stop(&music->sound); music.reset(); }
        current_music.clear();
        char path[256];
        auto v = std::make_unique<AudioVoice>();
        ma_decoder_config cfg = ma_decoder_config_init(ma_format_f32, 2, 44100);
        snprintf(path, sizeof path, "%s.ogg", base);
        if (ma_decoder_init_file(path, &cfg, &v->decoder) != MA_SUCCESS) {
            EM_ASM({console.warn('[th19-audio] ogg decode failed: '+UTF8ToString($0));}, path);
            snprintf(path, sizeof path, "%s.wav", base);
            if (ma_decoder_init_file(path, &cfg, &v->decoder) != MA_SUCCESS) { EM_ASM({console.error('[th19-audio] wav decode also failed: '+UTF8ToString($0));}, path); return false; }
        } else { EM_ASM({console.log('[th19-audio] ogg decode OK: '+UTF8ToString($0));}, path); }
        v->decoded = true;
        ma_data_source_set_loop_point_in_pcm_frames(&v->decoder, intro / align, total / align);
        if (!attach(*v)) return false;
        ma_sound_set_looping(&v->sound, MA_TRUE);
        ma_sound_set_volume(&v->sound, music_volume);
        ma_sound_start(&v->sound);
        music = std::move(v);current_music=base;
        return true;
    }
    void pump() {
        if (!ready || !stream) return;
        i32 queued = SDL_GetAudioStreamQueued(stream) / 8;
        if (!refill && queued < 4096) refill = true;
        if (!refill) return;
        if (queued >= 6144) { refill = false; return; }
        for (i32 n = 0; n < 6 && queued < 6144; ++n) {
            float pcm[2048]{};
            ma_uint64 frames = 0;
            if (ma_engine_read_pcm_frames(&engine, pcm, 1024, &frames) != MA_SUCCESS) return;
            for (auto& s : pcm) s = std::clamp(s, -1.0f, 1.0f);
            if (!SDL_PutAudioStreamData(stream, pcm, sizeof(pcm))) return;
            queued += 1024;
        }
        if (queued >= 6144) refill = false;
    }
};
AudioEngine g_audio;

bool init_audio() {
    auto cfg = ma_engine_config_init();
    cfg.noDevice = MA_TRUE;
    cfg.channels = 2;
    cfg.sampleRate = 44100;
    if (ma_engine_init(&cfg, &g_audio.engine) != MA_SUCCESS) { EM_ASM({console.error('[th19-audio] ma_engine_init failed');}); return false; }
    g_audio.engine_ready = true;
    // The shared renderer initializes only SDL_INIT_VIDEO|EVENTS. SDL3's
    // WebAudio backend requires the audio subsystem before opening a device
    // stream, otherwise SDL_OpenAudioDeviceStream returns NULL and audio never
    // starts. Mirror th09 AudioDevice: init audio + sample-frames hint.
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) { EM_ASM({console.error('[th19-audio] SDL_InitSubSystem(SDL_INIT_AUDIO) failed: '+UTF8ToString($0));}, SDL_GetError()); return false; }
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "2048");
    const SDL_AudioSpec spec{SDL_AUDIO_F32, 2, 44100};
    g_audio.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!g_audio.stream) { EM_ASM({console.error('[th19-audio] SDL_OpenAudioDeviceStream failed: '+UTF8ToString($0));}, SDL_GetError()); return false; }
    if (!g_audio.load_sfx(7,  th19::SOUNDS[7].path)) return false;
    if (!g_audio.load_sfx(14, th19::SOUNDS[14].path)) return false;
    if (!g_audio.load_sfx(9,  "/se_cancel00.wav")) { EM_ASM({console.error('[th19-audio] sfx se_cancel00 load failed');}); return false; }
    if (!g_audio.load_sfx(10, "/se_select00.wav")) { EM_ASM({console.error('[th19-audio] sfx se_select00 load failed');}); return false; }
    SDL_ResumeAudioStreamDevice(g_audio.stream);
    g_audio.ready = true;
    EM_ASM({console.log('[th19-audio] init_audio OK');});
    return true;
}
bool rb_collect_effects=false;
std::vector<i32> rb_effects;
u32 g_sfx_tick=1;
std::map<int,u32> g_sfx_played;
void stop_sfx(int id){if(rb_collect_effects){rb_effects.push_back(-id-1);return;}auto i=g_audio.sfx.find(id);if(i!=g_audio.sfx.end()){if(i->second->immediate)eagler::audio::web_sfx::stop(i->second.get());else ma_sound_stop(&i->second->sound);}}
void play_sfx(i32 id) {
    if(!g_audio.ready||id<0||id>=int(std::size(th19::SOUNDS)))return;
    if(g_sfx_played[id]==g_sfx_tick)return;
    g_sfx_played[id]=g_sfx_tick;
    if(rb_collect_effects){rb_effects.push_back(id);return;}
    if(!g_audio.sfx.count(id)&&!g_audio.load_sfx(id,th19::SOUNDS[id].path))return;
    g_audio.play_sfx(id);
}
static bool g_music_enabled = true;
bool g_paused=false,g_pause_confirm=false;
int g_pause_choice=0,g_pause_answer=1;
void set_music_enabled(bool enabled) {
    g_music_enabled = enabled;
    if (!enabled && g_audio.music) ma_sound_stop(&g_audio.music->sound);
}
// 标题 BGM：th19_01（龍の知性），循环点 intro=204800 total=26493888 align=4（s16le 2ch）
void start_title_music() {
    if (!g_music_enabled) { EM_ASM({console.warn('[th19-audio] start_title_music: music disabled');}); return; }
    EM_ASM({console.log('[th19-audio] start_title_music called');});
    g_audio.play_music("/music/th19_01", 204800, 26493888, 4);
}
// 曲目 → BGM 文件 + 循环点（musiccmt.txt 的 @bgm/ 映射 + music-layout.json 循环点）
// file=th19_XX 文件号；intro/total=PCM 字节偏移；align=帧字节数(s16le 2ch=4)
struct MusicBgm { i32 file; u32 intro; u32 total; u32 align; };
static const MusicBgm MUSIC_BGM[22] = {
    { 1, 204800, 26493888, 4 }, { 2, 61696, 32503528, 4 }, { 3, 251168, 22169408, 4 },
    { 7, 2326592, 28216192, 4 }, { 8, 2317056, 30205952, 4 }, { 9, 274432, 29336064, 4 },
    { 10, 250880, 24299136, 4 }, { 11, 1544448, 28184320, 4 }, { 12, 480768, 37802496, 4 },
    { 13, 366208, 30320704, 4 }, { 14, 2687872, 22979584, 4 }, { 6, 643072, 22924288, 4 },
    { 4, 2313344, 17132672, 4 }, { 5, 290816, 19107840, 4 }, { 15, 491904, 17846400, 4 },
    { 16, 127616, 30083072, 4 }, { 17, 2263680, 25405952, 4 }, { 18, 244096, 29609984, 4 },
    { 19, 4169216, 40031744, 4 }, { 20, 247808, 44851200, 4 }, { 21, 320800, 5263136, 4 },
    { 22, 13904384, 16118272, 4 }
};
void play_music_track(i32 track) {
    if (track < 0 || track >= 22) return;
    if (!g_music_enabled) return;
    const MusicBgm& b = MUSIC_BGM[track];
    char base[64];
    snprintf(base, sizeof base, "/music/th19_%02d", b.file);
    g_audio.play_music(base, b.intro, b.total, b.align);
}
// MSG music IDs use the native 0x5c5b60 table, not Music Room order.
int g_story_music=-1,g_story_final_music=-1;
void play_story_music(int id){
    static const int files[24]={1,2,3,13,14,6,4,5,15,16,17,18,19,20,7,8,9,10,11,12,21,22,90,91};
    if(!g_music_enabled||id<0||id>=24||id==g_story_music)return;
    const int file=files[id];bool ok=false;
    if(file==90)ok=g_audio.play_music("/music/th19_90",496128,4730112,4);
    else if(file==91)ok=g_audio.play_music("/music/th19_91",264672,4497888,4);
    else for(const auto& b:MUSIC_BGM)if(b.file==file){char path[40];std::snprintf(path,sizeof path,"/music/th19_%02d",file);ok=g_audio.play_music(path,b.intro,b.total,b.align);break;}
    if(ok)g_story_music=id;
}
// global sprite id -> (entry index, sprite index) within title.anm
std::map<u32, std::pair<i32,i32>> g_sprite_map;
// Separate texture tables: ascii, title and sig use independent entry indices.
std::map<i32, TextureAllocation> g_ascii_tex, g_title_tex, g_sig_tex;
std::map<i32, TextureAllocation> g_front_tex, g_plf_tex[19];
std::vector<th19::AnmVm> g_bg_vms;      // 标题背景（一直显示，转场不淡出）
std::vector<th19::AnmVm> g_title_vms;   // 灵梦/logo/菜单图标/立绘（转场淡出）
std::vector<u8> g_title_v_bytes;std::vector<th19::AnmEntry> g_title_v;
std::map<i32,TextureAllocation> g_title_v_tex;std::unordered_map<i32,th19::AnmScriptLoc> g_title_v_scripts;
std::map<u32,std::pair<i32,i32>> g_title_v_sprites;
th19::AnmVm g_title_v_vm;
std::vector<th19::AnmVm> g_sub_vms;   // 子界面标题条脚本（Music 206/Manual 207/Achieve 208 等）
// global script id -> stream location (createChild references global ids)
std::unordered_map<i32, th19::AnmScriptLoc> g_scripts;

// ---- loading screen (ascii1280.anm entry#7 = ascii/loading.png) ----
// sprites 912 (少女祈祷中) / 913 (Now Loading) / 914 (装饰环), scripts 14-19.
std::map<u32, std::pair<i32,i32>> g_loading_sprites;      // sprite id -> (entry, sprite idx)
std::unordered_map<i32, th19::AnmScriptLoc> g_loading_scripts; // script id -> stream loc
std::vector<th19::AnmVm> g_loading_vms;                    // root VMs (script 17 + 19)
i32 g_loading_entry = -1;                                  // entry index of loading.png

i32 g_sel = 0;
u32 g_frame = 0;   // frame counter
int g_bob_phase = 0;  // menu selection bob phase (0=idle; set to 8 on move, decrements each frame)
// Phase flow: loading screen (sig.anm + 少女祈祷中) → title screen.
// The title.anm scripts already order the reveal themselves: background (frame 0)
// → Reimu (frame 60) → logo slide-in (frame 120).
enum { PHASE_TITLE=0, PHASE_LOADING=1 };
i32 g_phase = PHASE_LOADING;   // 启动先显示加载界面
const u32 LOADING_FRAMES = 180;  // loading screen dwell
const char* const MENU[8] = {
    "Story Mode", "VS Mode", "Online VS Mode", "Achievement",
    "Music Room", "Option", "Manual", "Quit"
};
const float MENU_X = 64.0f, MENU_Y0 = 500.0f, MENU_DY = 50.0f;
const float SCREEN_W = 1280.0f, SCREEN_H = 960.0f;

// ---- menu screen state machine (FUN_0055ac60 switch param_1[6]) ----
// 0 Story / 1 VS / 2 Online VS(灰,跳过) / 3 Achievement / 4 Music / 5 Option / 6 Manual / 7 Quit
enum {
    SCR_TITLE=0,     // 主菜单
    SCR_STORY,       // Story Mode 选关/难度
    SCR_VS,          // VS Mode 选角色
    SCR_ACHIEVE,     // 成就页面
    SCR_MUSIC,       // 音乐室
    SCR_OPTION,      // 选项
    SCR_MANUAL,      // 操作说明
    SCR_KEYCONFIG,   // 按键配置（Option 里 Key Config 跳转）
    SCR_ENDING,      // 结局播放
    SCR_CHARSEL,     // 角色选择（Story/VS 难度后）
    SCR_DIALOG,      // 剧情对话（选人后、进游戏前）
    SCR_GAME,        // 正式进入游戏（对战场）
    SCR_STAFF,
    SCR_VSRULES,
};
i32 g_screen = SCR_TITLE;
// 转场：点击后菜单图标/灵梦/logo/版权水印淡出（FUN_0055a110 case4 设脚本3 淡出 + FUN_00404690），
// 背景（星空）保留。不是黑屏。
enum { TRANS_NONE=0, TRANS_OUT, TRANS_IN };
i32 g_trans = TRANS_NONE;
u32 g_trans_frame = 0;
i32 g_next_screen = SCR_TITLE;
const u32 TRANS_FRAMES = 24;   // 淡出/淡入各 24 帧
float g_fade = 1.0f;           // 当前屏幕内容整体透明度（背景除外），转场时 1→0→1
float g_alpha_scale = 1.0f;    // collect_vm 用的全局 alpha 缩放
// 子界面各屏的选中索引（Story 难度/VS 角色/成就/音乐/选项各自独立）
i32 g_sub_sel = 0;
i32 g_achievement_frame = 0;
i32 g_achievement_detail_frame = 0;
th19::AnmVm g_achievement_details[4];
bool g_achievement_exiting=false;
i32 g_sub_case_delay = 0;   // 进入 Option/Music 后延迟 1 帧发 case（等子项 createChild 完成）
i32 g_bgm_vol = 100;         // Option BGM 音量 0-100（默认值，源码 Default 重置 0x5064=低字节 BGM）
i32 g_se_vol  = 80;          // Option SE 音量 0-100
i32 g_screen_mode = 0;       // Option 画面模式 0=FullScreen 1=Window
i32 g_key_sel = 0;           // KeyConfig 选中项
i32 g_key_mode = 0;          // KeyConfig 0=Controller Select 1=按键绑定列表
// 切换难度/菜单的滑动动画：游标缓动到选中项（FUN_005596a0 设 case=难度+7 →
// 脚本19 CASE 7-10 posTime 30帧缓动滑动，相邻难度 Y 差 120 屏幕像素）
float g_story_cursor = 0.0f;   // Story 难度列表滑动游标
float g_vs_cursor = 0.0f;      // VS 菜单列表滑动游标
i32 g_story_difficulty = 0;   // 选中的难度 0-3（Story 难度选择后存档）
i32 g_vs_mode = 0;            // 0 story, 1 local VS CPU (VS menu selection)
i32 g_vs_player_select=0;     // 0=1P vs CPU, 1=1P vs 2P
i32 g_vs_rule_confirmed=0;
i32 g_vs_entry=0;int g_vs_entry_transition=-1;
i32 g_key_return_screen=SCR_OPTION;
th19::AnmVm g_vs_cursor_vm[2],g_vs_portrait_vm[2];
th19::ReflectionRng g_vs_scene_rng;
int g_vs_scene_side=0;th19::VsScene g_vs_scene{1,1};
bool g_vs_first_confirmed=false;bool g_vs_second_confirmed=false;int g_vs_match_confirm=-1;
th19::VsMatch g_vs_match;bool g_battle_timeout=false;
int g_vs_rules[7]={4,1,1,1,1,1,1};bool g_vs_rule_edit=false;int g_vs_rule_pending=0;
i32 g_vs_rule_time=5,g_vs_rule_round=1,g_vs_rule_card=0,g_vs_rule_p1life=5,g_vs_rule_p2life=5;
i32 g_char_sel = 0;           // 角色选择页选中角色 0-18
// 角色选择 19 角色 4 方向邻接表（原版 DAT_005a9c18，表序 [上,下,左,右]，位 0x10/0x20/0x40/0x80）
// 蜂窝导航：上下走对角、左右走同排；首尾 2 角色行左右回绕到唯一邻位
static const i32 CHAR_NAV[19][4] = {
    {17, 2, 1, 1}, {18, 4, 0, 0}, {0, 5, 4, 3}, {1, 6, 2, 4}, {1, 7, 3, 2},
    {2, 8, 7, 6}, {3, 9, 5, 7}, {4, 10, 6, 5}, {5, 11, 10, 9}, {6, 12, 8, 10},
    {7, 13, 9, 8}, {8, 14, 13, 12}, {9, 15, 11, 13}, {10, 16, 12, 11}, {11, 17, 16, 15},
    {12, 18, 14, 16}, {13, 18, 15, 14}, {14, 0, 18, 18}, {16, 1, 17, 17}
};
// 角色选择 19 图标六边形坐标（父锚点 script#76 pos=(640,200)）
static const float CH_CX[19] = {576,704, 512,640,768, 576,704,832, 512,640,768, 448,576,704, 512,640,768, 576,704};
static const float CH_CY[19] = {200,200, 312,312,312, 424,424,424, 536,536,536, 648,648,648, 760,760,760, 872,872};
// 角色选择页入场动画（原版 title.anm 脚本）
i32 g_charsel_frame = 0;       // 进入角色选择后的帧计数（标题滑入/图标入场/光标弹出/Clear展开）
i32 g_cursor_select = 0;       // 选择动画剩余帧（Z 按下：切 cursor1pd 按下态 + 缩放 1.5→1）
i32 g_char_switch_frame = 0;   // 切换角色后的帧计数（立绘/称号/介绍滑入，≤16帧）
float g_p2_cursor_cx=704.f,g_p2_cursor_cy=200.f;
float g_cursor_cx = 576.0f, g_cursor_cy = 200.0f;  // 光标显示位置（缓动逼近选中角色）
bool g_char_cleared[19] = {false};  // 通关标志占位（原版 score.dat 0x2eb16；存档未接入前全 false）
// 确认闪烁：选中项文字快速闪几下（FUN_004e5640 param_4 相位 & 3 交替）
i32 g_flash_phase = 0;       // 闪烁相位，>0 时逐帧递减，选中项按 &3 交替黑字/黄字
i32 g_confirm_delay = 0;     // 确认后延迟帧数（约19帧闪烁结束才转场）
int g_charsel_cancel_frame=-1;
int g_difficulty_confirm_frame=-1;
int g_menu_cancel_frame=-1;std::vector<th19::AnmVm> g_retiring_menu;int g_retiring_age=0;bool g_retiring_vs=false;
i32 g_cancel_delay = 0;      // X 取消动画剩余帧数（20帧：图标向中心聚拢+淡出）
float g_cancel_fade = 1.0f;  // X 取消时难度图标淡出系数（1→0）
float g_cancel_slide = 0.0f; // X 取消时图标向中心(640,368)聚拢进度（0→1）

// ---- 游戏场（正式进入游戏）状态 ----
// 对战场 384×448（x 居中 ±192，y 上→下 0..448）；1280×960 屏幕按 2x 渲染居中：
//   screen_x = 640 + fx*2, screen_y = 32 + fy*2
// 对战场 = 左右分屏（front.anm 边框实测：左场[32,624]、右场[656,1248]，各 592×896 屏 = 296×448 场 @2x）。
// 场坐标 x∈[-148,148] 居中、y∈[0,448] 上→下；1P 左场、2P/CPU 右场，出生各 (0,400) 底部居中
// （FUN_005478f0：位置字段 +0x68c/+0x690 写入 (0.0,400.0)；精灵在 y=448 下方朝上飞 = 场底）。
const float GAME_FIELD_W = 296.0f, GAME_FIELD_H = 448.0f;   // 单场尺寸（场坐标）
const float GAME_SCALE = 2.0f;                              // 场→屏幕缩放
const float GAME_F1_CX = 328.0f, GAME_F2_CX = 952.0f;       // 左/右场中心屏 X（front.anm 边框）
const float GAME_FY0 = 32.0f;                               // 场顶屏 Y
float g_p1_x = 0.0f, g_p1_y = 400.0f;   // 1P 位置（场坐标，出生底部居中）
float g_touch_motion_x = 0.0f, g_touch_motion_y = 0.0f;
bool g_touch_unlimited = false;
float g_p2_x = 0.0f, g_p2_y = 400.0f;   // 2P/CPU 位置（场坐标，出生底部居中）
i32 g_game_load_frames = 0;             // 进入游戏加载页剩余帧（>0 显示少女祈祷中）
// 游戏资源：world01.anm 背景 + 选中角色 plXX.anm 本体
std::vector<u8> g_world_bytes; std::vector<th19::AnmEntry> g_world;
std::map<i32, TextureAllocation> g_world_tex;
std::vector<u8> g_player_bytes; std::vector<th19::AnmEntry> g_player;
std::map<i32, TextureAllocation> g_player_tex;
std::map<u32, std::pair<i32,i32>> g_player_sprites;
std::unordered_map<i32, th19::AnmScriptLoc> g_player_scripts;
th19::AnmVm g_player_vm;                // 玩家 idle 动画 VM
i32 g_p1_dir = 0;                        // 玩家朝向：-1 左 / 0 前向 / 1 右（pl00.anm entry0 脚本 -35/-36/-33）
// 对手（CPU 占位，暂 pl01）+ front.anm 边框/HUD 精灵映射
std::vector<u8> g_opp_bytes; std::vector<th19::AnmEntry> g_opp;
std::map<i32, TextureAllocation> g_opp_tex;
std::map<u32, std::pair<i32,i32>> g_opp_sprites;
th19::AnmVm g_opp_vm;                    // 对手 idle 动画 VM（CPU 占位）
// ---- ECL 出怪/弹幕引擎（cpp/game 的 EclVm/EclProgram/GameWorld）----
std::vector<u8> g_enemy_bytes; std::vector<th19::AnmEntry> g_enemy;
std::map<i32, TextureAllocation> g_enemy_tex;
std::map<u32, std::pair<i32,i32>> g_enemy_sprites;
std::vector<u8> g_bullet_bytes; std::vector<th19::AnmEntry> g_bullet;
std::map<i32, TextureAllocation> g_bullet_tex;
std::map<u32, std::pair<i32,i32>> g_bullet_sprites;
struct EclField {
    std::vector<std::unique_ptr<th19::EclProgram>> programs;
    std::vector<std::string> loaded;
    th19::GameWorld world;
    th19::EclVm vm;
};
EclField g_ecl_fields[2];
th19::ScreenShake g_screen_shake;
int g_battle_report_frame=-1,g_battle_winner=0;int g_stage_deaths[2]{};int g_vs_attack_counts[2][2]{};th19::AnmVm g_battle_reports[2];
th19::AttackWarning g_attack_warning;th19::AnmVm g_attack_warning_vm;int g_attack_warning_frame=-1;
// Existing diagnostics expose the left field; simulation owns two independent fields.
th19::GameWorld& g_ecl_world=g_ecl_fields[0].world;
th19::EclVm& g_ecl_vm=g_ecl_fields[0].vm;
u32 g_ecl_generation=0;
bool g_ecl_ready = false;
u32 g_game_seed = 1;
// ---- 能力卡菜单（abcard.anm 符卡贴图 + abmenu.anm UI 框架 + ability.txt 名字）----
std::vector<u8> g_abcard_bytes; std::vector<th19::AnmEntry> g_abcard;
std::map<i32, TextureAllocation> g_abcard_tex;
bool g_abcard_loaded = false;
std::vector<u8> g_ability_bytes;
std::vector<th19::AnmEntry> g_ability_entries;
std::map<i32,TextureAllocation> g_ability_tex;
std::map<u32,std::pair<i32,i32>> g_ability_sprites;
std::unordered_map<i32,th19::AnmScriptLoc> g_ability_scripts;

std::vector<int> g_owned_cards;
std::vector<th19::AnmVm> g_owned_card_vms;
bool g_cards_dim=false;
int g_cards_draw_frame=-1;
int g_card_reward=-1,g_card_reward_frame=0,g_card_reward_exit=-1;
th19::AnmVm g_card_reward_vm;
std::unordered_map<i32,th19::AnmScriptLoc> g_card_scripts;
void load_ability_cards();
th19::VsCardSelection g_vs_cards[2];
std::vector<th19::AnmVm> g_vs_card_vms[2];
void init_vs_cards(int side);
void move_vs_card(int side,int delta){
 const bool all=g_vs_rules[2]==3;auto&list=g_vs_card_vms[side];if(list.empty())return;
 const int i=g_vs_cards[side].move(delta,all),n=int(list.size());
 auto fire=[&](int slot,int event){list[(slot+n*2)%n].interrupt(event);};
 if(delta>0){fire(i,15);fire(i-1,11);if(all){fire(i+1,17);fire(i-2,13);}}
 else{fire(i,16);fire(i+1,12);if(all){fire(i-1,18);fire(i+2,14);}}
}

void begin_card_reward(int opponent);
bool tick_card_reward(bool confirm);
void draw_card_reward();
void start_owned_card_menu();
void select_owned_card(int delta);
void draw_owned_card_menu();
void tick_owned_card_menu();
void close_owned_card_menu();
std::vector<u8> g_card_menu_bytes;
std::vector<th19::AnmEntry> g_card_menu_entries;
std::map<i32,TextureAllocation> g_card_menu_tex;
std::unordered_map<i32,th19::AnmScriptLoc> g_card_menu_scripts;
std::vector<th19::AnmVm> g_card_menu_cards;
int g_card_menu_index=0,g_card_menu_exit=-1;
th19::AnmVm g_card_name_vm,g_card_summary_vm;

std::map<u32, std::pair<i32,i32>> g_front_sprites;   // front.anm 边框/HUD 精灵映射
// 大字提示（front.anm entry13 精灵 168-180：勝利/完全勝利/敗北/弾幕開始/第一~最終戦）
i32 g_big_sprite = -1;   // 当前大字精灵 id（-1=无）
i32 g_big_frame = 0;     // 大字动画帧计数
// 符卡名横幅：对手释放的符卡名（spellcard_names.h 从 plXX.ecl 提取）
i32 g_opp_sel = 1;          // 对手角色索引（当前占位 pl01=魔理沙）
i32 g_spell_idx = 0;        // 当前符卡名索引
bool g_spell_show = false;  // 是否显示符卡名横幅
// HUD：命数（心形 sprite10 满/13 空）+ boom（星形 sprite16）+ 充能槽（11 圆点 sprite23）
// （front.anm entry0：sc16 心形 case2→13空/case3→10满；sc17 星形 case17-20→16-19；sc28-38 充能圆点 x=-376..-176）
i32 g_life_capacity[2]={5,5}; // Native player +0x58, separate from remaining +0x54.
i32 g_p1_lives = 5, g_p2_lives = 5;   // 命数（剩余生命）
i32 g_p1_bombs = 3, g_p2_bombs = 3;   // boom 炸弹数
i32 g_p1_fragments=0, g_p2_fragments=0; // Awarded only by character gameplay, never by HUD.
th19::ChargeGauge g_charge[2];
th19::PlayerShotTable g_shot_table;
th19::PlayerShotTable g_opp_shot_table;



struct ReimuSkillVisual {th19::ReimuSkill state;std::array<th19::AnmVm,3> animations;std::array<th19::ReimuSkill::Point,3> positions;};

struct MarisaSkillVisual {th19::MarisaSkill state;th19::AnmVm vm;float target_x=0;};


struct ReflectedVisual {th19::ReflectedBulletFlight state;float target_x=0,target_y=0;int target_side=1;};
std::vector<ReflectedVisual> g_reflected_flights;
th19::ReflectionRng g_reflection_rng;
int g_reflection_arrivals=0;
struct SpiritFlightVisual {th19::SpiritTransit state;th19::ReflectedBulletFlight curve;th19::AnmVm vm;int sender=0;};
std::vector<SpiritFlightVisual> g_spirit_flights;
th19::ReflectionRng g_spirit_rng;
int g_spirit_clock=0,g_spirit_arrivals=0,g_spirit_overflow=0;
// Passive modifier flag writers are still under audit; do not invent a character mapping.
bool g_spirit_enhanced[2][3]{},g_spirit_suppressed[2][3]{};
// Global rank source is still unconnected; keep its unenhanced value explicit.
int g_reflection_rank=0,g_reflection_rank_cap=0;
struct ChiyariSkillVisual {int frame=-1;th19::AnmVm vm;float x=0,y=0;};


std::unordered_map<i32,th19::AnmScriptLoc> g_bullet_scripts;
std::unordered_map<i32,std::array<float,2>> g_bullet_sprite_sizes;
struct SkillDamageCircle {float x,y,radius,growth;int remaining,damage;int interval=1;float vx=0,vy=0;};




struct PlayerAttackState {
th19::ChargeAttack charge_attack;
th19::ChargeArea charge_area;
th19::RanSkill ran_skill;
std::vector<ReimuSkillVisual> reimu_skills;
std::vector<MarisaSkillVisual> marisa_skills;
int marisa_skill_arrivals=0;
ChiyariSkillVisual chiyari_skill;
int chiyari_skill_spawns=0;
std::vector<SkillDamageCircle> skill_damage_circles;
float skill_origin_x=0,skill_origin_y=0;
th19::AnmVm charge_area_vm,tsukasa_skill_back;
float charge_area_x=0,charge_area_y=0;
float option_lock_x=0,option_lock_y=400;
};
PlayerAttackState g_attacks[2];
bool g_test_card_shoot_only=false;
struct PlayerProjectile {float x,y,vx,vy;int age=0;th19::AnmVm vm;int option=0,type=0,update=0,laser_id=0;float offset_x=0,offset_y=0,length=0,angle=0,speed=0,turn_rate=0,acceleration=0;bool ending=false;u32 target_id=0;int damage=0;float hit_width=0,hit_height=0;bool consumed=false;float table_turn=0,table_acceleration=0;int lifetime=0,laser_duration=0,transition_frame=0,turn_wait=-1;bool charged=false;bool piercing=false;int hit_callback=0,hit_script=-1,init_callback=0;int side=0;bool ability_bank=false,effect_bank=false;float card_lock_y=0,hit_growth=0;int card_id=0;};
std::vector<PlayerProjectile> g_player_projectiles;
// Independent SHT hit ANMs survive the projectile that created them.
std::vector<PlayerProjectile> g_player_hit_effects;
// Native 0x4d09b0 snapshots origin and rotation, registers a separate VM,
// and leaves it alive when the source shot is destroyed. pl00 scripts 8/10
// emit scripts 11/12 this way; these are not attached to the moving talisman.
void collect_shot_detached_anms(PlayerProjectile& source){
    auto& children=source.vm.children;
    for(auto it=children.begin();it!=children.end();){
        if(!it->detached_spawn){++it;continue;}
        PlayerProjectile effect{};effect.x=source.x+it->spawn_x;effect.y=source.y+it->spawn_y;
        effect.side=source.side;effect.ability_bank=source.ability_bank;effect.effect_bank=source.effect_bank;
        effect.vm=std::move(*it);effect.vm.detached_spawn=false;
        effect.vm.rot_x+=effect.vm.spawn_rot_x;effect.vm.rot_y+=effect.vm.spawn_rot_y;effect.vm.rot_z+=(source.vm.rotation_mode==1?source.angle:effect.vm.spawn_rot_z);
        effect.angle=effect.vm.rot_z;
        g_player_hit_effects.push_back(std::move(effect));it=children.erase(it);
    }
}

std::vector<PlayerProjectile> g_card_options;
struct PlayerOption {
    float x=0,y=0,phase=0;
    int charge_offset_x=0,charge_offset_y=0;
    th19::AnmVm vm;
};
std::vector<PlayerOption> g_player_options[2];
std::unordered_map<i32,th19::AnmScriptLoc> g_opp_scripts;
int g_player_power=2;

std::vector<th19::PlayerShotTable::Position> g_player_trail[2];
void init_player_options();
void tick_player_options(bool focused);
void draw_player_options();
int g_shoot_frame=0,g_opp_shoot_frame=0;
th19::CpuControl g_cpu_left_control;u8 g_cpu_left_bits=0;
th19::CpuControl g_cpu_control;th19::CpuDifficulty g_cpu_difficulty;th19::CpuStory g_cpu_story;
bool g_cpu_automatic=true;int g_cpu_frames=0,g_cpu_steers=0,g_cpu_releases[5]={},g_cpu_story_events=0;
u8 g_opponent_controls=0;int g_opp_direction=0;
std::vector<u8> g_focus_bytes;
std::vector<th19::AnmEntry> g_focus_entries;
std::map<i32,TextureAllocation> g_focus_tex;
std::map<u32,std::pair<i32,i32>> g_focus_sprites;
std::unordered_map<i32,th19::AnmScriptLoc> g_focus_scripts;
th19::AnmVm g_focus_vm[2];
th19::AnmVm g_barrier_vm[2],g_charge_pattern[2],g_region_vm[2];
th19::AnmVm g_gauge_markers[2][8];
th19::AnmVm g_gauge_particles[2];
int g_gauge_marker_state[2][4]={};
th19::AutoBarrier g_barrier[2];
th19::PlayerBomb g_bombs[2];
th19::AnmVm g_bomb_area[2],g_bomb_back[2],g_bomb_extra[2][2];
int g_bomb_visual_tail[2]={};
th19::PlayerDamage g_player_damage[2];
bool g_player_collision_enabled=true;
bool g_charge_visual_held[2]={};
int g_charge_visual_level[2]={};
std::vector<u8> g_aura_bytes;
std::vector<th19::AnmEntry> g_aura_entries;
std::map<i32,TextureAllocation> g_aura_tex;
std::unordered_map<i32,th19::AnmScriptLoc> g_aura_scripts;
void init_charge_visuals();
void tick_charge_visuals();
bool trigger_player_barrier(int side){
    if(side<0||side>1||!g_barrier[side].hit(true,g_screen!=SCR_GAME))return false;
    // Native 544c90 ends effect.anm script 1 when the shield breaks.
    g_barrier_vm[side].interrupt_all(1);
    // Card20: native409060 sets +ba; barrier break calls4ee770 at radius128.
    // This call targets ordinary bullets only and preserves cancellation locks.
    if(side==0&&std::find(g_owned_cards.begin(),g_owned_cards.end(),20)!=g_owned_cards.end())
        g_ecl_fields[side].world.cancel_bullets_circle(g_p1_x,g_p1_y,128.f);
    return true;
}
bool g_focus_down[2]{},g_focus_active[2]{};


i32 g_p1_charge = 0, g_p2_charge = 0; // 充能槽填充段数（0..11）
// world01.std 背景分层滚动（视差）
std::vector<u8> g_world_std_bytes;
struct BgScrollLayer { i32 entry; float speed; float x; };   // entry=world01.anm entry 索引, speed=滚动速度, x=水平偏移
std::vector<BgScrollLayer> g_bg_layers;
float g_bg_scroll = 0.0f;   // 累计滚动进度（帧累积，正=向下滚）

// Native title background distortion; foreground/menu panels are separate ANM VMs.
th19::TitleWave g_title_wave;
TextureAllocation g_title_wave_capture;
float g_panel_tilt=0; // legacy selection-box angle, not the background mesh
void update_panel() {
    if(g_phase!=PHASE_TITLE)return;
    int palette=g_sel;
    if(g_screen==SCR_STORY||g_screen==SCR_VS||g_screen==SCR_VSRULES||g_screen==SCR_KEYCONFIG)palette=9;
    else if(g_screen==SCR_CHARSEL)palette=8;
    g_title_wave.select(palette);
    g_title_wave.tick();
    const float tilt=g_screen==SCR_STORY?(float(g_sub_sel)-1)*5:0;
    g_panel_tilt+=(tilt-g_panel_tilt)*.12f;
}

bool read_file(const char* path, std::vector<u8>& out) {
    auto* f = SDL_IOFromFile(path, "rb");
    if (!f) return false;
    const Sint64 size = SDL_GetIOSize(f);
    if (size < 0) { SDL_CloseIO(f); return false; }
    out.resize(size_t(size));
    const bool ok = size == 0 || SDL_ReadIO(f, out.data(), out.size()) == size;
    SDL_CloseIO(f);
    return ok;
}

// eagler-host：资源从 /th19.dat（THA1 归档）读取；standalone 开发时回退到预载文件。
// Archive 持有 g_dat_bytes 指针，必须保持存活（g_dat_bytes/g_dat/g_dat_open 在前部声明）。
bool read_asset(const char* name, std::vector<u8>& out) {
    if (read_file(name, out)) return true;
    if (!g_dat_open) {
        if (!read_file("/th19.dat", g_dat_bytes)) return false;
        if (!g_dat.open(g_dat_bytes.data(), u32(g_dat_bytes.size()))) return false;
        g_dat_open = true;
    }
    // 归档条目名不带前导斜杠
    return g_dat.read(name[0] == '/' ? name + 1 : name, out, true);
}

bool load_anm(const char* path, std::vector<u8>& bytes, std::vector<th19::AnmEntry>& entries) {
    if (!read_asset(path, bytes)) return false;
    return th19::parse_anm(bytes.data(), u32(bytes.size()), entries);
}

void decode_textures(std::vector<th19::AnmEntry>& entries, const std::vector<u8>& bytes,
                     std::map<i32, TextureAllocation>& out) {
    for (i32 ei = 0; ei < (i32)entries.size(); ++ei) {
        const auto& e = entries[ei];
        if (e.thtx_size == 0 || e.data_offset + e.thtx_size > bytes.size()) continue;
        auto alloc = gfx.create_texture(bytes.data() + e.data_offset, e.thtx_size);
        if (alloc.handle) out[ei] = alloc;
    }
}

bool load_help() {
    if (!load_anm("/help.anm", g_help_bytes, g_help)) return false;
    decode_textures(g_help, g_help_bytes, g_help_tex);
    g_help_sprites.clear();
    for (i32 ei = 0; ei < (i32)g_help.size(); ++ei)
        for (i32 si = 0; si < (i32)g_help[ei].sprites.size(); ++si)
            g_help_sprites[g_help[ei].sprites[si].id] = {ei, si};
    g_help_scripts.clear();
    for (i32 ei = 0; ei < (i32)g_help.size(); ++ei) {
        const auto& e = g_help[ei];
        for (const auto& sc : e.scripts) {
            th19::AnmScriptLoc loc;
            loc.stream = g_help_bytes.data() + e.base + sc.offset;
            loc.len = u32(g_help_bytes.size()) - e.base - sc.offset;
            loc.table = &e.scripts;
            g_help_scripts[sc.id] = loc;
        }
    }
    for (i32 i = 0; i < 13; ++i) {
        char path[64];
        std::snprintf(path, sizeof path, "/help_%02d.png", i + 1);
        std::vector<u8> bytes;
        if (read_asset(path, bytes)) {
            auto alloc = gfx.create_texture(bytes.data(), u32(bytes.size()));
            if (alloc.handle) g_help_page_tex[i] = alloc;
        }
    }
    return true;
}

void build_sprite_map() {
    g_sprite_map.clear();
    for (i32 ei = 0; ei < (i32)g_title.size(); ++ei)
        for (i32 si = 0; si < (i32)g_title[ei].sprites.size(); ++si)
            g_sprite_map[g_title[ei].sprites[si].id] = {ei, si};
}

// build global script-id table across all title.anm entries
void build_script_map() {
    g_scripts.clear();
    for (i32 ei = 0; ei < (i32)g_title.size(); ++ei) {
        const auto& e = g_title[ei];
        for (const auto& sc : e.scripts) {
            th19::AnmScriptLoc loc;
            loc.stream = g_title_bytes.data() + e.base + sc.offset;
            loc.len = u32(g_title_bytes.size()) - e.base - sc.offset;
            loc.table = &e.scripts;
            g_scripts[sc.id] = loc;
        }
    }
}

// bind a title VM to a global script id
bool bind_script(i32 script_id, th19::AnmVm& out) {
    auto it = g_scripts.find(script_id);
    if (it == g_scripts.end() || !it->second.stream) return false;
    out.bind_entry(it->second.stream, it->second.len, &g_scripts);
    return true;
}

// 绑定标题前台元素（灵梦/logo/菜单图标/立绘）。返回主菜单时重播其入场演出。
void bind_title_vms(bool returning=false) {
    g_title_vms.clear();
    th19::AnmVm vm;
    if (bind_script(0, vm)) g_title_vms.push_back(std::move(vm)); // reimu
    if (bind_script(4, vm)) g_title_vms.push_back(std::move(vm)); // logo
    if (bind_script(11, vm)) g_title_vms.push_back(std::move(vm));// menu icons (TITLE ROOT)
    if (bind_script(87, vm)) g_title_vms.push_back(std::move(vm));// portrait (MENU ROOT)
    // TitleInf 55a110: return path uses immediate interrupt2, not startup replay.
    if(returning)for(auto&v:g_title_vms){v.tick();v.interrupt_all(2);v.tick();}
    g_title_v_vm={};
    auto version=g_title_v_scripts.find(0);
    if(version!=g_title_v_scripts.end()){
        g_title_v_vm.bind_entry(version->second.stream,version->second.len,&g_title_v_scripts);
        if(returning){g_title_v_vm.tick();g_title_v_vm.interrupt(2);}
    }

}

bool bind_help_script(i32 script_id, th19::AnmVm& out) {
    auto it = g_help_scripts.find(script_id);
    if (it == g_help_scripts.end() || !it->second.stream) return false;
    out.bind_entry(it->second.stream, it->second.len, &g_help_scripts);
    return true;
}

void start_help_menu() {
    g_help_vms.clear();
    for (i32 i = 0; i < 13; ++i) {
        th19::AnmVm vm;
        if (bind_help_script(i, vm)) g_help_vms.push_back(std::move(vm));
    }
    g_help_mode = 0;
    g_help_anim = 0; g_help_anim_frame = 0; g_help_anim_next = 0;
}

void tick_help_transition(){
        if (g_help_mode == 1 && g_help_anim != 0) {
            if (++g_help_anim_frame >= 20) {
                g_help_anim_frame = 0;
                if (g_help_anim == 1) {
                    g_help_anim = 0;                       // 淡入完成
                } else if (g_help_anim == 2 || g_help_anim == 3) {
                    const i32 act = g_help_anim_next;      // 滑出完成
                    g_help_anim = 0; g_help_anim_next = 0;
                    if (act == 1) { ++g_help_page; g_help_anim = 1; }
                    else if (act == 2) { --g_help_page; g_help_anim = 1; }
                    else { start_help_menu(); }
                }
            }
        }
}

void start_animation() {
    g_bg_vms.clear();
    g_title_vms.clear();
    th19::AnmVm vm;
    // Title screen root scripts (dump_title_scripts.js): each root creates its
    // children via the ANM createChild instruction (opcode 500/502).
    // 背景 = script 1（title_bk01 星空背景，进入子界面时保留）；
    // 灵梦/logo/菜单图标/立绘 = 转场时淡出。
    if (bind_script(1, vm)) g_bg_vms.push_back(std::move(vm));   // background
    bind_title_vms();
}

// ---- loading screen (ascii/loading.png) ----
// Find the ascii entry holding sprite 912 (loading.png), build its sprite/script
// maps, then bind the two root scripts (17 = 少女祈祷中 text group, 19 = rings).
void build_loading() {
    g_loading_sprites.clear();
    g_loading_scripts.clear();
    g_loading_entry = -1;
    for (i32 ei = 0; ei < (i32)g_ascii.size(); ++ei) {
        const auto& e = g_ascii[ei];
        for (i32 si = 0; si < (i32)e.sprites.size(); ++si) {
            if (e.sprites[si].id == 912) { g_loading_entry = ei; break; }
        }
        if (g_loading_entry >= 0) break;
    }
    if (g_loading_entry < 0) return;
    const auto& e = g_ascii[g_loading_entry];
    for (i32 si = 0; si < (i32)e.sprites.size(); ++si)
        g_loading_sprites[e.sprites[si].id] = {g_loading_entry, si};
    for (const auto& sc : e.scripts) {
        th19::AnmScriptLoc loc;
        loc.stream = g_ascii_bytes.data() + e.base + sc.offset;
        loc.len = u32(g_ascii_bytes.size()) - e.base - sc.offset;
        loc.table = &e.scripts;
        g_loading_scripts[sc.id] = loc;
    }
}
bool bind_loading_script(i32 script_id, th19::AnmVm& out) {
    auto it = g_loading_scripts.find(script_id);
    if (it == g_loading_scripts.end() || !it->second.stream) return false;
    out.bind_entry(it->second.stream, it->second.len, &g_loading_scripts);
    return true;
}
void start_loading() {
    g_loading_vms.clear();
    th19::AnmVm vm;
    if (bind_loading_script(17, vm)) g_loading_vms.push_back(std::move(vm));  // 少女祈祷中 + Now Loading + 装饰环
    // script 19 (16 个白色气泡环散布全屏) 已删除——用户反馈是"气泡重影"，多余。
}

// 结局播放状态（start_sub_screen 前声明，供 SCR_ENDING case 使用）
i32 g_ending_id = 0;                 // 当前结局 0-18
i32 g_ending_line = 0;
i32 g_ending_reveal_frame = 0;   // 最近一次揭示台词的帧号（逐行淡入）
i32 g_ending_cg_frame = -1;      // 结局立绘淡入起始帧（进入角色台词块时置位，-1=未进入）
extern i32 g_ending_return_screen;
bool g_clear_loaded=false,g_ending_repeat=false;u8 g_character_clear[19]{};
extern u32 g_trophy_unlocked[23]; extern u32 g_music_unlocked[22];
u32 g_save_revision=0;
bool g_native_score_loaded=false;
u32 g_native_score_fingerprint=0;
bool g_notice_registered[24]{};
std::vector<int> g_pending_route_notices;
std::string notice_progress_path(){char p[72];std::snprintf(p,sizeof p,"/save/th19-notices-%08x.bin",g_native_score_fingerprint);return p;}
void save_notice_progress(){
 std::vector<u8>b={'T','1','9','N',1};for(bool v:g_notice_registered)b.push_back(v);
 for(int id:g_pending_route_notices)b.push_back(u8(id));
 FILE*f=fopen(notice_progress_path().c_str(),"wb");if(f){size_t n=fwrite(b.data(),1,b.size(),f);fclose(f);if(n==b.size())++g_save_revision;}
}
void load_notice_progress(){
 std::vector<u8>b;if(!read_file(notice_progress_path().c_str(),b)||b.size()<29||b.size()>52||std::memcmp(b.data(),"T19N\1",5))return;
 for(int i=0;i<24;++i)if(b[5+i]>1)return;
 bool seen[24]{};for(size_t i=29;i<b.size();++i){int id=b[i];if(id<1||id>23||seen[id]||!b[5+id])return;seen[id]=true;}
 for(int i=0;i<24;++i)g_notice_registered[i]|=b[5+i]!=0;
 g_pending_route_notices.assign(b.begin()+29,b.end());
}
// 518f81..518feb: trophy 17 belongs to character 13; characters
// 14..17 map to trophies 13..16. The other route IDs are unchanged.
int route_trophy(int ch){return ch<0||ch>=19?-1:ch==13?17:ch>=14&&ch<=17?ch-1:ch;}
std::vector<int> g_trophy_popup_queue;
bool g_story_challenge_run=false;int g_story_initial_lives=5,g_story_continues=0;
void grant_route_trophy(int ch,bool announce=false){int id=route_trophy(ch);if(id>=0&&!g_trophy_unlocked[id]){g_trophy_unlocked[id]=1;if(announce)g_trophy_popup_queue.push_back(id);}}
std::string web_progress_path(){char p[64];std::snprintf(p,sizeof p,"/save/th19-progress-%08x.bin",g_native_score_fingerprint);return p;}
void save_web_progress(){
 std::vector<u8>b={'T','1','9','P',1};
 b.insert(b.end(),g_character_clear,g_character_clear+19);
 for(int i=0;i<23;++i)b.push_back(g_trophy_unlocked[i]!=0);
 FILE*f=fopen(web_progress_path().c_str(),"wb");if(f){size_t n=fwrite(b.data(),1,b.size(),f);fclose(f);if(n==b.size())++g_save_revision;}
}
bool g_story_available[19]={true},g_vs_available[19]={true,true,true,true,true,true,true,true,true,true};
bool character_available(int ch){return ch>=0&&ch<19&&(g_vs_mode?g_vs_available[ch]:g_story_available[ch]);}
// Use one decoder for startup and website imports; reject invalid saves before replacing progress.
bool decode_native_score(const std::vector<u8>&raw,std::vector<u8>&plain,const u8*&status){
 if(raw.size()<24||raw.size()>16*1024*1024)return false;
  auto word=[](const u8*p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;};
  if(word(raw.data())!=0x54483139||word(raw.data()+8)!=2||word(raw.data()+4)!=raw.size())return false;
  u32 packed=word(raw.data()+16),size=word(raw.data()+20);if(packed!=raw.size()-24||size!=0x2eb78)return false;
  std::vector<u8>z(packed);plain.resize(size);th19::resource_crypt(raw.data()+24,z.data(),packed,{0xac,0x35,16,packed},false);
  th19::Lzss codec;u32 written=0;if(!codec.decode(z.data(),packed,plain.data(),size,written)||written!=size)return false;
  bool valid=true;status=nullptr;
  for(u32 pos=0;pos<size;){if(size-pos<12){valid=false;break;}const u8*b=plain.data()+pos;u32 n=word(b+8),tag=word(b);if(n<12||n>size-pos){valid=false;break;}
   u32 sum=0;for(u32 j=8;j<n;++j)sum+=b[j];if(sum!=word(b+4)){valid=false;break;}
   if(tag==0x45453&&n==0x318)status=b;else if(tag!=0x35243||n!=0x2538){valid=false;break;}pos+=n;
  }
 return valid&&status;
}
void load_native_score(){
 std::vector<u8> raw,plain;
 for(const char*path:{"/save/scoreth19.dat","/save/scoreth19bak.dat"}){
  const u8*status=nullptr;
  if(!read_file(path,raw)||!decode_native_score(raw,plain,status))continue;
  g_native_score_fingerprint=2166136261u;for(u8 v:raw)g_native_score_fingerprint=(g_native_score_fingerprint^v)*16777619u;
  auto flag=[&](int offset){return status[offset-0x2e868]!=0;};
  for(int i=0;i<19;++i){g_character_clear[i]=flag(0x2eb16+i);g_char_cleared[i]=g_character_clear[i];g_story_available[i]=flag(0x2ead6+i);g_vs_available[i]=flag(0x2eaf6+i);}
  for(int i=0;i<23;++i)g_trophy_unlocked[i]=flag(0x2e914+i);
  for(int i=0;i<24;++i)g_notice_registered[i]=flag(0x2ea16+i);
  for(int i=0;i<22;++i)g_music_unlocked[i]=flag(0x2e89e + i);
  g_clear_loaded=true;g_native_score_loaded=true;return;
 }
}
// Native518e80 ->413320/4132c0: story and VS grants are independent.
void apply_route_unlocks(int ch){
 auto story=[&](std::initializer_list<int> ids){for(int i:ids)g_story_available[i]=true;};
 auto versus=[&](std::initializer_list<int> ids){for(int i:ids)g_vs_available[i]=true;};
 switch(ch){
 case 0:story({1,2});versus({13,17,18});break;
 case 1:story({3,6,5});versus({11,15,18});break;
 case 2:story({3,4,9});versus({10,14,18});break;
 case 3:story({7,8});versus({13,16,12});break;
 case 4:story({8});versus({10,14});break;
 case 5:story({9});versus({15,10});break;
 case 6:story({8});versus({15,11});break;
 case 7:versus({16,12});break;
 case 8:versus({14,11});break;
 case 9:versus({12,16});break;
 case 13:story({18});break;
 case 17:story({13});break;
 }
 int count=0;for(int i=0;i<19;++i)count+=g_character_clear[i]!=0;
 if(count>=10)story({10,11,12});
 if(count>=13)story({14,15,16});
 if(count>=16)story({17});
}
void load_web_clears(){
 load_notice_progress();
 std::vector<u8>b;
 if(read_file(web_progress_path().c_str(),b)){
  if(b.size()!=47||std::memcmp(b.data(),"T19P\1",5)!=0)return;
  for(size_t i=5;i<b.size();++i)if(b[i]>1)return;
  for(int i=0;i<23;++i)g_trophy_unlocked[i]|=b[24+i];
  b=std::vector<u8>(b.begin()+5,b.begin()+24);
 }else{
  // Legacy test clears have no source-save identity. Never merge them
  // into an imported native save and turn locked content into unlocked.
  if(g_native_score_loaded||!read_file("/save/th19-character-clear.bin",b)||b.size()!=19)return;
 }
 for(u8 v:b)if(v>1)return;
 for(int i=0;i<19;++i)if(b[i]){g_character_clear[i]=1;g_char_cleared[i]=true;}
 for(int i=0;i<19;++i)if(b[i]){apply_route_unlocks(i);grant_route_trophy(i);}
 g_clear_loaded=true;
}
void enter_ending_clear(){
    if(!g_clear_loaded){
        std::vector<u8>b; bool loaded=false;
        if(read_file("/save/th19-character-clear.bin",b)&&b.size()==19){std::copy(b.begin(),b.end(),g_character_clear);loaded=true;}
        // Native scoreth19.dat is encoded; never infer flags from arbitrary byte runs.
        for(int i=0;i<19;++i)g_char_cleared[i]=g_character_clear[i]!=0;
        g_clear_loaded=true;
    }
    g_ending_repeat=g_character_clear[g_ending_id]!=0;
    if(g_ending_return_screen==SCR_TITLE){g_character_clear[g_ending_id]=1;g_char_cleared[g_ending_id]=true;apply_route_unlocks(g_ending_id);grant_route_trophy(g_ending_id,true);
      if(g_story_challenge_run&&!g_vs_mode&&g_story_difficulty>=1){
       auto award=[&](int id){if(!g_trophy_unlocked[id]){g_trophy_unlocked[id]=1;g_trophy_popup_queue.push_back(id);}};
       if(g_story_continues==0){award(19);if(g_p1_lives==g_story_initial_lives)award(20);}
       if(g_story_difficulty==2)award(21);if(g_story_difficulty>=3)award(22);
       g_story_challenge_run=false;
      }
      int count=0;bool all_vs=true;for(int i=0;i<19;++i){count+=g_character_clear[i]!=0;all_vs&=g_vs_available[i];}
      for(int id:native_route_notices(g_ending_id,count,all_vs))if(!g_notice_registered[id]){g_notice_registered[id]=true;g_pending_route_notices.push_back(id);}
      save_notice_progress();save_web_progress();FILE*f=fopen("/save/th19-character-clear.bin","wb");if(f){fwrite(g_character_clear,1,19,f);fclose(f);}}
}
int g_ending_group=0;
int g_final_exit=-1,g_ending_tail=-1,g_staff_frame=0,g_staff_fade=-1;size_t g_staff_event=0;
std::vector<u8> g_staff_bytes;std::vector<th19::AnmEntry> g_staff_entries;
std::map<i32,TextureAllocation> g_staff_tex;std::map<u32,std::pair<i32,i32>> g_staff_sprites;
std::unordered_map<i32,th19::AnmScriptLoc> g_staff_scripts;th19::AnmVm g_staff_slots[16];
void start_staff();void tick_staff(bool confirm);void draw_staff();
void init_music_animation();void tick_music_animation();void select_music_animation();void reveal_music_comment();void close_music_animation();
i32 g_ending_return_screen = SCR_ACHIEVE;  // 结局播完返回的界面（成就触发→成就；正式游戏结局→主菜单）

// ending background + character sprite (lazy-loaded per ending id)
TextureAllocation g_end_bg_tex, g_end_chr_tex;
i32 g_end_loaded_id = -1;               // 当前台词行号

// 进入子界面：绑定该界面的标题条/图标脚本。返回 false 表示无需额外脚本（Option）。
void init_vs_cards(int side){
 load_ability_cards();auto&list=g_vs_card_vms[side];list.clear();
 if(g_vs_rules[2]<2)return;
 const bool all=g_vs_rules[2]==3;const int count=all?39:2;
 auto it=g_card_scripts.find(25);if(it==g_card_scripts.end())return;
 g_vs_cards[side].index%=count;
 for(int i=0;i<count;++i){th19::AnmVm v;v.bind_entry(it->second.stream,it->second.len,&g_card_scripts);v.tick();v.sprite=5+2*(all?i:1+2*(side?g_opp_sel:g_char_sel)+i);
 const int d=all?(i-g_vs_cards[side].index+count)%count:i-g_vs_cards[side].index;
 if(d==0)v.interrupt(20);else if(d==1)v.interrupt(21);else if(d==(all?count-1:-1))v.interrupt(19);
 list.push_back(std::move(v));}
}
void start_sub_screen(i32 screen) {
    g_sub_vms.clear();
    th19::AnmVm vm;
    auto bind = [&](i32 id){ if (bind_script(id, vm)) g_sub_vms.push_back(std::move(vm)); };
    switch (screen) {
        case SCR_STORY: bind(19);for(auto&v:g_sub_vms){v.tick();v.interrupt_all(3);v.tick();v.interrupt_all(g_sub_sel+13);}break;
        case SCR_VS: bind(190);for(auto&v:g_sub_vms){v.tick();v.interrupt_all(3);v.tick();v.interrupt_all(g_sub_sel+7);}break;
        case SCR_ACHIEVE: bind(208);g_achievement_frame=0;g_achievement_detail_frame=0;g_achievement_exiting=false;
            for(int i=0;i<4;++i){auto&v=g_achievement_details[i];v={};bind_script(237+i,v);v.tick();}break;
        case SCR_MUSIC:   bind(206); init_music_animation(); break; // sl_music 标题条（列表/评论为文字渲染，非 ANM；203 是 VS 列表，勿绑）
        case SCR_MANUAL:  bind(207); g_help_sel = 0; g_help_page = 0; start_help_menu(); break; // sl_manual 标题条 + 帮助菜单
        case SCR_OPTION:  break;                        // Option 无标题条，标题+内容全部文字渲染（OptionTaskInf / text.anm）
        case SCR_CHARSEL:
            g_vs_first_confirmed=false;g_vs_second_confirmed=false;g_vs_match_confirm=-1;
            g_vs_cursor_vm[0]={};bind_script(21,g_vs_cursor_vm[0]);
            if(!character_available(g_char_sel))for(int i=0;i<19;++i)if(character_available(i)){g_char_sel=i;break;}
            if(!character_available(g_opp_sel))for(int i=0;i<19;++i)if(character_available(i)){g_opp_sel=i;break;}
            if(g_vs_mode){
                init_vs_cards(0);init_vs_cards(1);
                g_vs_first_confirmed=false;g_vs_second_confirmed=false;g_vs_match_confirm=-1;g_p2_cursor_cx=CH_CX[g_opp_sel];g_p2_cursor_cy=CH_CY[g_opp_sel];
                // 556340 selects 21/22, 21/24, or 23/24 for the three VS entries.
                const int scripts[2]={g_vs_entry==2?23:21,g_vs_entry==0?22:24};
                for(int side=0;side<2;++side){
                    g_vs_cursor_vm[side]={};bind_script(scripts[side],g_vs_cursor_vm[side]);
                    auto&portrait=g_vs_portrait_vm[side];portrait={};bind_script(78+side,portrait);
                    portrait.tick();portrait.interrupt_all(3);portrait.tick();portrait.interrupt_all((side?g_opp_sel:g_char_sel)+7);
                }
            }
            break;
        case SCR_KEYCONFIG: g_key_sel = 0; g_key_mode = 0; break;  // Key Config 无标题条，文字渲染
        case SCR_ENDING: enter_ending_clear();g_ending_tail=-1;g_ending_group=END_GROUP_OFFSETS[g_ending_id];g_ending_line = END_GROUPS[g_ending_group].last; g_ending_reveal_frame = g_frame; g_ending_cg_frame = -1; g_end_loaded_id = -1; play_music_track(20); break;  // 结局：播 bgm/th19_21，重置台词行
        default: break;
    }
    if (screen == SCR_MUSIC) g_sub_case_delay = 1;   // 仅 Music 子项初始化后发 case（Option 内容是文字，非 ANM）
}

// Flat 2D sprite pipeline: color = texture * vertex-tint (Multiply), so the
// per-sprite tint (color + alpha) actually applies. th19 tints white glyphs
// via the vertex diffuse; First would ignore the tint entirely.
PipelineState sprite_pipeline() {
    PipelineState p;
    p.depthTest = p.depthWrite = p.fog = false;
    p.blend = true; p.sourceBlend = BlendFactor::SourceAlpha; p.destinationBlend = BlendFactor::InverseSourceAlpha;
    p.color.operation = p.alpha.operation = ColorOperation::Multiply;
    p.color.first = p.alpha.first = {ArgumentSource::Texture};
    p.color.second = p.alpha.second = {ArgumentSource::Diffuse};
    return p;
}

// Additive pipeline for procedural glow effects. SourceAlpha ×
// texture + destination, so overlapping glows brighten toward gold/white.
PipelineState additive_pipeline() {
    PipelineState p = sprite_pipeline();
    p.destinationBlend = BlendFactor::One;
    return p;
}

// 1x1 white texture for the fullscreen fade overlay (screen transition).
TextureAllocation g_white_tex;
TextureAllocation g_music_tex;        // 曲名预渲染图（No.+曲名，单列 22 行，40px 行距）
TextureAllocation g_music_locked_tex; // 未解锁占位图（No.+？？？）
TextureAllocation g_music_dialog_tex; // 未解锁确认对话框（8 行）
TextureAllocation g_music_comment_tex; // 曲目评论图（22 曲 × 8 行）
TextureAllocation g_trophy_num_tex;     // 成就网格数字（1-23 + 锁定"？？"）
TextureAllocation g_trophy_title_tex;   // 成就称号（23 + 锁定占位）
TextureAllocation g_trophy_title_big_tex; // 成就详情大字标题
TextureAllocation g_trophy_desc_tex;    // 成就说明（23×3 未取得 + 23×3 取得）
TextureAllocation g_ending_tex;      // 结局台词图集
// Fresh profiles start locked. Native import and actual route clears grant achievements.
u32 g_trophy_unlocked[23] = {};
// 未解锁状态（0=锁定 / 1=解锁）。原版由存档进度驱动；移植暂无存档，默认解锁前 3 曲演示锁定态。
u32 g_music_unlocked[22] = {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// 未解锁曲目也可播放（用户要求：没解锁的也能听，只是曲名不能显示）——曲名显示"？？？"，评论照常
i32 g_music_scroll = 0;               // 滚动目标（整数，可视窗口首项索引）
i32 g_music_prompt = 0;             // 未解锁提示标志（源码 0xaf0：0=未弹 / 1=已弹提示）
i32 g_music_comment_track = 0;        // 评论区显示的已确认曲目（源码 0xaec），仅在按 Z 时更新
float g_music_scroll_cur = 0.0f;      // 当前滚动（浮点，缓动）
void build_white_texture() {
    u8 rgba[4] = {255,255,255,255};
    g_white_tex = gfx.create_raw(1, 1, rgba);
}

// 选中框渐变纹理：黑色，左右边缘 alpha 渐变到 0（实机选中项黑底左右边缘逐渐透明）。
// 横向渐变：左 0 → 中间不透明 → 右 0，中间大部分保持不透明（实机边缘渐变约 8.6%）。
TextureAllocation g_box_tex;
void build_select_box_texture() {
    const u32 W = 512, H = 2;
    std::vector<u8> rgba(W * H * 4, 0);
    for (u32 y = 0; y < H; ++y) {
        for (u32 x = 0; x < W; ++x) {
            const float t = (float(x) + 0.5f) / float(W);   // 0..1
            // 左右各约 9% 渐变到透明，中间保持不透明
            const float edge = 0.09f;
            float a;
            if (t < edge) a = t / edge;
            else if (t > 1.0f - edge) a = (1.0f - t) / edge;
            else a = 1.0f;
            a = a * a * (3.0f - 2.0f * a);   // smoothstep 平滑
            const size_t i = (size_t(y) * W + x) * 4;
            rgba[i + 0] = 0; rgba[i + 1] = 0; rgba[i + 2] = 0;   // 黑色
            rgba[i + 3] = u8(a * 255.0f);                          // 渐变 alpha
        }
    }
    g_box_tex = gfx.create_raw(W, H, rgba.data());
}

// draw one sprite (given its decoded texture) at (x,y), scale (sx,sy), tint (0xAARRGGBB).
// anchor (opcode 421 +0x4c7/+0x4c8): 0 = center (pos is center), 1 = top-left (pos is corner).
void draw_sprite(const TextureAllocation& alloc, const th19::AnmSprite& sp,
                 float x, float y, float sx, float sy, u8 ax, u8 ay, u32 tint) {
    if (g_skip_present || !alloc.handle) return;
    const float tw = float(alloc.width), th = float(alloc.height);
    const float u0 = sp.x / tw, v0 = sp.y / th;
    const float u1 = (sp.x + sp.w) / tw, v1 = (sp.y + sp.h) / th;
    const float w = sp.w * sx, h = sp.h * sy;
    // anchor: 0=center, 1=top/left, 2=bottom/right
    const float x0 = (ax == 2) ? x - w : (ax == 1 ? x : x - w * 0.5f);
    const float x1 = x0 + w;
    const float y0 = (ay == 2) ? y - h : (ay == 1 ? y : y - h * 0.5f);
    const float y1 = y0 + h;
    SpriteVertex q[4];
    q[0] = {{x0, y0, 0}, 1, tint, {u0, v0}};
    q[1] = {{x1, y0, 0}, 1, tint, {u1, v0}};
    q[2] = {{x0, y1, 0}, 1, tint, {u0, v1}};
    q[3] = {{x1, y1, 0}, 1, tint, {u1, v1}};
    gfx.draw_sprite(alloc.handle, sprite_pipeline(), q, 4);
}

// A single draw command collected from the ANM VM tree, then sorted by layer
// so sprites composite correctly (background layer 25 < reimu 26 < menu text 29
// < logo 30 < menu icons 35).
struct DrawCmd {
    i32 layer;
    u32 handle;
    float x, y, sx, sy, sw, sh;   // pos, scale, sprite pixel size
    u8 ax, ay;
    u32 tint;
    float u0, v0, u1, v1;
    float rot;                    // rotation (radians) around the sprite center
    float skew_x = 0;             // horizontal skew (shear)
    float skew_y = 0;             // vertical skew (shear) — 随难度上下倾斜
    bool additive;                // procedural glow / effect: additive blend
    std::vector<SpriteVertex> mesh;
    u32 mesh_fog=0;
    int mesh_blend=0;bool line_strip=false;
    int address_u=1,address_v=1;
    bool mesh_depth=false;
    bool world_capture=false,mesh_fog_enabled=true;
    bool mesh_depth_write=true;
    int clip_side=-1;
};
std::vector<DrawCmd> g_draw;
int g_actor_clip=-1;

// push a fullscreen black quad with the given alpha (0..255) at topmost layer
void draw_fade_quad(u32 alpha) {
    if (!g_white_tex.handle || alpha == 0) return;
    DrawCmd c;
    c.layer = 100; c.handle = g_white_tex.handle;
    c.x = SCREEN_W * 0.5f; c.y = SCREEN_H * 0.5f;
    c.sx = SCREEN_W; c.sy = SCREEN_H; c.sw = 1.0f; c.sh = 1.0f;
    c.ax = 0; c.ay = 0;
    c.tint = (alpha << 24);   // black, alpha-controlled
    c.u0 = 0; c.v0 = 0; c.u1 = 1; c.v1 = 1;
    c.rot = 0; c.additive = false;
    g_draw.push_back(c);
}

// append a quad into g_draw (from a decoded sprite + texture), not drawn yet
void emit_quad(i32 layer, const TextureAllocation& alloc, const th19::AnmSprite& sp,
               float x, float y, float sx, float sy, u8 ax, u8 ay, u32 tint, float rot,
               bool additive=false, int blend_mode=0) {
    if (g_skip_present || !alloc.handle) return;
    DrawCmd c;
    c.layer = layer; c.handle = alloc.handle;c.clip_side=g_actor_clip;
    c.x = x; c.y = y; c.sx = sx; c.sy = sy;
    // Native sprite geometry: add per-sprite offset, apply sprite and VM
    // scales, then rotate into world space (4c8d30 / 4c8780).
    const float sprite_dx=sp.offset_x*sp.scale_x*sx;
    const float sprite_dy=sp.offset_y*sp.scale_y*sy;
    c.x+=sprite_dx*std::cos(rot)-sprite_dy*std::sin(rot);
    c.y+=sprite_dx*std::sin(rot)+sprite_dy*std::cos(rot);
    c.sx*=sp.scale_x;c.sy*=sp.scale_y;
    c.sw = sp.w; c.sh = sp.h;
    c.ax = ax; c.ay = ay; c.tint = tint;
    const float tw = float(alloc.width), th = float(alloc.height);
    c.u0 = sp.x / tw; c.v0 = sp.y / th;
    c.u1 = (sp.x + sp.w) / tw; c.v1 = (sp.y + sp.h) / th;
    c.rot = rot;
    c.additive = additive;
    c.mesh_blend = blend_mode;
    g_draw.push_back(c);
}

// Native layer 40 runs in composite task 005321a0 (priority 65), after
// background layer 25 (priority 63), before foreground layer 26 (priority 70).
// Its @R texture contains only the background pass, never text or portraits.
void draw_title_wave() {
    if(!g_title_wave_capture.handle){
        std::vector<u8> pixels(1280*960*4);
        g_title_wave_capture=gfx.create_raw(1280,960,pixels.data());
    }
    DrawCmd cmd{};cmd.layer=26;cmd.handle=g_title_wave_capture.handle;
    cmd.world_capture=true;cmd.additive=true;cmd.mesh_fog_enabled=false;
    cmd.address_u=cmd.address_v=3;
    std::array<SpriteVertex,th19::TitleWave::columns*th19::TitleWave::rows> grid;
    for(int x=0;x<th19::TitleWave::columns;++x)for(int y=0;y<th19::TitleWave::rows;++y){
        const auto v=g_title_wave.vertex(x,y);
        grid[x*th19::TitleWave::rows+y]={{v.x,v.y,0},1,v.tint,{v.u,v.v}};
    }
    for(int x=0;x<th19::TitleWave::columns-1;++x)for(int y=0;y<th19::TitleWave::rows-1;++y){
        const int a=x*th19::TitleWave::rows+y,b=a+th19::TitleWave::rows;
        // Native alternating triangle strip: a,b,a+1 then a+1,b,b+1.
        cmd.mesh.insert(cmd.mesh.end(),{grid[a],grid[b],grid[a+1],grid[a+1],grid[b],grid[b+1]});
    }
    g_draw.push_back(std::move(cmd));
}

void collect_vm(const th19::AnmVm& vm, float ox = 0.0f, float oy = 0.0f, bool vsExit=false) {
    // 子 VM 位置相对父 VM 累加（原版 ANM 语义）。标题根脚本均在 (0,0)，
    // 故主菜单/背景不受影响；Option/Music 组在 (640,480)/(640,520)，
    // 子项 pos=(0,0) 相对父组定位。
    const float x = vm.x + ox, y = vm.y + oy;
    if (vm.visible && vm.sprite >= 0) {
        auto it = g_sprite_map.find(u32(vm.sprite));
        if (it != g_sprite_map.end()) {
            const i32 entry = it->second.first;
            const auto& s = g_title[entry].sprites[it->second.second];
            auto ti = g_title_tex.find(entry);
            if (ti != g_title_tex.end()) {
                u32 a = u32(vm.alpha > 255 ? 255 : vm.alpha);
                a = u32(a * g_alpha_scale);            // 转场淡出淡入
                const u32 tint = (a << 24) | (u32(vm.r) << 16) | (u32(vm.g) << 8) | u32(vm.b);
                emit_quad(vm.layer, ti->second, s, x, y, vm.sx, vm.sy,
                          vm.anchor_x, vm.anchor_y, tint, vm.rot_z);
            }
        }
    }
    // title.anm 191/192 are procedural subtractive panels, not sprites.
    const float unit=(vm.coordinate_mode==1||vm.coordinate_mode==5)?2.f:1.f;
    // Keep the original ANM geometry animation.  In particular, VS panels
    // widen as the selected row moves down; applying a fixed scale here
    // flattened that animation into one constant-width plate.
    auto mesh=presentation_geometry(vm,unit,unit);
    if(!mesh.empty()){
        DrawCmd cmd{};cmd.layer=vm.layer;cmd.handle=g_white_tex.handle;
        cmd.mesh_blend=vm.blend_mode;cmd.mesh_fog_enabled=false;
        for(const auto&p:mesh){const u32 a=u32((p.color>>24)*g_alpha_scale);
            cmd.mesh.push_back(SpriteVertex{{ox+vm.x*unit+p.x,oy+vm.y*unit+p.y,0},1,(p.color&0xffffff)|(a<<24),{.5f,.5f}});}
        g_draw.push_back(std::move(cmd));
    }
    for (const auto& c : vm.children) collect_vm(c, x, y, vsExit);
}

// collect the loading-screen VM tree (sprites live in ascii entry#7, texture in g_ascii_tex)
// 少女祈祷中 (912) + Now Loading (913) sit at the bottom-right corner; the
// decorative rings (914) stay full-screen (they fly in from the screen edges).
const float LOAD_TEXT_X = 960.0f, LOAD_TEXT_Y = 780.0f;
void collect_loading_vm(const th19::AnmVm& vm) {
    if (vm.visible && vm.sprite >= 0) {
        auto it = g_loading_sprites.find(u32(vm.sprite));
        if (it != g_loading_sprites.end()) {
            const i32 entry = it->second.first;
            const auto& s = g_ascii[entry].sprites[it->second.second];
            auto ti = g_ascii_tex.find(entry);
            if (ti != g_ascii_tex.end()) {
                const u32 a = u32(vm.alpha > 255 ? 255 : vm.alpha);
                const u32 tint = (a << 24) | (u32(vm.r) << 16) | (u32(vm.g) << 8) | u32(vm.b);
                float ox = 0, oy = 0;
                // 文字 (912/913) 和彩色装饰环 (914, layer 50 = script 16) 在右下角；
                // 16 个白色粒子环 (914, layer 29 = script 18) 保持全屏飞入。
                if (vm.sprite == 912 || vm.sprite == 913 || (vm.sprite == 914 && vm.layer >= 50))
                    { ox = LOAD_TEXT_X; oy = LOAD_TEXT_Y; }
                emit_quad(vm.layer, ti->second, s, vm.x + ox, vm.y + oy, vm.sx, vm.sy,
                          vm.anchor_x, vm.anchor_y, tint, vm.rot_z);
            }
        }
    }
    for (const auto& c : vm.children) collect_loading_vm(c);
}

// draw the sig.anm copyright splash (1280x960) full-screen
void draw_sig_background() {
    if (g_sig.empty()) return;
    const auto& s = g_sig[0].sprites[0];
    auto ti = g_sig_tex.find(0);
    if (ti == g_sig_tex.end()) return;
    emit_quad(0, ti->second, s, 640.0f, 480.0f, 1.0f, 1.0f, 0, 0, 0xffffffff, 0);
}

// 0x4db7d0 uses destination alpha as its four-mask accumulation buffer.
// Color masks provide the same result without changing gameplay blend state.
void screen_switch_pipeline(int pass,PipelineState& p) {
    if(pass==-10){p.colorMask=8;p.blend=false;p.alphaTest=false;}
    else if(pass==-11){p.colorMask=8;p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::One;}
    else if(pass==-12){p.colorMask=7;p.sourceBlend=BlendFactor::DestinationAlpha;p.destinationBlend=BlendFactor::InverseDestinationAlpha;}
}
// sort g_draw by layer and emit the quads (ascending layer = painter's order)
// Field-local actors share the same clipping for quads and triangle meshes.
void flush_draw() {
    // EXE50e8d0: bottom-right %2.1ffps overlay, measured from presentations.
    g_present_fps.present(emscripten_get_now());g_fps_glyphs=0;
    const int fpsClip=g_actor_clip;g_actor_clip=-1;
    char fps[32];std::snprintf(fps,sizeof fps,"%2.1ffps",g_present_fps.value);
    float fpsX=1176;
    const u32 fpsColor=g_present_fps.value<30?0xff5050ff:g_present_fps.value<40?0xffa0a0ff:0xffffffff;
    for(const char* p=fps;*p;++p){const auto& glyph=g_fps_font[static_cast<unsigned char>(*p)];
        if(!glyph.texture.handle)continue;
        emit_quad(1200,glyph.texture,glyph.sprite,fpsX,940,.5f,.5f,1,1,fpsColor,0);
        ++g_fps_glyphs;fpsX+=glyph.sprite.w*.5f;
    }
    g_actor_clip=fpsClip;
    std::stable_sort(g_draw.begin(), g_draw.end(),
                     [](const DrawCmd& a, const DrawCmd& b) { return a.layer < b.layer; });
    static std::vector<SpriteVertex> meshClipScratch;
    for (const auto& c : g_draw) {
        if(c.world_capture)gfx.capture_screen(c.handle);
        if (!c.mesh.empty()) { auto pipeline=c.additive?additive_pipeline():sprite_pipeline();
            if(c.mesh_blend==2){pipeline.destinationBlend=BlendFactor::One;pipeline.blendEquation=BlendEquation::ReverseSubtract;}
        // Native 0x4c9dc0 mode4: D3DBLEND_INVDESTCOLOR / INVSRCCOLOR, ADD.
        if(c.mesh_blend==4){pipeline.sourceBlend=BlendFactor::InverseDestinationColor;pipeline.destinationBlend=BlendFactor::InverseSourceColor;pipeline.blendEquation=BlendEquation::Add;}
            pipeline.addressU=c.address_u==2?Address::Mirror:c.address_u==3?Address::Clamp:Address::Repeat;pipeline.addressV=c.address_v==2?Address::Mirror:c.address_v==3?Address::Clamp:Address::Repeat;
            pipeline.depthTest=c.mesh_depth;pipeline.depthWrite=c.mesh_depth&&c.mesh_depth_write;pipeline.depthCompare=Compare::LessEqual;
            pipeline.alphaTest=true;pipeline.alphaReference=1;pipeline.alphaCompare=Compare::GreaterEqual;
            pipeline.fog=c.mesh_fog_enabled;pipeline.fogColor=c.mesh_fog;screen_switch_pipeline(c.mesh_blend,pipeline);if(c.line_strip)gfx.draw_line_strip(c.handle,pipeline,c.mesh.data(),u32(c.mesh.size()));
            else if(c.clip_side<0)gfx.draw_sprite(c.handle,pipeline,c.mesh.data(),u32(c.mesh.size()));
            else {meshClipScratch.clear();
                for(size_t i=0;i+2<c.mesh.size();i+=3){const auto poly=th19::sdl::clipFieldPolygon(c.mesh.data()+i,3,c.clip_side);
                    for(size_t j=1;j+1<poly.count;++j)meshClipScratch.insert(meshClipScratch.end(),{poly.vertices[0],poly.vertices[j],poly.vertices[j+1]});}
                if(!meshClipScratch.empty())gfx.draw_sprite(c.handle,pipeline,meshClipScratch.data(),u32(meshClipScratch.size()));
            }continue; }
        const float w = c.sw * c.sx, h = c.sh * c.sy;
        // anchor: 0=center, 1=top/left, 2=bottom/right
        const float x0 = (c.ax == 2) ? c.x - w : (c.ax == 1 ? c.x : c.x - w * 0.5f);
        const float x1 = x0 + w;
        const float y0 = (c.ay == 2) ? c.y - h : (c.ay == 1 ? c.y : c.y - h * 0.5f);
        const float y1 = y0 + h;
        // rotation around the sprite center
        const float cr = std::cos(c.rot), sr = std::sin(c.rot);
        const float cx = c.x, cy = c.y;
        auto rot = [&](float px, float py)->std::pair<float,float>{
            const float dx = px - cx, dy = py - cy;
            return { cx + dx * cr - dy * sr, cy + dx * sr + dy * cr };
        };
        // skew_x 水平剪切：顶边 +skew_x，底边 -skew_x
        // skew_y 垂直剪切：左边 +skew_y，右边 -skew_y（随难度上下倾斜）
        const float hx0 = c.skew_x, hx1 = -c.skew_x;
        const float vy0 = c.skew_y, vy1 = -c.skew_y;
        auto p0 = rot(x0 + hx0, y0 + vy0), p1 = rot(x1 + hx0, y0 + vy1);
        auto p2 = rot(x0 + hx1, y1 + vy0), p3 = rot(x1 + hx1, y1 + vy1);
        SpriteVertex q[4];
        q[0] = {{p0.first, p0.second, 0}, 1, c.tint, {c.u0, c.v0}};
        q[1] = {{p1.first, p1.second, 0}, 1, c.tint, {c.u1, c.v0}};
        q[2] = {{p2.first, p2.second, 0}, 1, c.tint, {c.u0, c.v1}};
        q[3] = {{p3.first, p3.second, 0}, 1, c.tint, {c.u1, c.v1}};
        auto pipeline=c.additive?additive_pipeline():sprite_pipeline();
        if(c.mesh_blend==2){pipeline.destinationBlend=BlendFactor::One;pipeline.blendEquation=BlendEquation::ReverseSubtract;}
        // Native 0x4c9dc0 mode4: D3DBLEND_INVDESTCOLOR / INVSRCCOLOR, ADD.
        if(c.mesh_blend==4){pipeline.sourceBlend=BlendFactor::InverseDestinationColor;pipeline.destinationBlend=BlendFactor::InverseSourceColor;pipeline.blendEquation=BlendEquation::Add;}
        if(c.mesh_blend<0){pipeline.addressU=c.address_u==3?Address::Clamp:Address::Repeat;pipeline.addressV=c.address_v==3?Address::Clamp:Address::Repeat;}
        screen_switch_pipeline(c.mesh_blend,pipeline);
        if(c.clip_side<0){gfx.draw_sprite(c.handle,pipeline,q,4);continue;}
        // Clip the rotated quad, interpolating UVs at every field boundary.
        const SpriteVertex input[]{q[0],q[1],q[3],q[2]};
        const auto polygon=th19::sdl::clipFieldPolygon(input,4,c.clip_side);
        std::array<SpriteVertex,30> triangles;u32 count=0;
        for(size_t i=1;i+1<polygon.count;++i){triangles[count++]=polygon.vertices[0];triangles[count++]=polygon.vertices[i];triangles[count++]=polygon.vertices[i+1];}
        if(count)gfx.draw_sprite(c.handle,pipeline,triangles.data(),count);
    }
    g_draw.clear();
}

// menu text: FUN_004e5640 sets 3 color states (0xAARRGGBB). Selected item gets a
// bob offset from DAT_005a8358 when bob_phase>=1 (diagonal jitter on move).
// Font type 7 = ascii1280 entry#3 (main), type 9 = entry#4 (shadow). Shadow = 4px.
// 给文字 tint 应用转场淡出系数 g_fade（背景不淡出，文字随当前屏淡出）
u32 fade_tint(u32 tint) {
    u32 a = (tint >> 24) & 0xff;
    a = u32(a * g_fade);
    return (tint & 0x00ffffff) | (a << 24);
}

// 标题场景帧计数：入场顺序 = 背景淡入 → 灵梦淡入 → logo 弹出 → 选项最后淡入。
u32 g_title_frame = 0;
// 返回主菜单 logo 快进：>0 时每帧对 g_title_vms tick 8 次（整段 240 帧演出压到 30 帧 ≈ 0.5 秒播完）。
i32 g_logo_fast = 0;
// 55a110 waits through frame270; 559fa0 renders text only in states2..4.
float menu_intro_alpha() {
    return g_title_frame>270?1.0f:0.0f;
}
// 选项文字 tint：同时应用转场淡出（g_fade）与入场进度（menu_intro_alpha）。
u32 menu_fade(u32 tint) {
    u32 a = (tint >> 24) & 0xff;
    a = u32(a * g_fade * menu_intro_alpha());
    return (tint & 0x00ffffff) | (a << 24);
}

void draw_menu() {
    const i32 FONT_MAIN = 3, FONT_SHADOW = 4;
    auto mi = g_ascii_tex.find(FONT_MAIN), si = g_ascii_tex.find(FONT_SHADOW);
    if (mi == g_ascii_tex.end() || si == g_ascii_tex.end()) return;
    // DAT_005a8358 x-component table (raw floats at 8-byte stride), 640-space /2.0.
    // phase 0..8 = {2,-1,0,3,-3,2,-2,0,2}; rendered 1280-space offset = raw value.
    static const float BOB[9] = {2.0f, -1.0f, 0.0f, 3.0f, -3.0f, 2.0f, -2.0f, 0.0f, 2.0f};
    const float bob = (g_bob_phase >= 1 && g_bob_phase <= 9) ? BOB[g_bob_phase - 1] : 0.0f;
    for (i32 i = 0; i < 8; ++i) {
        const char* s = MENU[i];
        float cx = MENU_X, cy = MENU_Y0 + i * MENU_DY;
        u32 main_tint, shadow_tint;
        if (i == 2) {                       // 559fa0: locked Online VS (half alpha)
            main_tint = 0x80606060;
            shadow_tint = 0x80000000;
        } else if (i == g_sel) {             // selected: light yellow-white + black shadow
            if (g_flash_phase > 0 && (g_flash_phase & 3) < 2) {
                // 确认闪烁：黑字 + 亮黄阴影（快速闪几下）
                main_tint = 0xff000000;
                shadow_tint = 0xffffff80;
            } else {
                main_tint = 0xffffff80;
                shadow_tint = 0xff000000;
            }
            cx += bob; cy += bob;            // diagonal selection bob (DAT_005a8358)
        } else {                             // 4e5640: dark tan + native gray outline
            main_tint = 0xff808060;
            shadow_tint = 0xff404040;
        }
        for (const char* p = s; *p; ++p) {
            i32 idx = i32(u8(*p)) - 0x20;
            if (idx < 0 || idx >= (i32)g_ascii[FONT_MAIN].sprites.size()) { cx += 14; continue; }
            const auto& ms = g_ascii[FONT_MAIN].sprites[idx];
            const float gx = cx + ms.w * 0.5f, gy = cy + ms.h * 0.5f;
            // 4e7610 queues native font 9 once, then font 7. The atlas
            // already contains the outline; eight offset copies made it thicker.
            const auto& ss = g_ascii[FONT_SHADOW].sprites[idx];
            const auto glyph=[&](const auto& texture,const auto& sprite,u32 color){
                const float x=cx+sprite.w*.5f,y=cy+sprite.h*.5f;
                // Title text uses the atlas outline without an extra offset projection.
                emit_quad(29,texture,sprite,x,y,1,1,0,0,menu_fade(color),0);
            };
            glyph(si->second,ss,shadow_tint);
            glyph(mi->second,ms,main_tint);
            cx += ms.w;
        }
    }
}

// 通用文字列表渲染（子界面选项），复用主菜单同款字体与 3 色状态。
// items = 选项字符串数组，count = 项数，sel = 选中项（-1 = 无选中）。
void draw_text_list(const char* const* items, i32 count, i32 sel,
                    float x, float y0, float dy) {
    const i32 FONT_MAIN = 3, FONT_SHADOW = 4;
    auto mi = g_ascii_tex.find(FONT_MAIN), si = g_ascii_tex.find(FONT_SHADOW);
    if (mi == g_ascii_tex.end() || si == g_ascii_tex.end()) return;
    for (i32 i = 0; i < count; ++i) {
        float cx = x, cy = y0 + i * dy;
        u32 main_tint, shadow_tint;
        if (i == sel) { main_tint = 0xffffff80; shadow_tint = 0xff000000; }
        else          { main_tint = 0xff808060; shadow_tint = 0xff000000; }
        for (const char* p = items[i]; *p; ++p) {
            i32 idx = i32(u8(*p)) - 0x20;
            if (idx < 0 || idx >= (i32)g_ascii[FONT_MAIN].sprites.size()) { cx += 14; continue; }
            const auto& ms = g_ascii[FONT_MAIN].sprites[idx];
            const float gx = cx + ms.w * 0.5f, gy = cy + ms.h * 0.5f;
            // 黑色描边：黑字 8 方向偏移成描边，主字居中（所有项）
            const auto& ss = g_ascii[FONT_SHADOW].sprites[idx];
            const u32 ot = fade_tint(shadow_tint);
            emit_quad(29, si->second, ss, gx - 1, gy, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx + 1, gy, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx, gy - 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx, gy + 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx - 1, gy - 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx + 1, gy - 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx - 1, gy + 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx + 1, gy + 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, mi->second, ms, gx, gy, 1, 1, 0, 0, fade_tint(main_tint), 0);
            cx += ms.w;
        }
    }
}

// ---- 各子界面文字内容（英文，取自 title.cpp 字符串表/逆向） ----
static const char* const OPTION_ITEMS[6] = {
    "Screen", "BGM Vol", "SE Vol", "Key Config", "Default", "Quit"
};
// 源码 FUN_00536a40 switch(DAT_0062ee7c>>2&0x1f)：4 种画面模式
static const char* const SCREEN_MODES[4] = { "FullScreen", "Window", "BorderlessDBD", "Borderless" };

// 渲染一行文字（主菜单字体 entry3 主字 + entry4 阴影，char-0x20 映射，8 方向描边）
static void emit_opt_text(const char* s, float x, float y, u32 main_tint, bool outline) {
    const i32 FONT_MAIN = 3, FONT_SHADOW = 4;
    auto mi = g_ascii_tex.find(FONT_MAIN), si = g_ascii_tex.find(FONT_SHADOW);
    if (mi == g_ascii_tex.end()) return;
    float cx = x;
    for (const char* p = s; *p; ++p) {
        i32 idx = i32(u8(*p)) - 0x20;
        if (idx < 0 || idx >= (i32)g_ascii[FONT_MAIN].sprites.size()) { cx += 14; continue; }
        const auto& ms = g_ascii[FONT_MAIN].sprites[idx];
        const float gx = cx + ms.w * 0.5f, gy = y + ms.h * 0.5f;
        if (outline && si != g_ascii_tex.end()) {
            const auto& ss = g_ascii[FONT_SHADOW].sprites[idx];
            const u32 ot = 0xff000000;   // Option 字体不做淡入淡出
            emit_quad(29, si->second, ss, gx - 1, gy, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx + 1, gy, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx, gy - 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx, gy + 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx - 1, gy - 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx + 1, gy - 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx - 1, gy + 1, 1, 1, 0, 0, ot, 0);
            emit_quad(29, si->second, ss, gx + 1, gy + 1, 1, 1, 0, 0, ot, 0);
        }
        emit_quad(29, mi->second, ms, gx, gy, 1, 1, 0, 0, main_tint, 0);
        cx += ms.w;
    }
}

// 量测一行文字的屏幕宽度（与 emit_opt_text 同字体/映射，用于居中）
static float opt_text_width(const char* s) {
    const i32 FONT_MAIN = 3;
    auto mi = g_ascii_tex.find(FONT_MAIN);
    if (mi == g_ascii_tex.end()) return 0.0f;
    float w = 0.0f;
    for (const char* p = s; *p; ++p) {
        i32 idx = i32(u8(*p)) - 0x20;
        if (idx < 0 || idx >= (i32)g_ascii[FONT_MAIN].sprites.size()) { w += 14.0f; continue; }
        w += g_ascii[FONT_MAIN].sprites[idx].w;
    }
    return w;
}

// Option 屏（设置）：标题 + 6 项 + 值（源码 FUN_00536a40 绘制 / FUN_00536d60 调节）
void draw_option() {
    // 用户要求：居中排布（标题居中，正文/值列居中靠拢）。无淡入淡出。
    emit_opt_text("Option", 640.0f - opt_text_width("Option") * 0.5f, 200.0f, 0xffffffff, true);   // 标题（居中）
    static const float BOB[9] = {2.0f, -1.0f, 0.0f, 3.0f, -3.0f, 2.0f, -2.0f, 0.0f, 2.0f};
    for (i32 i = 0; i < 6; ++i) {
        const float cy = 280.0f + i * 56.0f;
        const float bob = (i == g_sub_sel && g_bob_phase >= 1 && g_bob_phase <= 9) ? BOB[g_bob_phase - 1] : 0.0f;
        const u32 main_tint = (i == g_sub_sel) ? 0xffffff80 : 0xff808060;
        emit_opt_text(OPTION_ITEMS[i], 420.0f + bob, cy + bob, main_tint, true);
        char val[32];
        if (i == 0)      std::snprintf(val, sizeof val, "%s", SCREEN_MODES[g_screen_mode]);
        else if (i == 1) std::snprintf(val, sizeof val, "%d%%", g_bgm_vol);
        else if (i == 2) std::snprintf(val, sizeof val, "%d%%", g_se_vol);
        else continue;
        // 值：描边 + 选中高亮（选中时白色）
        const u32 val_tint = (i == g_sub_sel) ? 0xffffff80 : 0xff808060;
        emit_opt_text(val, 680.0f + bob, cy + bob, val_tint, true);
    }
}

// 应用 BGM/SE 音量到音频引擎（源码 FUN_00536cd0 -> 音频系统 SetVol）
static void apply_option_volumes() {
    g_audio.music_volume=g_bgm_vol/100.f;g_audio.sfx_volume=g_se_vol/100.f;
    if (g_audio.music) ma_sound_set_volume(&g_audio.music->sound, g_bgm_vol / 100.0f);
    for (auto& kv : g_audio.sfx) {const float volume=g_se_vol/100.f*kv.second->gain;if(kv.second->immediate)eagler::audio::web_sfx::volume(kv.second.get(),volume);else ma_sound_set_volume(&kv.second->sound,volume);}
}

// Option 值调节（源码 FUN_00536d60 case 2：左 0x40 / 右 0x80，音量每次 ±5 夹 0-100）
static void option_adjust(i32 dir) {
    if (g_sub_sel == 0) {
        g_screen_mode = (g_screen_mode + dir + 4) % 4;   // Screen: 4 种模式循环（源码 0-9 映射 4 标签）
    } else if (g_sub_sel == 1) {
        g_bgm_vol += dir * 5;
        if (g_bgm_vol < 0) g_bgm_vol = 0;
        if (g_bgm_vol > 100) g_bgm_vol = 100;
    } else if (g_sub_sel == 2) {
        g_se_vol += dir * 5;
        if (g_se_vol < 0) g_se_vol = 0;
        if (g_se_vol > 100) g_se_vol = 100;
    } else return;
    apply_option_volumes();
    play_sfx(10);   // 调节反馈音（近似 se_select00，具体文件待核实）
}

// Option Z 确认（源码 FUN_00536d60：Default 重置 / Quit 返回）
static void option_confirm() {
    if (g_sub_sel == 4) {                 // Default: 重置音量（源码 _DAT_0062bdde = 0x5064）
        g_bgm_vol = 100; g_se_vol = 80;
        apply_option_volumes();
        play_sfx(7);
    } else if (g_sub_sel == 5) {          // Quit: 返回主菜单
        play_sfx(9);
        g_cancel_delay = 20;
        g_cancel_fade = 1.0f;
        g_cancel_slide = 0.0f;
        g_next_screen = SCR_TITLE;
    } else if (g_sub_sel == 3) {          // Key Config: 跳转按键配置页
        play_sfx(7);
        g_key_return_screen=SCR_OPTION;g_next_screen = SCR_KEYCONFIG;
        g_confirm_delay = 19;
        g_flash_phase = 30;
    }
}

// 按键动作名（源码 keyconfig.cpp 指针表 PTR_005a8640 / PTR_005a8660）
static const char* const KEY_ACTIONS[10] = {
    "Shot", "Charge", "Bomb", "Slow", "Pause", "Up", "Down", "Left", "Right", "Quit"
};
static const char* const KEY_NAMES[10] = {
    "Z", "C", "X", "Shift", "Esc", "Up", "Down", "Left", "Right", "Esc"
};

// Controller Select 屏项（源码 FUN_005231b0：1Player/2Player/KeyConfig ALL Reset/Quit）
static const char* const CONTROLLER_ITEMS[4] = {
    "1Player <Keyboard Full>", "2Player <Keyboard Full>", "KeyConfig ALL Reset", "Quit"
};

// Key Config 屏（源码 FUN_005231b0 Controller Select / FUN_00522590 按键绑定）
void draw_keyconfig() {
    static const float BOB[9] = {2.0f, -1.0f, 0.0f, 3.0f, -3.0f, 2.0f, -2.0f, 0.0f, 2.0f};
    if (g_key_mode == 0) {
        emit_opt_text("Controller Select", 640.0f - opt_text_width("Controller Select") * 0.5f, 200.0f, 0xffffffff, true);
        for (i32 i = 0; i < 4; ++i) {
            const float cy = 300.0f + i * 55.0f;
            const float bob = (i == g_key_sel && g_bob_phase >= 1 && g_bob_phase <= 9) ? BOB[g_bob_phase - 1] : 0.0f;
            const u32 main_tint = (i == g_key_sel) ? 0xffffff80 : 0xff808060;
            emit_opt_text(CONTROLLER_ITEMS[i], 300.0f + bob, cy + bob, main_tint, true);
        }
    } else {
        emit_opt_text("Key Config", 640.0f - opt_text_width("Key Config") * 0.5f, 200.0f, 0xffffffff, true);
        for (i32 i = 0; i < 10; ++i) {
            const float cy = 300.0f + i * 50.0f;
            const float bob = (i == g_key_sel && g_bob_phase >= 1 && g_bob_phase <= 9) ? BOB[g_bob_phase - 1] : 0.0f;
            const u32 main_tint = (i == g_key_sel) ? 0xffffff80 : 0xff808060;
            emit_opt_text(KEY_ACTIONS[i], 300.0f + bob, cy + bob, main_tint, true);
            emit_opt_text(KEY_NAMES[i], 640.0f + bob, cy + bob, main_tint, true);
        }
    }
}
static const char* const ACHIEVE_TITLE = "Achievements";
static const char* const MANUAL_TITLE  = "Manual";
static const char* const MUSIC_TITLE   = "Music Room";
static const char* const OPTION_TITLE  = "Option";

// 手动渲染一个 title.anm 图标（从全局 sprite 表查纹理 + sprite），居中于 (cx,cy)
void emit_icon(i32 sprite_id, float cx, float cy, u32 tint, i32 layer=35) {
    auto it = g_sprite_map.find(u32(sprite_id));
    if (it == g_sprite_map.end()) return;
    const i32 entry = it->second.first;
    const auto& s = g_title[entry].sprites[it->second.second];
    auto ti = g_title_tex.find(entry);
    if (ti == g_title_tex.end()) return;
    emit_quad(layer, ti->second, s, cx, cy, 1, 1, 0, 0, tint, 0);
}

// 绘制选中项的斜切平行四边形黑底框（实机选中项文字后方是一块半透明黑底，
// 向右下倾斜的平行四边形，左右边缘 alpha 渐变到透明，选中项不透明度更高）。
void emit_select_box(float cx, float cy, float w, float h, float angle_deg, u32 tint, i32 layer) {
    if (!g_box_tex.handle) return;
    DrawCmd c;
    c.layer = layer; c.handle = g_box_tex.handle;
    c.x = cx; c.y = cy;
    c.sx = w; c.sy = h; c.sw = 1.0f; c.sh = 1.0f;   // sw/sh=1，渲染尺寸=sx/sy（原 512×528 会贯穿屏幕）
    c.ax = 0; c.ay = 0;
    c.tint = tint;
    c.u0 = 0; c.v0 = 0; c.u1 = 1; c.v1 = 1;
    c.rot = angle_deg * 3.14159265f / 180.0f;   // 度→弧度，绕中心旋转（负=右上，正=右下）
    c.skew_x = 0; c.skew_y = 0; c.additive = false;
    g_draw.push_back(c);
}

// Story 难度选择：4 个难度图标竖排，选中难度滑到屏幕中央（脚本19 CASE 7-10 posTime
// 30帧缓动，实机中心距 240px）。
// 源码绘制（脚本 7-16 case 状态机 + FUN_005596a0）：
//   选中难度 = 左列 sprite 4-7（亮色正常态）alpha 255
//   未选中   = 右列 sprite 9-12（暗色版）alpha 192/128/64 按距离递减
//   选中瞬间右列闪烁 255→0（可省略）。没有黑框矩形。
void draw_story_difficulty() {
    const float CY = 368.0f;    // 选中难度图标中心（脚本7 case 7/13 y=368）
    const float DY = 240.0f;    // 实机相邻难度中心距（脚本 case 槽位差 240）
    // 根脚本19 CASE 7-10 整组 Y 偏移 -90/-30/+30/+90（选中 Easy→Lunatic 整组下移 60/档）
    const float group_y = -90.0f + 60.0f * g_story_cursor;
    for (i32 i = 0; i < 4; ++i) {
        float cy = CY + (i - g_story_cursor) * DY + group_y;
        cy += (CY - cy) * g_cancel_slide;   // 取消时向中心(640,368)聚拢
        const i32 d = std::abs(i - g_sub_sel);
        const i32 layer = d == 0 ? 35 : d == 1 ? 34 : d == 2 ? 30 : 29;
        if (i == g_sub_sel) {
            // Story 难度选择的原版信息底板要完整包住当前难度图标。
            // 它只属于 Story 难度列表，VS 底板由各自的 ANM 几何处理。
            const float boxw = 520.0f, boxh = 200.0f;
            const u32 box_tint = fade_tint(u32(0x60 * g_cancel_fade) << 24);
            emit_select_box(640.0f, cy, boxw, boxh, g_panel_tilt, box_tint, 34);
            // 选中：左列亮色 sprite，全 alpha（取消时淡出）；Z 确认时快速闪烁（FUN_005596a0 script 6/2）
            u32 sel_tint = u32(0xff * g_cancel_fade) << 24 | 0xffffff;
            if (g_flash_phase > 0 && (g_flash_phase & 3) < 2) sel_tint = (u32(0x30 * g_cancel_fade) << 24) | 0xffffff;
            emit_icon(4 + i, 640.0f, cy, fade_tint(sel_tint), layer);
        } else {
            // 未选中：右列暗色 sprite，alpha 192/128/64（取消时淡出）
            static const u32 A[4] = {0, 192, 128, 64};
            emit_icon(9 + i, 640.0f, cy, fade_tint((u32(A[d] * g_cancel_fade) << 24) | 0xffffff), layer);
        }
    }
}

void draw_text(const std::string& utf8, float px, float cx, float cy, u32 tint, i32 layer, u8 anchor);
// VS menu sprites 102-106. Rule settings are a separate native task.
void draw_vs_menu() {
    const float CY = 480.0f;    // 根脚本 190 pos y
    const float DY = 140.0f;    // 脚本 180-184 case 槽位差 140
    for (i32 i = 0; i < 5; ++i) {
        float cy = CY + (i - g_vs_cursor) * DY;
        cy += (CY - cy) * g_cancel_slide;   // 取消时向中心聚拢
        const i32 d = std::abs(i - g_sub_sel);
        const i32 layer = d == 0 ? 35 : d == 1 ? 34 : d == 2 ? 30 : 29;
        if (i == g_sub_sel) {
            emit_icon(102 + i, 640.0f, cy, fade_tint(u32(0xff * g_cancel_fade) << 24 | 0xffffff), layer);
        } else {
            static const u32 A[5] = {0, 192, 128, 64, 48};
            emit_icon(107 + i, 640.0f, cy, fade_tint((u32(A[d] * g_cancel_fade) << 24) | 0xffffff), layer);
        }
    }

}

// 子界面文字列表：主菜单文字用 640×480 逻辑坐标（渲染 ×2），所以这里坐标
// 直接给 1280 空间值（640 逻辑 ×2）。FUN_0055a870 主菜单 X=32 Y=250 行距25 →
// 屏幕 64/500/50；角色选择 X=211 Y=150 行34 列38。
void collect_help_vm(const th19::AnmVm& vm, bool selected) {
    if (!vm.visible || vm.sprite < 0) return;
    auto it = g_help_sprites.find(u32(vm.sprite));
    if (it == g_help_sprites.end()) return;
    const i32 entry = it->second.first;
    const auto& sp = g_help[entry].sprites[it->second.second];
    auto ti = g_help_tex.find(entry);
    if (ti == g_help_tex.end()) return;
    u32 a = u32(vm.alpha > 255 ? 255 : vm.alpha);
    a = u32(a * g_alpha_scale);
    const u32 tint = selected ? ((a << 24) | 0xffffff) : ((a << 24) | 0x808080);
    emit_quad(vm.layer, ti->second, sp, vm.x, vm.y, vm.sx, vm.sy,
              vm.anchor_x, vm.anchor_y, tint, vm.rot_z);
    for (const auto& c : vm.children) collect_help_vm(c, selected);
}

void draw_help() {
    if (g_help_mode == 0) {
        for (i32 i = 0; i < (i32)g_help_vms.size(); ++i)
            collect_help_vm(g_help_vms[i], i == g_help_sel);
    } else {
        auto ti = g_help_page_tex.find(g_help_page);
        if (ti != g_help_page_tex.end()) {
            // 详情页内容 sprite 13 = 768×896（位于 1024×1024 纹理左上角 0,0），layer 45
            // 屏幕 1280×960，页面居中：左缘 (1280-768)/2=256、上缘 (960-896)/2=32，即中心 (640,480)。
            // 进入淡入(alpha 0→255)；下一页向上滑出；上一页/返回向下滑出。
            const float PW = 768.0f, PH = 896.0f;
            float py = 480.0f;
            u32 alpha = 255;
            const float t = std::min(1.0f, float(g_help_anim_frame) / 20.0f);
            if (g_help_anim == 1) {                 // 淡入
                alpha = u32(255.0f * t);
            } else if (g_help_anim == 2) {          // 向上滑出（下一页）
                py = 480.0f - 304.0f * t;
                alpha = u32(255.0f * (1.0f - t));
            } else if (g_help_anim == 3) {          // 向下滑出（上一页/返回）
                py = 480.0f + 208.0f * t;
                alpha = u32(255.0f * (1.0f - t));
            }
            th19::AnmSprite sp;
            sp.id = 13; sp.x = 0; sp.y = 0; sp.w = PW; sp.h = PH;
            emit_quad(45, ti->second, sp, 640.0f, py, 1, 1, 0, 0, (alpha << 24) | 0xffffff, 0.0f);
        }
    }
}

// 555200: title.anm210..231 list and text.anm30..37 comment VMs.
th19::AnmVm g_music_rows[22],g_music_lines[8];
int g_music_age=0,g_music_reveal_age=0,g_music_revealed=0;
bool g_music_exiting=false,g_music_warning_shown=false;
void select_music_animation(){
 for(int i=0;i<22;++i){auto&v=g_music_rows[i];if(!v.stream)continue;
  v.interrupt(i==g_sub_sel?2:3);
  const float x=128-(i==g_sub_sel?8:0),y=192+(i-g_music_scroll)*40;
  v.ip_pos={true,4,0,0,v.x,v.y,v.z,x,y,0};
 }
}
void reveal_music_comment(){
 g_music_revealed=0;g_music_reveal_age=0;g_music_warning_shown=g_music_prompt!=0;
 for(auto&v:g_music_lines)v.interrupt(3);
}
void init_music_animation(){
 g_music_age=0;g_music_reveal_age=0;g_music_revealed=0;g_music_exiting=false;g_music_warning_shown=false;
 g_music_prompt=0;g_music_scroll=0;g_music_comment_track=0;
 for(auto&v:g_music_rows)v={};
 for(int i=0;i<8;++i){auto&v=g_music_lines[i];v={};auto it=g_story_text_scripts.find(30+i);
  if(it!=g_story_text_scripts.end()){v.bind_entry(it->second.stream,it->second.len,&g_story_text_scripts);v.tick();v.alpha=0;}}
}
void close_music_animation(){g_music_exiting=true;for(auto&v:g_music_rows)v.interrupt(1);for(auto&v:g_music_lines)v.interrupt(1);}
void tick_music_animation(){
 ++g_music_age;
 // Two list entries are constructed each frame in native state0.
 if(!g_music_exiting&&g_music_age<=11)for(int i=(g_music_age-1)*2;i<std::min(22,g_music_age*2);++i){
  auto&v=g_music_rows[i];bind_script(210+i,v);v.tick();v.interrupt(i==g_sub_sel?2:3);
  v.x=128-(i==g_sub_sel?8:0);v.y=192+(i-g_music_scroll)*40;
 }
 if(!g_music_exiting&&g_music_age>15){
  ++g_music_reveal_age;
  if(g_music_reveal_age%2==0&&g_music_revealed<8)g_music_lines[g_music_revealed++].interrupt(2);
 }
 for(auto&v:g_music_rows)if(v.stream)v.tick();for(auto&v:g_music_lines)if(v.stream)v.tick();
}
void draw_music(){
 for(int i=0;i<22;++i){const auto&v=g_music_rows[i];
  if(!v.stream||!v.visible||i<g_music_scroll||i>=g_music_scroll+10)continue;
  const std::string title=g_music_unlocked[i]?MUSIC_NAMES[i]:MUSIC_LOCKED[i];
  // 4cd610 font0: 16.5 truncated to16; rasterized at double resolution.
  draw_text(title,32,v.x,v.y,v.color(),36,1);
 }
 for(int i=0;i<g_music_revealed;++i){const auto&v=g_music_lines[i];if(!v.visible)continue;
  const char*line=g_music_warning_shown?MUSIC_LOCK_WARNING[i]:MUSIC_COMMENTS[g_music_comment_track][i];
  // Native COLORREF 008080ff is RGB ff8080, not 8080ff.
  draw_text(line,32,v.x,v.y,(u32(v.alpha)<<24)|(g_music_warning_shown?0xff8080:0xffffff),36,1);
 }
}

// Achievement 成就屏（源码 FUN_00554850 逆向）：
//   - 6 列网格，23 成就（6×4 缺一格）；锁定显示"？？？"占位
//   - 详情区（脚本 237-240）：大字称号 + 3 行说明（取得/未取得）
// ---- MS Gothic font rendering (stb_truetype) ----
std::vector<u8> g_font_bytes;
stbtt_fontinfo g_font;
bool g_font_ok = false;
struct FontTex { TextureAllocation tex; float w, h; };
std::map<std::pair<std::string,float>, FontTex> g_font_cache;

static void utf8_decode(const std::string& str, std::vector<i32>& out) {
    out.clear();
    const size_t n = str.size();
    for (size_t i = 0; i < n; ) {
        unsigned char c = (unsigned char)str[i];
        i32 cp;
        if (c < 0x80) { cp = c; i += 1; }
        else if ((c & 0xE0) == 0xC0) { cp = ((c & 0x1F) << 6) | ((unsigned char)str[i+1] & 0x3F); i += 2; }
        else if ((c & 0xF0) == 0xE0) { cp = ((c & 0x0F) << 12) | (((unsigned char)str[i+1] & 0x3F) << 6) | ((unsigned char)str[i+2] & 0x3F); i += 3; }
        else if ((c & 0xF8) == 0xF0) { cp = ((c & 0x07) << 18) | (((unsigned char)str[i+1] & 0x3F) << 12) | (((unsigned char)str[i+2] & 0x3F) << 6) | ((unsigned char)str[i+3] & 0x3F); i += 4; }
        else { cp = (i32)'?'; i += 1; }
        out.push_back(cp);
    }
}

bool render_text(const std::string& utf8, float px, FontTex& out, bool plain=false) {
    auto key = std::make_pair(utf8, plain?-px:px);
    auto it = g_font_cache.find(key);
    if (it != g_font_cache.end()) { out = it->second; return out.tex.handle != 0; }
    out = FontTex{};
    if (!g_font_ok || utf8.empty()) return false;
    std::vector<i32> cps;
    utf8_decode(utf8, cps);
    if (cps.empty()) return false;
    const float scale = stbtt_ScaleForPixelHeight(&g_font, px);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&g_font, &ascent, &descent, &lineGap);
    float total_w = 0;
    for (int cp : cps) { int adv, lsb; stbtt_GetCodepointHMetrics(&g_font, cp, &adv, &lsb); total_w += adv * scale; }
    const int OUTLINE = plain?0:2;   // 黑边描边宽度（像素）
    int W = (int)std::ceil(total_w) + OUTLINE * 4;
    int H = (int)std::ceil((ascent - descent + lineGap) * scale) + OUTLINE * 4;
    if (W < 1) W = 1; if (H < 1) H = 1;
    std::vector<u8> rgba((size_t)W * H * 4, 0);
    float x = float(OUTLINE * 2);
    for (int cp : cps) {
        int adv, lsb; stbtt_GetCodepointHMetrics(&g_font, cp, &adv, &lsb);
        int cw, ch, xoff, yoff;
        unsigned char* bmp = stbtt_GetCodepointBitmap(&g_font, scale, scale, cp, &cw, &ch, &xoff, &yoff);
        if (bmp) {
            const int bx = (int)(x + xoff), by = (int)(ascent * scale + yoff);
            // 黑边描边：在半径 OUTLINE 的 Chebyshev 盘内 stamp 黑色（取最大 alpha）
            for (int dy = -OUTLINE; dy <= OUTLINE; ++dy) for (int dx = -OUTLINE; dx <= OUTLINE; ++dx) {
                if (dx == 0 && dy == 0) continue;
                for (int j = 0; j < ch; ++j) for (int i = 0; i < cw; ++i) {
                    u8 a = bmp[j * cw + i]; if (!a) continue;
                    int px0 = bx + i + dx, py0 = by + j + dy;
                    if (px0 >= 0 && px0 < W && py0 >= 0 && py0 < H) {
                        size_t o = ((size_t)py0 * W + px0) * 4;
                        if (a > rgba[o+3]) { rgba[o] = 0; rgba[o+1] = 0; rgba[o+2] = 0; rgba[o+3] = a; }
                    }
                }
            }
            // 前景：白色字形居中 stamp
            for (int j = 0; j < ch; ++j) for (int i = 0; i < cw; ++i) {
                u8 a = bmp[j * cw + i]; if (!a) continue;
                int px0 = bx + i, py0 = by + j;
                if (px0 >= 0 && px0 < W && py0 >= 0 && py0 < H) {
                    size_t o = ((size_t)py0 * W + px0) * 4;
                    rgba[o] = 255; rgba[o+1] = 255; rgba[o+2] = 255; rgba[o+3] = a;
                }
            }
            stbtt_FreeBitmap(bmp, nullptr);
        }
        x += adv * scale;
    }
    out.tex = gfx.create_raw(W, H, rgba.data());
    out.w = (float)W; out.h = (float)H;
    g_font_cache[key] = out;
    return out.tex.handle != 0;
}

// anchor: 0=center, 1=top-left
void draw_text(const std::string& utf8, float px, float cx, float cy, u32 tint, i32 layer, u8 anchor = 0) {
    FontTex ft;
    if (!render_text(utf8, px, ft)) return;
    th19::AnmSprite sp; sp.id = 0; sp.x = 0; sp.y = 0; sp.w = ft.w; sp.h = ft.h;
    emit_quad(layer, ft.tex, sp, cx, cy, 1, 1, anchor, anchor, tint, 0);
}


// NoticeInf 536110/55a7c0: original ANM assets, LIFO notifications.
int g_notice_id=-1,g_notice_age=0,g_notice_close=-1;
std::vector<u8> g_notice_bytes;std::vector<th19::AnmEntry> g_notice_entries;
std::map<i32,TextureAllocation> g_notice_tex;
std::unordered_map<i32,th19::AnmScriptLoc> g_notice_scripts;
th19::AnmVm g_notice_panel,g_notice_text;
bool tick_route_notice(bool confirm){
 if(g_screen!=SCR_TITLE||g_trans!=TRANS_NONE||g_phase!=PHASE_TITLE)return false;
 if(g_notice_id<0){
  if(g_pending_route_notices.empty())return false;
  if(g_notice_entries.empty()){
   if(!load_anm("/notice.anm",g_notice_bytes,g_notice_entries))return false;
   decode_textures(g_notice_entries,g_notice_bytes,g_notice_tex);
   for(const auto&e:g_notice_entries)for(const auto&sc:e.scripts)g_notice_scripts[sc.id]={g_notice_bytes.data()+e.base+sc.offset,u32(g_notice_bytes.size()-e.base-sc.offset),&e.scripts};
  }
  if(!g_notice_scripts.count(0)||!g_notice_scripts.count(4))return false;
  g_notice_id=g_pending_route_notices.back();g_notice_age=0;g_notice_close=-1;
  auto bind=[&](th19::AnmVm&v,int id){v={};const auto&sc=g_notice_scripts.at(id);v.bind_entry(sc.stream,sc.len,&g_notice_scripts);};
  bind(g_notice_panel,0);bind(g_notice_text,4);play_sfx(83);
 }
 if(g_notice_close>=0){
  if(++g_notice_close>=30){g_pending_route_notices.pop_back();save_notice_progress();g_notice_id=-1;return true;}
 }else if(g_notice_age>19&&confirm){g_notice_close=0;g_notice_panel.interrupt_all(1);g_notice_text.interrupt_all(1);play_sfx(7);}
 g_notice_panel.tick();g_notice_text.tick();++g_notice_age;return true;
}
void draw_route_notice(){
 if(g_notice_id<0||g_screen!=SCR_TITLE)return;
 g_alpha_scale=1;
 auto draw=[&](auto&&self,const th19::AnmVm&v)->void{
  if(!v.visible)return;
  if(v.geometry==603){th19::AnmSprite sp;sp.w=sp.h=1;emit_quad(200,g_white_tex,sp,v.x,v.y,v.geometry_width,v.geometry_height,v.anchor_x,v.anchor_y,v.color(),0);}
  else if(v.sprite>=0&&v.sprite<2&&!g_notice_entries.empty()){
   auto t=g_notice_tex.find(0);if(t!=g_notice_tex.end())emit_quad(200,t->second,g_notice_entries[0].sprites[v.sprite],v.x,v.y,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z);
  }
  for(const auto&c:v.children)self(self,c);
 };draw(draw,g_notice_panel);
 if(g_notice_text.visible){
  const auto&v=g_notice_text;u32 tint=(u32(v.alpha)<<24)|0x80d0ff;
  // Native dynamic texture sprite2: 1024x256, text rows at y56 and128.
  for(int row=0;row<2;++row){FontTex ft;if(render_text(NATIVE_NOTICE_TEXT[g_notice_id][row],48,ft))draw_text(NATIVE_NOTICE_TEXT[g_notice_id][row],48,v.x+(1024-ft.w)*.5f,v.y-128+56+72*row,tint,201,1);}
 }
}


int g_trophy_popup_id=-1,g_trophy_popup_age=0;
std::vector<u8> g_trophy_popup_bytes;std::vector<th19::AnmEntry> g_trophy_popup_entries;
std::map<i32,TextureAllocation> g_trophy_popup_tex;
std::unordered_map<i32,th19::AnmScriptLoc> g_trophy_popup_scripts;
th19::AnmVm g_trophy_popup_vm;
void tick_trophy_popup(){
 if(g_trophy_popup_id>=0&&g_trophy_popup_age>=240)g_trophy_popup_id=-1;
 if(g_trophy_popup_id<0){
  if(g_trophy_popup_queue.empty())return;
  if(g_trophy_popup_entries.empty()){
   if(!load_anm("/trophy.anm",g_trophy_popup_bytes,g_trophy_popup_entries))return;
   decode_textures(g_trophy_popup_entries,g_trophy_popup_bytes,g_trophy_popup_tex);
   for(const auto&e:g_trophy_popup_entries)for(const auto&sc:e.scripts)g_trophy_popup_scripts[sc.id]={g_trophy_popup_bytes.data()+e.base+sc.offset,u32(g_trophy_popup_bytes.size()-e.base-sc.offset),&e.scripts};
  }
  auto it=g_trophy_popup_scripts.find(0);if(it==g_trophy_popup_scripts.end())return;
  g_trophy_popup_id=g_trophy_popup_queue.front();g_trophy_popup_queue.erase(g_trophy_popup_queue.begin());g_trophy_popup_age=0;
  g_trophy_popup_vm={};g_trophy_popup_vm.bind_entry(it->second.stream,it->second.len,&g_trophy_popup_scripts);play_sfx(79);
 }
 g_trophy_popup_vm.tick();++g_trophy_popup_age;
}
void draw_trophy_popup(){
 if(g_trophy_popup_id<0)return;g_alpha_scale=1;
 auto draw=[&](auto&&self,const th19::AnmVm&v,float px,float py)->void{
  float x=px+v.x+v.child_x,y=py+v.y+v.child_y;
  if(v.visible&&v.sprite>=0&&v.sprite<3){auto t=g_trophy_popup_tex.find(0);if(t!=g_trophy_popup_tex.end())emit_quad(1100,t->second,g_trophy_popup_entries[0].sprites[v.sprite],x,y,v.sx,v.sy*std::cos(v.rot_x),v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1);}
  if(v.visible&&v.sprite==5){
   std::string title=std::string("『")+TROPHY_TEXT[g_trophy_popup_id].title+"』";
   const std::string rows[2]={title,"の実績を手に入れた"};
   for(int row=0;row<2;++row){FontTex ft;if(render_text(rows[row],24,ft))draw_text(rows[row],24,x-ft.w*.5f,y-27+28*row,(u32(v.alpha)<<24)|0xc0c080,1101,1);}
  }
  // Native children outlive the root emitter's delete instruction.
  for(const auto&c:v.children)self(self,c,x,y);
 };draw(draw,g_trophy_popup_vm,0,0);
}

static const char* LOCKED_DETAIL = "？？？？？？？？？？？？"; // 原版 DAT_005a18ec（详情标题）

// Proportional menu lettering for the card acquisition banner.
void draw_ascii_centered(const char* s, float cx, float cy, u32 main_tint, u32 shadow_tint, float scale = 1.0f) {
    const i32 FONT_MAIN = 3, FONT_SHADOW = 4;
    auto mi = g_ascii_tex.find(FONT_MAIN), si = g_ascii_tex.find(FONT_SHADOW);
    if (mi == g_ascii_tex.end() || si == g_ascii_tex.end()) return;
    float total_w = 0.0f;
    for (const char* p = s; *p; ++p) {
        i32 idx = i32(u8(*p)) - 0x20;
        if (idx >= 0 && idx < (i32)g_ascii[FONT_MAIN].sprites.size()) total_w += g_ascii[FONT_MAIN].sprites[idx].w * scale;
        else total_w += 14 * scale;
    }
    float px = cx - total_w * 0.5f;
    for (const char* p = s; *p; ++p) {
        i32 idx = i32(u8(*p)) - 0x20;
        if (idx < 0 || idx >= (i32)g_ascii[FONT_MAIN].sprites.size()) { px += 14 * scale; continue; }
        const auto& ms = g_ascii[FONT_MAIN].sprites[idx];
        const float gx = px + ms.w * 0.5f * scale, gy = cy;
        const auto& ss = g_ascii[FONT_SHADOW].sprites[idx];
        emit_quad(29, si->second, ss, gx - 1, gy, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, si->second, ss, gx + 1, gy, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, si->second, ss, gx, gy - 1, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, si->second, ss, gx, gy + 1, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, si->second, ss, gx - 1, gy - 1, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, si->second, ss, gx + 1, gy - 1, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, si->second, ss, gx - 1, gy + 1, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, si->second, ss, gx + 1, gy + 1, scale, scale, 0, 0, shadow_tint, 0);
        emit_quad(29, mi->second, ms, gx, gy, scale, scale, 0, 0, main_tint, 0);
        px += ms.w * scale;
    }
}

// ---- ending background + sprite (lazy per ending id) ----
void load_ending_anm() {
    g_end_loaded_id = g_ending_id;
    g_end_bg_tex = {}; g_end_chr_tex = {};
    char path[64];
    std::vector<u8> bytes; std::vector<th19::AnmEntry> entries;
    std::snprintf(path, sizeof path, "/%s", END_BG[g_ending_id]);
    if (load_anm(path, bytes, entries)) {
        for (auto& e : entries) if (e.thtx_size && e.data_offset + e.thtx_size <= bytes.size()) {
            auto alloc = gfx.create_texture(bytes.data() + e.data_offset, e.thtx_size);
            if (alloc.handle) { g_end_bg_tex = alloc; break; }
        }
    }
    std::snprintf(path, sizeof path, "/%s", END_SPRITE[g_ending_id]);
    if (load_anm(path, bytes, entries)) {
        for (auto& e : entries) if (e.thtx_size && e.data_offset + e.thtx_size <= bytes.size()) {
            auto alloc = gfx.create_texture(bytes.data() + e.data_offset, e.thtx_size);
            if (alloc.handle) { g_end_chr_tex = alloc; break; }
        }
    }
}

void draw_achievement() {
    const i32 total = 23, cols = 6;
    // 原版 FUN_00554700 布局（640 逻辑空间 ×2 = 1280）：
    //   grid_x0=DAT_005aa3e8=211, grid_y0=DAT_005aa3b8=150,
    //   cell_w=_DAT_005aa340=38, cell_h=_DAT_005aa334=34
    const float cell_w = 76.0f, cell_h = 68.0f;
    const float grid_x0 = 422.0f, grid_y0 = 300.0f;
    const auto atlas = g_ascii_tex.find(0);
    for (i32 i = 0; !g_achievement_exiting && atlas != g_ascii_tex.end() && i < total; ++i) {
        const float x = grid_x0 + (i % cols) * cell_w;
        const float y = grid_y0 + (i / cols) * cell_h;
        const u32 tint = (i == g_sub_sel) ? 0xffffffff : 0xff707070;
        char label[8] = "--";
        if (g_trophy_unlocked[i]) std::snprintf(label, sizeof label, "%2d", i + 1);

        // 004e5850 font 4: '0' starts at sprite 239, so '-' selects cross sprite 236.
        // Each slot advances 12 logical pixels, including the leading space in "%2d".
        for (int slot = 0; label[slot]; ++slot) {
            if (label[slot] == ' ') continue;
            const auto& glyph = g_ascii[0].sprites[239 + label[slot] - '0'];
            const float gx = x + slot * 24.0f;
            emit_quad(29, atlas->second, glyph, gx + 4, y + 4, 1, 1, 1, 1, 0x7f000000, 0);
            emit_quad(29, atlas->second, glyph, gx, y, 1, 1, 1, 1, tint, 0);
        }
    }
    const i32 sel = g_sub_sel;
    const bool unlocked = g_trophy_unlocked[sel] != 0;
    const char* title = unlocked ? TROPHY_TEXT[sel].title : LOCKED_DETAIL;
    // 称号 + 介绍：同字号（原版 FUN_004cd2e0 字体表 index 4 = 20.5 → GDI 高 20 逻辑×2 = 40px），黑描边加粗
    for(int row=0;row<4;++row){const auto&v=g_achievement_details[row];if(!v.visible||!v.alpha)continue;
        const char*line=row==0?title:unlocked?TROPHY_TEXT[sel].unlocked[row-1]:TROPHY_TEXT[sel].locked[row-1];
        draw_text(line,40,v.x,v.y,v.color(),36,0);
    }
}

// 结局台词颜色：endXX.msg 的 0x09 字段（RGB 字节序，第1字节=R）
// 结局台词颜色：直接用 endXX.msg 0x09 状态字段逐行取色（END_LINE_COLORS，字节序 RGB）
// 不再按角色名关键词匹配——旁白行里出现角色名（如“霊夢は溜め息を吐いた”）会误判。

// Original staff.msg scheduling and native506b70 slot replacement.
void start_staff(){
    g_screen=SCR_STAFF;g_trans=TRANS_NONE;g_fade=1;g_staff_frame=0;g_staff_event=0;g_staff_fade=-1;
    for(auto&v:g_staff_slots)v={};
    if(g_staff_entries.empty()&&load_anm("/staff.anm",g_staff_bytes,g_staff_entries)){
        decode_textures(g_staff_entries,g_staff_bytes,g_staff_tex);
        for(int i=0;i<int(g_staff_entries.size());++i){const auto&e=g_staff_entries[i];
            for(int j=0;j<int(e.sprites.size());++j)g_staff_sprites[e.sprites[j].id]={i,j};
            for(const auto&sc:e.scripts)g_staff_scripts[sc.id]={g_staff_bytes.data()+e.base+sc.offset,u32(g_staff_bytes.size()-e.base-sc.offset),&e.scripts};}
    }
}
void tick_staff(bool confirm){
    while(g_staff_event<std::size(STAFF_EVENTS)&&STAFF_EVENTS[g_staff_event].frame<=g_staff_frame){
        const auto&e=STAFF_EVENTS[g_staff_event++];auto it=g_staff_scripts.find(e.script);
        if(it!=g_staff_scripts.end()){auto&v=g_staff_slots[e.slot];v={};v.bind_entry(it->second.stream,it->second.len,&g_staff_scripts);}}
    if(g_staff_frame==80)play_music_track(21);
    for(auto&v:g_staff_slots)if(v.stream)v.tick();
    if(g_staff_frame<2760)++g_staff_frame;
    else if(g_staff_fade<0){if(confirm)g_staff_fade=0;}
    else if(++g_staff_fade>=60){g_next_screen=g_ending_return_screen;g_trans=TRANS_OUT;g_trans_frame=0;}
}
void draw_staff(){
    auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y)->void{
        if(!v.visible)return;x+=v.x+v.child_x;y+=v.y+v.child_y;
        auto sp=g_staff_sprites.find(v.sprite);if(sp!=g_staff_sprites.end()){auto tx=g_staff_tex.find(sp->second.first);
            if(tx!=g_staff_tex.end())emit_quad(50+v.layer,tx->second,g_staff_entries[sp->second.first].sprites[sp->second.second],x,y,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1);}
        for(const auto&c:v.children)self(self,c,x,y);
    };for(const auto&v:g_staff_slots)draw(draw,v,0,0);
    if(g_staff_fade>=0){th19::AnmSprite black;black.w=black.h=1;emit_quad(1000,g_white_tex,black,0,0,1280,960,1,1,u32(std::min(255,g_staff_fade*255/60))<<24,0);}
}

void draw_ending() {
    if (g_end_loaded_id != g_ending_id) load_ending_anm();
    const i32 base = END_LINE_OFFSET[g_ending_id];
    const i32 cnt  = END_LINE_OFFSET[g_ending_id + 1] - base;

    // 空行判定：整行全为全角空格 U+3000（UTF-8 E3 80 80）→ 空行/分块
    auto is_blank = [](const char* t) {
        while ((unsigned char)t[0] == 0xE3 && (unsigned char)t[1] == 0x80 && (unsigned char)t[2] == 0x80) t += 3;
        return t[0] == 0;
    };
    // 颜色分类：0=旁白 gray #C0C0D0，1=角色，2=结局标题 green #60F000
    auto cat_of = [](u32 c) {
        if (c == 0xffc0c0d0) return 0;
        if (c == 0xff60f000) return 2;
        return 1;
    };
    // 立绘出现点：第一条角色（非旁白）台词——原版旁白说完立绘就出现
    i32 char_idx = cnt;
    for (i32 i = 0; i < cnt; ++i) if (cat_of(END_LINE_COLORS[base + i]) == 1) { char_idx = i; break; }
    const bool in_char = (g_ending_line >= char_idx);

    // 图片：旁白阶段显示 ebg 背景，角色阶段淡入 end 立绘（原版 0x07 加载 ebgXX.anm + endXX.anm 两张）
    if (in_char) {
        if (g_ending_cg_frame < 0) g_ending_cg_frame = g_frame;
        i32 a = (g_frame - g_ending_cg_frame) * 255 / 20; if (a < 0) a = 0; if (a > 255) a = 255;
        if (g_end_chr_tex.handle) {
            th19::AnmSprite sp; sp.id = 0; sp.x = 0; sp.y = 0;
            sp.w = (float)g_end_chr_tex.width; sp.h = (float)g_end_chr_tex.height;
            emit_quad(51, g_end_chr_tex, sp, SCREEN_W * 0.5f, SCREEN_H * 0.5f, 1, 1, 0, 0, 0x00ffffff | (a << 24), 0);
        }
    } else if (g_end_bg_tex.handle) {
        th19::AnmSprite sp; sp.id = 0; sp.x = 0; sp.y = 0;
        sp.w = (float)g_end_bg_tex.width; sp.h = (float)g_end_bg_tex.height;
        emit_quad(50, g_end_bg_tex, sp, SCREEN_W * 0.5f, SCREEN_H * 0.5f, 1, 1, 0, 0, 0xffffffff, 0);
    }

    // 台词：按颜色分类分块（旁白/角色/标题），块间清空；块内每满 4 行清空重新开始（句子没说完是例外，跨页不拆句）
    // 句子结束判定：以 。！？!?」』… 等句末标点结尾
    auto ends_sentence = [](const char* t) {
        size_t n = 0; while (t[n]) ++n;
        while (n > 0 && (t[n-1] == ' ' || (n >= 3 && (unsigned char)t[n-3] == 0xE3 && (unsigned char)t[n-2] == 0x80 && (unsigned char)t[n-1] == 0x80))) {
            if (t[n-1] == ' ') --n; else n -= 3;
        }
        if (n < 1) return true;
        if (t[n-1] == '!' || t[n-1] == '?') return true;
        if (n < 3) return false;
        const unsigned char a = t[n-3], b = t[n-2], c = t[n-1];
        if (a == 0xE3 && b == 0x80 && (c == 0x82 || c == 0x8D || c == 0x8F)) return true;   // 。」』
        if (a == 0xE2 && b == 0x80 && c == 0xA6) return true;                              // …
        if (a == 0xEF && b == 0xBC && (c == 0x81 || c == 0x9F)) return true;                // ！？
        return false;
    };
    const float rowH = 40.0f;
    const float cx_left = 128.0f;   // 原版 text.anm 脚本 0x56..0x5a：x=128，锚点(1,1)=左上
    const float y0 = 752.0f;        // 首行 y=752，行距 40（实测原版截图墨迹 x=132/y=753 起）
    i32 win_start = END_GROUPS[g_ending_group].page;
    i32 row = 0;
    for (i32 i = win_start; i <= g_ending_line && i < cnt; ++i) {
        const char* txt = END_LINES[base + i];
        if (is_blank(txt)) continue;
        u32 tint = END_LINE_COLORS[base + i];
        if (i == g_ending_line) {   // 最新一行淡入
            i32 a = (g_frame - g_ending_reveal_frame) * 255 / 10; if (a < 0) a = 0; if (a > 255) a = 255;
            tint = (tint & 0x00ffffff) | (a << 24);
        }
        draw_text(txt, 30.0f, cx_left, y0 + float(row) * rowH, tint, 100, 1);
        ++row;
    }
    if(g_ending_tail>=60){th19::AnmSprite black;black.w=black.h=1;emit_quad(1000,g_white_tex,black,0,0,1280,960,1,1,u32(std::min(255,(g_ending_tail-60)*255/60))<<24,0);}

}


// 角色选择页（原版布局：左=选中大立绘+信息，中=19 图标网格，右=Clear+剧情）
// 网格 7 行：2/3/3/3/3/3/2（第一行和最后一行 2 个，中间 5 行各 3 个），Reimu 在顶
// 图标 chara00~chara18 = title.anm entry15~33（128×146）；大立绘 face_plXXbs = entry34+i
void draw_charsel() {
    // 切换角色滑入动画：立绘 10 帧 linear、称号/介绍 16 帧 ease-in（原版 face/name/story 脚本 case7）
    const i32 sw = g_char_switch_frame;
    const float face_t = sw < 10 ? float(sw) / 10.0f : 1.0f;        // 立绘 linear
    const float inf_t = sw < 16 ? float(sw) / 16.0f : 1.0f;         // 称号/介绍
    const float inf_e = inf_t * inf_t;                              // ease-in x²（原版 m1）

    // 左：选中角色大立绘 face_plXXbs（原版 script#46 case7：pos(-320,960)→(0,960) 10帧 + 淡入，锚点左下(1,2)，layer29）
    if(!g_vs_mode){
        const i32 fentry = 34 + g_char_sel;
        auto fit = g_title_tex.find(fentry);
        if (fit != g_title_tex.end() && !g_title[fentry].sprites.empty()) {
            const auto& s = g_title[fentry].sprites[0];
            th19::AnmSprite sp; sp.id = 0; sp.x = s.x; sp.y = s.y; sp.w = s.w; sp.h = s.h;
            const float fx = -320.0f + 320.0f * face_t;             // 从左外滑入
            const u32 a = u32(255.0f * face_t);                     // 淡入
            emit_quad(29, fit->second, sp, fx, 960.0f, 1.0f, 1.0f, 1, 2, (a << 24) | 0xffffff, 0);
        }
    }

    if(g_vs_mode){for(const auto&vm:g_vs_portrait_vm)collect_vm(vm);
 for(int side=0;side<2;++side)for(const auto&v:g_vs_card_vms[side])if(v.visible&&v.alpha>0)for(int ei=0;ei<int(g_abcard.size());++ei)for(const auto&sp:g_abcard[ei].sprites)if(int(sp.id)==v.sprite){auto tx=g_abcard_tex.find(ei);if(tx!=g_abcard_tex.end())emit_quad(v.layer,tx->second,sp,(side?1020.f:280.f)+v.x,810+v.y,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z);}
 }

    // 4e0520: font12, colour ffffff80, centered axes, -14 degrees.
    // 59837c format "%s + <= =>"; keyboard slow binding resolves to Shift.
    if(g_vs_mode&&g_vs_rules[2]>=2){
        auto glyph=[&](unsigned char c)->std::pair<int,const th19::AnmSprite*>{
            for(int ei=0;ei<int(g_ascii.size());++ei)for(const auto&sp:g_ascii[ei].sprites)
                if(int(sp.id)==0x2a8+c)return {ei,&sp};
            return {-1,nullptr};
        };
        const std::string hint="Shift + <= =>";float width=0;
        for(unsigned char c:hint){auto g=glyph(c);if(g.second)width+=g.second->w;}
        const float angle=-14.f*3.141592654f/180.f,cs=std::cos(angle),sn=std::sin(angle);
        for(int side=0;side<2;++side)if(!g_vs_card_vms[side].empty()){
            float x=-width*.5f;const float cx=(side?1020.f:280.f)+24,cy=890;
            for(unsigned char c:hint){auto g=glyph(c);if(!g.second)continue;
                auto tx=g_ascii_tex.find(g.first);const float y=-g.second->h*.5f;
                if(tx!=g_ascii_tex.end())emit_quad(35,tx->second,*g.second,cx+x*cs-y*sn,cy+x*sn+y*cs,1,1,1,1,0xffffff80,angle);
                x+=g.second->w;
            }
        }
    }


    // 顶部标题条 sl_player（原版 script#209：sprite123 512×128，锚点下中(0,2)，从 y0 滑到 y128，layer30）
    {
        const i32 tentry = 95;
        auto tit = g_title_tex.find(tentry);
        if (tit != g_title_tex.end() && !g_title[tentry].sprites.empty()) {
            const auto& ts = g_title[tentry].sprites[0];
            th19::AnmSprite sp; sp.id = 0; sp.x = ts.x; sp.y = ts.y; sp.w = ts.w; sp.h = ts.h;
            float ty = 128.0f;
            if (g_charsel_frame < 16) ty = 128.0f * float(g_charsel_frame) / 16.0f;
            emit_quad(30, tit->second, sp, 640.0f, ty, 1.0f, 1.0f, 0, 2, 0xffffffff, 0);
        }
    }

    // 六边形图标网格（layer30）：逐行延迟 + 绕Y轴翻转展开(scaleX 0→1, 30帧) + 快速淡入（原版 chara 脚本）
    for (i32 i = 0; i < 19; ++i) {
        const i32 entry = character_available(i)?15+i:13; // title.anm scripts27..45 interrupt2 -> sprite23/chara99.png
        auto it = g_title_tex.find(entry);
        if (it == g_title_tex.end() || g_title[entry].sprites.empty()) continue;
        const auto& s = g_title[entry].sprites[0];
        th19::AnmSprite sp; sp.id = 0; sp.x = s.x; sp.y = s.y; sp.w = s.w; sp.h = s.h;
        const i32 row = i32((CH_CY[i] - 200.0f) / 112.0f + 0.5f);
        const i32 t = g_charsel_frame - row * 3;                    // 原版 wait = 行号×3
        float scx = 1.0f, alpha = 1.0f;
        if (t < 0) { scx = 0.0f; alpha = 0.0f; }
        else {
            if (t < 30) scx = float(t) / 30.0f;                     // 绕Y轴翻转展开
            if (t < 6) alpha = float(t) / 6.0f;                     // 快速淡入
        }
        const u32 base = (i == g_char_sel || (g_vs_mode && i == g_opp_sel)) ? 0xffffff : 0xb0b0b0;
        const u32 tint = (u32(alpha * 255.0f) << 24) | base;
        emit_quad(30, it->second, sp, CH_CX[i], CH_CY[i], scx, 1.0f, 0, 0, tint, 0);
    }

    // 通关标记 rank00 sprite14（原版 script#81：pos(930,290) scale(1,0)→(1,1) 10帧展开，layer35）
    // 显示条件 = 通关标志 0x2eb16（原版 score.dat）；存档未接入前用 g_char_cleared 占位（当前全 false）
    if (!g_vs_mode && g_char_cleared[g_char_sel]) {
        const i32 rentry = 4;   // title/rank00.png
        auto rit = g_title_tex.find(rentry);
        if (rit != g_title_tex.end()) {
            for (const auto& rs : g_title[rentry].sprites) {
                if (rs.id == 14) {
                    th19::AnmSprite sp; sp.id = 0; sp.x = rs.x; sp.y = rs.y; sp.w = rs.w; sp.h = rs.h;
                    const float t = std::clamp(float(std::min(g_charsel_frame,g_char_switch_frame))/10.f,0.f,1.f);
                    const float angle=3.14159265359f*(1-t); // title.anm81: rotation X/Z pi -> 0, scale Y 0 -> 1.
                    emit_quad(35, rit->second, sp, 930.0f, 290.0f, 1.0f, t*std::cos(angle), 0, 0, 0xffffffff, angle);
                    break;
                }
            }
        }
    }

    // 左侧称号 nameXX（原版 script#118 case7：pos(-50,720)→(270,640) 16帧 ease-in + 淡入，layer34）
    if(!g_vs_mode){
        const i32 nentry = 53 + g_char_sel;
        auto nit = g_title_tex.find(nentry);
        if (nit != g_title_tex.end() && !g_title[nentry].sprites.empty()) {
            const auto& ns = g_title[nentry].sprites[0];
            th19::AnmSprite sp; sp.id = 0; sp.x = ns.x; sp.y = ns.y; sp.w = ns.w; sp.h = ns.h;
            const float nx = -50.0f + 320.0f * inf_e;
            const float ny = 720.0f - 80.0f * inf_e;
            const u32 a = u32(255.0f * inf_t);
            emit_quad(34, nit->second, sp, nx, ny, 1.0f, 1.0f, 0, 0, (a << 24) | 0xffffff, 0);
        }
    }

    // 右侧介绍 storyXX（原版 script#160 case7：pos(720,580)→(1040,500) 16帧 ease-in + 淡入，layer34）
    if(!g_vs_mode){
        const i32 sentry = 73 + g_char_sel;
        auto sit = g_title_tex.find(sentry);
        if (sit != g_title_tex.end() && !g_title[sentry].sprites.empty()) {
            const auto& ss = g_title[sentry].sprites[0];
            th19::AnmSprite sp; sp.id = 0; sp.x = ss.x; sp.y = ss.y; sp.w = ss.w; sp.h = ss.h;
            const float sx = 720.0f + 320.0f * inf_e;
            const float sy = 580.0f - 80.0f * inf_e;
            const u32 a = u32(255.0f * inf_t);
            emit_quad(34, sit->second, sp, sx, sy, 1.0f, 1.0f, 0, 0, (a << 24) | 0xffffff, 0);
        }
    }

    // VS uses the original cursor scripts including the CPU label and easing.
    if(g_vs_mode){
        collect_vm(g_vs_cursor_vm[0],g_cursor_cx,g_cursor_cy);
        collect_vm(g_vs_cursor_vm[1],g_p2_cursor_cx,g_p2_cursor_cy);
    }
    // Story shares the native 1P cursor (21), including confirmation interrupt6.
    if(!g_vs_mode)collect_vm(g_vs_cursor_vm[0],g_cursor_cx,g_cursor_cy);

}

void draw_title_version(){
    if(!g_title_v_vm.stream){auto it=g_title_v_scripts.find(0);if(it!=g_title_v_scripts.end())g_title_v_vm.bind_entry(it->second.stream,it->second.len,&g_title_v_scripts);}
    if(!g_title_v_vm.stream)return;
    auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy)->void{
        if(!v.visible)return;x+=(v.x+v.child_x)*sx;y+=(v.y+v.child_y)*sy;
        auto it=g_title_v_sprites.find(u32(v.sprite));
        if(it!=g_title_v_sprites.end()){auto tx=g_title_v_tex.find(it->second.first);if(tx!=g_title_v_tex.end())
            emit_quad(v.layer,tx->second,g_title_v[it->second.first].sprites[it->second.second],x,y,sx*v.sx,sy*v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);}
        for(const auto&c:v.children)self(self,c,x,y,sx*v.sx,sy*v.sy);
    };draw(draw,g_title_v_vm,0,0,1,1);
}
void draw_vs_rules(){
    for(int i=0;i<17;++i){const auto&r=VS_RULE_ROWS[i];const int sel=r.selection;
        const bool chosen=sel==g_sub_sel;
        const u32 color=sel<0?0xffffffff:chosen?0xffffff20:g_vs_rule_edit?0x80303030:0xff808080;
        const char*text=i>=10?VS_RULE_VALUES[sel][g_vs_rule_edit&&chosen?g_vs_rule_pending:g_vs_rules[sel]]:r.text;
        // 519a60/519770: all 17 rows use native font4 (40px), including heading.
        draw_text(text,40,640+r.x,r.y,color,35,0);
    }
}
void draw_sub_screen() {
    switch (g_screen) {
        case SCR_STORY:
            // Original difficulty root19 is rendered with g_sub_vms.
            break;
        case SCR_VSRULES:draw_vs_rules();break;
        case SCR_VS:
            break; // Original title.anm root190 is collected through g_sub_vms.
            break;
        case SCR_ACHIEVE:
            draw_achievement();   // 列表 6×4 网格 + 详情文字
            break;
        case SCR_MUSIC:
            draw_music();   // 标题条/列表底由 206+203 渲染，曲名文字由预渲染图叠加
            break;
        case SCR_OPTION:
            draw_option();
            break;
        case SCR_MANUAL:
            draw_help();
            break;
        case SCR_KEYCONFIG:
            draw_keyconfig();
            break;
        case SCR_CHARSEL:
            draw_charsel();
            break;
        case SCR_ENDING:
            draw_ending();
            break;
        default: break;
    }
}

// 子界面的可选项数量（Story 4 难度 / VS 5 / Option 6），其余 0。
i32 sub_item_count() {
    switch (g_screen) {
        case SCR_STORY: return 4;
        case SCR_VS:    return 5;
        case SCR_OPTION:return 6;   // Screen/BGM Vol/SE Vol/Key Config/Default/Quit
        case SCR_MUSIC: return 22;  // musiccmt.txt 22 曲
        case SCR_ACHIEVE: return 23; // trophy.txt 23 成就
        case SCR_KEYCONFIG:return 10;   // 10 个按键动作
        default: return 0;
    }
}

// 绑定玩家方向动画。所有 19 角色 entry0 脚本顺序一致：index 0=前向(idle) 1=左 2=过渡 3=右 4=过渡 5=特殊，
// 仅脚本 id 不同（pl00=-36/-35/-33，pl01=-48/-47/-45…），故按数组索引取 id 而非硬编码。
void bind_player_dir(i32 dir) {
    if (g_player.empty()) return;
    const auto& scs = g_player[0].scripts;
    const i32 idx = dir < 0 ? 1 : (dir > 0 ? 3 : 0);
    if (idx >= (i32)scs.size()) return;
    auto it = g_player_scripts.find(scs[idx].id);
    if (it == g_player_scripts.end() || !it->second.stream) return;
    g_player_vm = th19::AnmVm();
    g_player_vm.bind_entry(it->second.stream, it->second.len, &g_player_scripts);
}

// 绑定对手方向动画（按 entry0 脚本数组索引，见 bind_player_dir）
void bind_opp_dir(i32 dir) {
    if (g_opp.empty()) return;
    const auto& scs = g_opp[0].scripts;
    const i32 idx = dir < 0 ? 1 : (dir > 0 ? 3 : 0);
    if (idx >= (i32)scs.size()) return;
    const auto& e = g_opp[0];
    const auto& sc = scs[idx];
    g_opp_vm = th19::AnmVm();
    g_opp_vm.bind_entry(g_opp_bytes.data() + e.base + sc.offset,
                        u32(g_opp_bytes.size()) - e.base - sc.offset, nullptr);
}
// 画一个场内角色（idle/方向精灵，场坐标→屏，cx=该场中心屏 X）
void draw_field_player(const th19::AnmVm& vm, const std::map<u32,std::pair<i32,i32>>& spmap,
                       const std::vector<th19::AnmEntry>& entries, const std::map<i32,TextureAllocation>& tex,
                       float fx, float fy, float cx) {
    if (vm.sprite < 0) return;
    auto it = spmap.find(u32(vm.sprite));
    if (it == spmap.end()) return;
    const i32 ei = it->second.first;
    const auto& sp = entries[ei].sprites[it->second.second];
    auto ti = tex.find(ei);
    if (ti == tex.end()) return;
    g_actor_clip=cx>640?1:0;
    emit_quad(30, ti->second, sp, cx + fx * GAME_SCALE, GAME_FY0 + fy * GAME_SCALE,
              GAME_SCALE, GAME_SCALE, 0, 0,
              g_screen==SCR_GAME&&g_barrier[cx>640?1:0].invulnerable>0&&g_player_damage[cx>640?1:0].frame%3==0?0xffff0000:0xffffffff, 0);
    g_actor_clip=-1;
}
// 画 front.anm 对战场边框（脚本 sc5-sc11：sprite→pos，锚点左上，layer 27，alpha 128）
void draw_field_frame() {
    const i32 sids[7] = {0, 1, 2, 3, 5, 4, 6};    // 左竖/中竖/右竖/左上横/右上横/左下横/右下横
    const float px[7]  = {0, 624, 1248, 32, 656, 32, 656};
    const float py[7]  = {0, 0, 0, 0, 0, 928, 928};
    for (i32 i = 0; i < 7; ++i) {
        auto it = g_front_sprites.find(u32(sids[i]));
        if (it == g_front_sprites.end()) continue;
        const i32 ei = it->second.first;
        const auto& sp = g_front[ei].sprites[it->second.second];
        auto ti = g_front_tex.find(ei);
        if (ti == g_front_tex.end()) continue;
        emit_quad(27, ti->second, sp, px[i], py[i], 1.0f, 1.0f, 1, 1, 0x80ffffff, 0);
    }
}

// 画一个 front.anm 精灵（锚点左上，绝对屏幕坐标）
void draw_front_sprite(i32 sid, float x, float y, i32 layer, u32 tint) {
    auto it = g_front_sprites.find(u32(sid));
    if (it == g_front_sprites.end()) return;
    const i32 ei = it->second.first;
    const auto& sp = g_front[ei].sprites[it->second.second];
    auto ti = g_front_tex.find(ei);
    if (ti == g_front_tex.end()) return;
    emit_quad(layer, ti->second, sp, x, y, 1.0f, 1.0f, 1, 1, tint, 0);
}
// Native AuraCtrlInf 0x4e9740 / 0x4e8a20, aura.anm 0..12.
void draw_charge_gauge(float cx,const th19::ChargeGauge& gauge){
    if(g_skip_present)return;
    auto rect=[&](float x,float y,float w,float h,u32 color){th19::AnmSprite sp;sp.w=sp.h=1;emit_quad(34,g_white_tex,sp,x,y,w,h,1,1,color,0);};
    const float left=cx-226, y=912;
    rect(left,y,452,8,0xffffffff);rect(left+1,y+1,450,6,0x80000020);
    rect(left,y,452.f*gauge.energy/2500,7,0xff2060a8);
    if(gauge.charge>0)rect(left,y,452.f*gauge.charge/2500,7,0xffffffff);
    const int side=cx>640?1:0;
    // aura.anm 13 creates six script-14 points per frame; +4 is native
    // 0x5aa2d4, positioning the emitter at the charge bar's vertical centre.
    const auto& emitter=g_gauge_particles[side];
    for(const auto&v:emitter.children){
        auto mesh=presentation_geometry(v);if(mesh.empty())continue;
        DrawCmd q;q.layer=34;q.handle=g_white_tex.handle;q.mesh_blend=v.blend_mode;
        for(const auto&p:mesh)q.mesh.push_back(SpriteVertex{{left+452.f*gauge.charge/2500+v.x*GAME_SCALE+p.x,y+4+v.y*GAME_SCALE+p.y,0},1,p.color,{.5f,.5f}});
        g_draw.push_back(std::move(q));
    }
    for(int i=0;i<4;++i){
        float x=cx+th19::ChargeGauge::marker(gauge.thresholds[i])-2;
        // Native 0x4e95c0 drives the foreground/glow pair in aura.anm.
        for(int j=0;j<2;++j){const auto&v=g_gauge_markers[side][i*2+j];auto mesh=presentation_geometry(v);
            if(mesh.empty())continue;DrawCmd q;q.layer=34;q.handle=g_white_tex.handle;q.additive=v.blend_mode==1;q.mesh_blend=v.blend_mode;
            for(const auto&p:mesh)q.mesh.push_back(SpriteVertex{{cx+th19::ChargeGauge::marker(gauge.thresholds[i])+(v.x+p.x)*GAME_SCALE,y+(v.y+p.y)*GAME_SCALE,0},1,p.color,{.5f,.5f}});
            g_draw.push_back(std::move(q));
        }
        if(i>=2){char label[16];std::snprintf(label,sizeof(label),"Lv%d",1+(i==2?gauge.ex_level:gauge.boss_level));FontTex ft;if(render_text(label,18,ft)){th19::AnmSprite sp;sp.w=ft.w;sp.h=ft.h;emit_quad(35,ft.tex,sp,x-ft.w/2,923,1,1,1,1,0xffc0c0c0,0);}}
    }
    // 0x4e8a20: current charged tier, anchored at that tier's threshold.
    const int tier=gauge.level();
    if(tier>=2){char label[40];u32 color=0xff80d0ff;
        if(tier==2)std::snprintf(label,sizeof(label),"Skill");
        else if(tier==3){std::snprintf(label,sizeof(label),"Ex.Attack Lv%d",1+std::clamp(gauge.ex_level,0,7));color=0xffffff80;}
        else{std::snprintf(label,sizeof(label),"Boss Lv%d",1+std::clamp(gauge.boss_level,0,7));color=0xffff8080;}
        FontTex ft;if(render_text(label,18,ft)){th19::AnmSprite sp;sp.w=ft.w;sp.h=ft.h;
        // 0x4e8a20 passes absolute HUD y=448; 0x4e6c30 multiplies
        // by screen scale without adding the playfield origin. Keep the
        // user-requested lower Skill placement, but use native y for EX/Boss.
        const float promptY=tier==2?GAME_FY0+448*GAME_SCALE:448*GAME_SCALE;
        emit_quad(35,ft.tex,sp,cx+th19::ChargeGauge::marker(gauge.thresholds[tier-1])-ft.w*.5f,promptY,1,1,1,1,color,0);}}
}

void draw_procedural_node(const th19::AnmVm&v,int side,float x,float y,float sx=1,float sy=1,int layerOverride=-1,bool recurse=true,float parentAngle=0,bool inheritAngle=false,float secondaryParent=1){
    if(g_skip_present||!v.visible)return;
    const float pc=std::cos(parentAngle),ps=std::sin(parentAngle);
    const float lx=(v.x+v.child_x)*sx,ly=(v.y+v.child_y)*sy;
    x+=lx*pc-ly*ps;y+=lx*ps+ly*pc;
    const float cx=side?GAME_F2_CX:GAME_F1_CX;
    auto mesh=presentation_geometry(v,sx,sy,inheritAngle?secondaryParent:-1);
    if(!mesh.empty()){
        DrawCmd cmd{};cmd.handle=g_white_tex.handle;cmd.layer=layerOverride>=0?layerOverride:(v.layer==7?29:28);cmd.mesh_fog_enabled=false;cmd.additive=v.blend_mode==1;cmd.mesh_blend=v.blend_mode;
        cmd.mesh.reserve(mesh.size());
        auto triangle=[&](SpriteVertex a,SpriteVertex b,SpriteVertex c){
            // Most ring triangles lie fully inside one field. Preserve their
            // exact vertices and avoid four clipping passes in that common case.
            const float left=side?656.f:32.f,right=left+592.f;
            auto outside=[&](const SpriteVertex&v){return (v.position.x<left?1u:0u)|(v.position.x>right?2u:0u)|(v.position.y<32?4u:0u)|(v.position.y>928?8u:0u);};
            const auto ma=outside(a),mb=outside(b),mc=outside(c);
            if(ma&mb&mc)return;
            if(!(ma|mb|mc)){cmd.mesh.insert(cmd.mesh.end(),{a,b,c});return;}
            std::array<SpriteVertex,12> poly{},next{};poly[0]=a;poly[1]=b;poly[2]=c;size_t count=3;
            const float bounds[]={side?656.f:32.f,side?1248.f:624.f,32.f,928.f};
            for(int edge=0;edge<4&&count;++edge){size_t size=0;
                auto distance=[&](const SpriteVertex&v){float n=edge<2?v.position.x:v.position.y;return edge%2?bounds[edge]-n:n-bounds[edge];};
                auto prev=poly[count-1];float dp=distance(prev);
                for(size_t index=0;index<count;++index){const auto&cur=poly[index];float dc=distance(cur);if((dp<0)!=(dc<0)){
                    float t=dp/(dp-dc);auto v=prev;v.position.x+=(cur.position.x-prev.position.x)*t;v.position.y+=(cur.position.y-prev.position.y)*t;
                    v.color=0;for(int k=0;k<4;++k){int p=(prev.color>>(k*8))&255,q=(cur.color>>(k*8))&255;v.color|=u32(p+(q-p)*t)<<(k*8);}next[size++]=v;
                }if(dc>=0)next[size++]=cur;prev=cur;dp=dc;}poly=next;count=size;
            }for(size_t i=1;i+1<count;++i){cmd.mesh.push_back(poly[0]);cmd.mesh.push_back(poly[i]);cmd.mesh.push_back(poly[i+1]);}
        };

        for(size_t i=0;i+2<mesh.size();i+=3){
            auto vert=[&](const th19::AnmGeometryVertex&p){return SpriteVertex{{cx+(x+p.x*pc-p.y*ps)*GAME_SCALE,GAME_FY0+(y+p.x*ps+p.y*pc)*GAME_SCALE,0},1,p.color,{.5f,.5f}};};
            triangle(vert(mesh[i]),vert(mesh[i+1]),vert(mesh[i+2]));
        }
        if(!cmd.mesh.empty())g_draw.push_back(std::move(cmd));
    }
    if(recurse)for(const auto&c:v.children)draw_procedural_node(c,side,x,y,sx*v.sx,sy*v.sy,layerOverride,true,inheritAngle?parentAngle+v.rot_z:0,inheritAngle,secondaryParent*v.secondary_sx);
}
void draw_charge_regions(){
    for(int side=0;side<2;++side){
        const float px=side?g_p2_x:g_p1_x,py=side?g_p2_y:g_p1_y;
        if(g_barrier_vm[side].stream)draw_procedural_node(g_barrier_vm[side],side,px,py);
        const auto&r=g_ecl_fields[side].world.charge_region;
        if(g_region_vm[side].stream)for(int i=0;i<r.count;++i){float scale=r.character==9&&i>0?.6f:1.f;draw_procedural_node(g_region_vm[side],side,r.centers[i].x,r.centers[i].y,scale,scale);}
        const auto&v=g_charge_pattern[side];if(!v.stream||!v.visible)continue;
        g_actor_clip=side;
        for(int ei=0;ei<int(g_aura_entries.size());++ei)for(const auto&sp:g_aura_entries[ei].sprites)if(int(sp.id)==v.sprite){
            auto t=g_aura_tex.find(ei);if(t!=g_aura_tex.end())emit_quad(33,t->second,sp,(side?GAME_F2_CX:GAME_F1_CX)+(px+v.x)*GAME_SCALE,GAME_FY0+(py+v.y)*GAME_SCALE,v.sx*GAME_SCALE,v.sy*GAME_SCALE,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1);
        }
        g_actor_clip=-1;
    }
}
// Native HUD 0x512fd0 / 0x512d20; ascii1280 scripts 2/3 and sprites 239..250.
int g_timer_audit[2][8]{};
struct Timer {th19::AnmVm digits[2];u32 generation=0;int frame=-1,seconds=-1;bool shown=false,dim=false,spell=false;};
Timer rb_draw_boss_timer_timers[2];
void draw_boss_timer(int side,float cx){
    
    static std::unordered_map<i32,th19::AnmScriptLoc> scripts;
    if(scripts.empty())for(const auto&e:g_ascii)for(const auto&sc:e.scripts)
        scripts[sc.id]={g_ascii_bytes.data()+e.base+sc.offset,u32(g_ascii_bytes.size()-e.base-sc.offset),&e.scripts};
    auto&t=rb_draw_boss_timer_timers[side];const auto&w=g_ecl_fields[side].world;
    if(t.generation!=g_ecl_generation){t=Timer{};t.generation=g_ecl_generation;}
    int remaining=-1;
    if(!(w.boss_control_flags&1))for(u32 id:w.boss_slots)if(id){
        for(const auto&e:w.enemies)if(e.active&&e.id==id){remaining=e.phases.remaining_frames();break;}
        if(remaining>=0)break;
    }
    if(remaining<=0){t.shown=false;return;}
    const int seconds=std::min(99,remaining/60),hundredths=remaining/60>99?99:(remaining%60)*100/60;
    if(!t.shown){
        for(int i=0;i<2;++i){auto it=scripts.find(i+2);if(it==scripts.end())return;
            t.digits[i]={};t.digits[i].bind_entry(it->second.stream,it->second.len,&scripts);t.digits[i].tick();
            t.digits[i].interrupt_returning(w.spell.active?2:3);t.digits[i].interrupt_returning(4);}
        t.seconds=-1;t.dim=false;t.spell=w.spell.active;t.shown=true;t.frame=-1;
    }
    if(t.frame!=g_frame&&!g_paused){
        if(t.spell!=w.spell.active){for(auto&v:t.digits)v.interrupt_returning(w.spell.active?2:3);t.spell=w.spell.active;}
        const float py=side?g_p2_y:g_p1_y;
        const bool dim=t.dim?py<160:py<128;
        if(dim!=t.dim){for(auto&v:t.digits)v.interrupt_returning(dim?5:4);t.dim=dim;}
        if(seconds!=t.seconds){int signal=seconds>t.seconds?7:seconds<2?9:seconds<5?8:0;
            if(signal)for(auto&v:t.digits)v.interrupt_returning(signal);
            if(t.seconds>=0&&seconds<t.seconds&&seconds<5)play_sfx(seconds<2?12:11);
            t.seconds=seconds;}
        const int elapsed=t.frame<0?1:std::max(1,int(g_frame)-t.frame);
        for(int n=0;n<elapsed;++n)for(auto&v:t.digits)v.tick();t.frame=g_frame;
    }
    if(g_paused)t.frame=g_frame;
    g_timer_audit[side][0]=remaining;g_timer_audit[side][1]=int(t.digits[0].y*100);g_timer_audit[side][2]=int(t.digits[0].ip_pos.t1*100);g_timer_audit[side][3]=t.digits[0].ip_pos.elapsed;g_timer_audit[side][4]=t.spell;g_timer_audit[side][5]=g_paused;g_timer_audit[side][6]=t.frame;g_timer_audit[side][7]=g_frame;
    auto glyph=[&](int id,float x,float y,float sx,float sy,u32 color,int ax,int ay){
        for(int ei=0;ei<int(g_ascii.size());++ei)for(const auto&sp:g_ascii[ei].sprites)if(int(sp.id)==id){
            auto tx=g_ascii_tex.find(ei);if(tx!=g_ascii_tex.end())emit_quad(40,tx->second,sp,x,y,sx,sy,ax,ay,color,0);return;}
    };
    for(int i=0;i<2;++i){const auto&v=t.digits[i];glyph(239+(i?seconds%10:seconds/10),cx+v.x*GAME_SCALE,GAME_FY0+v.y*GAME_SCALE,v.sx,v.sy,v.color(),v.anchor_x,v.anchor_y);}
    const auto&v=t.digits[1];const float x=cx+(v.x+14)*GAME_SCALE,y=GAME_FY0+(v.y-8)*GAME_SCALE;
    glyph(250,x,y,1,1,v.color(),1,1);
    glyph(239+hundredths/10,x+8*GAME_SCALE,y+6*GAME_SCALE,.6f,.6f,v.color(),1,1);
    glyph(239+hundredths%10,x+(8+7.2f)*GAME_SCALE,y+6*GAME_SCALE,.6f,.6f,v.color(),1,1);
}
// 画某个玩家（场 center cx）的 HUD：命数心形 + boom 星形 + 充能槽圆点
th19::AnmVm rb_draw_field_hud_stars[2][7];int rb_draw_field_hud_labels[2][7]{};
u32 rb_draw_field_hud_generation=~0u,rb_draw_field_hud_last[2]{};
th19::AnmVm rb_draw_field_hud_wins[2][3];int rb_draw_field_hud_counts[2]{};int rb_draw_field_hud_winFrame[2]={-1,-1};
int rb_draw_field_hud_matchRound=0;
void draw_field_hud(float cx, i32 lives, i32 bombs, i32 charge, i32 fragments) {
    draw_boss_timer(cx>640?1:0,cx);
    // 0x544f30 / 0x4e5850: font 6 (sprite char+0x120), outline font 8;
    // right-aligned count and left-aligned 0.6-scale suffix share a baseline.
    const auto& chain=g_ecl_fields[cx>640?1:0].world;
    if(chain.combo_display>0){
        auto glyph=[&](u32 id)->std::pair<i32,const th19::AnmSprite*>{
            for(i32 ei=0;ei<i32(g_ascii.size());++ei)for(const auto&sp:g_ascii[ei].sprites)
                if(sp.id==id)return {ei,&sp};
            return {-1,nullptr};
        };
        const u32 tint=chain.combo_timer<=0&&chain.combo_display_timer%6>=3?0xff808080:0xffffff00;
        auto text=[&](const std::string&s,float scale,bool right){
            float x=cx-232.f;
            if(right)for(unsigned char c:s){auto g=glyph(c+0x120);if(g.second)x-=g.second->w*scale;}
            for(unsigned char c:s){
                for(int pass=0;pass<2;++pass){auto g=glyph(c+(pass?0x120:0x182));
                    auto tex=g_ascii_tex.find(g.first);if(g.second&&tex!=g_ascii_tex.end())
                        emit_quad(35,tex->second,*g.second,x,64.f,scale,scale,1,2,pass?tint:0xff000000,0);
                }
                auto g=glyph(c+0x120);if(g.second)x+=g.second->w*scale;
            }
        };
        text(std::to_string(chain.combo_display),1.f,true);text("hits",.6f,false);
    }
    (void)charge;
    draw_charge_gauge(cx,g_charge[cx>640?1:0]);
    // front.anm script16 selects native layer30 (foreground HUD).
    // Local layer30 is player shots; use the HUD pass shared by gauge rb_draw_field_hud_labels.
    for(i32 i=0;i<g_life_capacity[cx>640?1:0];++i)draw_front_sprite(i<lives?13:10,cx-16*g_life_capacity[cx>640?1:0]+i*32,32,35,0xffffffff);
    // 515200 -> front.anm17 interrupts: stock20/2, active7, fragments18/19, empty3.
    static std::unordered_map<i32,th19::AnmScriptLoc> scripts;
    
    
    if(scripts.empty())for(const auto&e:g_front)for(const auto&sc:e.scripts)
        scripts[sc.id]={g_front_bytes.data()+e.base+sc.offset,u32(g_front_bytes.size()-e.base-sc.offset),&e.scripts};
    if(rb_draw_field_hud_generation!=g_ecl_generation){for(auto&row:rb_draw_field_hud_labels)for(auto&l:row)l=0;rb_draw_field_hud_generation=g_ecl_generation;}
    const int side=cx>640?1:0,stock=std::clamp(bombs,0,7);
    for(int i=0;i<7;++i){auto&v=rb_draw_field_hud_stars[side][i];
        int label=i<stock?20:i==stock&&g_bombs[side].occupied()?7:
            i==stock+(g_bombs[side].occupied()?1:0)&&fragments>0?17+std::clamp(fragments,1,2):3;
        if(rb_draw_field_hud_labels[side][i]!=label){
            if(!rb_draw_field_hud_labels[side][i]){auto it=scripts.find(17);if(it==scripts.end())continue;v={};v.bind_entry(it->second.stream,it->second.len,&scripts);v.tick();}
            v.interrupt(label);if(label==20||label==18||label==19){v.tick();v.interrupt(2);}
            rb_draw_field_hud_labels[side][i]=label;
        }
        if(!g_paused&&rb_draw_field_hud_last[side]!=g_frame)v.tick();
        auto it=g_front_sprites.find(v.sprite);if(it==g_front_sprites.end()||!v.visible||v.alpha<=0)continue;
        auto ti=g_front_tex.find(it->second.first);if(ti==g_front_tex.end())continue;
        emit_quad(35,ti->second,g_front[it->second.first].sprites[it->second.second],cx+280+v.x,928-i*16+v.y,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);
    }
    rb_draw_field_hud_last[side]=g_frame;

    // 5142f0: front.anm18, left(32+32*i,16), right(1248-32*i,16).
    // Separate victory rb_draw_field_hud_stars from the bomb stock's front.anm17 instances.
    
    
    if(!g_vs_mode||g_vs_match.round<rb_draw_field_hud_matchRound||g_vs_match.wins[0]+g_vs_match.wins[1]==0){
        rb_draw_field_hud_counts[0]=rb_draw_field_hud_counts[1]=0;rb_draw_field_hud_matchRound=g_vs_match.round;
    }
    rb_draw_field_hud_matchRound=g_vs_match.round;g_win_stars_drawn[side]=0;
    const int won=g_vs_mode?std::clamp(g_vs_match.wins[side],0,3):0;
    for(int i=0;i<won;++i){auto&v=rb_draw_field_hud_wins[side][i];
        if(i>=rb_draw_field_hud_counts[side]){auto it=scripts.find(18);if(it==scripts.end())continue;
            v={};v.bind_entry(it->second.stream,it->second.len,&scripts);v.tick();}
        if(!g_paused&&rb_draw_field_hud_winFrame[side]!=g_frame)v.tick();
        auto it=g_front_sprites.find(v.sprite);if(it==g_front_sprites.end()||!v.visible)continue;
        auto ti=g_front_tex.find(it->second.first);if(ti==g_front_tex.end())continue;
        const int oldClip=g_actor_clip;g_actor_clip=-1;
        emit_quad(35,ti->second,g_front[it->second.first].sprites[it->second.second],side?1248-i*32:32+i*32,16,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);
        g_actor_clip=oldClip;
        ++g_win_stars_drawn[side];
    }
    rb_draw_field_hud_counts[side]=won;rb_draw_field_hud_winFrame[side]=g_frame;

}

// 显示一个大字提示（front.anm entry13），缩放 1.0→2.0 入场 + 停留 + 淡出
void show_big_text(i32 sprite_id) { g_big_sprite = sprite_id; g_big_frame = 0; }
// 大字动画：0-15 缩放 1.0→2.0 淡入，15-75 停留，75-105 淡出（屏幕居中）
void draw_big_text() {
    if (g_big_sprite < 0) return;
    auto it = g_front_sprites.find(u32(g_big_sprite));
    if (it == g_front_sprites.end()) { g_big_sprite = -1; return; }
    const i32 ei = it->second.first;
    const auto& sp = g_front[ei].sprites[it->second.second];
    auto ti = g_front_tex.find(ei);
    if (ti == g_front_tex.end()) { g_big_sprite = -1; return; }
    float scale = 2.0f;
    u32 alpha = 255;
    if (g_big_frame < 15)      { scale = 1.0f + float(g_big_frame) / 15.0f; alpha = u32(float(g_big_frame) / 15.0f * 255.0f); }
    else if (g_big_frame < 75) { scale = 2.0f; alpha = 255; }
    else if (g_big_frame < 105){ scale = 2.0f; alpha = u32(255.0f * float(105 - g_big_frame) / 30.0f); }
    else { g_big_sprite = -1; g_big_frame = 0; return; }
    for(float cx:{GAME_F1_CX,GAME_F2_CX}) emit_quad(40, ti->second, sp, cx, SCREEN_H * 0.5f, scale*.5f, scale*.5f, 0, 0, 0x00ffffff | (alpha << 24), 0);
    if(!g_paused)++g_big_frame;
}

// 显示对手符卡名横幅（索引指向 SPELL_CARDS[g_opp_sel].names）
void show_spell_name(i32 idx) { g_spell_idx = idx; g_spell_show = true; }
// 符卡名横幅：顶部居中，半透明黑底 + 白色日文符卡名（render_text 走 msgothic 字形）
void draw_spell_name() {
    if (!g_spell_show) return;
    const SpellCardList& sc = SPELL_CARDS[g_opp_sel];
    if (sc.count <= 0 || g_spell_idx < 0 || g_spell_idx >= sc.count) { g_spell_show = false; return; }
    { th19::AnmSprite sp; sp.id = 0; sp.x = 0; sp.y = 0; sp.w = 1.0f; sp.h = 1.0f;
      emit_quad(39, g_white_tex, sp, SCREEN_W * 0.5f, 130.0f, 720.0f, 70.0f, 0, 0, 0xc0000000, 0); }
    draw_text(std::string(sc.names[g_spell_idx]), 40.0f, SCREEN_W * 0.5f, 130.0f, 0xffffffff, 40);
}

// ---- 游戏场（正式进入游戏）----
// STD contains object geometry, instances and a timed camera script.
th19::StdScene g_stage;
int g_world_id=1;
TextureAllocation g_world_capture;
std::map<i32, th19::AnmVm> g_stage_anms;
std::map<i32,th19::WorldStrip> g_stage_strips;
std::unordered_map<i32,th19::AnmScriptLoc> g_world_scripts;
struct StageOverlay{int layer=0;th19::AnmVm vm;};
std::map<int,StageOverlay> g_stage_overlays;
std::map<u32,std::pair<i32,i32>> g_stage_sprites;
void dispatch_world_events(){
    for(const auto&e:g_stage.animationEvents){if(e.script<0){g_stage_overlays.erase(e.slot);continue;}auto s=g_world_scripts.find(e.script);if(s!=g_world_scripts.end()){auto&o=g_stage_overlays[e.slot];o.layer=e.layer;o.vm={};o.vm.bind_entry(s->second.stream,s->second.len,&g_world_scripts);o.vm.tick();}}
    for(int label:g_stage.interrupts){for(auto&e:g_stage_anms)e.second.interrupt_all(label);for(auto&e:g_stage_overlays)e.second.vm.interrupt_all(label);}
}
void tick_world(){g_screen_shake.tick();for(auto&e:g_stage_strips)e.second.tick();for(auto&e:g_stage_anms)e.second.tick();g_stage.tick();dispatch_world_events();for(auto&e:g_stage_overlays)e.second.vm.tick();}
void load_world_std() {
    g_stage_anms.clear();g_stage_strips.clear();g_stage_sprites.clear();g_stage_overlays.clear();g_world_scripts.clear();
    char path[40];std::snprintf(path,sizeof path,"/world%02d.std",g_world_id+1);
    g_stage={};if(read_asset(path,g_world_std_bytes))g_stage.load(g_world_std_bytes);
    for(const auto&e:g_world)for(const auto&s:e.scripts)g_world_scripts[s.id]={g_world_bytes.data()+e.base+s.offset,u32(g_world_bytes.size()-e.base-s.offset),&e.scripts};
    for(i32 ei=0;ei<i32(g_world.size());++ei){const auto&e=g_world[ei];
        for(i32 si=0;si<i32(e.sprites.size());++si)g_stage_sprites[e.sprites[si].id]={ei,si};
    }
    for(const auto&q:g_stage.quads){if(g_stage_anms.count(q.animation))continue;
        auto sc=g_world_scripts.find(q.script);if(sc==g_world_scripts.end())continue;
        auto&vm=g_stage_anms[q.animation];vm.bind_entry(sc->second.stream,sc->second.len,&g_world_scripts);vm.tick();
        if(vm.special_effect==5||vm.special_effect==6)g_stage_strips[q.animation].init(vm.special_effect,0x197900u+q.animation*7919);
    }
    g_stage.tick();
    dispatch_world_events();
}
bool load_world(int id) {
    if(id<0||id>=17)return false;
    g_stage_anms.clear();g_stage_overlays.clear();g_world_scripts.clear();
    for(const auto&kv:g_world_tex)gfx.destroy(kv.second.handle);
    g_world_tex.clear();g_world.clear();g_world_bytes.clear();g_world_id=id;
    char path[32];std::snprintf(path,sizeof path,"/world%02d.anm",id+1);
    if(!load_anm(path,g_world_bytes,g_world)){g_stage={};return false;}
    decode_textures(g_world,g_world_bytes,g_world_tex);load_world_std();
    return !g_stage.quads.empty()&&!g_world_tex.empty();
}
void draw_world_background() {
    if(g_skip_present)return; // Stage/ANM clocks advance in tick_world(), not here.
    th19::AnmSprite white;white.w=white.h=1;
    // Native stage pass 0x4ec721 clears its viewport with the current fog
    // color (+0x3bc). STD 13 controls the outer frame clear, not this pass.
    emit_quad(0,g_white_tex,white,0,0,SCREEN_W,SCREEN_H,1,1,g_stage.fog|0xff000000,0);
    const auto stage_begin=g_draw.size();
    auto norm=[](th19::Vec3 v){float n=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);return n>0?th19::Vec3{v.x/n,v.y/n,v.z/n}:th19::Vec3{0,0,1};};
    auto cross=[](th19::Vec3 a,th19::Vec3 b){return th19::Vec3{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};};
    auto dot=[](th19::Vec3 a,th19::Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;};
    auto direction=g_stage.direction;direction.x+=g_stage.directionOffset.x;direction.y+=g_stage.directionOffset.y;direction.z+=g_stage.directionOffset.z;
    const auto forward=norm(direction),right=norm(cross(g_stage.up,forward)),up=cross(forward,right);
    const float focal=480.f/std::tan(g_stage.fov*.5f);
    for(const auto&q:g_stage.quads){auto vi=g_stage_anms.find(q.animation);if(vi==g_stage_anms.end())continue;
        const auto&vm=vi->second;if(!vm.visible||vm.alpha<=0)continue;auto si=g_stage_sprites.find(u32(vm.sprite));if(si==g_stage_sprites.end())continue;
        auto ti=g_world_tex.find(si->second.first);if(ti==g_world_tex.end())continue;
        const auto&sp=g_world[si->second.first].sprites[si->second.second];
        DrawCmd c{};c.layer=1+q.order;c.handle=ti->second.handle;c.mesh_fog=g_stage.fog;c.additive=vm.blend_mode==1;c.mesh_blend=vm.blend_mode;c.mesh_depth=vm.render_mode==8||vm.special_effect==5||vm.special_effect==6;c.mesh_depth_write=vm.depth_write&&vm.special_effect==0;c.address_u=vm.address_u;c.address_v=vm.address_v;
        auto strip=g_stage_strips.find(q.animation);const bool isStrip=strip!=g_stage_strips.end();
        auto vertex=[&](float u,float v){
            // Native STD size overrides ANM scale when nonzero (0x4ebb30).
            float w=q.w!=0?q.w:sp.w*vm.sx,h=q.h!=0?q.h:sp.h*vm.sy;
            float ax=vm.anchor_x==1?0:vm.anchor_x==2?1:.5f,ay=vm.anchor_y==1?0:vm.anchor_y==2?1:.5f;
            th19::Vec3 p{(u-ax)*w,(v-ay)*h,0};
            if(isStrip){int row=std::clamp(int(std::round(v*(strip->second.count()-1))),0,strip->second.count()-1);p=strip->second.points[row];
                p.x=(p.x+(1-2*u)*strip->second.width(row))*(q.w!=0?q.w/sp.w:vm.sx);p.y*=q.h!=0?q.h/sp.h:vm.sy;}

            static constexpr int orders[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
            for(int axis:orders[std::clamp(vm.rotation_order,0,5)]){if((vm.render_mode==4||vm.render_mode==6)&&axis!=2)continue;float a=axis==0?vm.rot_x:axis==1?vm.rot_y:vm.rot_z,co=std::cos(a),sn=std::sin(a);auto t=p;
                if(axis==0){p.y=t.y*co-t.z*sn;p.z=t.y*sn+t.z*co;}
                if(axis==1){p.x=t.x*co+t.z*sn;p.z=-t.x*sn+t.z*co;}
                if(axis==2){p.x=t.x*co-t.y*sn;p.y=t.x*sn+t.y*co;}}
            if(vm.render_mode==4||vm.render_mode==6){auto t=p;p={right.x*t.x-up.x*t.y,right.y*t.x-up.y*t.y,right.z*t.x-up.z*t.y};}
            p.x+=q.x+vm.x-g_stage.eye.x-g_stage.eyeOffset.x;p.y+=q.y+vm.y-g_stage.eye.y-g_stage.eyeOffset.y;p.z+=q.z+vm.z-g_stage.eye.z-g_stage.eyeOffset.z;
            const float z=dot(p,forward);float fog=std::clamp((z-g_stage.fogNear)/std::max(1.f,g_stage.fogFar-g_stage.fogNear),0.f,1.f);
            u32 col=(u32(vm.alpha)<<24)|(u32(vm.r)<<16)|(u32(vm.g)<<8)|vm.b;
            if(isStrip)col=strip->second.color(int(std::round(v*(strip->second.count()-1))),int(u));
            float tu=u,tv=isStrip?v*float(strip->second.count()-1)/strip->second.count():v;
            return SpriteVertex{{dot(p,right),dot(p,up),z},1,col,{(sp.x+tu*sp.w)/ti->second.width+vm.u_offset,(sp.y+tv*sp.h)/ti->second.height+vm.v_offset},u32(255*(1-fog))<<24};
        };
        // RHW provides perspective-correct UVs; subdivisions interpolate fog.
        // Native camera easing and ANM UV animation remain under audit.
        const int NX=isStrip?1:2,NY=isStrip?strip->second.count()-1:4;
        for(int y=0;y<NY;++y)for(int x=0;x<NX;++x){auto a=vertex(float(x)/NX,float(y)/NY),b=vertex(float(x+1)/NX,float(y)/NY),d=vertex(float(x)/NX,float(y+1)/NY),e=vertex(float(x+1)/NX,float(y+1)/NY);
            // Clip in camera space before perspective division; dropping whole
            // cells makes nearby pillars and ground disappear at the near plane.
            std::vector<SpriteVertex> poly{a,b,e,d},clipped;
            auto prev=poly.back();
            for(const auto&cur:poly){
                if((prev.position.z>=1)!=(cur.position.z>=1)){
                    float t=(1-prev.position.z)/(cur.position.z-prev.position.z);auto v=prev;
                    v.position={prev.position.x+(cur.position.x-prev.position.x)*t,prev.position.y+(cur.position.y-prev.position.y)*t,1};
                    u32 color=0;for(int shift=0;shift<32;shift+=8)color|=u32(float((prev.color>>shift)&255)+(float((cur.color>>shift)&255)-float((prev.color>>shift)&255))*t)<<shift;v.color=color;
                    v.uv.x+=(cur.uv.x-prev.uv.x)*t;v.uv.y+=(cur.uv.y-prev.uv.y)*t;clipped.push_back(v);
                }
                if(cur.position.z>=1)clipped.push_back(cur);prev=cur;
            }
            for(auto&v:clipped){float z=v.position.z;v.reciprocal_w=1/z;
                float distance=vm.render_mode==6?std::sqrt(v.position.x*v.position.x+v.position.y*v.position.y+z*z):z;
                float fog=vm.render_mode==4?0:std::clamp((distance-g_stage.fogNear)/std::max(1.f,g_stage.fogFar-g_stage.fogNear),0.f,1.f);v.specular=u32(255*(1-fog))<<24;
                v.position={640+focal*v.position.x/z,480-focal*v.position.y/z,1-1/z};
            }
            for(size_t i=1;i+1<clipped.size();++i)c.mesh.insert(c.mesh.end(),{clipped[0],clipped[i],clipped[i+1]});}
        g_draw.push_back(std::move(c));
    }
    // Layer 3 uses native origin 0x62ce48: (screenWidth/2, (screenHeight-448)/2), 0x4b53a0.
    for(const auto&e:g_stage_overlays){const auto&vm=e.second.vm;auto s=g_stage_sprites.find(vm.sprite);if(!vm.visible||s==g_stage_sprites.end())continue;auto t=g_world_tex.find(s->second.first);if(t==g_world_tex.end())continue;
        emit_quad(1+e.second.layer,t->second,g_world[s->second.first].sprites[s->second.second],640+vm.x*2,(960-448)*.5f+vm.y*2,vm.sx*2,vm.sy*2,vm.anchor_x,vm.anchor_y,(u32(vm.alpha)<<24)|(u32(vm.r)<<16)|(u32(vm.g)<<8)|vm.b,vm.rot_z,vm.blend_mode==1);
        if(!g_skip_present){auto&cmd=g_draw.back();cmd.u0+=vm.u_offset;cmd.u1+=vm.u_offset;cmd.v0+=vm.v_offset;cmd.v1+=vm.v_offset;}
    }
    // 4ec550 copies 62c414/418 into the stage camera's render offset.
    // ECL517 uses slot 2; field-specific cameras consume slots 0/1.
    for(size_t i=stage_begin;i<g_draw.size();++i){auto&c=g_draw[i];
        c.x+=g_screen_shake.stage_x;c.y+=g_screen_shake.stage_y;
        for(auto&v:c.mesh){v.position.x+=g_screen_shake.stage_x;v.position.y+=g_screen_shake.stage_y;}
    }
    // STD 17: original 0x4ea770 creates 17x7 / 17x17 meshes; 0x4e9a20
    // offsets their interior vertices, keeping the four field edges fixed.
    if(g_stage.distortion==1||g_stage.distortion==2){
        if(!g_world_capture.handle){std::vector<u8> pixels(1280*960*4);g_world_capture=gfx.create_raw(1280,960,pixels.data());}
        DrawCmd c{};c.layer=26;c.handle=g_world_capture.handle;c.world_capture=true;c.mesh_fog_enabled=false;c.address_u=c.address_v=3;
        int nx=17,ny=g_stage.distortion==1?7:17;float frame=float(g_stage.clock-g_stage.distortionStart);
        for(int side=0;side<2;++side){std::vector<SpriteVertex> grid;grid.reserve(nx*ny);
            for(int x=0;x<nx;++x)for(int y=0;y<ny;++y){float px=(side?656.f:32.f)+592.f*x/(nx-1),py=32.f+896.f*y/(ny-1);float dx=0,dy=0;
                if(x>0&&y>0&&x<nx-1&&y<ny-1){float amplitude=24*(1-float(y)/(ny-1));dx=std::sin(frame*3.14159265f/64+(x*ny+y)*3.14159265f/4.7f)*amplitude;dy=std::sin(frame*3.14159265f/80-x*3.14159265f/2.1f)*amplitude;}
                grid.push_back({{px+dx,py+dy,0},1,0xc0ffffff,{px/1280,py/960}});
            }
            for(int x=0;x<nx-1;++x)for(int y=0;y<ny-1;++y){int a=x*ny+y,b=a+ny;c.mesh.insert(c.mesh.end(),{grid[a],grid[b],grid[b+1],grid[a],grid[b+1],grid[a+1]});}
        }
        g_draw.push_back(std::move(c));
    }

}

// ---- 剧情对话（th19 msg 格式：type17=XOR加密文本 util_xor 0x77/7/16，thtk thmsg06.c 实据）----
th19::StorySession g_story_session;
th19::StoryPlayback g_story_playback;
bool g_vs_dialogue_active=false;
EclField g_story_field;
std::vector<u8> g_boss_bytes;
std::vector<th19::AnmEntry> g_boss_entries;
std::map<i32,TextureAllocation> g_boss_tex;
std::map<u32,std::pair<i32,i32>> g_boss_sprites;
std::unordered_map<i32,th19::AnmScriptLoc> g_boss_scripts;
struct StoryActorAnimation {i32 script=-1;u32 generation=0,interrupt_generation=0;th19::AnmVm vm;th19::ChargePoint mist;};
std::unordered_map<u32,StoryActorAnimation> g_story_animations;
void start_story_actor();
void tick_story_actor();
void draw_story_actor();
th19::PauseAnimation g_pause_animation;
int g_pause_view=0;
void retry_story_stage();
void init_pause_animation(bool retry=false){
    static std::unordered_map<i32,th19::AnmScriptLoc> scripts;
    if(scripts.empty())for(const auto&e:g_front)for(const auto&sc:e.scripts)
        scripts[sc.id]={g_front_bytes.data()+e.base+sc.offset,u32(g_front_bytes.size()-e.base-sc.offset),&e.scripts};
    g_pause_animation.start(scripts,g_owned_cards.empty(),retry);g_pause_view=0;
}
void tick_pause_animation(){
    if(g_pause_animation.tick()){
        g_paused=g_pause_confirm=false;
        if(g_pause_animation.retryMenu&&!g_pause_animation.quit){retry_story_stage();return;}
        // 537d40 -> scene4; TitleInf retains DAT_005d1c14=3 from character selection.
        if(g_pause_animation.quit){g_story_challenge_run=false;g_story_field.world.enemies.clear();g_next_screen=SCR_CHARSEL;g_trans=TRANS_OUT;g_trans_frame=TRANS_FRAMES-1;}
    }
}
void draw_pause_node(const th19::AnmVm&v,float x,float y){
    if(!v.visible)return;x+=v.x;y+=v.y;
    auto s=g_front_sprites.find(v.sprite);
    // Script 52 belongs to native layer 29, not the pause menu layer 34.
    // Keep lower scene layers out of this UI pass over the frozen field.
    if(v.layer>=32&&s!=g_front_sprites.end()&&(v.color()>>24)>0){auto t=g_front_tex.find(s->second.first);
        if(t!=g_front_tex.end())emit_quad(71,t->second,g_front[s->second.first].sprites[s->second.second],x,y,v.sx,v.sy,v.anchor_x,v.anchor_y,th19::PauseAnimation::menuColor(v),v.rot_z);}
    for(const auto&c:v.children)draw_pause_node(c,x,y);
}
void draw_pause(){
    if(!g_paused){if(g_pause_animation.quit&&g_trans==TRANS_OUT){th19::AnmSprite black;black.w=black.h=1;emit_quad(100,g_white_tex,black,0,0,1280,960,1,1,0xff000000,0);}return;}
    th19::AnmSprite shade;shade.w=shade.h=1;
    emit_quad(70,g_white_tex,shade,0,0,1280,960,1,1,0x80304060,0);
    if(g_pause_view){size_t first=g_draw.size();if(g_pause_view==1)draw_help();else if(g_pause_view==2)draw_option();else if(g_pause_view==4)draw_owned_card_menu();else draw_keyconfig();
        for(size_t i=first;i<g_draw.size();++i)g_draw[i].layer+=g_pause_view==4?0:72;return;}
    draw_pause_node(g_pause_animation.root,128,0);
    if(g_pause_animation.quit&&!g_pause_animation.retryMenu&&g_pause_animation.phase==4){float t=std::clamp(float(g_pause_animation.age-20)/10,0.f,1.f);emit_quad(100,g_white_tex,shade,0,0,1280,960,1,1,u32(t*255)<<24,0);}
}

th19::DialogueBubble g_dialogue_bubble;
std::unordered_map<i32,th19::AnmScriptLoc> g_bubble_scripts;
void tick_dialogue_bubble(){
 if(g_bubble_scripts.empty())for(const auto&e:g_front)for(const auto&sc:e.scripts)g_bubble_scripts[sc.id]={g_front_bytes.data()+e.base+sc.offset,u32(g_front_bytes.size()-e.base-sc.offset),&e.scripts};
 g_dialogue_bubble.tick(g_story_playback,g_bubble_scripts,&g_story_text_scripts);
}
struct DialoguePortrait {th19::AnmVm body,face;bool active=false;};
DialoguePortrait g_dialogue_portraits[2];
th19::AnmVm g_dialogue_name;
std::unordered_map<i32,th19::AnmScriptLoc> g_dialogue_name_scripts;
void reset_dialogue_portraits(){g_dialogue_bubble={};for(auto&p:g_dialogue_portraits)p={};g_dialogue_name={};g_dialogue_name_scripts.clear();}
void tick_dialogue_portraits(){
    auto interrupt=[&](int side,int label,bool expression=false){
        auto&p=g_dialogue_portraits[side];if(!p.active)return;
        if(expression){p.body.interrupt_returning(label);p.face.interrupt_returning(label);}
        else {p.body.interrupt(label);p.face.interrupt(label);}
    };
    for(const auto&e:g_story_playback.emitted){
        if(e.op==1||e.op==2){int side=e.op-1,ch=side?g_opp_sel:g_char_sel;auto&p=g_dialogue_portraits[side];p={};
            for(const auto&entry:g_plf[ch])for(const auto&sc:entry.scripts){
                th19::AnmVm*vm=sc.id==2+side?&p.body:sc.id==4+side?&p.face:nullptr;
                if(vm){vm->bind(g_plf_bytes[ch].data()+entry.base+sc.offset,u32(g_plf_bytes[ch].size()-entry.base-sc.offset));vm->tick();p.active=true;}
            }
        }else if(e.op==4||e.op==5){interrupt(e.op-4,1);if(e.op==5&&g_dialogue_name.stream)g_dialogue_name.interrupt_all(1);}
        else if(e.op==7||e.op==8){interrupt(e.op-7,2);interrupt(1-(e.op-7),3);}
        else if(e.op==13||e.op==14)interrupt(e.op-13,17+e.arg,true);
        else if(e.op==23||e.op==24)interrupt(e.op-23,7);
        else if(e.op==20){
            g_dialogue_name={};g_dialogue_name_scripts.clear();
            for(const auto&entry:g_plf[g_opp_sel])for(const auto&sc:entry.scripts)
                g_dialogue_name_scripts[sc.id]={g_plf_bytes[g_opp_sel].data()+entry.base+sc.offset,u32(g_plf_bytes[g_opp_sel].size()-entry.base-sc.offset),&entry.scripts};
            auto it=g_dialogue_name_scripts.find(8);
            if(it!=g_dialogue_name_scripts.end())g_dialogue_name.bind_entry(it->second.stream,it->second.len,&g_dialogue_name_scripts);
        }
    }
    for(auto&p:g_dialogue_portraits)if(p.active){p.body.tick();p.face.tick();if(!p.body.visible&&!p.face.visible)p.active=false;}
    if(g_dialogue_name.stream)g_dialogue_name.tick();
}
void start_game(bool preserveMusic=false);
void draw_game();
void enter_story_stage() {
    g_paused=g_pause_confirm=false;
    g_opp_sel=g_story_session.opponent();
    // Stage stock reset is independent of the surviving story player's lives.
    g_p1_bombs=g_p2_bombs=3;g_p1_fragments=g_p2_fragments=0;
    // 0x517d9f: side1 always initializes; side0 preserves lives when
    // story stage-advance flag (DAT_0062b0ac & 4) is set.
    g_p2_lives=5;
    start_game();
    g_story_playback.begin(g_story_session.scene());reset_dialogue_portraits();
    g_story_field=EclField{};g_story_animations.clear();
    // 556340 state4 and 5539c0 both enter through screenswitch.anm.
    g_game_load_frames=0;
    g_screen=SCR_DIALOG;
}

void retry_story_stage(){
    if(!g_story_session.retry_stage())return;
    // 516f00 scene14 / 517d9f continue flag16 restores the player's stock.
    g_story_continues=std::min(9,g_story_continues+1);
    g_p1_lives=g_story_initial_lives;
    enter_story_stage();
    g_game_load_frames=0;
}

// 517790: VS stage and music share a random choice of participating character.
void start_vs_match(bool newMatch=true) {
    g_story_challenge_run=false;
    if(newMatch){
        g_vs_match.begin(g_vs_rules[0],g_vs_rules[1]);
        // 0x517790: subsequent rounds reuse the participating side already chosen.
        g_vs_scene_side=g_vs_scene_rng.next()&1;
        g_vs_scene=th19::vsScene(g_char_sel,g_opp_sel,g_vs_scene_side);
        g_owned_cards.clear();g_owned_card_vms.clear();g_card_options.clear();g_card_reward=-1;g_card_reward_exit=-1;
        // VS selection must supply verified card IDs for each side.
        // Do not fabricate a hand from character indices or grant the whole bank.
    }else g_vs_match.next();
    g_vs_mode=1;
    // Difficulty must come from the difficulty selector, never the VS menu index.
    g_story_session.begin(th19::STORY_ROUTES[g_char_sel]);
    // Keep the opponent chosen on the second selector.
    g_story_session.skip_scene();
    g_p1_lives=g_vs_rules[3]+1;g_p2_lives=g_vs_rules[4]+1;g_p1_bombs=g_p2_bombs=3;g_p1_fragments=g_p2_fragments=0;
    g_life_capacity[0]=g_p1_lives;g_life_capacity[1]=g_p2_lives;
    start_game(!newMatch);play_story_music(g_vs_scene.music);
    g_cpu_automatic=g_vs_entry!=0;g_opponent_controls=0;
    g_game_load_frames=0;g_screen=SCR_GAME;
    // EXE 50efc0: completed wins choose the announcement; draws repeat it.
    const int completed=g_vs_match.wins[0]+g_vs_match.wins[1];
    const int script=g_vs_rules[1]==0?347:g_vs_rules[1]==1?(completed==0?351:completed==1?355:367):351+4*std::min(completed,4);
    show_big_text(175+(script-347)/4);
}
// screenswitch.anm, native 0x4db540 / 0x4dbe90. The four VMs live
// across stage destruction/recreation; their origin is (320,240).
struct StoryScreenSwitch {
    std::vector<u8> bytes;
    std::vector<th19::AnmEntry> entries;
    std::map<i32,TextureAllocation> textures;
    std::unordered_map<i32,th19::AnmScriptLoc> scripts;
    th19::AnmVm masks[4],overlay;
    bool vs_match=false,vs_next_round=false,story_start=false;
    int phase=0,frame=0; // 1 close, 2 reveal
} g_story_switch;
bool bind_story_switch(int first) {
    auto& s=g_story_switch;
    if(s.scripts.empty()){
        if(!load_anm("/screenswitch.anm",s.bytes,s.entries))return false;
        decode_textures(s.entries,s.bytes,s.textures);
        for(const auto&e:s.entries)for(const auto&sc:e.scripts)
            s.scripts[sc.id]={s.bytes.data()+e.base+sc.offset,u32(s.bytes.size()-e.base-sc.offset),&e.scripts};
    }
    for(int i=0;i<4;++i){
        const auto it=s.scripts.find(first+i);if(it==s.scripts.end())return false;
        s.masks[i]={};s.masks[i].bind_entry(it->second.stream,it->second.len,&s.scripts);s.masks[i].tick();
    }
    if(first==3){
        const auto it=s.scripts.find(11);if(it==s.scripts.end())return false;
        s.overlay={};s.overlay.bind_entry(it->second.stream,it->second.len,&s.scripts);s.overlay.tick();
    }
    s.frame=0;return true;
}
void start_dialogue();
void start_story_switch() {
    g_story_switch.vs_match=false;g_story_switch.story_start=false;
    if(bind_story_switch(3))g_story_switch.phase=1;
    else enter_story_stage();
}
void tick_story_switch() {
    auto& s=g_story_switch;
    for(auto&v:s.masks)v.tick();s.overlay.tick();
    ++s.frame;
    // 0x5539c0 state6: create at frame10, advance after frame39.
    if(s.phase==1&&s.frame==30){
        if(s.story_start){s.story_start=false;start_dialogue();}
        else if(s.vs_match){start_vs_match(!s.vs_next_round);s.vs_match=false;s.vs_next_round=false;}else enter_story_stage();
        // 0x515d80 sends interrupt1 after loading the new stage.
        bind_story_switch(7);s.phase=2;
    }else if(s.phase==2){
        bool deleted=true;for(const auto&v:s.masks)deleted=deleted&&!v.visible;
        // 0x4db750 also has a 60-frame upper bound after interrupt1.
        if(deleted||s.frame>=60)s.phase=0;
    }
}
void draw_story_switch() {
    const auto&s=g_story_switch;if(!s.phase)return;
    auto alphaBuffer=[&](u32 color){
        DrawCmd q{};q.layer=90;q.handle=g_white_tex.handle;q.mesh_blend=-10;q.mesh_fog_enabled=false;
        q.mesh={{{0,0,0},1,color,{.5f,.5f}},{{1280,0,0},1,color,{.5f,.5f}},{{0,960,0},1,color,{.5f,.5f}},
                {{1280,0,0},1,color,{.5f,.5f}},{{1280,960,0},1,color,{.5f,.5f}},{{0,960,0},1,color,{.5f,.5f}}};
        g_draw.push_back(std::move(q));
    };
    alphaBuffer(0);
    for(const auto&v:s.masks){
        if(!v.visible)continue;
        // DAT_0062bc2c==0x15 is D3DFMT_A8R8G8B8 (not a scene ID).
        // 0x4db7d0 preserves RGB and accumulates srcAlpha squared in A.
        auto mesh=presentation_geometry(v);
        if(!mesh.empty()){
            DrawCmd q{};q.layer=90;q.handle=g_white_tex.handle;q.mesh_blend=-11;q.mesh_fog_enabled=false;
            for(const auto&p:mesh)q.mesh.push_back(SpriteVertex{{640+(v.x+p.x)*2,480+(v.y+p.y)*2,0},1,p.color,{.5f,.5f}});
            g_draw.push_back(std::move(q));
        }else for(int ei=0;ei<int(s.entries.size());++ei)for(const auto&sp:s.entries[ei].sprites)if(int(sp.id)==v.sprite){
            auto t=s.textures.find(ei);if(t!=s.textures.end()){
                emit_quad(90,t->second,sp,640+v.x*2,480+v.y*2,v.sx*2,v.sy*2,v.anchor_x,v.anchor_y,v.color(),v.rot_z,false,-11);
                // 0x4ca02d / 0x4ca079: ANM sampler 0=wrap,1=clamp,2=mirror.
                // Sprite0 intentionally extends beyond its 64px gradient.
                if(!g_skip_present){g_draw.back().address_u=v.address_u==1?3:1;
                    g_draw.back().address_v=v.address_v==1?3:1;}
            }
        }
    }
    const auto&v=s.overlay;
    for(int ei=0;ei<int(s.entries.size());++ei)for(const auto&sp:s.entries[ei].sprites)if(int(sp.id)==v.sprite){
        auto t=s.textures.find(ei);if(t==s.textures.end())continue;
        // Script11 is coordinate_mode0: texture/positions already 1280x960.
        emit_quad(90,t->second,sp,v.x,v.y,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,false,-12);
        if(!g_skip_present){auto&q=g_draw.back();q.u0+=v.u_offset;q.u1+=v.u_offset;q.v0+=v.v_offset;q.v1+=v.v_offset;
            q.address_u=v.address_u;q.address_v=v.address_v;}
    }
    // Browser canvas compositing must not inherit the temporary alpha mask.
    alphaBuffer(0xffffffff);
}
void start_dialogue() {
    g_vs_mode=0;g_life_capacity[0]=g_life_capacity[1]=5;
    g_story_challenge_run=true;g_story_initial_lives=5;g_story_continues=0;
    g_story_switch.phase=0;
    g_owned_cards.clear();g_owned_card_vms.clear();g_card_options.clear();g_card_reward=-1;g_card_reward_exit=-1;
    g_p1_lives=g_p2_lives=5;g_p1_bombs=g_p2_bombs=3;g_p1_fragments=g_p2_fragments=0;g_player_collision_enabled=true;
    g_story_session.begin(th19::STORY_ROUTES[g_char_sel]);
    enter_story_stage();
}
// Battle-result entry point. Only a real result (or the explicit test harness)
// may enter post-battle dialogue; ending a wave never advances the route.
// The timeout flag is part of the result state from the first report frame;
// native 515540 selects a different ANM pair before the result animation starts.
bool begin_battle_report(int winner, bool timeout) {
    if(g_battle_report_frame>=0||g_story_session.phase()!=th19::StoryPhase::Battle)return false;
    g_battle_timeout=timeout;g_battle_winner=winner;g_battle_report_frame=0;g_battle_reports[0]={};g_battle_reports[1]={};g_big_sprite=-1;
    // A completed round clears the active playfield before the result card is
    // shown.  Leaving ECL objects alive caused bullets/enemies to remain
    // visible through the victory announcement and looked like a freeze.
    if(!timeout){
        for(auto&field:g_ecl_fields){field.world.bullets.clear();field.world.lasers.clear();field.world.enemies.clear();field.world.pickups.clear();field.world.bullet_cancels.clear();}
        g_player_projectiles.clear();g_player_hit_effects.clear();
    }
    return true;
}
bool resolve_story_battle(bool won) { return begin_battle_report(won?0:1,false); }
void tick_battle_report(){
    if(g_battle_report_frame==120){
        // Native515540 credits the winner when the result is announced.
        if(g_vs_mode)g_vs_match.result(g_battle_winner);
        static std::unordered_map<i32,th19::AnmScriptLoc> scripts;
        if(scripts.empty())for(const auto&e:g_front)for(const auto&sc:e.scripts)
            scripts[sc.id]={g_front_bytes.data()+e.base+sc.offset,u32(g_front_bytes.size()-e.base-sc.offset),&e.scripts};
        // 515540: winner's death counter +0x64==0 selects result flag0x100000.
        const bool complete=g_battle_winner>=0&&g_stage_deaths[g_battle_winner]==0;
        for(int side=0;side<2;++side){int id=g_battle_winner<0?346:g_battle_timeout?(side==g_battle_winner?340:345):side==g_battle_winner?(complete?337:334):(complete?344:343);
            auto it=scripts.find(id);if(it==scripts.end())continue;auto&v=g_battle_reports[side];
            v.bind_entry(it->second.stream,it->second.len,&scripts);v.tick();v.interrupt_all(side+7);
        }
    }
    for(auto&v:g_battle_reports)if(v.stream)v.tick();
    if(g_battle_report_frame==320)for(auto&v:g_battle_reports)v.interrupt_all(1);
    if(++g_battle_report_frame>328){
        g_battle_report_frame=-1;
        if(g_vs_mode){
            if(g_vs_match.wins[0]<g_vs_match.target&&g_vs_match.wins[1]<g_vs_match.target){
                g_story_switch.vs_match=true;g_story_switch.vs_next_round=true;
                if(bind_story_switch(3))g_story_switch.phase=1;else start_vs_match(false);
                return;
            }
            // Native 0x50f320/0x515050 selects and opens the winner's VS MSG
            // after the final result animation.  Keep VS mode alive while the
            // dialogue player draws the two selected fighters.
            g_ecl_ready=false;g_player_projectiles.clear();g_player_hit_effects.clear();
            g_story_field=EclField{};g_story_animations.clear();
            const int winner=g_battle_winner==0?g_char_sel:g_opp_sel;
            const int loser=g_battle_winner==0?g_opp_sel:g_char_sel;
            if(const auto*scene=th19::vsDialogueScene(winner,loser,g_battle_winner)){
                g_vs_dialogue_active=true;g_story_playback.begin(scene);reset_dialogue_portraits();g_screen=SCR_DIALOG;
            }else{g_vs_mode=0;g_next_screen=SCR_TITLE;g_trans=TRANS_OUT;g_trans_frame=0;}
            return;
        }
        if(g_story_session.resolve_battle(g_battle_winner==0)){
            g_story_playback.begin(g_story_session.scene());reset_dialogue_portraits();
            g_story_field=EclField{};g_story_animations.clear();g_screen=SCR_DIALOG;
        }
    }
}
void draw_battle_report(){
    if(g_battle_report_frame<120)return;
    auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy)->void{
        if(!v.visible)return;
        const float unit=(v.coordinate_mode==2||v.coordinate_mode==4||v.coordinate_mode==6)?1.f:2.f;
        x+=(v.x+v.child_x)*unit;y+=(v.y+v.child_y)*unit;
        auto mesh=presentation_geometry(v,sx*unit,sy*unit);
        if(!mesh.empty()){
            DrawCmd cmd{};cmd.handle=g_white_tex.handle;cmd.layer=42;cmd.mesh_blend=v.blend_mode;cmd.additive=v.blend_mode==1;
            for(const auto&p:mesh)cmd.mesh.push_back(SpriteVertex{{x+p.x,y+p.y,0},1,p.color,{.5f,.5f}});
            g_draw.push_back(std::move(cmd));
        }
        auto it=g_front_sprites.find(v.sprite);if(it!=g_front_sprites.end()){
            auto tx=g_front_tex.find(it->second.first);if(tx!=g_front_tex.end())
                emit_quad(42,tx->second,g_front[it->second.first].sprites[it->second.second],x,y,sx*unit*v.sx,sy*unit*v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);
        }
        for(const auto&c:v.children){
            const float csx=c.ignore_parent_transform?sx:sx*v.sx;
            const float csy=c.ignore_parent_transform?sy:sy*v.sy;
            self(self,c,x,y,csx,csy);
        }
    };for(const auto&v:g_battle_reports)draw(draw,v,0,0,1,1);
}


// Persistent original plXXf ANM scripts: entry, speaking/listening, shake,
// expression changes and exit continue even while the MSG waits for input.
void draw_dialogue() {
    draw_game();
    auto name=[&](auto&& self,const th19::AnmVm&v,float x,float y)->void{
        if(!v.visible)return;x+=v.x;y+=v.y;
        if(v.alpha>0)for(int ei=0;ei<int(g_plf[g_opp_sel].size());++ei)for(const auto&sp:g_plf[g_opp_sel][ei].sprites)if(int(sp.id)==v.sprite){
            auto t=g_plf_tex[g_opp_sel].find(ei);if(t!=g_plf_tex[g_opp_sel].end())
                emit_quad(v.layer+4,t->second,sp,x,y,v.sx,v.sy,v.anchor_x,v.anchor_y,(u32(v.alpha)<<24)|(u32(v.r)<<16)|(u32(v.g)<<8)|v.b,v.rot_z,v.blend_mode==1);
        }
        for(const auto&c:v.children)self(self,c,x,y);
    };
    for(int side=0;side<2;++side){const auto&p=g_dialogue_portraits[side];if(!p.active)continue;
        int ch=side?g_opp_sel:g_char_sel;
        for(const auto*v:{&p.body,&p.face}){
            if(!v->visible||v->alpha<=0)continue;
            for(int ei=0;ei<int(g_plf[ch].size());++ei)for(const auto&sp:g_plf[ch][ei].sprites)if(int(sp.id)==v->sprite){
                auto t=g_plf_tex[ch].find(ei);if(t==g_plf_tex[ch].end())continue;
                emit_quad(v->layer+4,t->second,sp,(side?1280.f:0.f)+v->x,v->y,side?-v->sx:v->sx,v->sy,
                    v->anchor_x,v->anchor_y,(u32(v->alpha)<<24)|(u32(v->r)<<16)|(u32(v->g)<<8)|v->b,v->rot_z);
            }
        }
    }
    if(g_dialogue_name.stream)name(name,g_dialogue_name,0,0);
    for(const auto&bubble:g_dialogue_bubble.items){
    const auto*page=bubble.page;const auto&vm=bubble.vm;const float width=bubble.width;
    auto draw=[&](auto&& self,const th19::AnmVm& v,float x,float y)->void {
        x+=v.x;y+=v.y;
        auto it=g_front_sprites.find(v.sprite);
        if(v.visible&&v.alpha>0&&it!=g_front_sprites.end()){
            auto ti=g_front_tex.find(it->second.first);
            if(ti!=g_front_tex.end())emit_quad(50,ti->second,g_front[it->second.first].sprites[it->second.second],x,y,v.sx,v.sy,v.anchor_x,v.anchor_y,(u32(v.alpha)<<24)|(u32(v.r)<<16)|(u32(v.g)<<8)|v.b,v.rot_z,v.blend_mode==1);
            if(ti!=g_front_tex.end()&&!g_skip_present&&!g_draw.empty()){auto& q=g_draw.back();q.u0+=.5f/ti->second.width;q.u1-=.5f/ti->second.width;}
        }
        for(const auto&c:v.children)self(self,c,x,y);
    };
    draw(draw,vm,page->bubble_x*2,page->bubble_y*2);
    const float left=page->bubble_x*2+(page->side==0?16.f:-48.f-width);
    auto textGlyph=[&](const std::string&str,float px,const th19::AnmVm&v,float offset){
        if(!v.stream||!v.visible||v.alpha<=0)return;FontTex ft;if(!render_text(str,px,ft,true))return;
        th19::AnmSprite sp;sp.w=ft.w;sp.h=ft.h;
        emit_quad(51,ft.tex,sp,left+v.x+offset*v.sx,page->bubble_y*2+v.y+2,v.sx,v.sy,1,1,u32(v.alpha)<<24,0);
    };
    for(i32 i=0;i<page->line_count;++i)textGlyph(page->lines[i],32,bubble.body[i],0);
    // 0x4cd610 doubles offset and glyph advance; 0x4b3254 creates the 15px ruby font.
    // Keep individual advances, including spaces, instead of measuring a proportional string.
    for(i32 i=0;i<page->annotation_count;++i){const auto&a=page->annotations[i];if(a.line<0||a.line>=4)continue;
        const std::string str=a.text?a.text:"";int glyph=0;
        for(size_t n=0;n<str.size();++glyph){unsigned char c=str[n];size_t len=c<128?1:c<224?2:c<240?3:4;
            textGlyph(str.substr(n,len),15,bubble.ruby[a.line],float(2*a.x+2*a.advance*glyph));n+=len;}
    }
    }

}

void load_ecl_engine();   // 前置声明（定义在 start_game 之后）

// 加载选中角色本体 plXX.anm + world01.anm，绑定 idle 动画，重置出生位置。
void start_game(bool preserveMusic) {
    g_screen_shake.reset();
    for(int side=0;side<2;++side){g_focus_down[side]=g_focus_active[side]=false;g_focus_vm[side]={};}
    if(g_focus_entries.empty()&&load_anm("/effect.anm",g_focus_bytes,g_focus_entries)){
        decode_textures(g_focus_entries,g_focus_bytes,g_focus_tex);
        for(int ei=0;ei<int(g_focus_entries.size());++ei){const auto&e=g_focus_entries[ei];
            for(int si=0;si<int(e.sprites.size());++si)g_focus_sprites[e.sprites[si].id]={ei,si};
            for(const auto&sc:e.scripts)g_focus_scripts[sc.id]={g_focus_bytes.data()+e.base+sc.offset,u32(g_focus_bytes.size()-e.base-sc.offset),&e.scripts};
        }
    }
    init_charge_visuals();
    if(!preserveMusic)g_story_music=-1;
    g_story_final_music=-1;
    g_cpu_control={};g_cpu_left_control={};g_cpu_left_bits=0;g_cpu_automatic=true;g_cpu_frames=g_cpu_steers=g_cpu_story_events=0;std::fill(std::begin(g_cpu_releases),std::end(g_cpu_releases),0);g_cpu_story.reset(g_story_session.stage_index()+1,g_story_difficulty);g_cpu_difficulty.reset(g_story_difficulty,0);g_attacks[0]={};g_attacks[1]={};g_player_projectiles.clear();g_player_hit_effects.clear();g_shoot_frame=g_opp_shoot_frame=0;g_opponent_controls=0;g_opp_direction=0;g_shot_table={};g_attacks[0].charge_attack={};g_attacks[0].charge_area={};g_attacks[0].charge_area_vm={};g_attacks[0].tsukasa_skill_back={};g_attacks[0].ran_skill={};g_attacks[0].reimu_skills.clear();g_attacks[0].marisa_skills.clear();g_attacks[0].marisa_skill_arrivals=0;g_reflected_flights.clear();g_reflection_arrivals=0;g_reflection_rng={};g_attacks[0].chiyari_skill={};g_attacks[0].chiyari_skill_spawns=0;g_attacks[0].skill_damage_circles.clear();
    {char path[32];std::snprintf(path,sizeof path,"/pl%02d.sht",g_char_sel);std::vector<u8> data;if(read_asset(path,data))g_shot_table.load(data);}

    for(int side=0;side<2;++side){g_charge[side]=th19::ChargeGauge{};char shot[32];std::snprintf(shot,sizeof(shot),"/pl%02d.sht",side?g_opp_sel:g_char_sel);std::vector<u8> data;if(read_asset(shot,data))g_charge[side].load(data);}

    char path[32]; std::snprintf(path, sizeof path, "/pl%02d.anm", g_char_sel);
    for (const auto& kv : g_player_tex) gfx.destroy(kv.second.handle);
    g_player_bytes.clear(); g_player.clear(); g_player_tex.clear();
    g_player_sprites.clear(); g_player_scripts.clear();
    if (load_anm(path, g_player_bytes, g_player)) {
        decode_textures(g_player, g_player_bytes, g_player_tex);
        for (i32 ei = 0; ei < (i32)g_player.size(); ++ei)
            for (i32 si = 0; si < (i32)g_player[ei].sprites.size(); ++si)
                g_player_sprites[g_player[ei].sprites[si].id] = {ei, si};
        for (i32 ei = 0; ei < (i32)g_player.size(); ++ei) {
            const auto& e = g_player[ei];
            for (const auto& sc : e.scripts) {
                th19::AnmScriptLoc loc;
                loc.stream = g_player_bytes.data() + e.base + sc.offset;
                loc.len = u32(g_player_bytes.size()) - e.base - sc.offset;
                loc.table = &e.scripts;
                g_player_scripts[sc.id] = loc;
            }
        }
        {int ordinal=0;for(const auto&e:g_player)for(const auto&sc:e.scripts){if(sc.id<0)g_player_scripts[ordinal]=g_player_scripts.at(sc.id);++ordinal;}}
        // 绑定前向动画（脚本 -36：8 帧 seq 循环）
        g_p1_dir = 0;
        bind_player_dir(0);
    }
    load_world(g_vs_mode?g_vs_scene.background:th19::STORY_ROUTES[g_char_sel].stages[g_story_session.stage_index()].background);
    // Use the opponent from this route's current MSG entry, including after a restart.
    for (const auto& kv : g_opp_tex) gfx.destroy(kv.second.handle);
    g_opp_tex.clear(); g_opp_sprites.clear(); g_opp.clear(); g_opp_bytes.clear();
    g_opp_vm = th19::AnmVm();
    {
        std::snprintf(path, sizeof path, "/pl%02d.anm", g_opp_sel);
        if (load_anm(path, g_opp_bytes, g_opp)) {
            decode_textures(g_opp, g_opp_bytes, g_opp_tex);
            for (i32 ei = 0; ei < (i32)g_opp.size(); ++ei)
                for (i32 si = 0; si < (i32)g_opp[ei].sprites.size(); ++si)
                    g_opp_sprites[g_opp[ei].sprites[si].id] = {ei, si};
            bind_opp_dir(0);   // idle 前向
        }
    }
    if (g_front_sprites.empty()) {
        for (i32 ei = 0; ei < (i32)g_front.size(); ++ei)
            for (i32 si = 0; si < (i32)g_front[ei].sprites.size(); ++si)
                g_front_sprites[g_front[ei].sprites[si].id] = {ei, si};
    }
    g_bg_scroll = 0.0f;
    g_spell_show = false;
    g_big_sprite = -1;
    g_p1_x = 0.0f; g_p1_y = 400.0f;
    g_p2_x = 0.0f; g_p2_y = 400.0f;
    init_player_options();
    load_ecl_engine();
}

// 递归加载一个 ECL 程序（含 include 依赖），去重后接入 vm + world。
static void ecl_load_program(EclField& field,const char* name) {
    for (const auto& n : field.loaded) if (n == name) return;
    std::vector<u8> bytes;
    if (!read_asset(name, bytes)) return;
    auto prog = std::make_unique<th19::EclProgram>();
    if (!prog->load(bytes.data(), u32(bytes.size()))) return;
    field.loaded.push_back(name);
    for (const auto& inc : prog->includes) {
        std::string incname = "/" + inc;
        ecl_load_program(field,incname.c_str());
    }
    field.vm.attach_program(*prog);
    field.world.attach_program(*prog);
    field.programs.push_back(std::move(prog));
}

void start_story_actor(){
    g_story_field=EclField{};g_story_animations.clear();
    char path[32];std::snprintf(path,sizeof path,"/pl%02d.ecl",g_opp_sel);
    ecl_load_program(g_story_field,path);
    g_story_field.world.message_active=true;
    g_story_field.world.spawn_enemy("MsgBoss",0,128,200,0,0);
    for(const auto&kv:g_boss_tex)gfx.destroy(kv.second.handle);
    g_boss_bytes.clear();g_boss_entries.clear();g_boss_tex.clear();g_boss_sprites.clear();g_boss_scripts.clear();
    std::snprintf(path,sizeof path,"/pl%02db.anm",g_opp_sel);
    if(load_anm(path,g_boss_bytes,g_boss_entries)){
        decode_textures(g_boss_entries,g_boss_bytes,g_boss_tex);
        for(int ei=0;ei<int(g_boss_entries.size());++ei){const auto&e=g_boss_entries[ei];
            for(int si=0;si<int(e.sprites.size());++si)g_boss_sprites[e.sprites[si].id]={ei,si};
            for(const auto&sc:e.scripts)g_boss_scripts[sc.id]={g_boss_bytes.data()+e.base+sc.offset,u32(g_boss_bytes.size()-e.base-sc.offset),&e.scripts};
        }
    }
}
void tick_story_actor(){
    auto&w=g_story_field.world;
    w.message_active=g_screen==SCR_DIALOG&&!g_story_playback.finished;
    w.message_signal=g_story_playback.signal;w.tick_wave();
    for(int sound:w.sound_events)play_sfx(sound);w.sound_events.clear();
    for(const auto&e:w.enemies)for(int slot=0;slot<16;++slot){
        const auto&binding=e.animations[slot];
        if(!e.active||binding.script<0||(binding.file!=3&&binding.file!=1))continue;
        const auto&entries=binding.file==3?g_boss_entries:g_focus_entries;
        const auto&scripts=binding.file==3?g_boss_scripts:g_focus_scripts;
        int ordinal=binding.script;
        int index=0,id=-1;for(const auto&entry:entries)for(const auto&sc:entry.scripts)if(index++==ordinal)id=sc.id;
        auto it=scripts.find(id);if(it==scripts.end())continue;
        auto&a=g_story_animations[e.id*16+slot];
        if(a.script!=id||a.generation!=binding.generation){a={};a.script=id;a.generation=binding.generation;a.mist.seed+=e.id*31+slot*7919;a.vm.bind_entry(it->second.stream,it->second.len,&scripts);}
        if(a.interrupt_generation!=binding.interrupt_generation){a.vm.interrupt_all(binding.interrupt_label);a.interrupt_generation=binding.interrupt_generation;}
        a.vm.tick();
        if(a.vm.special_effect==1)a.mist.tick(binding.rotation,a.vm.effect_angle,a.vm.ivar[0]);
    }
}
void draw_story_actor(){
    g_actor_clip=0;
    for(const auto&e:g_story_field.world.enemies)for(int slot=0;slot<16;++slot){
        if(!e.active)continue;
        const auto&binding=e.animations[slot];
        auto a=g_story_animations.find(e.id*16+slot);if(a==g_story_animations.end())continue;
        const auto&v=a->second.vm;
        if(binding.file==1&&v.special_effect==1){
            auto s=g_focus_sprites.find(62);if(s==g_focus_sprites.end())continue;
            auto t=g_focus_tex.find(s->second.first);if(t==g_focus_tex.end())continue;
            const auto&mist=a->second.mist;
            for(const auto&p:mist.particles){
                auto pos=mist.position(p);float progress=std::min(1.f,float(p.age)/(2*mist.duration));
                float scale=p.light?2-progress:.6f+1.8f*progress;
                int alpha=int((p.light?96:80)*std::min(1.f,float(p.age)/(p.light?40:10)));
                u32 rgb=p.light?(u32(u8(44-v.r))<<16)|(u32(u8(44-v.g))<<8)|u8(44-v.b):(u32(v.r)<<16)|(u32(v.g)<<8)|v.b;
                emit_quad(p.light?28:29,t->second,g_focus_entries[s->second.first].sprites[s->second.second],
                    GAME_F1_CX+(e.x+pos.x)*GAME_SCALE,GAME_FY0+(e.y+pos.y)*GAME_SCALE,
                    GAME_SCALE*scale,GAME_SCALE*scale,0,0,(u32(alpha)<<24)|rgb,0,p.light,p.light?1:2);
            }continue;
        }
        if(binding.file!=3)continue;
        auto s=g_boss_sprites.find(v.sprite);
        if(!v.visible||v.alpha<=0||s==g_boss_sprites.end())continue;
        auto t=g_boss_tex.find(s->second.first);if(t==g_boss_tex.end())continue;
        emit_quad(30,t->second,g_boss_entries[s->second.first].sprites[s->second.second],
            GAME_F1_CX+(e.x+v.x)*GAME_SCALE,GAME_FY0+(e.y+v.y)*GAME_SCALE,
            GAME_SCALE*e.sx*v.sx,GAME_SCALE*e.sy*v.sy,v.anchor_x,v.anchor_y,
            (u32(v.alpha)<<24)|(u32(v.r)<<16)|(u32(v.g)<<8)|v.b,v.rot_z+binding.rotation,v.blend_mode==1,v.blend_mode);
    }
    g_actor_clip=-1;
}

// 加载 ECL 出怪/弹幕引擎：enemy.anm + bullet.anm 贴图 + plXX.ecl/default/common/wave 程序，启动 main 波次流。
void load_ecl_engine() {
    g_spirit_flights.clear();g_spirit_clock=g_spirit_arrivals=g_spirit_overflow=0;
    g_spirit_rng.state=g_game_seed?g_game_seed:1;
    if (g_enemy.empty()) {
        if (load_anm("/enemy.anm", g_enemy_bytes, g_enemy)) {
            decode_textures(g_enemy, g_enemy_bytes, g_enemy_tex);
            for (i32 ei = 0; ei < (i32)g_enemy.size(); ++ei)
                for (i32 si = 0; si < (i32)g_enemy[ei].sprites.size(); ++si)
                    g_enemy_sprites[g_enemy[ei].sprites[si].id] = {ei, si};
        }
    }
    if (g_bullet.empty()) {
        if (load_anm("/bullet.anm", g_bullet_bytes, g_bullet)) {
            decode_textures(g_bullet, g_bullet_bytes, g_bullet_tex);
            for(const auto&e:g_bullet)for(const auto&sc:e.scripts)g_bullet_scripts[sc.id]={g_bullet_bytes.data()+e.base+sc.offset,u32(g_bullet_bytes.size()-e.base-sc.offset),&e.scripts};
            for (i32 ei = 0; ei < (i32)g_bullet.size(); ++ei)
                for (i32 si = 0; si < (i32)g_bullet[ei].sprites.size(); ++si)
                    {const auto&sp=g_bullet[ei].sprites[si];g_bullet_sprites[sp.id] = {ei, si};g_bullet_sprite_sizes[sp.id]={sp.w,sp.h};}
        }
    }
    for(int side=0;side<2;++side){g_bombs[side]={};g_bomb_area[side]={};g_bomb_back[side]={};g_bomb_visual_tail[side]=0;for(auto&v:g_bomb_extra[side])v={};}
    g_battle_report_frame=-1;g_stage_deaths[0]=g_stage_deaths[1]=0;for(auto&counts:g_vs_attack_counts)counts[0]=counts[1]=0;
    ++g_ecl_generation;g_attack_warning={};g_attack_warning_vm={};
    for(int side=0;side<2;++side){
        auto& field=g_ecl_fields[side];
        field.vm=th19::EclVm{};field.world=th19::GameWorld{};
        field.programs.clear();field.loaded.clear();
        // Development seed, not a reconstruction of the native shared RNG stream.
        field.world.wave_schedule.seed(g_game_seed);field.world.extra_rng.state=g_game_seed?g_game_seed:1;
        field.world.difficulty=g_story_difficulty;field.world.bullet_scripts=&g_bullet_scripts;field.world.bullet_sprite_sizes=&g_bullet_sprite_sizes;
        // Native 0x558419 initializes story progress to 1; 0x553c47 increments
        // it between stages. 0x51d4e0 uses this one-based value for rewards.
        field.world.reward_mode=0;
        field.world.reward_stage=g_story_session.stage_index()+1;
        char path[32];
        std::snprintf(path,sizeof path,"/pl%02d.ecl",side?g_opp_sel:g_char_sel);
        ecl_load_program(field,path);
        ecl_load_program(field,"/default.ecl");
        ecl_load_program(field,"/common.ecl");
        field.vm.attach_world(&field.world);
        if(!field.programs.empty())field.world.attach_enemy_program(*field.programs[0]);
        field.vm.start("main");
    }
    g_ecl_ready = true;
}

// Read-only renderer diagnostics: requested main slots, resolved scripts,
// textured root draws, unresolved slots. Reset on each collection pass.
std::array<i32,4> g_actor_visual_counts[2]{};
// ECL positions are local to each player field, never relative to the full screen.
struct EnemyAnimation {i32 script=-1,frame=-1;u32 generation=0,interrupt_generation=0;th19::AnmVm vm;};
std::unordered_map<th19::u64,EnemyAnimation> rb_draw_ecl_objects_animations[2];
std::unordered_map<u32,th19::LaserVisual> rb_draw_ecl_objects_laserHeads[2];
std::unordered_map<u32,EnemyAnimation> rb_draw_ecl_objects_curveVisuals[2],rb_draw_ecl_objects_curveOrigins[2];
std::unordered_map<u32,EnemyAnimation> rb_draw_ecl_objects_extraAnimations[2];
EnemyAnimation rb_draw_ecl_objects_spellNames[2],rb_draw_ecl_objects_spellRings[2],rb_draw_ecl_objects_spellUnderlines[2];
u32 rb_draw_ecl_objects_generation=0;
void draw_ecl_objects() {
    if (!g_ecl_ready) return;

    
    static std::unordered_map<i32,th19::AnmScriptLoc> scripts;
    
    
    
    
    
    static std::unordered_map<i32,th19::AnmScriptLoc> spellAsciiScripts;
    struct AttackAsset {std::vector<u8> bytes;std::vector<th19::AnmEntry> entries;std::map<i32,TextureAllocation> textures;std::map<u32,std::pair<i32,i32>> sprites;std::unordered_map<i32,th19::AnmScriptLoc> scripts;bool attempted=false;};
    static std::map<std::string,AttackAsset> attackAssets;
    
    if(rb_draw_ecl_objects_generation!=g_ecl_generation){rb_draw_ecl_objects_animations[0].clear();rb_draw_ecl_objects_animations[1].clear();rb_draw_ecl_objects_laserHeads[0].clear();rb_draw_ecl_objects_laserHeads[1].clear();rb_draw_ecl_objects_curveVisuals[0].clear();rb_draw_ecl_objects_curveVisuals[1].clear();rb_draw_ecl_objects_curveOrigins[0].clear();rb_draw_ecl_objects_curveOrigins[1].clear();rb_draw_ecl_objects_extraAnimations[0].clear();rb_draw_ecl_objects_extraAnimations[1].clear();rb_draw_ecl_objects_spellNames[0]={};rb_draw_ecl_objects_spellNames[1]={};rb_draw_ecl_objects_spellRings[0]={};rb_draw_ecl_objects_spellRings[1]={};rb_draw_ecl_objects_spellUnderlines[0]={};rb_draw_ecl_objects_spellUnderlines[1]={};rb_draw_ecl_objects_generation=g_ecl_generation;}
    if(spellAsciiScripts.empty())for(const auto&e:g_ascii)for(const auto&sc:e.scripts)
        spellAsciiScripts[sc.id]={g_ascii_bytes.data()+e.base+sc.offset,u32(g_ascii_bytes.size()-e.base-sc.offset),&e.scripts};
    if(scripts.empty())for(const auto&e:g_enemy)for(const auto&sc:e.scripts)scripts[sc.id]={g_enemy_bytes.data()+e.base+sc.offset,u32(g_enemy_bytes.size()-e.base-sc.offset),&e.scripts};
    for(int side=0;side<2;++side){
    g_actor_clip=side;
    g_actor_visual_counts[side]={};
    auto& world=g_ecl_fields[side].world;
    const float cx=side?GAME_F2_CX:GAME_F1_CX;
    if(world.spell.active){
        auto&asset=attackAssets[world.spell.animation_file];if(!asset.attempted){asset.attempted=true;
            if(load_anm(("/"+world.spell.animation_file).c_str(),asset.bytes,asset.entries)){
                decode_textures(asset.entries,asset.bytes,asset.textures);
                for(int ei=0;ei<int(asset.entries.size());++ei){const auto&entry=asset.entries[ei];
                    for(int si=0;si<int(entry.sprites.size());++si)asset.sprites[entry.sprites[si].id]={ei,si};
                    for(const auto&sc:entry.scripts)asset.scripts[sc.id]={asset.bytes.data()+entry.base+sc.offset,u32(asset.bytes.size()-entry.base-sc.offset),&entry.scripts};}}}
        auto&bg=rb_draw_ecl_objects_animations[side][0];if(bg.generation!=world.spell.generation||!bg.vm.stream){bg={};bg.generation=world.spell.generation;
            auto it=asset.scripts.find(0);if(it!=asset.scripts.end())bg.vm.bind_entry(it->second.stream,it->second.len,&asset.scripts);}
        if(!g_paused&&bg.frame!=g_frame){bg.vm.tick();bg.frame=g_frame;}
        auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy)->void{
            if(!v.visible)return;x+=(v.x+v.child_x)*sx;y+=(v.y+v.child_y)*sy;
            auto it=asset.sprites.find(v.sprite);if(it!=asset.sprites.end()){auto ti=asset.textures.find(it->second.first);
                // pl13b script3 uses ANM303 mode2 (reverse subtraction).
                // Passing only the additive boolean rendered its black texture as an opaque plane.
                if(ti!=asset.textures.end()){
                    const auto&sp=asset.entries[it->second.first].sprites[it->second.second];
                    if(v.geometry==600&&v.segments>=2){
                        // 4bcbc0 mode9: primary colour on outer edge, secondary on inner
                        // only when ANM423 requests it. Used by pl09b/pl11b backgrounds.
                        DrawCmd q{};q.layer=25;q.clip_side=side;q.handle=ti->second.handle;q.mesh_blend=v.blend_mode;q.mesh_fog_enabled=false;
                        q.address_u=v.address_u;q.address_v=v.address_v;
                        const int n=std::clamp(v.segments,2,256);
                        const float outer=(v.sy+v.sx*.5f)*sx,inner=(v.sy-v.sx*.5f)*sy;
                        const u32 primary=(u32(v.alpha)<<24)|(u32(v.r)<<16)|(u32(v.g)<<8)|v.b;
                        auto vertex=[&](int i,bool outside){float a=v.rot_z+6.28318530718f*i/(n-1),r=outside?outer:inner;
                            return SpriteVertex{{cx+GAME_SCALE*(x+std::cos(a)*r),GAME_FY0+GAME_SCALE*(y+std::sin(a)*r),0},1,outside?primary:v.color(),
                                {(sp.x+(outside?0:sp.w))/ti->second.width+v.u_offset,float(v.ivar[1])*i/(n-1)+v.v_offset}};};
                        for(int i=0;i<n-1;++i){auto a=vertex(i,true),b=vertex(i,false),c=vertex(i+1,true),d=vertex(i+1,false);q.mesh.insert(q.mesh.end(),{a,b,c,b,d,c});}
                        g_draw.push_back(std::move(q));
                    }else{
                        emit_quad(25,ti->second,sp,cx+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,GAME_SCALE*sx*v.sx,GAME_SCALE*sy*v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);
                        if(!g_skip_present){auto&q=g_draw.back();q.u0+=v.u_offset;q.u1+=v.u_offset;q.v0+=v.v_offset;q.v1+=v.v_offset;
                            q.address_u=v.address_u;q.address_v=v.address_v;}
                    }
                }}
            for(const auto&c:v.children)self(self,c,x,y,sx*v.sx,sy*v.sy);
        };draw(draw,bg.vm,0,0,1,1);
        // 4f699c: effect.anm6 creates textured ring4/5, positioned at the spell host.
        auto&ring=rb_draw_ecl_objects_spellRings[side];bool newRing=ring.generation!=world.spell.generation||!ring.vm.stream;
        const bool ringTick=!g_paused&&ring.frame!=g_frame;
        if(ring.generation!=world.spell.generation||!ring.vm.stream){ring={};ring.generation=world.spell.generation;
            auto it=g_focus_scripts.find(6);if(it!=g_focus_scripts.end()){
                ring.vm.bind_entry(it->second.stream,it->second.len,&g_focus_scripts);ring.vm.tick();ring.frame=g_frame;
                for(auto&child:ring.vm.children)child.ivar[2]=world.spell.timeout;
            }}
        if(!g_paused&&ring.frame!=g_frame){ring.vm.tick();ring.frame=g_frame;}
        for(const auto&host:world.enemies)if(host.active&&host.id==world.spell.host_id){
            // 4f6e90 follows the live boss with a .05 interpolation each update.
            if(newRing){ring.vm.x=host.x;ring.vm.y=host.y;}
            else if(ringTick){ring.vm.x+=(host.x-ring.vm.x)*.05f;ring.vm.y+=(host.y-ring.vm.y)*.05f;}
            for(const auto&v:ring.vm.children){if(!v.visible||v.geometry!=600||v.segments<2)continue;
                auto si=g_focus_sprites.find(v.sprite);if(si==g_focus_sprites.end())continue;
                auto ti=g_focus_tex.find(si->second.first);if(ti==g_focus_tex.end())continue;
                const auto&sp=g_focus_entries[si->second.first].sprites[si->second.second];
                DrawCmd q{};q.layer=30;q.clip_side=side;q.handle=ti->second.handle;q.mesh_blend=v.blend_mode;q.mesh_fog_enabled=false;
                // 4bcbc0: N pairs, duplicate closing pair; radius=sy, half-width=sx/2.
                const int n=std::clamp(v.segments,2,256);const float outer=v.sy+v.sx*.5f,inner=v.sy-v.sx*.5f;
                auto vertex=[&](int i,bool outside){float a=v.rot_z+6.28318530718f*i/(n-1),r=outside?outer:inner;
                    return SpriteVertex{{cx+GAME_SCALE*(ring.vm.x+v.x+std::cos(a)*r),GAME_FY0+GAME_SCALE*(ring.vm.y+v.y+std::sin(a)*r),0},1,v.color(),
                        {(sp.x+(outside?0:sp.w))/ti->second.width+v.u_offset,float(v.ivar[1])*i/(n-1)+v.v_offset}};};
                for(int i=0;i<n-1;++i){auto a=vertex(i,true),b=vertex(i,false),c=vertex(i+1,true),d=vertex(i+1,false);q.mesh.insert(q.mesh.end(),{a,b,c,b,d,c});}
                g_draw.push_back(std::move(q));
            }break;
        }
        // 4f67c0 creates text.anm2+side at field centre; original script
        // owns right/top anchor, fade, scale and flight into the upper-right corner.
        auto&name=rb_draw_ecl_objects_spellNames[side];
        if(name.generation!=world.spell.generation||!name.vm.stream){name={};name.generation=world.spell.generation;
            auto it=g_story_text_scripts.find(2+side);if(it!=g_story_text_scripts.end())name.vm.bind_entry(it->second.stream,it->second.len,&g_story_text_scripts);}
        if(!g_paused&&name.frame!=g_frame){
            // 4f6e90: after120 frames, fade when player reaches upper96;
            // restore only below128, avoiding flicker at a single threshold.
            if(name.vm.time>=120){if(name.script!=1&&world.player_y<96){name.vm.interrupt(3);name.script=1;}
                else if(name.script==1&&world.player_y>128){name.vm.interrupt(2);name.script=0;}}
            name.vm.tick();name.frame=g_frame;
        }
        // 4f6886 creates ascii1280.anm0 alongside text.anm2+side: sprite285 underline.
        auto&underline=rb_draw_ecl_objects_spellUnderlines[side];
        if(underline.generation!=world.spell.generation||!underline.vm.stream){underline={};underline.generation=world.spell.generation;
            auto it=spellAsciiScripts.find(0);if(it!=spellAsciiScripts.end())underline.vm.bind_entry(it->second.stream,it->second.len,&spellAsciiScripts);}
        if(!g_paused&&underline.frame!=g_frame){
            if(underline.vm.time>=120&&underline.script!=name.script){underline.vm.interrupt(name.script==1?3:2);underline.script=name.script;}
            underline.vm.tick();underline.frame=g_frame;
        }
        const auto&uv=underline.vm;
        if(uv.visible&&uv.alpha>0)for(int ei=0;ei<int(g_ascii.size());++ei)for(const auto&sp:g_ascii[ei].sprites)if(int(sp.id)==uv.sprite){
            auto ti=g_ascii_tex.find(ei);if(ti!=g_ascii_tex.end())emit_quad(38,ti->second,sp,cx+uv.x,GAME_FY0+uv.y,uv.sx,uv.sy,uv.anchor_x,uv.anchor_y,uv.color(),uv.rot_z,uv.blend_mode==1,uv.blend_mode);
        }
        const auto&nv=name.vm;FontTex ft;
        // 4cd480 font0:16.5 -> integer16; native text renderer uses doubled glyphs.
        if(nv.visible&&nv.alpha>0&&render_text(nativeSpellText(world.spell.name_bytes),32,ft)){
            th19::AnmSprite sp;sp.w=ft.w;sp.h=ft.h;
            emit_quad(39,ft.tex,sp,cx+nv.x,GAME_FY0+nv.y,nv.sx,nv.sy,nv.anchor_x,nv.anchor_y,nv.color(),nv.rot_z);
        }
    }
    for(const auto&f:world.extra_flights){
        auto&asset=attackAssets[f.file];if(!asset.attempted){asset.attempted=true;
            if(load_anm(("/"+f.file).c_str(),asset.bytes,asset.entries)){
                decode_textures(asset.entries,asset.bytes,asset.textures);
                for(int ei=0;ei<int(asset.entries.size());++ei){const auto&entry=asset.entries[ei];
                    for(int si=0;si<int(entry.sprites.size());++si)asset.sprites[entry.sprites[si].id]={ei,si};
                    for(const auto&sc:entry.scripts)asset.scripts[sc.id]={asset.bytes.data()+entry.base+sc.offset,u32(asset.bytes.size()-entry.base-sc.offset),&entry.scripts};}}}
        auto&v=rb_draw_ecl_objects_extraAnimations[side][f.id];
        if(v.script!=f.script){v={};v.script=f.script;auto it=asset.scripts.find(f.script);
            if(it!=asset.scripts.end())v.vm.bind_entry(it->second.stream,it->second.len,&asset.scripts);}
        while(v.frame<f.age){v.vm.tick();++v.frame;}
        th19::ReflectedBulletFlight curve;curve.frame=f.age;curve.duration=f.duration;
        curve.from={(side?GAME_F1_CX:GAME_F2_CX)+GAME_SCALE*f.source_x,GAME_FY0+GAME_SCALE*f.source_y};
        curve.to={cx+GAME_SCALE*f.x,GAME_FY0+GAME_SCALE*f.y};
        curve.start_tangent={f.tangent_start,(f.character<=1||f.character==4||f.character==5||f.character==6)?-400.f:200.f};curve.end_tangent={f.tangent_end,(f.character<=1||f.character==4||f.character==5||f.character==6)?100.f:-300.f};
        auto pos=curve.position();g_actor_clip=-1;
        auto draw=[&](auto&&self,const th19::AnmVm&vm,float x,float y)->void{
            if(!vm.visible)return;auto it=asset.sprites.find(vm.sprite);
            if(it!=asset.sprites.end()){auto tx=asset.textures.find(it->second.first);if(tx!=asset.textures.end()){
                const float scale=(f.character<=4)?.8f*GAME_SCALE:1.f;
                const u32 color=f.character==0?0xc0ff8080u:(f.character>=1&&f.character<=4)?(vm.color()&0xffffff)|0xc0000000u:vm.color();
                emit_quad(34,tx->second,asset.entries[it->second.first].sprites[it->second.second],x+vm.x,y+vm.y,
                    scale*vm.sx,scale*vm.sy,vm.anchor_x,vm.anchor_y,color,vm.rot_z,vm.blend_mode==1,vm.blend_mode);
            }}for(const auto&child:vm.children)self(self,child,x+vm.x,y+vm.y);
        };draw(draw,v.vm,pos.x,pos.y);g_actor_clip=side;
    }
    for (const auto& e : world.enemies) {
        if(!e.active||e.sprite<0)continue;
        for(int slot=0;slot<16;++slot){const auto&definition=e.animations[slot];
        // Wave ECL ANIM slot 2 resolves to enemy.anm. Never reinterpret another file's IDs.
        if(definition.script<0)continue;
        if(slot==0)++g_actor_visual_counts[side][0];
        auto*scriptMap=&scripts;const auto*spriteMap=&g_enemy_sprites;const auto*textures=&g_enemy_tex;const auto*entries=&g_enemy;
        const int fileIndex=definition.file-2;
        // 50d380: slot1 is the shared effect.anm resource, not a user ANIM index.
        if(definition.file!=1&&(fileIndex<0||fileIndex>=int(e.animation_files.size()))){if(slot==0)++g_actor_visual_counts[side][3];continue;}
        const std::string file=definition.file==1?"effect.anm":e.animation_files[fileIndex];
        if(file!="enemy.anm"){
            auto&asset=attackAssets[file];if(!asset.attempted){asset.attempted=true;
                if(load_anm(("/"+file).c_str(),asset.bytes,asset.entries)){
                    decode_textures(asset.entries,asset.bytes,asset.textures);
                    for(int ei=0;ei<int(asset.entries.size());++ei){const auto&entry=asset.entries[ei];
                        for(int si=0;si<int(entry.sprites.size());++si)asset.sprites[entry.sprites[si].id]={ei,si};
                        for(const auto&sc:entry.scripts)asset.scripts[sc.id]={asset.bytes.data()+entry.base+sc.offset,u32(asset.bytes.size()-entry.base-sc.offset),&entry.scripts};}
                }}
            scriptMap=&asset.scripts;spriteMap=&asset.sprites;textures=&asset.textures;entries=&asset.entries;
        }
        auto&anim=rb_draw_ecl_objects_animations[side][(th19::u64(e.id)<<4)|slot];
        int ordinal=definition.script;
        if(anim.script!=ordinal||anim.generation!=definition.generation){int id=-1,index=0;
            for(const auto&entry:*entries)for(const auto&sc:entry.scripts)if(index++==ordinal)id=sc.id;
            auto it=scriptMap->find(id);if(it==scriptMap->end()){if(slot==0)++g_actor_visual_counts[side][3];continue;}anim={};anim.script=ordinal;anim.generation=definition.generation;anim.vm.bind_entry(it->second.stream,it->second.len,scriptMap);}
        if(slot==0)++g_actor_visual_counts[side][1];
        if(!g_paused&&anim.frame!=g_frame){
            if(anim.interrupt_generation!=definition.interrupt_generation){anim.vm.interrupt_all(definition.interrupt_label);anim.interrupt_generation=definition.interrupt_generation;}
            anim.vm.tick();anim.frame=g_frame;
        }
        auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy)->void{
            if(!v.visible)return;x+=(v.x+v.child_x)*sx;y+=(v.y+v.child_y)*sy;
            // Enemy update 0x5089d0 applies ANM424 to the host motion angle.
            const float heading=e.polar?e.angle:std::atan2(e.vy,e.vx);
            const auto facing=th19::nativeEnemyFacing(v.rotation_mode,(&v==&anim.vm&&definition.rotation_set)?definition.rotation:v.rot_z,v.sx,heading);
            const float rotation=facing.angle;
            auto it=spriteMap->find(v.sprite);
            if(it!=spriteMap->end()){auto ti=textures->find(it->second.first);if(ti!=textures->end()){
                if(slot==0 && &v==&anim.vm && v.alpha>0 && sx*v.sx!=0 && sy*v.sy!=0)++g_actor_visual_counts[side][2];
                // Preserve native ANM order: body layer7 maps to31; muzzle
                // mist layer6 must remain below it, including character-bank children.
                const int layer=24+v.layer;
                emit_quad(layer,ti->second,(*entries)[it->second.first].sprites[it->second.second],cx+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,GAME_SCALE*sx*facing.scale_x,GAME_SCALE*sy*v.sy,v.anchor_x,v.anchor_y,slot==0&&e.feedback_color?e.feedback_color:v.color(),rotation,v.blend_mode==1);
                if(!g_skip_present&&!g_draw.empty()){auto&q=g_draw.back();q.mesh_blend=v.blend_mode;q.u0+=v.u_offset;q.u1+=v.u_offset;q.v0+=v.v_offset;q.v1+=v.v_offset;
                    if(v.render_mode==8){
                        // 4c6910: scaled sprite plane, ANM437 rotation order, then host translation.
                        static constexpr int order[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
                        auto point=[&](float u,float t){float ax=v.anchor_x==1?0:v.anchor_x==2?1:.5f,ay=v.anchor_y==1?0:v.anchor_y==2?1:.5f;
                            th19::Vec3 p{(u-ax)*q.sw*q.sx,(t-ay)*q.sh*q.sy,0};
                            for(int axis:order[std::clamp(v.rotation_order,0,5)]){float a=axis==0?v.rot_x:axis==1?v.rot_y:rotation,c=std::cos(a),s=std::sin(a);auto old=p;
                                if(axis==0){p.y=old.y*c-old.z*s;p.z=old.y*s+old.z*c;}
                                if(axis==1){p.x=old.x*c+old.z*s;p.z=-old.x*s+old.z*c;}
                                if(axis==2){p.x=old.x*c-old.y*s;p.y=old.x*s+old.y*c;}}
                            return SpriteVertex{{q.x+p.x,q.y+p.y,0},1,q.tint,{q.u0+(q.u1-q.u0)*u,q.v0+(q.v1-q.v0)*t}};};
                        auto a=point(0,0),b=point(1,0),c=point(0,1),d=point(1,1);q.mesh={a,b,c,b,d,c};q.mesh_fog_enabled=false;
                    }
                }
            }}
            for(const auto&c:v.children)self(self,c,x,y,sx*v.sx,sy*v.sy);
        };draw(draw,anim.vm,e.x,e.y,slot==0?e.sx:1,slot==0?e.sy:1);
        // pl16b 13/14 are procedural circles, not texture sprites.
        draw_procedural_node(anim.vm,side,e.x,e.y,slot==0?e.sx:1,slot==0?e.sy:1);
        }
    }
    // Retire handles with their enemy rather than retaining every historical spawn.
    for(auto it=rb_draw_ecl_objects_extraAnimations[side].begin();it!=rb_draw_ecl_objects_extraAnimations[side].end();){
        bool liveFlight=false;for(const auto&f:world.extra_flights)if(f.id==it->first){liveFlight=true;break;}
        if(!liveFlight)it=rb_draw_ecl_objects_extraAnimations[side].erase(it);else ++it;
    }
    std::unordered_map<u32,bool> live;for(const auto&f:world.extra_flights)live[f.id]=true;for(const auto&e:world.enemies)if(e.active)live[e.id]=true;
    for(auto it=rb_draw_ecl_objects_animations[side].begin();it!=rb_draw_ecl_objects_animations[side].end();)if(it->first!=0&&!live.count(u32(it->first>>4)))it=rb_draw_ecl_objects_animations[side].erase(it);else ++it;

    for (const auto& s : world.spirits) {
        if (!s.active) continue;
        auto it = g_enemy_sprites.find(u32(s.type));
        if (it == g_enemy_sprites.end()) continue;
        const i32 ei = it->second.first;
        const auto& sp = g_enemy[ei].sprites[it->second.second];
        auto ti = g_enemy_tex.find(ei);
        if (ti == g_enemy_tex.end()) continue;
        emit_quad(31, ti->second, sp, cx + 2*s.x, 32 + 2*s.y, 1.0f, 1.0f, 0, 0, 0xffffffff, 0);
    }
    for(const auto&laser:world.lasers){
        if(laser.curved){
            auto*map=&g_bullet_scripts;const auto*sprites=&g_bullet_sprites;const auto*textures=&g_bullet_tex;const auto*entries=&g_bullet;
            if(!laser.curve_file.empty()){
                auto&asset=attackAssets[laser.curve_file];if(!asset.attempted){asset.attempted=true;
                    if(load_anm(("/"+laser.curve_file).c_str(),asset.bytes,asset.entries)){
                        decode_textures(asset.entries,asset.bytes,asset.textures);
                        for(int ei=0;ei<int(asset.entries.size());++ei){const auto&e=asset.entries[ei];
                            for(int si=0;si<int(e.sprites.size());++si)asset.sprites[e.sprites[si].id]={ei,si};
                            for(const auto&sc:e.scripts)asset.scripts[sc.id]={asset.bytes.data()+e.base+sc.offset,u32(asset.bytes.size()-e.base-sc.offset),&e.scripts};}
                    }}map=&asset.scripts;sprites=&asset.sprites;textures=&asset.textures;entries=&asset.entries;
            }
            auto&anim=rb_draw_ecl_objects_curveVisuals[side][laser.id];
            if(anim.frame<0){const int script=laser.source.type==2?11:laser.source.type==1?325:145+laser.source.type;
                auto it=map->find(script);if(it==map->end())continue;
                anim.vm.bind_entry(it->second.stream,it->second.len,map);anim.vm.tick();anim.frame=0;}
            while(anim.frame<laser.age){anim.vm.tick();++anim.frame;}
            const auto&v=anim.vm;auto spIt=sprites->find(laser.source.type==0?524+laser.source.color:v.sprite);
            if(!g_skip_present&&v.visible&&spIt!=sprites->end()){
                auto tx=textures->find(spIt->second.first);if(tx!=textures->end()){
                    const auto&sp=(*entries)[spIt->second.first].sprites[spIt->second.second];
                    DrawCmd q{};q.layer=32;q.handle=tx->second.handle;q.clip_side=side;q.mesh_blend=laser.curve.body_additive?1:0;q.additive=laser.curve.body_additive;q.mesh_fog_enabled=false;
                    const auto&nodes=laser.curve.nodes;
                    // 526990: each node has two width-edge vertices; U runs along the entire strip.
                    auto vertex=[&](size_t i,int edge){const auto&p=nodes[i];float a=p.angle;
                        if(i)a+=std::remainder(nodes[i-1].angle-a,6.28318530718f)*.5f;
                        a+=1.57079632679f;float offset=laser.width*.5f*(edge?1.f:-1.f);
                        float u=(sp.x+sp.w*float(i)/float(nodes.size()-1))/tx->second.width+v.u_offset;
                        float vv=(sp.y+sp.h*edge)/tx->second.height+v.v_offset;
                        return SpriteVertex{{cx+GAME_SCALE*(p.x+std::cos(a)*offset),GAME_FY0+GAME_SCALE*(p.y+std::sin(a)*offset),0},1,v.color(),{u,vv}};};
                    for(size_t i=1;i<nodes.size();++i){auto a=vertex(i-1,0),b=vertex(i-1,1),c=vertex(i,0),d=vertex(i,1);
                        q.mesh.insert(q.mesh.end(),{a,b,c,c,b,d});}
                    if(!q.mesh.empty())g_draw.push_back(std::move(q));
                }
            }
            // 5286c0 initializes a separate bullet.anm color+58 VM.
            auto&origin=rb_draw_ecl_objects_curveOrigins[side][laser.id];
            if(origin.frame<0){auto sc=g_bullet_scripts.find(58+laser.source.color);
                if(sc!=g_bullet_scripts.end()){origin.vm.bind_entry(sc->second.stream,sc->second.len,&g_bullet_scripts);origin.vm.tick();
                    // 5286c0 writes laser+17e3 = source VM+4bb after initialization.
                    // The overlapping sources must add their rays, not alpha-cover one another.
                    origin.vm.blend_mode=1;origin.frame=0;}}
            if(origin.frame>=0)while(origin.frame<laser.age){origin.vm.tick();++origin.frame;}
            // 526990: source animation lasts until clock exceeds node count.
            if(origin.frame>=0&&laser.curve.time<=int(laser.curve.nodes.size())&&!laser.curve.nodes.empty()){
                const auto&o=origin.vm;const auto&p=laser.curve.nodes.back();auto si=g_bullet_sprites.find(o.sprite);
                if(o.visible&&si!=g_bullet_sprites.end()){auto tx=g_bullet_tex.find(si->second.first);
                    if(tx!=g_bullet_tex.end())emit_quad(32,tx->second,g_bullet[si->second.first].sprites[si->second.second],
                        cx+GAME_SCALE*(p.x+o.x),GAME_FY0+GAME_SCALE*(p.y+o.y),GAME_SCALE*o.sx,GAME_SCALE*o.sy,
                        o.anchor_x,o.anchor_y,o.color(),o.rot_z,o.blend_mode==1,o.blend_mode);}
            }
            continue;
        }

        if((laser.source.type!=38&&!(laser.straight&&laser.source.type==8))||laser.source.color<0||laser.source.color>=16||laser.width<=0)continue;
        auto&visual=rb_draw_ecl_objects_laserHeads[side][laser.id];
        visual.seek(laser.age,laser.source.type,laser.source.color,g_bullet_scripts);
        const auto&body=visual.body;
        auto it=g_bullet_sprites.find(visual.bodySprite());
        if(it!=g_bullet_sprites.end()&&body.visible){
            const int ei=it->second.first;const auto&sp=g_bullet[ei].sprites[it->second.second];
            auto ti=g_bullet_tex.find(ei);
            if(ti!=g_bullet_tex.end()){
                float x=laser.x+std::cos(laser.angle)*laser.length*.5f;
                float y=laser.y+std::sin(laser.angle)*laser.length*.5f;
                emit_quad(32,ti->second,sp,cx+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,
                    GAME_SCALE*laser.width/sp.w,GAME_SCALE*laser.length/sp.h,0,0,body.color(),
                    laser.angle+1.57079632679f,body.blend_mode==1,body.blend_mode);
                if(!g_skip_present&&!g_draw.empty()){auto&q=g_draw.back();q.u0+=body.u_offset;q.u1+=body.u_offset;q.v0+=body.v_offset;q.v1+=body.v_offset;}
            }
        }
        // 52da40 / 52dff0: independent third VM at the moving laser tip.
        if(laser.straight){const auto&t=visual.tip;auto ts=g_bullet_sprites.find(t.sprite);
            if(t.visible&&ts!=g_bullet_sprites.end()){auto tex=g_bullet_tex.find(ts->second.first);
                if(tex!=g_bullet_tex.end()){
                    emit_quad(32,tex->second,g_bullet[ts->second.first].sprites[ts->second.second],
                        cx+GAME_SCALE*(laser.x+std::cos(laser.angle)*laser.length),
                        GAME_FY0+GAME_SCALE*(laser.y+std::sin(laser.angle)*laser.length),
                        GAME_SCALE*t.sx,GAME_SCALE*t.sy,t.anchor_x,t.anchor_y,t.color(),t.rot_z,t.blend_mode==1,t.blend_mode);
                    if(!g_skip_present&&!g_draw.empty()){auto&q=g_draw.back();q.u0+=t.u_offset;q.u1+=t.u_offset;q.v0+=t.v_offset;q.v1+=t.v_offset;}
                }
            }
        }
        if(laser.straight&&laser.travelled!=0.f)continue;
        const auto&v=visual.origin;auto hi=g_bullet_sprites.find(v.sprite);
        if(hi!=g_bullet_sprites.end()&&v.visible){auto ht=g_bullet_tex.find(hi->second.first);
            if(ht!=g_bullet_tex.end()){
                emit_quad(32,ht->second,g_bullet[hi->second.first].sprites[hi->second.second],
                    cx+GAME_SCALE*laser.x,GAME_FY0+GAME_SCALE*laser.y,GAME_SCALE*v.sx,GAME_SCALE*v.sy,
                    v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);
                if(!g_skip_present&&!g_draw.empty()){auto&q=g_draw.back();q.u0+=v.u_offset;q.u1+=v.u_offset;q.v0+=v.v_offset;q.v1+=v.v_offset;}
            }
        }
    }
    for(auto it=rb_draw_ecl_objects_curveVisuals[side].begin();it!=rb_draw_ecl_objects_curveVisuals[side].end();){
        bool alive=std::any_of(world.lasers.begin(),world.lasers.end(),[&](const auto&l){return l.curved&&l.id==it->first;});
        if(!alive){rb_draw_ecl_objects_curveOrigins[side].erase(it->first);it=rb_draw_ecl_objects_curveVisuals[side].erase(it);}else ++it;
    }
    for(auto it=rb_draw_ecl_objects_laserHeads[side].begin();it!=rb_draw_ecl_objects_laserHeads[side].end();){
        bool alive=std::any_of(world.lasers.begin(),world.lasers.end(),[&](const auto&l){return l.id==it->first;});
        if(!alive)it=rb_draw_ecl_objects_laserHeads[side].erase(it);else ++it;
    }
    for(const auto&p:world.graze_particles){
        if(g_skip_present||!p.active||p.count<2)continue;DrawCmd q{};q.layer=33;q.handle=g_white_tex.handle;q.line_strip=true;q.mesh_fog_enabled=false;
        for(int i=0;i<p.count;++i)q.mesh.push_back(SpriteVertex{{cx+GAME_SCALE*p.x+p.points[i].x,GAME_FY0+GAME_SCALE*p.y+p.points[i].y,0},1,p.colors[i],{.5f,.5f}});
        g_draw.push_back(std::move(q));
    }
    for (const auto& b : world.bullets) {
        if(b.state==4)continue;
        auto drawBullet=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy,bool root,bool mapped)->void{
            if(!v.visible)return;
            const int sprite=mapped?th19::nativeBulletFrameSprite(b.kind,b.color,v.sprite):v.sprite;
            x+=v.x+v.child_x;y+=v.y+v.child_y;
            auto it=g_bullet_sprites.find(sprite);
            if(it!=g_bullet_sprites.end()){auto ti=g_bullet_tex.find(it->second.first);if(ti!=g_bullet_tex.end()){
                float angle=root?th19::nativeBulletRotation(v.rotation_mode,v.rot_z,b.visual_heading):v.rot_z;
                emit_quad(32,ti->second,g_bullet[it->second.first].sprites[it->second.second],cx+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,
                    GAME_SCALE*sx*v.sx,GAME_SCALE*sy*v.sy,v.anchor_x,v.anchor_y,root&&b.graze_tint?b.graze_tint:v.color(),angle,v.blend_mode==1,v.blend_mode);
                if(!g_skip_present&&!g_draw.empty()){auto&q=g_draw.back();q.u0+=v.u_offset;q.u1+=v.u_offset;q.v0+=v.v_offset;q.v1+=v.v_offset;}
            }}
            for(const auto&child:v.children)self(self,child,x,y,sx*v.sx,sy*v.sy,false,false);
        };
        if(b.attached_effect.stream)drawBullet(drawBullet,b.attached_effect,b.x,b.y,1,1,false,false);
        if(b.animation.stream)drawBullet(drawBullet,b.animation,b.x+b.graze_x,b.y+b.graze_y,1,1,true,true);
        else {th19::AnmVm fallback;fallback.sprite=b.sprite;fallback.sx=fallback.sy=.5f;drawBullet(drawBullet,fallback,b.x,b.y,1,1,false,false);}
    }

}
    g_actor_clip=-1;
}

// ---- 能力卡菜单（abcard.anm 符卡贴图 + ability.txt 名字）----
// 19 角色卡贴图小写代码（与 ability_names.h 顺序一致）
static const char* const AB_CARD_CODE[19] = {
    "reimu","marisa","sanae","ran","aun","nazurin","seiran","orin",
    "tsukasa","mamizou","yachie","saki","toutetsu","suika","biten",
    "enoko","chiyari","hisami","zanmu",
};
static i32 g_ab_card_big[19][2] = {};
static i32 g_ab_card_small[19][2] = {};

void load_ability_cards() {
    if (g_abcard_loaded) return;
    if (!load_anm("/abcard.anm", g_abcard_bytes, g_abcard)) return;
    decode_textures(g_abcard, g_abcard_bytes, g_abcard_tex);
    std::map<std::string, i32> nm;
    for (i32 ei = 0; ei < (i32)g_abcard.size(); ++ei) {
        const std::string& n = g_abcard[ei].name;
        size_t p0 = n.rfind('/');
        std::string base = (p0 == std::string::npos) ? n : n.substr(p0 + 1);
        size_t p1 = base.rfind('.');
        if (p1 != std::string::npos) base = base.substr(0, p1);
        nm[base] = ei;
    }
    for (i32 c = 0; c < 19; ++c) for (i32 k = 0; k < 2; ++k) {
        char big[64], small[64];
        std::snprintf(big, sizeof big, "card_%s%d_max", AB_CARD_CODE[c], k + 1);
        std::snprintf(small, sizeof small, "card_%s%d", AB_CARD_CODE[c], k + 1);
        auto b = nm.find(big), s = nm.find(small);
        g_ab_card_big[c][k]   = (b != nm.end()) ? b->second : -1;
        g_ab_card_small[c][k] = (s != nm.end()) ? s->second : -1;
    }
    for(const auto&e:g_abcard)for(const auto&sc:e.scripts)
        g_card_scripts[sc.id]={g_abcard_bytes.data()+e.base+sc.offset,u32(g_abcard_bytes.size()-e.base-sc.offset),&e.scripts};
    if(load_anm("/ability.anm",g_ability_bytes,g_ability_entries)){
        decode_textures(g_ability_entries,g_ability_bytes,g_ability_tex);
        for(int ei=0;ei<int(g_ability_entries.size());++ei){const auto&e=g_ability_entries[ei];
            for(int si=0;si<int(e.sprites.size());++si)g_ability_sprites[e.sprites[si].id]={ei,si};
            for(const auto&sc:e.scripts)g_ability_scripts[sc.id]={g_ability_bytes.data()+e.base+sc.offset,u32(g_ability_bytes.size()-e.base-sc.offset),&e.scripts};
        }
    }
    if(load_anm("/abmenu.anm",g_card_menu_bytes,g_card_menu_entries)){
        decode_textures(g_card_menu_entries,g_card_menu_bytes,g_card_menu_tex);
        for(const auto&e:g_card_menu_entries)for(const auto&sc:e.scripts)g_card_menu_scripts[sc.id]={g_card_menu_bytes.data()+e.base+sc.offset,u32(g_card_menu_bytes.size()-e.base-sc.offset),&e.scripts};
    }
    g_abcard_loaded = true;
}

// Original reward controller5539c0: script22, opponent's random card,
// confirmation after119 frames, then a ten-frame exit before stage progression.
void begin_card_reward(int opponent){
    load_ability_cards();g_card_reward=1+opponent*2+(g_spirit_rng.next()&1);
    g_card_reward_frame=0;g_card_reward_exit=-1;g_card_reward_vm={};g_card_name_vm={};g_card_summary_vm={};
    auto it=g_card_scripts.find(22);if(it!=g_card_scripts.end()){
        g_card_reward_vm.bind_entry(it->second.stream,it->second.len,&g_card_scripts);g_card_reward_vm.tick();
        // Native metadata table5c5640, +14: large sprite5+2*card ID.
        g_card_reward_vm.sprite=5+2*g_card_reward;
    }
}
bool tick_card_reward(bool confirm){
    if(g_card_reward<0)return true;
    g_card_reward_vm.tick();++g_card_reward_frame;
    if(g_card_reward_frame==40){
        for(auto pair:{std::pair<th19::AnmVm*,int>{&g_card_name_vm,0},{&g_card_summary_vm,11}}){auto it=g_card_menu_scripts.find(pair.second);if(it!=g_card_menu_scripts.end()){pair.first->bind_entry(it->second.stream,it->second.len,&g_card_menu_scripts);pair.first->tick();}}
    }else {g_card_name_vm.tick();g_card_summary_vm.tick();}
    if(g_card_reward_exit>=0){if(++g_card_reward_exit>=10){g_card_reward=-1;return true;}}
    else if(g_card_reward_frame>119&&confirm){
        if(g_owned_cards.size()<256){g_owned_cards.push_back(g_card_reward);
            // Card11 update4085e0 ->537c30: one-time +2, clear fragments at cap.
            if(g_card_reward==11){g_p1_bombs=std::min(7,g_p1_bombs+2);if(g_p1_bombs>=7)g_p1_fragments=0;}
        }
        g_card_reward_exit=0;
    }
    return false;
}
// Text output uses the existing font backend; animation/placement follow4e44e0.
void draw_card_text_line(const char*text,float x,float y,float size,u32 color,int layer){
    FontTex ft;if(!render_text(text,size,ft))return;
    th19::AnmSprite sp;sp.w=ft.w;sp.h=ft.h;
    // abmenu text scripts421=0x10000: centered X, top-aligned Y.
    emit_quad(layer,ft.tex,sp,x,y,1,1,0,1,color,0);
}
// ANM rotation order is shared with the native sprite/world renderer.
void emit_card_vm(const th19::AnmVm&v,const TextureAllocation&t,const th19::AnmSprite&sp,float x,float y,int layer){
    DrawCmd q{};q.layer=layer;q.handle=t.handle;q.mesh_fog_enabled=false;q.mesh_blend=v.blend_mode;q.additive=v.blend_mode==1;
    auto vertex=[&](float u,float w){
        float ax=v.anchor_x==1?0:v.anchor_x==2?1:.5f,ay=v.anchor_y==1?0:v.anchor_y==2?1:.5f;
        struct {float x,y,z;} p{(u-ax)*sp.w*v.sx,(w-ay)*sp.h*v.sy,0};
        static constexpr int orders[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
        for(int axis:orders[std::clamp(v.rotation_order,0,5)]){float a=axis==0?v.rot_x:axis==1?v.rot_y:v.rot_z,c=std::cos(a),sn=std::sin(a);auto old=p;
            if(axis==0){p.y=old.y*c-old.z*sn;p.z=old.y*sn+old.z*c;}
            if(axis==1){p.x=old.x*c+old.z*sn;p.z=-old.x*sn+old.z*c;}
            if(axis==2){p.x=old.x*c-old.y*sn;p.y=old.x*sn+old.y*c;}}
        return SpriteVertex{{x+p.x,y+p.y,0},1,v.color(),{(sp.x+u*sp.w)/t.width,(sp.y+w*sp.h)/t.height}};
    };
    for(auto uv:{std::pair<float,float>{0,0},{1,0},{1,1},{0,0},{1,1},{0,1}})q.mesh.push_back(vertex(uv.first,uv.second));g_draw.push_back(std::move(q));
}
void draw_owned_card_menu(){
    if(g_card_menu_exit>=0)return;
    for(size_t i=0;i<g_card_menu_cards.size();++i){const auto&v=g_card_menu_cards[i];if(!v.visible||std::abs(int(i)-g_card_menu_index)>3)continue;
        for(int ei=0;ei<int(g_abcard.size());++ei)for(const auto&sp:g_abcard[ei].sprites)if(int(sp.id)==v.sprite){auto t=g_abcard_tex.find(ei);if(t!=g_abcard_tex.end())emit_card_vm(v,t->second,sp,328+v.x,324+v.y,73+(i==size_t(g_card_menu_index)));}}
    if(g_owned_cards.empty())return;int id=g_owned_cards[g_card_menu_index];
    // Pause caller537d40 origin(328,100); description origin gets +92.
    draw_card_text_line(CARD_TEXT[id][0],328,502,64*.67f,0xffffffff,76);
    std::istringstream lines(CARD_TEXT[id][2]);std::string line;float y=690;
    while(std::getline(lines,line)){draw_card_text_line(line.c_str(),328,y,32*.67f,0xffffffff,76);y+=38;}
}
void select_owned_card(int delta){
    if(g_owned_cards.empty()||g_card_menu_exit>=0)return;
    const int old=g_card_menu_index;
    g_card_menu_index=std::clamp(old+delta,0,int(g_owned_cards.size())-1);
    if(delta&&old==g_card_menu_index)return;
    // 4e0f60: initial positions27..31; directional motion11..22.
    static constexpr int right[]={15,13,11,17,19,21}; // relative -3..2
    static constexpr int left[]={22,20,18,12,14,16}; // relative -2..3
    for(size_t i=0;i<g_card_menu_cards.size();++i){auto&v=g_card_menu_cards[i];int d=int(i)-g_card_menu_index;
        const int label=delta>0?(d>=-3&&d<=2?right[d+3]:-1):delta<0?(d>=-2&&d<=3?left[d+2]:-1):(std::abs(d)<=2?29+d:-1);
        v.visible=label>=0;if(v.visible)v.interrupt(label);
    }
}
void close_owned_card_menu(){if(g_card_menu_exit<0){g_card_menu_cards.clear();g_card_menu_exit=0;}}
void start_owned_card_menu(){
    load_ability_cards();g_card_menu_cards.clear();g_card_menu_index=0;g_card_menu_exit=-1;auto it=g_card_scripts.find(17);
    if(it!=g_card_scripts.end())for(int id:g_owned_cards){th19::AnmVm v;v.bind_entry(it->second.stream,it->second.len,&g_card_scripts);v.tick();v.sprite=5+2*id;g_card_menu_cards.push_back(std::move(v));}select_owned_card(0);
}
void tick_owned_card_menu(){if(g_card_menu_exit>=0){if(++g_card_menu_exit>=10){g_pause_view=0;g_card_menu_exit=-1;}return;}for(auto&v:g_card_menu_cards)v.tick();}
void draw_card_reward(){
    load_ability_cards();
    if(g_owned_card_vms.size()!=g_owned_cards.size()){
        g_owned_card_vms.clear();g_cards_dim=false;
        auto it=g_card_scripts.find(6);
        if(it!=g_card_scripts.end())for(int id:g_owned_cards){
            th19::AnmVm vm;vm.bind_entry(it->second.stream,it->second.len,&g_card_scripts);vm.tick();
            if(!vm.children.empty())vm.children[0].sprite=6+2*id;
            g_owned_card_vms.push_back(std::move(vm));
        }
    }
    if(!g_paused&&g_cards_draw_frame!=g_frame){
        bool dim=g_cards_dim;
        if(!dim&&g_p1_y>368&&g_p1_x<-96)dim=true;
        else if(dim&&(g_p1_y<352||g_p1_x>-80))dim=false;
        for(auto&vm:g_owned_card_vms){if(dim!=g_cards_dim)vm.interrupt_all(dim?5:4);vm.tick();}
        g_cards_dim=dim;g_cards_draw_frame=g_frame;
    }
    auto draw=[&](auto&&self,const th19::AnmVm&vm,float x,float y,float sx,float sy)->void{
        x+=vm.x*sx;y+=vm.y*sy;sx*=vm.sx;sy*=vm.sy;
        if(vm.visible&&vm.sprite>=0)for(int ei=0;ei<int(g_abcard.size());++ei)for(const auto&sp:g_abcard[ei].sprites)if(int(sp.id)==vm.sprite){
            auto tx=g_abcard_tex.find(ei);if(tx!=g_abcard_tex.end())emit_quad(vm.layer,tx->second,sp,x,y,sx,sy,vm.anchor_x,vm.anchor_y,vm.color(),vm.rot_z,vm.blend_mode==1,vm.blend_mode);
        }
        for(const auto&child:vm.children)self(self,child,x,y,sx,sy);
    };
    for(size_t i=0;i<g_owned_card_vms.size();++i)draw(draw,g_owned_card_vms[i],24+56.f*(i+1)/(g_owned_card_vms.size()+1),0,1,1);
    if(g_card_reward<0)return;
    const auto&v=g_card_reward_vm;
    if(g_card_reward_exit<0){
        const size_t first=g_draw.size();draw_ascii_centered("New Card Get!!",328,276,0xffffd000,0xff000000,1.f);
        const float stretch=std::clamp(g_card_reward_frame/40.f,0.f,1.f);
        for(size_t i=first;i<g_draw.size();++i){g_draw[i].layer=52;g_draw[i].x=328+(g_draw[i].x-328)*stretch;g_draw[i].sx*=stretch;}
    }
    if(g_card_reward_frame>=40){
        // Native5539c0: origin328,220; name +310; short description +80; scale.75.
        draw_card_text_line(CARD_TEXT[g_card_reward][0],328+g_card_name_vm.x*.75f,530+g_card_name_vm.y*.75f,64*.75f,g_card_name_vm.color(),52);
        std::string description=CARD_TEXT[g_card_reward][2];description=description.substr(0,description.find('\n'));
        draw_card_text_line(description.c_str(),328,610,32*.75f,g_card_summary_vm.color(),52);
    }
    if(!v.visible)return;
    for(int ei=0;ei<int(g_abcard.size());++ei)for(const auto&sp:g_abcard[ei].sprites)if(int(sp.id)==v.sprite){
        auto t=g_abcard_tex.find(ei);if(t==g_abcard_tex.end())continue;
        emit_card_vm(v,t->second,sp,v.x,v.y,v.layer);
    }
}

void init_charge_visuals(){
    if(g_aura_entries.empty()&&load_anm("/aura.anm",g_aura_bytes,g_aura_entries)){
        decode_textures(g_aura_entries,g_aura_bytes,g_aura_tex);
        for(const auto&e:g_aura_entries)for(const auto&sc:e.scripts)g_aura_scripts[sc.id]={g_aura_bytes.data()+e.base+sc.offset,u32(g_aura_bytes.size()-e.base-sc.offset),&e.scripts};
    }
    for(int side=0;side<2;++side){
        g_charge_visual_held[side]=false;g_charge_visual_level[side]=0;g_charge_pattern[side]={};g_region_vm[side]={};g_barrier_vm[side]={};
        g_gauge_particles[side]={};
        for(int i=0;i<8;++i){auto&v=g_gauge_markers[side][i];v={};auto it=g_aura_scripts.find(5+i);if(it!=g_aura_scripts.end()){v.bind_entry(it->second.stream,it->second.len,&g_aura_scripts);v.tick();}}
        std::fill(std::begin(g_gauge_marker_state[side]),std::end(g_gauge_marker_state[side]),0);
        g_barrier[side].reset();
        if(g_vs_mode){const int rule=g_vs_rules[5+side];const int base[]={0,60000,1800,900};const int extra[]={0,6000,900,600};g_barrier[side].reset(base[rule],extra[rule]);}
        g_player_damage[side]={};
    }
}
void tick_charge_visuals(){
    for(int side=0;side<2;++side){
        const auto&g=g_charge[side];
        auto&particles=g_gauge_particles[side];
        if(g.charge<1)particles={};
        else{
            if(!particles.stream){auto it=g_aura_scripts.find(13);if(it!=g_aura_scripts.end()){particles.bind_entry(it->second.stream,it->second.len,&g_aura_scripts);particles.time=-1;}}
            particles.tick();
            particles.children.erase(std::remove_if(particles.children.begin(),particles.children.end(),[](const auto&v){return !v.visible&&v.halted;}),particles.children.end());
        }
        for(int i=0;i<4;++i){int state=g.charge>=g.thresholds[i]?3:g.energy>=g.thresholds[i]?2:1;
            if(state!=g_gauge_marker_state[side][i]){g_gauge_marker_state[side][i]=state;for(int j=0;j<2;++j)g_gauge_markers[side][i*2+j].interrupt(state==3?2:state==2?3:5);}
            for(int j=0;j<2;++j)g_gauge_markers[side][i*2+j].tick();
        }
        if(g_barrier[side].ready()&&!g_barrier_vm[side].stream){
            auto it=g_focus_scripts.find(1);if(it!=g_focus_scripts.end()){g_barrier_vm[side]={};g_barrier_vm[side].bind_entry(it->second.stream,it->second.len,&g_focus_scripts);play_sfx(59);}
        }
        g_barrier_vm[side].tick();
        if(g_barrier_vm[side].stream&&!g_barrier_vm[side].visible)g_barrier_vm[side]={};
        g_barrier[side].tick(); // 544dd0 creates the ring on the next ready-state tick.
        auto&pattern=g_charge_pattern[side];auto&region=g_region_vm[side];
        const bool held=g_screen==SCR_GAME&&g_charge[side].held;
        const int level=g_charge[side].level(),ch=side?g_opp_sel:g_char_sel;
        auto&scripts=side?g_opp_scripts:g_player_scripts;
        if(held&&!g_charge_visual_held[side]){
            auto it=g_aura_scripts.find(29);if(it!=g_aura_scripts.end()){pattern={};pattern.bind_entry(it->second.stream,it->second.len,&g_aura_scripts);}
            int ordinal=th19::ChargeRegion::supported(ch)?th19::ChargeRegion::scripts[ch]:-1;
            auto r=scripts.find(ordinal);if(ordinal>=0&&r!=scripts.end()){region={};region.bind_entry(r->second.stream,r->second.len,&scripts);}
        }else if(!held&&g_charge_visual_held[side]){pattern={};region.interrupt_all(1);}
        if(pattern.stream){pattern.tick();if(level>g_charge_visual_level[side])pattern.interrupt_returning(7+level);}
        if(region.stream){region.tick();if(held){const auto&r=g_ecl_fields[side].world.charge_region;region.sx=r.radius;region.sy=(ch==0||ch==3||ch==7||ch==8)?r.radius:1;
            region.secondary_sx=r.minor;
            if(ch==4)region.rot_z=-1.570796327f;
            else if(ch==7)region.rot_z=r.angle-1.570796327f; // 0x41c770 writes movement angle to ANM +0x44
            else if(ch==8||ch==10||ch==11||ch==14||ch==18)region.rot_z=r.angle+((ch==10||ch==18)?-1.570796327f:ch==14?.392699082f:0);
            if(ch==14){region.sx=1;region.sy=r.radius;}}}
        g_charge_visual_held[side]=held;g_charge_visual_level[side]=level;
    }
}

void tick_focus(int side,bool down){
    if(down&&!g_focus_down[side]){auto it=g_focus_scripts.find(19);if(it!=g_focus_scripts.end()){
        g_focus_vm[side]={};g_focus_vm[side].bind_entry(it->second.stream,it->second.len,&g_focus_scripts);g_focus_active[side]=true;
    }}else if(!down&&g_focus_down[side])g_focus_vm[side].interrupt_all(1);
    g_focus_down[side]=down;
    if(g_focus_active[side]){g_focus_vm[side].tick();if(!g_focus_vm[side].visible)g_focus_active[side]=false;}
}
void draw_focus_node(int side,const th19::AnmVm& vm){
    if(g_skip_present)return;
    if(vm.visible&&vm.sprite>=0){auto it=g_focus_sprites.find(vm.sprite);
        if(it!=g_focus_sprites.end()){auto ti=g_focus_tex.find(it->second.first);if(ti!=g_focus_tex.end()){
            const auto&sp=g_focus_entries[it->second.first].sprites[it->second.second];
            const u32 tint=(u32(vm.alpha)<<24)|(u32(vm.r)<<16)|(u32(vm.g)<<8)|vm.b;
            emit_quad(34,ti->second,sp,(side?GAME_F2_CX:GAME_F1_CX)+GAME_SCALE*((side?g_p2_x:g_p1_x)+vm.x),GAME_FY0+GAME_SCALE*((side?g_p2_y:g_p1_y)+vm.y),GAME_SCALE*vm.sx,GAME_SCALE*vm.sy,vm.anchor_x,vm.anchor_y,tint,vm.rot_z);
        }}
    }
    for(const auto& child:vm.children)draw_focus_node(side,child);
}
void draw_focus(){for(int side=0;side<2;++side)if(g_focus_active[side]){g_actor_clip=side;draw_focus_node(side,g_focus_vm[side]);}g_actor_clip=-1;}

bool bind_player_animation(th19::AnmVm& vm,int ordinal,const std::vector<th19::AnmEntry>&entries,
                           const std::unordered_map<i32,th19::AnmScriptLoc>&scripts){
    int index=0;
    for(const auto&e:entries)for(const auto&s:e.scripts)if(index++==ordinal){
        auto it=scripts.find(s.id);if(it==scripts.end())return false;
        vm={};vm.bind_entry(it->second.stream,it->second.len,&scripts);vm.tick();return true;
    }
    return false;
}
void init_player_options(){
    g_opp_scripts.clear();
    for(const auto&e:g_opp)for(const auto&s:e.scripts)g_opp_scripts[s.id]={g_opp_bytes.data()+e.base+s.offset,u32(g_opp_bytes.size()-e.base-s.offset),&e.scripts};
    {int ordinal=0;for(const auto&e:g_opp)for(const auto&sc:e.scripts){if(sc.id<0)g_opp_scripts[ordinal]=g_opp_scripts.at(sc.id);++ordinal;}}
    char path[32];std::snprintf(path,sizeof path,"/pl%02d.sht",g_opp_sel);std::vector<u8>data;
    g_opp_shot_table={};if(read_asset(path,data))g_opp_shot_table.load(data);
    for(int side=0;side<2;++side){float x=side?g_p2_x:g_p1_x,y=side?g_p2_y:g_p1_y;g_attacks[side].option_lock_x=x;g_attacks[side].option_lock_y=y;g_player_trail[side].assign(33,{x,y});}
    for(int side=0;side<2;++side){
        auto&options=g_player_options[side];options.clear();const auto&table=side?g_opp_shot_table:g_shot_table;
        for(int i=0;i<g_player_power;++i){PlayerOption o;auto p=table.option_position(g_player_power,i,false);
            o.x=(side?g_p2_x:g_p1_x)+p.x;o.y=(side?g_p2_y:g_p1_y)+p.y;o.phase=i*3.14159265359f;
            if(bind_player_animation(o.vm,table.option_script,side?g_opp:g_player,side?g_opp_scripts:g_player_scripts))options.push_back(std::move(o));
        }
    }
}
void tick_player_options(bool focused){
    // 408310/408c50/408d60/408e60/408f60: simple additional options, default546d10 follow.
    std::vector<int> desired;
    for(int id:g_owned_cards)if(th19::cardOption(id)&&desired.size()<12)desired.push_back(id);
    bool rebuild=desired.size()!=g_card_options.size();
    if(!rebuild)for(size_t i=0;i<desired.size();++i)if(desired[i]!=g_card_options[i].card_id)rebuild=true;
    if(rebuild){g_card_options.clear();for(int id:desired){
        PlayerProjectile option{};option.card_id=id;option.offset_y=th19::cardOption(id)->y;
        option.x=g_p1_x;option.y=g_p1_y+option.offset_y;option.offset_x=g_p1_x;option.card_lock_y=g_p1_y;option.ability_bank=true;if(id==27){option.offset_x=0;option.card_lock_y=0;}
        const int script=th19::cardOption(id)->script;
        if(bind_player_animation(option.vm,script,g_ability_entries,g_ability_scripts))g_card_options.push_back(std::move(option));
    }}
    for(auto&o:g_card_options){
        if(o.card_id==7||o.card_id==8){
            if(!focused){o.offset_x=g_p1_x;o.card_lock_y=g_p1_y;}
            o.x=o.offset_x;o.y=o.card_lock_y+o.offset_y;o.vm.tick();continue;
        }
        if(o.card_id==27){
            const auto&trail=g_player_trail[0];
            if(!g_charge[0].held&&!trail.empty()){const auto&t=trail[std::min(size_t(8),trail.size()-1)];o.offset_x=t.x-g_p1_x;o.card_lock_y=t.y-g_p1_y;}
            const float spread=g_charge[0].held?1.5f:1.f;o.x=g_p1_x+o.offset_x*spread;o.y=g_p1_y+o.card_lock_y*spread;o.vm.tick();continue;
        }
        if(o.card_id==35){
            auto&enemies=g_ecl_fields[0].world.enemies;const th19::Enemy*target=nullptr;float best=1e20f;
            for(const auto&e:enemies)if(e.active&&e.sprite>=0&&!(e.flags&0x21)&&!(e.status_flags&0xc00)){
                float d=(e.x-o.x)*(e.x-o.x)+(e.y-o.y)*(e.y-o.y);
                if(e.id==o.target_id){target=&e;best=d;break;}if(d<best){target=&e;best=d;}
            }
            o.target_id=target?target->id:0;
            if(target){float distance=std::sqrt(best),step=distance<48?0:distance<64?-2:distance<128?2:5;
                o.x=std::trunc((o.x+(target->x-o.x)/std::max(1.f,distance)*step)*128)/128;
                o.y=std::trunc((o.y+(target->y-o.y)/std::max(1.f,distance)*step)*128)/128;}
            o.vm.tick();continue;
        }
        int x=int(o.x*128),y=int(o.y*128),dx=(int(g_p1_x*128)-x)*30/100,dy=(int((g_p1_y+o.offset_y)*128)-y)*30/100;
        if(dx==0&&dy==0){o.x=g_p1_x;o.y=g_p1_y+o.offset_y;}else{o.x=(x+dx)/128.f;o.y=(y+dy)/128.f;}
        o.vm.tick();
    }
    for(int side=0;side<2;++side){int ch=side?g_opp_sel:g_char_sel;bool slow=side?(g_opponent_controls&8)!=0:focused;
        float px=side?g_p2_x:g_p1_x,py=side?g_p2_y:g_p1_y;
        auto&trail=g_player_trail[side];if(trail.empty())trail.assign(33,{px,py});
        if(trail[0].x!=px||trail[0].y!=py){trail.insert(trail.begin(),{px,py});trail.resize(33);}
        if(!slow){g_attacks[side].option_lock_x=px;g_attacks[side].option_lock_y=py;}
        const auto&table=side?g_opp_shot_table:g_shot_table;
        const int power=ch==3?g_attacks[side].ran_skill.power(g_player_power):g_player_power;
        auto&options=g_player_options[side];
        if(int(options.size())>power)options.resize(power);
        while(int(options.size())<power){PlayerOption o;o.x=px;o.y=py;o.phase=options.size()*3.14159265359f;
            if(!bind_player_animation(o.vm,table.option_script,side?g_opp:g_player,side?g_opp_scripts:g_player_scripts))break;
            options.push_back(std::move(o));}
        for(int i=0;i<int(g_player_options[side].size());++i){auto&o=g_player_options[side][i];auto p=table.option_position(power,i,slow);
            float tx=px+p.x,ty=py+p.y;bool snap=false;
            // Native option callbacks: Ran locks on focus; Seiran orbits.
            if(ch==3){tx=g_attacks[side].option_lock_x+p.x;ty=g_attacks[side].option_lock_y+p.y;}
            if(ch==6){float radius=slow?24.f:40.f;tx=px+std::cos(o.phase)*radius;ty=py+std::sin(o.phase)*radius;o.phase=std::remainder(o.phase+3.14159265359f/60.f,6.28318530718f);
                if(tx< -160){tx+=320;snap=true;}else if(tx>160){tx-=320;snap=true;}}
            // 0x54a190: charge freezes the relative history offset, at 1.5x spread.
            if(ch==13){
                const bool charging=g_charge[side].charge>0;
                if(!charging){
                    const auto&previous=trail[std::min(32,16+i*16)];
                    o.charge_offset_x=int(previous.x*128)-int(px*128);
                    o.charge_offset_y=int(previous.y*128)-int(py*128);
                }
                const float spread=charging?1.5f:1.f;
                tx=(int(px*128)+int(o.charge_offset_x*spread))/128.f;
                ty=(int(py*128)+int(o.charge_offset_y*spread))/128.f;
            }
            if(ch==17){
                const th19::Enemy*target=nullptr;float best=1e20f;
                for(const auto&e:g_ecl_fields[side].world.enemies)if(e.active&&e.sprite>=0&&!(e.flags&3)&&e.y>=0&&e.y<448){float d=(e.x-o.x)*(e.x-o.x)+(e.y-o.y)*(e.y-o.y);if(d<best){best=d;target=&e;}}
                if(target){float distance=std::sqrt(best);float step=distance<16?0:(distance<48?-2:(distance<128?2:5));tx=o.x+(target->x-o.x)/std::max(1.f,distance)*step;ty=o.y+(target->y-o.y)/std::max(1.f,distance)*step;}
            }
            // 0x546d10: fixed-point positions approach the target by 30%.
            int x=int(o.x*128),y=int(o.y*128),dx=(int(tx*128)-x)*30/100,dy=(int(ty*128)-y)*30/100;
            if(snap||(dx==0&&dy==0)){o.x=tx;o.y=ty;}else{o.x=(x+dx)/128.f;o.y=(y+dy)/128.f;}
            o.vm.tick();
        }
    }
}
void draw_player_options(){
    for(int side=0;side<2;++side){if(g_screen==SCR_GAME&&!g_player_damage[side].visible())continue;g_actor_clip=side;const auto&sprites=side?g_opp_sprites:g_player_sprites;const auto&textures=side?g_opp_tex:g_player_tex;const auto&entries=side?g_opp:g_player;
        auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y)->void{
            x+=v.x;y+=v.y;auto s=sprites.find(v.sprite);
            if(v.visible&&v.alpha>0&&s!=sprites.end()){auto t=textures.find(s->second.first);if(t!=textures.end())emit_quad(29,t->second,entries[s->second.first].sprites[s->second.second],(side?GAME_F2_CX:GAME_F1_CX)+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,GAME_SCALE*v.sx,GAME_SCALE*v.sy,v.anchor_x,v.anchor_y,(u32(v.alpha)<<24)|(u32(v.r)<<16)|(u32(v.g)<<8)|v.b,v.rot_z,v.blend_mode==1);}
            for(const auto&c:v.children)self(self,c,x,y);
        };
        for(const auto&o:g_player_options[side])draw(draw,o.vm,o.x,o.y);
    }g_actor_clip=-1;
}
// SHT groups 0..4 are power levels, 5..9 their focused counterparts.
void tick_spirit_transits(){
    for(int side=0;side<2;++side){
        auto&field=g_ecl_fields[side];std::vector<const th19::EclProgram*> source;
        for(const auto&p:field.programs)source.push_back(p.get());
        field.world.send_native_extra_credit(g_ecl_fields[1-side].world,source,side?g_opp_sel:g_char_sel);
    }

    ++g_spirit_clock;
    for(auto&v:g_spirit_flights){if(v.state.tick()){
        const auto&r=v.state.request;const int k=r.kind-2,target=1-v.sender;
        const auto a=th19::spiritArrival(v.sender,r.kind,r.inherited,v.state.targetX,
            k>=0&&k<3&&(g_spirit_enhanced[v.sender][k]||(v.sender==0&&th19::cardEnhancesSpirit(g_owned_cards,r.kind))),k>=0&&k<3&&(g_spirit_suppressed[target][k]||(target==0&&th19::cardSuppressesSpirit(g_owned_cards,r.kind))));
        if(g_ecl_fields[target].world.receive_spirit(a,v.state.targetX,v.state.targetY))++g_spirit_arrivals;
    }else{v.curve.frame=std::max(0,v.state.age);if(v.vm.stream)v.vm.tick();}}
    g_spirit_flights.erase(std::remove_if(g_spirit_flights.begin(),g_spirit_flights.end(),[](const auto&v){return v.state.age>v.state.duration;}),g_spirit_flights.end());
    for(int side=0;side<2;++side){auto&w=g_ecl_fields[side].world;
        for(const auto&r:w.spirit_requests){
            // Native checks registered on-field spirits, not pending requests.
            if(w.counted_spirits()>=th19::spiritCapacity(g_spirit_clock)){
                // 0x51bb57 -> 0x548bc0(100): overflow contributes to ordinary
                // counter bullets, not score, pickups or instant enemy creation.
                w.pure_spirit_meter.add_bullets(100,w.difficulty,r.x,r.y);if(side==0)++w.pending_story_credit;++g_spirit_overflow;continue;}
            const float rx=g_spirit_rng.unit(),ry=g_spirit_rng.unit();
            const unsigned delay=g_spirit_rng.next();float a=0,b=0;unsigned sign=0;
            if(r.kind==2||r.kind==3)a=g_spirit_rng.signedUnit();
            if(r.kind==3){b=g_spirit_rng.signedUnit();sign=g_spirit_rng.next();}
            SpiritFlightVisual v;v.sender=side;v.state=th19::prepareSpiritTransit(r,delay,rx,ry,a,b,sign,w.opponent_x);
            auto&c=v.curve;c.duration=v.state.duration;
            c.from={(side?GAME_F2_CX:GAME_F1_CX)*.5f+r.x,GAME_FY0*.5f+r.y};
            c.to={(side?GAME_F1_CX:GAME_F2_CX)*.5f+v.state.targetX,GAME_FY0*.5f+v.state.targetY};
            // 0x51b6c0 / mode 8: end tangent is drawn before start tangent.
            c.end_tangent={g_spirit_rng.signedUnit()*100.f,120.f};
            c.start_tangent={g_spirit_rng.signedUnit()*100.f,-400.f};
            g_spirit_flights.push_back(std::move(v));
        }w.spirit_requests.clear();
        for(const auto&r:w.chain_bullet_transits){
            ReflectedVisual v;v.target_side=1-side;auto&s=v.state;
            const float dx=g_reflection_rng.signedUnit(),dy=g_reflection_rng.unit();
            const auto dest=s.destination(r.x,dx,dy);v.target_x=dest.x;v.target_y=dest.y;
            s.type=th19::chainBulletType(r.kind,r.frame,r.frame>=3600?g_reflection_rng.next():0);
            s.color=r.color;s.base_speed=s.baseSpeed(r.speed);s.duration=s.flightDuration(g_reflection_rng.next());
            s.from={(side?GAME_F2_CX:GAME_F1_CX)+GAME_SCALE*r.x,GAME_FY0+GAME_SCALE*r.y};
            s.to={(side?GAME_F1_CX:GAME_F2_CX)+GAME_SCALE*dest.x,GAME_FY0+GAME_SCALE*dest.y};
            s.start_tangent={g_reflection_rng.signedUnit()*100,-400};s.end_tangent={g_reflection_rng.signedUnit()*100,100};
            g_reflected_flights.push_back(v);
        }w.chain_bullet_transits.clear();
        for(const auto&r:w.counter_bullet_requests){
            const float radius=(g_reflection_rng.unit()-1.f)*24.f;
            const float angle=g_reflection_rng.signedUnit()*3.141592741f;
            const float sourceX=r.x+std::cos(angle)*radius,sourceY=r.y+std::sin(angle)*radius;
            ReflectedVisual v;v.target_side=1-side;
            v.target_x=g_reflection_rng.signedUnit()*148.f;v.target_y=(g_reflection_rng.unit()-1.f)*16.f;
            auto&s=v.state;s.type=th19::counterBulletType(g_spirit_clock,g_spirit_clock>=3600?g_reflection_rng.next():0);
            s.color=g_reflection_rng.next()&15;s.base_speed=1.f;
            s.duration=s.flightDuration(g_reflection_rng.next());
            s.from={(side?GAME_F2_CX:GAME_F1_CX)+GAME_SCALE*sourceX,GAME_FY0+GAME_SCALE*sourceY};
            s.to={(side?GAME_F1_CX:GAME_F2_CX)+GAME_SCALE*v.target_x,GAME_FY0+GAME_SCALE*v.target_y};
            s.start_tangent={g_reflection_rng.signedUnit()*100,-400};s.end_tangent={g_reflection_rng.signedUnit()*100,100};
            g_reflected_flights.push_back(v);
        }w.counter_bullet_requests.clear();
    }
}
void tick_player_shots(bool firing,bool focused){
    // 54e490: ordinary emitter is disabled while the aura/charge counter is active.
    firing=firing&&!g_charge[0].held;
    const bool rightFiring=(g_opponent_controls&1)&&g_player_damage[1].controllable()&&!g_charge[1].held;
    for(int side=0;side<2;++side)g_ecl_fields[side].world.card_near_damage=side==0&&th19::ownsCard(g_owned_cards,28);
    for(auto&v:g_reflected_flights){auto&s=v.state;
        if(++s.frame>=s.duration){auto&b=g_ecl_fields[v.target_side].world.spawn_bullet();b.x=v.target_x;b.y=v.target_y;
            b.kind=s.type;b.color=s.color;b.sprite=th19::nativeBulletSprite(s.type,s.color);b.reflected=true;
            float angle=s.arrivalAngle(b.x,g_reflection_rng.unit());float speed=th19::cardArrivalSpeed(s.arrivalSpeed(g_reflection_rng.unit(),g_reflection_rank,g_reflection_rank_cap),v.target_side==0&&th19::ownsCard(g_owned_cards,36));
            b.vx=std::cos(angle)*speed;b.vy=std::sin(angle)*speed;++g_reflection_arrivals;}}
    g_reflected_flights.erase(std::remove_if(g_reflected_flights.begin(),g_reflected_flights.end(),[](const auto&v){return v.state.arrived();}),g_reflected_flights.end());
    for(int side=0;side<2;++side){
        float px=side?g_p2_x:g_p1_x,py=side?g_p2_y:g_p1_y,opx=side?g_p1_x:g_p2_x,opy=side?g_p1_y:g_p2_y;
        float cx=side?GAME_F2_CX:GAME_F1_CX,ocx=side?GAME_F1_CX:GAME_F2_CX;
    g_attacks[side].ran_skill.tick();
    if(g_attacks[side].chiyari_skill.frame>=0){auto&s=g_attacks[side].chiyari_skill;
        // 0x42b7d0: recompute from both live player positions, t=frame/60.
        float t=s.frame/60.f;s.x=(cx+GAME_SCALE*px)*(1-t)+(ocx+GAME_SCALE*opx)*t;
        s.y=GAME_FY0+GAME_SCALE*(py*(1-t)+opy*t);s.vm.tick();
        if(++s.frame>60)s.frame=-1;
    }
    for(auto&skill:g_attacks[side].marisa_skills){
        auto&s=skill.state;
        if(s.arrived()){
            std::vector<const th19::EclProgram*> source;for(const auto&p:g_ecl_fields[side].programs)source.push_back(p.get());
            g_ecl_fields[1-side].world.spawn_enemy("ExtraAttack00",skill.target_x,448,200,0,0,&source);
            ++g_attacks[side].marisa_skill_arrivals;skill.vm.alpha=0;
        }else{skill.vm.tick();skill.vm.sx=skill.vm.sy=.8f*GAME_SCALE;skill.vm.alpha=192;++s.frame;}
    }
    g_attacks[side].marisa_skills.erase(std::remove_if(g_attacks[side].marisa_skills.begin(),g_attacks[side].marisa_skills.end(),[](const auto&s){return s.state.arrived()&&s.vm.alpha==0;}),g_attacks[side].marisa_skills.end());
    for(auto&skill:g_attacks[side].reimu_skills){auto&s=skill.state;skill.positions=s.centers();
        if(s.frame==45){for(auto&v:skill.animations)v.interrupt_all(1);stop_sfx(49);play_sfx(44);}
        for(auto&v:skill.animations)v.tick();
        const int count=s.frame<45?3:1;auto&w=g_ecl_fields[side].world;
        for(int i=0;i<count;++i){const auto p=skill.positions[i];const float r=s.clearRadius();
            w.cancel_bullets_circle(p.x,p.y,r);w.cancel_lasers_circle(p.x,p.y,r);
            if(s.createsDamage())g_attacks[side].skill_damage_circles.push_back({p.x,p.y,64,s.damageGrowth(),s.damageDuration(),s.damage()});
        }
        s.tick();
    }
    g_attacks[side].reimu_skills.erase(std::remove_if(g_attacks[side].reimu_skills.begin(),g_attacks[side].reimu_skills.end(),[](const auto&s){return !s.state.active();}),g_attacks[side].reimu_skills.end());
    // 0x5200f0 / 0x51fa95: damage circles retain their own lifetime and
    // radial growth, independently of the visible spell and bullet-clear area.
    for(auto&c:g_attacks[side].skill_damage_circles){c.radius+=c.growth;c.x+=c.vx;c.y+=c.vy;auto&w=g_ecl_fields[side].world;
        for(auto&e:w.enemies){float dx=std::max(0.f,std::abs(e.x-c.x)-e.hitbox_w*.5f),dy=std::max(0.f,std::abs(e.y-c.y)-e.hitbox_h*.5f);
            if(c.remaining%c.interval==0&&dx*dx+dy*dy<=c.radius*c.radius)w.damage_enemy(e,c.damage);}
        --c.remaining;
    }
    g_attacks[side].skill_damage_circles.erase(std::remove_if(g_attacks[side].skill_damage_circles.begin(),g_attacks[side].skill_damage_circles.end(),[](const auto&c){return c.remaining<=0;}),g_attacks[side].skill_damage_circles.end());
    if(g_attacks[side].charge_area.character==12){g_attacks[side].charge_area.x=px;g_attacks[side].charge_area.y=py;g_attacks[side].charge_area_x=px;g_attacks[side].charge_area_y=py;}
    if(g_attacks[side].charge_area.active()){
        auto&w=g_ecl_fields[side].world;
        if(g_attacks[side].charge_area.character==7){
            const float r=g_attacks[side].charge_area.radius();g_attacks[side].charge_area_vm.sx=g_attacks[side].charge_area_vm.sy=r;
            g_attacks[side].charge_area.converted_count+=w.purify_orin(g_attacks[side].charge_area.x,g_attacks[side].charge_area.y,r);
        }else if(g_attacks[side].charge_area.character==8){
            const float r=g_attacks[side].charge_area.radius();g_attacks[side].charge_area_vm.sx=g_attacks[side].charge_area_vm.sy=r;
            // 0x51c8c0: preserve color, type6, no laser/large-bullet conversion.
            for(auto&b:w.bullets)if(th19::tsukasaReflects(b.state,unsigned(b.kind),b.x,b.y,g_attacks[side].charge_area.x,g_attacks[side].charge_area.y,r)&&th19::reflectedSprite(b.color)>=0){
                ReflectedVisual v;v.target_side=1-side;auto&s=v.state;float dx=g_reflection_rng.signedUnit(),dy=g_reflection_rng.unit();auto dest=s.destination(b.x,dx,dy);
                v.target_x=dest.x;v.target_y=dest.y;s.color=b.color;s.base_speed=s.baseSpeed(std::hypot(b.vx,b.vy));
                s.duration=s.flightDuration(g_reflection_rng.next());
                s.from={cx+GAME_SCALE*b.x,GAME_FY0+GAME_SCALE*b.y};
                s.to={ocx+GAME_SCALE*dest.x,GAME_FY0+GAME_SCALE*dest.y};
                s.start_tangent={g_reflection_rng.signedUnit()*100,-400};s.end_tangent={g_reflection_rng.signedUnit()*100,100};
                g_reflected_flights.push_back(v);b.state=4;
            }
        }else if(g_attacks[side].charge_area.character==18){
            const float r=g_attacks[side].charge_area.radius();g_attacks[side].charge_area_vm.sx=r;g_attacks[side].charge_area_vm.sy=1;
            w.cancel_bullets_circle(g_attacks[side].charge_area.x,g_attacks[side].charge_area.y,r*.2f);
            w.cancel_lasers_circle(g_attacks[side].charge_area.x,g_attacks[side].charge_area.y,r*.2f);
            g_attacks[side].charge_area.converted_count+=w.convert_zanmu(g_attacks[side].charge_area.x,g_attacks[side].charge_area.y,r,g_attacks[side].charge_area.conversion_sequence);
        }else if(g_attacks[side].charge_area.character==5){
            // 0x41b420 sets area flags 0x2c0. 0x520450 accepts only
            // enemy flags 0x700000 AND 0x80000, sets Enemy+0x6250 bit 2;
            // 0x509ee0 invokes death independently of HP/automatic-death bit.
            for(auto&e:w.enemies)if((e.status_flags&0x700000)&&(e.status_flags&0x80000)){
                float r=g_attacks[side].charge_area_vm.sx+e.hitbox_w*.5f,dx=e.x-g_attacks[side].charge_area.x,dy=e.y-g_attacks[side].charge_area.y;
                if(dx*dx+dy*dy<=r*r)w.damage_enemy(e,0,true);
            }
        }else{
            for(auto&e:w.enemies)if(g_attacks[side].charge_area.contains(e.x,e.y))w.damage_enemy(e,g_attacks[side].charge_area.damage());
            w.bullets.erase(std::remove_if(w.bullets.begin(),w.bullets.end(),[&](const auto&b){return g_attacks[side].charge_area.contains(b.x,b.y);}),w.bullets.end());
        }
    }
    g_attacks[side].charge_area.tick(px,py);
    if(g_attacks[side].charge_area_vm.stream)g_attacks[side].charge_area_vm.tick();
    if(g_attacks[side].tsukasa_skill_back.stream)g_attacks[side].tsukasa_skill_back.tick();
    if(g_attacks[side].charge_area.character==8&&!g_attacks[side].charge_area.active()){g_attacks[side].charge_area_vm.visible=false;g_attacks[side].tsukasa_skill_back.visible=false;}
    }
    for(auto& effect:g_player_hit_effects)effect.vm.tick();
    g_player_hit_effects.erase(std::remove_if(g_player_hit_effects.begin(),g_player_hit_effects.end(),[](const auto&e){return !e.vm.visible;}),g_player_hit_effects.end());
    for(auto& b:g_player_projectiles){
        const float old_x=b.x,old_y=b.y;const int side=b.side;
        const bool sideFiring=side?rightFiring:firing;
        if(b.lifetime>0&&b.age>=b.lifetime&&!b.ending){b.vm.interrupt_all(1);b.ending=true;b.damage=0;}
        const bool beam=b.type==2||b.update==9;
        if(beam){float x=side?g_p2_x:g_p1_x,y=side?g_p2_y:g_p1_y;if(b.option>0&&b.option<=int(g_player_options[side].size())){x=g_player_options[side][b.option-1].x;y=g_player_options[side][b.option-1].y;}
            if(b.ability_bank){auto card=std::find_if(g_card_options.begin(),g_card_options.end(),[&](const auto&o){return o.card_id==b.card_id;});if(card!=g_card_options.end()){x=card->x;y=card->y;}else b.ending=true;}
            // Marisa's released beam stays at its release origin (0x54c830).
            if(b.update==9){x=b.x-b.offset_x;y=b.y-b.offset_y;}
            b.x=x+b.offset_x;b.y=y+b.offset_y;
            // 0x54c830 / 0x54d370: add18 only below512; last step may reach522.
            if(!b.ending&&b.length<512.f)b.length+=18.f;
            const bool expired=b.update==9?b.age>=120:(b.laser_duration>0?b.age>=b.laser_duration:!sideFiring);
            if(expired&&!b.ending){b.vm.interrupt_all(1);b.ending=true;}
        }else{
            if(b.update==1){float speed=std::hypot(b.vx,b.vy),angle=std::atan2(b.vy,b.vx);const th19::Enemy*target=nullptr;float best=1e20f;
                for(const auto&e:g_ecl_fields[side].world.enemies)if(e.active&&e.sprite>=0&&!(e.flags&3)&&e.y>=0&&e.y<448){float d=(e.x-b.x)*(e.x-b.x)+(e.y-b.y)*(e.y-b.y);if(e.id==b.target_id){target=&e;break;}if(d<best){target=&e;best=d;}}
                b.target_id=target?target->id:0;th19::advanceHomingShot(b.age,target!=nullptr,target?std::atan2(target->y-b.y,target->x-b.x):angle,angle,speed);
                b.angle=angle;b.speed=speed;b.vx=std::cos(angle)*speed;b.vy=std::sin(angle)*speed;
            }
            if(b.update==10){
                if(b.age%30==0){b.turn_rate=(th19::AnmVm::rand_unit()*2-1)*3.14159265359f/180;b.acceleration=(th19::AnmVm::rand_unit()*2-1)*.2f;}
                b.angle=std::remainder(b.angle+b.turn_rate,6.28318530718f);if(b.speed>=2||b.acceleration>=0)b.speed+=b.acceleration;
                b.vx=std::cos(b.angle)*b.speed;b.vy=std::sin(b.angle)*b.speed;
            }
            if(b.update==6&&!b.ending)b.speed+=.1f; //54b780
            if(b.update==8)th19::advanceChargeTalisman(b.age,b.speed);
            if(b.update==11){b.speed+=.1f;b.length+=b.speed;
                if(!b.vm.children.empty())b.vm.children[0].sx=b.length;
                if(b.age>129)b.consumed=true;
            }
            // 0x54c080: Sanae's radial shots stop for four frames, then
            // turn towards the enemy that crossed their horizontal band.
            if(!b.ending&&b.update==4&&b.turn_wait<0){
                for(const auto&e:g_ecl_fields[side].world.enemies)if(e.active&&e.sprite>=0&&!(e.flags&0x21)&&std::abs(e.y-b.y)<=16&&std::abs(e.x-b.x)>=16){b.offset_x=e.x;b.offset_y=e.y;b.speed=0;b.turn_wait=0;b.vm.interrupt_all(2);break;}
            }
            if(!b.ending&&b.update==4&&b.turn_wait>=0&&b.turn_wait<=4){if(b.turn_wait==4){b.angle=std::atan2(b.offset_y-b.y,b.offset_x-b.x);b.speed=14;}++b.turn_wait;}
            if(b.update==13){
                if(b.age==b.transition_frame){b.speed=0;b.vm.interrupt_all(2);play_sfx(27);}
                else {const float step=th19::enokoChargeStep(b.age,b.transition_frame);b.x+=std::cos(b.angle)*step;b.y+=std::sin(b.angle)*step;}
            }
            if(!b.ending&&(b.update==3||b.update==12||b.update==14)){th19::advanceSpecialShot(b.update,b.angle,b.speed);b.vx=std::cos(b.angle)*b.speed;b.vy=std::sin(b.angle)*b.speed;}
            // Native 0x54d... adds SHT +0x18/+0x20 after the callback.
            if(!b.ending){b.angle=std::remainder(b.angle+b.table_turn,6.28318530718f);b.speed+=b.table_acceleration;}
            b.vx=std::cos(b.angle)*b.speed;b.vy=std::sin(b.angle)*b.speed;
            if(b.update!=13||!th19::enokoChargeFrozen(b.age,b.transition_frame)){b.x+=b.vx;b.y+=b.vy;}
        }
        if(!b.ending&&b.damage>0){
            auto&world=g_ecl_fields[side].world;
            const bool previousLaserHit=beam&&b.target_id!=0;if(beam)b.target_id=0;
            th19::Enemy* nearest=nullptr;float distance=1e30f;
            const bool blockingLaser=beam&&b.hit_callback==2;
            for(auto&e:world.enemies){
                if(!e.active||e.sprite<0||(e.flags&0x21)||e.hitbox_w<=0)continue;
                if(blockingLaser){
                    float d=th19::laserContactDistance(b.x,b.y,b.angle,b.length,b.hit_height,e.x,e.y,e.hitbox_w,e.hitbox_h);
                    if(d>=0&&d<distance){distance=d;nearest=&e;}continue;
                }
                float x0=old_x,y0=old_y,x1=b.x,y1=b.y;if(beam){x1+=std::cos(b.angle)*b.length;y1+=std::sin(b.angle)*b.length;}
                if(b.update==11){x0=b.x-std::cos(b.angle)*b.length;y0=b.y-std::sin(b.angle)*b.length;}
                if(th19::sweptShotHit(x0,y0,x1,y1,b.angle,b.hit_width,b.update==11?28.f:b.hit_height,e.x,e.y,e.hitbox_w,e.hitbox_h)){
                    if(beam){b.target_id=e.id;if(th19::laserDamageTick(b.hit_callback,b.age,b.init_callback))world.damage_enemy(e,b.damage);}
                    else if(b.update==11||b.piercing)world.damage_enemy(e,b.damage);
                    else {float d=(e.x-old_x)*(e.x-old_x)+(e.y-old_y)*(e.y-old_y);if(d<distance){distance=d;nearest=&e;}}
                    // The Enoko trap (SHT update 13, init 21) is a one-contact
                    // projectile.  Mark it consumed at the collision site,
                    // matching the native hit callback instead of allowing the
                    // swept rectangle to register the same trap on later frames.
                    if(b.update==13){world.damage_enemy(e,b.damage);b.consumed=true;nearest=nullptr;break;}
                }
            }
            if(nearest){
                if(blockingLaser){b.target_id=nearest->id;b.length=std::max(0.f,distance+8.f);
                    if(th19::laserDamageTick(b.hit_callback,b.age,b.init_callback))world.damage_enemy(*nearest,b.damage);
                    // 0x54cd70: independent SHT +0x2e ANM on even contact
                    // frames, moving 64 units forward over 20 frames (mode 4).
                    if(b.age>0&&(b.age&1)==0&&b.hit_script>=0){
                        PlayerProjectile effect{};effect.side=side;effect.angle=b.angle;effect.ability_bank=b.ability_bank;
                        effect.x=b.x+std::cos(b.angle)*b.length;effect.y=b.y+std::sin(b.angle)*b.length;
                        if(bind_player_animation(effect.vm,b.hit_script,b.ability_bank?g_ability_entries:(side?g_opp:g_player),b.ability_bank?g_ability_scripts:(side?g_opp_scripts:g_player_scripts))){
                            effect.vm.rot_z=b.angle;
                            auto&ip=effect.vm.ip_pos;ip={};ip.active=true;ip.dur=20;ip.mode=4;
                            ip.t0=std::cos(b.angle)*64.f;ip.t1=std::sin(b.angle)*64.f;
                            g_player_hit_effects.push_back(std::move(effect));
                        }
                    }}
                else {
                    const bool flameHit=b.hit_callback==3||b.hit_callback==9;
                    const bool frog=b.hit_callback==4; // 54c2f0, also used by pl02.sht group 11
                    if(flameHit||frog){
                        const float speed=frog?2.f:.3f,angle=frog?b.angle:-1.57079632679f;
                        g_attacks[side].skill_damage_circles.push_back({b.x,b.y,24,frog?b.hit_growth:1.f,frog?20:16,b.damage,frog?4:2,std::cos(angle)*speed,std::sin(angle)*speed});
                        // All three callbacks return +ac for the initial contact.
                        world.damage_enemy(*nearest,b.damage);
                        b.speed=speed;play_sfx(65);
                    }else{world.damage_enemy(*nearest,b.damage);
                        if(b.ability_bank&&b.hit_callback==5){PlayerProjectile effect{};effect.x=b.x;effect.y=b.y;effect.side=side;effect.effect_bank=true;effect.angle=b.angle+(th19::AnmVm::rand_unit()*2-1)*.3490658504f;
                            if(bind_player_animation(effect.vm,142,g_focus_entries,g_focus_scripts))g_player_hit_effects.push_back(std::move(effect));}
                        b.speed/=8.f;}
                    if(b.ability_bank||flameHit||frog){b.ending=true;b.damage=0;b.table_acceleration=b.table_turn=0;b.vm.interrupt_all(1);}
                    else {
                        // Enoko's trap is the native update-13 projectile.  Its
                        // contact callback enters the hit/vanish label; keeping
                        // the live SHT record after contact leaves a second trap
                        // body behind when several enemies overlap its rectangle.
                        b.consumed=true;
                    }
                }}
            // Native hit callbacks enter ANM label 2; update returns to 3
            // when contact ends. Keep the beam alive throughout contact.
            if(beam&&previousLaserHit!=(b.target_id!=0))b.vm.interrupt_all(b.target_id?2:3);
        }
        if(b.vm.rotation_mode==1)b.vm.rot_z=b.angle;
        ++b.age;b.vm.tick();
        collect_shot_detached_anms(b);
    }
    for(int side=0;side<2;++side){auto&w=g_ecl_fields[side].world;const bool collectCard=side==0&&std::find(g_owned_cards.begin(),g_owned_cards.end(),4)!=g_owned_cards.end();w.pickup_speed=collectCard?10:5;w.pickup_range=collectCard?110:70;w.tick_pickups(side?g_p2_x:g_p1_x,side?g_p2_y:g_p1_y);g_charge[side].energy=std::clamp(g_charge[side].energy+w.pending_energy,0,2500);w.pending_energy=0;}
    g_player_projectiles.erase(std::remove_if(g_player_projectiles.begin(),g_player_projectiles.end(),[](const auto& b){
        // 0x54df10 skips viewport culling for SHT types 2 and 8. In
        // particular Orin's tail remains visible after its head leaves.
        const bool outside=b.type!=2&&b.type!=8&&(b.y < -64||b.y>512||std::abs(b.x)>220);
        return b.consumed||!b.vm.visible||outside||(b.age>std::max(1200,b.lifetime+120));
    }),g_player_projectiles.end());
    auto emit=[&](int side,int group,int clock,int secondary,bool charged,const PlayerProjectile*card=nullptr){
    const auto&table=side?g_opp_shot_table:g_shot_table;
    if(group<int(table.groups.size()))for(const auto& d:table.groups[group]){
        if(d.option>0&&d.option<5&&d.option>int(g_player_options[side].size()))continue;
        if(d.option>6)continue;
        if(!d.due(clock,secondary))continue;
        if(d.type==2&&std::any_of(g_player_projectiles.begin(),g_player_projectiles.end(),[&](const auto&b){return b.side==side&&b.type==2&&!b.ending&&b.laser_id==d.laser_id&&b.card_id==(card?card->card_id:0);}))continue;
        float x=side?g_p2_x:g_p1_x,y=side?g_p2_y:g_p1_y;
        if(d.option>0&&d.option<5){const auto&o=g_player_options[side][d.option-1];x=o.x;y=o.y;}
        else if(d.option==5){x=g_attacks[side].skill_origin_x;y=g_attacks[side].skill_origin_y;} // player +0xc18/+0xc1c
        else if(d.option==6){x=y=0;} // 0x54e960: constant vector 0x5a03c4
        if(card){x=card->x;y=card->y;}
        PlayerProjectile b{x+d.x,y+d.y,std::cos(d.angle)*d.speed,std::sin(d.angle)*d.speed};
        if(d.init==5||d.init==13){float best=1e20f,angle=d.angle;for(const auto&e:g_ecl_fields[side].world.enemies)if(e.active&&e.sprite>=0&&!(e.flags&3)){float distance=(e.x-b.x)*(e.x-b.x)+(e.y-b.y)*(e.y-b.y);if(distance<best){best=distance;angle=std::atan2(e.y-b.y,e.x-b.x);}}b.angle=angle;b.vx=std::cos(angle)*d.speed;b.vy=std::sin(angle)*d.speed;}
        b.side=side;b.ability_bank=card!=nullptr;b.card_id=card?card->card_id:0;b.init_callback=d.init;
        if(!bind_player_animation(b.vm,d.script,card?g_ability_entries:(side?g_opp:g_player),card?g_ability_scripts:(side?g_opp_scripts:g_player_scripts)))continue;
        if(d.init==9){b.y=480;b.x+=(th19::AnmVm::rand_unit()*2-1)*32;} //54b7a0
        if(d.init==15){b.x+=(th19::AnmVm::rand_unit()*2-1)*32;b.vm.sx=b.vm.sy=2;} // 0x54b9a0
        b.angle=std::atan2(b.vy,b.vx);if(d.speed==0)b.angle=d.angle;b.speed=d.speed;
        if(d.init==17){b.angle=-1.57079632679f+(side?g_opp_direction:g_p1_dir)*3.14159265359f/30;b.vx=std::cos(b.angle)*b.speed;b.vy=std::sin(b.angle)*b.speed;b.length=16;}
        if(d.init==14||d.init==16||d.init==20||d.init==22){b.angle=th19::initializeSpecialShot(d.init,b.angle,th19::AnmVm::rand_unit());b.vx=std::cos(b.angle)*b.speed;b.vy=std::sin(b.angle)*b.speed;}
        b.piercing=d.init==11||d.init==16;b.hit_script=d.hit_script;b.hit_callback=d.hit;b.hit_growth=d.hit_growth;b.damage=d.damage;b.hit_width=d.hit_width;b.hit_height=d.hit_height;b.option=d.option;b.type=d.type;b.update=d.update;b.laser_id=d.laser_id;b.offset_x=d.x;b.offset_y=d.y;
        b.table_turn=d.angular_velocity;b.table_acceleration=d.acceleration;b.lifetime=d.lifetime;b.laser_duration=d.laser_duration;b.transition_frame=d.transition_frame;b.charged=charged;
        play_sfx(d.sound);
        if(d.init==2||d.init==12||d.init==18)play_sfx(20);
        if(charged)++g_attacks[side].charge_attack.emitted;
        collect_shot_detached_anms(b);
        g_player_projectiles.push_back(std::move(b));
    }
    };
    const int power=g_char_sel==3?g_attacks[0].ran_skill.power(g_player_power):g_player_power;
    if(firing){if(!g_test_card_shoot_only)emit(0,power+(focused?5:0),g_shoot_frame,g_shoot_frame,false);for(const auto&o:g_card_options)emit(0,th19::cardOption(o.card_id)->group,g_shoot_frame,g_shoot_frame,false,&o);++g_shoot_frame;}else g_shoot_frame=0;
    if(g_attacks[0].charge_attack.firing()){
        emit(0,g_attacks[0].charge_attack.group+(g_char_sel==3&&g_attacks[0].ran_skill.active()?1:0),g_attacks[0].charge_attack.frame,g_attacks[0].charge_attack.secondary_clock(),true);
        if(g_char_sel==1&&g_charge[0].last_release==2&&g_attacks[0].charge_attack.frame==0)emit(0,11,0,0,true);
    }
    if(rightFiring){emit(1,(g_opp_sel==3?g_attacks[1].ran_skill.power(g_player_power):g_player_power)+((g_opponent_controls&8)?5:0),g_opp_shoot_frame,g_opp_shoot_frame,false);++g_opp_shoot_frame;}else g_opp_shoot_frame=0;
    // Do not carry the disputed Skill side-beam branch into a later C1 release.
    auto&rightAttack=g_attacks[1].charge_attack;
    if(rightAttack.firing())emit(1,rightAttack.group+(g_opp_sel==3&&g_attacks[1].ran_skill.active()?1:0),rightAttack.frame,rightAttack.secondary_clock(),true);
    if(g_opp_sel==1&&g_charge[1].last_release==2&&rightAttack.frame==0)emit(1,11,0,0,true);
    for(auto&a:g_attacks)a.charge_attack.tick();
}
void bind_warning_portrait(int ch,int sender);
bool start_player_bomb(int side);
void tick_cpu_opponent(int side=1){
 if(!g_cpu_automatic)return;
 auto&w=g_ecl_fields[side].world;auto&left=g_ecl_fields[1-side].world;
 auto&bits=side?g_opponent_controls:g_cpu_left_bits;auto&control=side?g_cpu_control:g_cpu_left_control;
 float px=side?g_p2_x:g_p1_x,py=side?g_p2_y:g_p1_y;int character=side?g_opp_sel:g_char_sel;
 if(!g_vs_mode&&side==1){
 const bool bossPresent=left.boss_occupied()||w.boss_occupied();
 // 548bc0/548c60/548d00: player 0 contributes 1/5/10 per event.
 // 51afe0 contributes one each second, not one each frame.
 ++g_cpu_frames;
 g_cpu_story.add(left.pending_story_credit+(g_cpu_frames%60==59?1:0),bossPresent);
 left.pending_story_credit=0;w.pending_story_credit=0;
 const auto event=g_cpu_story.tick(left.boss_occupied());
 if(event.tier){
  g_charge[1].ex_level=std::clamp(g_charge[1].ex_level+event.extraIncrement,0,7);
  left.story_boss_phase=event.rank;if(event.finalBoss)left.story_final_boss=1;
  left.incoming_extra_level=g_charge[1].ex_level;left.incoming_boss_level=g_charge[1].boss_level;
  std::vector<const th19::EclProgram*> source;for(const auto&p:g_ecl_fields[1].programs)source.push_back(p.get());
  if(left.receive_charge_attack(event.tier,0,0,source)){
   ++g_cpu_story_events;g_attack_warning.start(1,event.tier,g_charge[1].ex_level,g_charge[1].boss_level);
   g_attack_warning_vm={};auto it=g_aura_scripts.find(g_attack_warning.aura_script());
   if(it!=g_aura_scripts.end())g_attack_warning_vm.bind_entry(it->second.stream,it->second.len,&g_aura_scripts);
   g_attack_warning_frame=-1;bind_warning_portrait(g_opp_sel,1);play_sfx(77);
  }
 }
 }
 if(!g_player_damage[side].controllable()){bits=0;return;}
 std::vector<th19::CpuAvoidance::Hazard> hazards;
 std::vector<th19::CpuObstacle::Object> obstacles;
 auto add=[&](th19::CpuAvoidance::Hazard h,float angle,float speed,bool line=false){hazards.push_back(h);obstacles.push_back({h,angle,speed,line});};
 for(const auto&b:w.bullets)if(b.state==1)add({b.x,b.y,0,0,0,th19::nativeBulletContactRadius(b.kind),true},std::atan2(b.vy,b.vx),std::hypot(b.vx,b.vy));
 for(const auto&e:w.enemies)if(e.active&&!(e.status_flags&0x200)&&!(e.flags&0x22)&&e.invulnerability_time<1&&!(e.status_flags&0x400)&&e.contact_w>0){
  const bool rect=e.flags&0x1000;add({e.x,e.y,e.contact_w*1.5f,e.contact_h*1.5f,e.contact_angle,e.contact_w*.5f,!rect},std::atan2(e.vy,e.vx),std::hypot(e.vx,e.vy));
 }
 for(const auto&l:w.lasers)if(l.active&&(l.phase==2||l.phase==4)&&l.length>16&&l.width>(l.straight?3.f:0.f)){
  add({l.x+std::cos(l.angle)*l.length*.5f,l.y+std::sin(l.angle)*l.length*.5f,l.length*(l.straight?.8f:.95f),l.collisionWidth(),l.angle,0,false},l.angle,l.source.speed,true);
 }
 std::vector<th19::CpuTarget::Enemy> targets;std::vector<th19::Enemy*> actors;
 for(auto&e:w.enemies)if(e.active&&!(e.status_flags&0x200)&&!(e.flags&0x31)){
  targets.push_back({e.id,e.x,e.y,e.status_flags,e.cpu_target_cooldown});actors.push_back(&e);
 }
 int selected=th19::CpuTarget::select(px,py,targets,character);
 for(size_t i=0;i<actors.size();++i)actors[i]->cpu_target_cooldown=targets[i].cooldown;
 th19::CpuTarget::Pursuit pursuit;int direction=th19::CpuAvoidance::idleDirection(px,py);bool slow=false;
 if(selected>=0){pursuit=th19::CpuTarget::pursue(character,px,py,targets[selected]);direction=pursuit.direction;slow=pursuit.slow;}
 else{const th19::Pickup*nearest=nullptr;float best=120*120;
  for(const auto&i:w.pickups)if(i.active){float dx=i.x-px,dy=i.y-py,d=dx*dx+dy*dy;if(d<best){best=d;nearest=&i;}}
  if(nearest){int dx=(nearest->x>px)-(nearest->x<px),dy=(nearest->y>py)-(nearest->y<py);
   direction=dy<0?(dx<0?5:dx>0?6:1):dy>0?(dx<0?7:dx>0?8:2):dx<0?3:dx>0?4:0;}
 }
 int resolved=th19::CpuObstacle::steer(px,py,direction,obstacles);if(resolved!=direction)++g_cpu_steers;
 auto boundary=th19::CpuAvoidance::boundaryDirection(resolved,px,py);
 const auto&m=g_charge[side];auto positions=th19::CpuAvoidance::positions(px,py,int(m.move_speed*128),int(m.slow_speed*128),int(m.diagonal_speed*128),int(m.slow_diagonal_speed*128));
 auto perception=th19::CpuAvoidance::perceive(positions,hazards,3);
 th19::CpuControl::Frame frame;frame.energy=m.energy;frame.currentCharge=m.charge;frame.thresholds=m.thresholds;
 frame.blocked=perception.blocked;frame.allBlocked=perception.allBlocked;frame.ignoreDangerPercent=g_cpu_difficulty.ignoreDangerPercent;frame.storyController=!g_vs_mode;
 frame.eligibleTargetCount=int(targets.size());frame.pursuit=selected>=0?&pursuit:nullptr;
 frame.activeSpecialTarget=selected>=0&&(targets[selected].status&0x700000)&&!(targets[selected].status&0x80000);
 frame.resolvedDirection=boundary.index;frame.resolvedSlow=slow;frame.boundaryHold=boundary.holdFrames;
 auto result=control.tick(frame,[]{return unsigned(th19::AnmVm::rand_unit()*65536);});if((result.bits&4)&&!(bits&4))start_player_bomb(side);bits=result.bits;
 if(selected>=0&&result.targetCooldown)actors[selected]->cpu_target_cooldown=result.targetCooldown;
}
void tick_charge_release(int side,bool down,bool slow){
 const int ch=side?g_opp_sel:g_char_sel;
 float px=side?g_p2_x:g_p1_x,py=side?g_p2_y:g_p1_y,opx=side?g_p1_x:g_p2_x,opy=side?g_p1_y:g_p2_y;
 float cx=side?GAME_F2_CX:GAME_F1_CX,ocx=side?GAME_F1_CX:GAME_F2_CX;
 auto&entries=side?g_opp:g_player;auto&scripts=side?g_opp_scripts:g_player_scripts;
 auto&bombs=side?g_p2_bombs:g_p1_bombs;auto&fragments=side?g_p2_fragments:g_p1_fragments;
bool held=g_charge[side].held;int level=g_charge[side].level();g_charge[side].charge_speed_percent=side==0&&std::find(g_owned_cards.begin(),g_owned_cards.end(),9)!=g_owned_cards.end()?125:100;g_charge[side].tick(g_player_damage[side].controllable()&&down,g_attacks[side].charge_attack.frame>=0||g_attacks[side].charge_area.frame>=0);
                if(g_charge[side].held&&!held)play_sfx(84);
                if(g_charge[side].level()>level)play_sfx(g_charge[side].level()==4?88:86);
                if(!g_charge[side].held&&held){stop_sfx(84);stop_sfx(88);if(g_charge[side].last_release>=1){
                    // 0x4e79e0 dispatches each Skill before ordinary release.
                    bool areaStarted=false,accepted=false;
                    if(side==1)++g_cpu_releases[std::clamp(g_charge[side].last_release,0,4)];
                    g_attacks[side].skill_origin_x=px;g_attacks[side].skill_origin_y=py;
                    if(g_charge[side].last_release==2){
                        if(ch==0){ReimuSkillVisual skill;skill.state.x=px;skill.state.y=py;skill.state.angle=(th19::AnmVm::rand_unit()*2-1)*3.141592654f;
                            accepted=true;for(auto&v:skill.animations)accepted=bind_player_animation(v,20,entries,scripts)&&accepted;
                            if(accepted){skill.positions=skill.state.centers();g_attacks[side].reimu_skills.push_back(std::move(skill));g_barrier[side].invulnerable=40;play_sfx(49);}}
                        else if(ch==1){auto loc=g_bullet_scripts.find(63);accepted=loc!=g_bullet_scripts.end();
                            // Native EXLaserInf creates six crossing flights to
                            // the opponent.  The separate player-side C2 laser
                            // lanes remain three and are handled by pl01.sht.
                            if(accepted){for(int i=0;i<6;++i){MarisaSkillVisual v;auto&s=v.state;
                                s.duration=th19::MarisaSkill::flightDuration(unsigned(th19::AnmVm::rand_unit()*65536));
                                s.from_x=cx+GAME_SCALE*px;s.from_y=GAME_FY0+GAME_SCALE*py;
                                v.target_x=opx;s.to_x=ocx+GAME_SCALE*v.target_x;s.to_y=GAME_FY0+448*GAME_SCALE;
                                float a=-1.57079632679f+i*6.28318530718f/6;s.tx=600*std::cos(a);s.ty=600*std::sin(a);
                                v.vm.bind_entry(loc->second.stream,loc->second.len,&g_bullet_scripts);v.vm.tick();v.vm.sx=v.vm.sy=.8f*GAME_SCALE;v.vm.r=255;v.vm.g=v.vm.b=128;v.vm.alpha=128;
                                g_attacks[side].marisa_skills.push_back(std::move(v));}play_sfx(51);}}
                        else if(ch==3){accepted=g_attacks[side].ran_skill.release();if(accepted){g_attacks[side].option_lock_x=px;g_attacks[side].option_lock_y=py;tick_player_options(slow);play_sfx(13);}}
                        else if(ch==9&&g_attacks[side].charge_area.frame<0&&th19::mamizouSkillFragment(bombs,fragments))accepted=areaStarted=g_attacks[side].charge_area.release_skill(9,px,py);
                        else if(ch==5||ch==7||ch==8||ch==10||ch==12||ch==18)accepted=areaStarted=g_attacks[side].charge_area.release_skill(ch,px,py);
                        else if(ch==16&&g_attacks[side].chiyari_skill.frame<0){
                            accepted=bind_player_animation(g_attacks[side].chiyari_skill.vm,9,entries,scripts);
                            if(accepted){g_attacks[side].chiyari_skill.frame=0;std::vector<const th19::EclProgram*> source;
                                for(const auto&p:g_ecl_fields[side].programs)source.push_back(p.get());
                                // 0x42b6b0: opposing field, own SkillAttack program, life 4000.
                                g_ecl_fields[1-side].world.spawn_enemy("SkillAttack",0,0,4000,0,0,&source);++g_attacks[side].chiyari_skill_spawns;}}
                        else accepted=g_attacks[side].charge_attack.release_skill(ch);
                        if(accepted){if(side==0)g_cpu_story.add(50,g_ecl_fields[0].world.boss_occupied()||g_ecl_fields[1].world.boss_occupied());g_charge[side].energy=std::max(0,g_charge[side].energy-(g_charge[side].thresholds[1]-g_charge[side].thresholds[0]));
                            // Orin 0x41d1b0 selects group 11 alongside the purification area.
                            if(ch==7)g_attacks[side].charge_attack.release_skill(7);
                            if(ch==7)play_sfx(30);
                            if(ch==4)g_barrier[side].invulnerable=90;
                            if(ch==5)g_barrier[side].invulnerable=40;
                            if(ch==6)play_sfx(49);if(ch==13)play_sfx(45);}
                    }
                    if(g_charge[side].last_release>=3){
                        // 0x4e79e0 C3/C4 -> 0x40b420, before opponent attack dispatch.
                        if(g_barrier[side].restore_from_charge(g_charge[side].last_release))g_barrier_vm[side]={};
                        auto&target=g_ecl_fields[1-side].world;
                        const int tier=g_charge[side].last_release==4&&!target.boss_occupied()?4:3;
                        if(side==0)g_cpu_story.add(tier==4?350:300,g_ecl_fields[0].world.boss_occupied()||g_ecl_fields[1].world.boss_occupied());
                        std::vector<const th19::EclProgram*> source;for(const auto&p:g_ecl_fields[side].programs)source.push_back(p.get());
                        if(target.receive_charge_attack(tier,0,0,source)){
                            auto&count=g_vs_attack_counts[side][tier==4?1:0];count=std::min(999,count+1);
                            g_attack_warning.start(side,tier,g_charge[side].ex_level,g_charge[side].boss_level);
                            g_attack_warning_vm={};auto it=g_aura_scripts.find(g_attack_warning.aura_script());
                            if(it!=g_aura_scripts.end())g_attack_warning_vm.bind_entry(it->second.stream,it->second.len,&g_aura_scripts);
                            g_attack_warning_frame=-1;bind_warning_portrait(ch,side);play_sfx(77);g_charge[side].spend_attack(tier);
                            // Native4e7b2a/4e8184 ->537b90: one fragment per accepted EX/Boss.
                            if(side==0&&th19::ownsCard(g_owned_cards,38)&&th19::mamizouSkillFragment(bombs,fragments))play_sfx(46);
                        }
                    }
                    // 0x4e83b9..0x4e8440 gates the ordinary release on the
                    // charge value. Marisa Skill sets +d4 for a later C1;
                    // it must not create the group 10/11 beams immediately.
                    if(th19::ChargeAttack::ordinary_after_release(ch,g_charge[side].last_release,accepted))g_attacks[side].charge_attack.release(ch);
                    if(areaStarted||g_attacks[side].charge_area.release(ch,px,py)){g_attacks[side].charge_area_x=px;g_attacks[side].charge_area_y=py;bind_player_animation(g_attacks[side].charge_area_vm,g_attacks[side].charge_area.script(),entries,scripts);if(ch==8)bind_player_animation(g_attacks[side].tsukasa_skill_back,13,entries,scripts);if(ch!=7&&ch!=8&&ch!=18)play_sfx(ch==5?29:ch==9?46:g_attacks[side].charge_area.enhanced?(ch==10?41:45):(ch==10?41:44));}
                }}

}
bool start_player_bomb(int side){
    auto&b=g_bombs[side];const int ch=side?g_opp_sel:g_char_sel;
    if(!th19::PlayerBomb::supported(ch)||!g_player_damage[side].controllable())return false;
    auto&scripts=side?g_opp_scripts:g_player_scripts;
    int area=th19::PlayerBomb::areas[ch],back=th19::PlayerBomb::backs[ch];
    if(!scripts.count(area)||!scripts.count(back))return false;
    auto&stock=side?g_p2_bombs:g_p1_bombs;
    if(!b.start(ch,g_charge[side].energy,stock,side?g_p2_x:g_p1_x,side?g_p2_y:g_p1_y,side?g_opp_direction:g_p1_dir))return false;
    if(side==0&&std::find(g_owned_cards.begin(),g_owned_cards.end(),12)!=g_owned_cards.end()){
        b.duration=b.duration*150/100;if(ch==0)b.duration=b.duration*150/100;
    }
    auto bind=[&](th19::AnmVm&v,int script){v={};auto l=scripts.at(script);v.bind_entry(l.stream,l.len,&scripts);v.tick();};
    g_bomb_visual_tail[side]=0;
    bind(g_bomb_area[side],area);bind(g_bomb_back[side],back);
    for(int i=0;i<2;++i){g_bomb_extra[side][i]={};if(i+1<b.count)bind(g_bomb_extra[side][i],area); }
    play_sfx(49);return true;
}
void tick_player_bombs(){
    for(int side=0;side<2;++side){auto&b=g_bombs[side];if(b.frame<0){
            if(g_bomb_visual_tail[side]>0){--g_bomb_visual_tail[side];g_bomb_area[side].tick();g_bomb_back[side].tick();for(auto&v:g_bomb_extra[side])v.tick();}
            continue;
        }
        auto&area=g_bomb_area[side];auto&back=g_bomb_back[side];
        if(b.active()){
            if(b.character==5){
                float tx=side?g_p2_x:g_p1_x,ty=side?g_p2_y:g_p1_y,best=320.f*320.f;
                for(const auto&e:g_ecl_fields[side].world.enemies){
                    if(!e.active||(e.flags&0x21)||!(e.status_flags&0x700000)||(e.status_flags&0x80c00))continue;
                    const float d=(e.x-b.x)*(e.x-b.x)+(e.y-b.y)*(e.y-b.y);
                    if(d<best){best=d;tx=e.x;ty=e.y;}
                }b.seek(tx,ty);
            }
            b.advance(side?g_p2_x:g_p1_x,side?g_p2_y:g_p1_y);
            if(b.character==13)for(int i=0;i<2&&i<int(g_player_options[side].size());++i)b.centers[i+1]={g_player_options[side][i].x,g_player_options[side][i].y};
            auto scale=[&](th19::AnmVm&v){v.sx=b.radius;v.sy=b.width;v.rot_z=b.angle;
                if(b.character==4||b.character==11||b.character==18){v.sy=30;v.secondary_sx=b.width;v.secondary_sy=30;}
            };scale(area);for(int i=1;i<b.count;++i)scale(g_bomb_extra[side][i-1]);
            g_barrier[side].invulnerable=std::max(g_barrier[side].invulnerable,40);
            auto&w=g_ecl_fields[side].world;
            if(b.character==10||b.character==14){
                const int sides=b.character==10?6:8;
                w.cancel_bullets_polygon(b.x,b.y,b.radius,b.angle,sides);
                auto inside=[&](float px,float py){return th19::nativePolygonContact(px-b.x,py-b.y,b.radius,b.angle,sides);};
                w.cancel_lasers_region(inside);
                for(auto&e:w.enemies)if(inside(e.x,e.y))w.damage_enemy(e,b.character==10?20:10);
            }else if(b.character==4||b.character==11||b.character==18){
                auto contact=[&](float px,float py,float r){return b.character==18?th19::nativeStarContact(px-b.x,py-b.y,b.radius,b.width,b.angle,r):th19::nativeEllipseContact(px-b.x,py-b.y,b.radius,b.width,b.angle,r);};
                w.cancel_bullets_shape(contact);w.cancel_lasers_region([&](float px,float py){return contact(px,py,0);});
                for(auto&e:w.enemies)if(contact(e.x,e.y,0))w.damage_enemy(e,10);
            }else if(b.character==1||b.character==3||b.character==7||b.character==8){
                auto rectangle=[&](float x,float y,float width,float height,float angle=0.f){
                    w.cancel_bullets_rect(x,y,width,height,angle);
                    auto inside=[=](float px,float py){float dx=px-x,dy=py-y;return std::abs(dx*std::cos(angle)+dy*std::sin(angle))<=width*.5f&&std::abs(-dx*std::sin(angle)+dy*std::cos(angle))<=height*.5f;};
                    w.cancel_lasers_region(inside);
                    for(auto&e:w.enemies)if(inside(e.x,e.y))w.damage_enemy(e,20);
                };
                if(b.character==7)rectangle(b.x,b.y,b.radius*2,b.radius,b.angle);
                else if(b.character==8)rectangle(b.x,b.y,b.radius*4,b.radius,b.angle);
                else if(b.character==1)rectangle(b.x,224,b.radius*2,448);
                else{rectangle(b.x,b.y,b.radius*7,b.radius);rectangle(b.x,b.y,b.radius,b.radius*7);}
            }else{
                for(int i=0;i<b.count;++i){const auto&p=b.centers[i];
                    w.cancel_bullets_circle(p.x,p.y,b.radius);w.cancel_lasers_circle(p.x,p.y,b.radius);
                    for(auto&e:w.enemies)if(std::hypot(e.x-p.x,e.y-p.y)<=b.radius)w.damage_enemy(e,b.character==9?20:b.character==16?14:10);
                }
            }
        }else{
            if(b.frame==b.duration){area.interrupt_all(1);back.interrupt_all(1);for(int i=1;i<b.count;++i)g_bomb_extra[side][i-1].interrupt_all(1);play_sfx(44);}
            b.advance(0,0);
        }
        // Native ticks write PlayerStat+24 throughout the bomb, then restore
        // PlayerStat+28 (SHT+2c *10, native-stat-init.c) after the ten-frame exit.
        g_charge[side].energy=b.gauge_energy();
        if(!b.occupied())g_charge[side].energy=std::clamp(g_charge[side].thresholds[0],0,2500);
        area.tick();back.tick();for(int i=1;i<b.count;++i)g_bomb_extra[side][i-1].tick();
        if(!b.occupied()){b.frame=-1;g_bomb_visual_tail[side]=10;} // ANM teardown outlives the native bomb controller.
    }
}
void draw_player_bombs(){
    for(int side=0;side<2;++side){const auto&b=g_bombs[side];if(b.frame<0&&g_bomb_visual_tail[side]<=0)continue;
        g_actor_clip=side;
        draw_procedural_node(g_bomb_back[side],side,b.back_x,b.back_y,1,1,26);
        for(int i=0;i<b.count;++i)draw_procedural_node(i?g_bomb_extra[side][i-1]:g_bomb_area[side],side,b.centers[i].x,b.centers[i].y,1,1,29,true,0,true);
    }g_actor_clip=-1;
}

void draw_player_shots(){
    for(int side=0;side<2;++side){
        const auto&sprites=side?g_opp_sprites:g_player_sprites;const auto&textures=side?g_opp_tex:g_player_tex;const auto&entries=side?g_opp:g_player;float cx=side?GAME_F2_CX:GAME_F1_CX;
    g_actor_clip=side;
    for(const auto&skill:g_attacks[side].reimu_skills)for(int i=0;i<3;++i){
        auto drawSkill=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy)->void{
            if(!v.visible)return;x+=(v.x+v.child_x)*sx;y+=(v.y+v.child_y)*sy;
            auto it=sprites.find(v.sprite);if(it!=sprites.end()){auto tx=textures.find(it->second.first);if(tx!=textures.end()){
                emit_quad(31,tx->second,entries[it->second.first].sprites[it->second.second],cx+x*GAME_SCALE,GAME_FY0+y*GAME_SCALE,GAME_SCALE*sx*v.sx,GAME_SCALE*sy*v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1);
                if(!g_skip_present&&!g_draw.empty())g_draw.back().mesh_blend=v.blend_mode;
            }}for(const auto&c:v.children)self(self,c,x,y,sx*v.sx,sy*v.sy);
        };
        drawSkill(drawSkill,skill.animations[i],skill.positions[i].x,skill.positions[i].y,1,1);
    }
    if(g_attacks[side].tsukasa_skill_back.stream&&g_attacks[side].tsukasa_skill_back.visible)draw_procedural_node(g_attacks[side].tsukasa_skill_back,side,g_attacks[side].charge_area_x,g_attacks[side].charge_area_y);
    if(g_attacks[side].charge_area_vm.stream&&g_attacks[side].charge_area_vm.visible){
        draw_procedural_node(g_attacks[side].charge_area_vm,side,g_attacks[side].charge_area_x,g_attacks[side].charge_area_y);
        auto drawArea=[&](auto&&self,const th19::AnmVm&v,float x,float y)->void{
            if(!v.visible)return;x+=v.x+v.child_x;y+=v.y+v.child_y;
            auto it=sprites.find(v.sprite);if(it!=sprites.end()){auto tx=textures.find(it->second.first);if(tx!=textures.end())emit_quad(31,tx->second,entries[it->second.first].sprites[it->second.second],cx+x*GAME_SCALE,GAME_FY0+y*GAME_SCALE,GAME_SCALE*v.sx,GAME_SCALE*v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1);}
            for(const auto&c:v.children)self(self,c,x,y);
        };drawArea(drawArea,g_attacks[side].charge_area_vm,g_attacks[side].charge_area_x,g_attacks[side].charge_area_y);
    }
    }
    auto drawShot=[&](const PlayerProjectile& b){
        g_actor_clip=b.side;const auto&sprites=b.effect_bank?g_focus_sprites:b.ability_bank?g_ability_sprites:(b.side?g_opp_sprites:g_player_sprites);const auto&textures=b.effect_bank?g_focus_tex:b.ability_bank?g_ability_tex:(b.side?g_opp_tex:g_player_tex);const auto&entries=b.effect_bank?g_focus_entries:b.ability_bank?g_ability_entries:(b.side?g_opp:g_player);
        if(b.update==11&&!b.vm.children.empty()){
            auto tail=b.vm.children[0];tail.rot_z+=b.angle;draw_procedural_node(tail,b.side,b.x,b.y);
        }
        auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y,float inheritedX,float inheritedY)->void{
        if(!v.visible)return;x+=(v.x+v.child_x)*inheritedX;y+=(v.y+v.child_y)*inheritedY;
        auto it=sprites.find(v.sprite);
        if(it!=sprites.end()){auto tex=textures.find(it->second.first);if(tex!=textures.end()){
        const auto&sp=entries[it->second.first].sprites[it->second.second];
        float rotation=v.rot_z,flip=1;const float angle=b.angle;
        if(v.rotation_mode==1)rotation=angle;
        else if(v.rotation_mode==2){rotation=angle;flip=std::cos(angle)<0?-1:1;}
        else if(v.rotation_mode==3)rotation=angle+3.14159265359f;
        else if(v.rotation_mode==4)rotation=angle+1.57079632679f;
        else if(v.rotation_mode==5)rotation=angle-1.57079632679f;
        // Only the direction-bound laser strips use the native dynamic
        // length. The rotating muzzle child is a separate, square ANM.
        // Native writes +74/+6c only on the root VM. Saki
        // script8 is an independent subtractive strip with its own extent/UV.
        const bool laserStrip=(b.type==2||b.update==9)&&(&v==&b.vm);
        float sx=v.sx*inheritedX;if(laserStrip)sx*=b.length/std::max(1.f,sp.w);
        emit_quad(30,tex->second,sp,(b.side?GAME_F2_CX:GAME_F1_CX)+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,GAME_SCALE*sx*flip,GAME_SCALE*v.sy*inheritedY,v.anchor_x,v.anchor_y,v.color(),rotation,v.blend_mode==1);
        if(!g_skip_present&&!g_draw.empty()){auto&q=g_draw.back();q.mesh_blend=v.blend_mode;if(laserStrip)q.u1=q.u0+(q.u1-q.u0)*b.length/512.f;q.u0+=v.u_offset;q.u1+=v.u_offset;q.v0+=v.v_offset;q.v1+=v.v_offset;}
        }}for(const auto&c:v.children)self(self,c,x,y,inheritedX*v.sx,inheritedY*v.sy);
        };draw(draw,b.vm,b.x,b.y,1,1);
    };
    if(g_player_damage[0].visible())for(const auto&o:g_card_options)drawShot(o);
    for(const auto&b:g_player_projectiles)drawShot(b);
    for(const auto&b:g_player_hit_effects)drawShot(b);
    g_actor_clip=-1;
    // pl16 ordinal 9 is already in full-screen pixels (native render mode 2).
    for(int side=0;side<2;++side){
    if(g_attacks[side].chiyari_skill.frame>=0&&g_attacks[side].chiyari_skill.vm.visible){const auto&v=g_attacks[side].chiyari_skill.vm;auto mesh=presentation_geometry(v);
        if(!mesh.empty()){DrawCmd q;q.layer=v.layer;q.handle=g_white_tex.handle;q.mesh_blend=v.blend_mode;
            for(const auto&p:mesh)q.mesh.push_back(SpriteVertex{{g_attacks[side].chiyari_skill.x+p.x,g_attacks[side].chiyari_skill.y+p.y,0},1,p.color,{.5f,.5f}});
            g_draw.push_back(std::move(q));}}
    }
    for(const auto&v:g_reflected_flights){auto it=g_bullet_sprites.find(th19::nativeBulletSprite(v.state.type,v.state.color));
        if(it==g_bullet_sprites.end())continue;auto tx=g_bullet_tex.find(it->second.first);if(tx==g_bullet_tex.end())continue;
        auto p=v.state.position();emit_quad(31,tx->second,g_bullet[it->second.first].sprites[it->second.second],p.x,p.y,GAME_SCALE*.5f,GAME_SCALE*.5f,0,0,0xc08080ff,0);
    }
    // Native EXLaserInf draws across both fields, including the centre gap.
    for(int side=0;side<2;++side)for(const auto&skill:g_attacks[side].marisa_skills){const auto&v=skill.vm;auto it=g_bullet_sprites.find(v.sprite);
        if(!v.visible||it==g_bullet_sprites.end())continue;auto tx=g_bullet_tex.find(it->second.first);if(tx==g_bullet_tex.end())continue;
        auto p=skill.state.position();emit_quad(31,tx->second,g_bullet[it->second.first].sprites[it->second.second],p.x,p.y,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1);
    }
}

// Native item script table 0x59afc0 resolves to bullet.anm 115..128.
struct Visual {th19::AnmVm vm;float x=0,y=0;int side=0,script=-1;u32 frame=0;bool effect=false;};
struct ChainVisual {th19::AnmVm vm;float x=0,y=0;int side=0,age=0,duration=0;u32 frame=0;};
std::vector<ChainVisual> rb_draw_combat_items_chainVisuals;
std::unordered_map<u32,Visual> rb_draw_combat_items_items[2];std::vector<Visual> rb_draw_combat_items_deaths,rb_draw_combat_items_cancels;
u32 rb_draw_combat_items_generation=0;
void draw_combat_items(){
    
    
    
    static std::vector<u8> ghostBytes;static std::vector<th19::AnmEntry> ghostEntries;
    static std::unordered_map<i32,th19::AnmScriptLoc> ghostScripts;
    static std::map<i32,TextureAllocation> ghostTextures;
    static std::map<u32,std::pair<i32,i32>> ghostSprites;
    if(ghostScripts.empty()&&load_anm("/ghost.anm",ghostBytes,ghostEntries))
        for(const auto&entry:ghostEntries)for(const auto&sc:entry.scripts)ghostScripts[sc.id]={ghostBytes.data()+entry.base+sc.offset,u32(ghostBytes.size()-entry.base-sc.offset),&entry.scripts};
    if(ghostTextures.empty()&&!ghostEntries.empty()){
        decode_textures(ghostEntries,ghostBytes,ghostTextures);
        for(int i=0;i<int(ghostEntries.size());++i)for(int j=0;j<int(ghostEntries[i].sprites.size());++j)
            ghostSprites[ghostEntries[i].sprites[j].id]={i,j};
    }
    g_actor_clip=-1;
    auto drawSpirit=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy)->void{
        if(!v.visible)return;x+=(v.x+v.child_x)*sx;y+=(v.y+v.child_y)*sy;
        const auto mesh=presentation_geometry(v,sx,sy);
        if(!mesh.empty()){DrawCmd cmd{};cmd.handle=g_white_tex.handle;cmd.layer=34;
            cmd.mesh_fog_enabled=false;cmd.additive=v.blend_mode==1;cmd.mesh_blend=v.blend_mode;
            for(const auto&p:mesh)cmd.mesh.push_back(SpriteVertex{{(x+p.x)*2,(y+p.y)*2,0},1,p.color,{.5f,.5f}});
            g_draw.push_back(std::move(cmd));}
        auto s=ghostSprites.find(v.sprite);if(s!=ghostSprites.end()){auto t=ghostTextures.find(s->second.first);
            if(t!=ghostTextures.end())emit_quad(34,t->second,ghostEntries[s->second.first].sprites[s->second.second],x*2,y*2,sx*v.sx*2,sy*v.sy*2,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);}
        for(const auto&child:v.children)self(self,child,x,y,sx*v.sx,sy*v.sy);
    };
    for(auto&v:g_spirit_flights){if(!v.vm.stream){auto s=ghostScripts.find(v.state.script);if(s!=ghostScripts.end()){v.vm.bind_entry(s->second.stream,s->second.len,&ghostScripts);v.vm.tick();}}
        auto p=v.curve.position();drawSpirit(drawSpirit,v.vm,p.x,p.y,1,1);}
    
    static std::unordered_map<i32,th19::AnmScriptLoc> itemScripts;
    
    if(rb_draw_combat_items_generation!=g_ecl_generation){rb_draw_combat_items_items[0].clear();rb_draw_combat_items_items[1].clear();rb_draw_combat_items_deaths.clear();rb_draw_combat_items_cancels.clear();rb_draw_combat_items_chainVisuals.clear();rb_draw_combat_items_generation=g_ecl_generation;}
    if(itemScripts.empty())for(const auto&e:g_bullet)for(const auto&sc:e.scripts)itemScripts[sc.id]={g_bullet_bytes.data()+e.base+sc.offset,u32(g_bullet_bytes.size()-e.base-sc.offset),&e.scripts};
    auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y,int side,bool effect,float sx,float sy)->void{
        if(!v.visible)return;x+=(v.x+v.child_x)*sx;y+=(v.y+v.child_y)*sy;
        const auto&sp=effect?g_focus_sprites:g_bullet_sprites;const auto&tx=effect?g_focus_tex:g_bullet_tex;const auto&en=effect?g_focus_entries:g_bullet;
        auto a=sp.find(v.sprite);if(a!=sp.end()){auto t=tx.find(a->second.first);if(t!=tx.end())emit_quad(effect?33:32,t->second,en[a->second.first].sprites[a->second.second],(side?GAME_F2_CX:GAME_F1_CX)+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,GAME_SCALE*sx*v.sx,GAME_SCALE*sy*v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z,v.blend_mode==1,v.blend_mode);}
        for(const auto&c:v.children)self(self,c,x,y,side,effect,sx*v.sx,sy*v.sy);
    };
    for(int side=0;side<2;++side){auto&w=g_ecl_fields[side].world;g_actor_clip=side;
        for(const auto&c:w.death_chain_visuals){auto it=ghostScripts.find(16);if(it==ghostScripts.end())continue;
            ChainVisual v;v.x=c.x;v.y=c.y;v.side=side;v.duration=c.duration;v.frame=g_frame;
            v.vm.bind_entry(it->second.stream,it->second.len,&ghostScripts);v.vm.tick();
            auto&i=v.vm.ip_scale;i.active=true;i.dur=c.duration;i.mode=5;i.v0=i.v1=1;i.t0=i.t1=c.target;
            rb_draw_combat_items_chainVisuals.push_back(std::move(v));}w.death_chain_visuals.clear();
        for(const auto&e:w.deaths){Visual v;v.x=e.x;v.y=e.y;v.side=side;v.script=e.script;auto it=g_focus_scripts.find(e.script);if(e.file==1&&it!=g_focus_scripts.end()){v.vm.bind_entry(it->second.stream,it->second.len,&g_focus_scripts);v.vm.rot_z=e.angle;rb_draw_combat_items_deaths.push_back(std::move(v));}play_sfx(e.sound);}w.deaths.clear();
        for(const auto&e:w.bullet_cancels){auto it=itemScripts.find(e.script);Visual v;v.x=e.x;v.y=e.y;v.side=side;
            if(it!=itemScripts.end())v.vm.bind_entry(it->second.stream,it->second.len,&itemScripts);
            // 0x4eeb30 creates only the descriptor's bullet.anm script.
            // A negative script deliberately suppresses the visual.
            if(v.vm.stream)rb_draw_combat_items_cancels.push_back(std::move(v));}
        w.bullet_cancels.clear();
        std::unordered_map<u32,bool> live;
        for(const auto&p:w.pickups){live[p.id]=true;auto&v=rb_draw_combat_items_items[side][p.id];if(!v.vm.stream){auto it=itemScripts.find(114+p.type);if(it==itemScripts.end())continue;v.vm.bind_entry(it->second.stream,it->second.len,&itemScripts);}
            if(!g_paused&&v.frame!=g_frame){v.vm.tick();v.frame=g_frame;}draw(draw,v.vm,p.x,p.y,side,false,1,1);}
        for(auto it=rb_draw_combat_items_items[side].begin();it!=rb_draw_combat_items_items[side].end();)if(!live.count(it->first))it=rb_draw_combat_items_items[side].erase(it);else ++it;
    }
    for(auto&v:rb_draw_combat_items_chainVisuals){g_actor_clip=v.side;
        if(!g_paused&&v.frame!=g_frame){
            // 0x51c8c0 interrupts case 1 exactly ten ticks before region expiry.
            if(v.age==v.duration-10)v.vm.interrupt_all(1);
            v.vm.tick();++v.age;v.frame=g_frame;}
        draw_procedural_node(v.vm,v.side,v.x,v.y);}
    rb_draw_combat_items_chainVisuals.erase(std::remove_if(rb_draw_combat_items_chainVisuals.begin(),rb_draw_combat_items_chainVisuals.end(),[](const auto&v){return !v.vm.visible;}),rb_draw_combat_items_chainVisuals.end());
    for(auto&v:rb_draw_combat_items_deaths){g_actor_clip=v.side;if(!g_paused&&v.frame!=g_frame){v.vm.tick();v.frame=g_frame;}
        if(v.script==21||v.script==22||v.script==25){
            // Player hit/death roots 22/21 use default child transforms.
            // 0x4d0770: +564 is the root anchor, +568 the immediate parent.
            // 0x404050: offsets do NOT inherit scale without flag 0x40000.
            auto visit=[&](auto&&self,const th19::AnmVm&n,bool root)->void{
                if(!n.visible)return;
                float sx=root?1.f:v.vm.sx,sy=root?1.f:v.vm.sy;
                float x=v.x+(root?0.f:v.vm.x+v.vm.child_x)+n.x+n.child_x;
                float y=v.y+(root?0.f:v.vm.y+v.vm.child_y)+n.y+n.child_y;
                auto a=g_focus_sprites.find(n.sprite);if(a!=g_focus_sprites.end()){
                    auto t=g_focus_tex.find(a->second.first);if(t!=g_focus_tex.end())
                    emit_quad(33,t->second,g_focus_entries[a->second.first].sprites[a->second.second],
                        (v.side?GAME_F2_CX:GAME_F1_CX)+GAME_SCALE*x,GAME_FY0+GAME_SCALE*y,
                        GAME_SCALE*sx*n.sx,GAME_SCALE*sy*n.sy,n.anchor_x,n.anchor_y,n.color(),n.rot_z,n.blend_mode==1,n.blend_mode);
                }
                draw_procedural_node(n,v.side,x-(n.x+n.child_x)*sx,y-(n.y+n.child_y)*sy,sx,sy,34,false);
                for(const auto&c:n.children)self(self,c,false);
            };visit(visit,v.vm,true);
        }else{draw(draw,v.vm,v.x,v.y,v.side,true,1,1);draw_procedural_node(v.vm,v.side,v.x,v.y);}
    }
    for(auto&v:rb_draw_combat_items_cancels){g_actor_clip=v.side;if(!g_paused&&v.frame!=g_frame){v.vm.tick();v.frame=g_frame;}draw(draw,v.vm,v.x,v.y,v.side,v.effect,1,1);}
    rb_draw_combat_items_cancels.erase(std::remove_if(rb_draw_combat_items_cancels.begin(),rb_draw_combat_items_cancels.end(),[](const auto&v){return !v.vm.visible;}),rb_draw_combat_items_cancels.end());
    rb_draw_combat_items_deaths.erase(std::remove_if(rb_draw_combat_items_deaths.begin(),rb_draw_combat_items_deaths.end(),[](const auto&v){return !v.vm.visible;}),rb_draw_combat_items_deaths.end());g_actor_clip=-1;
}

// 游戏场绘制：world01 背景 + 1P(左场)/对手(右场) + front.anm 边框。
th19::AnmVm g_warning_portrait;int g_warning_portrait_character=-1;
void bind_warning_portrait(int ch,int side){
    g_warning_portrait={};g_warning_portrait_character=ch;
    for(const auto&e:g_plf[ch])for(const auto&sc:e.scripts)if(sc.id==side)
        g_warning_portrait.bind(g_plf_bytes[ch].data()+e.base+sc.offset,u32(g_plf_bytes[ch].size()-e.base-sc.offset));
}
// 0x4e7610 / 0x4e5850: native font7/9 and12/13, sprite widths and top anchor.
void draw_warning_ascii(const std::string&text,int font,float cx,float y,u32 outline){
    auto glyph=[&](int id)->std::pair<int,const th19::AnmSprite*>{
        for(int ei=0;ei<int(g_ascii.size());++ei)for(const auto&sp:g_ascii[ei].sprites)if(int(sp.id)==id)return {ei,&sp};
        return {-1,nullptr};
    };
    const int base=font==7?0x1e4:0x2a8,shadow=font==7?0x246:0x30c;
    float width=0;for(unsigned char c:text){auto g=glyph(base+c);if(g.second)width+=g.second->w;}
    for(int pass=0;pass<2;++pass){float x=cx-width*.5f;
        for(unsigned char c:text){auto g=glyph((pass?base:shadow)+c);auto main=glyph(base+c);
            if(g.second){auto tx=g_ascii_tex.find(g.first);if(tx!=g_ascii_tex.end())
                emit_quad(42,tx->second,*g.second,x,y,1,1,1,1,pass?0xffffffff:outline,0);}
            if(main.second)x+=main.second->w;
        }
    }
}
void draw_attack_warning(){
    if(!g_attack_warning.visible()&&(!g_attack_warning_vm.stream||!g_attack_warning_vm.visible)&&(!g_warning_portrait.stream||!g_warning_portrait.visible))return;
    if(!g_paused&&g_attack_warning_frame!=g_frame){g_attack_warning_vm.tick();g_warning_portrait.tick();g_attack_warning_frame=g_frame;}
    auto draw=[&](auto&&self,const th19::AnmVm&v,float x,float y,float sx,float sy)->void{
        if(!v.visible)return;const float unit=(v.coordinate_mode==2||v.coordinate_mode==4||v.coordinate_mode==6)?1.f:2.f;
        x+=(v.x+v.child_x)*unit;y+=(v.y+v.child_y)*unit;
        auto mesh=presentation_geometry(v,sx*unit,sy*unit);if(!mesh.empty()){
            DrawCmd cmd{};cmd.handle=g_white_tex.handle;cmd.layer=v.layer==29?39:40;cmd.mesh_fog_enabled=false;cmd.mesh_blend=v.blend_mode;cmd.additive=v.blend_mode==1;
            for(const auto&p:mesh)cmd.mesh.push_back(SpriteVertex{{x+p.x,y+p.y,0},1,p.color,{.5f,.5f}});
            g_draw.push_back(std::move(cmd));}
        for(const auto&c:v.children)self(self,c,x,y,sx*v.sx,sy*v.sy);
    };draw(draw,g_attack_warning_vm,0,0,1,1);
    const auto&v=g_warning_portrait;const int ch=g_warning_portrait_character;
    if(ch>=0&&v.visible&&v.alpha>0)for(int ei=0;ei<int(g_plf[ch].size());++ei)for(const auto&sp:g_plf[ch][ei].sprites)if(int(sp.id)==v.sprite){
        auto t=g_plf_tex[ch].find(ei);if(t!=g_plf_tex[ch].end())emit_quad(41,t->second,sp,v.x,v.y,v.sx,v.sy,v.anchor_x,v.anchor_y,v.color(),v.rot_z);
    }
    if(g_attack_warning.visible()){
    const float x=g_attack_warning.x()*2;
    draw_warning_ascii("Warning !!",7,x,th19::AttackWarning::title_y*2,g_attack_warning.shadow_color());
    draw_warning_ascii(g_attack_warning.detail(g_story_difficulty),12,x,th19::AttackWarning::detail_y*2,g_attack_warning.shadow_color());
    if(!g_paused)g_attack_warning.after_draw();
    }
}

// Base native collision/death path. Character-specific rescue callbacks and
// transformed hitboxes remain separate from the verified ordinary contacts.
void tick_player_damage(){
    if(g_screen!=SCR_GAME||g_big_sprite>=0)return;
    for(int side=0;side<2;++side){
        auto&d=g_player_damage[side];auto&barrier=g_barrier[side];
        auto&w=g_ecl_fields[side].world;auto&lives=side?g_p2_lives:g_p1_lives;
        auto&px=side?g_p2_x:g_p1_x;auto&py=side?g_p2_y:g_p1_y;
        // Native ECL1036 calls death directly on the opposite player, bypassing
        // collision, barrier and the eight-frame hit grace; retain death VFX
        // and the existing 30-frame result transition instead of instant victory.
        auto&forced=g_ecl_fields[1-side].world.opponent_defeat_requested;
        th19::PlayerDamage::Event event;
        if(forced){forced=false;lives=0;event=d.die(lives,barrier.invulnerable);}
        else event=d.tick(lives,barrier.invulnerable);
        if(event==th19::PlayerDamage::Death){
            w.deaths.push_back({px,py,0,21,-1,1});
            // 0x549830 resets the automatic barrier and grants 180 invulnerability.
            barrier.reset(barrier.recovery,barrier.increase);barrier.invulnerable=180;g_barrier_vm[side]={};
            g_charge[side].held=false;g_charge[side].charge=0;
            g_player_projectiles.erase(std::remove_if(g_player_projectiles.begin(),g_player_projectiles.end(),[side](const auto&b){return b.side==side;}),g_player_projectiles.end());
            g_attacks[side].charge_attack={};g_attacks[side].charge_area={};
        }
        if(event==th19::PlayerDamage::Respawn){px=0;py=480;}
        if(d.state==th19::PlayerDamage::Respawning)py=d.respawnY();
        if(d.state==th19::PlayerDamage::Repelled){px+=std::cos(d.angle)*d.repelSpeed();py+=std::sin(d.angle)*d.repelSpeed();
            const auto&sprites=side?g_opp_sprites:g_player_sprites;const auto&entries=side?g_opp:g_player;
            auto sp=sprites.find(side?g_opp_vm.sprite:g_player_vm.sprite);float mx=16,my=24;
            if(sp!=sprites.end()){const auto&s=entries[sp->second.first].sprites[sp->second.second];mx=s.w*.5f;my=s.h*.5f;}
            px=std::clamp(px,-GAME_FIELD_W*.5f+mx,GAME_FIELD_W*.5f-mx);py=std::clamp(py,my,GAME_FIELD_H-my);
        }
        if(event==th19::PlayerDamage::Death)++g_stage_deaths[side];
        if(event==th19::PlayerDamage::Defeat){resolve_story_battle(side==1);return;}
    }
}
void check_player_contacts(){
    if(!g_player_collision_enabled||g_screen!=SCR_GAME||g_big_sprite>=0)return;
    for(int side=0;side<2;++side){
        auto&d=g_player_damage[side];auto&barrier=g_barrier[side];
        auto&w=g_ecl_fields[side].world;float px=side?g_p2_x:g_p1_x,py=side?g_p2_y:g_p1_y;
        const float cardRadius=side==0&&std::find(g_owned_cards.begin(),g_owned_cards.end(),2)!=g_owned_cards.end()?1.5f:3.f;
        w.tick_bullet_graze(px,py,cardRadius);
        if(!d.vulnerable(barrier.invulnerable))continue;
        bool hit=false;float sx=0,sy=0;
        for(auto&b:w.bullets){
            // Native ordinary bullets use the table radius, never sprite size.
            if(b.state!=1)continue;
            if(th19::nativeOrdinaryBulletContact(b.kind,px-b.x,py-b.y,th19::nativeBulletContactRadius(b.kind),cardRadius)==1){hit=true;sx=b.x;sy=b.y;break;}
        }
        if(!hit)for(const auto&e:w.enemies){
            // 0x509980: bit 1/5 excludes contact, status 0x400 excludes contact;
            // ECL557 enables the rotated rectangle path (549570 / 4d44e0).
            if(!e.active||e.sprite<0||(e.flags&0x22)||(e.status_flags&0x400))continue;
            if(side==0&&th19::cardIgnoresSpirit(g_owned_cards,e.status_flags))continue;
            if((e.flags&0x1000)?th19::nativeEnemyRectangleContact(px-e.x,py-e.y,e.contact_angle,e.contact_w,e.contact_h,cardRadius):th19::nativeCircleContact(px-e.x,py-e.y,3,e.contact_w*.5f)){hit=true;sx=e.x;sy=e.y;break;}
        }
        if(!hit)for(const auto&laser:w.lasers){
            if(laser.hitsPlayer(px,py,cardRadius)){hit=true;sx=laser.x;sy=laser.y;break;}
        }
        if(!hit)continue;
        bool shield=trigger_player_barrier(side);d.contact(shield,px,py,sx,sy,barrier.invulnerable);
        if(shield)play_sfx(27);
        else{play_sfx(2);w.deaths.push_back({px,py,0,22,-1,1});g_charge[side].held=false;g_charge[side].charge=0;}
    }
}

void draw_game() {
    // world01.std 分层滚动背景（bg 远景/nd 近景 视差滚动）
    draw_world_background();
    // 1P 左场（idle/方向精灵）
    draw_card_reward();
    if(g_screen==SCR_DIALOG||g_player_damage[0].visible())draw_field_player(g_player_vm, g_player_sprites, g_player, g_player_tex, g_p1_x, g_p1_y, GAME_F1_CX);
    // 对手右场（CPU 输入驱动）
    if (g_opp_vm.sprite >= 0&&(g_screen==SCR_DIALOG||g_player_damage[1].visible()))
        draw_field_player(g_opp_vm, g_opp_sprites, g_opp, g_opp_tex, g_p2_x, g_p2_y, GAME_F2_CX);
    // ECL 出怪/弹幕对象（动物灵敌人 + 释放动物灵 + 弹幕）
    draw_ecl_objects();
    draw_charge_regions();
    draw_story_actor();
    draw_player_options();
    draw_player_bombs();
    draw_player_shots();
    draw_combat_items();
    draw_focus();
    // front.anm 对战场边框（layer 27）
    draw_field_frame();
    // HUD：命数/boom/充能槽（1P 左场 + 对手右场）
    draw_field_hud(GAME_F1_CX, g_p1_lives, g_p1_bombs, g_p1_charge, g_p1_fragments);
    draw_field_hud(GAME_F2_CX, g_p2_lives, g_p2_bombs, g_p2_charge, g_p2_fragments);
    // 大字提示（勝利/敗北/弾幕開始等，layer 40）
    draw_big_text();draw_battle_report();
    if(g_vs_mode&&g_vs_match.limit>0){char timer[16];std::snprintf(timer,sizeof(timer),"%d",(g_vs_match.remaining+59)/60);draw_warning_ascii(timer,4,636,0,0xff000000);}
    // 符卡名横幅
    draw_spell_name();
    draw_attack_warning();
    // 能力卡菜单（A 键切换）
}

void render() {
    if(!g_skip_present)gfx.clear(0xff000000);
    g_draw.clear();
    // 进入游戏加载页：少女祈祷中（LoadingInf），短暂显示后进游戏场
    if (g_game_load_frames > 0) {
        for (const auto& vm : g_loading_vms) collect_loading_vm(vm);
        if(!g_skip_present){draw_trophy_popup();flush_draw();gfx.present();}
        return;
    }
    if(g_screen==SCR_STAFF){draw_staff();if(!g_skip_present){draw_trophy_popup();flush_draw();gfx.present();}return;}
    // 正式进入游戏场
    if (g_screen == SCR_GAME) {
        draw_game();
        draw_pause();
        draw_story_switch();
        if(g_final_exit>=180){th19::AnmSprite black;black.w=black.h=1;emit_quad(1000,g_white_tex,black,0,0,1280,960,1,1,u32(std::min(255,(g_final_exit-180)*255/60))<<24,0);}
        if(!g_skip_present){draw_trophy_popup();flush_draw();gfx.present();}
        return;
    }
    // 剧情对话
    if (g_screen == SCR_DIALOG) {
        draw_dialogue();
        draw_pause();
        draw_story_switch();
        if(g_final_exit>=180){th19::AnmSprite black;black.w=black.h=1;emit_quad(1000,g_white_tex,black,0,0,1280,960,1,1,u32(std::min(255,(g_final_exit-180)*255/60))<<24,0);}
        if(!g_skip_present){draw_trophy_popup();flush_draw();gfx.present();}
        return;
    }
    if (g_phase == PHASE_TITLE) {
        // 背景（星空）始终显示，转场不淡出
        g_alpha_scale = 1.0f;
        for (const auto& vm : g_bg_vms) collect_vm(vm);
        for(auto&cmd:g_draw)if(cmd.layer==25){
            const float k=g_title_wave.background/255;
            const u32 t=cmd.tint;
            cmd.tint=(t&0xff000000)|(u32(((t>>16)&255)*k)<<16)|(u32(((t>>8)&255)*k)<<8)|u32((t&255)*k);
        }
        draw_title_wave();
        if (g_screen == SCR_TITLE) {
            // 主菜单：灵梦/logo/菜单图标/立绘 + 菜单文字 + 波纹，转场时整体淡出
            g_alpha_scale = g_fade;
            for (const auto& vm : g_title_vms) collect_vm(vm);
            draw_menu();
            draw_title_version();
        } else {
            // 子界面：半透明信息底板 + 标题条/图标 + 内容，转场时淡入
            g_alpha_scale = g_fade;
            for (const auto& vm : g_sub_vms) collect_vm(vm);
            draw_sub_screen();
        }
        g_alpha_scale=1;for(const auto&v:g_retiring_menu)collect_vm(v,0,0,g_retiring_vs);
        draw_story_switch();
    } else { // PHASE_LOADING
        // loading screen: sig.anm copyright splash + 少女祈祷中 animation
        draw_sig_background();
        for (const auto& vm : g_loading_vms) collect_loading_vm(vm);
    }
    draw_route_notice();
    if(!g_skip_present){draw_trophy_popup();flush_draw();gfx.present();}
}

bool rb_frame_oldEsc=false,rb_frame_oldUp=false,rb_frame_oldDown=false,rb_frame_oldOk=false,rb_frame_oldCancel=false;
bool rb_frame_oldLeft=false,rb_frame_oldRight=false;
void frame() {
    ++g_frame;
    ++g_sfx_tick;
    const bool* pauseKeys=game_keyboard_state();
    auto pressed=[&](int code){return pauseKeys[code]||g_injected_keys[code];};
    
    if(g_network_active&&g_network_frame==0)rb_frame_oldEsc=rb_frame_oldUp=rb_frame_oldDown=rb_frame_oldOk=rb_frame_oldCancel=false;
    bool esc=pressed(SDL_SCANCODE_ESCAPE),pu=pressed(SDL_SCANCODE_UP),pd=pressed(SDL_SCANCODE_DOWN);
    bool ok=pressed(SDL_SCANCODE_Z)||pressed(SDL_SCANCODE_RETURN),cancel=pressed(SDL_SCANCODE_X);
    const bool pauseContext=(g_screen==SCR_GAME||g_screen==SCR_DIALOG)&&g_game_load_frames==0&&g_trans==TRANS_NONE&&g_story_switch.phase==0;
    const bool wasPaused=g_paused;
    if(pauseContext&&g_paused&&g_pause_animation.retryMenu){
        if(g_pause_animation.inputReady()){
            if((pu&&!rb_frame_oldUp)||(pd&&!rb_frame_oldDown)){g_pause_choice=1-g_pause_choice;g_pause_animation.select(g_pause_choice);play_sfx(10);}
            if(ok&&!rb_frame_oldOk){g_pause_animation.close(g_pause_choice==1);play_sfx(7);}
        }
    }else if(pauseContext){
        if(esc&&!rb_frame_oldEsc){if(g_paused){if(g_pause_view==1&&g_help_mode==1){start_help_menu();}else if(g_pause_view==4)close_owned_card_menu();else if(g_pause_view)g_pause_view=0;else if(g_pause_animation.inputReady()){if(g_pause_confirm){g_pause_confirm=false;g_pause_animation.cancel();}else g_pause_animation.close(false);}}else{g_paused=true;g_pause_choice=0;g_pause_confirm=false;init_pause_animation();play_sfx(14);}}
        else if(g_paused){
            if(g_pause_view){
                
                bool left=pressed(SDL_SCANCODE_LEFT),right=pressed(SDL_SCANCODE_RIGHT);
                if(cancel&&!rb_frame_oldCancel){if(g_pause_view==1&&g_help_mode==1){start_help_menu();}else if(g_pause_view==3)g_pause_view=2;else if(g_pause_view==4)close_owned_card_menu();else g_pause_view=0;play_sfx(9);}
                else if(g_pause_view==1){
                    if(g_help_mode==0){
                        if((pu&&!rb_frame_oldUp)||(left&&!rb_frame_oldLeft)){g_help_sel=std::max(0,g_help_sel-1);play_sfx(10);}
                        if((pd&&!rb_frame_oldDown)||(right&&!rb_frame_oldRight)){g_help_sel=std::min(12,g_help_sel+1);play_sfx(10);}
                        if(ok&&!rb_frame_oldOk){g_help_page=g_help_sel;g_help_mode=1;g_help_anim=1;g_help_anim_frame=0;play_sfx(7);}
                    }else if(g_help_anim==0){
                        if(pu&&!rb_frame_oldUp&&g_help_page>0){g_help_anim=3;g_help_anim_next=2;g_help_anim_frame=0;play_sfx(7);}
                        else if(pd&&!rb_frame_oldDown&&g_help_page<12){g_help_anim=2;g_help_anim_next=1;g_help_anim_frame=0;play_sfx(7);}
                        else if(ok&&!rb_frame_oldOk){g_help_anim=3;g_help_anim_next=3;g_help_anim_frame=0;play_sfx(9);}
                    }
                }
                else if(g_pause_view==4){if((left&&!rb_frame_oldLeft)||(pu&&!rb_frame_oldUp)){select_owned_card(-1);play_sfx(10);}if((right&&!rb_frame_oldRight)||(pd&&!rb_frame_oldDown)){select_owned_card(1);play_sfx(10);}}
                else if(g_pause_view==2){
                    if(pu&&!rb_frame_oldUp)g_sub_sel=(g_sub_sel+5)%6;if(pd&&!rb_frame_oldDown)g_sub_sel=(g_sub_sel+1)%6;
                    if(left&&!rb_frame_oldLeft)option_adjust(-1);if(right&&!rb_frame_oldRight)option_adjust(1);
                    if(ok&&!rb_frame_oldOk){if(g_sub_sel==5)g_pause_view=0;else if(g_sub_sel==3){g_pause_view=3;g_key_sel=0;g_key_mode=0;}else if(g_sub_sel==4){g_bgm_vol=100;g_se_vol=80;apply_option_volumes();}}
                }else {if(pu&&!rb_frame_oldUp)g_key_sel=std::max(0,g_key_sel-1);if(pd&&!rb_frame_oldDown)g_key_sel=std::min(g_key_mode?9:1,g_key_sel+1);if(ok&&!rb_frame_oldOk)g_key_mode=1;}
                rb_frame_oldLeft=left;rb_frame_oldRight=right;
            }
            else if(!g_pause_animation.inputReady()){}
            else if(cancel&&!rb_frame_oldCancel){if(g_pause_confirm){g_pause_confirm=false;g_pause_animation.cancel();}else g_pause_animation.close(false);play_sfx(9);}
            else if((pu&&!rb_frame_oldUp)||(pd&&!rb_frame_oldDown)){if(g_pause_confirm){g_pause_answer=1-g_pause_answer;g_pause_animation.selectAnswer(g_pause_answer);}else {do{g_pause_choice=(g_pause_choice+(pu?4:1))%5;}while(g_pause_choice==2&&g_owned_cards.empty());g_pause_animation.select(g_pause_choice);}play_sfx(10);}
            else if(ok&&!rb_frame_oldOk){play_sfx(7);
                if(!g_pause_confirm){if(g_pause_choice==0)g_pause_animation.close(false);
                    else if(g_pause_choice==2){g_pause_view=4;start_owned_card_menu();}
                    else if(g_pause_choice>=3){g_pause_view=g_pause_choice==3?1:2;g_help_sel=0;g_help_page=0;start_help_menu();g_sub_sel=0;}
                    else{g_pause_confirm=true;g_pause_answer=1;g_pause_animation.confirm();}}
                else if(g_pause_answer==1){g_pause_confirm=false;g_pause_animation.cancel();}
                else g_pause_animation.close(true);
            }
        }
    }
    rb_frame_oldEsc=esc;rb_frame_oldUp=pu;rb_frame_oldDown=pd;rb_frame_oldOk=ok;rb_frame_oldCancel=cancel;
    if(g_paused||wasPaused||(pauseContext&&esc)){
        // Native pause freezes BGM playback. Keep the decoder alive so resume
        // does not restart the track from its intro point.
        if(g_paused&&!wasPaused&&g_audio.music){
            ma_sound_stop(&g_audio.music->sound);
            if(g_audio.stream)SDL_ClearAudioStream(g_audio.stream);
        }
        if(g_paused){tick_pause_animation();if(g_pause_view==4)tick_owned_card_menu();if(g_pause_view==1){if(g_help_mode==0)for(auto&vm:g_help_vms)vm.tick();tick_help_transition();}}
        if(!g_paused&&g_music_enabled&&g_audio.music)ma_sound_start(&g_audio.music->sound);
        g_audio.pump();render();return;
    }
    if(g_audio.music)ma_sound_set_volume(&g_audio.music->sound,g_bgm_vol/100.0f);
    if(g_battle_report_frame>=0){tick_battle_report();g_audio.pump();render();return;}
    if((g_screen==SCR_GAME||g_screen==SCR_DIALOG)&&g_attack_warning.advance_freeze()){g_audio.pump();render();return;}
    if(g_story_switch.phase){tick_story_switch();g_audio.pump();render();return;}
    if(g_vs_mode&&g_screen==SCR_GAME&&g_big_sprite<0&&g_vs_match.tick()){
        const int winner=th19::VsMatch::timeout(g_stage_deaths[0],g_stage_deaths[1],g_vs_attack_counts[0][0]+g_vs_attack_counts[0][1],g_vs_attack_counts[1][0]+g_vs_attack_counts[1][1],g_charge[0].energy,g_charge[1].energy);
        begin_battle_report(winner,true);
        g_audio.pump();render();return;
    }
    // 5593b0/5596a0 cancel states return after six ticks; keep the
    // departing VMs alive so their own 15/20-frame exit scripts can finish.
    if(!g_retiring_menu.empty()){for(auto&v:g_retiring_menu)v.tick();if(++g_retiring_age>=60)g_retiring_menu.clear();}
    if(g_menu_cancel_frame>=0&&++g_menu_cancel_frame>=6){
        g_menu_cancel_frame=-1;g_retiring_vs=g_screen==SCR_VS;g_retiring_menu=std::move(g_sub_vms);g_retiring_age=6;
        g_screen=g_next_screen;g_trans=TRANS_NONE;g_fade=1;g_cancel_fade=1;g_cancel_slide=0;
        if(g_screen==SCR_TITLE){g_sel=g_vs_mode?1:0;bind_title_vms(true);g_title_frame=271;g_logo_fast=0;}
        else{g_sub_sel=g_vs_entry;start_sub_screen(g_screen);}
    }
    // 556340 state5: after six ticks release the VS selection VMs using
    // their exit interrupt; the difficulty screen restores the chosen rank.
    if(g_charsel_cancel_frame>=0&&++g_charsel_cancel_frame>=6){
        g_charsel_cancel_frame=-1;g_retiring_menu.clear();g_retiring_age=0;g_retiring_vs=false;
        for(auto&v:g_vs_cursor_vm){v.interrupt_all(1);g_retiring_menu.push_back(std::move(v));v={};}
        for(auto&v:g_vs_portrait_vm){v.interrupt_all(1);g_retiring_menu.push_back(std::move(v));v={};}
        g_screen=SCR_STORY;g_sub_sel=g_story_difficulty;g_story_cursor=float(g_sub_sel);
        g_trans=TRANS_NONE;g_fade=1;start_sub_screen(SCR_STORY);
    }
    // 5596a0 state3 changes page at14 while the original ANM exit continues.
    if(g_difficulty_confirm_frame>=0&&++g_difficulty_confirm_frame>=14){
        g_difficulty_confirm_frame=-1;g_retiring_vs=false;
        g_retiring_menu=std::move(g_sub_vms);g_retiring_age=14;
        g_screen=SCR_CHARSEL;g_trans=TRANS_NONE;g_fade=1;
        g_charsel_frame=0;g_cursor_select=0;g_char_switch_frame=0;
        g_cursor_cx=CH_CX[g_char_sel];g_cursor_cy=CH_CY[g_char_sel];
        start_sub_screen(SCR_CHARSEL);
    }
    // 556340 state4: confirmation animation, screen switch at10, battle at40.
    if(g_vs_match_confirm>=0&&++g_vs_match_confirm==10){
        g_vs_match_confirm=-1;g_story_switch.vs_match=g_vs_mode;g_story_switch.story_start=!g_vs_mode;
        if(bind_story_switch(3))g_story_switch.phase=1;
        else if(g_vs_mode)start_vs_match();else start_dialogue();
    }
    // 5593b0 state3: after fourteen ticks enter native state8 (difficulty).
    if(g_vs_entry_transition>=0&&++g_vs_entry_transition>=14){
        g_vs_entry_transition=-1;
        if(g_vs_entry==3){g_screen=SCR_VSRULES;g_sub_sel=0;g_vs_rule_edit=false;start_sub_screen(g_screen);}
        else if(g_vs_entry==4){g_key_return_screen=SCR_VS;g_screen=SCR_KEYCONFIG;start_sub_screen(g_screen);}
        else {g_screen=SCR_STORY;g_sub_sel=1;g_story_cursor=1;
            start_sub_screen(SCR_STORY);}

    }
    if (g_bob_phase > 0) --g_bob_phase;   // bob counts down after a selection move
    // 进入游戏加载页：倒计时（少女祈祷中），归零进游戏场
    if (g_game_load_frames > 0) {
        --g_game_load_frames;
        for (auto& vm : g_loading_vms) vm.tick();
        if (g_game_load_frames == 0) {
            g_screen = g_vs_mode ? SCR_GAME : SCR_DIALOG;
            if(g_vs_mode)show_big_text(175);
        }
        g_audio.pump();
        render();
        if(!g_skip_present)EM_ASM({ if (Module.onGameFrame) Module.onGameFrame(1, 0); });
        return;
    }
    // 游戏场内 tick 玩家 idle 动画（其余转场逻辑照常走）
    if (g_screen == SCR_GAME || g_screen == SCR_DIALOG) { g_player_vm.tick(); g_opp_vm.tick(); tick_player_options(pressed(SDL_SCANCODE_LSHIFT)||pressed(SDL_SCANCODE_RSHIFT)); ++g_bg_scroll; tick_world(); if (g_screen == SCR_GAME && g_big_sprite < 0 && g_ecl_ready) { for(int side=0;side<2;++side){auto& field=g_ecl_fields[side];field.world.player_x=side?g_p2_x:g_p1_x;field.world.player_y=side?g_p2_y:g_p1_y;field.world.opponent_x=side?g_p1_x:g_p2_x;field.world.opponent_y=side?g_p1_y:g_p2_y;field.world.opponent_controls=side?((pressed(SDL_SCANCODE_LEFT)?64:0)|(pressed(SDL_SCANCODE_RIGHT)?128:0)|g_cpu_left_bits):g_opponent_controls;field.world.incoming_extra_level=g_charge[1-side].ex_level;field.world.incoming_boss_level=g_charge[1-side].boss_level;field.vm.step();field.world.tick_wave();for(const auto&request:field.world.screen_shake_events)g_screen_shake.start(request);field.world.screen_shake_events.clear();for(int sound:field.world.sound_events)play_sfx(sound);field.world.sound_events.clear();} } }
    if (g_screen == SCR_CHARSEL) {
        if(!g_vs_mode)g_vs_cursor_vm[0].tick();
        if(g_vs_mode){for(auto&list:g_vs_card_vms)for(auto&v:list)v.tick();for(auto&vm:g_vs_cursor_vm)vm.tick();for(auto&vm:g_vs_portrait_vm)vm.tick();}
        g_p2_cursor_cx+=(CH_CX[g_opp_sel]-g_p2_cursor_cx)*0.28f;
        g_p2_cursor_cy+=(CH_CY[g_opp_sel]-g_p2_cursor_cy)*0.28f;
        if(std::fabs(CH_CX[g_opp_sel]-g_p2_cursor_cx)<.5f)g_p2_cursor_cx=CH_CX[g_opp_sel];
        if(std::fabs(CH_CY[g_opp_sel]-g_p2_cursor_cy)<.5f)g_p2_cursor_cy=CH_CY[g_opp_sel];
        ++g_charsel_frame;                             // 入场帧计数（标题/图标/Clear/光标弹出）
        ++g_char_switch_frame;                         // 切换帧计数（立绘/称号/介绍滑入）
        g_cursor_cx += (CH_CX[g_char_sel] - g_cursor_cx) * 0.28f;  // 光标滑动缓动
        g_cursor_cy += (CH_CY[g_char_sel] - g_cursor_cy) * 0.28f;
        if (std::fabs(CH_CX[g_char_sel] - g_cursor_cx) < 0.5f) g_cursor_cx = CH_CX[g_char_sel];
        if (std::fabs(CH_CY[g_char_sel] - g_cursor_cy) < 0.5f) g_cursor_cy = CH_CY[g_char_sel];
    }
    if (g_cursor_select > 0) --g_cursor_select;        // 选择动画倒计时
    if (g_flash_phase > 0) --g_flash_phase;  // 确认闪烁相位逐帧递减
    if (g_confirm_delay > 0) {
        // 确认后先闪烁，延迟归零才触发淡出转场
        if (--g_confirm_delay == 0) { g_trans = TRANS_OUT; g_trans_frame = 0; }
    }
    if (g_cancel_delay > 0) {
        // X 取消：图标向中心聚拢 + 淡出（20帧），结束后转场回菜单
        g_cancel_fade = (g_cancel_delay > 20) ? 1.0f : float(g_cancel_delay - 1) / 20.0f;  // >20 为闪烁延迟阶段（不淡出）
        g_cancel_slide = 1.0f - g_cancel_fade;
        if (--g_cancel_delay == 0) { g_trans = TRANS_OUT; g_trans_frame = 0; }
    }
    // 切换难度/菜单的滑动演出：游标缓动逼近选中项（脚本19 CASE 7-10 posTime 30帧）
    g_story_cursor += (g_sub_sel - g_story_cursor) * 0.18f;
    g_vs_cursor    += (g_sub_sel - g_vs_cursor)    * 0.18f;
    if (std::fabs(g_sub_sel - g_story_cursor) < 0.01f) g_story_cursor = float(g_sub_sel);
    if (std::fabs(g_sub_sel - g_vs_cursor)    < 0.01f) g_vs_cursor    = float(g_sub_sel);
    // Music 列表平滑滚动：缓动逼近整数目标
    if (g_screen == SCR_MUSIC) {
        g_music_scroll_cur += (float(g_music_scroll) - g_music_scroll_cur) * 0.25f;
        if (std::fabs(float(g_music_scroll) - g_music_scroll_cur) < 0.01f) g_music_scroll_cur = float(g_music_scroll);
    }
    update_panel();   // EXE title background mesh and menu-dependent color
    if (g_phase == PHASE_TITLE) {
        if(g_screen==SCR_TITLE&&g_title_v_vm.stream)g_title_v_vm.tick();
        ++g_title_frame;   // 标题场景帧计数（驱动选项入场顺序）
        if(g_screen==SCR_ACHIEVE){
            ++g_achievement_frame;
            if(!g_achievement_exiting){
                const int age=g_achievement_detail_frame++;
                if(age==0){g_achievement_details[0].interrupt(4);for(int i=1;i<4;++i)g_achievement_details[i].interrupt(5);}
                for(int i=1;i<4;++i)if(age==1+2*i)g_achievement_details[i].interrupt(4);
            }
            for(auto&v:g_achievement_details)v.tick();
        }
        for (auto& vm : g_bg_vms) vm.tick();
        if (g_logo_fast > 0) {
            for (i32 k = 0; k < 8; ++k) for (auto& vm : g_title_vms) vm.tick();
            --g_logo_fast;
        } else {
            for (auto& vm : g_title_vms) vm.tick();
        }
        if (g_trans == TRANS_NONE) {
            if (g_screen != SCR_TITLE) for (auto& vm : g_sub_vms) vm.tick();
            if (g_screen == SCR_MANUAL && g_help_mode == 0) for (auto& vm : g_help_vms) vm.tick();
            // 进入 Music：子 VM 首次 tick 已 createChild 出列表项，发 case=sel+7 驱动选中态
            if (g_screen == SCR_MUSIC) {
                if (g_sub_case_delay > 0 && --g_sub_case_delay == 0) {
                    for (auto& vm : g_sub_vms) vm.interrupt_all(g_sub_sel + 7);
                }
            }
        }
        if(g_screen==SCR_MANUAL)tick_help_transition();
        if(g_screen==SCR_MUSIC)tick_music_animation();
    } else { // PHASE_LOADING
        for (auto& vm : g_loading_vms) vm.tick();
        if (g_frame >= LOADING_FRAMES) { g_phase = PHASE_TITLE; g_frame = 0; g_title_frame = 0; start_title_music(); }
    }
    g_audio.pump();
    // 转场推进：当前屏内容淡出 → 切屏 → 新屏内容淡入（背景不淡出）
    if (g_trans == TRANS_OUT) {
        g_fade = 1.0f - float(++g_trans_frame) / TRANS_FRAMES;
        if (g_trans_frame >= TRANS_FRAMES) {
            const i32 old_screen = g_screen;   // 离开的屏幕（用于判断是否从音乐室返回）
            g_screen = g_next_screen;
            if((old_screen==SCR_GAME||old_screen==SCR_DIALOG)&&g_screen!=SCR_GAME&&g_screen!=SCR_DIALOG){start_title_music();g_story_music=-1;}
            if (g_screen == SCR_CHARSEL) { g_charsel_frame = 0; g_cursor_select = 0; g_char_switch_frame = 0; g_cursor_cx = CH_CX[g_char_sel]; g_cursor_cy = CH_CY[g_char_sel]; }
            g_sub_sel = (g_screen == SCR_STORY) ? 1 : 0;   // Story 默认 normal(1)
            g_story_cursor = (float)g_sub_sel;   // 同步游标，进入直接停在默认项
            g_vs_cursor = (float)g_sub_sel;
            g_cancel_fade = 1.0f;   // 重置取消淡出（否则再次进入时图标全透明，只剩背景盖过难度）
            g_cancel_slide = 0.0f;  // 重置取消聚拢
            if (g_screen != SCR_TITLE) {
                start_sub_screen(g_screen);
                if (old_screen == SCR_ENDING) start_title_music();   // 结局返回成就界面也重播标题曲
            } else {
                g_flash_phase=0;g_bob_phase=0;g_confirm_delay=0;g_cancel_delay=0;
                bind_title_vms(true); g_title_frame = 271; g_logo_fast = 0;   // 返回主菜单：重播灵梦/logo 入场演出（0.5秒快进）
                if (old_screen == SCR_MUSIC || old_screen == SCR_ENDING || old_screen == SCR_STAFF) start_title_music();   // 从音乐室/结局返回才重播标题曲
            }
            if (g_screen == SCR_MUSIC) {
                g_music_scroll = 0; g_music_scroll_cur = 0.0f; g_music_comment_track = 0;  // 进入音乐室重置状态（标题曲继续播，不打断）
            }
            g_trans = TRANS_IN;
            g_trans_frame = 0;
            g_fade = 0.0f;
        }
    } else if (g_trans == TRANS_IN) {
        g_fade = float(++g_trans_frame) / TRANS_FRAMES;
        if (g_trans_frame >= TRANS_FRAMES) { g_trans = TRANS_NONE; g_fade = 1.0f; }
    }
    if((g_screen==SCR_GAME||g_screen==SCR_DIALOG)&&g_game_load_frames==0)tick_charge_visuals();
    render();
    // Host health follows presented frames; catch-up ticks still run all logic.
    if(!g_skip_present)EM_ASM({ if (Module.onGameFrame) Module.onGameFrame(1, 0); });
}

bool rb_game_tick_prev_up = false, rb_game_tick_prev_dn = false, rb_game_tick_prev_z = false, rb_game_tick_prev_x = false;
bool rb_game_tick_prev_left = false, rb_game_tick_prev_right = false, rb_game_tick_prev_v = false, rb_game_tick_prev_c = false;
u8 rb_game_tick_previousP2=0;
touhou::input::TouchState touch_state(int side=0){
    touhou::input::TouchState s;
    s.context=g_paused?3:g_screen==SCR_GAME?1:g_screen==SCR_DIALOG?2:0;
    s.instance=g_world_id+1;s.ready=s.context==1&&g_game_load_frames==0&&g_player_damage[side].controllable();
    s.x=side?g_p2_x:g_p1_x;s.y=side?g_p2_y:g_p1_y;s.fast=g_charge[side].move_speed;s.slow=g_charge[side].slow_speed;
    s.min_x=-GAME_FIELD_W*.5f+8;s.max_x=GAME_FIELD_W*.5f-8;s.min_y=8;s.max_y=GAME_FIELD_H-8;
    return s;
}
void sample_touch_input(){
    std::fill(std::begin(g_touch_keys),std::end(g_touch_keys),false);
    if(g_network_active)return;
    const auto physical=SDL_GetKeyboardState(nullptr);
    const bool arrows=physical[SDL_SCANCODE_LEFT]||physical[SDL_SCANCODE_RIGHT]||physical[SDL_SCANCODE_UP]||physical[SDL_SCANCODE_DOWN]||g_injected_keys[SDL_SCANCODE_LEFT]||g_injected_keys[SDL_SCANCODE_RIGHT]||g_injected_keys[SDL_SCANCODE_UP]||g_injected_keys[SDL_SCANCODE_DOWN];
    const auto sample=g_touch_input.sample(touch_state(),std::uint64_t(emscripten_get_now()),physical[SDL_SCANCODE_LSHIFT]||g_injected_keys[SDL_SCANCODE_LSHIFT],arrows);
    const int vk[]={90,88,16,27,38,40,37,39,17,13};
    const int scans[]={SDL_SCANCODE_Z,SDL_SCANCODE_C,SDL_SCANCODE_LSHIFT,SDL_SCANCODE_ESCAPE,SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT,SDL_SCANCODE_LCTRL,SDL_SCANCODE_RETURN};
    for(unsigned n=0;n<10;++n)g_touch_keys[scans[n]]=sample.keys[vk[n]];
    if(sample.motion){
        float dx=sample.x-g_p1_x,dy=sample.y-g_p1_y;
        if(sample.motion==1){const auto s=touch_state();const float speed=sample.keys[16]?s.slow:s.fast,length=std::hypot(dx,dy);if(length>speed&&length>0){dx*=speed/length;dy*=speed/length;}}
        g_touch_motion_x=dx;g_touch_motion_y=dy;g_touch_unlimited=true;
    }else if(!g_touch_input.active()){g_touch_motion_x=g_touch_motion_y=0;g_touch_unlimited=false;}
}
void game_tick() {
    if (g_loop_paused) return;
    sample_touch_input();
    tick_trophy_popup();
    const bool* keys = game_keyboard_state();
    
    
    if(g_network_active&&g_network_frame==0)rb_game_tick_prev_up=rb_game_tick_prev_dn=rb_game_tick_prev_z=rb_game_tick_prev_x=rb_game_tick_prev_left=rb_game_tick_prev_right=rb_game_tick_prev_v=rb_game_tick_prev_c=false;
    const bool up = keys[SDL_SCANCODE_UP]    || g_injected_keys[SDL_SCANCODE_UP];
    const bool dn = keys[SDL_SCANCODE_DOWN]  || g_injected_keys[SDL_SCANCODE_DOWN];
    const bool left  = keys[SDL_SCANCODE_LEFT]  || g_injected_keys[SDL_SCANCODE_LEFT];
    const bool right = keys[SDL_SCANCODE_RIGHT] || g_injected_keys[SDL_SCANCODE_RIGHT];
    const bool z  = keys[SDL_SCANCODE_Z]     || keys[SDL_SCANCODE_RETURN]
                 || g_injected_keys[SDL_SCANCODE_Z] || g_injected_keys[SDL_SCANCODE_RETURN];
    const bool x  = keys[SDL_SCANCODE_X]     || keys[SDL_SCANCODE_ESCAPE]
                 || g_injected_keys[SDL_SCANCODE_X] || g_injected_keys[SDL_SCANCODE_ESCAPE];
    const bool v  = keys[SDL_SCANCODE_V]     || g_injected_keys[SDL_SCANCODE_V];
    const bool c  = keys[SDL_SCANCODE_C]     || g_injected_keys[SDL_SCANCODE_C];
    const bool noticeBlocked=tick_route_notice(z&&!rb_game_tick_prev_z);
    // Match the actual menu reveal; keep updating key edges while blocked,
    // so holding confirm during the intro cannot activate it on reveal.
    const bool titleIntroBlocked=g_screen==SCR_TITLE&&menu_intro_alpha()==0;
    const bool gameInputBlocked=titleIntroBlocked||noticeBlocked||g_charsel_cancel_frame>=0||g_difficulty_confirm_frame>=0||g_menu_cancel_frame>=0||g_vs_entry_transition>=0||g_battle_report_frame>=0||g_attack_warning.freeze_frames>0||g_final_exit>=0||g_story_switch.phase!=0||(g_screen==SCR_GAME||g_screen==SCR_DIALOG)&&(g_paused||keys[SDL_SCANCODE_ESCAPE]||g_injected_keys[SDL_SCANCODE_ESCAPE]);
    if(g_final_exit>=0&&++g_final_exit>=240){g_final_exit=-1;g_screen=SCR_ENDING;g_trans=TRANS_NONE;start_sub_screen(SCR_ENDING);}
    if(g_screen==SCR_STAFF&&g_trans==TRANS_NONE){
        //507a90 returns6 until every12th update when replay is permitted.
        const int steps=g_ending_repeat&&z?12:1;
        for(int i=0;i<steps&&g_screen==SCR_STAFF&&g_trans==TRANS_NONE;++i)tick_staff(i==0&&z&&!rb_game_tick_prev_z);
    }
    if(g_screen==SCR_ENDING&&g_ending_tail>=0){
        if(++g_ending_tail>=120){if(g_ending_return_screen==SCR_TITLE)start_staff();else{g_next_screen=g_ending_return_screen;g_trans=TRANS_OUT;g_trans_frame=0;g_ending_tail=-1;}}
    }
    if (!gameInputBlocked && g_phase == PHASE_TITLE && g_trans == TRANS_NONE && g_confirm_delay == 0 && g_cancel_delay == 0 && g_game_load_frames == 0) {
        if (g_screen == SCR_TITLE) {
            // 上下选择，跳过灰色 Online VS Mode (index 2)
            if (up && !rb_game_tick_prev_up) {
                do { g_sel = (g_sel + 7) % 8; } while (g_sel == 2);
                g_bob_phase = 8; play_sfx(10);
            }
            if (dn && !rb_game_tick_prev_dn) {
                do { g_sel = (g_sel + 1) % 8; } while (g_sel == 2);
                g_bob_phase = 8; play_sfx(10);
            }
            if (z && !rb_game_tick_prev_z) {
                // Z 确认：选中项快速闪几下（FUN_00402650(30) 设闪烁相位），
                // 闪烁约 19 帧后再淡出转场（FUN_0055a110 case2→4 的 0x13 帧延迟）
                play_sfx(7);
                switch (g_sel) {
                    case 0: g_vs_mode=0;g_next_screen = SCR_STORY;   break;
                    case 1: g_next_screen = SCR_VS;      break;
                    case 3: g_next_screen = SCR_ACHIEVE; break;
                    case 4: g_next_screen = SCR_MUSIC;   break;
                    case 5: g_next_screen = SCR_OPTION;  break;
                    case 6: g_next_screen = SCR_MANUAL;  break;
                    case 7: g_next_screen = SCR_TITLE;   break; // Quit 暂不退出
                    default: g_next_screen = SCR_TITLE;  break;
                }
                g_bob_phase = 8;
                g_flash_phase = 30;
                g_confirm_delay = 19;
            }
        } else if (g_screen == SCR_STORY || g_screen == SCR_VS) {
            const int maxSel=g_screen==SCR_STORY?3:4;
            if(up&&!rb_game_tick_prev_up){g_sub_sel=(g_sub_sel+maxSel)%(maxSel+1);play_sfx(10);}
            if(dn&&!rb_game_tick_prev_dn){g_sub_sel=(g_sub_sel+1)%(maxSel+1);play_sfx(10);}
            if(g_screen==SCR_VS){
                if((up&&!rb_game_tick_prev_up)||(dn&&!rb_game_tick_prev_dn))for(auto&vm:g_sub_vms){vm.interrupt_all(3);vm.tick();vm.interrupt_all(g_sub_sel+7);}
                if(z&&!rb_game_tick_prev_z){
                    g_vs_entry=g_sub_sel;g_vs_mode=1;g_vs_entry_transition=0;
                    for(auto&vm:g_sub_vms)vm.interrupt_all(6);
                    play_sfx(7);
                }
            }else {
                if((up&&!rb_game_tick_prev_up)||(dn&&!rb_game_tick_prev_dn))for(auto&vm:g_sub_vms){vm.interrupt_all(3);vm.tick();vm.interrupt_all(g_sub_sel+7);}
                if(z&&!rb_game_tick_prev_z){
                    g_story_difficulty=g_sub_sel;g_difficulty_confirm_frame=0;play_sfx(7);
                    // 5596a0: interrupt6 on the difficulty tree, then interrupt2
                    // on script(7+selection) only. Keep its scripted flash/scale/move.
                    th19::AnmVm selected;bind_script(g_sub_sel+7,selected);
                    auto confirmSelected=[&](auto&&self,th19::AnmVm&v)->void{
                        if(v.stream==selected.stream)v.interrupt(2);
                        for(auto&child:v.children)self(self,child);
                    };
                    for(auto&vm:g_sub_vms){vm.interrupt_all(6);confirmSelected(confirmSelected,vm);}
                }
            }
            if(x&&!rb_game_tick_prev_x){g_next_screen=(g_screen==SCR_STORY&&g_vs_mode)?SCR_VS:SCR_TITLE;
                for(auto&vm:g_sub_vms)vm.interrupt_all(1);
                g_menu_cancel_frame=0;play_sfx(9);}
        } else if (g_screen == SCR_MANUAL) {
            if (g_help_mode == 0) {
                if ((up || left) && !rb_game_tick_prev_up && !rb_game_tick_prev_left && g_help_sel > 0) { --g_help_sel; g_bob_phase = 8; play_sfx(10); }
                if ((dn || right) && !rb_game_tick_prev_dn && !rb_game_tick_prev_right && g_help_sel < 12) { ++g_help_sel; g_bob_phase = 8; play_sfx(10); }
                if (z && !rb_game_tick_prev_z) { g_help_page = g_help_sel; g_help_mode = 1; g_help_anim = 1; g_help_anim_frame = 0; play_sfx(7); }
                if (x && !rb_game_tick_prev_x) {
                    play_sfx(9);
                    g_cancel_delay = 20;
                    g_cancel_fade = 1.0f;
                    g_cancel_slide = 0.0f;
                    g_next_screen = SCR_TITLE;
                }
            } else {
                // 页态：动画播放中忽略输入；否则触发滑出动画
                if (g_help_anim == 0) {
                    if (up && !rb_game_tick_prev_up && g_help_page > 0) { g_help_anim = 3; g_help_anim_frame = 0; g_help_anim_next = 2; play_sfx(7); }
                    else if (dn && !rb_game_tick_prev_dn && g_help_page < 12) { g_help_anim = 2; g_help_anim_frame = 0; g_help_anim_next = 1; play_sfx(7); }
                    else if (z && !rb_game_tick_prev_z) { g_help_anim = 3; g_help_anim_frame = 0; g_help_anim_next = 3; play_sfx(9); }
                    else if (x && !rb_game_tick_prev_x) { g_help_anim = 3; g_help_anim_frame = 0; g_help_anim_next = 3; play_sfx(9); }
                }
            }
        } else if(g_screen==SCR_VSRULES){
            if(!g_vs_rule_edit){
                if(up&&!rb_game_tick_prev_up){g_sub_sel=(g_sub_sel+8)%9;play_sfx(10);}
                if(dn&&!rb_game_tick_prev_dn){g_sub_sel=(g_sub_sel+1)%9;play_sfx(10);}
                if(g_sub_sel<3&&((left&&!rb_game_tick_prev_left)||(right&&!rb_game_tick_prev_right))){int&i=g_vs_rules[g_sub_sel];i=(i+(left?-1:1)+VS_RULE_COUNTS[g_sub_sel])%VS_RULE_COUNTS[g_sub_sel];play_sfx(10);}
                if(z&&!rb_game_tick_prev_z){play_sfx(7);
                    if(g_sub_sel>=3&&g_sub_sel<=6){g_vs_rule_edit=true;g_vs_rule_pending=g_vs_rules[g_sub_sel];}
                    else if(g_sub_sel==7){int defaults[]={4,1,1,1,1,1,1};std::copy(defaults,defaults+7,g_vs_rules);}
                }
                if((x&&!rb_game_tick_prev_x)||(z&&!rb_game_tick_prev_z&&g_sub_sel==8)){g_screen=SCR_VS;g_sub_sel=3;start_sub_screen(g_screen);play_sfx(9);}
            }else{
                if((left&&!rb_game_tick_prev_left)||(up&&!rb_game_tick_prev_up))g_vs_rule_pending=(g_vs_rule_pending+VS_RULE_COUNTS[g_sub_sel]-1)%VS_RULE_COUNTS[g_sub_sel];
                if((right&&!rb_game_tick_prev_right)||(dn&&!rb_game_tick_prev_dn))g_vs_rule_pending=(g_vs_rule_pending+1)%VS_RULE_COUNTS[g_sub_sel];
                if(z&&!rb_game_tick_prev_z){g_vs_rules[g_sub_sel]=g_vs_rule_pending;g_vs_rule_edit=false;play_sfx(7);}
                if(x&&!rb_game_tick_prev_x){g_vs_rule_edit=false;play_sfx(9);}
            }
        } else if (g_screen == SCR_KEYCONFIG) {
            if (g_key_mode == 0) {
                // Controller Select：上下选择 + Z 确认 + X 返回 Option
                if (up && !rb_game_tick_prev_up && g_key_sel > 0) { --g_key_sel; g_bob_phase = 8; play_sfx(10); }
                if (dn && !rb_game_tick_prev_dn && g_key_sel < 3) { ++g_key_sel; g_bob_phase = 8; play_sfx(10); }
                if (z && !rb_game_tick_prev_z) {
                    if (g_key_sel == 0 || g_key_sel == 1) {   // 1Player/2Player -> 按键绑定
                        g_key_mode = 1; g_key_sel = 0; play_sfx(7);
                    } else if (g_key_sel == 2) {              // KeyConfig ALL Reset（占位）
                        play_sfx(7);
                    } else {                                   // Quit -> 返回 Option
                        play_sfx(9);
                        g_cancel_delay = 20; g_cancel_fade = 1.0f; g_cancel_slide = 0.0f;
                        g_next_screen = g_key_return_screen;
                    }
                }
                if (x && !rb_game_tick_prev_x) {
                    play_sfx(9);
                    g_cancel_delay = 20;
                    g_cancel_fade = 1.0f;
                    g_cancel_slide = 0.0f;
                    g_next_screen = g_key_return_screen;
                }
            } else {
                // Key Config（按键绑定列表）：上下选择 + X 返回 Controller Select
                if (up && !rb_game_tick_prev_up && g_key_sel > 0) { --g_key_sel; g_bob_phase = 8; play_sfx(10); }
                if (dn && !rb_game_tick_prev_dn && g_key_sel < 9) { ++g_key_sel; g_bob_phase = 8; play_sfx(10); }
                if (x && !rb_game_tick_prev_x) { g_key_mode = 0; g_key_sel = 0; play_sfx(9); }
            }
        } else if (g_screen == SCR_ENDING) {
            if(g_ending_tail<0&&z&&!rb_game_tick_prev_z){
                const int count=END_LINE_OFFSET[g_ending_id+1]-END_LINE_OFFSET[g_ending_id];
                if(g_ending_group+1>=END_GROUP_OFFSETS[g_ending_id+1])g_ending_tail=0;
                else {++g_ending_group;g_ending_line=END_GROUPS[g_ending_group].last;g_ending_reveal_frame=g_frame;}
            }
        } else if (g_screen == SCR_CHARSEL) {
            // 556340: CPU entries share input, choosing side 1 then side 2.
            const bool second=g_vs_mode&&g_vs_first_confirmed;
            
            auto key2=[&](int c){return keys[c]||g_injected_keys[c];};
            const u8 p2=(key2(SDL_SCANCODE_W)?1:0)|(key2(SDL_SCANCODE_S)?2:0)|(key2(SDL_SCANCODE_A)?4:0)|(key2(SDL_SCANCODE_D)?8:0)|(key2(SDL_SCANCODE_J)?16:0)|(key2(SDL_SCANCODE_K)?32:0);
            const u8 p2edge=p2&~rb_game_tick_previousP2;rb_game_tick_previousP2=p2;
            int& choice=second?g_opp_sel:g_char_sel;
            int dir=-1;
            if(up&&!rb_game_tick_prev_up)dir=0;else if(dn&&!rb_game_tick_prev_dn)dir=1;
            else if(left&&!rb_game_tick_prev_left)dir=2;else if(right&&!rb_game_tick_prev_right)dir=3;
            if(second&&g_vs_entry==0){if(p2edge&1)dir=0;else if(p2edge&2)dir=1;else if(p2edge&4)dir=2;else if(p2edge&8)dir=3;}
            if(g_vs_mode&&(key2(SDL_SCANCODE_LSHIFT)||key2(SDL_SCANCODE_RSHIFT))){if(dir>=2&&g_vs_match_confirm<0&&g_vs_rules[2]>=2){move_vs_card(second?1:0,dir==2?-1:1);play_sfx(0);}dir=-1;}
            if(dir>=0&&g_vs_match_confirm<0){int next=CHAR_NAV[choice][dir];for(int attempt=0;attempt<19&&!character_available(next);++attempt)next=CHAR_NAV[next][dir];if(character_available(next)&&next!=choice){choice=next;g_bob_phase=8;g_char_switch_frame=0;play_sfx(10);
                if(g_vs_mode){
                    init_vs_cards(second?1:0);auto&vm=g_vs_portrait_vm[second?1:0];vm.interrupt_all(3);vm.tick();vm.interrupt_all(choice+7);

                }}}
            if(g_vs_match_confirm<0&&character_available(choice)&&((z&&!rb_game_tick_prev_z)||(second&&g_vs_entry==0&&(p2edge&16)))){
                play_sfx(7);g_cursor_select=10;
                if(g_vs_mode&&!second){g_vs_first_confirmed=true;g_vs_cursor_vm[0].interrupt(6);}
                else if(g_vs_mode&&!g_vs_second_confirmed){g_vs_second_confirmed=true;g_vs_cursor_vm[1].interrupt(6);g_vs_match_confirm=0;}
                else if(!g_vs_mode){g_vs_first_confirmed=true;g_vs_cursor_vm[0].interrupt(6);g_vs_match_confirm=0;}
            }
            if((x&&!rb_game_tick_prev_x)||(second&&g_vs_entry==0&&(p2edge&32))){
                play_sfx(9);
                if(!g_vs_mode&&g_vs_first_confirmed){g_vs_first_confirmed=false;g_vs_match_confirm=-1;g_vs_cursor_vm[0].interrupt(3);}
                else if(g_vs_second_confirmed){g_vs_second_confirmed=false;g_vs_match_confirm=-1;g_vs_cursor_vm[1].interrupt(3);}
                else if(second){g_vs_first_confirmed=false;g_vs_cursor_vm[0].interrupt(3);}
                else if(g_vs_mode){g_charsel_cancel_frame=0;}
                else {g_cancel_delay=20;g_cancel_fade=1;g_cancel_slide=0;g_next_screen=SCR_STORY;}
            }
        } else if (g_screen == SCR_DIALOG) {
            if(const auto* scene=g_story_session.scene())g_story_final_music=scene->final_music;
            // X does not skip story scenes; only confirm advances dialogue.
            if (g_card_reward<0 && z && !rb_game_tick_prev_z && g_story_playback.confirm()) play_sfx(0);
            g_story_playback.tick();
            tick_dialogue_portraits();tick_dialogue_bubble();play_story_music(g_story_playback.music);
            if(g_story_playback.spawn)start_story_actor();
            tick_story_actor();
            if(g_story_playback.finished){
                if(g_vs_dialogue_active){
                    g_vs_dialogue_active=false;g_vs_mode=0;g_next_screen=SCR_TITLE;g_trans=TRANS_OUT;g_trans_frame=0;
                    return;
                }
                if(g_story_session.phase()==th19::StoryPhase::Won&&g_card_reward<0&&g_card_reward_exit<0)begin_card_reward(g_story_session.opponent());
                if(tick_card_reward(z&&!rb_game_tick_prev_z)){
                const int stage=g_story_session.stage_index();
                g_card_reward_exit=-1;
                g_story_session.skip_scene();
                if(g_story_session.phase()==th19::StoryPhase::Before&&g_story_session.stage_index()!=stage)start_story_switch();
                else if(g_story_session.phase()==th19::StoryPhase::Ending){
                    // 0x532cf0 state 0xf enters EndingInf; 0x507ff0 selects
                    // endXX.msg from the player character (DAT_0062b0cc).
                    g_ending_id=g_char_sel;g_ending_return_screen=SCR_TITLE;
                    g_final_exit=0; //516xxx progression: fade at180, EndingInf at240.
                }
                else if(g_story_session.phase()==th19::StoryPhase::Failed){
                    // 53aa10 opens front.anm89 after the losing MSG scene.
                    g_paused=true;g_pause_confirm=false;g_pause_choice=0;init_pause_animation(true);
                }
            }
            }
            if (!g_vs_dialogue_active && g_story_session.phase() == th19::StoryPhase::Battle) {
                g_screen=SCR_GAME;show_big_text(175);play_story_music(g_story_final_music);
            }
        } else if (g_screen == SCR_GAME) {
            tick_story_actor();
            // Gameplay movement; acquired cards use the native HUD path.
            const bool cpuLeft=g_vs_mode&&g_vs_entry==2;
            if(cpuLeft&&g_big_sprite<0)tick_cpu_opponent(0);
            const bool moveLeft=cpuLeft?(g_cpu_left_bits&64)!=0:(keys[SDL_SCANCODE_LEFT]||(g_injected_keys[SDL_SCANCODE_LEFT])),moveRight=cpuLeft?(g_cpu_left_bits&128)!=0:(keys[SDL_SCANCODE_RIGHT]||(g_injected_keys[SDL_SCANCODE_RIGHT]));
            const bool moveUp=cpuLeft?(g_cpu_left_bits&16)!=0:(keys[SDL_SCANCODE_UP]||(g_injected_keys[SDL_SCANCODE_UP])),moveDown=cpuLeft?(g_cpu_left_bits&32)!=0:(keys[SDL_SCANCODE_DOWN]||(g_injected_keys[SDL_SCANCODE_DOWN]));
            const bool slow=cpuLeft?(g_cpu_left_bits&8)!=0:keys[SDL_SCANCODE_LSHIFT]||g_injected_keys[SDL_SCANCODE_LSHIFT];
            tick_focus(0,slow);
            const bool diagonal=(moveLeft!=moveRight)&&(moveUp!=moveDown);
            const auto& movement=g_charge[0];
            const float spd=!g_player_damage[0].controllable()?0:diagonal?(slow?movement.slow_diagonal_speed:movement.diagonal_speed):(slow?movement.slow_speed:movement.move_speed);
            if(g_big_sprite<0){
                if(g_vs_mode&&g_vs_entry==0){
                    // Web local-player2 binding; independent from the CPU controller.
                    auto k=[&](int c){return keys[c]||g_injected_keys[c];};
                    const u8 bits=(k(SDL_SCANCODE_J)?1:0)|(k(SDL_SCANCODE_K)?2:0)|(k(SDL_SCANCODE_L)?4:0)|(k(SDL_SCANCODE_LCTRL)?8:0)|
                        (k(SDL_SCANCODE_W)?16:0)|(k(SDL_SCANCODE_S)?32:0)|(k(SDL_SCANCODE_A)?64:0)|(k(SDL_SCANCODE_D)?128:0);
                    if((bits&4)&&!(g_opponent_controls&4))start_player_bomb(1);
                    g_opponent_controls=bits;g_cpu_automatic=false;
                }else tick_cpu_opponent();
                if(!cpuLeft&&c&&!rb_game_tick_prev_c)start_player_bomb(0);
                tick_player_bombs();
                tick_charge_release(0,cpuLeft?(g_cpu_left_bits&2)!=0:keys[SDL_SCANCODE_X]||g_injected_keys[SDL_SCANCODE_X],slow);
                tick_charge_release(1,(g_opponent_controls&2)!=0,(g_opponent_controls&8)!=0);
                tick_player_shots((cpuLeft?(g_cpu_left_bits&1)!=0:z)&&g_player_damage[0].controllable(),slow);
                tick_spirit_transits();
                for(auto&field:g_ecl_fields){field.world.finish_hit_feedback(field.world.spell.active?1u:0u);
                    for(int sound:field.world.sound_events)play_sfx(sound);field.world.sound_events.clear();}}
            tick_focus(1,(g_opponent_controls&8)!=0);
            if (!cpuLeft && g_touch_unlimited && g_player_damage[0].controllable()) {
                g_p1_x += g_touch_motion_x; g_p1_y += g_touch_motion_y;
            }
            g_touch_motion_x = g_touch_motion_y = 0.0f;
            if (moveLeft)  g_p1_x -= spd;
            if (moveRight) g_p1_x += spd;
            if (moveUp)    g_p1_y -= spd;
            if (moveDown)    g_p1_y += spd;
            // 左右移动方向动画：左→脚本-35、右→脚本-33、否则前向-36（pl00.anm 反编译）
            const i32 ndir = moveLeft ? -1 : (moveRight ? 1 : 0);
            if (ndir != g_p1_dir) { g_p1_dir = ndir; bind_player_dir(ndir); }
            float marginX=16,marginY=24;
            auto sprite=g_player_sprites.find(g_player_vm.sprite);
            if(sprite!=g_player_sprites.end()){const auto&sp=g_player[sprite->second.first].sprites[sprite->second.second];marginX=sp.w*.5f;marginY=sp.h*.5f;}
            g_p1_x=std::clamp(g_p1_x,-GAME_FIELD_W*.5f+marginX,GAME_FIELD_W*.5f-marginX);
            if(g_player_damage[0].state!=th19::PlayerDamage::Respawning)g_p1_y=std::clamp(g_p1_y,marginY,GAME_FIELD_H-marginY);
            // Native CPU output bits feed the same SHT movement speeds.
            if(g_big_sprite<0&&g_player_damage[1].controllable()){
                const int dx=((g_opponent_controls&128)!=0)-((g_opponent_controls&64)!=0);
                const int dy=((g_opponent_controls&32)!=0)-((g_opponent_controls&16)!=0);
                const auto&m=g_charge[1];bool slow=(g_opponent_controls&8)!=0;
                float speed=dx&&dy?(slow?m.slow_diagonal_speed:m.diagonal_speed):(slow?m.slow_speed:m.move_speed);
                const bool direct=g_network_active&&g_network_unlimited[1];
                g_p2_x=std::clamp(g_p2_x+(direct?g_network_motion_x[1]:dx*speed),-140.f,140.f);
                g_p2_y=std::clamp(g_p2_y+(direct?g_network_motion_y[1]:dy*speed),32.f,432.f);
                if(dx!=g_opp_direction){g_opp_direction=dx;bind_opp_dir(dx);}
            }
            if(g_big_sprite<0)for(int side=0;side<2;++side){auto&w=g_ecl_fields[side].world;auto&r=w.charge_region;float px=side?g_p2_x:g_p1_x,py=side?g_p2_y:g_p1_y;
                r.tick(side?g_opp_sel:g_char_sel,g_charge[side].held,px,py,side==0&&std::find(g_owned_cards.begin(),g_owned_cards.end(),10)!=g_owned_cards.end()?125.f:100.f);
                if(r.character==5){const auto target=w.nazrin_seek_target(r.x,r.y,px,py);r.seek(target.x,target.y);}
                if(r.character==13)for(int i=1;i<3;++i){const auto&o=g_player_options[side];r.centers[i]=i<=int(o.size())?th19::ChargeRegion::Point{o[i-1].x,o[i-1].y}:th19::ChargeRegion::Point{px,py};}
            }
            tick_player_damage();check_player_contacts();
            // Esc is handled by the pause controller before gameplay updates.
        } else {
            // 子界面内：上下/左右选择 + X 返回（边界停止，不循环）
            const i32 cnt = sub_item_count();
            bool sel_changed = false;
            if (g_screen == SCR_OPTION) {
                // Option：上下选择，左右调节值（源码 FUN_00536d60 case 2：左 0x40/右 0x80）
                if (up && !rb_game_tick_prev_up && g_sub_sel > 0) { --g_sub_sel; g_bob_phase = 8; play_sfx(10); }
                if (dn && !rb_game_tick_prev_dn && g_sub_sel < cnt - 1) { ++g_sub_sel; g_bob_phase = 8; play_sfx(10); }
                if (left && !rb_game_tick_prev_left) option_adjust(-1);
                if (right && !rb_game_tick_prev_right) option_adjust(1);
                if (z && !rb_game_tick_prev_z) option_confirm();
            } else {
                if (cnt > 0) {
                    if (g_screen == SCR_ACHIEVE) {
                        // 6 列网格导航（源码 FUN_00554850 case 1：左右 %6 回绕，上下 ±6）
                        if (left && !rb_game_tick_prev_left) {
                            if (g_sub_sel % 6 == 0) g_sub_sel = std::min(g_sub_sel + 5, 22); else --g_sub_sel;
                            sel_changed = true; g_achievement_detail_frame=0; g_bob_phase = 8; play_sfx(10);
                        }
                        if (right && !rb_game_tick_prev_right) {
                            if (g_sub_sel % 6 == 5) g_sub_sel -= 5; else g_sub_sel=std::min(22,g_sub_sel+1);
                            sel_changed = true; g_achievement_detail_frame=0; g_bob_phase = 8; play_sfx(10);
                        }
                        if (up && !rb_game_tick_prev_up && g_sub_sel >= 6) { g_sub_sel -= 6; sel_changed = true; g_achievement_detail_frame=0; g_bob_phase = 8; play_sfx(10); }
                        if (dn && !rb_game_tick_prev_dn && g_sub_sel + 6 < 23) { g_sub_sel += 6; sel_changed = true; g_achievement_detail_frame=0; g_bob_phase = 8; play_sfx(10); }
                        // Z 在已解锁成就上触发结局播放（源码 FUN_00554850 case 1）
                        if (z && !rb_game_tick_prev_z) {
                            const i32 eid = g_achieve_ending[g_sub_sel];
                            if (eid >= 0 && g_trophy_unlocked[g_sub_sel]) {
                                g_ending_id = eid;
                                g_ending_line = 0;
                                g_ending_return_screen = SCR_ACHIEVE;  // 成就触发结局：播完回成就界面
                                g_next_screen = SCR_ENDING;
                                g_trans = TRANS_OUT; g_trans_frame = 0;
                                play_sfx(7);
                            }
                        }
                    } else {
                        if ((up || left) && !rb_game_tick_prev_up && !rb_game_tick_prev_left && g_sub_sel > 0) { --g_sub_sel; sel_changed = true; g_bob_phase = 8; play_sfx(10); }
                        if ((dn || right) && !rb_game_tick_prev_dn && !rb_game_tick_prev_right && g_sub_sel < cnt - 1) { ++g_sub_sel; sel_changed = true; g_bob_phase = 8; play_sfx(10); }
                        // Story 难度选择 Z 确认：选中难度闪烁 → 14 帧 → 跳角色选择（FUN_005596a0 case2→3）
                        if (g_screen == SCR_STORY && z && !rb_game_tick_prev_z) {
                            g_story_difficulty = g_sub_sel;
                            g_char_sel = 0;
                            g_flash_phase = 14;    // 闪烁 14 帧（FUN_005596a0 case2 script 6/2）
                            g_cancel_delay = 34;   // 14 帧闪烁 + 20 帧聚拢淡出（取消同款动画）
                            g_cancel_fade = 1.0f;
                            g_cancel_slide = 0.0f;
                            g_next_screen = SCR_CHARSEL;
                            play_sfx(7);
                        }
                    }
                }
                // Music 曲目播放（源码 FUN_00555200 case 2）：未解锁也可播放，仅曲名显示"？？？"
                if (g_screen == SCR_MUSIC) {
                    if (z && !rb_game_tick_prev_z) {
                        g_music_comment_track = g_sub_sel;   // 确认曲目（源码 0xaec = 0x2c）
                        const bool music_locked = g_music_unlocked[g_sub_sel] == 0;
                        if (music_locked && !g_music_prompt) {
                            g_music_prompt = 1;   // 首次确认锁定曲：弹未解锁提示，不播放
                            play_sfx(9);
                        } else {
                            play_music_track(g_sub_sel);
                            play_sfx(7);
                            g_music_prompt = 0;
                        }
                        reveal_music_comment();
                    }
                    if (sel_changed) {
                        if(g_music_revealed>=8)g_music_prompt = 0;   // native clears retry flag after text has been revealed
                        // 边缘滚动：sel 移出 [scroll, scroll+9] 才滚动（源码 case 2）
                        if (g_sub_sel < g_music_scroll) g_music_scroll = g_sub_sel;
                        else if (g_music_scroll + 10 <= g_sub_sel) g_music_scroll = g_sub_sel - 9;
                        select_music_animation();
                    }
                }
            }
            if (x && !rb_game_tick_prev_x) {
                if(g_screen==SCR_MUSIC)close_music_animation();
                if(g_screen==SCR_ACHIEVE){g_achievement_exiting=true;for(auto&v:g_achievement_details)v.interrupt(1);for(auto&v:g_sub_vms)v.interrupt_all(1);}
                // X 取消：取消音效 + 图标向中心(640,368)聚拢 + 淡出
                // （逆向：脚本 case 1 = posTime 20帧→(640,368) + alphaTime 20帧→0 + del）
                play_sfx(9);
                g_cancel_delay = 20;
                g_cancel_fade = 1.0f;
                g_cancel_slide = 0.0f;
                g_next_screen = SCR_TITLE;
            }
        }
    }
    rb_game_tick_prev_up = up; rb_game_tick_prev_dn = dn; rb_game_tick_prev_z = z; rb_game_tick_prev_x = x;
    rb_game_tick_prev_left = left; rb_game_tick_prev_right = right; rb_game_tick_prev_v = v; rb_game_tick_prev_c = c;
    frame();
}
#include "ApplicationCheckpoint.inc"
std::map<u32,std::unique_ptr<ApplicationCheckpoint>> rb_checkpoints;
std::vector<std::unique_ptr<ApplicationCheckpoint>> rb_checkpoint_pool;
u32 rb_next_checkpoint=1;
void loop(void*) {
    if(g_loop_paused){g_fixed_step.reset();return;}
    const int ticks=g_fixed_step.advance(emscripten_get_now());
    for(int i=0;i<ticks;++i){g_skip_present=i+1<ticks;game_tick();}
    g_skip_present=false;
}
}

extern "C" {

// Prediction is restricted to live gameplay. Pause, dialogue, result cards,
// and resource-changing transitions run only with contiguous confirmed input.
__attribute__((export_name("th19_rollback_can_predict"))) u32 th19_rollback_can_predict(u32 keys){
 return g_network_active&&!g_network_finished&&g_screen==SCR_GAME&&g_trans==TRANS_NONE&&
 !g_story_switch.phase&&!g_game_load_frames&&!g_paused&&g_battle_report_frame<0&&!(keys&1024);
}
__attribute__((export_name("th19_rollback_effect_begin"))) void th19_rollback_effect_begin(){rb_effects.clear();rb_collect_effects=true;}
__attribute__((export_name("th19_rollback_effect_end"))) u32 th19_rollback_effect_end(){rb_collect_effects=false;return rb_effects.size();}
__attribute__((export_name("th19_rollback_effect_data"))) u32 th19_rollback_effect_data(){return u32(rb_effects.data());}
__attribute__((export_name("th19_rollback_effect_commit"))) void th19_rollback_effect_commit(i32 code){
 if(code<0){stop_sfx(-code-1);return;}
 if(!g_audio.ready||code>=int(std::size(th19::SOUNDS)))return;
 if(!g_audio.sfx.count(code)&&!g_audio.load_sfx(code,th19::SOUNDS[code].path))return;
 g_audio.play_sfx(code);
}

// Candidate diagnostics: not exposed by production launcher.
__attribute__((export_name("th19_checkpoint_save"))) u32 th19_checkpoint_save(){
 if(!g_network_active||g_screen!=SCR_GAME||g_story_switch.phase||g_trans!=TRANS_NONE||g_paused||g_battle_report_frame>=0||rb_checkpoints.size()>=16)return 0;
 u32 id=rb_next_checkpoint++;
 if(rb_checkpoint_pool.empty())rb_checkpoints[id]=std::make_unique<ApplicationCheckpoint>();
 else{auto checkpoint=std::move(rb_checkpoint_pool.back());rb_checkpoint_pool.pop_back();checkpoint->capture();rb_checkpoints[id]=std::move(checkpoint);}
 return id;
}
__attribute__((export_name("th19_checkpoint_load"))) u32 th19_checkpoint_load(u32 id){
 auto it=rb_checkpoints.find(id);return it!=rb_checkpoints.end()&&it->second->restore();
}
__attribute__((export_name("th19_checkpoint_release"))) void th19_checkpoint_release(u32 id){
 auto it=rb_checkpoints.find(id);if(it==rb_checkpoints.end())return;
 if(it->second->generation==g_ecl_generation&&rb_checkpoint_pool.size()<8)rb_checkpoint_pool.push_back(std::move(it->second));
 rb_checkpoints.erase(it);
}

static i32 networkRules[4]={0,1,2,2};
__attribute__((export_name("th19_network_set_rules"))) u32 th19_network_set_rules(i32 time,i32 rounds,i32 left,i32 right){
 if(g_network_active||time!=0||rounds<0||rounds>2||left<0||left>4||right<0||right>4)return 0;
 networkRules[0]=time;networkRules[1]=rounds;networkRules[2]=left;networkRules[3]=right;return 1;
}

// Network frames enter the existing local human-versus-human simulation.
// Only confirmed input reaches this entry point; SDL device state is excluded.
__attribute__((export_name("th19_network_begin"))) u32 th19_network_begin(u32 seed,i32 difficulty,i32 left,i32 right){
    if(g_network_active||difficulty<0||difficulty>3||left<0||left>=19||right<0||right>=19)return 0;
    g_network_active=true;g_network_finished=false;g_network_frame=0;
    g_game_seed=seed?seed:1;g_vs_scene_rng.state=g_game_seed;
    th19::AnmVm::seed_random(g_game_seed);
    g_frame=0;g_phase=PHASE_TITLE;g_screen=SCR_GAME;g_trans=TRANS_NONE;
    g_story_switch.phase=0;g_vs_dialogue_active=false;g_paused=g_pause_confirm=false;
    g_confirm_delay=g_cancel_delay=g_game_load_frames=0;g_notice_id=-1;
    g_vs_entry=0;g_char_sel=left;g_opp_sel=right;g_story_difficulty=difficulty;
    const int rules[7]={networkRules[0],networkRules[1],1,networkRules[2],networkRules[3],1,1};std::copy(rules,rules+7,g_vs_rules);
    std::fill(std::begin(g_injected_keys),std::end(g_injected_keys),0);
    g_touch_unlimited=false;g_touch_motion_x=g_touch_motion_y=0;
    start_vs_match();g_cpu_automatic=false;g_player_collision_enabled=true;
    g_loop_paused=true;return 1;
}
__attribute__((export_name("th19_network_step"))) u32 th19_network_step(u32 frame,u32 left,u32 right,
    float leftX,float leftY,float rightX,float rightY,u32 leftDirect,u32 rightDirect,u32 present){
    if(!g_network_active||g_network_finished||frame!=g_network_frame||(left|right)>2047||
        leftDirect>1||rightDirect>1||!std::isfinite(leftX)||!std::isfinite(leftY)||
        !std::isfinite(rightX)||!std::isfinite(rightY)||std::abs(leftX)>512||std::abs(leftY)>512||
        std::abs(rightX)>512||std::abs(rightY)>512)return 0;
    std::fill(std::begin(g_injected_keys),std::end(g_injected_keys),0);
    const int codes[2][8]={{SDL_SCANCODE_Z,SDL_SCANCODE_X,SDL_SCANCODE_C,SDL_SCANCODE_LSHIFT,
        SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT},
        {SDL_SCANCODE_J,SDL_SCANCODE_K,SDL_SCANCODE_L,SDL_SCANCODE_LCTRL,
        SDL_SCANCODE_W,SDL_SCANCODE_S,SDL_SCANCODE_A,SDL_SCANCODE_D}};
    const u32 inputs[2]={left,right};
    for(int side=0;side<2;++side)for(int bit=0;bit<8;++bit)
        g_injected_keys[codes[side][bit]]=(inputs[side]&(1u<<bit))!=0;
    // Both seats may operate the shared pause menu; gameplay remains separate.
    if(g_paused||g_screen==SCR_DIALOG)for(int bit=0;bit<8;++bit)g_injected_keys[codes[0][bit]]=((left|right)&(1u<<bit))!=0;
    g_injected_keys[SDL_SCANCODE_ESCAPE]=((left|right)&1024)!=0;
    g_network_unlimited[0]=leftDirect;g_network_unlimited[1]=rightDirect;
    g_network_motion_x[0]=leftX;g_network_motion_y[0]=leftY;
    g_network_motion_x[1]=rightX;g_network_motion_y[1]=rightY;
    g_touch_unlimited=leftDirect;g_touch_motion_x=leftX;g_touch_motion_y=leftY;
    g_skip_present=present==0;g_loop_paused=false;game_tick();
    g_loop_paused=true;g_skip_present=false;++g_network_frame;
    if(g_screen==SCR_CHARSEL||g_screen==SCR_TITLE)g_network_finished=true;
    return g_network_finished?2:1;
}
__attribute__((export_name("th19_network_end"))) void th19_network_end(){
    rb_checkpoints.clear();rb_checkpoint_pool.clear();rb_collect_effects=false;rb_effects.clear();
    g_network_active=false;g_network_finished=false;g_loop_paused=true;
    std::fill(std::begin(g_injected_keys),std::end(g_injected_keys),0);
    g_touch_unlimited=false;g_touch_motion_x=g_touch_motion_y=0;
}
__attribute__((export_name("th19_network_hash"))) u32 th19_network_hash(){
    u32 hash=2166136261u;
    auto integer=[&](u32 value){for(int i=0;i<4;++i){hash^=(value>>(8*i))&255;hash*=16777619u;}};
    auto number=[&](float value){u32 bits;std::memcpy(&bits,&value,4);integer(bits);};
    integer(g_network_frame);integer(g_screen);integer(g_vs_match.round);
    integer(g_vs_match.wins[0]);integer(g_vs_match.wins[1]);integer(g_vs_match.remaining);
    integer(g_paused);integer(g_battle_report_frame);integer(g_battle_winner);
    number(g_p1_x);number(g_p1_y);number(g_p2_x);number(g_p2_y);
    integer(g_p1_lives);integer(g_p2_lives);integer(g_p1_bombs);integer(g_p2_bombs);
    for(int side=0;side<2;++side){
        integer(g_charge[side].charge);integer(g_charge[side].energy);
        const auto&w=g_ecl_fields[side].world;
        integer(w.bullets.size());integer(w.enemies.size());integer(w.lasers.size());
        for(const auto&b:w.bullets){number(b.x);number(b.y);number(b.vx);number(b.vy);integer(b.kind);}
        for(const auto&e:w.enemies){number(e.x);number(e.y);integer(e.life);integer(e.active);}
        for(const auto&l:w.lasers){number(l.x);number(l.y);number(l.angle);number(l.length);number(l.width);
            for(const auto&node:l.curve.nodes){number(node.x);number(node.y);}}
    }
    return hash;
}
// Restore the already loaded native title for the Launcher room workspace.
// Program, graphics resources, retail archive and persistent files stay owned
// by this Runtime; a rematch starts a fresh network session on the same core.
__attribute__((export_name("th19_room_title"))) u32 th19_room_title(){
    th19_network_end();
    g_touch_input.reset();
    g_phase=PHASE_TITLE;g_screen=SCR_TITLE;g_trans=TRANS_NONE;
    g_frame=0;g_title_frame=0;g_logo_fast=0;g_sel=0;g_fade=1;
    g_vs_mode=0;g_cpu_automatic=false;g_vs_dialogue_active=false;
    g_story_switch.phase=0;g_paused=g_pause_confirm=false;
    g_confirm_delay=g_cancel_delay=g_game_load_frames=0;g_notice_id=-1;
    g_charsel_cancel_frame=g_difficulty_confirm_frame=g_menu_cancel_frame=-1;
    g_vs_entry_transition=g_battle_report_frame=g_final_exit=-1;
    g_sub_vms.clear();g_retiring_menu.clear();g_big_sprite=-1;
    start_animation();start_title_music();g_loop_paused=false;render();return 1;
}
__attribute__((export_name("th19_network_info"))) u32 th19_network_info(){
    static i32 state[8];state[0]=g_network_active;state[1]=g_network_frame;state[2]=g_network_finished;
    state[3]=g_vs_mode;state[4]=g_vs_entry;state[5]=g_cpu_automatic;
    state[6]=g_char_sel;state[7]=g_opp_sel;return (u32)state;
}

__attribute__((export_name("th19_game_open"))) u32 th19_game_open(u32 seed) {
    g_game_seed = seed;g_vs_scene_rng.state=seed?seed:1;
    if (!gfx.initialize()) return 0;
    init_audio();
    if (!load_anm("/ascii1280.anm", g_ascii_bytes, g_ascii)) return 0;
    decode_textures(g_ascii, g_ascii_bytes, g_ascii_tex);
    // Resolve the small FPS alphabet once; avoid scanning the ANM every frame.
    g_fps_font={};g_present_fps={};
    for(i32 ei=0;ei<i32(g_ascii.size());++ei){auto tx=g_ascii_tex.find(ei);if(tx==g_ascii_tex.end())continue;
        for(const auto&sp:g_ascii[ei].sprites)if(sp.id<96&&!g_fps_font[sp.id+32].texture.handle)
            g_fps_font[sp.id+32]={tx->second,sp};
    }
    if (!load_anm("/sig.anm", g_sig_bytes, g_sig)) return 0;
    decode_textures(g_sig, g_sig_bytes, g_sig_tex);
    if (!load_anm("/title.anm", g_title_bytes, g_title)) return 0;
    decode_textures(g_title, g_title_bytes, g_title_tex);
    // title_v.anm is the version-mark animation (not a VS background).
    // The normal title background alone
    // leaves the original VS page without its camera/overlay pass).
    if (load_anm("/title_v.anm", g_title_v_bytes, g_title_v)) {
        decode_textures(g_title_v,g_title_v_bytes,g_title_v_tex);g_title_v_scripts.clear();g_title_v_sprites.clear();
        for(i32 ei=0;ei<i32(g_title_v.size());++ei)for(i32 si=0;si<i32(g_title_v[ei].sprites.size());++si)g_title_v_sprites[g_title_v[ei].sprites[si].id]={ei,si};
        for(const auto&e:g_title_v)for(const auto&sc:e.scripts)
            g_title_v_scripts[sc.id]={g_title_v_bytes.data()+e.base+sc.offset,u32(g_title_v_bytes.size()-e.base-sc.offset),&e.scripts};
    }
    if (!load_anm("/front.anm", g_front_bytes, g_front)) return 0;
    if (!load_anm("/text.anm", g_story_text_bytes, g_story_text)) return 0;
    g_story_text_scripts.clear();for(const auto&e:g_story_text)for(const auto&sc:e.scripts)
        g_story_text_scripts[sc.id]={g_story_text_bytes.data()+e.base+sc.offset,u32(g_story_text_bytes.size()-e.base-sc.offset),&e.scripts};
    decode_textures(g_front, g_front_bytes, g_front_tex);
    for (i32 i = 0; i < 19; ++i) {
        char path[32]; std::snprintf(path, sizeof path, "/pl%02df.anm", i);
        if (!load_anm(path, g_plf_bytes[i], g_plf[i])) return 0;
        decode_textures(g_plf[i], g_plf_bytes[i], g_plf_tex[i]);
    }
    load_native_score();
    load_web_clears();
    load_help();
    build_sprite_map();
    build_script_map();
    build_white_texture();
    build_select_box_texture();
    g_music_tex = gfx.create_texture(g_music_names, g_music_names_len);
    g_music_locked_tex = gfx.create_texture(g_music_locked, g_music_locked_len);
    g_music_dialog_tex = gfx.create_texture(g_music_dialog, g_music_dialog_len);
    g_music_comment_tex = gfx.create_texture(g_music_comment, g_music_comment_len);
    if (read_asset("/msgothic.ttc", g_font_bytes) || read_asset("/msgothic.ttf", g_font_bytes)) {const int offset=stbtt_GetFontOffsetForIndex(g_font_bytes.data(),0);g_font_ok=offset>=0&&stbtt_InitFont(&g_font,g_font_bytes.data(),offset)!=0;}
    start_animation();
    build_loading();
    start_loading();
    Viewport vp; vp.x = 0; vp.y = 0; vp.width = 1280; vp.height = 960;
    gfx.viewport(vp);
    return 1;
}
__attribute__((export_name("th19_loop_start"))) void th19_loop_start() {
    if (g_loop_started) return;
    g_loop_started = true;
    g_fixed_step.reset();
    emscripten_set_main_loop_arg(loop, nullptr, 0, 0);
}
__attribute__((export_name("th19_loop_stop"))) void th19_loop_stop() {
    if (!g_loop_started) return;
    g_loop_started = false;
    emscripten_cancel_main_loop();
}
__attribute__((export_name("th19_loop_pause"))) void th19_loop_pause(u32 paused) {
    g_loop_paused = paused != 0;
    g_fixed_step.reset();
}
__attribute__((export_name("th19_key"))) void th19_key(u32 code, u32 down) {
    if (code < 512) g_injected_keys[code] = down ? 1 : 0;
}
__attribute__((export_name("th19_touch_side"))) void th19_touch_side(u32 side){g_touch_side=side==1?1:0;g_touch_input.reset();}
__attribute__((export_name("th19_touch_sample"))) u32 th19_touch_sample(u32 side){
    static struct {u32 keys,direct;float x,y;} out{};out={};
    const auto s=touch_state(side==1?1:0);
    const auto sample=g_touch_input.sample(s,std::uint64_t(emscripten_get_now()),false,false);
    out.keys=(sample.keys[90]?1u:0u)|(sample.keys[88]?4u:0u)|(sample.keys[16]?8u:0u)|(sample.keys[38]?16u:0u)|(sample.keys[40]?32u:0u)|(sample.keys[37]?64u:0u)|(sample.keys[39]?128u:0u)|(sample.keys[27]?1024u:0u);
    if(sample.motion){out.direct=1;out.x=sample.x-s.x;out.y=sample.y-s.y;
      if(sample.motion==1){const float speed=sample.keys[16]?s.slow:s.fast,length=std::hypot(out.x,out.y);if(length>speed&&length>0){out.x*=speed/length;out.y*=speed/length;}}
      out.x=std::clamp(out.x,-512.f,512.f);out.y=std::clamp(out.y,-512.f,512.f);
    }
    return u32(&out);
}
__attribute__((export_name("th19_touch_options"))) void th19_touch_options(u32 enabled,u32 mode,float sensitivity,u32 two_finger,u32 double_tap){
    if(g_touch_input.enabled!=(enabled!=0)||g_touch_input.mode!=int(mode))g_touch_input.reset();
    g_touch_input.enabled=enabled!=0;g_touch_input.mode=std::clamp(int(mode),0,3);g_touch_input.unlimited=mode==1;
    g_touch_input.sensitivity=std::clamp(sensitivity,1.f,3.f);g_touch_input.two_finger=two_finger!=0;g_touch_input.double_tap=double_tap!=0;
}
__attribute__((export_name("th19_touch"))) void th19_touch(u32 type,u32 id,float x,float y){
    if(type>2||!std::isfinite(x)||!std::isfinite(y))return;
    g_touch_input.pointer(int(type),int(id),x,y,std::uint64_t(emscripten_get_now()),touch_state(g_touch_side),g_injected_keys[SDL_SCANCODE_LSHIFT]!=0);
}
__attribute__((export_name("th19_touch_controls"))) void th19_touch_controls(u32 fire,u32 focus,u32 bomb,u32 escape,float x,float y){g_touch_input.controls(fire!=0,focus!=0,bomb,escape,x*32767.f,y*32767.f);}
__attribute__((export_name("th19_touch_cancel"))) void th19_touch_cancel(){g_touch_input.reset();g_touch_input.fire=g_touch_input.focus=false;g_touch_input.stick_x=g_touch_input.stick_y=0;std::fill(std::begin(g_touch_keys),std::end(g_touch_keys),false);g_touch_motion_x=g_touch_motion_y=0;}
__attribute__((export_name("th19_touch_motion"))) void th19_touch_motion(float dx, float dy, u32 unlimited) {
    g_touch_unlimited = unlimited != 0;
    // Direct displacement follows the same ownership as keyboard movement.
    // Retain the user's touch mode, but never queue human input for a CPU.
    if (!g_touch_unlimited || (g_vs_mode && g_vs_entry == 2)) {
        g_touch_motion_x = g_touch_motion_y = 0.0f;
        return;
    }
    g_touch_motion_x += dx; g_touch_motion_y += dy;
}
__attribute__((export_name("th19_keys_clear"))) void th19_keys_clear() {
    memset(g_injected_keys, 0, sizeof(g_injected_keys));
    g_touch_motion_x = g_touch_motion_y = 0.0f;
}
__attribute__((export_name("th19_music_enabled"))) void th19_music_enabled(u32 enabled) {
    set_music_enabled(enabled != 0);
    if (g_music_enabled && g_phase == PHASE_TITLE && g_screen == SCR_TITLE && g_frame > LOADING_FRAMES + 10) {
        if (!g_audio.music) start_title_music();
    }
}
__attribute__((export_name("th19_test_vs_rules"))) u32 th19_test_vs_rules(i32 action,i32 value){
    if(action==10){g_vs_entry=0;g_char_sel=std::clamp(value/100,0,18);g_opp_sel=std::clamp(value%100,0,18);g_vs_rules[0]=1;g_vs_rules[1]=1;g_vs_rules[3]=1;g_vs_rules[4]=1;start_vs_match();g_vs_match.wins[0]=g_vs_match.target-1;begin_battle_report(value>=1900?1:0,false);}
    if(action==9){g_vs_entry=0;g_char_sel=0;g_opp_sel=1;g_vs_rules[0]=1;g_vs_rules[1]=2;g_vs_rules[3]=1;g_vs_rules[4]=1;start_vs_match();g_vs_match.wins[0]=g_vs_match.target-1;begin_battle_report(0,false);}
    if(action==4){g_vs_entry=2;g_char_sel=0;g_opp_sel=1;start_vs_match();}
    if(action==0){g_vs_entry=0;g_vs_rules[0]=1;g_vs_rules[1]=1;g_vs_rules[3]=1;g_vs_rules[4]=2;g_vs_rules[5]=2;g_vs_rules[6]=3;g_char_sel=0;g_opp_sel=1;start_vs_match();}
    if(action==1){g_vs_match.remaining=std::max(1,value);g_big_sprite=-1;}
    if(action==2)resolve_story_battle(value==0);
    if(action==3){g_stage_deaths[0]=value;g_stage_deaths[1]=0;for(auto&counts:g_vs_attack_counts)counts[0]=counts[1]=0;g_charge[0].ex_level=g_charge[1].ex_level=0;g_charge[0].boss_level=g_charge[1].boss_level=0;g_charge[0].energy=g_charge[1].energy=500;}
    static i32 out[16];out[0]=g_vs_match.round;out[1]=g_vs_match.wins[0];out[2]=g_vs_match.wins[1];out[3]=g_vs_match.remaining;out[4]=g_battle_winner;out[5]=g_battle_report_frame;out[6]=g_p1_lives;out[7]=g_p2_lives;out[8]=g_barrier[0].recovery;out[9]=g_barrier[1].recovery;out[10]=g_screen;out[11]=g_vs_match.target;out[12]=g_battle_timeout;out[13]=g_cpu_automatic;out[14]=g_cpu_left_bits;out[15]=g_opponent_controls;return (u32)out;
}
__attribute__((export_name("th19_overlay_status"))) u32 th19_overlay_status(){
 static i32 out[4];out[0]=g_win_stars_drawn[0];out[1]=g_win_stars_drawn[1];out[2]=int(g_present_fps.value*1000);out[3]=g_fps_glyphs;return (u32)out;
}
__attribute__((export_name("th19_title_status"))) u32 th19_title_status() {
    g_status[0] = (i32)g_frame;
    g_status[1] = g_screen;
    g_status[2] = g_story_session.stage_index();
    g_status[3] = g_story_session.opponent();
    g_status[4] = static_cast<i32>(g_story_session.phase());
    g_status[5] = g_screen==SCR_DIALOG?g_story_playback.page_index:g_story_session.page_index();
    g_status[6] = g_ecl_ready ? 1 : 0;
    g_status[7] = 1;
    g_status[8]=g_story_playback.waiting?1:0;
    g_status[9]=g_story_playback.time;
    g_status[10]=0;g_status[11]=g_status[12]=0;
    for(const auto&e:g_story_field.world.enemies)if(e.active&&e.anm_slot==3&&e.sprite>=0){++g_status[10];g_status[11]=i32(e.x*100);g_status[12]=i32(e.y*100);}
    g_status[13]=g_paused;g_status[14]=g_world_id;g_status[15]=int(g_player_options[0].size());g_status[16]=int(g_player_projectiles.size());g_status[17]=g_stage.time;g_status[18]=g_pause_confirm;
    g_status[19]=g_pause_view;g_status[20]=g_help_mode;g_status[21]=g_help_sel;g_status[22]=g_help_page;
    g_status[23]=g_pause_animation.child(62)?th19::PauseAnimation::menuColor(*g_pause_animation.child(62))>>24:0;g_status[24]=g_dialogue_name.stream&&g_dialogue_name.visible;
    g_status[25]=i32(g_dialogue_portraits[0].body.x*100);g_status[26]=i32(g_dialogue_portraits[1].body.x*100);
    g_status[27]=g_barrier[0].state;g_status[28]=g_barrier[0].elapsed;
    g_status[29]=g_charge_pattern[0].stream?g_charge_pattern[0].sprite:-1;g_status[30]=i32(g_charge_pattern[0].sx*100);g_status[31]=g_charge_visual_level[0];
    g_status[32]=g_barrier_vm[0].stream&&g_barrier_vm[0].visible&&g_barrier_vm[0].children.size()==2;
    g_status[33]=g_story_switch.phase;g_status[34]=g_story_switch.frame;g_status[35]=g_game_load_frames;g_status[36]=g_p1_lives;g_status[37]=g_p2_lives;
    return (u32)&g_status[0];
}
__attribute__((export_name("th19_marisa_skill_status"))) u32 th19_marisa_skill_status(){
    static i32 state[5];state[0]=int(g_attacks[0].marisa_skills.size());state[1]=g_attacks[0].marisa_skill_arrivals;state[2]=g_attacks[0].marisa_skills.empty()?-1:g_attacks[0].marisa_skills.front().state.frame;
    state[3]=int(g_ecl_fields[1].world.lasers.size());state[4]=0;for(const auto&l:g_ecl_fields[1].world.lasers)state[4]=std::max(state[4],int(l.width*100));return (u32)state;
}
__attribute__((export_name("th19_chiyari_skill_status"))) u32 th19_chiyari_skill_status(){
    static i32 state[16];state[0]=g_attacks[0].chiyari_skill.frame;state[1]=g_attacks[0].chiyari_skill_spawns;
    state[2]=int(g_ecl_fields[1].world.enemies.size());state[3]=int(g_attacks[0].chiyari_skill.x);state[4]=int(g_attacks[0].chiyari_skill.y);
    const auto&w=g_ecl_fields[1].world;state[5]=int(w.unhandled_opcodes.size());state[6]=w.field_ints[0];
    for(int i=7;i<16;++i)state[i]=0;
    for(const auto&e:w.enemies)if(e.sprite==11){state[7]++;state[8]=int(e.x);state[9]=int(e.y);state[10]=int(e.sx*1000);state[11]=int(e.animation_files.size());}
    return (u32)state;
}
// Diagnostic preview changes the live background without changing route progression.
__attribute__((export_name("th19_test_wave"))) i32 th19_test_wave(i32 wave){
    if(!g_ecl_ready||wave<1||wave>22)return 0;
    ++g_ecl_generation;
    for(auto&field:g_ecl_fields){
        field.world=th19::GameWorld{};field.world.difficulty=g_story_difficulty;field.world.bullet_scripts=&g_bullet_scripts;field.world.bullet_sprite_sizes=&g_bullet_sprite_sizes;
        for(const auto&p:field.programs)field.world.attach_program(*p);
        field.vm.start("DebugSkipFunc");
        char name[32];std::snprintf(name,sizeof name,"Wave%02d%s",(wave+1)/2,wave%2?"":"f");
        field.world.spawn_enemy(name,0,0,0,0,0);
    }
    return 1;
}
__attribute__((export_name("th19_test_attack_rank"))) void th19_test_attack_rank(i32 rank){g_charge[0].boss_level=std::clamp(rank,0,7);g_charge[0].energy=2500;}
__attribute__((export_name("th19_opponent_attack_status"))) u32 th19_opponent_attack_status(){
    static i32 out[14];std::fill(std::begin(out),std::end(out),0);const auto&w=g_ecl_fields[1].world;
    out[0]=w.boss_occupied();out[1]=int(w.bullets.size());out[2]=int(w.lasers.size());out[3]=int(w.unhandled_opcodes.size());
    for(const auto&e:w.enemies)if(e.active){++out[4];if(e.status_flags&0x80){++out[5];out[6]=int(e.x);out[7]=int(e.y);}
        if(e.vm&&e.vm->error())++out[8];}
    for(const auto&b:w.bullets){if(b.animation.stream&&b.animation.visible&&b.animation.alpha>0)++out[9];if(b.kind==11)++out[10];if(b.kind==1)++out[11];}
    out[12]=w.spell.active;out[13]=w.spell.number;return (u32)out;
}
// Isolated visual audit: keep the original initialized Boss actor, then enter an
// actual source-program phase. This does not assert natural story reachability.
__attribute__((export_name("th19_test_spell_phase"))) i32 th19_test_spell_phase(i32 subIndex,i32 rank){
    auto&w=g_ecl_fields[1].world;
    // Negative rank requests an isolated original ECL actor preview, not a
    // live gameplay trigger. Do not inherit the old Boss ANM slots or effects.
    if(rank<0){
        std::vector<const th19::EclProgram*> programs;const th19::EclProgram* source=nullptr;
        for(const auto&p:g_ecl_fields[0].programs){programs.push_back(p.get());for(const auto&s:p->subs)if(s.name=="BossAttack")source=p.get();}
        if(!source||subIndex<0||subIndex>=int(source->subs.size()))return 0;
        w.enemies.clear();w.boss_slots.fill(0);w.spell={};w.bullets.clear();w.lasers.clear();w.extra_flights.clear();
        if(rank==-2){ // Diagnostic: exercise credit -> flight -> original ECL, not direct actor spawn.
            auto&sender=g_ecl_fields[0].world;sender.native_extra_reservations.clear();sender.native_extra_sequence=0;
            sender.attack_credit_x=0;sender.attack_credit_y=220;sender.pending_native_attack_credit=300;
            w.extra_arrivals=0;sender.send_native_extra_credit(w,programs,g_char_sel);
        }else w.spawn_enemy(source->subs[subIndex].name,0,180,200,0,0,&programs);
        g_player_collision_enabled=false;g_player_projectiles.clear();return 1;
    }

    for(auto&e:w.enemies)if(e.active&&(e.status_flags&0x80)&&e.vm){
        const th19::EclProgram* source=nullptr;
        for(const auto&p:g_ecl_fields[0].programs)for(const auto&s:p->subs)if(s.name=="BossAttack")source=p.get();
        if(!source)return 0;const auto&p=*source;
        if(subIndex<0||subIndex>=int(p.subs.size()))return 0;
        e.attack_rank=rank;e.pending_phase.clear();e.phases={};
        e.vm->start(p.subs[subIndex].name.c_str());
        w.bullets.clear();w.lasers.clear();w.unhandled_opcodes.clear();w.spell.active=false;
        g_player_collision_enabled=false;g_player_projectiles.clear();return 1;
    }return 0;
}
__attribute__((export_name("th19_test_spell_diagnostics"))) u32 th19_test_spell_diagnostics(){
    static i32 out[520];std::fill(std::begin(out),std::end(out),0);
    const auto&w=g_ecl_fields[1].world;int n=0;
    for(const auto&entry:w.unhandled_opcodes){if(n>=250)break;out[8+n*2]=entry.first;out[9+n*2]=entry.second;++n;}
    out[0]=n;out[3]=int(w.extra_flights.size());out[4]=w.extra_arrivals;
    out[5]=g_actor_visual_counts[1][1];out[6]=g_actor_visual_counts[1][2];out[7]=g_actor_visual_counts[1][3];
    for(const auto&e:w.enemies)if(e.active){if(e.sprite>=0)++out[1];
        if(!std::isfinite(e.x)||!std::isfinite(e.y))++out[2];}
    for(const auto&b:w.bullets)if(!std::isfinite(b.x)||!std::isfinite(b.y))++out[2];
    return (u32)out;
}
__attribute__((export_name("th19_charge_attack_status"))) u32 th19_charge_attack_status(){
    static i32 state[30];std::fill(std::begin(state),std::end(state),0);
    state[0]=g_char_sel;state[1]=g_attacks[0].charge_attack.frame;state[2]=g_attacks[0].charge_attack.emitted;state[3]=g_charge[0].charge;state[4]=g_charge[0].last_release;
    for(const auto&b:g_player_projectiles)if(b.charged){++state[5];if(b.vm.visible&&b.vm.sprite>=0)++state[6];state[7]=int(b.speed*1000);}
    state[8]=g_attacks[0].charge_area.frame;state[9]=g_attacks[0].charge_area_vm.stream&&g_attacks[0].charge_area_vm.visible;state[10]=g_attacks[0].charge_area_vm.sprite;state[11]=int(g_attacks[0].charge_area_vm.children.size());
    for(const auto&b:g_player_projectiles)if(b.charged){if(!std::isfinite(b.x)||!std::isfinite(b.y)||!std::isfinite(b.speed)||!std::isfinite(b.angle))++state[12];if(b.ending)++state[13];if(std::abs(b.speed)<.001f)++state[14];state[15]=std::max(state[15],b.age);}
    state[16]=g_attacks[0].charge_attack.group;state[17]=g_charge[0].energy;state[18]=g_charge[0].thresholds[0];state[19]=g_charge[0].thresholds[1];
    state[20]=g_attacks[0].ran_skill.remaining;state[21]=int(g_player_options[0].size());state[22]=g_p1_bombs;state[23]=g_p1_fragments;state[24]=g_attacks[0].charge_area.enhanced;state[25]=g_barrier[0].invulnerable;
    state[26]=int(g_attacks[0].reimu_skills.size());state[27]=g_attacks[0].reimu_skills.empty()?-1:g_attacks[0].reimu_skills.front().state.frame;state[28]=int(g_attacks[0].skill_damage_circles.size());state[29]=g_ecl_fields[0].world.defeated;
    return (u32)state;
}
__attribute__((export_name("th19_test_cpu_runtime"))) u32 th19_test_cpu_runtime(i32 action,i32 value){
 if(action==6||action==7){
    auto&w=g_ecl_fields[0].world;
    if(action==6){w.bullets.clear();w.grazed_bullets=0;th19::LaserShooter shooter;shooter.type=0;shooter.speed=0;shooter.aim_mode=1;
      w.fire_bullets(shooter,g_p1_x+20,g_p1_y);}
    static i32 grazeStatus[4];grazeStatus[0]=w.grazed_bullets;
    grazeStatus[1]=w.bullets.empty()?-1:w.bullets[0].graze.contactFrames;
    grazeStatus[2]=w.bullets.empty()?-1:w.bullets[0].attached_effect.alpha;
    grazeStatus[3]=w.bullets.empty()?-1:w.bullets[0].graze.stock;
    return u32(grazeStatus);
 }
 if(action==8){ // Diagnostic repeat-aura fixture: preserve inactive first target in the vector.
    auto&w=g_ecl_fields[0].world;g_player_collision_enabled=false;
    if(value==0){g_ecl_fields[0].vm.start("DebugSkipFunc");w.enemies.clear();w.bullets.clear();w.lasers.clear();}
    else for(auto&e:w.enemies)e.active=false;
    auto&e=w.spawn_enemy("",value==0?-90.f:90.f,220,1000,0,0);e.vm.reset();e.status_flags=0x100000;e.sprite=0;
 }
 if(action==0){g_cpu_automatic=value!=0;}
 if(action==1){g_story_difficulty=std::clamp(value,0,3);g_cpu_difficulty.reset(g_story_difficulty,0);g_cpu_story.reset(1,g_story_difficulty);for(auto&f:g_ecl_fields)f.world.difficulty=g_story_difficulty;}
 if(action==2){g_charge[1].energy=2500;g_charge[1].cooldown=0;}
 if(action==3){g_ecl_fields[0].world.pending_story_credit+=value;}
 if(action==4){auto&w=g_ecl_fields[1].world;w.bullets.clear();auto&b=w.spawn_bullet();b.x=g_p2_x;b.y=g_p2_y-60;b.vy=2;b.kind=1;}
 if(action==5){g_cpu_story.reset(value,g_story_difficulty);g_ecl_fields[0].world.boss_slots={};g_ecl_fields[1].world.boss_slots={};}
 static i32 out[24];out[0]=g_cpu_automatic;out[1]=g_opponent_controls;out[2]=g_cpu_frames;out[3]=g_cpu_steers;
 out[4]=g_cpu_story.state;out[5]=g_cpu_story.credit;out[6]=g_cpu_story_events;out[7]=g_charge[1].charge;out[8]=g_charge[1].last_release;out[9]=g_charge[1].energy;
 for(int i=0;i<5;++i)out[10+i]=g_cpu_releases[i];out[15]=g_attacks[1].charge_attack.emitted;
 out[16]=g_attacks[1].charge_area.frame;out[17]=int(g_attacks[1].reimu_skills.size());out[18]=int(g_attacks[1].marisa_skills.size());out[19]=g_attacks[1].ran_skill.remaining;
 out[20]=g_attacks[1].chiyari_skill_spawns;out[21]=g_ecl_fields[0].world.story_boss_phase;out[22]=g_ecl_fields[0].world.story_final_boss;out[23]=g_charge[1].thresholds[1];return (u32)out;
}
__attribute__((export_name("th19_test_cpu_input"))) u32 th19_test_cpu_input(i32 bits){
    if(bits==-2){g_player_projectiles.clear();g_player_hit_effects.clear();for(auto&field:g_ecl_fields){auto&w=field.world;w.enemies.clear();
        auto&e=w.spawn_enemy("",0,260,100000,0,0);e.sprite=0;e.hitbox_w=180;e.hitbox_h=24;e.vm.reset();}}
    if(bits>=0){g_cpu_automatic=false;g_opponent_controls=u8(bits);}static i32 out[8];out[0]=g_opponent_controls;out[1]=g_opp_shoot_frame;out[2]=out[3]=0;
    for(const auto&b:g_player_projectiles)++out[2+b.side];out[4]=int(g_p2_x*1000);out[5]=int(g_p2_y*1000);for(int side=0;side<2;++side)out[6+side]=g_ecl_fields[side].world.enemies.empty()?-1:g_ecl_fields[side].world.enemies[0].life;return u32(uintptr_t(out));
}
__attribute__((export_name("th19_test_player_damage"))) u32 th19_test_player_damage(i32 action){
    static i32 out[12];
    if(action==1){g_player_collision_enabled=true;g_p1_lives=g_p2_lives=5;
        for(int side=0;side<2;++side){auto&f=g_ecl_fields[side];f.vm.start("DebugSkipFunc");
            f.world.enemies.clear();f.world.bullets.clear();f.world.lasers.clear();f.world.death_chains.clear();
            g_player_damage[side]={};g_barrier[side].reset();}
    }
    if(action==2||action==3){auto&w=g_ecl_fields[0].world;w.bullets.clear();
        if(action==3){g_barrier[0].state=2;g_barrier[0].elapsed=0;g_barrier[0].invulnerable=0;g_player_damage[0]={};}
        auto&b=w.spawn_bullet();b.x=g_p1_x;b.y=g_p1_y;b.kind=1;b.sprite=th19::nativeBulletSprite(1,0);
    }
    if(action==4){g_ecl_fields[0].world.bullets.clear();g_ecl_fields[1].world.bullets.clear();g_ecl_fields[0].world.lasers.clear();}
    if(action==5||action==6){auto&w=g_ecl_fields[0].world;w.lasers.clear();w.bullets.clear();
        th19::InfiniteLaser l;l.x=g_p1_x-50;l.y=g_p1_y;l.length=100;l.width=12;l.phase=action==5?3:2;
        l.source.max_length=100;l.source.width=12;l.source.warning=l.source.hold=1000;l.source.type=38;l.source.color=6;
        w.lasers.push_back(l);g_barrier[0].state=2;g_barrier[0].elapsed=0;g_barrier[0].invulnerable=0;g_player_damage[0]={};
    }
    for(int side=0;side<2;++side){out[side*6]=g_player_damage[side].state;out[side*6+1]=g_player_damage[side].frame;
        out[side*6+2]=side?g_p2_lives:g_p1_lives;out[side*6+3]=g_barrier[side].invulnerable;
        out[side*6+4]=int((side?g_p2_y:g_p1_y)*1000);out[side*6+5]=g_barrier[side].state;}
    return (u32)out;
}
__attribute__((export_name("th19_test_tsukasa_reflection"))) u32 th19_test_tsukasa_reflection(i32 action){
    auto&w=g_ecl_fields[0].world;static i32 out[5];
    if(action==1){w.bullets.clear();g_ecl_fields[1].world.bullets.clear();g_reflected_flights.clear();g_reflection_arrivals=0;
        for(int k=0;k<6;++k){auto&b=w.spawn_bullet();b.x=g_p1_x;b.y=g_p1_y-20-k*4;b.kind=k;b.color=k;b.sprite=96+k;b.vy=2;}}
    out[0]=int(g_reflected_flights.size());out[1]=g_reflection_arrivals;out[2]=0;out[3]=0;out[4]=0;
    for(const auto&b:w.bullets)if(b.state==1)++out[2];
    for(const auto&b:g_ecl_fields[1].world.bullets)if(b.reflected){++out[3];out[4]|=1<<b.color;}
    return (u32)out;
}
__attribute__((export_name("th19_test_spirit_routes"))) u32 th19_test_spirit_routes(i32 action){
    static i32 out[18];
    if(action==4){g_reflected_flights.clear();g_reflection_arrivals=0;g_spirit_clock=10800;
        for(auto&f:g_ecl_fields){f.vm.start("DebugSkipFunc");auto&w=f.world;w.enemies.clear();w.bullets.clear();w.counter_bullet_requests.clear();w.pure_spirit_meter={};
            w.pure_spirit_meter.add_bullets(1400,1,0,200);}}
    if(action==3||action==5){g_spirit_flights.clear();g_spirit_arrivals=0;
        if(action==5){g_reflected_flights.clear();g_reflection_arrivals=0;g_spirit_clock=0;}
        for(auto&f:g_ecl_fields){f.vm.start("DebugSkipFunc");auto&w=f.world;w.enemies.clear();w.bullets.clear();w.spirit_requests.clear();w.pure_spirit_meter={};w.combo=0;w.difficulty=1;
            w.reward_mode=action==5?0:-1;w.reward_stage=1;w.battle_frame=0;
            for(int n=0;n<14;++n){auto&e=w.spawn_enemy("",0,200,1,0,0);e.sprite=0;w.damage_enemy(e,1);}
        }}
    if(action==2){g_spirit_flights.clear();
        for(int side=0;side<2;++side){auto&f=g_ecl_fields[side];auto&w=f.world;f.vm.start("DebugSkipFunc");w.enemies.clear();w.bullets.clear();w.spirit_requests.clear();
            const char*names[]={"GhostWolf","GhostOtterL","GhostEagle"};
            for(int n=0;n<3;++n){auto&e=w.spawn_enemy(names[n],w.player_x+(n-1)*32,w.player_y-32,200,0,0);e.ecl_ints={1,0,1,0};}
        }}
    if(action==1){g_spirit_flights.clear();g_spirit_arrivals=0;g_spirit_clock=0;g_spirit_rng.state=1;
        for(int side=0;side<2;++side){auto&w=g_ecl_fields[side].world;w.enemies.clear();w.spirit_requests.clear();
            for(float distance:{40.f,90.f,180.f}){auto&e=w.spawn_enemy("",w.player_x,w.player_y-distance,1,0,0);
                e.sprite=0;w.set_spirit_kind(e,1,true);e.ecl_ints[2]=2;w.damage_enemy(e,1);}
        }}
    out[0]=int(g_spirit_flights.size());out[1]=g_spirit_arrivals;out[2]=g_spirit_overflow;
    out[3]=g_ecl_fields[0].world.counted_spirits();out[4]=g_ecl_fields[1].world.counted_spirits();
    out[5]=int(g_ecl_fields[0].world.unhandled_opcodes.size());out[6]=int(g_ecl_fields[1].world.unhandled_opcodes.size());out[7]=g_spirit_clock;
    for(int side=0;side<2;++side){out[8+side]=0;out[10+side]=0;out[12+side]=0;
        for(const auto&e:g_ecl_fields[side].world.enemies)if(e.active&&(e.status_flags&0x700000u)){
            out[8+side]+=bool(e.status_flags&0x80000u);out[10+side]+=int(e.speed*1000);++out[12+side];}}
    out[14]=int(g_reflected_flights.size());out[15]=g_reflection_arrivals;out[16]=out[17]=0;
    for(int side=0;side<2;++side)for(const auto&b:g_ecl_fields[side].world.bullets)if(b.reflected)out[16+side]|=1<<b.kind;
    return (u32)out;
}
__attribute__((export_name("th19_test_shared_cancel"))) u32 th19_test_shared_cancel(i32 action){
    auto&w=g_ecl_fields[0].world;static i32 state[6];
    if(action==1){w.enemies.clear();w.lasers.clear();w.bullets.clear();w.death_chains.clear();
        th19::InfiniteLaser l;l.id=w.next_id++;l.x=-64;l.y=200;l.angle=0;l.length=128;l.width=14;l.phase=2;
        l.source.type=38;l.source.color=6;l.source.max_length=128;l.source.speed=0;l.source.hold=1000;w.lasers.push_back(l);
    }
    if(action==2)w.cancel_lasers_circle(-24,200,0);
    state[0]=int(w.lasers.size());state[1]=0;for(const auto&l:w.lasers)state[1]+=l.fragment;
    state[2]=w.lasers.empty()?0:int(w.lasers[0].length);state[3]=w.cancelled_bullets;
    state[4]=int(w.death_chains.size());state[5]=w.defeated;return (u32)state;
}
__attribute__((export_name("th19_test_zanmu_targets"))) u32 th19_test_zanmu_targets(i32 reset){
    auto&w=g_ecl_fields[0].world;
    if(reset){w.enemies.clear();for(int i=0;i<6;++i){auto&e=w.spawn_enemy("",g_p1_x+(i-2)*30,g_p1_y-100,200,0,0);
        e.status_flags=i==4?0x200000u:i==5?0x10800000u:0x10000000u;e.sprite=0;e.vm.reset();}}
    static i32 s[4];s[0]=g_attacks[0].charge_area.converted_count;s[1]=g_attacks[0].charge_area.conversion_sequence;s[2]=0;s[3]=int(g_attacks[0].charge_area.radius());
    for(const auto&e:w.enemies)if(e.active&&e.vm&&std::string(e.vm->current_sub_name()).find("Ghost")!=std::string::npos)++s[2];
    return (u32)s;
}
// Deterministic audit entry points: same initialization/input/tick paths,
// with waves suspended so enemies cannot consume the shots under inspection.
__attribute__((export_name("th19_test_character"))) i32 th19_test_character(i32 ch){
    if(ch<0||ch>=19)return 0;
    memset(g_injected_keys,0,sizeof(g_injected_keys));g_paused=false;g_phase=PHASE_TITLE;g_screen=SCR_GAME;
    g_story_switch.phase=0;g_trans=TRANS_NONE;g_confirm_delay=g_cancel_delay=g_game_load_frames=0;
    g_char_sel=ch;g_story_session.begin(th19::STORY_ROUTES[ch]);g_opp_sel=g_story_session.opponent();g_story_session.skip_scene();start_game();
    for(auto&field:g_ecl_fields){field.vm.start("DebugSkipFunc");field.world.enemies.clear();field.world.bullets.clear();}
    g_cpu_automatic=false;g_opponent_controls=0;g_p1_bombs=g_p2_bombs=3;g_p1_fragments=g_p2_fragments=0;g_charge[0].energy=2500;game_tick();render();return 1;
}
__attribute__((export_name("th19_test_cpu_character"))) i32 th19_test_cpu_character(i32 ch){
    if(ch<0||ch>=19)return 0;g_opp_sel=ch;start_game();
    for(auto&field:g_ecl_fields){field.vm.start("DebugSkipFunc");field.world.enemies.clear();field.world.bullets.clear();}
    game_tick();render();return 1;
}
__attribute__((export_name("th19_test_story_start"))) i32 th19_test_story_start(i32 ch){
    if(ch<0||ch>=19)return 0;
    memset(g_injected_keys,0,sizeof(g_injected_keys));g_paused=false;g_phase=PHASE_TITLE;
    g_trans=TRANS_NONE;g_confirm_delay=g_cancel_delay=0;g_char_sel=ch;
    start_dialogue();render();return 1;
}
// Original pl00 504-emitter fixture: observations only, except explicit source removal.
__attribute__((export_name("th19_test_shot_trails"))) u32 th19_test_shot_trails(i32 clearSources){
    if(clearSources){g_player_projectiles.clear();g_attacks[0].charge_attack={};}
    static i32 result[12];std::fill(std::begin(result),std::end(result),0);
    result[0]=int(g_player_projectiles.size());result[1]=int(g_player_hit_effects.size());
    for(const auto& b:g_player_hit_effects)if(b.side==0){
        result[2]=int(b.x*1000);result[3]=int(b.y*1000);result[4]=b.vm.alpha;
        result[5]=b.vm.sprite;result[6]=int(b.vm.rot_z*1000);result[7]=b.vm.time;
    }
    for(const auto& b:g_player_projectiles)result[8]+=int(b.vm.children.size());
    for(const auto& b:g_player_projectiles)if(b.side==0&&(b.vm.sprite==29||b.vm.sprite==30)){
        ++result[9];result[10]=std::max(result[10],int(b.vm.sx*1000));result[11]=std::max(result[11],b.vm.alpha);
    }
    return (u32)result;
}
// Real ordinary SHT shots against one durable target; inspect flame aftermath.
__attribute__((export_name("th19_test_flame_hit"))) u32 th19_test_flame_hit(i32 mode){
    auto&w=g_ecl_fields[0].world;
    if(mode==1){w.enemies.clear();w.bullets.clear();g_player_projectiles.clear();g_player_hit_effects.clear();g_attacks[0].skill_damage_circles.clear();
        g_cpu_automatic=false;g_opponent_controls=0;g_test_card_shoot_only=false;
        auto&e=w.spawn_enemy("",g_p1_x,g_p1_y-48,100000,0,0);e.sprite=0;e.hitbox_w=240;e.hitbox_h=24;e.vm.reset();
    }
    if(mode==2)g_player_projectiles.erase(std::remove_if(g_player_projectiles.begin(),g_player_projectiles.end(),[](const auto&b){return !b.ending;}),g_player_projectiles.end());
    static i32 r[8];std::fill(std::begin(r),std::end(r),0);r[0]=w.enemies.empty()?-1:w.enemies.front().life;
    r[1]=int(g_attacks[0].skill_damage_circles.size());
    for(const auto&b:g_player_projectiles)if(b.side==0&&b.ending&&(b.hit_callback==3||b.hit_callback==4||b.hit_callback==9)){
        ++r[2];r[3]+=int(b.vm.children.size());r[4]=b.vm.sprite;r[5]=int(b.speed*1000);r[6]+=b.damage;
    }
    r[7]=int(g_player_hit_effects.size());return (u32)r;
}
// Deterministic laser occlusion fixture: actual input/shots, two durable targets.
__attribute__((export_name("th19_test_laser_targets"))) u32 th19_test_laser_targets(i32 reset){
    auto&w=g_ecl_fields[0].world;
    if(reset==3){ // Measure established beams without ordinary-shot/next-burst contamination.
        g_player_projectiles.erase(std::remove_if(g_player_projectiles.begin(),g_player_projectiles.end(),[](const auto&b){return b.type!=2&&b.update!=9;}),g_player_projectiles.end());
        g_attacks[0].charge_attack={};g_test_card_shoot_only=true;
        int i=0;for(auto&e:w.enemies){e.life=100000;e.x=g_p1_x;e.y=g_p1_y-(i++?80.f:48.f);e.hitbox_w=1000;e.hitbox_h=24;}
    }else if(reset){w.enemies.clear();g_player_projectiles.clear();g_player_hit_effects.clear();
        for(float y:{260.f,150.f}){auto&e=w.spawn_enemy("",g_p1_x,y,100000,0,0);
            e.sprite=0;e.hitbox_w=180;e.hitbox_h=24;e.vm.reset();}
        if(reset==2){int i=0;for(auto&e:w.enemies){e.x=g_p1_x+(i++==0?-96:96);e.y=g_p1_y-48;e.hitbox_w=24;e.hitbox_h=80;}}}
    static i32 state[6];std::fill(std::begin(state),std::end(state),0);
    state[5]=int(g_player_hit_effects.size());
    for(int i=0;i<2&&i<int(w.enemies.size());++i)state[i]=w.enemies[i].life;
    for(const auto&b:g_player_projectiles)if(!b.ending&&(b.type==2||b.update==9)){
        ++state[2];state[3]=std::max(state[3],int(b.length));if(b.hit_callback==8)++state[4];}
    return (u32)state;
}
// Controlled feedback capture on a real C4 actor; never used by gameplay.
__attribute__((export_name("th19_test_enemy_feedback"))) u32 th19_test_enemy_feedback(i32 mode){
    static i32 result[4];std::fill(std::begin(result),std::end(result),0);
    auto&w=g_ecl_fields[1].world;
    for(auto&e:w.enemies)if(e.active&&(e.status_flags&0x80)&&e.sprite>=0){
        if(mode>=0){e.hit_cooldown=0;e.age=mode==2?5:4;
            int threshold=0;for(const auto&t:e.phases.entries)if(t.hp>=0){threshold=t.hp;break;}
            e.life=threshold+(mode==0?1000:50);w.sound_events.clear();
            if(mode==0)w.damage_enemy(e,1);
            w.finish_hit_feedback(w.spell.active?1u:0u);}
        result[0]=e.phase_health();result[1]=e.hit_cooldown;result[2]=e.feedback_color;
        result[3]=w.sound_events.empty()?-1:w.sound_events.back();break;
    }render();return (u32)result;
}
__attribute__((export_name("th19_test_ticks"))) void th19_test_ticks(i32 count){
    const bool paused=g_loop_paused;g_loop_paused=false;
    count=std::clamp(count,0,600);
    for(int i=0;i<count;++i){g_skip_present=i+1<count;game_tick();}
    g_skip_present=false;g_loop_paused=paused;g_fixed_step.reset();
}
// Exercise actual dialogue input/timing without presenting each key edge.
// Stops at battle/ending/title; it never injects a result or skips a scene.
__attribute__((export_name("th19_test_story_pump"))) void th19_test_story_pump(){
    const bool paused=g_loop_paused;g_loop_paused=false;
    for(int i=0;i<600&&g_screen==SCR_DIALOG;++i){
        g_injected_keys[SDL_SCANCODE_Z]=(i%20)<2;
        g_skip_present=true;game_tick();
    }
    g_injected_keys[SDL_SCANCODE_Z]=false;g_skip_present=false;
    g_loop_paused=paused;g_fixed_step.reset();render();
}
__attribute__((export_name("th19_character_combat_status"))) u32 th19_character_combat_status(){
    static i32 state[17];std::fill(std::begin(state),std::end(state),0);
    state[0]=g_char_sel;
    for(const auto&b:g_player_projectiles)if(b.option==1||b.option==2){
        ++state[b.option];if(b.vm.visible&&b.vm.sprite>=0)++state[2+b.option];
    }
    const auto&r=g_ecl_fields[0].world.charge_region;state[5]=r.count;state[6]=int(r.radius*1000);state[7]=int(r.angle*1000000);
    for(int i=0;i<3;++i){state[8+i*2]=int(r.centers[i].x*1000);state[9+i*2]=int(r.centers[i].y*1000);}
    state[14]=int(g_p1_x*1000);state[15]=int(g_p1_y*1000);state[16]=r.held;
    return (u32)state;
}
// Read-only diagnostic for both players; state and original ANM remain shared
// by local VS and network VS (including rollback checkpoints).
__attribute__((export_name("th19_barrier_visual_status"))) u32 th19_barrier_visual_status(){
 static i32 out[12];for(int side=0;side<2;++side){const auto&v=g_barrier_vm[side];int i=side*6;
 out[i]=g_barrier[side].state;out[i+1]=v.stream&&v.visible;out[i+2]=int(v.children.size());
 out[i+3]=g_charge[side].level();out[i+4]=g_charge[side].last_release;out[i+5]=g_barrier[side].invulnerable;}
 return u32(out);
}
__attribute__((export_name("th19_test_barrier_hit"))) i32 th19_test_barrier_hit(i32 side){
    if(!trigger_player_barrier(side))return 0;play_sfx(27);return 1;
}
__attribute__((export_name("th19_test_chain_hits"))) void th19_test_chain_hits(i32 side,i32 count){
    if(side<0||side>1)return;
    auto&w=g_ecl_fields[side].world;
    for(int i=0;i<std::clamp(count,0,100);++i){th19::Enemy e;e.sprite=0;e.life=1;w.damage_enemy(e,1);}
    render();
}
__attribute__((export_name("th19_background_preview"))) i32 th19_background_preview(i32 id,i32 frames) {
    if((g_screen!=SCR_GAME&&g_screen!=SCR_DIALOG)||id<0||id>=17)return 0;
    if(!load_world(id))return 0;
    for(int i=0;i<std::clamp(frames,0,15000);++i)tick_world();
    render();return 1;
}
// Diagnostic battle result injection; not bound to any gameplay input.
// Lifecycle fixture: actual original BossAttack/BossDead scripts, without
// a victory injection. Damage is forced to isolate the final-Boss handoff.
__attribute__((export_name("th19_test_final_boss"))) i32 th19_test_final_boss(i32 action){
    auto&w=g_ecl_fields[0].world;
    if(action==0||action==2||action==3){
        g_cpu_automatic=false;g_opponent_controls=0;g_player_collision_enabled=false;
        w.enemies.clear();w.bullets.clear();w.lasers.clear();w.boss_slots={};
        g_ecl_fields[0].vm.start("DebugSkipFunc");
        w.story_final_boss=action==0;w.story_boss_phase=action==3?3:2;
        std::vector<const th19::EclProgram*> source;
        for(const auto&p:g_ecl_fields[1].programs)source.push_back(p.get());
        return w.receive_charge_attack(4,0,0,source);
    }
    if(action==1){
        for(auto&e:w.enemies)if(e.active&&(e.status_flags&0x80)){
            e.pending_phase="BossDead";return 1;
        }
        return 0;
    }
    if(action==4)return w.spell.active;
    return g_player_damage[1].state;
}
__attribute__((export_name("th19_test_battle_result"))) i32 th19_test_battle_result(i32 won) {
    if(g_screen!=SCR_GAME)return 0;
    (won?g_p2_lives:g_p1_lives)=-1; // deterministic defeated-player fixture
    return resolve_story_battle(won!=0);
}
// Reward fixture: positive IDs reset, negative IDs append for combination tests.
// No keyboard shortcut in normal gameplay.
__attribute__((export_name("th19_test_card_reward"))) void th19_test_card_reward(i32 id){
    const bool append=id<0;if(append)id=-id;
    if(id<1||id>38)return;load_ability_cards();if(!append){g_owned_cards.clear();g_owned_card_vms.clear();g_card_options.clear();}
    begin_card_reward((id-1)/2);g_card_reward=id;g_card_reward_vm.sprite=5+2*id;
    for(int i=0;i<120;++i)tick_card_reward(false);tick_card_reward(true);
    for(int i=0;i<10;++i)tick_card_reward(false);g_card_reward_exit=-1;
}
__attribute__((export_name("th19_test_card_shoot_only"))) void th19_test_card_shoot_only(i32 enabled){g_test_card_shoot_only=enabled!=0;}
// Read-only diagnostics for reward/inventory and native stat wiring.
__attribute__((export_name("th19_card_status"))) u32 th19_card_status(){
    static i32 state[267];std::fill(std::begin(state),std::end(state),-1);
    for(size_t i=0;i<g_owned_cards.size()&&i<256;++i)state[11+i]=g_owned_cards[i];
    state[0]=g_card_reward;state[1]=g_card_reward_frame;state[2]=g_card_reward_exit;
    state[3]=int(g_owned_cards.size());state[4]=g_owned_cards.empty()?-1:g_owned_cards.back();
    state[5]=g_cards_dim;state[6]=g_charge[0].charge_speed_percent;state[7]=g_bombs[0].duration;state[8]=int(g_card_options.size());state[9]=0;for(const auto&b:g_player_projectiles)if(b.ability_bank)++state[9];state[10]=g_p1_bombs;return (u32)state;
}
__attribute__((export_name("th19_test_bomb"))) u32 th19_test_bomb(i32 action) {
    auto&w=g_ecl_fields[0].world;
    if(action==1){w.bullets.clear();for(float offset:{20.f,100.f,300.f})for(float dy:{0.f,-96.f,96.f}){
        auto&b=w.spawn_bullet();b.x=g_p1_x+offset;b.y=g_p1_y+dy;b.hit_width=4;b.state=1;
    }}
    if(action==2)g_p1_bombs=0;
    static i32 status[12];const auto&b=g_bombs[0];
    status[0]=b.frame;status[1]=b.duration;status[2]=g_p1_bombs;status[3]=b.active();
    status[4]=int(b.radius*1000);status[5]=int(b.x*1000);status[6]=int(b.y*1000);
    status[7]=w.cancelled_bullets;status[8]=g_barrier[0].invulnerable;
    status[9]=g_bomb_area[0].visible;status[10]=g_bomb_back[0].visible;status[11]=g_charge[0].energy;
    return (u32)status;
}
__attribute__((export_name("th19_test_challenge_clear"))) void th19_test_challenge_clear(i32 difficulty,i32 continues,i32 lives){
 g_story_difficulty=std::clamp(difficulty,0,3);g_story_continues=std::max(0,continues);g_story_initial_lives=5;g_p1_lives=lives;g_story_challenge_run=true;g_vs_mode=0;g_ending_id=0;g_ending_return_screen=SCR_TITLE;enter_ending_clear();
}
__attribute__((export_name("th19_achievement_status"))) u32 th19_achievement_status(){
 static i32 s[9];s[0]=g_screen;s[1]=g_sub_sel;s[2]=g_achievement_detail_frame;s[3]=g_achievement_exiting;
 for(int i=0;i<4;++i)s[4+i]=g_achievement_details[i].alpha;s[8]=g_sub_vms.empty()?-1:int(g_sub_vms[0].y);return (u32)s;
}
__attribute__((export_name("th19_music_room_status"))) u32 th19_music_room_status(){
 static i32 s[14];s[0]=g_screen;s[1]=g_sub_sel;s[2]=g_music_prompt;s[3]=g_music_revealed;s[4]=g_music_warning_shown;s[5]=g_music_age;
 s[6]=g_music_rows[g_sub_sel].color();s[7]=g_music_lines[0].alpha;s[8]=g_music_lines[7].alpha;
 s[9]=int(g_music_lines[0].y);s[10]=int(g_music_lines[7].y);s[11]=g_music_scroll;s[12]=g_music_exiting;s[13]=g_music_comment_track;return (u32)s;
}
__attribute__((export_name("th19_test_continue"))) u32 th19_test_continue(i32 stage){
 if(stage>=0){
  g_vs_mode=0;g_char_sel=0;g_story_session.begin(th19::STORY_ROUTES[0]);
  for(int i=0;i<std::min(stage,5);++i){g_story_session.skip_scene();g_story_session.resolve_battle(true);g_story_session.skip_scene();}
  g_story_initial_lives=5;g_story_continues=0;g_story_challenge_run=true;g_trans=TRANS_NONE;
  enter_story_stage();g_game_load_frames=0;g_story_session.skip_scene();g_p1_lives=0;
  g_story_session.resolve_battle(false);g_story_playback.begin(g_story_session.scene());reset_dialogue_portraits();
 }
 static i32 s[9];s[0]=g_screen;s[1]=g_story_session.stage_index();s[2]=int(g_story_session.phase());
 s[3]=g_paused;s[4]=g_pause_animation.retryMenu;s[5]=g_p1_lives;s[6]=g_story_continues;s[7]=g_pause_choice;s[8]=g_story_playback.finished;return (u32)s;
}
__attribute__((export_name("th19_test_ending_flow"))) void th19_test_ending_flow(i32 ch){
    g_char_sel=std::clamp(ch,0,18);g_ending_id=g_char_sel;g_ending_return_screen=SCR_TITLE;
    g_screen=SCR_GAME;g_final_exit=0;g_trans=TRANS_NONE;g_paused=false;
}
__attribute__((export_name("th19_ending_status"))) u32 th19_ending_status(){static i32 s[8];s[0]=g_screen;s[1]=g_final_exit;s[2]=g_ending_group;s[3]=g_ending_tail;s[4]=g_staff_frame;s[5]=g_staff_fade;s[6]=int(g_staff_event);s[7]=g_ending_line;return (u32)s;}
__attribute__((export_name("th19_boss_timer_status"))) u32 th19_boss_timer_status(){return (u32)g_timer_audit;}
__attribute__((export_name("th19_storage_revision"))) u32 th19_storage_revision() {
    return g_save_revision;
}
__attribute__((export_name("th19_trophy_popup_status"))) u32 th19_trophy_popup_status(){static i32 s[3];s[0]=g_trophy_popup_id;s[1]=g_trophy_popup_age;s[2]=int(g_trophy_popup_queue.size());return (u32)s;}
__attribute__((export_name("th19_notice_display_status"))) u32 th19_notice_display_status(){static i32 s[3];s[0]=g_notice_id;s[1]=g_notice_age;s[2]=g_notice_close;return (u32)s;}
__attribute__((export_name("th19_notice_status"))) u32 th19_notice_status(){static i32 s[25];std::fill(s,s+25,-1);s[0]=int(g_pending_route_notices.size());for(size_t i=0;i<g_pending_route_notices.size()&&i<24;++i)s[i+1]=g_pending_route_notices[i];return (u32)s;}
// Read-only progress diagnostics; no keyboard shortcut or unlock operation.
__attribute__((export_name("th19_progress_status"))) u32 th19_progress_status(){
 static u32 s[83];s[0]=g_native_score_loaded;s[1]=g_native_score_fingerprint;
 for(int i=0;i<19;++i){s[2+i]=g_character_clear[i];s[21+i]=g_story_available[i];s[40+i]=g_vs_available[i];}
 for(int i=0;i<23;++i)s[59+i]=g_trophy_unlocked[i];s[82]=g_ending_repeat;return (u32)s;
}
__attribute__((export_name("th19_ecl_status"))) u32 th19_ecl_status() {
    static i32 status[12];
    i32 active=0,errors=g_ecl_vm.error()?1:0;
    for(const auto& e:g_ecl_world.enemies){if(e.active)++active;if(e.vm&&e.vm->error())++errors;}
    status[0]=g_ecl_world.current_wave;
    status[1]=i32(g_ecl_world.wave_schedule.cursor());
    status[2]=i32(g_ecl_world.enemies.size());
    status[3]=active;status[4]=errors;
    status[5]=i32(g_ecl_world.unhandled_opcodes.size());
    status[6]=g_ecl_world.difficulty;status[7]=i32(g_ecl_world.last_spawned_id);status[8]=g_ecl_world.defeated;status[9]=g_ecl_world.collected;status[10]=g_charge[0].energy;status[11]=int(g_ecl_world.pickups.size());
    return (u32)status;
}
// The import candidate lives in MEMFS, outside the persistent save mount.
__attribute__((export_name("th19_validate_score_import"))) u32 th19_validate_score_import(){
 std::vector<u8>raw,plain;const u8*status=nullptr;
 return read_file("/score-import.dat",raw)&&decode_native_score(raw,plain,status);
}
__attribute__((export_name("th19_character_navigation_status"))) u32 th19_character_navigation_status(){
 static i32 s[4];s[0]=g_char_sel;s[1]=g_opp_sel;s[2]=g_char_switch_frame;s[3]=g_bob_phase;return (u32)s;
}
__attribute__((export_name("th19_entry_status"))) u32 th19_entry_status(){
 static i32 s[12];s[0]=g_screen;s[1]=g_story_switch.phase;s[2]=g_story_switch.frame;s[3]=g_vs_match_confirm;
 s[4]=g_world_id;s[5]=g_story_music;s[6]=g_vs_scene_side;s[7]=g_life_capacity[0];s[8]=g_life_capacity[1];
 s[9]=g_p1_lives;s[10]=g_p2_lives;s[11]=g_game_load_frames;return (u32)s;
}
__attribute__((export_name("th19_audio_path"))) const char* th19_audio_path(){return g_audio.current_music.c_str();}
__attribute__((export_name("th19_test_vs_scene"))) u32 th19_test_vs_scene(i32 left,i32 right,u32 seed,i32 life0,i32 life1){
 g_char_sel=std::clamp(left,0,18);g_opp_sel=std::clamp(right,0,18);g_vs_scene_rng.state=seed?seed:1;
 g_vs_rules[3]=std::clamp(life0,1,5)-1;g_vs_rules[4]=std::clamp(life1,1,5)-1;g_vs_entry=0;start_vs_match();render();return th19_entry_status();
}
__attribute__((export_name("th19_save_snapshot"))) u32 th19_save_snapshot() {
    return 1;
}
__attribute__((export_name("th19_game_close"))) void th19_game_close() {
    th19_loop_stop();
    if (g_audio.music) { ma_sound_stop(&g_audio.music->sound); g_audio.music.reset(); }
}
__attribute__((export_name("th19_error"))) const char* th19_error() {
    return gfx.error();
}
}





