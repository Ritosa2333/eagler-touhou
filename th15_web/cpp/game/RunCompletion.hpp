#pragma once
#include "RunSession.hpp"
#include "SessionCompletion.hpp"
namespace th15 {
struct RunCompletionServices {
 virtual ~RunCompletionServices()=default;
 virtual bool prepare_ending(StageGameplay&)=0;virtual bool finish_replay()=0;virtual bool finish_practice()=0;
 // Requests are consumed after the active callback returns, never by deleting
 // its own scene while an ECL or dialogue callback is still on the stack.
 virtual bool queue_next_stage()=0;virtual bool prepare_next_stage(i32)=0;
};
class RunCompletion final:private CompletionServices {
 StageGameplay& scene;SessionState& progress;SessionRuntime& runtime;CompletionRecords& records;RunCompletionServices& platform;bool fail(const std::string&);
 bool clear_notice()override;bool finish_player_options()override;bool end_bomb()override;bool prepare_ending()override;
 bool all_clear_bonus()const override;bool synchronize_clear_score()override;
 bool finish_replay()override;bool finish_practice()override;bool queue_next_scene()override;bool select_stage_resources(i32)override;
public:
 std::string error;
 RunCompletion(StageGameplay& s,SessionState& p,SessionRuntime& d,CompletionRecords& r,RunCompletionServices& h):scene(s),progress(p),runtime(d),records(r),platform(h){}
 bool complete();
};
}
