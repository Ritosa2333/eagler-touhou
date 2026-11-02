#pragma once
#include "StageGameplay.hpp"
namespace th17 {
// Stage-specific STD/ECL/MSG owners may overlap for the original 30-frame
// transition. Combat pools and effect hooks belong to the entire run.
class RunGameplay {
    AssetSource& files;AnmManager& animations;AnmEnvironment& environment;Rng& game;Rng& visual;
    GameRun& run;GameRecords& records;SpellRecords& spells;StageGameplayServices& host;
    const std::unordered_map<std::string,i32>* shared;
    std::unique_ptr<StageAssets> common;std::unique_ptr<RunStageObjects> objects;
    struct Scene {std::unique_ptr<StageAssets> assets;std::unique_ptr<StageGameplay> gameplay;i32 stage=0;};
    Scene previous,active;bool fail(const std::string&);
public:
    std::string error;
    RunGameplay(AssetSource&,AnmManager&,AnmEnvironment&,Rng&,Rng&,GameRun&,GameRecords&,SpellRecords&,StageGameplayServices&,const std::unordered_map<std::string,i32>* shared=nullptr);
    ~RunGameplay();
    bool load(i32 stage,const StageCamera&,const ReplayStageState* replay=nullptr);
    bool next(i32 stage,const StageCamera&,const ReplayStageState* replay=nullptr);
    bool update_previous(float rate);bool release_previous();
    StageGameplay* scene()const noexcept{return active.gameplay.get();}
    StageGameplay* previous_scene()const noexcept{return previous.gameplay.get();}
    StageAssets* scene_assets()const noexcept{return active.assets?active.assets.get():common.get();}
    RunStageObjects* run_objects()const noexcept{return objects.get();}
};
}
