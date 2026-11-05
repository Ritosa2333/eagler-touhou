#pragma once
#include "StageDefinition.hpp"
#include "StageResource.hpp"
#include "AnmManager.hpp"
#include "EclResource.hpp"
#include "ShtResource.hpp"
#include "PracticeConfig.hpp"
#include <unordered_map>
namespace th15 {
struct AssetSource {virtual ~AssetSource()=default;virtual bool read(const std::string&,std::vector<u8>&)=0;};
// Load a scene before entering its frame loop. File names resolve to logical
// resources; ECL aliases never become native addresses or device objects.
class StageAssets final:private EclResourceProvider {
    AssetSource& source;AnmManager& animations;const StageAssets* common=nullptr;const std::unordered_map<std::string,i32>* shared=nullptr;std::unordered_map<std::string,i32> loaded;
    i32 next_bank=1;bool initialized=false;
    bool read(const std::string&,std::vector<u8>&)override;
    bool animation(u32 slot,const std::string&)override;
    i32 load_animation(const std::string&);bool fail(const std::string&);
public:
    EclProgram scripts;StageResource background;MessageProgram messages;ShtResource shots;
    const StageDefinition* definition=nullptr;i32 character=0;
    i32 ascii=-1,text=-1,front=-1,bullet=-1,effect=-1,player=-1,logo=-1,scenery=-1;
    std::array<i32,6> enemy_banks{{-1,-1,-1,-1,-1,-1}};std::vector<std::string> animation_names;std::string error;
    // Optional source hook; no menu/launcher implicitly enables it.
    bool practice_active=false;PracticeConfig practice;PracticePatchEffects practice_effects;
    StageAssets(AssetSource& source,AnmManager& animations,const StageAssets* common=nullptr,const std::unordered_map<std::string,i32>* shared=nullptr):source(source),animations(animations),common(common),shared(shared){}
    ~StageAssets();
    StageAssets(const StageAssets&)=delete;StageAssets& operator=(const StageAssets&)=delete;
    bool load(u32 stage,i32 character,const PracticeConfig* practice_config=nullptr);
    const std::unordered_map<std::string,i32>& loaded_animations()const noexcept{return loaded;}
    DialogueResources dialogue_resources()const noexcept;
    SpellVisualResources spell_resources(i32 age)const noexcept;
};
}
