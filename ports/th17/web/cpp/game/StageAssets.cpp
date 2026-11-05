#include "StageAssets.hpp"
namespace th17 {
StageAssets::~StageAssets(){for(auto bank:owned)animations.unload(bank);}
bool StageAssets::fail(const std::string& value){if(error.empty())error=value;return false;}
bool StageAssets::read(const std::string& name,std::vector<u8>& bytes){return source.read(name,bytes)||fail("Missing TH17 stage asset: "+name);}
i32 StageAssets::load_animation(const std::string& name,i32 preferred){
    auto found=loaded.find(name);if(found!=loaded.end())return found->second;
    const auto reuse=[&](const std::unordered_map<std::string,i32>* resources)->i32{if(!resources)return -1;auto at=resources->find(name);if(at==resources->end())return -1;if(!animations.resource(at->second)){fail("Retained stage animation unavailable: "+name);return -1;}loaded.emplace(name,at->second);return at->second;};
    if(common){const i32 bank=reuse(&common->loaded);if(bank>=0)return bank;if(!error.empty())return -1;}
    if(shared){const i32 bank=reuse(shared);if(bank>=0)return bank;if(!error.empty())return -1;}
    std::vector<u8> data;if(!read(name,data))return -1;
    i32 bank=preferred;if(bank<0||animations.resource(bank)){while(animations.resource(next_bank))next_bank++;bank=next_bank++;}
    if(!animations.load(bank,data.data(),data.size())||!animations.name_resource(bank,name)){fail("TH17 animation load failed: "+name+": "+animations.error);return -1;}loaded.emplace(name,bank);owned.push_back(bank);return bank;
}
bool StageAssets::animation(u32 slot,const std::string& name){if(slot<10||slot>=14)return fail("TH17 enemy animation alias outside stage banks");const i32 bank=load_animation(name,slot==10?12:i32(slot)+9);if(bank<0)return false;enemy_banks[slot-8]=bank;return true;}
bool StageAssets::load(i32 stage,i32 selected_character,i32 selected_animal,const std::string& ascii_name){
    if(initialized)return fail("Stage assets already initialized");definition=StageConfiguration::get(stage);if(!definition||u32(selected_character)>=3||u32(selected_animal)>=3)return fail("Invalid TH17 stage loadout");character=selected_character;animal=selected_animal;
    if(ascii_name!="ascii.anm"&&ascii_name!="ascii_960.anm"&&ascii_name!="ascii_1280.anm")return fail("Invalid TH17 numeric font bank");
    const auto load=[&](i32& id,const std::string& name,i32 slot){id=load_animation(name,slot);return id>=0;};
    if(!load(ascii,ascii_name,2)||!animations.sprite_fallback(ascii)||!load(front,"front.anm",5)||!load(text,"text.anm",6)||!load(bullet,"bullet.anm",7)||!load(effect,"effect.anm",8)||!load(beast,"beast.anm",30))return false;
    enemy_banks[0]=bullet;enemy_banks[1]=effect;
    const std::string prefix="pl0"+std::to_string(character),shot_name=prefix+char('a'+animal)+".sht";if(!load(player,prefix+".anm",10))return false;
    std::vector<u8> data;if(!read(shot_name,data)||!shots.open(data.data(),data.size()))return fail("Invalid TH17 shot resource: "+shot_name);
    if(!read(definition->background,data)||!background.open(data.data(),data.size()))return fail("Invalid TH17 background: "+definition->background);
    if(!load(scenery,background.animation_name,9)||!load(logo,definition->logo,11))return false;
    const auto* message=definition->message(character,animal);if(!message||!read(*message,data)||!messages.open(data.data(),data.size()))return fail("Invalid TH17 dialogue resource");
    if(!scripts.load(definition->enemy_script,*this)||!scripts.error.empty()||!error.empty())return fail(scripts.error);
    if(scripts.find_index("main")<0)return fail("TH17 main routine unavailable");initialized=true;return true;
}
BattleResources StageAssets::battle_resources()const noexcept{return {player,bullet,effect,beast,text,enemy_banks};}
DialogueResources StageAssets::dialogue_resources()const noexcept{return {text,front,logo,player,enemy_banks,character};}
SpellVisualResources StageAssets::spell_resources()const{
    SpellVisualResources result;result.ascii=ascii;result.name=text;result.effect=effect;if(!definition)return result;
    const auto bank=[&](i32 index){return u32(index)<enemy_banks.size()?enemy_banks[index]:-1;};
    for(const auto& row:definition->visuals)result.backgrounds.push_back({bank(row.background_bank),row.background_script,row.independent!=0,bank(row.overlay_bank),row.overlay_script});return result;
}
HudResources StageAssets::hud_resources()const noexcept{return {front,ascii,logo,definition?std::array<i32,2>{definition->visuals[0].boss_name,definition->visuals[1].boss_name}:std::array<i32,2>{-1,-1}};}
}
