#include "RunGameplay.hpp"
namespace th15 {
bool RunGameplay::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Run scene construction failed":reason;return false;}
bool RunGameplay::load(u32 stage,i32 character,const StageCamera& camera,i32* requested){
 if(!construct(stage,character,requested))return false;if(!active.gameplay->prepare(camera))return fail(active.gameplay->error);return true;
}
bool RunGameplay::construct(u32 stage,i32 character,i32* requested){
 if(common||!error.empty())return fail("Run scenes already initialized");
 common=std::make_unique<StageAssets>(files,animations,nullptr,shared_assets);if(!common->load(stage,character,practice&&practice->active?&practice->run:nullptr))return fail(common->error);
 progress.stage=stage;progress.character=character;
 objects=std::make_unique<RunStageObjects>(*common,animations,environment,game,visual,progress,platform);
 objects->battle.player->life.practice=practice;
 objects->battle.enemy_world.practice=practice;
 active.gameplay=std::make_unique<StageGameplay>(*common,animations,environment,game,visual,progress,platform,requested,objects.get(),shared_scheduler);
 active.stage=stage;return true;
}
bool RunGameplay::next(u32 stage,const StageCamera& camera,i32* requested){
 if(!construct_next(stage,requested))return false;if(!active.gameplay->prepare(camera))return fail(active.gameplay->error);return true;
}
bool RunGameplay::construct_next(u32 stage,i32* requested){
 if((!active.gameplay&&!previous.gameplay)||!objects||!error.empty())return fail("Run scene unavailable during next-stage construction");
 const u32 from=active.gameplay?active.stage:previous.stage;
 if(from<1||from>=6||stage!=from+1)return fail("Normal stage transition is not the next original stage");
 if(active.gameplay&&previous.gameplay)return fail("Previous background must finish before another stage transition");
 auto next_assets=std::make_unique<StageAssets>(files,animations,common.get(),shared_assets);
 if(!next_assets->load(stage,progress.character))return fail(next_assets->error);
 if(active.gameplay){if(!active.gameplay->suspend_for_transition())return fail(active.gameplay->error);previous=std::move(active);}
 progress.stage=stage;progress.new_run=false;
 if(!objects->replace_stage(*next_assets))return fail(objects->error);
 active.assets=std::move(next_assets);active.stage=stage;
 active.gameplay=std::make_unique<StageGameplay>(*active.assets,animations,environment,game,visual,progress,platform,requested,objects.get(),shared_scheduler);
 return true;
}
bool RunGameplay::carry_current_background(){
 if(!active.gameplay||previous.gameplay||!objects||!error.empty())return fail("Background cannot transfer before previous stage release");
 previous=std::move(active);return true;
}
}
