#include "RunStageFlow.hpp"
namespace th15 {
bool RunStageFlow::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Run stage flow failed":reason;return false;}
RunStageFlow::~RunStageFlow(){if(run.scene())run.scene()->stage_completion={};}
bool RunStageFlow::bind(const StageCamera& view,i32* chapter_request){
 if(!error.empty()||!run.scene()||!driver.driver())return fail("Completion callback requires an initialized run scene");
 camera=view;requested_chapter=chapter_request;
 completion=std::make_unique<RunCompletion>(*run.scene(),progress,driver.driver()->runtime,records,static_cast<RunCompletionServices&>(*this));
 run.scene()->stage_completion=[this](){return completion->complete()||fail(completion->error);};return true;
}
bool RunStageFlow::step(const SessionGameplayInput& frame){
 if(!error.empty())return false;if(!driver.step(frame))return fail(driver.error);
 return !next_requested||advance();
}
bool RunStageFlow::advance(){
 if(!error.empty()||!next_requested||next_stage!=progress.stage||!run.scene()||!driver.driver())return fail("No completed next-stage request to consume");
 const bool auto_focus=driver.driver()->auto_focus;
 run.scene()->stage_completion={};completion.reset();
 exit=std::make_unique<RunStageExit>(run,driver,progress,replay,exit_platform);if(!exit->finish())return fail(exit->error);
 if(!run.construct_next(next_stage,requested_chapter))return fail(run.error);
 if(!initialization.initialize(camera,12))return fail(initialization.error);
 driver.driver()->auto_focus=auto_focus;next_requested=false;next_stage=0;return bind(camera,requested_chapter);
}
bool RunStageFlow::prepare_ending(StageGameplay& scene){return completion_platform.prepare_ending(scene);}
bool RunStageFlow::finish_replay(){return completion_platform.finish_replay();}
bool RunStageFlow::finish_practice(){return completion_platform.finish_practice();}
bool RunStageFlow::queue_next_stage(){if(next_requested)return fail("Duplicate stage completion request");next_requested=true;return true;}
bool RunStageFlow::prepare_next_stage(i32 stage){next_stage=stage;return completion_platform.prepare_next_stage(stage);}
}
