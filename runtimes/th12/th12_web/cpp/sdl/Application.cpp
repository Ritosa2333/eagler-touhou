#include "GraphicsDevice.hpp"
#if PORT_TITLE == 14
#include "PauseCaptureTh14.hpp"
#include "ApplicationCaptureTh14.hpp"
#include "StageUploadsTh14.hpp"
#include "StageDecodeTh14.hpp"
#include "StageWorkerWaitTh14.hpp"
#include "../game/ReplayCadenceTh14.hpp"
#include <random>
#endif
#include "AudioDevice.hpp"
#include "FontDevice.hpp"
#include "../game/MusicCatalog.hpp"
#include "../game/GameSession.hpp"
#include "../game/FrameStatistics.hpp"
#if defined(TH14_CPP_STAGE_DECODE)
#include "../game/StageWorkerTailTh14.hpp"
#endif
#include "../game/LoadingTh14.hpp"
#if PORT_TITLE == 14 && defined(TH14_CPP_BOOT_WORKER)
#include "../game/BootCppResourcesTh14.hpp"
#include "../game/TitleCppResourcesTh14.hpp"
#if defined(TH14_CPP_STAGE_WORKER) && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE) && !(defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER))
#define TH14_CPP_INITIAL_LOADING 1
#include "../game/InitialCppResourcesTh14.hpp"
#endif
#include "AudioCppBootTh14.hpp"
#endif
#include "../game/AnmRenderer.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#ifdef TH14_WEBGPU_BACKEND
#include "../../../portable/sdl/Th14WebGpuCallbacks.hpp"
#include "../game/StageFiles.hpp"
#elif PORT_TITLE == 14
#include "../../../portable/sdl/Th14AsyncCallbacks.hpp"
#endif
#include <emscripten/heap.h>
#include <malloc.h>
#include <emscripten/html5.h>
#include "../../../portable/sdl/FrameCadence.hpp"
#include "../../../portable/input/TouchController.hpp"
#include <algorithm>
#include <array>
#if defined(TH12_DELETE_GUARD)
// Diagnostic build only. The allocator reports a negative live-byte total just before the fault, with a free
// total of exactly the same magnitude, which is the signature of releasing more than was ever handed out. The
// damage shows up much later as an out-of-bounds access inside the allocator with no application frame in the
// stack, so every pointer handed out is recorded here and checked off when it comes back: one that returns twice,
// or was never handed out, is reported at the release that did it. The table is fixed size on purpose so that
// recording a pointer never allocates.
#include <cstdlib>
#include <cstdio>
#include <malloc.h>
#include <emscripten/heap.h>
extern "C" unsigned char __heap_base;
extern "C" void* __real_malloc(std::size_t);
extern "C" void* __real_calloc(std::size_t,std::size_t);
extern "C" void* __real_realloc(void*,std::size_t);
extern "C" void __real_free(void*);
namespace {
constexpr uint32_t kSlots=1u<<20;
uint32_t* live_table=nullptr;
uint32_t slot_of(void* p){return (uint32_t((reinterpret_cast<uintptr_t>(p)>>4)*2654435761u))&(kSlots-1);}
void track_add(void* p){
    if(!p)return;
    if(!live_table)live_table=static_cast<uint32_t*>(__real_calloc(kSlots,sizeof(uint32_t)));
    if(!live_table)return;
    uint32_t slot=slot_of(p);
    for(uint32_t n=0;n<kSlots;++n){uint32_t& e=live_table[slot];if(!e||e==uint32_t(reinterpret_cast<uintptr_t>(p))){e=uint32_t(reinterpret_cast<uintptr_t>(p));return;}slot=(slot+1)&(kSlots-1);}
}
// Returns 0 when the pointer was never handed out, 1 when it was live and is now cleared, 2 when the table is
// not ready yet and the check cannot be made.
int track_remove(void* p){
    if(!p)return 1;
    if(!live_table)return 2;
    uint32_t slot=slot_of(p);
    for(uint32_t n=0;n<kSlots;++n){uint32_t& e=live_table[slot];if(e==uint32_t(reinterpret_cast<uintptr_t>(p))){e=0;return 1;}if(!e)return 0;slot=(slot+1)&(kSlots-1);}
    return 0;
}
bool inside_heap(const void* p){
    const uintptr_t base=reinterpret_cast<uintptr_t>(&__heap_base);
    const uintptr_t end=base+emscripten_get_heap_size();
    const uintptr_t value=reinterpret_cast<uintptr_t>(p);
    return value>=base&&value<end;
}
void inspect(void* p,const char* kind){
    if(!p)return;
    if(!inside_heap(p)){std::fprintf(stderr,"ALLOC GUARD: %s of a non-heap pointer %p\n",kind,p);std::fflush(stderr);return;}
    const int state=track_remove(p);
    if(state==0){std::fprintf(stderr,"ALLOC GUARD: %s of a pointer that was never handed out: %p\n",kind,p);std::fflush(stderr);}
}
}
// The check lives in __wrap_free alone. Checking here as well would see every pointer a second time, after the
// wrapper had already cleared it, and report a double release for every ordinary delete.
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
void* operator new(std::size_t n){void* p=std::malloc(n?n:1);track_add(p);return p;}
void* operator new[](std::size_t n){return operator new(n);}
extern "C" void* __wrap_malloc(std::size_t n){void* p=__real_malloc(n);track_add(p);return p;}
extern "C" void* __wrap_calloc(std::size_t c,std::size_t n){void* p=__real_calloc(c,n);track_add(p);return p;}
extern "C" void* __wrap_realloc(void* p,std::size_t n){
    if(p)inspect(p,"realloc");
    void* q=__real_realloc(p,n);track_add(q);return q;
}
extern "C" void __wrap_free(void* p){
    if(p)inspect(p,"free");
    __real_free(p);
}
#endif
EM_JS(int, th11_browser_keyboard, (), { return typeof Module["resetBrowserKeyboard"] === "function"; });
#if PORT_TITLE == 14
// Browser has no Windows boot-time clock. Preserve native integer wrapping
// using this environment's actual monotonic millisecond input.
EM_JS(unsigned, th14_clock_ticks, (), { return performance.now() >>> 0; });
#if defined(TH14_STAGE_RESTORE_DELIVERY)
EM_JS(int,th14_stage_restore_begin,(int playback),{
    return (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.stageRestore?.begin(playback!==0)?1:0;
});
EM_JS(int,th14_stage_restore_ready,(unsigned operation),{
    return (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.stageRestore?.ready(operation)??2;
});
EM_JS(int,th14_stage_restore_committed,(unsigned operation),{
    return (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.stageRestore?.committed(operation)?1:0;
});
EM_JS(void,th14_stage_restore_cancel,(),{
    (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.stageRestore?.cancel();
});
#endif
#if defined(TH14_SHARED_SCENE_COPY)
EM_JS(int,th14_shared_scene_prepare,(),{
    return (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.sceneCopy?.prepare()?1:0;
});
EM_JS(int,th14_shared_scene_read,(unsigned source,unsigned layout,unsigned word,unsigned offset,unsigned destination),{
    const reader=(Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.sceneCopy;
    if(!reader)return 2;
    const offsets=Array.from(HEAPU32.subarray(layout>>>2,(layout>>>2)+75));
    const status=reader.read(HEAPU8.buffer,source,offsets,word,offset);
    if(status===1)HEAPU32[destination>>>2]=reader.value(word);
    return status;
});
EM_JS(void,th14_shared_scene_cancel,(),{
    (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.sceneCopy?.cancel();
});
#endif
static void schedule_th14_work(void(*callback)(void*),void* data,int delay){
#ifdef TH14_WEBGPU_BACKEND
    th14_webgpu_schedule(callback,data,delay);
#else
    th14_async_schedule(callback,data,delay);
#endif
}
#if defined(TH14_PHYSICAL_STAGE_MEMORY)
EM_ASYNC_JS(int,th14_stage_memory_open,(unsigned signature),{
    const runtime=Module.th14AsyncRuntime??Module.th14WebGpuRuntime;
    try{if(!runtime?.stageMemory)return 0;await runtime.stageMemory.open(signature);return 1;}
    catch(error){runtime?.fail(error);return 0;}
});
EM_JS(int,th14_stage_memory_prepare,(unsigned signature),{
    return (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.stageMemory?.prepare(wasmMemory,signature)?1:0;
});
EM_JS(int,th14_stage_memory_poll,(unsigned packet,unsigned stack,int playback),{
    return (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.stageMemory?.poll(packet,stack,playback!==0)??2;
});
EM_ASYNC_JS(void,th14_stage_memory_cancel_join,(),{
    await (Module.th14AsyncRuntime??Module.th14WebGpuRuntime)?.stageMemory?.cancelAndJoin();
});
#endif
#endif
EM_JS(void, th11_reset_browser_keyboard, (), { Module["resetBrowserKeyboard"]?.(); });
EM_JS(void, th11_browser_frame, (int ok,double milliseconds,unsigned ticks), { Module["onGameFrame"]?.(ok,milliseconds,ticks); });
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

namespace th11::sdl {
namespace {
#if PORT_TITLE == 14
constexpr const char* score_path="/save/scoreth14.dat";
constexpr const char* config_path="/save/th14.cfg";
constexpr const char* replay_prefix="th14";
#else
constexpr const char* score_path="/save/scoreth11.dat";
constexpr const char* config_path="/save/th11.cfg";
constexpr const char* replay_prefix="th11";
#endif
struct Application:StageResourceEffects
#if PORT_TITLE == 14
    ,DialogueEffects
#endif
{
    GraphicsDevice graphics;
    AudioDevice audio;
    FontDevice fonts{graphics};
    GameResources resources;
#if PORT_TITLE == 14
    static u32 reference_process_cookie(){
        // Windows supplies the original cookie before CRT/game entry. Use a
        // separate host entropy source; this is compatibility data, not RNG.
        std::random_device entropy;u32 value;
        do{value=entropy();}while(!NativeStackTh14::preinitialized_cookie(value));
        return value;
    }
    u32 process_cookie=reference_process_cookie();
    u32 initial_ticks=th14_clock_ticks();
    GameSession session{initial_ticks,false};
#else
    GameSession session;
#endif
    FrameStatistics frame_statistics;
    void reset_entry_frame_statistics(){
#if defined(TH14_CPP_INITIAL_LOADING)
        // Direct-start can reach this while the real loader is still live.
        // Adoption consumes its published reset without another Main store.
        if(initial_cpp_resources){
#if defined(TH14_CPP_STAGE_DECODE)
            if(initial_cpp_resources->control_ready())initial_statistics_reset=false;
#endif
            return;
        }
        if(initial_statistics_reset){initial_statistics_reset=false;return;}
#endif
        frame_statistics.reset_run();
    }

    AsciiText frame_text{session.resources.core.ascii};
    AnmRenderer renderer;
    std::vector<u8> archive;
    std::vector<u8> saved_score_state,saved_config_state;
    bool initialized=false;
#if PORT_TITLE == 14
    bool repeat_update=false;
    bool boot_scores_initialized=false;
#if defined(TH14_CPP_BOOT_WORKER)
    std::unique_ptr<BootCppResourcesTh14> boot_cpp_resources;
    std::unique_ptr<AudioCppBootTh14> audio_cpp_boot;
    std::unique_ptr<TitleCppResourcesTh14> title_cpp_resources;
#if defined(TH14_CPP_INITIAL_LOADING)
    std::unique_ptr<InitialCppResourcesTh14> initial_cpp_resources;
    bool initial_statistics_reset=false; // Main-only, set after loader join.
#endif
    bool title_cpp_signature_retired=false;
    bool cpp_menu_close_started=false,cpp_menu_close_done=false;
    bool boot_score_defaults_initialized=false,boot_common_resources_ready=false,boot_compositor_initialized=false;
#endif
    LoadingTh14& boot_loading=session.shared_loading;
    AnmResource boot_signature;
    u32 boot_signature_handle=0;
    bool boot_signature_pending=false;
    bool boot_signature_active=false;
    bool boot_signature_uploaded=false;
    bool boot_draw_callback_enabled=false;
    bool boot_ascii_uploaded=false;
    bool boot_ascii_pending=false;
#if defined(TH14_CPP_BOOT_WORKER)
    std::atomic<u32> boot_signature_draws{0};
#else
    u32 boot_signature_draws=0;
#endif
    struct ResourceUpload {AnmResource* resource;u16 file;u32 chunk=0;};
    std::vector<ResourceUpload> boot_uploads;
    u32 boot_upload_index=0;
    StageResources* boot_stage_owner=nullptr;
    enum class BootWork:u8 {Idle,Decode,Resume,Title,TitlePublish,InitialDecode,InitialResume,InitialPlayer,CoreDecode};
    struct BootWorkToken {Application* owner;u64 generation;BootWork work;u32 index;};
    u64 boot_worker_generation=0;
    BootWork boot_worker_work=BootWork::Idle;
    u32 boot_worker_index=0;
    bool boot_worker_active=false,boot_worker_suspended=false;
    bool boot_job_published=false,boot_title_ready=false,boot_worker_failed=false;
    bool initial_decode_complete=true;
    bool initial_upload_waiting=false;
    StageUploadsTh14 stage_uploads;
    StageWorkerWaitTh14 stage_worker_wait;
    struct StageWorkerWaitToken {Application* owner;u64 generation;};
    bool core_loading_done=false;
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
    u32 boot_core_decoded=0;
    StageDecodeTh14 stage_decode;
    struct StageDecodeToken {Application* owner;StageDecodeTh14::Token task;bool archive;};
#endif
#endif
    std::string error;
    // Lines whose plate had no sprite when they arrived, held until the plate's own walk binds one.
    std::vector<DialogueText> waiting_text;

    Application():renderer(graphics) {
        session.resources.effects=this;
#if PORT_TITLE == 14
        session.stage_load_clock=[](){return u64(SDL_GetTicksNS());};
#endif
    }
#if PORT_TITLE == 14
    ~Application(){
        cancel_boot_worker();session.shutdown();stage_uploads.cancel_all();
        // GameSession is declared before the host wait/upload members. Detach
        // only after shutdown, while all host members and controllers live.
        session.resources.effects=nullptr;
    }
#endif
#if PORT_TITLE == 14
    bool frame_statistics_sampled=false;
    u8 frame_skip_counter=0;
    bool present_this_update(){
        // Retail46a833 increments a byte, then compares its signed value.
        ++frame_skip_counter;
        const i32 count=frame_skip_counter<128?frame_skip_counter:i32(frame_skip_counter)-256;
        if(count<i32(u32(session.persistent_config().bytes[0x20])+1))return false;
        frame_skip_counter=0;return true;
    }
    double frame_statistics_time()const{
        // Retail42e4d0 samples46ac20, whose normal clock uses QPC precision.
        return double(SDL_GetTicksNS())/1000000000.;
    }
    void sample_frame_statistics(){
        frame_statistics_sampled=true;
#if defined(TH14_CPP_STAGE_DECODE)
        frame_statistics.sample_configured(frame_statistics_time(),session.animations.th14_control,session.persistent_config().bytes[0x20]);
#else
        frame_statistics.sample(frame_statistics_time(),session.state.phase==GameSessionPhase::stage&&session.battle&&session.battle->stage_active,u32(session.persistent_config().bytes[0x20])+1);
#endif
    }
    void reset_frame_window(){
        // Retail FPS starts with a zero window and includes initialization.
        // Host setup can pause/start before the first physical Main draw.
        if(!frame_statistics_sampled)return;
        frame_statistics.window_start=frame_statistics_time();frame_statistics.frames=0;
    }
#else
    void reset_frame_window(){frame_statistics.window_start=double(SDL_GetTicks())*.001;frame_statistics.frames=0;}
#endif
    bool read_file(const char* path,std::vector<u8>& data,u32 limit){
        SDL_IOStream* io=SDL_IOFromFile(path,"rb");if(!io)return false;const auto size=SDL_GetIOSize(io);
        if(size<0||u64(size)>limit){SDL_CloseIO(io);return false;}data.resize(size_t(size));
        const bool ok=SDL_ReadIO(io,data.data(),data.size())==data.size();SDL_CloseIO(io);return ok;
    }
    bool write_file(const std::string& path,const std::vector<u8>& data){
        const auto temporary=path+".tmp";SDL_IOStream* io=SDL_IOFromFile(temporary.c_str(),"wb");
        if(!io){error="Unable to create save file";return false;}
        const bool written=SDL_WriteIO(io,data.data(),data.size())==data.size();const bool closed=SDL_CloseIO(io);
        if(!written||!closed||std::rename(temporary.c_str(),path.c_str())!=0){error="Unable to finish save file";return false;}return true;
    }
#if PORT_TITLE == 14
    bool initialize_boot_scores(){
        if(boot_scores_initialized)return true;
        // Retail4442af follows the resource worker's file8/EffectManager.
        // Always construct defaults before reading an existing score file.
#if defined(TH14_CPP_BOOT_WORKER)
        if(!boot_score_defaults_initialized){session.initialize_score_defaults();boot_score_defaults_initialized=true;}
#else
        session.initialize_score_defaults();
#endif
        if(SDL_GetPathInfo(score_path,nullptr)){std::vector<u8> saved;if(!read_file(score_path,saved,4*1024*1024)||!session.load_scores(saved.data(),u32(saved.size()))){error=std::string("Invalid ")+score_path+": "+session.scores.error;return false;}}
        boot_scores_initialized=true;
        std::printf("[score14] score owner initialized after boot resources\n");
        return true;
    }
#endif
    bool save_scores(){
        if(!initialized)return true;
#if PORT_TITLE == 14
        // Closing an incomplete cold load must not persist zero-filled records.
        if(!boot_scores_initialized)return true;
#endif
        if(!session.title)session.scores.write_records(session.spell_records,session.clear_records);
        std::vector<u8> state;state.reserve(7*ScoreFile::character_size+ScoreFile::settings_size);
        for(const auto& character:session.scores.characters)state.insert(state.end(),character.begin(),character.end());
        state.insert(state.end(),session.scores.settings.begin(),session.scores.settings.end());
        if(state!=saved_score_state){std::vector<u8> data;if(!session.scores.save(data)){error="Unable to encode scores";return false;}
            if(!write_file(score_path,data))return false;saved_score_state=std::move(state);}
        const auto& cfg=session.persistent_config().bytes;std::vector<u8> config(cfg.begin(),cfg.end());
        if(config!=saved_config_state){if(!write_file(config_path,config))return false;saved_config_state=std::move(config);}
        return true;
    }
#if PORT_TITLE == 14
    bool write_stage_score(const std::vector<u8>& data)override{
        // This boundary is unconditional and synchronous, like456f60. The
        // caller continues after I/O failure; do not poison Application::error.
        // Invalidate the ordinary save cache even after a partial write.
        saved_score_state.clear();
        if(data.size()<24)return false;
        SDL_IOStream* io=SDL_IOFromFile(score_path,"wb");
        if(!io)return false;
        const bool header=SDL_WriteIO(io,data.data(),24)==24;
        if(!header){SDL_CloseIO(io);return false;}
        const bool payload=SDL_WriteIO(io,data.data()+24,data.size()-24)==data.size()-24;
        const bool closed=SDL_CloseIO(io);
        return header&&payload&&closed;
    }
#endif
    bool save_replay(u32 slot,const char* name){
        if(slot>99||!name||std::strlen(name)>8)return false;std::vector<u8> data;
        if(!session.save_replay(name,data,true,frame_statistics.slowdown())){error="No active recording to save";return false;}
        const bool touch=session.recording.uses_touch();char path[80];std::snprintf(path,sizeof(path),"/save/replay/%s_%.2u.%s",replay_prefix,slot,touch?"rpyx":"rpy");
        if(!write_file(path,data))return false;
        std::snprintf(path,sizeof(path),"/save/replay/%s_%.2u.%s",replay_prefix,slot,touch?"rpy":"rpyx");std::remove(path);return true;
    }
    bool stage_music_held()const{return kPortTitle!=14&&session.state.stage==6&&session.battle&&session.battle->frame<300&&!session.state.demo;}
    bool music_follows_pause()const{
#if PORT_TITLE == 14
        if(session.state.spell_practice_id>=0){
            // Retail448ef0 pauses music even for a forced replay exit.
            const auto* menu=session.pause_menu.get();
            if(!menu||!menu->replay||(menu->state!=2&&!menu->forced_exit))return false;
        }
#endif
        return !stage_music_held();
    }
    bool play_stage_music(u32 stage,bool activation=false){
        if(session.state.demo)return true;
#if PORT_TITLE == 14
        // Retail43697f skips direct-entry music in spell practice. Its ECL
        // -3/-1 requests remain responsible for selecting a track.
        if(activation&&session.state.spell_practice_id>=0)return true;
#endif
        if(!audio.music(stage_music(stage))){error=audio.error;return false;}
        audio.pause_music(kPortTitle!=14&&stage==6);return true;
    }
    bool pause(){if(!session.pause())return false;
#if PORT_TITLE == 14
        audio.pause_sounds(true);
#endif
        if(music_follows_pause())audio.pause_music(true);return true;}
    bool resume(){if(!session.resume())return false;
#if PORT_TITLE == 14
        audio.pause_sounds(false);
#endif
        if(music_follows_pause())audio.pause_music(false);
#if PORT_TITLE == 14
        return audio_events();
#else
        return true;
#endif
    }
#if PORT_TITLE == 14
    bool game_over_audio(bool replay){
        audio.pause_sounds(true);
        if(replay){if(music_follows_pause())audio.pause_music(true);}
        else if(session.state.spell_practice_id<0){
            audio.save_music();audio.pause_music(false);
            if(!audio.music(17)){error=audio.error;return false;}
        }
        return true;
    }
#endif
    bool audio_events(bool result_entry=false){
#if PORT_TITLE == 14 && defined(TH14_CPP_BOOT_WORKER)
        // Preserve startup music/events until the separate prepared owner is
        // adopted on Main; the loading title is not yet an active title.
        if(audio_cpp_boot&&!audio.statistics()[0])return true;
#endif
        if(session.ending){auto& ending=*session.ending;for(const auto sound:ending.sounds)audio.effects.enqueue(sound);
            if(ending.music_request>=0){audio.pause_music(false);if(!audio.music(ending.music_request)){error=audio.error;return false;}}
            if(ending.music_fade>=0)audio.fade_music(ending.music_fade);
        }
#if PORT_TITLE != 14
        if(session.pause_menu){for(const auto sound:session.pause_menu->sounds)audio.effects.enqueue(sound);session.pause_menu->sounds.clear();}
#else
        const auto menu_sounds=[this](){if(session.pause_menu){for(const auto sound:session.pause_menu->sounds)audio.effects.enqueue(sound);session.pause_menu->sounds.clear();}};
#endif
        if(session.title){
            for(const auto sound:session.title->sounds)audio.effects.enqueue(sound);
            if(session.title->music_pause)audio.pause_music(true);
            if(session.title->music_fade>=0)audio.fade_music(session.title->music_fade);
            if(session.title->music_request>=0){audio.pause_music(false);if(!audio.music(session.title->music_request)){error=audio.error;return false;}}
            if(session.title->volume_changed){audio.music_volume=session.config.music_volume();audio.effects.master_volume=session.config.sound_volume();audio.refresh_volume();}
            session.title->sounds.clear();
        }
#if PORT_TITLE == 14
        if(session.initial_battle_private()){menu_sounds();return true;}
#endif
        if(!session.battle){
#if PORT_TITLE == 14
            menu_sounds();
#endif
            return true;}
        for(const auto& event:session.battle->events){
            switch(event.kind){
            case BattleEventKind::Sound:
                if(event.positional)audio.effects.positioned(event.value,event.position.x);else audio.effects.enqueue(event.value);
                break;
            case BattleEventKind::StopSound:audio.effects.stop(event.value);break;
#if PORT_TITLE == 14
            // Retail4373b0 clears only sounds queued before the Player death callback.
            case BattleEventKind::GameOver:
                if(!game_over_audio(event.value!=0))return false;
                menu_sounds();break;
            case BattleEventKind::PauseResume:
                audio.pause_sounds(false);
                if(event.value){
                    if(!audio.restore_music()){error="Continue battle music restoration failed";return false;}
                    audio.discard_saved_music();
                }else if(music_follows_pause())audio.pause_music(false);
                break;
#endif
            case BattleEventKind::SoundPan:audio.effects.pan(event.value,event.extra);break;
            case BattleEventKind::StageMusic:if(!play_stage_music(event.value))return false;break;
            case BattleEventKind::BossMusic:
                if(!audio.music(stage_music(event.value,true))){error=audio.error;return false;}break;
            case BattleEventKind::MusicResume:audio.pause_music(false);break;
            case BattleEventKind::MusicFade:audio.fade_music(i32(event.position.x*60.f));break;
            case BattleEventKind::MusicStop:if(!audio.music(-1)){error=audio.error;return false;}break;
            default:break;
            }
        }
        session.battle->events.clear();
#if PORT_TITLE == 14
        // A completed result enters after the battle callbacks, without a death marker.
        if(result_entry&&!game_over_audio(false))return false;
        menu_sounds();
#endif
        return true;
    }
#if PORT_TITLE == 14
    DialogueEffects* dialogue_text_device()override{return this;}
    bool dialogue_text(const DialogueText& request)override{
        renderer.flush();
        auto* vm=session.animations.find(request.animation);
        if(!vm){error="TH14 dialogue text animation missing";return false;}
        if(!fonts.text(*vm,request)){error=fonts.error;return false;}
        return true;
    }
#endif
    bool text_events(){
        if(session.ending){renderer.flush();for(const auto& request:session.ending->text_requests){auto* vm=session.animations.find(request.animation);if(vm&&!fonts.text(*vm,request)){error=fonts.error;return false;}}session.ending->text_requests.clear();}
        if(session.title){renderer.flush();for(const auto& request:session.title->text_requests){auto* vm=session.animations.find(request.animation);if(vm&&!fonts.text(*vm,request)){error=fonts.error;return false;}}session.title->text_requests.clear();}
#if PORT_TITLE == 14
        if(session.initial_battle_private())return true;
#endif
        if(!session.battle)return true;
        renderer.flush();
        // A dialogue plate binds its own sprite with its own walk, and the generation's line arrives
        // before that: th16 and th17 write their first line into a plate standing at +0xac, due at
        // plate time 8, while th12's plate opens with its sprite command at time 0. A request whose
        // animation has no sprite yet is kept for the frame the plate reaches it, and dropped when
        // the animation is gone, so a line never outlives the plate that carries it.
        std::vector<DialogueText> pending;
        for(auto it=waiting_text.begin();it!=waiting_text.end();){
            if(!session.animations.find(it->animation)){it=waiting_text.erase(it);continue;}
            pending.push_back(*it);++it;
        }
        for(const auto& request:session.battle->dialogue_text_requests){
            // A new line for the same plate replaces the one still waiting.
            pending.erase(std::remove_if(pending.begin(),pending.end(),
                [&](const DialogueText& held){return held.animation==request.animation;}),pending.end());
            pending.push_back(request);
        }
        waiting_text.clear();
        for(const auto& request:pending){
            auto* vm=session.animations.find(request.animation);
            if(!vm)continue;
            if(!fonts.text(*vm,request)){
                if(fonts.waiting){waiting_text.push_back(request);continue;}
                error=fonts.error;return false;
            }
        }
        session.battle->dialogue_text_requests.clear();return true;
    }
    bool prepare(StageResources& stage)override{
        renderer.flush();
        graphics.error.clear();
#if PORT_TITLE == 14
        // First entry waits before battle construction; later entries keep
        // the existing world until the incoming resources are ready.
        {
            cancel_boot_worker();
            boot_uploads.clear();boot_upload_index=0;
            boot_stage_owner=&stage;
            // Like later Timeline loads, queue only the ECL ANIM resources.
            // stage.enemies duplicates one of those files and has no upload.
            // Retail44c4bf requests only the selected player at file9 before
            // the initial background/HUD/ECL uploads.
            // 44c4bf waits for file9 before435e1d creates the options.
            // The same worker then reads scene, HUD and each ECL ANIM only
            // after the preceding upload returns; those reads are not boot IO.
            initial_decode_complete=false;
#if defined(TH14_CPP_INITIAL_LOADING)
            initial_statistics_reset=false;
            initial_cpp_resources=std::make_unique<InitialCppResourcesTh14>(session,resources,stage,frame_statistics,audio.music_requests());
            initial_upload_waiting=true;
#else
            dispatch_initial_stage_work(BootWork::InitialPlayer,u32(session.state.character*2+session.state.subtype),0);
#endif
            // The initial controller's44c4bf resource requests do not
            // enter420d20. Its loading family belonged to cold startup;
            // later-stage workers create their own family separately.
            std::printf("[loading14] stage entry; selected player CPU resource pending\n");
            return true;
        }
#endif
        for(auto* file:{&stage.background,&stage.logo,&stage.enemies})if(!graphics.preload(*file)){error=graphics.error;return false;}
#if PORT_TITLE == 14 || PORT_TITLE == 16
        for(auto& file:stage.ecl_animations)if(!graphics.preload(file)){error=graphics.error;return false;}
#endif
        return true;
    }
#if PORT_TITLE == 14
    bool core_ready()const override{return core_loading_done;}
    bool defer_initial_stage_decode()const override{return true;}
#if defined(TH14_CPP_BOOT_WORKER)
    void read_cpp_boot_publications(){
        if(!boot_cpp_resources)return;
        boot_draw_callback_enabled=boot_cpp_resources->publication(0)!=0;
        boot_signature_uploaded=boot_cpp_resources->publication(1)!=0;
        boot_ascii_uploaded=boot_cpp_resources->publication(2)!=0;
    }
    bool initialize_cpp_boot_compositor(){
        if(boot_compositor_initialized||!boot_cpp_resources||!boot_cpp_resources->publication(3))return true;
        if(!session.initialize_compositor()){
            error="TH14 common-resource compositor: "+session.compositor.last_error;return false;
        }
        session.reset_compositor_cameras(false);boot_compositor_initialized=true;
        std::printf("[loading14] Main compositor ready after common resources\n");return true;
    }
    void publish_cpp_title_loading(){
        if(title_cpp_resources)title_cpp_resources->publish_loading_state(boot_signature_active,boot_signature_draws,
            __atomic_load_n(&session.stage_load_host_flags,__ATOMIC_ACQUIRE));
    }
    void cancel_cpp_title_resources(){
        if(!title_cpp_resources)return;
        title_cpp_resources->cancel_and_join();
        if(title_cpp_resources->publication(0)&&!boot_title_ready)session.resources.complete_cpp_title_resources();
        title_cpp_resources.reset();
    }
    bool poll_cpp_title_resources(){
        if(!title_cpp_resources)return true;
        publish_cpp_title_loading();
        const auto status=title_cpp_resources->poll();
        // Retail459607 enables Title6 before the worker returns. Acquire that
        // publication independently of the thread's terminal/return state.
        const bool ready=title_cpp_resources->publication(2)!=0;
        if(title_cpp_resources->publication(1)&&!title_cpp_signature_retired){
            session.animations.retire_resource(boot_signature);title_cpp_signature_retired=true;
        }
        if(ready&&!boot_title_ready){
            if(boot_upload_index!=8||(boot_signature_active&&!title_cpp_signature_retired)||!session.resources.complete_cpp_title_resources()){
                error="TH14 C++ title owner completion mismatch";return false;
            }
            boot_title_ready=true;
            std::printf("[loading14] real C++ title worker ready at draw%u\n",u32(boot_signature_draws));
        }
        if(status==0)return true;
        title_cpp_resources->join();
        if(status!=1){error="TH14 C++ title decode: "+title_cpp_resources->last_error();title_cpp_resources.reset();return false;}
        if(!boot_title_ready){error="TH14 C++ title worker returned without Title enable";title_cpp_resources.reset();return false;}
        // Main may already have activated Title and cleared the upload list.
        // Joining the retained callback must not adopt those owners again.
        title_cpp_resources.reset();return true;
    }
#if defined(TH14_CPP_INITIAL_LOADING)
    bool initial_cpp_worker_enabled()const override{return true;}
    bool begin_initial_cpp_loading()override{
        if(!initial_cpp_resources||!initial_cpp_resources->begin()){
            error="TH14 initial C++ resource worker could not start";graphics.error=error;return false;
        }
        std::printf("[loading14] real C++ initial Player/Scene resource worker started\n");return true;
    }
    bool pump_cpp_initial_upload_chunk(){
        if(!initial_cpp_resources)return true;
        AnmResource* resource=nullptr;u16 file=0;
        if(initial_cpp_resources->pending(resource,file)){
            // Close bits let47d0f0 return with earlier offers unacknowledged.
            // Main47d720 still rescans ascending slots on every chunk: the
            // completed count cannot identify a now-lower pending file.
            auto found=std::find_if(boot_uploads.begin(),boot_uploads.end(),[&](const auto& job){return job.resource==resource&&job.file==file;});
            if(found==boot_uploads.end()){boot_uploads.push_back({resource,file});found=boot_uploads.end()-1;}
            auto& job=*found;
            if(!graphics.preload_chunk(*resource,job.chunk)){error=graphics.error;return false;}
            if(++job.chunk>=resource->textures.size()){
                if(!session.animations.prepare_resource(*resource,file)){error=session.animations.last_error_text();return false;}
                if(!initial_cpp_resources->complete(*resource,file)){error="TH14 initial C++ file ACK rejected";return false;}
                std::printf("[loading14] initial C++ file %u ready after %u chunk callbacks\n",u32(file),job.chunk);
                ++boot_upload_index;
            }
        }
        return true;
    }
    bool poll_cpp_initial_completion(bool reap_terminal=true){
        if(!initial_cpp_resources)return true;
        const auto status=initial_cpp_resources->poll();
#if defined(TH14_CPP_STAGE_DECODE)
        if(!initial_cpp_resources->control_ready()){
            if(status==ReplayRngRestoreResultTh14::Waiting)return true;
            initial_cpp_resources->join();
            error="TH14 initial worker returned without Control enable: "+initial_cpp_resources->error_after_join();return false;
        }
        if(status!=ReplayRngRestoreResultTh14::Waiting){
            if(status!=ReplayRngRestoreResultTh14::Ready){initial_cpp_resources->join();error="TH14 initial worker failed after data publication";return false;}
            // Reaping remains a Main resource-poll job. The Control11 data
            // handoff after Overlay8 must not add a thread wait to this pass.
            if(reap_terminal)initial_cpp_resources->join();
        }
#else
        if(status==ReplayRngRestoreResultTh14::Waiting)return true;
        initial_cpp_resources->join();
#endif
        // Callback return publishes its final offers, but does not ACK any
        // unuploaded template. Drain every retained slot before Main activates.
        // This guard retains the existing incomplete-native-tail boundary.
#if defined(TH14_CPP_STAGE_DECODE)
        AnmResource* pending_resource=nullptr;u16 pending_file=0;
        if(initial_cpp_resources->pending(pending_resource,pending_file)){
            error="TH14 Control enabled before final file completion";return false;
        }
#else
        if(status==ReplayRngRestoreResultTh14::Ready){
            AnmResource* pending_resource=nullptr;u16 pending_file=0;
            if(initial_cpp_resources->pending(pending_resource,pending_file))return true;
        }
        if(status!=ReplayRngRestoreResultTh14::Ready){
            error="TH14 initial C++ resource worker: "+initial_cpp_resources->error_after_join();return false;
        }
#endif
        // Control enable publishes completed loading data. Retain the actor
        // until its independent return/reap boundary; activation needs no join.
        initial_statistics_reset=true;
        initial_decode_complete=true;initial_upload_waiting=false;return true;
    }
    bool pump_cpp_initial_upload(){
        if(!initial_cpp_resources)return true;
        if(!pump_cpp_initial_upload_chunk())return false;
        return poll_cpp_initial_completion();
    }
#endif
    void cancel_cpp_boot_resources(){
        if(!boot_cpp_resources)return;
        boot_cpp_resources->cancel_and_join();read_cpp_boot_publications();
        if(boot_cpp_resources->scores_initialized_after_join())boot_score_defaults_initialized=true;
        if(boot_cpp_resources->publication(3)&&session.resources.complete_cpp_core_bootstrap())boot_common_resources_ready=true;
        boot_cpp_resources.reset();
    }
    bool pump_cpp_boot_upload(){
        if(!boot_cpp_resources&&!title_cpp_resources)return true;
        AnmResource* offered=nullptr;u16 file=0;
        const bool pending=boot_cpp_resources?boot_cpp_resources->pending(offered,file):title_cpp_resources->pending(offered,file);
        if(!pending)return true;
        // Retail47d720 selects the smallest live slot again each chunk. A
        // completed-count cursor cannot identify a partially uploaded owner.
        auto found=std::find_if(boot_uploads.begin(),boot_uploads.end(),[&](const auto& job){return job.resource==offered&&job.file==file;});
        if(found==boot_uploads.end()){error="TH14 C++ bootstrap file owner changed";return false;}
        auto& job=*found;
        if(!graphics.preload_chunk(*job.resource,job.chunk)){error=graphics.error;return false;}
        const bool resource_ready=++job.chunk>=job.resource->textures.size();
        if(!resource_ready)return true;
        if(file==2)session.animations.fallback_sprites=job.resource;
        if(!session.animations.prepare_resource(*job.resource,file)){error=session.animations.last_error_text();return false;}
        std::printf("[loading14] file %u ready after %u chunk callbacks\n",u32(file),job.chunk);
        const bool accepted=boot_cpp_resources?boot_cpp_resources->complete(*job.resource,file):title_cpp_resources->complete(*job.resource,file);
        if(!accepted){error="TH14 C++ bootstrap/title file ACK rejected";return false;}
        // This is a completed-file count. The chosen vector entry retains
        // its own chunk progress when a lower file number arrives next.
        ++boot_upload_index;return true;
    }
    bool pump_cpp_stage_close_upload(){
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE) && !(defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER))
        // Retail47d720 services a published resource independently of the
        // controller's phase polling. Admit the worker's immutable offer
        // before this pump, retaining its actual stage owner and phase.
        StageResources* upload_owner=nullptr;AnmResource* upload_resource=nullptr;u16 upload_file=0;
        StageResourceEffects::Phase upload_phase;
        const auto offered=session.cpp_stage_upload_offer(upload_owner,upload_phase,upload_resource,upload_file);
        if(offered==StageResourceEffects::DecodeResult::Failed||
           (offered==StageResourceEffects::DecodeResult::Ready&&!stage_uploads.begin_file(*upload_owner,upload_phase,*upload_resource,upload_file))){
            error="TH14 worker upload offer owner changed";return false;
        }
#endif
        if(stage_uploads.pending()){
            renderer.flush();graphics.error.clear();
            if(!stage_uploads.advance([&](const auto& job){return graphics.preload_chunk(*job.resource,job.chunk);},[&](const auto& job){
                if(session.animations.prepare_resource(*job.resource,job.file)){
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE) && !(defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER))
                    if(!session.cpp_stage_complete_file(*job.resource,job.file)){graphics.error="TH14 ANM file completion owner changed";return false;}
#endif
                    return true;
                }
                graphics.error=session.animations.last_error_text();return false;
            })){
                error=graphics.error.empty()?"TH14 staged resource upload failed":graphics.error;return false;
            }
            // A successful stage upload continues through the normal session,
            // Player/HUD/old Stage callbacks and draw below, without a second
            // ANM update or a loading-frame return.
        }
        return true;
    }
    enum class CppMenuCloseState {Inactive,Waiting,Closed,Failed};
    CppMenuCloseState poll_cpp_menu_close(){
        if(cpp_menu_close_done)return CppMenuCloseState::Closed;
        const bool first=!cpp_menu_close_started;
        if(first){
            // Native444890 requests3 only for masked80, with no stage loader.
            // The stage number stays nonzero during ordinary gameplay;
            // Supervisor+99c instead guards a loading callback's lifetime.
            // Retain owners while their actual callback is still running.
            const auto flags=__atomic_load_n(&session.stage_load_host_flags,__ATOMIC_ACQUIRE);
            if((flags&0x180u)!=0x80u)return CppMenuCloseState::Inactive;
            bool returned_stage_loader=false;
#if defined(TH14_CPP_STAGE_WORKER)
            // Retail435f6f clears Supervisor+99c even when the loader returns
            // -1. A retained owner is not a running callback after that point.
            // Both Ready and Failed have left the callback body; join remains
            // inside retirement. Idle/Running must retain their owner guards.
#if defined(TH14_CPP_STAGE_DECODE)
            // Native Main4448a5 observes busy0 before the copy-interrupt
            // and thread-return tail. Retirement below still cancels/joins.
            returned_stage_loader=session.has_cpp_stage_worker()&&session.stage_loading_callback_allows_close();
#else
            returned_stage_loader=session.has_cpp_stage_worker()&&
                session.stage_cpp_worker_result()!=ReplayRngRestoreResultTh14::Waiting;
#endif
            if(session.has_cpp_stage_worker()&&!returned_stage_loader)
                return CppMenuCloseState::Inactive;
#endif
            bool returned_initial_loader=false;
#if defined(TH14_CPP_INITIAL_LOADING)
            // Callback return is independent of Main resource adoption and
            // battle activation. Both Ready and Failed have left the body;
            // retirement below joins before destroying any referenced owner.
#if defined(TH14_CPP_STAGE_DECODE)
            returned_initial_loader=initial_cpp_resources&&initial_cpp_resources->loading_callback_allows_close();
#else
            returned_initial_loader=initial_cpp_resources&&initial_cpp_resources->has_started()&&
                initial_cpp_resources->poll()!=ReplayRngRestoreResultTh14::Waiting;
#endif
            if(initial_cpp_resources&&!returned_initial_loader)return CppMenuCloseState::Inactive;
#endif
            if((boot_stage_owner||initial_upload_waiting||session.resources.loading_bundle()
                ||(session.battle&&session.battle->pending_stage))&&!returned_stage_loader&&!returned_initial_loader)
                return CppMenuCloseState::Inactive;
            cpp_menu_close_started=true;
        }
        // SDL adoption belongs to Main. The resource worker may be waiting
        // for it after its actual audio-loader join, even during shutdown.
        if(audio_cpp_boot)audio_cpp_boot->poll_main(audio);
        if(first){
            if(!pump_cpp_boot_upload()||!pump_cpp_stage_close_upload())return CppMenuCloseState::Failed;
#if defined(TH14_CPP_INITIAL_LOADING)
            // Ordinary Main47d720 services one pending resource chunk before
            // transition3. This does not drain/adopt or activate the battle.
            if(!pump_cpp_initial_upload_chunk())return CppMenuCloseState::Failed;
#endif
        }
        // Retail still pumps one resource chunk before its transition3.
        // Subsequent hosted opportunities service only the required audio
        // handshake; never manufacture more upload ACKs or advance ANMs.
        if((boot_cpp_resources&&boot_cpp_resources->poll()==0)
            ||(title_cpp_resources&&title_cpp_resources->poll()==0))return CppMenuCloseState::Waiting;
        if(title_cpp_resources){
            title_cpp_resources->join();
            if(title_cpp_resources->publication(1)&&!title_cpp_signature_retired){
                session.animations.retire_resource(boot_signature);title_cpp_signature_retired=true;
            }
            title_cpp_resources.reset();
        }
        if(boot_cpp_resources){
            boot_cpp_resources->join();
            if(boot_cpp_resources->scores_initialized_after_join())boot_score_defaults_initialized=true;
            boot_cpp_resources.reset();
        }
        // Retire the audio owner only after its sole resource-worker joiner.
        // Do not call complete_cpp_* on CPU-ready but unuploaded resources.
        cancel_boot_worker();session.stop_stage_workers();
        // No queued ANM reference may survive the joined loading owner.
        stage_uploads.cancel_all();boot_stage_owner=nullptr;audio.close();
        boot_signature_active=false;boot_signature_pending=false;
        session.state.phase=GameSessionPhase::finished;cpp_menu_close_done=true;
        return CppMenuCloseState::Closed;
    }
    bool poll_cpp_boot_resources(){
        if(audio_cpp_boot)audio_cpp_boot->poll_main(audio);
        if(!boot_cpp_resources)return true;
        read_cpp_boot_publications();
        if(!initialize_cpp_boot_compositor())return false;
        const auto status=boot_cpp_resources->poll();if(status==0)return true;
        boot_cpp_resources->join();read_cpp_boot_publications();
        if(boot_cpp_resources->scores_initialized_after_join())boot_score_defaults_initialized=true;
        if(status!=1){error="TH14 C++ bootstrap decode: "+boot_cpp_resources->last_error();boot_cpp_resources.reset();boot_worker_failed=true;return false;}
        if(boot_upload_index!=6||!session.resources.complete_cpp_core_bootstrap()){
            error="TH14 C++ bootstrap owner completion mismatch";boot_cpp_resources.reset();return false;
        }
        boot_common_resources_ready=true;
        if(!initialize_cpp_boot_compositor()||!initialize_boot_scores())return false;
        boot_cpp_resources.reset();
        audio_cpp_boot.reset();
        title_cpp_signature_retired=false;
        auto& core=session.resources.core;
#if defined(TH14_CPP_STAGE_DECODE)
        title_cpp_resources=std::make_unique<TitleCppResourcesTh14>(std::array<AnmResource*,2>{&core.title,&core.title_variant},&session.stage_load_host_flags,&session.animations.th14_effect_stop,&boot_signature_draws);
#else
        title_cpp_resources=std::make_unique<TitleCppResourcesTh14>(std::array<AnmResource*,2>{&core.title,&core.title_variant},&session.stage_load_host_flags,&boot_signature_draws);
#endif
        publish_cpp_title_loading();
        if(!title_cpp_resources->begin(archive.data(),u32(archive.size()))){error="Unable to start TH14 C++ title worker";return false;}
        std::printf("[loading14] real C++ title worker started\n");return true;
    }
#endif
    void cancel_boot_worker(
#if defined(TH14_CPP_INITIAL_LOADING) && defined(TH14_CPP_STAGE_DECODE)
        bool retain_initial_tail=false
#if defined(TH14_CPP_BOOT_WORKER)
        ,
#endif
#endif
#if defined(TH14_CPP_BOOT_WORKER)
        bool retain_title_tail=false
#endif
    ){
#if defined(TH14_CPP_INITIAL_LOADING)
        if(initial_cpp_resources
#if defined(TH14_CPP_STAGE_DECODE)
           &&!retain_initial_tail
#endif
        ){initial_cpp_resources->cancel_and_join();initial_cpp_resources.reset();}
#endif
#if defined(TH14_CPP_BOOT_WORKER)
        if(audio_cpp_boot)audio_cpp_boot->signal_cancel();
        if(!retain_title_tail)cancel_cpp_title_resources();
        cancel_cpp_boot_resources();
        audio_cpp_boot.reset();
#endif
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        th14_webgpu_archive_cancel();
#endif
        ++boot_worker_generation;boot_worker_active=boot_worker_suspended=false;
#if defined(TH14_CPP_BOOT_WORKER)
        boot_job_published=boot_worker_failed=false;
        if(!retain_title_tail)boot_title_ready=false;
#else
        boot_job_published=boot_title_ready=boot_worker_failed=false;
#endif
        boot_worker_work=BootWork::Idle;
        initial_upload_waiting=false;initial_decode_complete=true;
    }
    void suspend_boot_worker(){
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        if(stage_decode.pending()){stage_decode.suspend();th14_webgpu_archive_cancel();}
#endif
        if(boot_worker_active||initial_upload_waiting){
            ++boot_worker_generation;boot_worker_suspended=true;
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
            th14_webgpu_archive_cancel();
#endif
        }
    }
    void resume_boot_worker(){
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        stage_decode.resume();dispatch_stage_decode();
#endif
        if(!boot_worker_suspended)return;
        boot_worker_suspended=false;
        if(boot_worker_active&&boot_worker_work!=BootWork::Idle)dispatch_boot_work(boot_worker_work,boot_worker_index,0);
        else if(initial_upload_waiting)dispatch_initial_stage_work(boot_worker_work,boot_worker_index,0);
    }
    void dispatch_initial_stage_work(BootWork work,u32 index,int delay){
        initial_upload_waiting=true;boot_worker_work=work;boot_worker_index=index;
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        if((work==BootWork::InitialDecode||work==BootWork::InitialPlayer)&&boot_stage_owner){
            const auto& stage=*boot_stage_owner;std::vector<std::string> names;char name[64];
            auto add=[&](const char* format){std::snprintf(name,sizeof name,format,stage.stage_number);names.emplace_back(name);};
            if(work==BootWork::InitialPlayer)names.emplace_back(player_file_name(u32(session.state.character),u32(session.state.subtype)));
            else if(index==0){add(stage_files.scene);add(stage_files.background);}
            else if(index==1){
                add(stage_files.logo);
                stage_dialogue_name(name,sizeof name,stage.stage_number,u32(session.state.character*2+session.state.subtype));
                names.emplace_back(name);
            }else if(index==2){
                add(stage_files.timeline);
                for(const char* format:{stage_files.boss,stage_files.midboss}){
                    std::snprintf(name,sizeof name,format,stage.stage_number);
                    if(resources.has(name))names.emplace_back(name);
                }
                if(resources.has("default.ecl"))names.emplace_back("default.ecl");
            }else if(index>=3&&index<=7){
                u32 slot=index-3;
                while(slot<stage.ecl_animations.size()&&stage.ecl_animation_names[slot].empty())++slot;
                if(slot<stage.ecl_animations.size())names.push_back(stage.ecl_animation_names[slot]);
                else{
                    std::snprintf(name,sizeof name,stage_files.enemies,stage.stage_number);
                    if(std::find(stage.ecl_animation_names.begin(),stage.ecl_animation_names.end(),name)==stage.ecl_animation_names.end())names.emplace_back(name);
                }
            }
            if(!names.empty()){
                std::vector<const char*> pointers;for(const auto& name:names)pointers.push_back(name.c_str());
                // The import copies names before returning. It never holds an
                // Asyncify stack open while the independent CPU worker runs.
                th14_webgpu_archive_schedule(run_initial_stage_work,new BootWorkToken{this,boot_worker_generation,work,index},pointers.data(),u32(pointers.size()));
                return;
            }
        }
#endif
        schedule_th14_work(run_initial_stage_work,new BootWorkToken{this,boot_worker_generation,work,index},delay);
    }
    void dispatch_initial_upload_resume(u32 index){
        dispatch_initial_stage_work(BootWork::InitialResume,index,1);
    }
    static void run_initial_stage_work(void* raw){
        std::unique_ptr<BootWorkToken> token(static_cast<BootWorkToken*>(raw));auto& app=*token->owner;
        if(token->generation!=app.boot_worker_generation||app.boot_worker_suspended||!app.boot_stage_owner)return;
        if(!app.initial_upload_waiting||app.boot_worker_work!=token->work||app.boot_worker_index!=token->index){
            app.error="TH14 initial worker owner changed";app.boot_worker_failed=true;return;
        }
        app.boot_worker_work=BootWork::Idle;
        auto& stage=*app.boot_stage_owner;
        if(token->work==BootWork::InitialPlayer){
            if(token->index!=u32(app.session.state.character*2+app.session.state.subtype)||!app.boot_uploads.empty()){
                app.error="TH14 initial player resource owner changed";app.boot_worker_failed=true;return;
            }
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
            if(!th14_webgpu_archive_status()){
                app.error="TH14 background player resource decode failed";app.boot_worker_failed=true;return;
            }
#endif
            if(!app.session.resources.load_selected_player(u32(app.session.state.character),u32(app.session.state.subtype))){
                app.error=app.session.resources.error;app.boot_worker_failed=true;return;
            }
            app.boot_uploads.push_back({&app.session.resources.core.players[app.session.state.character],9});
            app.initial_upload_waiting=false;return;
        }
        if(token->work==BootWork::InitialResume){
            if(token->index>=app.boot_uploads.size()||app.boot_upload_index!=token->index+1){
                app.error="TH14 initial upload owner changed";app.boot_worker_failed=true;return;
            }
            const auto file=app.boot_uploads[token->index].file;
            if(file==9&&!app.session.prepare_initial_player()){
                app.error=app.session.error;app.boot_worker_failed=true;return;
            }
            // The background wait returns into the scene camera copy, before
            // HUD and Timeline decoding. No object VM starts at this point.
            if(file==u16((stage.stage_number&1)+3)&&!app.session.prepare_initial_scene(true)){
                app.error=app.session.error;app.boot_worker_failed=true;return;
            }
            const u32 step=file==9?0:file==u16((stage.stage_number&1)+3)?1:file==6?2:u32(file-10)+4;
            std::printf("[loading14] initial worker resumed after file%u\n",u32(file));
            app.dispatch_initial_stage_work(BootWork::InitialDecode,step,0);return;
        }
        if(token->work!=BootWork::InitialDecode){app.error="Invalid TH14 initial worker task";app.boot_worker_failed=true;return;}
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        if(!th14_webgpu_archive_status()){
            app.error="TH14 background archive decode failed";app.boot_worker_failed=true;return;
        }
#endif
        auto fail=[&](){app.error="TH14 initial resource "+last_resource+": "+app.resources.error();app.boot_worker_failed=true;};
        u32 step=token->index;
        if(step==0){
            // Retain the same private owner across scene CPU decode and upload.
            if(!app.session.prepare_initial_scene(false)){app.error=app.session.error;app.boot_worker_failed=true;return;}
            if(!app.resources.load_stage_scene(stage.stage_number,stage)){fail();return;}
            app.boot_uploads.push_back({&stage.background,u16((stage.stage_number&1)+3)});
        }else if(step==1){
            const u32 selection=u32(app.session.state.character*2+app.session.state.subtype);
            if(!app.resources.load_stage_hud(stage.stage_number,stage,selection)){fail();return;}
            app.boot_uploads.push_back({&stage.logo,6});
        }else{
            if(step==2){
                if(!app.resources.load_stage_timeline_plan(stage.stage_number,stage)){fail();return;}
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
                // ANIM names become available only after parsing ECL. Decode
                // that next dependency in the worker, without a fixed frame wait.
                app.dispatch_initial_stage_work(BootWork::InitialDecode,3,0);return;
#else
                step=3;
#endif
            }
            if(step<3||step>7){app.error="Invalid TH14 initial decode step";app.boot_worker_failed=true;return;}
            u32 slot=step-3;
            while(slot<stage.ecl_animations.size()&&stage.ecl_animation_names[slot].empty())++slot;
            if(slot<stage.ecl_animations.size()){
                if(!app.resources.load_stage_animation(stage,slot)){fail();return;}
                app.boot_uploads.push_back({&stage.ecl_animations[slot],u16(10+slot)});
            }else{
                if(!app.resources.complete_stage_timeline(stage.stage_number,stage)){fail();return;}
                app.initial_decode_complete=true;
            }
        }
        app.initial_upload_waiting=false;
    }
    void dispatch_boot_work(BootWork work,u32 index,int delay){
        boot_worker_work=work;boot_worker_index=index;
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        if(work==BootWork::CoreDecode&&index<4){
            // These remaining core jobs used to parse synchronously when the
            // title was enabled. Keep their order and final template owner,
            // but decode the ANM/ECL and decompress SHT in the CPU worker.
            std::vector<std::string> names;
            if(index==0)names.emplace_back("enemy.anm");
            else{
                const u32 character=index-1;names.emplace_back(player_file_name(character));
                for(u32 loadout=0;loadout<shot_loadout_count;++loadout)
                    names.emplace_back(shot_file_name(shot_file_slot(character,loadout)));
                if(index==3)names.emplace_back("default.ecl");
            }
            std::vector<const char*> pointers;for(const auto& name:names)pointers.push_back(name.c_str());
            th14_webgpu_archive_schedule(run_boot_work,new BootWorkToken{this,boot_worker_generation,work,index},pointers.data(),u32(pointers.size()));
            return;
        }
        if(work==BootWork::Decode&&index==0){
            // Bootstrap all three before the first upload, preserving Sig,
            // ASCII and text template order without holding an Asyncify stack
            // open while their CPU decoding runs in the independent worker.
            const char* names[]={"sig.anm","ascii.anm","text.anm"};
            th14_webgpu_archive_schedule(run_boot_work,new BootWorkToken{this,boot_worker_generation,work,index},names,3);
            return;
        }
        // Bootstrap already owns the first three files. Later CPU decodes may
        // run while loading animations update on the main thread.
        constexpr const char* names[]={nullptr,nullptr,nullptr,"front.anm","bullet.anm","effect.anm","title.anm","title_v.anm"};
        if(work==BootWork::Decode&&index<sizeof(names)/sizeof(*names)&&names[index]){
            const char* name=names[index];
            th14_webgpu_archive_schedule(run_boot_work,new BootWorkToken{this,boot_worker_generation,work,index},&name,1);
            return;
        }
#endif
        schedule_th14_work(run_boot_work,new BootWorkToken{this,boot_worker_generation,work,index},delay);
    }
    static void run_boot_work(void* raw){
        std::unique_ptr<BootWorkToken> token(static_cast<BootWorkToken*>(raw));
        auto& app=*token->owner;
        if(token->generation!=app.boot_worker_generation||!app.boot_worker_active||app.boot_worker_suspended)return;
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        if(token->work==BootWork::CoreDecode){
            if(token->index>=4||token->index!=app.boot_core_decoded||token->index!=app.boot_worker_index
                ||app.boot_worker_work!=BootWork::CoreDecode||app.boot_upload_index!=app.boot_uploads.size()
                ||app.boot_stage_owner||app.core_loading_done){
                app.error="TH14 remaining core resource owner changed";app.boot_worker_failed=true;return;
            }
            app.boot_worker_work=BootWork::Idle;
            if(!th14_webgpu_archive_status()){
                app.error="TH14 background core resource decode failed";app.boot_worker_failed=true;return;
            }
            constexpr u32 jobs[]={3,6,7,8};
            if(!app.session.resources.load_core_job(jobs[token->index])){
                app.error=app.session.resources.error;app.boot_worker_failed=true;return;
            }
            ++app.boot_core_decoded;
            if(app.boot_core_decoded<4)app.dispatch_boot_work(BootWork::CoreDecode,app.boot_core_decoded,0);
            else app.dispatch_boot_work(BootWork::Title,0,0);
            return;
        }
#endif
        app.boot_worker_work=BootWork::Idle;
        if(token->work==BootWork::Title){
            // Retail4595be polls Sleep16 after the resource worker returns.
            if(app.boot_signature_draws<180){app.dispatch_boot_work(BootWork::Title,0,16);return;}
            // 4595e1 retires Sig before 459615 enables Title. A main pass
            // between these worker boundaries can reuse its animation slots.
            app.session.animations.retire_resource(app.boot_signature);
            app.dispatch_boot_work(BootWork::TitlePublish,0,0);return;
        }
        if(token->work==BootWork::TitlePublish){
            app.boot_title_ready=true;
            std::printf("[loading14] title worker ready at draw%u\n",u32(app.boot_signature_draws));return;
        }
        if(token->work==BootWork::Decode){
            if(token->index!=app.boot_upload_index||token->index>=app.boot_uploads.size()||app.boot_job_published){
                app.error="TH14 boot decode owner changed";app.boot_worker_failed=true;return;
            }
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
            if(token->index==0){
                if(!th14_webgpu_archive_status()){
                    app.error="TH14 background bootstrap resource decode failed";app.boot_worker_failed=true;return;
                }
                if(!app.resources.open_anm("sig.anm",app.boot_signature)){
                    app.error="sig ANM: "+app.boot_signature.detail;app.boot_worker_failed=true;return;
                }
                if(!app.session.animations.prepare_resource(app.boot_signature,1)){
                    app.error=app.session.animations.last_error_text();app.boot_worker_failed=true;return;
                }
                if(!app.session.resources.load_core_bootstrap()){
                    app.error=app.session.resources.error;app.boot_worker_failed=true;return;
                }
            }
#endif
            if(token->index>=3){
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
                if(!th14_webgpu_archive_status()){
                    app.error="TH14 background boot resource decode failed";app.boot_worker_failed=true;return;
                }
#endif
                constexpr u32 jobs[]={0,1,2,4,5};
                if(!app.session.resources.load_core_job(jobs[token->index-3])){
                    app.error=app.session.resources.error;app.boot_worker_failed=true;return;
                }
            }
            app.boot_job_published=true;
            std::printf("[loading14] worker queued file%u\n",u32(app.boot_uploads[token->index].file));return;
        }
        if(token->index>=app.boot_uploads.size()||app.boot_upload_index!=token->index+1){
            app.error="TH14 boot upload owner changed";app.boot_worker_failed=true;return;
        }
        const auto file=app.boot_uploads[token->index].file;
        // Retail47d090 waits for the main upload pump, then the worker
        // publishes604/608. GPU completion alone does not enable draw60.
        if(file==1){app.boot_draw_callback_enabled=true;app.boot_signature_uploaded=true;}
        if(file==2)app.boot_ascii_uploaded=true;
        if(file==0){
            if(!app.session.initialize_compositor()){
                app.error="TH14 startup compositor: "+app.session.compositor.last_error;app.boot_worker_failed=true;return;
            }
            app.session.reset_compositor_cameras(false);
        }
        if(file==8&&!app.initialize_boot_scores()){app.boot_worker_failed=true;return;}
        std::printf("[loading14] worker resumed after file%u\n",u32(file));
        if(app.boot_upload_index<app.boot_uploads.size())app.dispatch_boot_work(BootWork::Decode,app.boot_upload_index,0);
        else{
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
            app.dispatch_boot_work(BootWork::CoreDecode,0,0);
#else
            app.dispatch_boot_work(BootWork::Title,0,0);
#endif
        }
    }
    static void finish_stage_worker_wait(void* raw){
        std::unique_ptr<StageWorkerWaitToken> token(static_cast<StageWorkerWaitToken*>(raw));
        auto& app=*token->owner;
        if(!app.stage_worker_wait.owns(token->generation))return;
        const u64 now=u64(SDL_GetTicksNS());
        if(!app.stage_worker_wait.ready(now)){
            const int remaining=int((app.stage_worker_wait.remaining(now)+999999)/1000000);
            schedule_th14_work(finish_stage_worker_wait,token.release(),remaining);return;
        }
        const u64 elapsed=now-app.stage_worker_wait.started_ns;
        if(!app.stage_worker_wait.finish(token->generation,now))return;
        std::printf("[loading14] worker wait completed after %.3fms\n",double(elapsed)/1000000.);
    }
    static void start_stage_worker_wait(void* raw){
        std::unique_ptr<StageWorkerWaitToken> token(static_cast<StageWorkerWaitToken*>(raw));
        auto& app=*token->owner;
        if(!app.stage_worker_wait.owns(token->generation))return;
#if defined(TH14_STAGE_RESTORE_DELIVERY)
        if(!app.session.battle||!app.session.battle->pending_stage||!app.session.resources.loading_bundle()){
            app.stage_worker_wait.cancel();th14_stage_restore_cancel();return;
        }
#endif
        if(!app.stage_worker_wait.start(token->generation,u64(SDL_GetTicksNS())))return;
#if defined(TH14_CPP_STAGE_WORKER)
        if(!app.session.start_cpp_stage_worker()){
            app.error=app.session.error;app.stage_worker_wait.cancel();return;
        }
#endif
#if defined(TH14_STAGE_RESTORE_DELIVERY)
#if !defined(TH14_PHYSICAL_STAGE_MEMORY) && !defined(TH14_CPP_STAGE_WORKER)
        if(!th14_stage_restore_begin(app.session.state.replay?1:0)){
            app.error="TH14 independent stage restore could not start";
            app.stage_worker_wait.cancel();return;
        }
#endif
#endif
        std::printf("[loading14] worker wait dispatched; minimum60ms\n");
        schedule_th14_work(finish_stage_worker_wait,token.release(),60);
    }
    bool begin_stage_worker_wait(bool initial)override{
        stop_stage_wait_channels();
        stage_worker_wait.begin(initial);
#if defined(TH14_SHARED_SCENE_COPY)
        th14_shared_scene_cancel();
#if !defined(TH14_CPP_STAGE_DECODE)
        if(!initial&&!th14_shared_scene_prepare()){graphics.error="TH14 shared camera worker preparation failed";return false;}
#endif
#endif
#if defined(TH14_PHYSICAL_STAGE_MEMORY)
        if(!initial&&!th14_stage_memory_prepare(th14_stage_memory_layout())){graphics.error="TH14 physical stage worker preparation failed";return false;}
#endif
        if(stage_worker_wait.waiting){
#if defined(TH14_CPP_STAGE_WORKER)
            // The preloaded language worker can start inside Supervisor1,
            // before this pass reaches ANM29 or the next Player callback.
            auto token=std::make_unique<StageWorkerWaitToken>(StageWorkerWaitToken{this,stage_worker_wait.generation});
            if(!stage_worker_wait.start(token->generation,u64(SDL_GetTicksNS()))||!session.start_cpp_stage_worker()){
                error=session.error.empty()?"TH14 C++ worker dispatch failed":session.error;
                stage_worker_wait.cancel();return false;
            }
            schedule_th14_work(finish_stage_worker_wait,token.release(),60);
#else
            // Dispatch after this callback pass. No scene/resource pointer is
            // retained by either timer, and a newer generation cancels it.
            schedule_th14_work(start_stage_worker_wait,new StageWorkerWaitToken{this,stage_worker_wait.generation},0);
#endif
        }
        return true;
    }
    bool stage_worker_wait_ready()const override{return stage_worker_wait.ready(u64(SDL_GetTicksNS()));}
    void stop_stage_workers()noexcept override{
#if defined(TH14_CPP_INITIAL_LOADING)
        if(initial_cpp_resources){initial_cpp_resources->cancel_and_join();initial_cpp_resources.reset();}
#endif
#if defined(TH14_CPP_STAGE_WORKER)
        session.stop_cpp_stage_worker();
#endif
        stop_stage_wait_channels();
    }
    void stop_stage_wait_channels()noexcept{
        stage_worker_wait.cancel();
#if defined(TH14_STAGE_RESTORE_DELIVERY)
        th14_stage_restore_cancel();
#endif
#if defined(TH14_SHARED_SCENE_COPY)
        th14_shared_scene_cancel();
#endif
#if defined(TH14_PHYSICAL_STAGE_MEMORY)
        th14_stage_memory_cancel_join();
#endif
    }
#if defined(TH14_CPP_STAGE_WORKER)
    bool stage_cpp_worker_enabled()const override{return true;}
#endif
#if defined(TH14_PHYSICAL_STAGE_MEMORY)
    bool stage_memory_worker_enabled()const override{return true;}
    ReplayRngRestoreResultTh14 stage_memory_worker(const StageRestoreMemoryTh14& packet,u32 stack,bool playback)override{
        const auto status=th14_stage_memory_poll(u32(reinterpret_cast<uintptr_t>(&packet)),stack,playback?1:0);
        return status==0?ReplayRngRestoreResultTh14::Waiting:status==1?ReplayRngRestoreResultTh14::Ready:ReplayRngRestoreResultTh14::Failed;
    }
#endif
#if defined(TH14_STAGE_RESTORE_DELIVERY)
    static u32 stage_restore_operation(ReplayRngRestorePointTh14 point){
        using P=ReplayRngRestorePointTh14;
        return point==P::GameplaySeedStored||point==P::StageSeedRecorded?0:
            point==P::ScriptSeedStored?1:point==P::GameplayCounterCleared?2:~0u;
    }
    ReplayRngRestoreResultTh14 stage_rng_store_readiness(ReplayRngRestorePointTh14 point)const override{
        const auto status=th14_stage_restore_ready(stage_restore_operation(point));
        return status==1?ReplayRngRestoreResultTh14::Ready:status==0?ReplayRngRestoreResultTh14::Waiting:ReplayRngRestoreResultTh14::Failed;
    }
    bool stage_rng_store_committed(ReplayRngRestorePointTh14 point)override{
        return th14_stage_restore_committed(stage_restore_operation(point))!=0;
    }
    ReplayRngRestoreResultTh14 stage_replay_snapshot_readiness()const override{
        const auto status=th14_stage_restore_ready(3);
        return status==1?ReplayRngRestoreResultTh14::Ready:status==0?ReplayRngRestoreResultTh14::Waiting:ReplayRngRestoreResultTh14::Failed;
    }
    bool stage_replay_snapshot_committed()override{return th14_stage_restore_committed(3)!=0;}
#endif
    bool draw_startup_loading()override{
#if defined(TH14_CPP_BOOT_WORKER)
        read_cpp_boot_publications();
#endif
        // 4442f0 registers draw60 disabled. Only44419a enables its bit2
        // after47d090 returned the prepared Sig file. The body444660 counts
        // unconditionally, but a disabled callback must never reach it.
        if(!boot_draw_callback_enabled)return true;
        if(boot_signature_uploaded&&boot_signature_pending){
            if(!boot_signature_handle){
                auto* vm=session.animations.create(boot_signature,0,1,0xffffffffu);
                if(!vm){graphics.error=session.animations.last_error_text();return false;}
                boot_signature_handle=vm->id;
            }
            boot_signature_pending=false;
            std::printf("[loading14] sig script0 created at draw priority60 after sig upload\n");
        }
#if defined(TH14_CPP_BOOT_WORKER)
        // Retail44472a reads ASCII readiness after Sig construction/update.
        if(boot_cpp_resources)boot_ascii_uploaded=boot_cpp_resources->publication(2)!=0;
#endif
        if(boot_ascii_uploaded&&boot_ascii_pending){
            if(!boot_loading.begin(session.animations,session.resources.core.ascii)){
                graphics.error=session.animations.last_error_text();return false;
            }
            boot_ascii_pending=false;
            std::printf("[loading14] ascii script17 created at draw priority60 after ascii upload\n");
        }
#if defined(TH14_CPP_BOOT_WORKER)
        boot_signature_draws=u32(boot_signature_draws)+1;
#else
        ++boot_signature_draws;
#endif
#if defined(TH14_CPP_BOOT_WORKER)
        publish_cpp_title_loading();
#endif
        return true;
    }
    bool stage_ready()const override{return boot_uploads.empty()&&initial_decode_complete;}
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
    void dispatch_stage_decode(){
        if(!stage_decode.dispatchable())return;
        const auto names=stage_decode.names();
        auto* token=new StageDecodeToken{this,stage_decode.dispatch(),!names.empty()};
        if(names.empty()){schedule_th14_work(run_stage_decode,token,0);return;}
        std::vector<const char*> pointers;for(const auto& name:names)pointers.push_back(name.c_str());
        th14_webgpu_archive_schedule(run_stage_decode,token,pointers.data(),u32(pointers.size()));
    }
    static void run_stage_decode(void* raw){
        std::unique_ptr<StageDecodeToken> token(static_cast<StageDecodeToken*>(raw));auto& app=*token->owner;
        if(!app.stage_decode.complete(token->task,!token->archive||th14_webgpu_archive_status()))return;
        if(app.stage_decode.result()==StageDecodeTh14::Result::Failed){app.graphics.error=app.stage_decode.error();return;}
        app.dispatch_stage_decode();
    }
    StageResourceEffects::DecodeResult decode_stage_phase(GameResources& source,StageResources& stage,StageResourceEffects::Phase phase,u32 selection)override{
        const auto result=stage_decode.begin(source,stage,phase,selection);
        if(result==StageDecodeTh14::Result::Failed)graphics.error=stage_decode.error();
        else dispatch_stage_decode();
        return result;
    }
    void cancel_stage_decode(StageResources& stage)override{
        if(stage_decode.owns(stage)){stage_decode.cancel(stage);th14_webgpu_archive_cancel();}
    }
#endif
#if defined(TH14_CPP_STAGE_DECODE) && !(defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER))
    bool stage_cpp_decode_enabled()const override{return true;}
    bool finish_cpp_stage_tail(u32 number,u32 mode_flags,Timer& clock,const float* rate,StageCppWorkerTh14& worker)override{
        if(!StageCppWorkerScopeTh14::owns(&session))return false;
        return th14_finish_stage_audio_tail(number,mode_flags,(session.persistent_config().bytes[0x38]&0x10)!=0,
            audio.music_requests(),frame_statistics,clock,rate,worker);
    }
    StageResourceEffects::DecodeResult decode_stage_phase(GameResources&,StageResources& stage,StageResourceEffects::Phase phase,u32 selection)override{
        const auto result=session.cpp_stage_decode(stage,phase,selection);
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
        StageResources* offered_owner=nullptr;AnmResource* resource=nullptr;u16 file=0;Phase offered_phase;
        const auto offered=session.cpp_stage_upload_offer(offered_owner,offered_phase,resource,file);
        if(offered==DecodeResult::Failed||(offered==DecodeResult::Ready&&
           (offered_owner!=&stage||!stage_uploads.begin_file(stage,offered_phase,*resource,file)))){
            graphics.error="TH14 pending ANM file upload owner changed";return DecodeResult::Failed;
        }
#endif
        return result;
    }
#endif
    bool prepare_stage_phase(StageResources& stage,StageResourceEffects::Phase phase)override{
        renderer.flush();graphics.error.clear();
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE) && !(defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER))
        if(!stage_uploads.complete_phase(stage,phase)){graphics.error="TH14 stage file upload still pending";return false;}
#else
        if(!stage_uploads.begin(stage,phase)){graphics.error="TH14 stage upload owner still pending";return false;}
#endif
        return true;
    }
    bool stage_phase_ready(StageResourceEffects::Phase phase)const override{return stage_uploads.ready(phase);}
#if defined(TH14_SHARED_SCENE_COPY)
    CameraReadResult read_stage_camera_word(const SceneCamera& source,u32 word,u32 offset,u32& value)override{
        static const auto layout=[](){std::array<u32,75> result{};
            for(u32 i=0;i<75;++i)result[i]=SceneCameraReadJobTh14::source_offset(i);return result;}();
        const auto status=th14_shared_scene_read(u32(reinterpret_cast<uintptr_t>(&source)),
            u32(reinterpret_cast<uintptr_t>(layout.data())),word,offset,u32(reinterpret_cast<uintptr_t>(&value)));
        return status==0?CameraReadResult::Waiting:status==1?CameraReadResult::Ready:CameraReadResult::Failed;
    }
#endif
    void finish_stage_phases(StageResources& stage,u32)override{
#if defined(TH14_SHARED_SCENE_COPY)
        th14_shared_scene_cancel();
#endif
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        stage_decode.finish(stage);
#endif
        stage_uploads.finish(stage);
    }
    void cancel_stage_phases(StageResources& stage,u32)override{
        // Queued raw resource references must disappear before GPU teardown
        // or StageResources destruction. release also covers boot ownership.
        stop_stage_workers();
        stage_uploads.cancel(stage);release(stage);
    }
    bool defer_ending_loading()const override{return true;}
    bool begin_ending_loading()override{
        if(!boot_loading.begin(session.animations,session.resources.core.ascii)){graphics.error="TH14 ending loading animation creation failed";return false;}
        return true;
    }
    void finish_ending_loading()override{boot_loading.finish(session.animations);std::printf("[loading14] ending ANM ready; interrupt1 delivered\n");}
#endif
    const std::string& last_error()const override{return graphics.error;}
    void release(StageResources& stage)override{
#if PORT_TITLE == 14
        cancel_stage_decode(stage);
        stage_uploads.cancel(stage);
        if(boot_stage_owner==&stage){cancel_boot_worker();boot_uploads.clear();boot_upload_index=0;boot_stage_owner=nullptr;boot_loading.finish(session.animations);}
#endif
        renderer.flush();renderer.invalidate();
        for(auto* file:{&stage.background,&stage.logo,&stage.enemies})graphics.unload(*file);
#if PORT_TITLE == 14 || PORT_TITLE == 16
        for(auto& file:stage.ecl_animations)graphics.unload(file);
#endif
    }
    bool prepare_animation(AnmResource& file)override{renderer.flush();return graphics.preload(file);}
#if PORT_TITLE == 14
    bool prepare_manual_image(AnmResource& file,u32 index,const u8* bytes,u32 size)override{renderer.flush();renderer.invalidate();return graphics.manual_image(file,index,bytes,size);}
#endif
    void release_animation(AnmResource& file)override{renderer.flush();renderer.invalidate();graphics.unload(file);}
    bool read_replay(const std::string& path,std::vector<u8>& bytes)override{return read_file(path.c_str(),bytes,64*1024*1024);}

    bool read_archive() {
        SDL_IOStream* io=SDL_IOFromFile("/th11.dat","rb");
        if(!io){error="Unable to open /th11.dat";return false;}
        const Sint64 size=SDL_GetIOSize(io);
        if(size<=0||size>256*1024*1024){SDL_CloseIO(io);error="Invalid th11.dat size";return false;}
        archive.resize(size_t(size));
        const size_t got=SDL_ReadIO(io,archive.data(),archive.size());
        SDL_CloseIO(io);
        if(got!=archive.size()){error="Unable to read /th11.dat";return false;}
        return resources.open_archive(archive.data(),u32(archive.size()));
    }

    bool initialize() {
        if(initialized){
#if PORT_TITLE == 14 && defined(TH14_CPP_BOOT_WORKER)
            if(audio_cpp_boot)return true;
#endif
            if(!audio.initialize(resources)){error=audio.error;return false;}return true;
        }
#if PORT_TITLE == 14 && defined(TH14_CPP_BOOT_WORKER)
        cpp_menu_close_started=cpp_menu_close_done=false;
        // Native469520 clears only the two host-close bits at cold startup.
        __atomic_fetch_and(&session.stage_load_host_flags,~u32(0x180),__ATOMIC_ACQ_REL);
        struct IncompleteBootGuard {Application& owner;~IncompleteBootGuard(){if(!owner.initialized)owner.cancel_boot_worker();}} guard{*this};
#endif
#if PORT_TITLE == 14 && defined(__EMSCRIPTEN__) && defined(TH14_CPP_BOOT_WORKER)
        // Compile/hot-tier the pure codec before any loading frames/workers.
        // This local blank record is discarded; live scores and RNG stay untouched.
        {ScoreFile warm_scores;std::vector<u8> discarded;(void)warm_scores.save(discarded);}
#endif
#if PORT_TITLE == 14
        mkdir("/save",0777);mkdir("/save/replay",0777);
        if(SDL_GetPathInfo(config_path,nullptr)){
            std::vector<u8> saved;
            if(!read_file(config_path,saved,u32(session.config.bytes.size()))||!session.config.open(saved.data(),u32(saved.size()))){
#if PORT_TITLE == 14
                // Retail44552d logs a rejected config, calls4376e0 to reset
                // it, then joins the ordinary startup path at445549.
                // Keep import validation strict; only cold startup recovers.
                session.config.reset();
                std::printf("[config14] Invalid %s; using defaults\n",config_path);
#else
                error=std::string("Invalid ")+config_path;return false;
#endif
            }else saved_config_state=std::move(saved);
        }
        if(!graphics.initialize(session.config.render_width(),session.config.render_height())){error=graphics.error;return false;}
#else
        if(!graphics.initialize()){error=graphics.error;return false;}
#endif
#if PORT_TITLE == 14 && defined(TH14_PHYSICAL_STAGE_MEMORY)
        if(!th14_stage_memory_open(th14_stage_memory_layout())){error="Unable to open TH14 physical stage kernel";return false;}
#endif
#if PORT_TITLE == 14
        session.animations.th14_native_stack.configure(process_cookie);
        session.animations.texture_high_precision=false;
        graphics.texture_high_precision=&session.animations.texture_high_precision;
        std::printf("[clock14] monotonic ticks=%u; initial seed=%u\n",initial_ticks,u32(u16(initial_ticks)));
#endif
        if(!read_archive()){if(error.empty())error=resources.error();return false;}
#if PORT_TITLE == 14 && defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        if(!th14_webgpu_archive_open(archive.data(),u32(archive.size()))){error="Unable to start TH14 background archive decoder";return false;}
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_BOOT_WORKER)
        cancel_boot_worker();
        if(!audio.initialize_device()){error=audio.error;return false;}
        audio_cpp_boot=std::make_unique<AudioCppBootTh14>();
        if(!audio_cpp_boot->begin(archive.data(),u32(archive.size()))){error="Unable to start TH14 audio sample worker";cancel_boot_worker();return false;}
#else
        if(!audio.initialize(resources)){error=audio.error;return false;}
#endif
        if(!fonts.initialize(
#if PORT_TITLE==14
            &session.font_bitmap
#endif
        )){error=fonts.error;return false;}
#if PORT_TITLE != 14
        mkdir("/save",0777);mkdir("/save/replay",0777);
#if PORT_TITLE != 14
        if(SDL_GetPathInfo(score_path,nullptr)){std::vector<u8> saved;if(!read_file(score_path,saved,4*1024*1024)||!session.load_scores(saved.data(),u32(saved.size()))){error=std::string("Invalid ")+score_path+": "+session.scores.error;return false;}}
#endif
        if(SDL_GetPathInfo(config_path,nullptr)){
            std::vector<u8> saved;
            if(!read_file(config_path,saved,u32(session.config.bytes.size()))||!session.config.open(saved.data(),u32(saved.size()))){
#if PORT_TITLE == 14
                // Retail44552d logs a rejected config, calls4376e0 to reset
                // it, then joins the ordinary startup path at445549.
                // Keep import validation strict; only cold startup recovers.
                session.config.reset();
                std::printf("[config14] Invalid %s; using defaults\n",config_path);
#else
                error=std::string("Invalid ")+config_path;return false;
#endif
            }else saved_config_state=std::move(saved);
        }
#endif
        audio.music_volume=session.config.music_volume();audio.effects.master_volume=session.config.sound_volume();audio.refresh_volume();
        session.recording_timestamp=u64(std::time(nullptr));
#if PORT_TITLE == 14
#if (!defined(TH14_WEBGPU_BACKEND) || !defined(TH14_ARCHIVE_WORKER)) && !defined(TH14_CPP_BOOT_WORKER)
        if(!resources.open_anm("sig.anm",boot_signature)){error="sig ANM: "+boot_signature.detail;return false;}
        if(!session.animations.prepare_resource(boot_signature,1)){error=session.animations.last_error_text();return false;}
#endif
#endif
        if(!session.open_title(resources,true)){error=session.error;return false;}
        auto& core=session.resources.core;
#if PORT_TITLE == 14
        // Upload real pending resources between frames so the original loading
        // ANM can update and draw while GPU work remains. Its random children
        // follow script17's own clock, never a captured RNG count.
        boot_signature_pending=true;boot_signature_active=true;boot_signature_draws=0;
        boot_signature_uploaded=false;boot_draw_callback_enabled=false;boot_ascii_uploaded=false;boot_ascii_pending=true;
        // These uploads also belong to the original main resource pump. Draw
        // Sig and ASCII only after their own file's GPU preparation completes.
        boot_uploads={{&boot_signature,1},{&core.ascii,2},{&core.text,0},{&core.front,5},{&core.bullet,7},{&core.effect,8},{&core.title,16},{&core.title_variant,17}};
        boot_upload_index=0;std::printf("[loading14] sig/ascii/text queued before common resources; %u resource uploads pending\n",u32(boot_uploads.size()));
#if !defined(TH14_CPP_BOOT_WORKER)
        cancel_boot_worker();
#endif
        boot_worker_active=true;
#if defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)
        boot_core_decoded=0;
#endif
#if defined(TH14_CPP_BOOT_WORKER)
        boot_cpp_resources=std::make_unique<BootCppResourcesTh14>(session,std::array<AnmResource*,6>{
            &boot_signature,&core.ascii,&core.text,&core.front,&core.bullet,&core.effect},
            [this](GameResources& source,std::string& failure){return audio_cpp_boot->finish_after_text(source,failure);});
        if(!boot_cpp_resources->begin(archive.data(),u32(archive.size()))){error="Unable to start TH14 C++ bootstrap";cancel_boot_worker();return false;}
        std::printf("[loading14] real C++ bootstrap worker started\n");
#else
        dispatch_boot_work(BootWork::Decode,0,0);
#endif
        boot_stage_owner=nullptr;
#else
#if PORT_TITLE == 16
        if(!graphics.preload(core.effect)){error=graphics.error;return false;}
#endif
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1],&core.players[2]})
            if(!graphics.preload(*file)){error=graphics.error;return false;}
#if PORT_TITLE == 16
        if(!graphics.preload(core.players[3])){error=graphics.error;return false;}
        for(auto& file:core.season_players)if(!graphics.preload(file)){error=graphics.error;return false;}
#endif
#endif
#if PORT_TITLE == 14
        renderer.viewport={0,0,session.config.render_width(),session.config.render_height(),0,1};
#else
        renderer.viewport={0,0,640,480,0,1};
#endif
        // GameBattle's STD owner creates backdrop VMs and publishes its camera.
#if PORT_TITLE != 14
        frame_statistics.window_start=double(SDL_GetTicks())*.001;
#endif
        initialized=true;return audio_events()&&text_events();
    }

    bool restart(u32 stage=1,i32 character=0,i32 subtype=0,i32 difficulty=1) {
#if PORT_TITLE == 14
        audio.discard_paused_sounds();
#endif
        if(!initialize())return false;
#if PORT_TITLE == 14
        // Join the old writer before reading or constructing its score owner.
        cancel_boot_worker();
        if(!initialize_boot_scores())return false;
#if defined(TH14_CPP_BOOT_WORKER)
        if(!boot_common_resources_ready){
            // Explicit replacement retires all old loading VMs before a
            // synchronous bootstrap fallback can replace their resource data.
            session.shutdown();
            if(!session.resources.load_core_bootstrap()){error=session.resources.error;return false;}
        }
#endif
        boot_signature_pending=false;boot_signature_active=false;boot_signature_handle=0;boot_signature_draws=0;
        boot_signature_uploaded=false;boot_draw_callback_enabled=false;boot_ascii_uploaded=false;boot_ascii_pending=false;
        if(!core_loading_done){if(!session.resources.prepare_core()){error=session.resources.error;return false;}core_loading_done=true;}
        boot_loading.finish(session.animations);boot_uploads.clear();boot_upload_index=0;
        boot_stage_owner=nullptr;stage_uploads.cancel_all();
#endif
        if(!save_scores())return false;session.recording_timestamp=u64(std::time(nullptr));
        renderer.invalidate();
        auto& core=session.resources.core;
#if PORT_TITLE == 14 || PORT_TITLE == 16
        graphics.unload(core.effect);
#endif
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1],&core.players[2]})graphics.unload(*file);
#if PORT_TITLE == 16
        graphics.unload(core.players[3]);for(auto& file:core.season_players)graphics.unload(file);
