#pragma once
#include "StageConfiguration.hpp"
#include "StageResource.hpp"
#include "GameBattle.hpp"
#include <unordered_map>
namespace th17 {
struct AssetSource {virtual ~AssetSource()=default;virtual bool read(const std::string&,std::vector<u8>&)=0;};
// Assets are prepared before activating game callbacks. Resource identifiers
// belong to the portable manager and never refer to executable addresses.
class StageAssets final:private EclResourceProvider {
    AssetSource& source;AnmManager& animations;const StageAssets* common;const std::unordered_map<std::string,i32>* shared;
    std::unordered_map<std::string,i32> loaded;std::vector<i32> owned;i32 next_bank=31;bool initialized=false;
    bool read(const std::string&,std::vector<u8>&)override;bool animation(u32,const std::string&)override;
    i32 load_animation(const std::string&,i32 preferred=-1);bool fail(const std::string&);
public:
    EclProgram scripts;StageResource background;MessageResource messages;ShtResource shots;
    const StageConfiguration* definition=nullptr;i32 character=0,animal=0;
    i32 ascii=-1,text=-1,front=-1,bullet=-1,effect=-1,player=-1,logo=-1,scenery=-1,beast=-1;
    std::array<i32,6> enemy_banks{{-1,-1,-1,-1,-1,-1}};std::string error;
    StageAssets(AssetSource& s,AnmManager& a,const StageAssets* common=nullptr,const std::unordered_map<std::string,i32>* shared=nullptr):source(s),animations(a),common(common),shared(shared){}
    ~StageAssets();StageAssets(const StageAssets&)=delete;StageAssets& operator=(const StageAssets&)=delete;
    bool load(i32 stage,i32 character,i32 animal,const std::string& ascii_name="ascii.anm");
    const std::unordered_map<std::string,i32>& loaded_animations()const noexcept{return loaded;}
    BattleResources battle_resources()const noexcept;
    DialogueResources dialogue_resources()const noexcept;
    SpellVisualResources spell_resources()const;
    HudResources hud_resources()const noexcept;
    bool ready()const noexcept{return initialized;}
};
}
