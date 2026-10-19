#pragma once
#include "RunInitialization.hpp"
#include "RunCompletion.hpp"
#include "RunStageExit.hpp"
namespace th15 {
// Own the actual completion callback and consume its stage-factory request only
// after the complete frame traversal. Title/ending/replay-result destinations
// remain explicit services for the outer application scene manager.
class RunStageFlow final:private RunCompletionServices {
 RunGameplay& run;RunSession& driver;SessionState& progress;RecordStore& records;
 RunInitialization& initialization;SessionReplay& replay;RunCompletionServices& completion_platform;RunStageExitServices& exit_platform;
 std::unique_ptr<RunCompletion> completion;std::unique_ptr<RunStageExit> exit;
 StageCamera camera;i32* requested_chapter=nullptr;bool next_requested=false;i32 next_stage=0;bool fail(const std::string&);
 bool prepare_ending(StageGameplay&)override;bool finish_replay()override;bool finish_practice()override;
 bool queue_next_stage()override;bool prepare_next_stage(i32)override;
public:
 std::string error;
 RunStageFlow(RunGameplay& r,RunSession& d,SessionState& p,RecordStore& v,RunInitialization& i,SessionReplay& q,RunCompletionServices& c,RunStageExitServices& h):run(r),driver(d),progress(p),records(v),initialization(i),replay(q),completion_platform(c),exit_platform(h){}
 ~RunStageFlow();
 bool bind(const StageCamera&,i32* chapter_request=nullptr);
 bool step(const SessionGameplayInput&);bool advance();
 bool pending()const noexcept{return next_requested;}i32 pending_stage()const noexcept{return next_stage;}
 const SessionExitPresentation* exit_presentation()const noexcept{return exit?&exit->presentation:nullptr;}
};
}