#endif
        if(session.resources.stage)for(AnmResource* file:{&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})graphics.unload(*file);
#if PORT_TITLE == 14 || PORT_TITLE == 16
        if(session.resources.stage)for(auto& file:session.resources.stage->ecl_animations)graphics.unload(file);
#endif
        if(!session.begin(resources,stage,character,subtype,difficulty,false)){error=session.error;return false;}
#if PORT_TITLE == 14
        // begin() clears the previous session's VMs. An interrupted cold boot
        // can still own sig textures; release them only after those references
        // are gone, without an extra ANM update consuming random values.
        if(session.animations.references(boot_signature)){error="TH14 sig still referenced after session replacement";return false;}
        graphics.unload(boot_signature);boot_signature={};
#endif
        reset_entry_frame_statistics();
#if PORT_TITLE == 14 || PORT_TITLE == 16
        if(!graphics.preload(core.effect)){error=graphics.error;return false;}
#endif
        for(AnmResource* file:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1],&core.players[2],&session.resources.stage->background,&session.resources.stage->logo,&session.resources.stage->enemies})
            if(!graphics.preload(*file)){error=graphics.error;return false;}
#if PORT_TITLE == 16
        if(!graphics.preload(core.players[3])){error=graphics.error;return false;}
        for(auto& file:core.season_players)if(!graphics.preload(file)){error=graphics.error;return false;}
