#include "RunGameplay.hpp"
namespace th17 {
RunGameplay::RunGameplay(AssetSource& f,AnmManager& a,AnmEnvironment& e,Rng& g,Rng& v,GameRun& r,GameRecords& data,SpellRecords& sp,StageGameplayServices& h,const std::unordered_map<std::string,i32>* retained):files(f),animations(a),environment(e),game(g),visual(v),run(r),records(data),spells(sp),host(h),shared(retained){}
RunGameplay::~RunGameplay(){active.gameplay.reset();previous.gameplay.reset();objects.reset();active.assets.reset();previous.assets.reset();common.reset();animations.collect_resources();}
bool RunGameplay::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Run stage construction failed":reason;return false;}
bool RunGameplay::load(i32 stage,const StageCamera& camera,const ReplayStageState* replay){
    if(common||!error.empty())return fail("Run already constructed");
    common=std::make_unique<StageAssets>(files,animations,nullptr,shared);if(!common->load(stage,run.character,run.animal))return fail(common->error);
    run.stage=stage;run.screen=0;active.stage=stage;
    active.gameplay=std::make_unique<StageGameplay>(*common,animations,environment,game,visual,run,records,spells,host);
    objects=active.gameplay->take_run_objects();
    return active.gameplay->prepare(camera,replay)||fail(active.gameplay->error);
}
bool RunGameplay::next(i32 stage,const StageCamera& camera,const ReplayStageState* replay){
    if(!active.gameplay||!objects||!error.empty())return fail("Run unavailable at next stage");
    if(active.stage<1||active.stage>=6||stage!=active.stage+1)return fail("Requested stage is not the next original stage");
    if(previous.gameplay)return fail("Previous stage background is still retained by its compositor");
    // Native 0x4310ad calls 0x40e330 before retaining the run: field spirits
    // are removed and the animal mode pauses until the incoming stage starts.
    if(!run.historical_web_replay){
        if(!objects->battle.items->spirits.clear())return fail(objects->battle.items->spirits.error);
        objects->battle.items->roaring.state.flags|=0x200;
    }
    auto retained=active.gameplay->capture_run();retained.stage=stage;retained.counters={};retained.chapter=retained.chapter_total=retained.chapter_defeated=0;
    if(replay)replay->seed_random(game,visual);
    auto resources=std::make_unique<StageAssets>(files,animations,common.get(),shared);if(!resources->load(stage,run.character,run.animal))return fail(resources->error);
    if(!active.gameplay->suspend_for_transition())return fail(active.gameplay->error);
    previous=std::move(active);run.stage=stage;run.screen=0;
    if(!objects->replace_stage(*resources))return fail(objects->error);
    active.assets=std::move(resources);active.stage=stage;
    active.gameplay=std::make_unique<StageGameplay>(*active.assets,animations,environment,game,visual,run,records,spells,host,objects.get());
    return active.gameplay->prepare(camera,replay,replay?nullptr:&retained,true)||fail(active.gameplay->error);
}
bool RunGameplay::update_previous(float rate){return !previous.gameplay||previous.gameplay->update_previous_background(rate)||fail(previous.gameplay->error);}
bool RunGameplay::release_previous(){previous.gameplay.reset();previous.assets.reset();animations.collect_resources();return error.empty();}
}
