#pragma once
#include "StageAssets.hpp"
#include "StageScene.hpp"
#include "StageDeformation.hpp"
#include "GameFrame.hpp"
#include "GameInput.hpp"
#include "AnmSceneEffects.hpp"
#include "GameStart.hpp"
#include "PopupManager.hpp"
#include "ReplayState.hpp"
#include "StageDrawState.hpp"
#include "RunStageObjects.hpp"
namespace th17 {
class StageGameplay;
// Only platform presentation and application destinations cross this boundary.
// Enemy scripts, dialogue, spell scenes, stage gates and all combat are owned
// below and use the same scheduler as the priority-16 game callback.
struct StageGameplayServices {
    virtual ~StageGameplayServices()=default;
    virtual bool audio(i32,float,PlayerShots::SoundAction,bool)=0;
    virtual bool popup(const Vec3&,i32,u32)=0;virtual bool trophy(i32)=0;
    virtual bool game_over()=0;virtual bool finish_practice()=0;
    virtual bool record_play_time()=0;virtual bool return_to_title()=0;
    virtual bool dialogue_text(AnmVm&,u32,u32,i32,i32,i32,const std::string&)=0;
    virtual bool spell_title(AnmVm&,const std::string&)=0;
    virtual bool music_control(i32 operation,i32 value)=0;
    virtual bool music_fade(i32 seconds)=0;virtual bool unlock_music(i32)=0;
    virtual bool transition(const SceneTransition&)=0;
    virtual i32 loading_state()const noexcept=0;virtual bool retire_loading()=0;
    virtual bool flush_audio()=0;virtual bool resume_environment(double)=0;
    // Native input activation occurs after pool/player reset and before main
    // ECL creation. A fixture without a replay owner may leave it unbound.
    virtual bool stage_input_ready(StageGameplay&){return true;}
};
struct StageGameplayFrame {BattleFrame battle;};
class StageGameplay {
    struct Services;std::unique_ptr<Services> services;
    StageAssets& assets;AnmManager& animations;AnmEnvironment& environment;
    Rng& game_random;Rng& visual_random;
    GameRun& run;GameRecords& records;SpellRecords& spell_records;StageGameplayServices& platform;
    FrameScheduler scheduler;std::array<FrameCallback,3> callbacks;
    std::unique_ptr<RunStageObjects> owned_objects;RunStageObjects& objects;
    StageGameplayFrame input;bool prepared=false,started=false,suspended=false;
    std::unique_ptr<ReplayStageState> replay_entry;
    HudStartContext hud_context()const noexcept;
    bool fail(const std::string&);i32 update_game();i32 update_background();
    i32 initialize_stage();bool start_stage();bool reset_world();bool activate_world();
    bool message(i32);bool next_stage();bool music(bool boss,bool show_title=true);
    void restore_run(const ReplayRunState&);
public:
    GameBattle& battle;StageScene background;StageDeformation deformation;PopupManager& popups;
    std::unique_ptr<GameFrame> controller;std::unique_ptr<GameProgression> progression;
    std::array<AnmVm*,4> scene_animations{};
    std::array<i32,20> capture_times{};
    StageDrawState background_draw;
    u32& background_flags=background_draw.flags;i32& background_transition_age=background_draw.transition.current;
    bool alternate_music=false;std::string error;
    StageGameplay(StageAssets&,AnmManager&,AnmEnvironment&,Rng& game,Rng& visual,GameRun&,GameRecords&,SpellRecords&,StageGameplayServices&,RunStageObjects* retained=nullptr);
    ~StageGameplay();
    // Replay seeds are installed before loading StageAssets; state is restored
    // after creating the player, matching native stage-factory ordering.
    bool prepare(const StageCamera&,const ReplayStageState* replay=nullptr,const ReplayRunState* retained=nullptr,bool defer_start=false);bool step(const StageGameplayFrame&);
    ReplayRunState capture_run()const noexcept;
    bool presented(double seconds);
    bool suspend_for_transition();bool update_previous_background(float rate);
    RunStageObjects& run_objects()noexcept{return objects;}
    std::unique_ptr<RunStageObjects> take_run_objects()noexcept{return std::move(owned_objects);}
    bool ready()const noexcept{return prepared;}bool active()const noexcept{return started;}
    FrameScheduler& frame_scheduler()noexcept{return scheduler;}
};
}
