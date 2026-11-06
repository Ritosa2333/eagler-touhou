#include "FrameImage.hpp"
#include "../game/ItemSpirits.hpp"
#include "../game/ScreenEffects.hpp"
#include <optional>

namespace th17 {
void ItemManager::save_rollback(multiplayer::FrameImage& image){
    struct Visual {AnmVm* owner=nullptr;std::optional<AnmVm> vm;};
    struct SavedSlot {u32 index;ItemState state;Visual body,arrow;bool arrow_ended;};
    const auto save_visual=[](const auto& owner){
        Visual visual;
        if(owner){visual.owner=owner.get();visual.vm=*owner;}
        return visual;
    };
    std::vector<SavedSlot> saved;
    for(u32 i=0;i<slots.size();++i){
        const auto& s=slots[i];
        if(s.state.state||s.body||s.arrow)saved.push_back({i,s.state,save_visual(s.body),save_visual(s.arrow),s.arrow_ended});
    }
    image.save(free_slots,requests,cancel_density,cancel_stamp,active_count,motion_scale,error);
    image.restore_with([this,saved=std::move(saved)]{
        const auto restore_visual=[](auto& owner,const Visual& visual){
            if(!visual.owner){owner.reset();return true;}
            if(owner.get()!=visual.owner)return false;
            *owner=*visual.vm;return true;
        };
        auto next=saved.begin();
        for(u32 i=0;i<slots.size();++i){
            auto& slot=slots[i];
            if(next==saved.end()||next->index!=i){slot={};continue;}
            const auto& s=*next++;
            slot.state=s.state;slot.arrow_ended=s.arrow_ended;
            if(!restore_visual(slot.body,s.body)||!restore_visual(slot.arrow,s.arrow))return false;
        }
        return true;
    });
}

void ItemSpirits::save_rollback(multiplayer::FrameImage& image){
    image.save(practice,collection.error,spirits.entries,spirits.generation,spirits.error,
        roaring.state,roaring.visuals,roaring.error,error);
    items.save_rollback(image);
}

void ScreenEffects::save_rollback(multiplayer::FrameImage& image){
    image.save(entries,context,world_offset,screen_offset,error);
    for(const auto& entry:entries){
        image.save(entry->retired);
        if(auto* shake=entry->shake.get())image.save(shake->age,shake->world_offset,shake->screen_offset);
        if(auto* nudge=entry->nudge.get())image.save(nudge->age,nudge->world_offset,nudge->screen_offset);
    }
}
}
