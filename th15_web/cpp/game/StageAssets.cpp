#include "StageAssets.hpp"
namespace th15 {
StageAssets::~StageAssets(){for(const auto& name:animation_names){auto found=loaded.find(name);if(found!=loaded.end())animations.unload(found->second);}}
bool StageAssets::fail(const std::string& message){if(error.empty())error=message;return false;}
bool StageAssets::read(const std::string& name,std::vector<u8>& data){if(!source.read(name,data))return fail("Missing stage asset: "+name);return true;}
i32 StageAssets::load_animation(const std::string& name){auto known=loaded.find(name);if(known!=loaded.end())return known->second;
 if(common&&(name=="ascii.anm"||name=="text.anm"||name=="front.anm"||name=="bullet.anm"||name=="effect.anm"||name=="pl0"+std::to_string(character)+".anm")){
  if(common->character!=character){fail("Shared run assets differ from character selection");return -1;}auto shared=common->loaded.find(name);if(shared==common->loaded.end()||!animations.resource(shared->second)){fail("Shared run animation unavailable: "+name);return -1;}return shared->second;
 }
 if(shared){auto found=shared->find(name);if(found!=shared->end()){if(!animations.resource(found->second)){fail("Application animation unavailable: "+name);return -1;}loaded.emplace(name,found->second);return found->second;}}
 std::vector<u8> data;if(!read(name,data))return -1;while(animations.resource(next_bank))next_bank++;const i32 bank=next_bank++;if(!animations.load(bank,data.data(),data.size())){fail(name+": "+animations.error);return -1;}animations.name_resource(bank,name);loaded.emplace(name,bank);animation_names.push_back(name);return bank;}
bool StageAssets::animation(u32 slot,const std::string& name){if(slot<10||slot>=14)return fail("ECL animation alias outside stage bank range");const i32 bank=load_animation(name);if(bank<0)return false;enemy_banks[slot-8]=bank;return true;}
bool StageAssets::load(u32 stage,i32 selected_character,const PracticeConfig* practice_config){
    if(initialized)return fail("Stage assets already initialized");definition=stage_definition(stage);if(!definition||selected_character<0||selected_character>=4)return fail("Invalid stage or character");character=selected_character;
    if(practice_config&&(!practice_config->valid()||practice_config->stage+1!=i32(stage)))return fail("Practice configuration differs from stage assets");
    auto load=[&](i32& id,const char* name){id=load_animation(name);return id>=0;};
    if(!load(ascii,"ascii.anm")||!animations.sprite_fallback(ascii)||!load(text,"text.anm")||!load(front,"front.anm")||!load(bullet,"bullet.anm")||!load(effect,"effect.anm"))return false;enemy_banks[0]=bullet;enemy_banks[1]=effect;
    const std::string prefix="pl0"+std::to_string(character);player=load_animation(prefix+".anm");if(player<0)return false;
    std::vector<u8> data;if(!read(prefix+".sht",data)||!shots.open(data.data(),data.size()))return fail("Invalid player shot resource: "+prefix);
    if(!read(definition->background,data)||!background.open(data.data(),data.size()))return fail("Invalid stage background: "+std::string(definition->background));
    scenery=load_animation(background.animation_name);logo=load_animation(definition->logo);if(scenery<0||logo<0)return false;
    if(!read(definition->dialogue[u32(character)],data)||!messages.open(data.data(),data.size()))return fail("Invalid stage dialogue: "+std::string(definition->dialogue[u32(character)]));
    if(!scripts.load(definition->script,*this)||!scripts.error.empty()||!error.empty())return fail(scripts.error);
    // After decoded ECL and original directory verification, before any main
    // routine/context is bound. Translated whole-file CRCs are irrelevant;
    // the source instruction-site inventory remains mandatory.
    if(practice_config&&practice_config->mode==1){
        std::string reason;if(!patch_practice_program(scripts,*practice_config,practice_effects,reason))return fail(reason);
        practice=*practice_config;practice_active=true;
    }
    if(scripts.find_index("main")<0)return fail("Stage main routine unavailable");initialized=true;return true;
}
DialogueResources StageAssets::dialogue_resources()const noexcept{return definition?definition->dialogue_resources(character,text,player,logo,front,enemy_banks):DialogueResources{};}
SpellVisualResources StageAssets::spell_resources(i32 age)const noexcept{return definition?definition->spell_visuals(age,ascii,text,effect,enemy_banks):SpellVisualResources{};}
}
