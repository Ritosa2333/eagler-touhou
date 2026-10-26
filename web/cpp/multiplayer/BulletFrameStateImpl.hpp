// Included once, after BulletScene::Slot has been defined.
#include "BulletFrameState.hpp"
#include "FrameValues.hpp"
#include "AnimationState.hpp"
namespace th15::multiplayer {
struct BulletFrameState::Data {
    struct Visual {AnmVm body,overlay;};
    struct Binding {
        i32 layer=0;Vec3 translation{};u32 ordering=0,part=0;
        AnmEnvironment* environment=nullptr;AnmObjectHost* host=nullptr;
        void capture(const AnmVm& v){layer=v.visual.layer;translation=v.visual.translation;ordering=v.ordering;part=v.presentation_part;environment=v.environment;host=v.object_host;}
        void restore(AnmVm& v)const{v.visual.layer=layer;v.visual.translation=translation;v.ordering=ordering;v.presentation_part=part;v.environment=environment;v.object_host=host;}
    };
    struct Entry {
        bool cancelling=false,graze_initialized=false,active=false;
        std::array<Timer,3> graze_flash{},graze_duration{};
        std::array<i32,3> graze_interval{};
        std::array<MultiplayerGrazeVisual,3> graze_views{};
        std::unique_ptr<Visual> visual;
        Binding body_binding,overlay_binding;
        AnmResource* resource=nullptr;float rate=1;std::string error;
    };
    BulletScene* owner=nullptr;
    std::vector<BulletManager::Slot> logic;
    std::vector<Entry> entries;
    u32 free_head=0,active_head=0,visible_count=0;
    BulletFrameContext context;
    BulletCancellationRewards rewards;
    std::array<std::vector<u32>,6> draw_groups;
    std::array<u32,BulletManager::capacity> cancellation_animations{};
    Rng game_random,visual_random;
    float minimum_distance_squared=0;bool world_paused=false,ready=false;
    std::string error,scene_error;
};
BulletFrameState::BulletFrameState()=default;
BulletFrameState::~BulletFrameState()=default;
BulletFrameState::BulletFrameState(BulletFrameState&&)noexcept=default;
BulletFrameState& BulletFrameState::operator=(BulletFrameState&&)noexcept=default;
void BulletFrameState::append_roots(BulletScene& scene,const AnmRegistry& registry,std::vector<AnmVm*>& out){
    for(size_t i=0;i<scene.slots.size();++i){auto& v=scene.slots[i]->visual;for(auto* vm:{&v.body,&v.overlay})
        if(scene.manager.slots[i].active||(vm->geometry.overlay&&AnimationState::registered_tree(registry,*vm)))out.push_back(vm);
    }
}
bool BulletFrameState::capture(BulletScene& scene){
    frame_values::IdentityGuard guard;error.clear();
    if(scene.manager.slots.size()!=BulletManager::capacity||scene.slots.size()!=BulletManager::capacity){error="Bullet rollback pool changed capacity";return false;}
    if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;s.owner=&scene;auto& manager=scene.manager;
    // Native initialize() preserves cold transform fields and flags. Save the
    // entire logical pool, including free slots, rather than normalizing it.
    s.logic=manager.slots;if(s.entries.size()<scene.slots.size())s.entries.resize(scene.slots.size());
    for(size_t i=0;i<scene.slots.size();++i){const auto& slot=*scene.slots[i];auto& entry=s.entries[i];const auto& visual=slot.visual;
        entry.cancelling=slot.cancelling;entry.graze_flash=slot.graze_flash;entry.graze_duration=slot.graze_duration;entry.graze_interval=slot.graze_interval;entry.graze_initialized=slot.graze_initialized;entry.graze_views=slot.graze_views;
        entry.active=manager.slots[i].active;
        if(entry.active){if(!entry.visual)entry.visual=std::make_unique<Data::Visual>();entry.visual->body=visual.body;entry.visual->overlay=visual.overlay;}
        // Rebinding a cold visual replaces its VM but preserves these fields.
        // Any cold VM with live children is also captured by AnimationState.
        entry.body_binding.capture(visual.body);entry.overlay_binding.capture(visual.overlay);
        entry.resource=visual.resource;entry.rate=visual.rate;entry.error=visual.error;
    }
    s.free_head=manager.free_head;s.active_head=manager.active_head;s.context=manager.context;s.rewards=manager.cancellation_rewards;
    s.minimum_distance_squared=manager.minimum_distance_squared;s.world_paused=manager.world_paused;s.visible_count=manager.visible_count;s.draw_groups=manager.draw_groups;s.error=manager.error;
    s.cancellation_animations=scene.cancellation_animations;s.scene_error=scene.error;s.game_random=scene.game_random;s.visual_random=scene.visual_random;s.ready=true;return true;
}
bool BulletFrameState::restore(BulletScene& scene)const{
    frame_values::IdentityGuard guard;error.clear();if(!saved_||!saved_->ready||saved_->owner!=&scene){error="Bullet rollback crossed its scene owner";return false;}
    const auto& s=*saved_;auto& manager=scene.manager;
    if(manager.slots.size()!=s.logic.size()||scene.slots.size()!=s.entries.size()){error="Bullet rollback pool changed capacity";return false;}
    // Slot hosts hold references into this vector. Never replace its storage.
    std::copy(s.logic.begin(),s.logic.end(),manager.slots.begin());
    for(size_t i=0;i<scene.slots.size();++i){auto& slot=*scene.slots[i];const auto& entry=s.entries[i];auto& visual=slot.visual;
        slot.cancelling=entry.cancelling;slot.graze_flash=entry.graze_flash;slot.graze_duration=entry.graze_duration;slot.graze_interval=entry.graze_interval;slot.graze_initialized=entry.graze_initialized;slot.graze_views=entry.graze_views;
        if(entry.active){visual.body=entry.visual->body;visual.overlay=entry.visual->overlay;}
        else{entry.body_binding.restore(visual.body);entry.overlay_binding.restore(visual.overlay);}
        visual.resource=entry.resource;visual.rate=entry.rate;visual.error=entry.error;
    }
    manager.free_head=s.free_head;manager.active_head=s.active_head;manager.context=s.context;manager.cancellation_rewards=s.rewards;
    manager.minimum_distance_squared=s.minimum_distance_squared;manager.world_paused=s.world_paused;manager.visible_count=s.visible_count;manager.draw_groups=s.draw_groups;manager.error=s.error;
    scene.cancellation_animations=s.cancellation_animations;scene.error=s.scene_error;scene.game_random=s.game_random;scene.visual_random=s.visual_random;return true;
}
}
