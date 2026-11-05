#pragma once
#include "GameEconomy.hpp"
#include "Timer.hpp"
#include <array>

namespace th11 {
enum class StageExit:u32 {None,NextStage,Results,ReplayEnd,Ending,Title};
struct ClearRecords {
    std::array<std::array<i32,5>,6> clears{};
    // The original indexes these pairs by difficulty * 6 + stage (stage is
    // one-based). Keep the unused slot zero, including when serializing.
    std::array<std::array<std::array<u8,2>,32>,6> stages{};
};
struct StageCompletionMode {
    i32 stage=1,selection=0,control_mode=0,replay_mode=0;
    bool practice=false,replay_practice=false,force_title=false;
    bool all_clear_bonus=false;
};
struct StageCompletionState {
    u32 hud_flags=0;
    Timer result_timer;
    i32 displayed_bonus=0,ending_frames=0,next_stage=0;
    StageExit exit=StageExit::None;
};
struct StageCompletionEffects {
    virtual ~StageCompletionEffects()=default;
    virtual bool stage_result_animation(){return false;}
    virtual void recall_player_options(){}
    virtual void request_stage_exit(StageExit){}
    virtual void start_ending_fade(){}
};
// MSG 21 (41eb15), the common award (41f2b0) and HUD ending clock (41b380).
// No platform or original-address dispatch is part of the live controller.
class StageCompletion {
public:
    StageCompletion(GameEconomy& e,ClearRecords& r,StageCompletionEffects& f,const float* speed)
        :economy(e),records(r),effects(f),rate(speed){}
    StageCompletionMode mode;
    StageCompletionState state;
    bool complete();
    void update();
    bool result_delay_elapsed()const noexcept{return !(state.hud_flags&0x200)||state.result_timer.current>119;}
private:
    GameEconomy& economy;ClearRecords& records;StageCompletionEffects& effects;const float* rate;
    void request(StageExit exit){state.exit=exit;effects.request_stage_exit(exit);}
    void count_clear();
};
}
