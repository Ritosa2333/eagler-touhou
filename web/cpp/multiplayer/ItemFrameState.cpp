#include "ItemFrameState.hpp"
#include "FrameValues.hpp"
#include "AnimationState.hpp"
namespace th15::multiplayer {
struct ItemFrameState::Data {
    struct Entry {
        ItemState state;
        std::shared_ptr<AnmVm> body,arrow;
        std::unique_ptr<AnmVm> body_image,arrow_image;
        Vec3 body_translation{};
        bool full_body=false,arrow_ended=false;
    };
    ItemManager* owner=nullptr;
    std::vector<Entry> entries;
    std::array<std::vector<u32>,2> free_slots;
    i32 requests=0,cancel_density=0,cancel_stamp=0,alternating_pieces=0,active_count=0;
    float motion_scale=1;
    std::string error;
    bool ready=false;
};
ItemFrameState::ItemFrameState()=default;
ItemFrameState::~ItemFrameState()=default;
ItemFrameState::ItemFrameState(ItemFrameState&&)noexcept=default;
ItemFrameState& ItemFrameState::operator=(ItemFrameState&&)noexcept=default;
void ItemFrameState::discard()noexcept{if(saved_){for(auto& entry:saved_->entries){entry.body.reset();entry.arrow.reset();}saved_->ready=false;}}
void ItemFrameState::append_roots(ItemManager& manager,const AnmRegistry& registry,std::vector<AnmVm*>& out){
    for(size_t i=0;i<manager.slots.size();++i){auto& slot=manager.slots[i];if(slot.body&&(i<ItemManager::ordinary_count||slot.state.state||(slot.body->geometry.overlay&&AnimationState::registered_tree(registry,*slot.body))))out.push_back(slot.body.get());if(slot.arrow)out.push_back(slot.arrow.get());}
}
bool ItemFrameState::capture(ItemManager& manager){
    frame_values::IdentityGuard guard;error.clear();if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;s.owner=&manager;
    if(s.entries.size()<ItemManager::pool_size)s.entries.resize(ItemManager::pool_size);
    for(size_t i=0;i<s.entries.size();++i){const auto& slot=manager.slots[i];auto& e=s.entries[i];e.state=slot.state;e.arrow_ended=slot.arrow_ended;e.body=slot.body;e.arrow=slot.arrow;
        e.full_body=bool(slot.body)&&(i<ItemManager::ordinary_count||slot.state.state!=0);
        if(e.full_body){if(!e.body_image)e.body_image=std::make_unique<AnmVm>();*e.body_image=*slot.body;}
        if(slot.body)e.body_translation=slot.body->visual.translation;
        if(slot.arrow){if(!e.arrow_image)e.arrow_image=std::make_unique<AnmVm>();*e.arrow_image=*slot.arrow;}
    }
    s.free_slots=manager.free_slots;s.requests=manager.requests;s.cancel_density=manager.cancel_density;s.cancel_stamp=manager.cancel_stamp;s.alternating_pieces=manager.alternating_pieces;s.active_count=manager.active_count;s.motion_scale=manager.motion_scale;s.error=manager.error;s.ready=true;return true;
}
bool ItemFrameState::restore(ItemManager& manager)const{
    frame_values::IdentityGuard guard;error.clear();if(!saved_||!saved_->ready||saved_->owner!=&manager){error="Item rollback crossed its scene owner";return false;}
    const auto& s=*saved_;
    for(size_t i=0;i<s.entries.size();++i){auto& slot=manager.slots[i];const auto& e=s.entries[i];slot.state=e.state;slot.arrow_ended=e.arrow_ended;slot.body=e.body;slot.arrow=e.arrow;
        if(e.full_body)*slot.body=*e.body_image;
        // bind_template replaces a cold cancel VM before use and preserves
        // translation. Ordinary arrows retain native state across kinds.
        else if(slot.body)slot.body->visual.translation=e.body_translation;
        if(slot.arrow)*slot.arrow=*e.arrow_image;
    }
    manager.free_slots=s.free_slots;manager.requests=s.requests;manager.cancel_density=s.cancel_density;manager.cancel_stamp=s.cancel_stamp;manager.alternating_pieces=s.alternating_pieces;manager.active_count=s.active_count;manager.motion_scale=s.motion_scale;manager.error=s.error;return true;
}
}