#endif
        if constexpr(kPortTitle==14)audio.discard_saved_music();
#if PORT_TITLE == 14
        if(session.state.phase==GameSessionPhase::loading)return true;
#endif
        if(!play_stage_music(session.state.stage,true))return false;
        return audio_events()&&text_events();
    }

#if PORT_TITLE == 14 && defined(TH14_CPP_BOOT_WORKER)
    bool tick_once(u32 held,bool pause_key=false) {
        // A failed frame can leave the independent sample loader waiting at
        // startup0, or a resource worker waiting for a Main upload. Retire all
        // bootstrap writers before returning the original failure to the host.
        if(cpp_menu_close_done)return false;
        if(boot_worker_failed)return false;
        const bool result=tick_once_unchecked(held,pause_key);
        if(!result&&(boot_worker_active||boot_cpp_resources||title_cpp_resources||audio_cpp_boot
#if defined(TH14_CPP_INITIAL_LOADING)
            ||initial_cpp_resources
#endif
            )){
            cancel_boot_worker();boot_worker_failed=true;
        }
        return result;
    }
    bool tick_once_unchecked(u32 held,bool pause_key=false) {
#else
    bool tick_once(u32 held,bool pause_key=false) {
#endif
#if PORT_TITLE == 14
        repeat_update=false;
#endif
        if(!initialize())return false;
#if PORT_TITLE == 14
        if(boot_worker_failed)return false;
        audio.update();
        // Retail4448be consumes one music request before config/input and
        // the47d720 upload pump, including every initial loading callback.
        if(!audio.consume_initial_music_request((session.persistent_config().bytes[0x38]&0x10)!=0)){error=audio.error;return false;}
#if defined(TH14_CPP_BOOT_WORKER)
        const auto menu_close=poll_cpp_menu_close();
        if(menu_close!=CppMenuCloseState::Inactive)return menu_close==CppMenuCloseState::Waiting;
        if(!poll_cpp_boot_resources()||!poll_cpp_title_resources())return false;
#if defined(TH14_CPP_INITIAL_LOADING) && defined(TH14_CPP_STAGE_DECODE)
        // A retained initial actor has no data writes after Control enable.
        // Reap only after return; shutdown still joins before owner teardown.
        if(initial_cpp_resources&&!boot_stage_owner&&initial_cpp_resources->poll()!=ReplayRngRestoreResultTh14::Waiting){
            const auto status=initial_cpp_resources->poll();initial_cpp_resources->join();
            if(status!=ReplayRngRestoreResultTh14::Ready){error="TH14 retained initial worker failed";return false;}
            initial_cpp_resources.reset();
        }
#endif
#endif
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE) && !(defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER))
        // Retail47d720 services a published resource independently of the
        // controller's phase polling. Admit the worker's immutable offer
        // before this pump, retaining its actual stage owner and phase.
        StageResources* upload_owner=nullptr;AnmResource* upload_resource=nullptr;u16 upload_file=0;
        StageResourceEffects::Phase upload_phase;
        const auto offered=session.cpp_stage_upload_offer(upload_owner,upload_phase,upload_resource,upload_file);
        if(offered==StageResourceEffects::DecodeResult::Failed||
           (offered==StageResourceEffects::DecodeResult::Ready&&!stage_uploads.begin_file(*upload_owner,upload_phase,*upload_resource,upload_file))){
            error="TH14 worker upload offer owner changed";return false;
        }
