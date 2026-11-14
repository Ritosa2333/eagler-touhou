#pragma once
#include "ShotFiles.hpp"
#include "GameResources.hpp"
#include <memory>

#if PORT_TITLE == 14
#include "GameConfig.hpp"
#include "ReplayRngRestoreTh14.hpp"
#endif
namespace th11 {
#if PORT_TITLE == 14
class AnmManager;
struct LoadingTh14;
struct DialogueEffects;
struct SceneCamera;
#if defined(TH14_CPP_STAGE_DECODE)
struct Timer;class StageCppWorkerTh14;
#endif
#endif

struct CoreResources {
    AnmResource ascii;
    AnmResource bullet;
    AnmResource enemy;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    AnmResource effect;
#endif
    AnmResource front;
    AnmResource text;
    AnmResource title;
    AnmResource title_variant;
    // The counts are the shot table's (ShotFiles.hpp): three players with two loadouts on
    // TH12, four with one on TH13, and the ANM files follow the players.
    AnmResource players[player_file_count];
    ShtResource shots[shot_file_count];
#if PORT_TITLE == 16
    AnmResource season_players[5];
    ShtResource season_shots[5];
#endif
    EclProgram defaults;
};

// Resource ownership for the live session. It intentionally contains
// no frame state, input or rendering callbacks, so failed loading cannot leave
// a half-created gameplay world behind.
struct StageResourceEffects {
    virtual ~StageResourceEffects()=default;
    virtual bool prepare(StageResources&){return true;}
#if PORT_TITLE == 14
    virtual bool core_ready()const{return true;}
    virtual bool stage_ready()const{return true;}
    // The retail initial worker creates Player before reading the remaining
    // scene/HUD/ECL resources. Synchronous consumers can keep load_stage.
    virtual bool defer_initial_stage_decode()const{return false;}
    // The60ms core minimum is a lower bound. Dispatch and timer completion
    // are host events, independent of the frame that requested this worker.
    virtual bool begin_stage_worker_wait(bool){return true;}
    virtual bool stage_worker_wait_ready()const{return true;}
    // Quiesce writers while battle, RNG, replay entry and scratch owners are
    // still alive. Resource cancellation/retirement happens separately later.
    virtual void stop_stage_workers()noexcept{}
    virtual bool stage_cpp_worker_enabled()const{return false;}
#if defined(TH14_CPP_STAGE_DECODE)
    virtual bool stage_cpp_decode_enabled()const{return false;}
#endif
    virtual bool stage_memory_worker_enabled()const{return false;}
    virtual ReplayRngRestoreResultTh14 stage_memory_worker(const StageRestoreMemoryTh14&,u32,bool){return ReplayRngRestoreResultTh14::Failed;}
    // Cold-start VM creation belongs to the original priority60 callback.
    virtual bool draw_startup_loading(){return true;}
    virtual bool defer_ending_loading()const{return false;}
    virtual bool begin_ending_loading(){return true;}
    virtual void finish_ending_loading(){}
    // Retail writes text inside priority27 dialogue processing, before normal
    // ANM at29 can hide it. Headless consumers may retain queued requests.
    virtual DialogueEffects* dialogue_text_device(){return nullptr;}
#endif
    virtual void release(StageResources&){}
    virtual bool prepare_animation(AnmResource&){return true;}
    virtual bool prepare_manual_image(AnmResource&,u32,const u8*,u32){return true;}
    virtual void release_animation(AnmResource&){}
    virtual bool read_replay(const std::string&,std::vector<u8>&){return false;}
    // Why the last prepare/prepare_animation failed. The caller reports this in
    // its own error text so a graphics failure is not flattened into a generic
    // "stage graphics preparation".
    virtual const std::string& last_error() const {static const std::string none;return none;}
#if PORT_TITLE == 14
    enum class Phase:u32 {Hud,Scene,Timeline};
    enum class DecodeResult:u32 {Waiting,Ready,Failed};
    enum class CameraReadResult:u32 {Waiting,Ready,Failed};
    virtual CameraReadResult read_stage_camera_word(const SceneCamera&,u32 word,u32 source_offset,u32& value);
    virtual ReplayRngRestoreResultTh14 stage_rng_store_readiness(ReplayRngRestorePointTh14)const{return ReplayRngRestoreResultTh14::Ready;}
    // Acknowledgement follows the real store; readiness alone cannot advance
    // an independent producer to the next instruction.
    virtual bool stage_rng_store_committed(ReplayRngRestorePointTh14){return true;}
    virtual DecodeResult decode_stage_phase(GameResources&,StageResources&,Phase,u32);
    // CPU work can already hold this owner before the first GPU request.
    virtual void cancel_stage_decode(StageResources&){}
    virtual bool prepare_stage_phase(StageResources& stage,Phase phase){
        if(phase==Phase::Hud)return prepare_animation(stage.logo);
        if(phase==Phase::Scene)return prepare_animation(stage.background);
        if(!prepare_animation(stage.enemies))return false;
        for(u32 i=0;i<stage.ecl_animations.size();++i)
            if(!stage.ecl_animation_names[i].empty()&&!prepare_animation(stage.ecl_animations[i]))return false;
        return true;
    }
    virtual bool stage_phase_ready(Phase)const{return true;}
    virtual void cancel_stage_phases(StageResources& stage,u32){release(stage);}
    virtual void finish_stage_phases(StageResources&,u32){}
    // Retail456133 begins the replay globals copy after the three RNG stores.
    virtual ReplayRngRestoreResultTh14 stage_replay_snapshot_readiness()const{return ReplayRngRestoreResultTh14::Ready;}
    virtual bool stage_replay_snapshot_committed(){return true;}
    // Retail controller retirement writes scores before dispatching its
    // replacement worker. A headless consumer may declare file I/O absent.
    // Appended to retain all existing virtual method slots and object sizes.
    virtual bool write_stage_score(const std::vector<u8>&){return true;}
    // Appended: retain all previous StageResourceEffects virtual slots.
    virtual bool initial_cpp_worker_enabled()const{return false;}
    virtual bool begin_initial_cpp_loading(){return true;}
#if defined(TH14_CPP_STAGE_DECODE)
    // Appended. Called on the actual resource worker after Bomb/Spell;
    // providers must perform the music/FPS/clock tail, not report ready early.
    virtual bool finish_cpp_stage_tail(u32,u32,Timer&,const float*,StageCppWorkerTh14&){return false;}
#endif
#endif
};
class GameSessionResources {
public:
    CoreResources core;
    // Stable addresses are required by live ANM VMs during the native
    // overlap between outgoing and incoming stage backgrounds.
    std::unique_ptr<StageResources> stage=std::make_unique<StageResources>();
    std::unique_ptr<StageResources> previous_stage;
    StageResourceEffects* effects=nullptr;
#if PORT_TITLE == 14
    AnmManager* animation_manager=nullptr;
    LoadingTh14* loading_owner=nullptr;
    u32 ascii_display_width=
#if defined(__EMSCRIPTEN__)
        GameConfig::browser_render_width;
#else
        640;
#endif
    const char* ascii_anm_name()const noexcept{return GameConfig::ascii_anm_for_width(ascii_display_width);}
#endif
    u32 stage_number=0;
    std::string error;

