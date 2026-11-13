#pragma once

#include "GameSessionResources.hpp"
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "GameEconomy.hpp"
#include "GameBattle.hpp"
#include "SceneCompositor.hpp"
#include "ReplayRecorder.hpp"
#include "GameInput.hpp"
#include "ScoreFile.hpp"
#include "GameConfig.hpp"
#include "TitleMenu.hpp"
#include "PauseMenu.hpp"
#include "Ending.hpp"
#include "FontBitmapTh14.hpp"
#include <string>
#include <memory>
#if PORT_TITLE == 14
#include <chrono>
#include "SessionFailureScopeTh14.hpp"
#endif

namespace th11 {
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
class StageCppWorkerTh14;
#if defined(TH14_CPP_STAGE_DECODE)
class StageCppDecodeTh14;
#endif
#endif

// High-level lifetime and frame boundary shared by the native and WebAssembly
// front ends.  Managers that implement bullets, enemies and players are
// attached at the battle boundary; this class owns the deterministic timing
// and resource transition so both front ends observe the same order.
enum class GameSessionPhase : u8 { title, stage, paused, game_over, finished, ending
#if PORT_TITLE == 14
    ,loading
#endif
};

struct GameSessionInput {
    u32 held=0;
    u32 pressed=0;
    u32 released=0;
    bool pause=false;
    bool confirm=false;
    bool cancel=false;
    float recording_fps=60;
#if PORT_TITLE == 14
    u32 dialogue_skip_frames=0,dialogue_fire_frames=0;
#endif
};

struct GameSessionState {
    GameSessionPhase phase=GameSessionPhase::title;
    u32 frame=0;
    u32 stage=0;
    i32 character=0;
    i32 subtype=0;
    i32 difficulty=1;
    bool focus=false;
    bool replay=false;
    bool demo=false;
    bool practice=false;
#if PORT_TITLE == 14
    i32 spell_practice_id=-1;
#endif
};

class GameSession final {
public:
#if PORT_TITLE == 14
    explicit GameSession(u32 initial_ticks=0);
    // The platform's cold loader constructs the score owner after file8.
    // Direct stage/session callers retain the ordinary eager defaults.
    GameSession(u32 initial_ticks,bool initialize_score_defaults);
    void initialize_score_defaults();
    ~GameSession();
    void stop_stage_workers()noexcept;
#if defined(TH14_CPP_STAGE_WORKER)
    void stop_cpp_stage_worker()noexcept;
    bool start_cpp_stage_worker();
    ReplayRngRestoreResultTh14 stage_cpp_worker_result()const noexcept;
    bool stage_cpp_worker_joined()const noexcept;
    // Presence is distinct from a Failed result, including an absent worker.
    bool has_cpp_stage_worker()const noexcept{return bool(stage_cpp_worker);}
#if defined(TH14_CPP_STAGE_DECODE)
    bool stage_loading_callback_busy()const noexcept;
    u32 stage_loading_finished_word()const noexcept;
    bool stage_loading_callback_allows_close()const noexcept;
#if defined(TH14_WORKER_RETURN_OBSERVER)
    bool observes_stage_worker(const StageCppWorkerTh14* value)const noexcept{return value==stage_cpp_worker.get();}
#endif
    StageResourceEffects::DecodeResult cpp_stage_decode(StageResources&,StageResourceEffects::Phase,u32);
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    bool cpp_stage_pending_file(AnmResource*&,u16&)const noexcept;
    StageResourceEffects::DecodeResult cpp_stage_upload_offer(StageResources*&,StageResourceEffects::Phase&,AnmResource*&,u16&)const noexcept;
    bool cpp_stage_complete_file(AnmResource&,u16)noexcept;
#endif
#endif
#endif
    void shutdown()noexcept;
#else
    GameSession();
#endif
    GameSessionResources resources;
    AnmManager animations;
#if PORT_TITLE==14
    LoadingTh14 shared_loading;
    FontBitmapTh14 font_bitmap;
#endif
    SceneCompositor compositor{animations};
    GameEconomy economy;
    SpellRecords spell_records;
    ClearRecords clear_records;
    ScoreFile scores;
    GameConfig config;
    Replay playback;
    ReplayRecorder recording;
    GameInput game_input;
    u64 recording_timestamp=0;
    std::unique_ptr<GameBattle> battle;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    std::unique_ptr<ControlStateTh14> th14_control_owner;
#endif
    std::unique_ptr<TitleMenu> title;
    std::unique_ptr<PauseMenu> pause_menu;
#if PORT_TITLE == 14
    // Only the existing help controller is used; no pause/game-over entry.
    std::unique_ptr<PauseMenu> title_manual;
#endif
    std::unique_ptr<Ending> ending;
    std::unique_ptr<AsciiText> title_ascii;
    ScreenFades title_fades;
    TitleSelection menu_selection;
    bool interactive=false;
    GameSessionState state;
    std::string error;

