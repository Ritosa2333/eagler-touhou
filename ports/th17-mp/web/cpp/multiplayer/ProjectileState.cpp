#include "FrameImage.hpp"
#include "../game/BulletManager.hpp"
#include "../game/EnemyManager.hpp"

namespace th17 {
void BulletManager::save_rollback(multiplayer::FrameImage& image){
    image.save(free_head,active_head,occupied,context,minimum_distance_squared,world_paused,
        cancellation_rewards,graze_history,error,visible_count,draw_groups);
    static_assert(capacity <= UINT16_MAX);
    struct Active {
        u16 index;
        BulletState state;
        Active(u16 i,const BulletState& value):index(i),state(value){}
    };
    struct Inactive {u16 index;std::shared_ptr<const BulletState> state;};
    struct Snapshot {
        std::vector<Active> active;
        std::vector<Inactive> inactive;
        void reset(){
            // A dense story pattern can keep over 700 bullets alive. Keep one
            // spare at that size so clearing an image does not force another
            // large allocation during the next repair.
            constexpr size_t maximum_cached_bytes=768*1024;
            if(active.capacity()*sizeof(Active)+inactive.capacity()*sizeof(Inactive)>maximum_cached_bytes){
                *this=Snapshot{};
                return;
            }
            active.clear();inactive.clear();
        }
    };
    auto& snapshot=image.reuse<Snapshot>();
    auto& active=snapshot.active;
    auto& inactive=snapshot.inactive;
    size_t active_count=0;
    for(u32 word:occupied)active_count+=__builtin_popcount(word);
    active.reserve((active_count+31)/32*32);
    for(u32 index=active_head;index!=none;index=slots[index].active_next)
        active.emplace_back(u16(index),slots[index].state);
    // Allocation removes the head and recycling prepends a used slot. The
    // untouched suffix therefore stays in its original descending order.
    u32 pristine_head=free_head;
    for(;pristine_head!=none&&!slots[pristine_head].pristine;pristine_head=slots[pristine_head].free_next){
        auto& slot=slots[pristine_head];
        // Reused bullets inherit fields from their previous life. The image
        // stays immutable until this slot leaves the free list again.
        auto cached=slot.inactive_image.lock();
        if(!cached){
            cached=std::shared_ptr<const BulletState>(new BulletState(slot.state));
            slot.inactive_image=cached;
        }
        inactive.push_back({u16(pristine_head),std::move(cached)});
    }
#if TH17_ROOM_BENCHMARK
    image.payload[image.BulletState]=u32(active.capacity()*sizeof(Active)+inactive.capacity()*sizeof(Inactive));
#endif
    image.restore_with([this,checkpoint=&snapshot,pristine_head]{
        // Collision hosts keep permanent references into the slot array.
        for(u32 index=pristine_head;index!=none&&!slots[index].pristine;--index){
            auto& slot=slots[index];
            slot.active=false;slot.active_next=slot.active_previous=none;
            slot.free_next=index-1;
            slot.state=pristine_state;slot.inactive_image.reset();slot.pristine=true;
        }
        u32 next=pristine_head;
        for(auto cached=checkpoint->inactive.rbegin();cached!=checkpoint->inactive.rend();++cached){
            auto& slot=slots[cached->index];
            slot.active=false;slot.active_next=slot.active_previous=none;
            slot.free_next=next;
            next=cached->index;
            if(slot.inactive_image.lock()!=cached->state)slot.state=*cached->state;
            slot.inactive_image=cached->state;slot.pristine=false;
        }
        u32 previous=none;
        for(const auto& entry:checkpoint->active){
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
