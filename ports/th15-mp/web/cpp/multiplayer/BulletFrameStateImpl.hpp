// Included once, after BulletScene::Slot has been defined.
#include "BulletFrameState.hpp"
#include "FrameValues.hpp"
#include "AnimationState.hpp"
namespace th15::multiplayer {
struct BulletFrameState::Data {
    // The 18 transform programs (including payload vectors) change on emission
    // and checkpoint replacement. Motion, timers and cursors change every tick.
    static void copy_runtime(BulletManager::Slot& to,const BulletManager::Slot& from){
        static_cast<BulletRuntime&>(to.state)=static_cast<const BulletRuntime&>(from.state);
        to.free_next=from.free_next;to.active_next=from.active_next;to.active_previous=from.active_previous;to.active=from.active;
    }
    static void copy_motion(BulletManager::Slot& to,const BulletManager::Slot& from){
        static_cast<BulletMotion&>(to.state)=static_cast<const BulletMotion&>(from.state);
        to.free_next=from.free_next;to.active_next=from.active_next;to.active_previous=from.active_previous;to.active=from.active;
    }
    static bool cached_body(const BulletScene& scene,size_t index,bool unchanged){
        const auto& vm=scene.slots[index]->visual.body;
        return !scene.manager.snapshot_exposed[index]&&!vm.rotation_parent&&!vm.creation_parent&&vm.error.empty()&&(unchanged||(vm.geometry.empty()&&vm.visual.draw_mode()<=3&&!(vm.visual.render_flags&0x2000)));
    }
    struct Binding {
        i32 layer=0;Vec3 translation{};u32 ordering=0,part=0;
        AnmEnvironment* environment=nullptr;AnmObjectHost* host=nullptr;
        void capture(const AnmVm& v){layer=v.visual.layer;translation=v.visual.translation;ordering=v.ordering;part=v.presentation_part;environment=v.environment;host=v.object_host;}
        void restore(AnmVm& v)const{v.visual.layer=layer;v.visual.translation=translation;v.ordering=ordering;v.presentation_part=part;v.environment=environment;v.object_host=host;}
    };
    struct Entry {
        AnmResource* resource=nullptr;float rate=1;
        bool cancelling=false,active=false,cached_overlay=false,cached_body=false;
        BulletScene::Slot::GrazeState graze;
        std::unique_ptr<AnmVm> body_image,overlay_image;
        Binding body_binding,overlay_binding;
        std::string error;
    };
    BulletScene* owner=nullptr;
    std::vector<BulletManager::Slot> logic;
    std::vector<u64> versions,runtime_versions;
    std::vector<Entry> entries;
    // Commands, interrupts and active curves version the complete value image.
    // Waiting scripts only change clocks, motion and these durable Draw fields.
    struct BodyMotion {
        Timer timer,age;Vec3 position{},rotation{},translation{};Vec2 secondary_scale{};
        std::array<Vec3,4> quad{};u64 program_version=0;float slowdown=0;
        i32 instruction_offset=-1,pending_interrupt=0;
        u32 flags=0,secondary_color=0,inherited_color=0;
        bool visible=false,presentation_motion=false,interpolation_idle=false;
    };
    static_assert(std::is_trivially_copyable_v<BodyMotion>);
    static void copy_body(BodyMotion& to,const AnmVm& from){
        to.timer=from.timer;to.age=from.age;to.position=from.variables.position;to.rotation=from.variables.rotation;
        to.translation=from.visual.translation;to.secondary_scale=from.visual.secondary_scale;to.quad=from.visual.quad;
        to.program_version=from.program_version;to.slowdown=from.slowdown;
        to.instruction_offset=from.instruction_offset;to.pending_interrupt=from.pending_interrupt;
        to.flags=from.visual.flags;to.secondary_color=from.visual.secondary_color;to.inherited_color=from.visual.inherited_color;
        to.visible=from.visible;to.presentation_motion=from.presentation_motion;to.interpolation_idle=from.interpolation_idle;
    }
    static void restore_body(AnmVm& to,const BodyMotion& from){
        to.timer=from.timer;to.age=from.age;to.variables.position=from.position;to.variables.rotation=from.rotation;
        to.visual.translation=from.translation;to.visual.secondary_scale=from.secondary_scale;to.visual.quad=from.quad;
        to.rotation_parent=to.creation_parent=nullptr;
        to.program_version=from.program_version;to.slowdown=from.slowdown;
        to.instruction_offset=from.instruction_offset;to.pending_interrupt=from.pending_interrupt;
        to.visual.flags=from.flags;to.visual.secondary_color=from.secondary_color;to.visual.inherited_color=from.inherited_color;
        to.visible=from.visible;to.presentation_motion=from.presentation_motion;to.interpolation_idle=from.interpolation_idle;
    }
    std::vector<BodyMotion> body_motion;
    std::vector<AnmVmState> body_programs;
    std::vector<AnmVm*> roots;
    u32 free_head=0,active_head=0,visible_count=0;
    BulletFrameContext context;
    BulletCancellationRewards rewards;
    std::array<std::vector<u32>,6> draw_groups;
    std::array<u32,BulletManager::capacity> cancellation_animations{};
    Rng game_random,visual_random;
    float minimum_distance_squared=0;bool world_paused=false,ready=false,own_visuals=true;
    std::string error,scene_error;
};
BulletFrameState::BulletFrameState()=default;
BulletFrameState::~BulletFrameState()=default;
BulletFrameState::BulletFrameState(BulletFrameState&&)noexcept=default;
BulletFrameState& BulletFrameState::operator=(BulletFrameState&&)noexcept=default;
void BulletFrameState::append_roots(std::vector<AnmVm*>& out)const{
    if(saved_&&saved_->ready)out.insert(out.end(),saved_->roots.begin(),saved_->roots.end());
}
bool BulletFrameState::capture(BulletScene& scene,bool own_visuals,const AnmRegistry* registry){
    frame_values::IdentityGuard guard;error.clear();
    if(scene.manager.slots.size()!=BulletManager::capacity||scene.slots.size()!=BulletManager::capacity){error="Bullet rollback pool changed capacity";return false;}
    if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;const bool same_owner=s.owner==&scene;s.ready=false;s.owner=&scene;s.own_visuals=own_visuals;auto& manager=scene.manager;
    // Native initialize() preserves cold transform fields and flags. Save the
    // entire logical pool, including free slots, rather than normalizing it.
    const bool initialized=same_owner&&s.logic.size()==manager.slots.size();
    if(!initialized)s.logic=manager.slots;
    if(s.entries.size()<scene.slots.size())s.entries.resize(scene.slots.size());
    if(!own_visuals&&s.body_motion.size()<scene.slots.size()){s.body_motion.resize(scene.slots.size());s.body_programs.resize(scene.slots.size());}
    s.roots.clear();
    for(size_t i=0;i<scene.slots.size();++i){auto& slot=*scene.slots[i];auto& entry=s.entries[i];const auto& visual=slot.visual;const auto& live=manager.slots[i];
        const bool same_content=initialized&&!manager.snapshot_exposed[i]&&s.versions[i]==manager.snapshot_versions[i];
        const bool same_program=same_content&&!own_visuals&&entry.cached_body&&s.body_motion[i].program_version==visual.body.program_version;
        // Ring reuse normally sees the same plain active bullet. Its versions
        // prove that program, ownership and unbound overlay are unchanged.
        if(same_program&&live.active&&entry.active&&entry.cached_overlay&&!live.state.overlay_active&&s.runtime_versions[i]==manager.snapshot_runtime_versions[i]&&!visual.body.rotation_parent&&!visual.body.creation_parent&&visual.body.error.empty()&&visual.error.empty()&&entry.error.empty()){
            Data::copy_motion(s.logic[i],live);Data::copy_body(s.body_motion[i],visual.body);
            if(entry.graze.version!=slot.graze_state.version)entry.graze=slot.graze_state;
            entry.cancelling=slot.cancelling;entry.resource=visual.resource;entry.rate=visual.rate;continue;
        }
        if(initialized){
            if(!same_content)s.logic[i]=live;
            else if(s.runtime_versions[i]!=manager.snapshot_runtime_versions[i])Data::copy_runtime(s.logic[i],live);
            else if(live.active||s.logic[i].active)Data::copy_motion(s.logic[i],live);
        }
        const bool body_cached=live.active&&!own_visuals&&Data::cached_body(scene,i,same_program);
        if(!own_visuals){
            const auto borrowed=[&](const AnmVm& vm){return vm.geometry.overlay&&(!registry||AnimationState::registered_tree(*registry,vm));};
            if((live.active&&!body_cached)||borrowed(visual.body))s.roots.push_back(&slot.visual.body);
            if((live.active&&(live.state.overlay_active||manager.snapshot_exposed[i]||visual.overlay.geometry.overlay))||borrowed(visual.overlay))s.roots.push_back(&slot.visual.overlay);
        }
        if(same_content&&!live.active&&!entry.active)continue;
        const bool cached=entry.cached_overlay;
        entry.cancelling=slot.cancelling;
        if(!same_content||entry.graze.version!=slot.graze_state.version)entry.graze=slot.graze_state;
        entry.active=manager.slots[i].active;
        entry.cached_body=body_cached;
        entry.cached_overlay=entry.active&&!manager.slots[i].state.overlay_active&&!manager.snapshot_exposed[i]&&!visual.overlay.geometry.overlay;
        if(entry.active&&own_visuals){
            if(!entry.body_image)entry.body_image=std::make_unique<AnmVm>();
            frame_values::copy_vm(*entry.body_image,visual.body);
        }else if(entry.cached_body){
            if(!same_program)s.body_programs[i]=static_cast<const AnmVmState&>(visual.body);
            Data::copy_body(s.body_motion[i],visual.body);
        }
        // Unbound overlays do not tick. Keep their exact bytes and refresh the
        // cached image only when emission or rebinding gives it a new identity.
        if(entry.active&&(own_visuals||entry.cached_overlay)){
            if(!entry.overlay_image)entry.overlay_image=std::make_unique<AnmVm>();
            if(own_visuals||!cached||!same_content)frame_values::copy_vm(*entry.overlay_image,visual.overlay);
        }
        // Rebinding a cold visual replaces its VM but preserves these fields.
        // Any cold VM with live children is also captured by AnimationState.
        if(!entry.active){entry.body_binding.capture(visual.body);entry.overlay_binding.capture(visual.overlay);}
        entry.resource=visual.resource;entry.rate=visual.rate;if(!entry.error.empty()||!visual.error.empty())entry.error=visual.error;
    }
    s.versions=manager.snapshot_versions;
    s.runtime_versions=manager.snapshot_runtime_versions;
    s.free_head=manager.free_head;s.active_head=manager.active_head;s.context=manager.context;s.rewards=manager.cancellation_rewards;
    s.minimum_distance_squared=manager.minimum_distance_squared;s.world_paused=manager.world_paused;s.visible_count=manager.visible_count;s.draw_groups=manager.draw_groups;s.error=manager.error;
    s.cancellation_animations=scene.cancellation_animations;s.scene_error=scene.error;s.game_random=scene.game_random;s.visual_random=scene.visual_random;s.ready=true;return true;
}
bool BulletFrameState::restore(BulletScene& scene)const{
    frame_values::IdentityGuard guard;error.clear();if(!saved_||!saved_->ready||saved_->owner!=&scene){error="Bullet rollback crossed its scene owner";return false;}
    const auto& s=*saved_;auto& manager=scene.manager;
    if(manager.slots.size()!=s.logic.size()||scene.slots.size()!=s.entries.size()){error="Bullet rollback pool changed capacity";return false;}
    // Slot hosts hold references into this vector. Never replace its storage.
    for(size_t i=0;i<s.logic.size();++i){
        if(!manager.snapshot_exposed[i]&&!manager.slots[i].active&&!s.logic[i].active&&s.versions[i]==manager.snapshot_versions[i]&&s.runtime_versions[i]==manager.snapshot_runtime_versions[i])continue;
        if(manager.snapshot_exposed[i]||s.versions[i]!=manager.snapshot_versions[i])manager.slots[i]=s.logic[i];
        else if(s.runtime_versions[i]!=manager.snapshot_runtime_versions[i])Data::copy_runtime(manager.slots[i],s.logic[i]);
        else Data::copy_motion(manager.slots[i],s.logic[i]);
        auto& slot=*scene.slots[i];const auto& entry=s.entries[i];auto& visual=slot.visual;
        slot.cancelling=entry.cancelling;
        if(manager.snapshot_exposed[i]||s.versions[i]!=manager.snapshot_versions[i]||slot.graze_state.version!=entry.graze.version)slot.graze_state=entry.graze;
        if(entry.active&&s.own_visuals)frame_values::copy_vm(visual.body,*entry.body_image);
        else if(entry.cached_body){
            if(manager.snapshot_exposed[i]||s.versions[i]!=manager.snapshot_versions[i]||visual.body.program_version!=s.body_motion[i].program_version)static_cast<AnmVmState&>(visual.body)=s.body_programs[i];
            Data::restore_body(visual.body,s.body_motion[i]);visual.body.error.clear();
            if(!visual.body.geometry.empty())visual.body.geometry.clear();
        }else if(!entry.active)entry.body_binding.restore(visual.body);
        if(entry.active&&(s.own_visuals||entry.cached_overlay)&&(s.own_visuals||manager.snapshot_exposed[i]||s.versions[i]!=manager.snapshot_versions[i]))frame_values::copy_vm(visual.overlay,*entry.overlay_image);else if(!entry.active)entry.overlay_binding.restore(visual.overlay);
        visual.resource=entry.resource;visual.rate=entry.rate;visual.error=entry.error;
    }
    // Retain the monotonic allocator, restore identities of the captured content.
    manager.snapshot_versions=s.versions;
    manager.snapshot_runtime_versions=s.runtime_versions;
    manager.free_head=s.free_head;manager.active_head=s.active_head;manager.context=s.context;manager.cancellation_rewards=s.rewards;
    manager.minimum_distance_squared=s.minimum_distance_squared;manager.world_paused=s.world_paused;manager.visible_count=s.visible_count;manager.draw_groups=s.draw_groups;manager.error=s.error;
    scene.cancellation_animations=s.cancellation_animations;scene.error=s.scene_error;scene.game_random=s.game_random;scene.visual_random=s.visual_random;return true;
}
}
