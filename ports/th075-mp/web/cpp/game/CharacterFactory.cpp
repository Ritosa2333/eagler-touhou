#include "ReimuCharacter.hpp"
#include "MarisaCharacter.hpp"
#include "SakuyaCharacter.hpp"
#include "AliceCharacter.hpp"
#include "PatchouliCharacter.hpp"
#include "YoumuCharacter.hpp"
#include "RemiliaCharacter.hpp"
#include "YuyukoCharacter.hpp"
#include "YukariCharacter.hpp"
#include "SuikaCharacter.hpp"
#include "MeilingCharacter.hpp"
#include "CharacterFactory.hpp"
namespace th075 {
namespace {constexpr ResourceEdition base=ResourceEdition::Base,patched=ResourceEdition::Patched;
const std::array<CharacterDefinition,11> definitions{{
 {CharacterId::Reimu,"reimu","00",647,10,base,base,base,{1,3,5,2,6,8,9,9,9,9}},
 {CharacterId::Marisa,"marisa","01",665,10,base,base,base,{3,2,4,6,0,8,9,9,9,9}},
 {CharacterId::Sakuya,"sakuya","02",637,10,base,base,base,{0,3,1,5,7,8,9,9,9,9}},
 {CharacterId::Alice,"alice","03",685,10,base,patched,base,{1,0,2,4,5,8,9,9,9,9}},
 {CharacterId::Patchouli,"patchouli","04",812,10,base,base,patched,{1,2,3,5,7,8,9,9,9,9}},
 {CharacterId::Youmu,"youmu","05",751,10,base,base,patched,{0,1,4,2,6,8,9,9,9,9}},
 {CharacterId::Remilia,"remilia","06",672,11,base,base,patched,{2,1,0,5,7,8,9,9,9,9}},
 {CharacterId::Yuyuko,"yuyuko","07",540,10,base,base,patched,{5,1,0,2,6,8,9,9,9,9}},
 {CharacterId::Yukari,"yukari","08",606,12,base,base,patched,{6,3,5,1,0,7,9,9,9,9}},
 {CharacterId::Suika,"suika","09",678,10,base,base,base,{2,3,5,4,1,6,0,0,0,0}},
 {CharacterId::Meiling,"meiling","10",612,10,patched,patched,patched,{1,3,5,2,6,8,9,9,9,9}}
}};}
const CharacterDefinition* character_definition(u8 id){return id<definitions.size()?&definitions[id]:nullptr;}
std::string CharacterDefinition::pattern_path()const{return "data/character/"+std::string(name)+"/"+name+".pat";}
std::string CharacterDefinition::script_path()const{return "data/character/"+std::string(name)+"/"+name+".sce";}
std::string CharacterDefinition::sprite_path()const{return "data/character/"+std::string(asset)+".dat";}
std::string CharacterDefinition::portrait_path()const{return "data/character/"+std::string(asset)+"2.dat";}
std::string CharacterDefinition::sound_path()const{return "wave/se"+std::string(asset)+".dat";}
void CharacterController::prepare_story(u8 stage){if(stage<1||stage>10)return;for(const auto& definition:definitions)if(std::strcmp(name(),definition.name)==0){story_sequence().opponent=definition.storyOpponents[stage-1];return;}}
std::unique_ptr<CharacterController> create_character(u8 id){switch(CharacterId(id)){
 case CharacterId::Reimu:return std::make_unique<ReimuCharacter>();
 case CharacterId::Marisa:return std::make_unique<MarisaCharacter>();
 case CharacterId::Sakuya:return std::make_unique<SakuyaCharacter>();
 case CharacterId::Alice:return std::make_unique<AliceCharacter>();
 case CharacterId::Patchouli:return std::make_unique<PatchouliCharacter>();
 case CharacterId::Youmu:return std::make_unique<YoumuCharacter>();
 case CharacterId::Remilia:return std::make_unique<RemiliaCharacter>();
 case CharacterId::Yuyuko:return std::make_unique<YuyukoCharacter>();
 case CharacterId::Yukari:return std::make_unique<YukariCharacter>();
 case CharacterId::Suika:return std::make_unique<SuikaCharacter>();
 case CharacterId::Meiling:return std::make_unique<MeilingCharacter>();
 }return nullptr;}
}
