#pragma once
#include "Player.hpp"
#include <initializer_list>
namespace th10 {
// Call-local resolution of a callback-free range. The registry remains unchanged
// structurally until an update_shot callback; flags/positions may still change.
// No allocation, persistent state, or simulation bytes are introduced.
class PlayerShotLookup {
public:
    static constexpr unsigned max_shots=128;
    static_assert(max_shots*4==512,"Update hash constants with scratch capacity");
private:
    struct Entry {u32 id;AnmVm* value;};
    Entry entries[max_shots*4];
    PlayerShot* end_=nullptr;
    bool active_=false;
    static u32 hash(u32 id) noexcept {return (id*2654435761u)>>23;}
    Entry& entry(u32 id) noexcept {
        u32 at=hash(id);while(entries[at].id&&entries[at].id!=id)at=(at+1)&511u;return entries[at];
    }
public:
    void prepare(PlayerShot* first,PlayerShot* limit,const AnmRegistry& registry) noexcept {
        if(end_&&first<end_)return;
        active_=false;end_=first+1;
        if(first->definition->on_update)return;
        unsigned count=0;
        for(end_=first;end_<limit;++end_){
            if(!end_->state)continue;
            if(end_->definition->on_update)break;
            ++count;
        }
        if(count<4)return;
        // At most 128 shots and two nonzero handles each: load <= 1/2.
        for(auto& e:entries)e={0,nullptr};
        unsigned remaining=0;
        for(auto* shot=first;shot<end_;++shot)if(shot->state){
            for(u32 id:{shot->animation,shot->secondary_animation})if(id){auto& e=entry(id);if(!e.id){e.id=id;++remaining;}}
        }
        active_=true;if(!remaining)return;
        for(auto* head:{registry.world_head,registry.ui_head})for(auto* node=head;node;node=node->next){
            auto* vm=node->value;const u32 id=vm->id;if(!id)continue;
            auto& e=entry(id);if(e.id&&!e.value){e.value=vm;if(!--remaining)return;}
        }
    }
    AnmVm* find(u32 id,const AnmRegistry& registry) noexcept {
        if(!active_)return registry.find(id);
        if(!id)return nullptr;
        auto& e=entry(id);return e.id?e.value:registry.find(id);
    }
};
}