    bool load_core(GameResources& source);
#if PORT_TITLE == 14
    bool prepare_core();
    bool load_core_job(u32 index);
#if (defined(TH14_WEBGPU_BACKEND) && defined(TH14_ARCHIVE_WORKER)) || defined(TH14_CPP_BOOT_WORKER)
    bool load_core_bootstrap();
#endif
#if defined(TH14_CPP_BOOT_WORKER)
    bool complete_cpp_core_bootstrap();
    bool complete_cpp_title_resources();
#endif
    bool select_player(u32 character,u32 subtype);
    bool load_selected_player(u32 character,u32 subtype);
    bool load_selected_player_from(GameResources&,u32 character,u32 subtype);
    void cancel_selected_player()noexcept{selected_player_pending=-1;}
    bool prepare_stage();
    using PhaseResult=StageResourceEffects::DecodeResult;
    bool begin_stage_loading(u32 number);
    PhaseResult advance_stage_phase(GameResources&,StageResourceEffects::Phase,u32 dialogue_selection);
    bool publish_stage_scene();
    void finish_stage_parts();
    // Call after Battle/ANM retirement, before replacing resource ownership.
    void cancel_stage_loading();
    StageResources* loading_bundle()const noexcept{return incoming_stage;}
#endif
    bool load_stage(GameResources& source,u32 number);
    void retire_previous();
private:
    GameResources* core_source=nullptr;
#if PORT_TITLE == 14
    bool core_deferred=false;
    u16 core_loaded_jobs=0;
    i32 selected_player_pending=-1;
    std::unique_ptr<StageResources> pending_stage;
    StageResources* incoming_stage=nullptr;
    u32 pending_number=0,decoded_phases=0,requested_phases=0,ready_phases=0;
#endif
};

}