    bool begin(GameResources& source, u32 stage=1, i32 character=0,
               i32 subtype=0, i32 difficulty=1, bool demo=false, bool replay=false,bool practice=false);
    bool open_title(GameResources&,bool first=false,TitleScreen screen=TitleScreen::Main);
    bool begin_replay(GameResources&,const u8*,u32,u32 stage=0,bool demo=false);
    bool save_replay(const char* name,std::vector<u8>& output,bool terminal=true,float slowdown=0);
    bool load_scores(const u8*,u32);
    bool save_scores(std::vector<u8>&);
    const GameConfig& persistent_config()const noexcept{
#if PORT_TITLE == 14
        if(replay_user_config)return *replay_user_config;
#endif
        return config;
    }
    bool initialize_compositor(){
#if PORT_TITLE == 14
        const auto& display=persistent_config();
        return compositor.initialize(resources.core.text,display.render_width(),display.render_height());
#else
        return compositor.initialize(resources.core.text);
#endif
    }
    void reset_compositor_cameras(bool restricted){
#if PORT_TITLE == 14
        const auto& display=persistent_config();
        compositor.reset_cameras(restricted,display.render_width(),display.render_height());
#else
        compositor.reset_cameras(restricted);
#endif
    }
    bool update(const GameSessionInput& input);
#if PORT_TITLE == 14
    // Main has already dispatched Overlay8 and acquired initial Control data.
    bool update_after_initial_overlay(const GameSessionInput& input);
#endif
#if PORT_TITLE == 14
    // Retail Supervisor samples41c800 after rendering while SpellManager lives.
    bool sample_spell_clock(double now);
    std::function<u64()> stage_load_clock=[](){return u64(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());};
    // An external scheduler can supply readiness at the real VM call/return.
    AnmManager::VmBoundaryCallbackTh14 stage_load_vm_boundary;
    StageObjectVmBoundaryTh14 stage_load_object_boundary;
    StageCameraWordBoundaryTh14 stage_load_camera_word_boundary;
    std::function<bool(const SceneCamera&,u32)> stage_load_perspective_boundary;
    u32 stage_load_host_flags=0;
    void request_host_close(){__atomic_store_n(&stage_load_host_flags,(__atomic_load_n(&stage_load_host_flags,__ATOMIC_RELAXED)&~0x100u)|0x80u,__ATOMIC_RELEASE);}
#endif
    bool pause();
    bool resume();
    bool return_to_title();
    bool start_stage(GameResources& source, u32 stage);
#if PORT_TITLE == 14
#if defined(TH14_CPP_STAGE_WORKER) && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    bool construct_initial_player();
    bool prepare_initial_pause();
#endif
    bool prepare_initial_player();
    bool prepare_initial_scene(bool complete);
    bool finish_loading(bool defer_initial_control=false);
#endif
    bool draw(AnmRenderer& renderer,AsciiText* overlay=nullptr);
    bool ready() const noexcept { return error.empty() && state.stage!=0
#if PORT_TITLE == 14
        &&state.phase!=GameSessionPhase::loading
#endif
        ; }
    bool battle_callbacks_attached() const noexcept { return battle_attached; }
#if PORT_TITLE == 14
    // Main may observe Loading and this Main-owned attachment flag while
    // an initial worker constructs the private battle. Do not inspect its
    // unique_ptr, replay cursor, events, or initialization fields until join.
    bool initial_battle_private()const noexcept{
        return state.phase==GameSessionPhase::loading&&!battle_attached;
    }
#endif
    const char* phase_name() const noexcept;
private:
#if PORT_TITLE == 14
    bool update_impl(const GameSessionInput&,bool overlay_updated_by_host);
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    std::shared_ptr<StageCppWorkerTh14> stage_cpp_worker;
#if defined(TH14_CPP_STAGE_DECODE)
    std::shared_ptr<StageCppDecodeTh14> stage_cpp_decode;
    StageResources* stage_cpp_decode_owner=nullptr;
#endif
#endif
    bool battle_attached=false;
#if PORT_TITLE == 14
    u32 transition_type0=0;
    bool demo_title_pending=false;
    bool pending_initial_control_activation=false;
    bool pending_core_title=false,pending_core_first=false;
    TitleScreen pending_core_screen=TitleScreen::Main;
    bool activate_title(GameResources&,bool,TitleScreen);
    bool activate_stage();
    bool prepare_initial_entry(ReplayStageState&);
    std::unique_ptr<GameConfig> replay_user_config;
#endif
    u32 last_held=0;
    bool last_pause=false;
    bool recording_active=false;
    u32 recording_seed=0;
    bool record_stage_entry(bool initial);
    bool open_result(bool completed);
    bool restart_run(bool continuation);
#if PORT_TITLE == 14
    bool continue_run_th14();
#endif
    bool open_ending();
    bool open_results();
    bool completed_run=false;
    i32 completed_score=0,completed_continues=0;
    u32 initial_stage=1;
    i32 next_continues=0;
#if PORT_TITLE == 14
    // Initial loader constructs this owner before ECL. Main borrows it while
    // paused and returns it here on resume; stage handoffs retain its lifetime.
    std::unique_ptr<PauseMenu> idle_pause_menu;
#endif
    float paused_rate=1;
    GameInput menu_input;
    u32 title_idle=0,demo_index=0;
    i32 last_replay_file=0;
    GameResources* source=nullptr;
    bool fail(const char* message) {
#if PORT_TITLE == 14
        if(SessionFailureScopeTh14::report(this,message))return false;
#endif
        error=message?message:"session error";return false;
    }
    void reset_state(bool retain_transition=false) noexcept;
};

}
