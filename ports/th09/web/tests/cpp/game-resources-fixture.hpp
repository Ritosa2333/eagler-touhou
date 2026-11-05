#pragma once
#include "../../cpp/game/GameResources.hpp"
#include <set>
namespace game_resources_test {
using namespace th09;
struct Fixture:ResourceReader,ResourceTextures {
    std::string name;std::vector<u8> bytes;std::set<u32> live;u32 next_handle=1,created=0,destroyed=0,fail_after=0xffffffff;
    Rng random;AnmExecutor executor{random};GameResources resources{*this,*this,executor};AnmVm vm;
    bool read(const char* n,std::vector<u8>& b)override{if(name!=n)return false;b=bytes;return true;}
    TextureAllocation create(const AnmTextureSource& s,const u8*,u32)override{if(created==fail_after)return {};const u32 handle=next_handle++;live.insert(handle);++created;return {handle,s.width,s.height};}
    void destroy(u32 handle)override{live.erase(handle);++destroyed;}
    bool load(const char* n,const u8* data,u32 size){name=n;bytes.assign(data,data+size);return resources.load(AnimationFile::menu,n);}
};
}
