#include "ItemFrameState.hpp"
#include "FrameValues.hpp"
#include "AnimationState.hpp"
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
void ItemFrameState::discard()noexcept{if(saved_){for(auto& entry:saved_->entries){entry.body.reset();entry.arrow.reset();}saved_->ready=false;}}
void ItemFrameState::append_roots(ItemManager& manager,const AnmRegistry& registry,std::vector<AnmVm*>& out){
    for(auto& slot:manager.slots){if(slot.body)out.push_back(slot.body.get());if(slot.arrow)out.push_back(slot.arrow.get());}
}
bool ItemFrameState::capture(ItemManager& manager,bool own_visuals){
    frame_values::IdentityGuard guard;error.clear();if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;s.owner=&manager;s.own_visuals=own_visuals;
    if(s.entries.size()<ItemManager::pool_size)s.entries.resize(ItemManager::pool_size);
    for(size_t i=0;i<s.entries.size();++i){const auto& slot=manager.slots[i];auto& e=s.entries[i];e.state=slot.state;e.arrow_ended=slot.arrow_ended;e.body=slot.body;e.arrow=slot.arrow;
        // Delayed cancel pickups retain their old VM until rebinding.
        // Preserve cold bodies as well as active bodies and reused arrows.
        if(slot.body&&own_visuals){if(!e.body_image)e.body_image=std::make_unique<AnmVm>();*e.body_image=*slot.body;}
        if(slot.arrow&&own_visuals){if(!e.arrow_image)e.arrow_image=std::make_unique<AnmVm>();*e.arrow_image=*slot.arrow;}
    }
    s.free_slots=manager.free_slots;s.requests=manager.requests;s.cancel_density=manager.cancel_density;s.cancel_stamp=manager.cancel_stamp;s.alternating_pieces=manager.alternating_pieces;s.active_count=manager.active_count;s.motion_scale=manager.motion_scale;s.error=manager.error;s.ready=true;return true;
}
bool ItemFrameState::restore(ItemManager& manager)const{
    frame_values::IdentityGuard guard;error.clear();if(!saved_||!saved_->ready||saved_->owner!=&manager){error="Item rollback crossed its scene owner";return false;}
    const auto& s=*saved_;
    for(size_t i=0;i<s.entries.size();++i){auto& slot=manager.slots[i];const auto& e=s.entries[i];slot.state=e.state;slot.arrow_ended=e.arrow_ended;slot.body=e.body;slot.arrow=e.arrow;
        if(slot.body&&s.own_visuals)*slot.body=*e.body_image;
        if(slot.arrow&&s.own_visuals)*slot.arrow=*e.arrow_image;
    }
    manager.free_slots=s.free_slots;manager.requests=s.requests;manager.cancel_density=s.cancel_density;manager.cancel_stamp=s.cancel_stamp;manager.alternating_pieces=s.alternating_pieces;manager.active_count=s.active_count;manager.motion_scale=s.motion_scale;manager.error=s.error;return true;
}
}