#endif
        if(stage_uploads.pending()){
            renderer.flush();graphics.error.clear();
            if(!stage_uploads.advance([&](const auto& job){return graphics.preload_chunk(*job.resource,job.chunk);},[&](const auto& job){
                if(session.animations.prepare_resource(*job.resource,job.file)){
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE) && !(defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER))
                    if(!session.cpp_stage_complete_file(*job.resource,job.file)){graphics.error="TH14 ANM file completion owner changed";return false;}
#endif
                    return true;
                }
                graphics.error=session.animations.last_error_text();return false;
            })){
                error=graphics.error.empty()?"TH14 staged resource upload failed":graphics.error;return false;
            }
            // A successful stage upload continues through the normal session,
            // Player/HUD/old Stage callbacks and draw below, without a second
            // ANM update or a loading-frame return.
        }
        if(boot_upload_index<boot_uploads.size()||boot_signature_active||boot_stage_owner){
#if defined(TH14_CPP_INITIAL_LOADING)
            if(initial_cpp_resources){
                if(!pump_cpp_initial_upload())return false;
            }else
#endif
#if defined(TH14_CPP_BOOT_WORKER)
            if(boot_cpp_resources||title_cpp_resources){
                if(!pump_cpp_boot_upload())return false;
            }else
#endif
            {
            bool cpp_boot_file_ready=false;
#if defined(TH14_CPP_BOOT_WORKER)
            if(boot_cpp_resources||title_cpp_resources){
                AnmResource* offered=nullptr;u16 file=0;
                if(boot_cpp_resources?boot_cpp_resources->pending(offered,file):title_cpp_resources->pending(offered,file)){
                    if(boot_upload_index>=boot_uploads.size()||boot_uploads[boot_upload_index].resource!=offered||boot_uploads[boot_upload_index].file!=file){error="TH14 C++ bootstrap file owner changed";return false;}
                    cpp_boot_file_ready=true;
                }
            }
#endif
            if(boot_upload_index<boot_uploads.size()&&!initial_upload_waiting&&(!boot_worker_active||boot_job_published||cpp_boot_file_ready)){
                auto& job=boot_uploads[boot_upload_index];
                if(!boot_worker_active&&!core_loading_done&&boot_upload_index>=3){
                    constexpr u32 title_jobs[]={0,1,2,4,5};
                    if(!session.resources.load_core_job(title_jobs[boot_upload_index-3])){error=session.resources.error;return false;}
                }
                // Original resource priority1 pumps one chunk. Its final chunk
                // prepares templates before the priority29/30 ANM callbacks.
                if(!graphics.preload_chunk(*job.resource,job.chunk)){error=graphics.error;return false;}
                const bool resource_ready=++job.chunk>=job.resource->textures.size();
#if defined(TH14_CPP_BOOT_WORKER)
                if(resource_ready&&cpp_boot_file_ready&&job.file==2)session.animations.fallback_sprites=job.resource;
#endif
                if(resource_ready&&!session.animations.prepare_resource(*job.resource,job.file)){error=session.animations.last_error_text();return false;}
                if(resource_ready){
                    if(!boot_worker_active&&job.resource==&boot_signature){boot_draw_callback_enabled=true;boot_signature_uploaded=true;}
                    if(!boot_worker_active&&job.resource==&session.resources.core.ascii)boot_ascii_uploaded=true;
                    if(!boot_worker_active&&!core_loading_done&&job.resource==&session.resources.core.text){
                        if(!session.initialize_compositor()){error="TH14 startup compositor: "+session.compositor.last_error;return false;}
                        session.reset_compositor_cameras(false);
                    }
                    std::printf("[loading14] file %u ready after %u chunk callbacks\n",u32(job.file),job.chunk);
                    const u32 completed=boot_upload_index++;
#if defined(TH14_CPP_BOOT_WORKER)
                    if(cpp_boot_file_ready){
                        const bool accepted=boot_cpp_resources?boot_cpp_resources->complete(*job.resource,job.file):title_cpp_resources->complete(*job.resource,job.file);
                        if(!accepted){error="TH14 C++ bootstrap/title file ACK rejected";return false;}
                    }else
#endif
                    if(boot_worker_active){boot_job_published=false;dispatch_boot_work(BootWork::Resume,completed,1);}
                    else if(boot_stage_owner)dispatch_initial_upload_resume(completed);
                }
            }
            }
#if defined(TH14_CPP_BOOT_WORKER)
            // Main3 reads the common-resource enable after Main47d720 too.
            if(!initialize_cpp_boot_compositor())return false;
            // Main47d720 may release the title worker in this same pass.
            // Read its live Title6 enable after the pump, before ANM29/30.
            if(!poll_cpp_title_resources())return false;
#endif
            // 45959a waits for actual priority60 callbacks to reach180. Only
            // after both upload readiness and that count does 4595e1 unload sig.
#if defined(TH14_CPP_BOOT_WORKER)
            // The real title thread owns Sleep16 and the live host-bit bypass.
            const bool signature_ready=boot_signature_active&&!boot_signature_pending&&boot_upload_index==boot_uploads.size()
                &&(boot_title_ready||(!boot_worker_active&&boot_signature_draws>=180));
#else
            const bool signature_ready=boot_signature_active&&!boot_signature_pending&&boot_signature_draws>=180&&boot_upload_index==boot_uploads.size()&&(!boot_worker_active||boot_title_ready);
#endif
            bool title_entry_advanced=false;
            if(signature_ready){
                session.animations.retire_resource(boot_signature);
                // The ready worker enables Title6 before overlay8/ANM29.
                // Its initial resource retirement and title creations share
                // that pass with the pending sig deletion, preserving the
                // allocator's free-list order. Do not add a second ANM tick.
                core_loading_done=true;
                if(session.state.phase==GameSessionPhase::loading){
                    if(!session.finish_loading()){error=session.error;return false;}
                }
                if(session.state.phase==GameSessionPhase::title){
                    boot_loading.finish(session.animations);
                    const GameSessionInput input{held,0,0,pause_key,bool(held&0x100),bool(held&0x2),frame_statistics.fps};
                    if(!session.update(input)){error=session.error;return false;}
                    if(!audio_events()||!text_events())return false;
                    title_entry_advanced=true;
                }
            }
            bool initial_stage_overlay_updated=false;
#if defined(TH14_CPP_INITIAL_LOADING) && defined(TH14_CPP_STAGE_DECODE)
            if(!title_entry_advanced&&boot_stage_owner&&initial_cpp_resources){
                // Overlay8 may overlap the worker's Control11 enable store.
                // Read completion again at this callback boundary, without
                // running Main's resource pump or an ANM update twice.
                if(!session.animations.update(true)){error=session.animations.last_error_text();return false;}
                initial_stage_overlay_updated=true;
                if(!poll_cpp_initial_completion(false))return false;
            }
#endif
            bool initial_stage_entry_advanced=false;
            if(boot_stage_owner&&boot_upload_index==boot_uploads.size()&&!initial_upload_waiting){
                // The worker clears Control+80 bit4 after its final upload.
                // Control11 then attaches the world before Player/ANM in the
                // next pass; do not create HUD after that pass's ANM update.
#if defined(TH14_CPP_INITIAL_LOADING) && defined(TH14_CPP_STAGE_DECODE)
                cancel_boot_worker(true);
#else
                cancel_boot_worker();
#endif
                // Control11 finishes the shared loading family after overlay8.
                boot_uploads.clear();boot_upload_index=0;boot_stage_owner=nullptr;
                if(!session.finish_loading(true)){error=session.error;return false;}
                reset_entry_frame_statistics();audio.effects.reset();
                if(!play_stage_music(session.state.stage,true))return false;
                const GameSessionInput input{held,0,0,pause_key,bool(held&0x100),bool(held&0x2),frame_statistics.fps};
                const bool updated=initial_stage_overlay_updated?session.update_after_initial_overlay(input):session.update(input);
                if(!updated){error=session.error;return false;}
                session.battle->th14_current_music=audio.current_music();
                if(!audio_events()||!text_events())return false;
                initial_stage_entry_advanced=true;
                std::printf("[loading14] initial worker ready; control callbacks attached\n");
            }
            if(!title_entry_advanced&&!initial_stage_entry_advanced&&
               ((!initial_stage_overlay_updated&&!session.animations.update(true))||!session.animations.update(false))){error="TH14 loading ANM update failed";return false;}
            if(signature_ready){
                if(session.animations.references(boot_signature)){error="TH14 sig still referenced after ANM retirement";return false;}
                graphics.unload(boot_signature);renderer.invalidate();boot_signature={};
                boot_signature_active=false;boot_signature_handle=0;
                std::printf("[loading14] sig retired after %u draw callbacks\n",u32(boot_signature_draws));
            }
            if(!initial_stage_entry_advanced&&boot_upload_index==boot_uploads.size()&&!boot_signature_active&&!initial_upload_waiting){
#if defined(TH14_CPP_BOOT_WORKER)
                // Title6 is already live. Its callback has no resource writes
                // after Ready; retain that owner until a later terminal poll.
                cancel_boot_worker(
#if defined(TH14_CPP_INITIAL_LOADING) && defined(TH14_CPP_STAGE_DECODE)
                    false,
#endif
                    boot_title_ready&&bool(title_cpp_resources));
#else
                cancel_boot_worker();
#endif
                core_loading_done=true;
                boot_loading.finish(session.animations);boot_uploads.clear();boot_upload_index=0;
                boot_stage_owner=nullptr;
                std::printf("[loading14] resource uploads complete; interrupt1 delivered\n");
                if(session.state.phase==GameSessionPhase::loading){
                    const bool existing_battle=session.battle_callbacks_attached();
                    if(!session.finish_loading()){error=session.error;return false;}
                    if(!existing_battle){reset_entry_frame_statistics();audio.effects.reset();
                        if(session.state.phase==GameSessionPhase::stage&&!play_stage_music(session.state.stage,true))return false;}
                    if(!audio_events()||!text_events())return false;
                }
            }
            // Main resource work and ANM updates run on skipped draws too.
            // Retail46a833 gates the draw60 signature callback with presentation.
            if(!present_this_update())return true;
            if(!session.draw(renderer,&frame_text)){error=session.error;return false;}
            th14_consume_deferred_captures(session.animations.deferred_captures,session,graphics,renderer);
            graphics.present();
            sample_frame_statistics();
            if(!session.sample_spell_clock(double(SDL_GetTicksNS())*1e-9)){error="Invalid spell timing state";return false;}
            return true;
        }
