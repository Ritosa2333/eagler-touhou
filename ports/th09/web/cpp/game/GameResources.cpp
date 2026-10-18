#include "GameResources.hpp"
namespace th09 {
namespace {
#include "ResourceData.inc"
}
const CharacterResources* character_resources(u32 n)noexcept{return n<16?&characters[n]:nullptr;}
const BackgroundResources* background_resources(u32 n)noexcept{return n<16?&backgrounds[n]:nullptr;}
void GameResources::release(Entry& e){for(auto handle:e.textures)if(handle)textures.destroy(handle);e.textures.clear();e.animation.reset();e.name.clear();}
bool GameResources::parse(Entry& next,AnimationFile slot){
    std::vector<u8> bytes;next.animation=std::make_unique<AnmResource>();
    if(!reader.read(next.name.c_str(),bytes)||!next.animation->load(i32(slot),bytes.data(),u32(bytes.size()))){error="Animation resource: "+next.name;return false;}
    return true;
}
bool GameResources::upload_one(Entry& next){
    const auto& sources=next.animation->textures();if(next.uploaded>=sources.size())return true;const u32 i=next.uploaded;const auto& source=sources[i];const u8* data=nullptr;u32 size=0;std::vector<u8> bytes;
    if(source.embedded){data=next.animation->data().data()+source.pixel_offset;size=source.pixel_size;}
    else if(!source.empty){if(!reader.read(source.name.c_str(),bytes)){error="Texture resource: "+source.name;return false;}data=bytes.data();size=u32(bytes.size());}
    const auto created=textures.create(source,data,size);
    if(!created.handle||!created.width||!created.height){if(created.handle)textures.destroy(created.handle);error="Texture allocation: "+source.name;return false;}
    next.textures.push_back(created.handle);if(!next.animation->configure_texture(i,created.handle,created.width,created.height)){error="Texture dimensions: "+source.name;return false;}
    ++next.uploaded;return true;
}
bool GameResources::load(AnimationFile slot,const char* name){
    const auto index=u32(slot);if(index>=entries.size()||!name)return false;auto& entry=entries[index];if(entry.animation&&entry.name==name)return true;
    Entry next;next.name=name;for(auto it=pending.begin();it!=pending.end();++it)if(it->slot==slot&&it->entry.name==name){next=std::move(it->entry);pending.erase(it);break;}
    if(!next.animation&&!parse(next,slot))return false;
    while(next.uploaded<next.animation->textures().size())if(!upload_one(next)){release(next);return false;}
    release(entry);entry=std::move(next);error.clear();return true;
}
void GameResources::preload(AnimationFile slot,const char* name){
    if(!name||u32(slot)>=entries.size())return;const auto& active=entries[u32(slot)];if(active.animation&&active.name==name)return;
    for(const auto& item:pending)if(item.slot==slot&&item.entry.name==name)return;
    if(pending.size()>=16)return;Pending item;item.slot=slot;item.entry.name=name;pending.push_back(std::move(item));
}
bool GameResources::warm_one(){
    for(auto it=pending.begin();it!=pending.end();++it){auto& p=*it;const auto previous=error;bool ok=true,worked=false;
        if(!p.entry.animation){const i32 prepared=reader.prepare(p.entry.name.c_str());if(prepared==0){error=previous;return true;}ok=prepared>0&&parse(p.entry,p.slot);worked=true;}else if(p.entry.uploaded<p.entry.animation->textures().size()){ok=upload_one(p.entry);worked=true;}
        if(!ok){release(p.entry);pending.erase(it);}error=previous;if(worked)return true;
    }return false;
}
void GameResources::cancel_preload(){reader.cancel_prepare();for(auto& p:pending)release(p.entry);pending.clear();}
AnmLoaded* GameResources::animation(AnimationFile slot){const auto index=u32(slot);return index<entries.size()&&entries[index].animation?&entries[index].animation->view():nullptr;}
bool GameResources::load_common(){return load(AnimationFile::text,"text.anm")&&load(AnimationFile::ascii,"ascii.anm")&&load(AnimationFile::capture,"capture.anm")&&load(AnimationFile::bullets,"etama.anm")&&load(AnimationFile::front,"front.anm");}
bool GameResources::load_match(i32 background,const i32 selected[2],const bool alternate[2],bool extra){
    const auto* bg=background_resources(u32(background));if(!bg)return false;
    for(u32 side=0;side<2;++side){const auto* ch=character_resources(u32(selected[side]));if(!ch||!load(character_animation(side),alternate[side]?ch->alternate_animation:ch->animation))return false;}
    const char* common=background==0||background==3||background==7?"enemy1.anm":background==12||background==15?"enemy13.anm":"enemy.anm";
    return load(AnimationFile::background,bg->animation)&&load(AnimationFile::enemies,common)&&(!extra||load(AnimationFile::extra_portraits,"pl06_fc_s.anm"));
}
bool GameResources::start(AnimationFile resource,AnmVm& vm,i32 script,bool reset){
    auto* file=animation(resource);if(!file||script<0||u32(script)>=file->scriptCount)return false;
    vm.scriptIndex=i16(script);if(reset){vm.pos=vm.pos2={};vm.fontWidth=vm.fontHeight=15;}
    executor.start(*file,vm,file->scripts[script]);return !executor.invalid;
}
bool GameResources::sprite(AnimationFile resource,AnmVm& vm,i32 index){auto* file=animation(resource);return file&&file->SetSprite(&vm,index)==0;}
}
