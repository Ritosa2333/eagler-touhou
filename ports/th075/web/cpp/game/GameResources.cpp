#include "GameResources.hpp"
namespace th075 {
namespace {std::string canonical(std::string s){for(auto& c:s)if(c==char(92))c='/';return s;}}
void MemoryResources::install(ResourceEdition edition,const std::string& path,const void* p,std::size_t n){const auto* data=static_cast<const u8*>(p);files[{edition,canonical(path)}]=std::vector<u8>(data,data+n);}
void MemoryResources::release(ResourceEdition edition,const std::string& path){files.erase({edition,canonical(path)});}
bool MemoryResources::contains(ResourceEdition edition,const std::string& path)const {return files.find({edition,canonical(path)})!=files.end();}
bool MemoryResources::read(ResourceEdition edition,const std::string& path,std::vector<u8>& out){auto found=files.find({edition,canonical(path)});if(found==files.end()&&edition==ResourceEdition::Patched)found=files.find({ResourceEdition::Base,canonical(path)});if(found==files.end())return false;out=found->second;return true;}
std::vector<ResourceRequest> character_resources(u8 id,u8 stage){const auto* d=character_definition(id);if(!d)return {};std::vector<ResourceRequest> requests{{d->sounds,d->sound_path()},{ResourceEdition::Patched,d->pattern_path()},{d->sprites,d->sprite_path()},{d->portraits,d->portrait_path()},{ResourceEdition::Patched,d->script_path()}};if(stage)requests.push_back({ResourceEdition::Base,"data/character/"+std::string(d->name)+"/stage"+std::to_string(stage)+".dat"});return requests;}
bool CharacterAssets::load(ResourceStore& store,const CharacterDefinition& definition,u8 palette){
 std::vector<u8> data;const auto read=[&](ResourceEdition edition,const std::string& path){if(store.read(edition,path,data))return true;error="Missing resource: "+path;return false;};error.clear();
 if(!read(definition.sounds,definition.sound_path())||!sounds.load(data.data(),data.size())){if(error.empty())error=sounds.error;return false;}
 if(!read(ResourceEdition::Patched,definition.pattern_path())||!pattern.load(data.data(),data.size())){if(error.empty())error=pattern.error;return false;}
 if(!read(definition.sprites,definition.sprite_path())||!sprites.load(data.data(),data.size(),u8(palette+1))){if(error.empty())error=sprites.error;return false;}
 if(!read(definition.portraits,definition.portrait_path())||!portraits.load(data.data(),data.size())){if(error.empty())error=portraits.error;return false;}
 if(!read(ResourceEdition::Patched,definition.script_path())||!script.load(data.data(),data.size())){if(error.empty())error=script.error;return false;}return true;
}
bool CharacterAssets::load_story(ResourceStore& store,const CharacterDefinition& definition,u8 stage){if(stage==0)return true;const std::string path="data/character/"+std::string(definition.name)+"/stage"+std::to_string(stage)+".dat";std::vector<u8> data;if(!store.read(ResourceEdition::Base,path,data)){error="Missing story artwork: "+path;return false;}if(!story.load(data.data(),data.size())){error=story.error;return false;}return true;}
}
