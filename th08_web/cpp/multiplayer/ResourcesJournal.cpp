#include "ResourcesJournal.hpp"

namespace th08::multiplayer {
bool ResourcesJournal::Bind(AnmLibrary& value){
    if(library||value.rollback_journal||value.rollback_failed)return false;
    for(u32 slot=0;slot<256;++slot){
        auto* resource=value.files[slot]?&value.files[slot]->resource:nullptr;
        auto& id=identities[slot];id={};if(!resource)continue;
        if(resource->loaded.numberEntriesToBeLoaded)return false;
        id.owner=resource;id.data=resource->raw.data();id.size=resource->raw.size();
        id.sprites=resource->sprites.data();id.sprite_count=resource->sprites.size();
        id.scripts=resource->script_pointers.data();id.script_count=resource->script_pointers.size();
    }
    Netplay::RollbackJournalConfig config;config.maxFrames=History;
    config.maxBytesPerFrame=16*1024*1024;config.maxBlocksPerFrame=10000;
    config.fastBulkCopy=true;config.coalesceRestore=true;
    if(!bytes.Reset(config))return false;
    library=&value;library->rollback_journal=&bytes;failed=false;return true;
}
bool ResourcesJournal::stable()const{
    if(!library)return false;
    for(u32 slot=0;slot<256;++slot){
        auto* owner=library->files[slot]?&library->files[slot]->resource:nullptr;
        const auto& id=identities[slot];if(owner!=id.owner)return false;
        if(owner&&(owner->raw.data()!=id.data||owner->raw.size()!=id.size||
            owner->sprites.data()!=id.sprites||owner->sprites.size()!=id.sprite_count||
            owner->script_pointers.data()!=id.scripts||owner->script_pointers.size()!=id.script_count))return false;
    }
    return true;
}
bool ResourcesJournal::BeginFrame(u32 frame){
    if(Failed()||!stable()||bytes.FrameCount()>=History||!bytes.BeginFrame(frame))return Fail();
    if(!touch(bytes,library->rollback_failed)||!touch(bytes,library->force_16bit))return Fail();
    for(auto& id:identities){
        auto* owner=id.owner;if(!owner)continue;
        if(!touch(bytes,owner->loaded))return Fail();
        if(!owner->sprites.empty()&&!bytes.Touch(owner->sprites.data(),owner->sprites.size()*sizeof(AnmLoadedSprite)))return Fail();
        for(const auto range:owner->script_ranges){
            if(range.first>range.last||range.last>owner->raw.size()||
               !bytes.Touch(owner->raw.data()+range.first,range.last-range.first))return Fail();
        }
    }
    return true;
}
void ResourcesJournal::Clear(){
    if(library&&library->rollback_journal==&bytes)library->rollback_journal=nullptr;
    library=nullptr;bytes.Clear();failed=false;
    for(auto& id:identities)id={};
}
u32 ResourcesJournal::AuditHash()const{
    if(!stable())return 0;
    u32 hash=2166136261u;
    const auto part=[&](const void* p,std::size_t n){auto* data=static_cast<const u8*>(p);for(std::size_t i=0;i<n;++i){hash^=data[i];hash*=16777619u;}};
    for(const auto& id:identities){
        auto* owner=id.owner;if(!owner)continue;
        part(&owner->loaded,sizeof(owner->loaded));
        part(owner->raw.data(),owner->raw.size()); // whole-buffer independent oracle
        part(owner->sprites.data(),owner->sprites.size()*sizeof(AnmLoadedSprite));
    }
    return hash;
}
}
