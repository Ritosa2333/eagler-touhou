#include "FrameImage.hpp"
#include "AnmCheckpoint.hpp"
#include "../game/ItemSpirits.hpp"
#include "../game/ScreenEffects.hpp"

namespace th17 {
void ItemManager::save_rollback(multiplayer::FrameImage& image){
    struct Visual {
        AnmVm* owner;
        multiplayer::AnmCheckpoint vm;
        Visual(AnmVm& value,multiplayer::AnmCheckpoint::Cache& cache,multiplayer::AnmCheckpoint::Curves& curves):
            owner(&value),vm(value,cache,curves){}
    };
    struct Idle {AnmVm* owner;std::shared_ptr<const AnmVm> vm;};
    struct SavedSlot {u32 index;ItemState state;u8 flags;bool arrow_ended;};
    std::vector<SavedSlot> saved;
    std::vector<Visual> visuals;
    std::vector<Idle> idle;
    multiplayer::AnmCheckpoint::Curves curves;
    std::array<u8,pool_size> flags{};
    if(!idle_visuals)idle_visuals=std::make_unique<std::array<IdleVisuals,pool_size>>();
    size_t slot_count=0,visual_count=0,idle_count=0,changing=0;
    const auto reusable=[&](const AnmVm& vm){
        return !vm.creation_parent&&!vm.rotation_parent&&(!vm.object_host||vm.object_host==&animations)&&
            !animations.registry.borrowed_root(vm);
    };
    for(u32 i=next_used(0);i<pool_size;i=next_used(i+1)){
        const auto& s=slots[i];
        if(!(s.state.state||s.body||s.arrow))continue;
        ++slot_count;
        const bool body_idle=s.body&&!s.state.state&&reusable(*s.body);
        const bool arrow_idle=s.arrow&&!s.state.state&&reusable(*s.arrow);
        flags[i]=u8(bool(s.body)|(bool(s.arrow)<<1)|(body_idle<<2)|(arrow_idle<<3));
        visual_count+=bool(s.body)+bool(s.arrow)-body_idle-arrow_idle;
        idle_count+=body_idle+arrow_idle;
        changing+=s.body&&!body_idle&&multiplayer::AnmCheckpoint::changing(*s.body);
        changing+=s.arrow&&!arrow_idle&&multiplayer::AnmCheckpoint::changing(*s.arrow);
    }
    saved.reserve(slot_count);
    visuals.reserve(visual_count);
    idle.reserve(idle_count);
    curves.changing.reserve(changing);
    const auto capture_visual=[&](AnmVm* vm,CachedVisual& cache,bool share){
        if(!vm)return;
        if(!share){cache.idle.reset();visuals.emplace_back(*vm,cache.curves,curves);return;}
        auto cached=cache.idle.lock();
        if(!cached){
            cached=std::shared_ptr<const AnmVm>(new AnmVm(*vm));
            cache.idle=cached;
        }
        idle.push_back({vm,std::move(cached)});
    };
    for(u32 i=next_used(0);i<pool_size;i=next_used(i+1)){
        const auto& s=slots[i];
        const u8 mode=flags[i];
        if(!mode&&!s.state.state)continue;
        saved.push_back({i,s.state,mode,s.arrow_ended});
        auto& cache=(*idle_visuals)[i];
        capture_visual(s.body.get(),cache.body,mode&4);
        capture_visual(s.arrow.get(),cache.arrow,mode&8);
#if TH17_ROOM_BENCHMARK
        if(!s.state.state)image.payload[image.IdleItemVisual]+=u32(
            (bool(s.body)&&!(mode&4)?sizeof(Visual):0)+(bool(s.arrow)&&!(mode&8)?sizeof(Visual):0)+
            (bool(mode&4)+bool(mode&8))*sizeof(Idle));
#endif
    }
#if TH17_ROOM_BENCHMARK
    image.payload[image.ItemVisual]=u32(saved.capacity()*sizeof(SavedSlot)+visuals.capacity()*sizeof(Visual)+idle.capacity()*sizeof(Idle)+curves.changing.capacity()*sizeof(AnmInterpolators));
#endif
    image.save(free_slots,requests,cancel_density,cancel_stamp,active_count,motion_scale,error);
    image.restore_with([this,saved=std::move(saved),visuals=std::move(visuals),idle=std::move(idle),curves=std::move(curves)]{
        if(!idle_visuals)return false;
        auto vm=visuals.begin();
        auto shared=idle.begin();
        const auto restore_visual=[&](auto& owner,CachedVisual& cache,bool present,bool share){
            if(!present){owner.reset();cache={};return true;}
            if(share){
                const auto& visual=*shared++;
                if(owner.get()!=visual.owner)return false;
                if(cache.idle.lock()!=visual.vm)*owner=*visual.vm;
                cache.idle=visual.vm;
                return true;
            }
            const auto& visual=*vm++;
            if(owner.get()!=visual.owner)return false;
            cache.idle.reset();
            visual.vm.restore(*owner,cache.curves,curves);return true;
        };
        auto next=saved.begin();
        for(u32 i=next_used(0);i<pool_size;i=next_used(i+1)){
            while(next!=saved.end()&&next->index<i)++next;
            if(next!=saved.end()&&next->index==i)continue;
            slots[i]={};
            (*idle_visuals)[i]={};
        }
        used.fill(0);
        for(const auto& s:saved){
            const auto i=s.index;
            used[i/32]|=u32(1)<<(i%32);
            auto& slot=slots[i];
            auto& cache=(*idle_visuals)[i];
            slot.state=s.state;slot.arrow_ended=s.arrow_ended;
            if(!restore_visual(slot.body,cache.body,s.flags&1,s.flags&4)||
               !restore_visual(slot.arrow,cache.arrow,s.flags&2,s.flags&8))return false;
        }
        return true;
    });
}

void ItemSpirits::save_rollback(multiplayer::FrameImage& image){
    image.save(practice,collection.error,spirits.entries,spirits.generation,spirits.error,
        roaring.state,roaring.visuals,roaring.partner_orbs,roaring.error,error);
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