#endif
        const auto before=session.state.phase;
#if PORT_TITLE == 14
        const bool ending_load_pending=session.ending&&session.ending->resource_loading();
#endif
        const GameSessionInput input{held,0,0,pause_key,bool(held&0x100),bool(held&0x2),frame_statistics.fps};
#if PORT_TITLE == 14
        if(session.battle)session.battle->th14_current_music=audio.current_music();
#endif
        if(!session.update(input)){error=session.error;return false;}
#if PORT_TITLE == 14
        if(session.ending&&session.ending->resource_loading()){
            if(ending_load_pending){
                if(!session.ending->finish_resource_loading()){error=session.ending->error;return false;}
            }else std::printf("[loading14] ending ANM pending: %s\n",session.ending->resource_loading_name().c_str());
        }
#endif
        const auto after=session.state.phase;
#if PORT_TITLE == 14
        const bool death_event=session.battle&&std::any_of(session.battle->events.begin(),session.battle->events.end(),[](const auto& event){return event.kind==BattleEventKind::GameOver;});
        const bool resume_event=session.battle&&std::any_of(session.battle->events.begin(),session.battle->events.end(),[](const auto& event){return event.kind==BattleEventKind::PauseResume;});
        const bool was_menu=before==GameSessionPhase::paused||before==GameSessionPhase::game_over;
        const bool is_menu=after==GameSessionPhase::paused||after==GameSessionPhase::game_over;
        if(was_menu&&!is_menu&&!resume_event)audio.discard_paused_sounds();
        if(!was_menu&&after==GameSessionPhase::paused&&!death_event)audio.pause_sounds(true);
