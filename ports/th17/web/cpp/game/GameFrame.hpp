#pragma once
#include "GameProgression.hpp"
#include "FrameScheduler.hpp"
namespace th17 {
struct SceneTransition {i32 mode=5,frames=60,layer=52;u32 color=0;};
struct GameFrameHost {
    virtual ~GameFrameHost()=default;
    virtual i32 initialize_stage()=0;virtual bool start_stage()=0;
    virtual i32 loading_state()const noexcept=0;virtual bool retire_loading()=0;
    virtual bool return_to_title()=0;virtual bool transition(const SceneTransition&)=0;
    virtual bool pause_stage_music()=0;virtual bool interrupt_stage_visual()=0;
    virtual bool reload_stage_music(i32)=0;virtual bool flush_audio()=0;virtual bool resume_environment(double seconds)=0;
    virtual bool advance_scene_animations()=0;
};
struct GameFrameContext {float rate=1;u32 pressed=0;};
// Priority-16 game callback. World objects retain their original independent
// priorities; this owner controls stage gates, replay-demo exit and run time.
class GameFrame {
    GameRun& run;Hud& hud;ItemScoreState& score;EnemyWorldState& enemies;GameFrameHost& host;
    i32 fail(const char*);
public:
    Timer age;std::array<i32,3> counters{};i32 shutdown_frames=0,global_phase=0,reset_age=0,reset_duration=0;std::string error;
    GameFrame(GameRun& r,Hud& h,ItemScoreState& s,EnemyWorldState& e,GameFrameHost& services):run(r),hud(h),score(s),enemies(e),host(services){age.set(0);}
    i32 update(const GameFrameContext&);
};
}
