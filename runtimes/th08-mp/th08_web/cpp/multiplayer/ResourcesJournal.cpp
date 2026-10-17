#include "ResourcesJournal.hpp"
#include <algorithm>

namespace th08::multiplayer {
bool ResourcesJournal::Bind(AnmLibrary& value){
    if(library||value.rollback_journal||value.rollback_failed)return false;
    for(u32 slot=0;slot<256;++slot){
        auto* resource=value.files[slot]?&value.files[slot]->resource:nullptr;
        auto& id=identities[slot];id={};if(!resource)continue;
        if(resource->loaded.numberEntriesToBeLoaded||resource->loaded.script_before_write||
           resource->loaded.script_write_context)return false;
        id.owner=resource;id.data=resource->raw.data();id.size=resource->raw.size();
        id.sprites=resource->sprites.data();id.sprite_count=resource->sprites.size();
        id.scripts=resource->script_pointers.data();id.script_count=resource->script_pointers.size();
    }
    Netplay::RollbackJournalConfig config;config.maxFrames=History;
    config.maxBytesPerFrame=16*1024*1024;config.maxBlocksPerFrame=10000;
    config.fastBulkCopy=true;config.coalesceRestore=true;
    if(!bytes.Reset(config))return false;
    library=&value;library->rollback_journal=&bytes;failed=false;
    // Install only after every owner and the journal have been validated.
    // No operation in this installation phase can fail halfway through.
    for(auto& id:identities)if(id.owner){
        id.owner->loaded.script_write_context=this;
        id.owner->loaded.script_before_write=&BeforeScriptWrite;
    }
    return true;
}
bool ResourcesJournal::stable()const{
    if(!library)return false;
    for(u32 slot=0;slot<256;++slot){
        auto* owner=library->files[slot]?&library->files[slot]->resource:nullptr;
        const auto& id=identities[slot];if(owner!=id.owner)return false;
        if(owner&&(owner->raw.data()!=id.data||owner->raw.size()!=id.size||
            owner->sprites.data()!=id.sprites||owner->sprites.size()!=id.sprite_count||
            owner->script_pointers.data()!=id.scripts||owner->script_pointers.size()!=id.script_count||
            owner->loaded.script_write_context!=this||owner->loaded.script_before_write!=&BeforeScriptWrite))return false;
    }
    return true;
}
bool ResourcesJournal::BeginFrame(u32 frame,bool extend){
    if(Failed()||!stable()||(!extend&&bytes.FrameCount()>=History)||!bytes.BeginFrame(frame,extend))return Fail();
    if(extend)return true;
    if(!touch(bytes,library->rollback_failed)||!touch(bytes,library->force_16bit))return Fail();
    for(auto& id:identities){
        auto* owner=id.owner;if(!owner)continue;
        if(!bytes.Touch(&owner->loaded,anm_loaded_state_bytes))return Fail();
        if(!owner->sprites.empty()&&!bytes.Touch(owner->sprites.data(),owner->sprites.size()*sizeof(AnmLoadedSprite)))return Fail();
#ifdef TH_MULTIPLAYER_FIXTURES
        // Independent eager reference for the write-barrier fixture.
        if(script_first_write)continue;
        for(const auto range:owner->script_ranges){
            if(range.first>range.last||range.last>owner->raw.size()||
               !bytes.Touch(owner->raw.data()+range.first,range.last-range.first))return Fail();
        }
#endif
    }
    return true;
}
bool ResourcesJournal::BeforeScriptWrite(void* context,const AnmLoaded& file,void* address,std::size_t size){
    return static_cast<ResourcesJournal*>(context)->TouchScript(file,address,size);
}
bool ResourcesJournal::TouchScript(const AnmLoaded& file,void* address,std::size_t size){
    if(Failed())return false;
    if(!library||file.anmIdx<0||file.anmIdx>=256||size!=sizeof(u32))return RejectWrite();
    const auto& id=identities[file.anmIdx];const auto* owner=id.owner;
    if(!owner||&owner->loaded!=&file||owner->raw.data()!=id.data||owner->raw.size()!=id.size)return RejectWrite();
    const auto base=reinterpret_cast<std::uintptr_t>(id.data),target=reinterpret_cast<std::uintptr_t>(address);
    if(target<base||target-base>id.size||size>id.size-(target-base))return RejectWrite();
    const std::size_t offset=target-base;
    auto range=std::upper_bound(owner->script_ranges.begin(),owner->script_ranges.end(),offset,
        [](std::size_t point,const auto& span){return point<span.first;});
    if(range==owner->script_ranges.begin())return RejectWrite();
    --range;if(offset>range->last||size>range->last-offset)return RejectWrite();
    if(!bytes.IsFrameOpen())return bytes.FrameCount()?RejectWrite():true;
#ifdef TH_MULTIPLAYER_FIXTURES
    if(!script_first_write)return true;
#endif
    return bytes.Touch(address,size)||RejectWrite();
}
void ResourcesJournal::Clear(){
    // Do not dereference an old identity if an invalid caller replaced it.
    if(library)for(u32 slot=0;slot<256;++slot){
        auto* owner=library->files[slot]?&library->files[slot]->resource:nullptr;
        if(owner&&owner==identities[slot].owner&&owner->loaded.script_write_context==this&&
           owner->loaded.script_before_write==&BeforeScriptWrite)owner->loaded.ClearScriptWriteHook();
    }
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
        part(&owner->loaded,anm_loaded_state_bytes);
        part(owner->raw.data(),owner->raw.size()); // whole-buffer independent oracle
        part(owner->sprites.data(),owner->sprites.size()*sizeof(AnmLoadedSprite));
    }
    return hash;
}
}