#endif
        if(before!=GameSessionPhase::stage&&after==GameSessionPhase::stage&&session.state.frame==0)reset_entry_frame_statistics();
        if(session.pause_menu)session.pause_menu->slowdown=frame_statistics.slowdown();
        if(session.title&&session.title->screen==TitleScreen::Results)session.title->result_slowdown=frame_statistics.slowdown();
        if(before!=GameSessionPhase::stage&&after==GameSessionPhase::stage
#if PORT_TITLE == 14
            &&!resume_event
#endif
        ){
            if(before==GameSessionPhase::paused&&session.state.frame!=0){if(music_follows_pause())audio.pause_music(false);}
            else if(kPortTitle==14&&before==GameSessionPhase::game_over&&session.state.frame!=0){
                if(!audio.restore_music()){error="Continue battle music restoration failed";return false;}
            }
            else{audio.effects.reset();if(!play_stage_music(session.state.stage,true))return false;}
        }
        if(kPortTitle==14&&before==GameSessionPhase::game_over&&after!=GameSessionPhase::game_over
#if PORT_TITLE == 14
            &&!resume_event
#endif
        )audio.discard_saved_music();
        if(before!=GameSessionPhase::paused&&after==GameSessionPhase::paused&&music_follows_pause()
#if PORT_TITLE == 14
            &&!death_event
#endif
        )audio.pause_music(true);
        if(before!=GameSessionPhase::game_over&&after==GameSessionPhase::game_over){
            session.pause_menu->timestamp=u64(std::time(nullptr));
#if PORT_TITLE != 14
            audio.pause_music(false);if(!audio.music(17)){error=audio.error;return false;}
#endif
        }
        if(before==GameSessionPhase::ending&&session.title)session.title->result_timestamp=u64(std::time(nullptr));
        if(session.title&&session.title->replay_scan_requested)scan_replays();
        if(session.title&&session.title->screen==TitleScreen::ReplaySave){auto& menu=*session.title;
            if(menu.substate==3&&!menu.pending_replay){const u64 stamp=u64(std::time(nullptr));std::memcpy(session.recording.header.data()+12,&stamp,8);auto entry=std::make_shared<ReplayMenuEntry>();
                if(!session.save_replay("        ",entry->file,true,frame_statistics.slowdown())||!entry->replay.open(entry->file.data(),u32(entry->file.size()))){error="Completed replay metadata unavailable";return false;}menu.pending_replay=std::move(entry);
            }
            if(menu.replay_save_requested){if(!save_replay(menu.replay_file+1,menu.entered_name.data()))return false;menu.replay_save_requested=false;menu.pending_replay.reset();scan_replays();if(!save_scores())return false;}
        }
        if(session.pause_menu){auto& menu=*session.pause_menu;
            if(menu.capture_requested){
                auto* vm=session.animations.find(menu.background_animation);if(!vm||!vm->sprite||!vm->resource){error="Pause screenshot sprite missing";return false;}
                const auto& sprite=*vm->sprite;const u32 target=graphics.texture(*vm->resource,sprite.texture);renderer.flush();
                i32 from[]={32,16,416,464};u32 source=GraphicsDevice::screen;
#if PORT_TITLE == 14
                const auto capture=th14_immediate_capture_source(session);
                std::copy(capture.rectangle,capture.rectangle+4,from);
                if(capture.text_intermediate){source=graphics.texture(session.resources.core.text,3);if(!source){error="Pause capture source texture missing";return false;}}
#endif
                const i32 to[]={i32(sprite.x),i32(sprite.y),i32(sprite.x+sprite.width),i32(sprite.y+sprite.height)};
                if(!graphics.backend.resample(source,from,target,to,nullptr,0,0)){error="Pause GPU capture failed";return false;}
                graphics.changed(target);renderer.invalidate();menu.capture_requested=false;
            }
            if(menu.scan_requested)scan_replays();
            if(menu.recording_metadata_requested){menu.timestamp=u64(std::time(nullptr));std::memcpy(session.recording.header.data()+12,&menu.timestamp,8);}
            if(menu.save_requested){if(!save_replay(menu.cursor.selected+1,menu.entered_name.data()))return false;menu.save_requested=false;scan_replays();if(!save_scores())return false;}
        }
