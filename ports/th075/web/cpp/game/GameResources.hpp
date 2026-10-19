#pragma once
#include "CharacterFactory.hpp"
#include "BitmapData.hpp"
#include "FighterScript.hpp"
#include "SoundPack.hpp"
#include "EffectPattern.hpp"
#include <map>
namespace th075 {
struct ResourceStore {
 virtual ~ResourceStore()=default;
 virtual bool read(ResourceEdition,const std::string&,std::vector<u8>&)=0;
};
// Files are resolved by edition before decoding. The web runtime can fetch a
// scene's declared resource list asynchronously, then enter it synchronously.
class MemoryResources final:public ResourceStore {
 std::map<std::pair<ResourceEdition,std::string>,std::vector<u8>> files;
public:
 void install(ResourceEdition,const std::string&,const void*,std::size_t);
 void release(ResourceEdition,const std::string&);
 bool read(ResourceEdition,const std::string&,std::vector<u8>&)override;
 bool contains(ResourceEdition,const std::string&)const;
};
struct ResourceRequest {ResourceEdition edition;std::string path;};
std::vector<ResourceRequest> character_resources(u8 character,u8 stage=0);
struct CharacterAssets {
 FighterPattern pattern;FighterScript script;SoundPack sounds;TexturePack sprites,portraits,story;
 std::string error;
 bool load(ResourceStore&,const CharacterDefinition&,u8 palette);
 bool load_story(ResourceStore&,const CharacterDefinition&,u8 stage);
};
}
