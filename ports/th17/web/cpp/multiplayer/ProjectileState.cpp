#include "FrameImage.hpp"
#include "../game/BulletManager.hpp"
#include "../game/EnemyManager.hpp"

namespace th17 {
void BulletManager::save_rollback(multiplayer::FrameImage& image){
    image.save(free_head,active_head,occupied,context,minimum_distance_squared,world_paused,
        cancellation_rewards,graze_history,error,visible_count,draw_groups);
    static_assert(capacity <= UINT16_MAX);
    struct Active {u16 index;BulletState state;};
    struct Inactive {u16 index;std::shared_ptr<const BulletState> state;};
    std::vector<Active> active;
    std::vector<Inactive> inactive;
    std::vector<u16> free;
    size_t active_count=0;
    for(u32 word:occupied)active_count+=__builtin_popcount(word);
    active.reserve(active_count);
    free.reserve(capacity);
    for(u32 index=active_head;index!=none;index=slots[index].active_next)
        active.push_back({u16(index),slots[index].state});
    for(u32 index=free_head;index!=none;index=slots[index].free_next)free.push_back(u16(index));
    for(u32 index=0;index<capacity;++index){
        auto& slot=slots[index];
        if(slot.active||slot.pristine)continue;
        // Reused bullets inherit fields from their previous life. The image
        // stays immutable until this slot leaves the free list again.
        auto cached=slot.inactive_image.lock();
        if(!cached){
            cached=std::shared_ptr<const BulletState>(new BulletState(slot.state));
            slot.inactive_image=cached;
        }
        inactive.push_back({u16(index),std::move(cached)});
    }
#if TH17_ROOM_BENCHMARK
    image.payload[image.BulletState]=u32(active.capacity()*sizeof(Active)+inactive.capacity()*sizeof(Inactive)+free.capacity()*sizeof(u16));
#endif
    image.restore_with([this,active=std::move(active),inactive=std::move(inactive),free=std::move(free)]{
        // Collision hosts keep permanent references into the slot array.
        u32 next=none;
        for(auto entry=free.rbegin();entry!=free.rend();++entry){
            const auto index=*entry;
            auto& slot=slots[index];
            slot.active=false;slot.active_next=slot.active_previous=none;
            slot.free_next=next;
            next=index;
        }
        auto cached=inactive.begin();
        for(u32 index=0;index<capacity;++index){
            if(occupied[index/32]&(u32(1)<<(index%32)))continue;
            auto& slot=slots[index];
            if(cached!=inactive.end()&&cached->index==index){
                if(slot.inactive_image.lock()!=cached->state)slot.state=*cached->state;
                slot.inactive_image=cached->state;slot.pristine=false;
                ++cached;
            }else if(!slot.pristine){
                slot.state=pristine_state;slot.inactive_image.reset();slot.pristine=true;
            }
        }
        u32 previous=none;
        for(const auto& entry:active){
            auto& slot=slots[entry.index];
            slot.state=entry.state;slot.inactive_image.reset();slot.pristine=false;
            slot.active=true;slot.free_next=slot.active_next=none;slot.active_previous=previous;
            if(previous!=none)slots[previous].active_next=entry.index;
            previous=entry.index;
        }
        return true;
    });
}

void EnemyManager::save_rollback(multiplayer::FrameImage& image){
    image.save(active,world,timer,next_identifier,rate,background_delta,error);
    for(const auto& enemy:active){
        image.save(enemy->state,enemy->error);
        image.restore_with([enemy,scripts=enemy->scripts.snapshot(),error=enemy->scripts.error]{
            if(!enemy->scripts.restore(scripts))return false;
            enemy->scripts.error=error;
            return true;
        });
    }
}
}