#if PORT_TITLE != 14
        if(session.battle&&after==GameSessionPhase::stage&&!session.battle->spell_timing.update(session.battle->spell_flags,session.battle->spells.frame_count,double(SDL_GetTicks())*.001,session.state.replay)){error="Invalid spell timing state";return false;}
#endif
        if(!audio_events(
#if PORT_TITLE == 14
            before!=GameSessionPhase::game_over&&after==GameSessionPhase::game_over&&!death_event
#endif
        )||!text_events())return false;
#if PORT_TITLE != 14
        audio.update();
#endif
        audio.pump();
#if PORT_TITLE == 14
        // Registered Replay30 restarts the update list before any draw pass.
        // Keep sound/text delivery for each update, and spell-clock sampling,
        // deferred copies and GPU presentation for the completed list only.
#if defined(TH14_CPP_STAGE_DECODE)
        const u32 replay_control_flags=session.animations.th14_control.callback_flags();
#else
        const u32 replay_control_flags=after==GameSessionPhase::loading||
            (session.battle&&session.battle->pending_stage)?4u:0u;
#endif
        repeat_update=th14_replay_repeat(after==GameSessionPhase::stage&&
            // Replay30 stays disabled until455ed0, even after Control clears4.
            session.battle_callbacks_attached()&&session.battle&&session.battle->stage_active,
            session.state.replay?1u:0u,replay_control_flags,held,session.playback.callback_frame());
        if(repeat_update)return true;
#endif
#if PORT_TITLE != 14
        frame_statistics.sample(double(SDL_GetTicks())*.001,after==GameSessionPhase::stage&&session.battle&&session.battle->stage_active&&!session.state.replay);
#endif
#if PORT_TITLE == 14
        if(!present_this_update())return true;
#endif
        frame_text.clear();
#if PORT_TITLE == 14
        // Retail42e5c3..42e5d7 skips the FPS callback in Supervisor4 (title).
        const bool show_frame_statistics=after!=GameSessionPhase::title;
#else
        constexpr bool show_frame_statistics=true;
#endif
        if(show_frame_statistics){const auto label=frame_statistics.label();frame_text.add(label.text.c_str(),label.position,label.style);}
        if(!session.draw(renderer,show_frame_statistics?&frame_text:nullptr)){error=session.error;return false;}
#if PORT_TITLE == 14
        th14_consume_deferred_captures(session.animations.deferred_captures,session,graphics,renderer);
#endif
        graphics.present();
#if PORT_TITLE == 14
        // Retail Main draws FPS65 before Present, then samples42e4d0.
        sample_frame_statistics();
        // 46a6d7/46a933 call41c800 whenever the spell manager is alive,
        // including the frame that opens the spell-result menu.
        if(!session.sample_spell_clock(double(SDL_GetTicksNS())*1e-9)){error="Invalid spell timing state";return false;}
