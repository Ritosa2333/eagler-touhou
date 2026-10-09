#pragma once
#include "StageGameplay.hpp"
namespace th15 {
// Run and scene ownership are separate: an advancing stage retains the player,
// bullet/laser/item pools, HUD, popups and effect hooks. The preceding background
// stays available to the compositor until its transition capture is complete.
class RunGameplay {
 AssetSource& files;AnmManager& animations;AnmEnvironment& environment;Rng& game;Rng& visual;SessionState& progress;StageGameplayServices& platform;FrameScheduler* shared_scheduler;const std::unordered_map<std::string,i32>* shared_assets;
 std::unique_ptr<StageAssets> common;
 std::unique_ptr<RunStageObjects> objects;
 struct Scene {std::unique_ptr<StageAssets> assets;std::unique_ptr<StageGameplay> gameplay;u32 stage=0;};
 Scene previous,active;bool fail(const std::string&);
public:
 PracticeState* practice=nullptr;
 std::string error;
 RunGameplay(AssetSource& f,AnmManager& a,AnmEnvironment& e,Rng& g,Rng& v,SessionState& p,StageGameplayServices& h,FrameScheduler* scheduler=nullptr,const std::unordered_map<std::string,i32>* shared=nullptr):files(f),animations(a),environment(e),game(g),visual(v),progress(p),platform(h),shared_scheduler(scheduler),shared_assets(shared){}
 bool load(u32 stage,i32 character,const StageCamera&,i32* requested_chapter=nullptr);
 bool next(u32 stage,const StageCamera&,i32* requested_chapter=nullptr);
 bool construct(u32 stage,i32 character,i32* requested_chapter=nullptr);
 bool construct_next(u32 stage,i32* requested_chapter=nullptr);
 bool carry_current_background();
 void release_previous(){previous.gameplay.reset();previous.assets.reset();animations.collect_resources();}
 StageGameplay* scene()const noexcept{return active.gameplay.get();}
 const StageAssets* scene_assets()const noexcept{return active.assets?active.assets.get():active.gameplay?common.get():nullptr;}
 StageGameplay* previous_scene()const noexcept{return previous.gameplay.get();}
 RunStageObjects* run_objects()const noexcept{return objects.get();}
};
}
