#include "ItemFrameState.hpp"
#include "FrameValues.hpp"
#include "AnimationState.hpp"
#include <algorithm>
namespace th15::multiplayer {
struct ItemFrameState::Data {
    struct Entry {
        ItemState state;
        std::shared_ptr<AnmVm> body,arrow;
        std::unique_ptr<AnmVm> body_image,arrow_image;
        bool arrow_ended=false;
    };
    ItemManager* owner=nullptr;
    std::vector<Entry> entries;
    std::array<u64,ItemManager::snapshot_pages> versions{},active{},bound{};
    std::array<std::vector<u32>,2> free_slots;
    i32 requests=0,cancel_density=0,cancel_stamp=0,alternating_pieces=0,active_count=0;
    float motion_scale=1;
    std::string error;
    bool ready=false,own_visuals=true;
};
ItemFrameState::ItemFrameState()=default;
ItemFrameState::~ItemFrameState()=default;
ItemFrameState::ItemFrameState(ItemFrameState&&)noexcept=default;
ItemFrameState& ItemFrameState::operator=(ItemFrameState&&)noexcept=default;
void ItemFrameState::discard()noexcept{
    if(!saved_)return;auto& s=*saved_;
    for(u32 page=0;page<ItemManager::snapshot_pages;++page){u64 mask=s.bound[page];const u32 begin=page*ItemManager::snapshot_page_size,end=std::min(begin+ItemManager::snapshot_page_size,ItemManager::pool_size);
        for(u32 i=begin;i<end&&mask;++i,mask>>=1)if(mask&1){s.entries[i].body.reset();s.entries[i].arrow.reset();}
    }s.ready=false;
}
void ItemFrameState::append_roots(ItemManager& manager,const AnmRegistry& registry,std::vector<AnmVm*>& out){
    for(auto& slot:manager.slots){if(slot.body)out.push_back(slot.body.get());if(slot.arrow)out.push_back(slot.arrow.get());}
}
bool ItemFrameState::capture(ItemManager& manager,bool own_visuals){
    frame_values::IdentityGuard guard;error.clear();if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;const bool reuse=s.owner==&manager&&s.entries.size()==ItemManager::pool_size,discarded=!s.ready;s.ready=false;s.owner=&manager;s.own_visuals=own_visuals;
    if(s.entries.size()<ItemManager::pool_size)s.entries.resize(ItemManager::pool_size);
    for(u32 page=0;page<ItemManager::snapshot_pages;++page){
        u64 mask=!reuse||s.versions[page]!=manager.snapshot_versions[page]?~u64(0):s.active[page]|manager.snapshot_active[page]|manager.snapshot_exposed[page]|(discarded?s.bound[page]:0);
        if(!mask)continue;const u32 begin=page*ItemManager::snapshot_page_size,end=std::min(begin+ItemManager::snapshot_page_size,ItemManager::pool_size);
        for(u32 i=begin;i<end&&mask;++i,mask>>=1)if(mask&1){const auto& slot=manager.slots[i];auto& e=s.entries[i];e.state=slot.state;e.arrow_ended=slot.arrow_ended;e.body=slot.body;e.arrow=slot.arrow;
            const u64 bit=u64(1)<<(i-begin);if(e.body||e.arrow)s.bound[page]|=bit;else s.bound[page]&=~bit;
        }
    }
    s.versions=manager.snapshot_versions;s.active=manager.snapshot_active;
    // Shared VMs remain mutable even through a const slot. Preserve cold bodies
    // and reused arrows independently of the logical pool page identities.
    if(own_visuals)for(size_t i=0;i<s.entries.size();++i){const auto& slot=manager.slots[i];auto& e=s.entries[i];
        if(slot.body){if(!e.body_image)e.body_image=std::make_unique<AnmVm>();*e.body_image=*slot.body;}
        if(slot.arrow){if(!e.arrow_image)e.arrow_image=std::make_unique<AnmVm>();*e.arrow_image=*slot.arrow;}
    }
    s.free_slots=manager.free_slots;s.requests=manager.requests;s.cancel_density=manager.cancel_density;s.cancel_stamp=manager.cancel_stamp;s.alternating_pieces=manager.alternating_pieces;s.active_count=manager.active_count;s.motion_scale=manager.motion_scale;s.error=manager.error;s.ready=true;return true;
}
bool ItemFrameState::restore(ItemManager& manager)const{
    frame_values::IdentityGuard guard;error.clear();if(!saved_||!saved_->ready||saved_->owner!=&manager){error="Item rollback crossed its scene owner";return false;}
    const auto& s=*saved_;
    for(u32 page=0;page<ItemManager::snapshot_pages;++page){
        u64 mask=s.versions[page]!=manager.snapshot_versions[page]?~u64(0):s.active[page]|manager.snapshot_active[page]|manager.snapshot_exposed[page];
        if(!mask)continue;const u32 begin=page*ItemManager::snapshot_page_size,end=std::min(begin+ItemManager::snapshot_page_size,ItemManager::pool_size);
        for(u32 i=begin;i<end&&mask;++i,mask>>=1)if(mask&1){auto& slot=manager.slots[i];const auto& e=s.entries[i];slot.state=e.state;slot.arrow_ended=e.arrow_ended;slot.body=e.body;slot.arrow=e.arrow;}
    }
    manager.snapshot_versions=s.versions;manager.snapshot_active=s.active;
    if(s.own_visuals)for(size_t i=0;i<s.entries.size();++i){auto& slot=manager.slots[i];const auto& e=s.entries[i];
        if(slot.body)*slot.body=*e.body_image;
        if(slot.arrow)*slot.arrow=*e.arrow_image;
    }
    manager.free_slots=s.free_slots;manager.requests=s.requests;manager.cancel_density=s.cancel_density;manager.cancel_stamp=s.cancel_stamp;manager.alternating_pieces=s.alternating_pieces;manager.active_count=s.active_count;manager.motion_scale=s.motion_scale;manager.error=s.error;return true;
}
}