#endif
        return true;
    }
    bool tick(u32 held,bool pause_key=false){
#if PORT_TITLE == 14
        do{if(!tick_once(held,pause_key))return false;}while(repeat_update);
        return true;
#else
        return tick_once(held,pause_key);
#endif
    }
    bool return_to_title(){
#if PORT_TITLE == 14
        audio.discard_paused_sounds();
#endif
        if(!save_scores())return false;
        if(!session.return_to_title()){error=session.error;return false;}
        if constexpr(kPortTitle==14)audio.discard_saved_music();
        audio.effects.reset();return true;
    }
    void scan_replays(){
        if(!session.title&&!session.pause_menu)return;
        auto load=[&](u32 slot,const std::string& path){std::vector<u8> bytes;if(!read_file(path.c_str(),bytes,64*1024*1024))return;
            auto entry=std::make_shared<TitleMenu::ReplayEntry>();if(!entry->replay.open(bytes.data(),u32(bytes.size())))return;
            entry->path=path;entry->replay.retain_metadata();if(session.title)session.title->replay_files[slot]=std::move(entry);else if(slot<25)session.pause_menu->replay_files[slot]=std::move(entry);};
        for(u32 slot=0;slot<25;++slot){char path[80];std::snprintf(path,sizeof(path),"/save/replay/%s_%.2u.rpy",replay_prefix,slot+1);load(slot,path);load(slot,std::string(path)+"x");}
        if(session.title&&session.title->screen==TitleScreen::Replays){std::vector<std::string> names;auto* dir=opendir("/save/replay");if(dir){while(auto* e=readdir(dir)){const std::string n=e->d_name;if(n.size()>=15&&n.rfind(std::string(replay_prefix)+"_ud",0)==0&&(n.substr(11)==".rpy"||n.substr(11)==".rpyx"))names.push_back(n);}closedir(dir);}
            std::sort(names.begin(),names.end());for(u32 i=0;i<names.size()&&i<50;++i)load(25+i,"/save/replay/"+names[i]);}
        if(session.title&&session.title->screen==TitleScreen::Replays)session.title->replay_scan_complete();
    }
};
Application app;
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
touhou::sdl::FrameCadence cadence;
bool running=false,suspended=false;u32 loop_epoch=0;double previous_frame=-1;
std::array<u8,256> previous_scans{};
SDL_Joystick* controller=nullptr;
void add_controller(SDL_JoystickID id){if(!controller)controller=SDL_OpenJoystick(id);}
void initialize_controller(){
    if(controller){SDL_CloseJoystick(controller);controller=nullptr;}
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);int count=0;auto* ids=SDL_GetJoysticks(&count);
    for(int i=0;i<count;++i)add_controller(ids[i]);SDL_free(ids);
}
u32 sample_controller(){
    if(app.session.title)app.session.title->controller_buttons=0;
    if(!controller)return 0;std::array<u8,128> buttons{};
    for(int i=0;i<std::min(128,SDL_GetNumJoystickButtons(controller));++i)if(SDL_GetJoystickButton(controller,i)){
        buttons[i]=128;if(i<32&&app.session.title)app.session.title->controller_buttons|=1u<<i;
    }
    const auto axis=[&](int index){const int value=index<SDL_GetNumJoystickAxes(controller)?SDL_GetJoystickAxis(controller,index):0;return i32(std::floor((value<0?value/32768.:value/32767.)*1000+.5));};
    i32 x=axis(0),y=axis(1);const auto hat=SDL_GetNumJoystickHats(controller)?SDL_GetJoystickHat(controller,0):0;
    if(hat&SDL_HAT_LEFT)x=-1000;if(hat&SDL_HAT_RIGHT)x=1000;if(hat&SDL_HAT_UP)y=-1000;if(hat&SDL_HAT_DOWN)y=1000;
#if PORT_TITLE == 14
    return controller_keys(0,buttons.data(),u32(buttons.size()),x,y,app.session.persistent_config().bytes.data());
#else
    return controller_keys(0,buttons.data(),u32(buttons.size()),x,y,app.session.config.bytes.data());
#endif
}
touhou::input::TouchState touch_state(){
    touhou::input::TouchState s;auto& session=app.session;
    if(session.state.phase!=GameSessionPhase::stage||!session.battle||!session.battle->player)return s;
    if(session.state.replay){s.context=3;return s;}
    if(session.battle->dialogue&&session.battle->dialogue->active){s.context=2;return s;}
    const auto& player=*session.battle->player;const auto& p=player.motion.state;
    s.context=1;s.instance=int(session.state.stage);s.ready=player.state.life_state==1;
    s.x=p.position.x;s.y=p.position.y;s.fast=float(p.normal_speed)/128;s.slow=float(p.focus_speed)/128;
    s.min_x=-184;s.max_x=184;s.min_y=32;s.max_y=432;return s;
}
void clear_inputs(){th11_reset_browser_keyboard();SDL_ResetKeyboard();for(auto& k:keyboard_map)k.hosted=false;previous_scans.fill(0);gestures.reset();if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;}
void touch_event(unsigned type,int id,float x,float y){
    if(std::isfinite(x)&&std::isfinite(y))gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);
}
void cancel_touch(){
    gestures.cancel_transient();
    if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;
}
bool sample_and_tick_once(){
    SDL_Event event;while(SDL_PollEvent(&event)){
#if PORT_TITLE == 14
        if(event.type==SDL_EVENT_WINDOW_CLOSE_REQUESTED||event.type==SDL_EVENT_QUIT)app.session.request_host_close();
#endif
        // SDL owns gestures that start on the canvas; the browser bridge only
        // forwards gestures originating outside it, avoiding double delivery.
        if(event.type==SDL_EVENT_FINGER_CANCELED){cancel_touch();continue;}
        if(event.type==SDL_EVENT_FINGER_DOWN||event.type==SDL_EVENT_FINGER_MOTION||event.type==SDL_EVENT_FINGER_UP)
            touch_event(event.type==SDL_EVENT_FINGER_DOWN?0:event.type==SDL_EVENT_FINGER_MOTION?1:2,
                        int(event.tfinger.fingerID),event.tfinger.x,event.tfinger.y);
        if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST)clear_inputs();
        else if(event.type==SDL_EVENT_JOYSTICK_ADDED)add_controller(event.jdevice.which);
        else if(event.type==SDL_EVENT_JOYSTICK_REMOVED&&controller&&SDL_GetJoystickID(controller)==event.jdevice.which){SDL_CloseJoystick(controller);controller=nullptr;int count=0;auto* ids=SDL_GetJoysticks(&count);for(int i=0;i<count;++i)add_controller(ids[i]);SDL_free(ids);}
    }
    bool keys[256]{};std::array<u8,256> scans{};const auto* physical=th11_browser_keyboard()?nullptr:SDL_GetKeyboardState(nullptr);
    for(const auto& k:keyboard_map)if(k.hosted||(physical&&k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){if(k.scan<256)scans[k.scan]=128;if(k.vk<256)keys[k.vk]=true;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=true;if(k.scan==28||k.scan==156)keys[13]=true;}
    if(app.session.title){auto& title=*app.session.title;for(u32 i=0;i<256;++i)title.key_edges[i]=scans[i]&~previous_scans[i];title.number_keys=0;for(u32 i=0;i<9;++i)if(keys[49+i])title.number_keys|=1u<<i;}
    previous_scans=scans;
    const auto sample=gestures.sample(touch_state(),SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
    for(u32 n=0;n<256;++n)keys[n]=keys[n]||sample.keys[n];
    if(auto* b=app.session.battle.get()){b->player_input.movement.touch_mode=sample.motion;b->player_input.movement.touch_x=sample.x;b->player_input.movement.touch_y=sample.y;}
    const u32 raw=sample_controller()|keyboard_keys(keys);
    const u32 held=(raw&~0x80100u)|((raw&0x80000)?256:0);
    return app.tick_once(held,(raw&256)!=0);
}
bool sample_and_tick(){
#if PORT_TITLE == 14
    // A registry restart samples physical input again, before replay input.
    do{if(!sample_and_tick_once())return false;}while(app.repeat_update);
    return true;
#else
    return sample_and_tick_once();
#endif
}
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE int th11_validate_file(unsigned kind,const unsigned char* data,unsigned size){
    if(kind==0){th11::ScoreFile scores;return scores.open(data,size);}
    if(kind==1){th11::Replay replay;return replay.open(data,size);}
    if(kind==2){th11::GameConfig config;return config.open(data,size);}
    return 0;
}
EMSCRIPTEN_KEEPALIVE int th11_save_scores(){return th11::sdl::app.save_scores();}
EMSCRIPTEN_KEEPALIVE int th11_save_replay(unsigned slot,const char* name){return th11::sdl::app.save_replay(slot,name);}
EMSCRIPTEN_KEEPALIVE int th11_initialize(){if(!th11::sdl::app.initialize())return 0;for(auto& k:th11::sdl::keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);th11::sdl::initialize_controller();return 1;}
// Readable while a load is in flight, which is the point: it is the only way to see where a load that never
// returns had got to. The browser harness polls it and shows it while the welcome panel is up.
EMSCRIPTEN_KEEPALIVE const char* th11_last_resource(){return th11::last_resource.c_str();}
#if TH11_DEVELOPMENT_HARNESS
EMSCRIPTEN_KEEPALIVE int th11_tick(unsigned held){return !th11::sdl::running&&th11::sdl::app.tick(held)?1:0;}
EMSCRIPTEN_KEEPALIVE int th11_probe_platform_tick(){return !th11::sdl::running&&th11::sdl::sample_and_tick()?1:0;}
EMSCRIPTEN_KEEPALIVE float th11_probe_player_x(){return th11::sdl::touch_state().x;}
EMSCRIPTEN_KEEPALIVE int th11_probe_touch_motion(){const auto* b=th11::sdl::app.session.battle.get();return b?b->player_input.movement.touch_mode:0;}
EMSCRIPTEN_KEEPALIVE int th11_probe_stage(unsigned stage,int character,int subtype,int difficulty){return th11::sdl::app.restart(stage,character,subtype,difficulty);}
EMSCRIPTEN_KEEPALIVE int th11_probe_finish(){auto& s=th11::sdl::app.session;if(!s.battle)return 0;s.economy.score_units=1234567;s.battle->completion.state.exit=th11::StageExit::Ending;return 1;}
EMSCRIPTEN_KEEPALIVE int th11_probe_complete(){auto& s=th11::sdl::app.session;
#if PORT_TITLE == 14
    if(s.state.phase!=th11::GameSessionPhase::stage||!s.battle||!s.battle->stage_active||s.battle->transitioning||s.battle->pending_stage)return 0;
#endif
    return s.battle&&s.battle->dialogue_stage_complete();}
EMSCRIPTEN_KEEPALIVE int th11_probe_dialogue(int id){auto& app=th11::sdl::app;return app.session.battle&&app.session.battle->start_dialogue(id)&&app.text_events();}
// A verification hook, not a game feature, and it sits with the other probes for that reason: the browser run's
// policy cannot dodge, so "the campaign reaches stage six and the ending" cannot be shown from the browser without
// staying alive. The stage machinery is what is under test there, not the player's dodging.
EMSCRIPTEN_KEEPALIVE int th11_probe_invincibility(unsigned frames){
    auto& s=th11::sdl::app.session;if(!s.battle||!s.battle->player)return 0;
    s.battle->player->state.invincibility.set(int(frames),&s.animations.rate);return 1;
}
EMSCRIPTEN_KEEPALIVE unsigned th11_text_writes(){return th11::sdl::app.fonts.writes;}
EMSCRIPTEN_KEEPALIVE int th11_audio_probe(unsigned command,int value,float x){
    auto& a=th11::sdl::app.audio;
    if(command==0)return a.music(value);
    if(command==1){a.effects.positioned(value,x);a.update();return 1;}
    if(command==2){for(unsigned n=1;n<=56;++n)a.sound_stop(n);a.effects.reset();return a.music(-1);}
    if(command==3){a.fade_music(value);return 1;}
    if(command==4){a.update();return 1;}
    return 0;
}
EMSCRIPTEN_KEEPALIVE const float* th11_audio_samples(){static float pcm[2048];return th11::sdl::app.audio.mix(pcm,1024)?pcm:nullptr;}
#endif
EMSCRIPTEN_KEEPALIVE const char* th11_error(){return th11::sdl::app.error.c_str();}
EMSCRIPTEN_KEEPALIVE unsigned th11_frame(){return th11::sdl::app.session.state.frame;}
EMSCRIPTEN_KEEPALIVE unsigned th11_phase(){return unsigned(th11::sdl::app.session.state.phase);}
// The stage is what tells a browser run apart from the title demo: the attract demo plays a fixed replay, so a
// stage number that changes, and a frame counter that advances inside it, is the evidence that a game is running.
EMSCRIPTEN_KEEPALIVE unsigned th11_stage(){return th11::sdl::app.session.state.stage;}
EMSCRIPTEN_KEEPALIVE unsigned th11_stage_frame(){const auto& s=th11::sdl::app.session.state;return s.frame;}
EMSCRIPTEN_KEEPALIVE int th11_return_title(){return th11::sdl::app.return_to_title()?1:0;}
EMSCRIPTEN_KEEPALIVE int th11_restart(){return th11::sdl::app.restart()?1:0;}
// The title menu's own state. A run that never leaves the title needs to say *where* in the title it is: the
// screen, its substate, the cursor and whether a start was requested are what separate "the keys never arrived"
// from "the keys arrived and the menu did not act on them".
EMSCRIPTEN_KEEPALIVE int th11_title(unsigned kind){const auto& s=th11::sdl::app.session;if(!s.title)return -1;
    switch(kind){case 0:return int(s.title->screen);case 1:return s.title->substate;case 2:return s.title->cursor.selected;
    case 3:return s.title->start_requested?1:0;case 4:return s.title->cursor.count;
#if PORT_TITLE == 14
    case 5:return s.title->unlock_progress;case 6:return s.title->unlock_timeout;
#endif
    default:return -1;}}
// What the game's own input object holds after a tick. The page can show that a key is down and the menu can show
// that it did not move, and those two together still cannot say whether the edge was ever produced in between.
EMSCRIPTEN_KEEPALIVE unsigned th11_input(unsigned kind){const auto& g=th11::sdl::app.session.game_input;
    switch(kind){case 0:return g.held;case 1:return g.pressed;case 2:return g.long_repeat;case 3:return g.released;default:return 0;}}
// Diagnostics for the page's health readout. These report what the allocator actually holds, so a run that
// ends in an out-of-bounds access can be told apart from one that merely grew: a footprint that climbs with
// every stage change is a leak, while a flat footprint at the moment of failure is corruption.
EMSCRIPTEN_KEEPALIVE unsigned th11_heap_bytes(){return unsigned(emscripten_get_heap_size());}
EMSCRIPTEN_KEEPALIVE unsigned th11_dynamic_allocs(){return unsigned(mallinfo().uordblks);}
EMSCRIPTEN_KEEPALIVE int th11_pause(){return th11::sdl::app.pause()?1:0;}
EMSCRIPTEN_KEEPALIVE int th11_resume(){return th11::sdl::app.resume()?1:0;}
EMSCRIPTEN_KEEPALIVE void th11_key(unsigned scan,unsigned down){for(auto& k:th11::sdl::keyboard_map)if(k.scan==scan)k.hosted=down!=0;}
EMSCRIPTEN_KEEPALIVE void th11_keys_clear(){th11::sdl::clear_inputs();}
EMSCRIPTEN_KEEPALIVE void th11_music_enabled(unsigned on){using namespace th11::sdl;app.audio.music_enabled=on!=0;app.audio.refresh_volume();}
EMSCRIPTEN_KEEPALIVE const unsigned* th11_audio_statistics(){return th11::sdl::app.audio.statistics();}
EMSCRIPTEN_KEEPALIVE void th11_loop_pause(unsigned on){using namespace th11::sdl;suspended=on!=0;app.audio.suspend(suspended);previous_frame=-1;cadence.reset();clear_inputs();app.reset_frame_window();}
EMSCRIPTEN_KEEPALIVE void th11_loop_stop(){
    using namespace th11::sdl;running=false;++loop_epoch;previous_frame=-1;cadence.reset();clear_inputs();app.audio.suspend(true);app.reset_frame_window();
#if PORT_TITLE == 14
    app.suspend_boot_worker();
#endif
}
EMSCRIPTEN_KEEPALIVE void th11_audio_close(){th11::sdl::app.audio.close();}
EMSCRIPTEN_KEEPALIVE void th11_loop_start(){
    using namespace th11::sdl;if(running||!app.initialized)return;
    running=true;suspended=false;previous_frame=-1;cadence.reset();clear_inputs();app.audio.suspend(false);app.reset_frame_window();
#if PORT_TITLE == 14
    app.resume_boot_worker();
#endif
#ifdef TH14_WEBGPU_BACKEND
    auto request_frames=th14_webgpu_request_frames;
#elif PORT_TITLE == 14
    auto request_frames=th14_async_request_frames;
#else
    auto request_frames=emscripten_request_animation_frame_loop;
#endif
    request_frames([](double time,void* epoch)->EM_BOOL{
        if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;
        const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
        if(suspended){cadence.reset();return EM_TRUE;}
        const auto ticks=cadence.advance(delta);bool ok=true;app.graphics.backend.defer=true;
        for(unsigned n=0;n<ticks&&ok;++n)ok=sample_and_tick();
        app.graphics.backend.commit();app.graphics.backend.defer=false;
        app.audio.pump();
        if(ticks)th11_browser_frame(ok?1:0,emscripten_get_now()-begin,ticks);
        if(!ok)running=false;return running?EM_TRUE:EM_FALSE;
    },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
EMSCRIPTEN_KEEPALIVE void th11_touch(unsigned type,int id,float x,float y){th11::sdl::touch_event(type,id,x,y);}
EMSCRIPTEN_KEEPALIVE void th11_touch_cancel(){th11::sdl::cancel_touch();}
EMSCRIPTEN_KEEPALIVE void th11_touch_options(unsigned enabled,unsigned mode,float sensitivity,unsigned two_finger,unsigned double_tap){using namespace th11::sdl;gestures.enabled=enabled!=0;gestures.unlimited=mode==1;gestures.sensitivity=std::isfinite(sensitivity)?std::clamp(sensitivity,1.f,3.f):1;gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;if(gestures.set_mode(mode<=3?int(mode):0))if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;if(!enabled)gestures.cancel();}
EMSCRIPTEN_KEEPALIVE void th11_touch_controls(unsigned enabled,unsigned fire,unsigned focus,unsigned bomb,unsigned escape){using namespace th11::sdl;gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
EMSCRIPTEN_KEEPALIVE void th11_touch_stick(float x,float y){using namespace th11::sdl;gestures.stick_x=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;gestures.stick_y=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
// Shared-host ABI (mirrors th10/th20 cpp/sdl/GameHost.cpp). The portable
// eagler-host.mjs drives touch through these names; game entry points above
// stay TH11-specific for the development harness.
__attribute__((export_name("sdl_touch"))) void sdl_touch(unsigned type,int id,float x,float y){th11_touch(type,id,x,y);}
__attribute__((export_name("sdl_touch_cancel"))) void sdl_touch_cancel(){th11_touch_cancel();}
__attribute__((export_name("sdl_touch_options"))) void sdl_touch_options(unsigned on,unsigned free,float speed){using namespace th11::sdl;gestures.enabled=on!=0;gestures.unlimited=free!=0;gestures.sensitivity=std::isfinite(speed)?std::clamp(speed,1.f,3.f):1;if(!on)gestures.cancel();}
__attribute__((export_name("sdl_touch_gestures"))) void sdl_touch_gestures(unsigned two,unsigned taps){using namespace th11::sdl;gestures.two_finger=two!=0;gestures.double_tap=taps!=0;}
__attribute__((export_name("sdl_touch_mode"))) void sdl_touch_mode(unsigned mode){using namespace th11::sdl;if(gestures.set_mode(mode<=3?int(mode):0))if(app.session.battle)app.session.battle->player_input.movement.touch_mode=0;}
__attribute__((export_name("sdl_touch_controls"))) void sdl_touch_controls(unsigned shoot,unsigned slow,unsigned bomb,unsigned escape,float x,float y){using namespace th11::sdl;gestures.controls(shoot!=0,slow!=0,bomb,escape,x,y);}
__attribute__((export_name("sdl_loop_pause"))) void sdl_loop_pause(unsigned on){th11_loop_pause(on);}
__attribute__((export_name("sdl_music_resource_changed"))) void sdl_music_resource_changed(){}
}

