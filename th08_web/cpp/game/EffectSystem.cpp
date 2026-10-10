#include "EffectSystem.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/JournalTouch.hpp"
#include <new>
#endif
namespace th08 {
namespace {
using E=EffectState;using S=EffectSystem;
i32 small(E& e,S& s){return s.transforms.small_spark(e);}i32 large(E& e,S& s){return s.transforms.large_spark(e);}
i32 accelerate(E& e,S&){return EffectTransforms::accelerate(e);}i32 orbit_init(E& e,S&){return EffectTransforms::orbit(e);}i32 orbit(E& e,S&){return EffectSpace::orbit_step(e);}
i32 inward_init(E& e,S& s){return s.transforms.inward(e);}i32 inward60(E& e,S&){return EffectTransforms::inward60(e);}i32 inward240(E& e,S&){return EffectTransforms::inward240(e);}
i32 outward_init(E& e,S& s){return s.transforms.outward(e);}i32 outward(E& e,S&){return EffectTransforms::outward90(e);}
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
i32 follow(E& e,S& s){return s.follow_player(e);}
#else
i32 follow(E& e,S& s){return EffectTransforms::follow(e,s.environment.player);}
#endif
i32 ambient_init(E& e,S& s){return s.space.ambient(e);}i32 ambient(E& e,S& s){return s.space.ambient_step(e);}
i32 glow_init(E& e,S& s){return s.space.glow(e,false);}i32 glow(E& e,S& s){return s.space.glow_step(e,false);}i32 tall_init(E& e,S& s){return s.space.glow(e,true);}i32 tall(E& e,S& s){return s.space.glow_step(e,true);}
i32 alive(E&,S&){return 1;}i32 edge(E& e,S&){return EffectTransforms::edge(e);}
void ring_draw(E& e,S& s){s.geometry.arcade=s.arcade;s.geometry.draw(e);}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
i32 ring_init(E& e,S& s){return s.initialize_geometry(e,ring_draw);}i32 alternative_init(E& e,S& s){return s.initialize_geometry(e,ring_draw,true);}
#else
i32 ring_init(E& e,S&){return EffectGeometry::initialize(e,ring_draw);}i32 alternative_init(E& e,S&){return EffectGeometry::initialize(e,ring_draw,true);}
#endif
i32 ring(E& e,S&){return EffectTransforms::ring(e);}i32 detailed(E& e,S&){return EffectTransforms::ring_detailed(e);}i32 timed(E& e,S&){return EffectTransforms::ring_timed(e);}i32 alpha(E& e,S&){return EffectTransforms::ring_alpha(e);}i32 moon(E& e,S&){return EffectTransforms::moon(e);}
i32 pulse(E& e,S&){return EffectBomb::pulsing(e);}i32 expand(E& e,S&){return EffectBomb::expanding(e);}i32 quartic(E& e,S&){return EffectBomb::quartic(e);}
template<u32 V>i32 ripple(E& e,S&){return EffectBomb::ripple(e,V);}
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
template<bool R>i32 burst(E& e,S& s){return s.burst(e,R);}
template<bool R>i32 burst_init(E& e,S& s){
    const auto position=e.position,parameters=e.parameters;auto* replacement=s.replace_fixed(e,35,position,0xffffffff,&parameters);
    if(!replacement)return 0;
    replacement->update=burst<R>;replacement->segments=R?54:44;replacement->width=R?6:4;return 0;
}
#else
template<bool R>i32 burst(E& e,S& s){return s.bomb.burst(e,R);}
template<bool R>i32 burst_init(E& e,S& s){
    const auto position=e.position,parameters=e.parameters;s.fixed(35,position,e.slot,0xffffffff,&parameters);
    e.update=burst<R>;e.segments=R?54:44;e.width=R?6:4;return 0;
}
#endif
const EffectDefinition definitions[66]{
 {28,nullptr,nullptr},{29,nullptr,nullptr},{30,nullptr,nullptr},{31,accelerate,large},
 {36,accelerate,small},{37,accelerate,small},{38,accelerate,small},{39,accelerate,small},{40,accelerate,small},{41,accelerate,small},{42,accelerate,small},{43,accelerate,small},
 {44,nullptr,nullptr},{45,orbit,orbit_init},{45,orbit,orbit_init},{45,orbit,orbit_init},{0,nullptr,nullptr},
 {32,inward60,inward_init},{33,inward240,inward_init},{51,ambient,ambient_init},{56,nullptr,nullptr},{52,outward,outward_init},{54,follow,nullptr},{104,alive,nullptr},{104,alive,nullptr},
 {35,nullptr,nullptr},{53,outward,outward_init},{34,inward60,inward_init},{57,nullptr,nullptr},{58,nullptr,nullptr},{59,nullptr,nullptr},{60,nullptr,nullptr},
 {48,nullptr,nullptr},{49,nullptr,nullptr},{50,nullptr,nullptr},{88,ring,ring_init},{88,burst<false>,burst_init<false>},{92,burst<false>,burst_init<true>},
 {71,nullptr,nullptr},{76,ring,ring_init},{81,detailed,ring_init},{82,pulse,ring_init},
 {83,ripple<0>,ring_init},{83,ripple<1>,ring_init},{83,ripple<2>,ring_init},{83,ripple<3>,ring_init},{84,expand,ring_init},{72,nullptr,nullptr},{85,quartic,ring_init},{86,ring,ring_init},
 {80,timed,ring_init},{73,glow,glow_init},{77,ring,ring_init},{88,alpha,ring_init},{88,alpha,ring_init},{87,detailed,ring_init},{96,detailed,alternative_init},{55,nullptr,nullptr},
 {100,detailed,alternative_init},{78,ring,ring_init},{102,nullptr,edge},{103,nullptr,edge},{75,nullptr,nullptr},{74,tall,tall_init},{77,moon,ring_init},{98,detailed,alternative_init}
};
void add(Vec3& a,const Vec3& b){a.x=Scalar::add(a.x,b.x);a.y=Scalar::add(a.y,b.y);a.z=Scalar::add(a.z,b.z);}
}
EffectSystem::EffectSystem(EffectPoolState& s,EffectEnvironment& e,AnmExecutor& a,AnmRenderer& r,Rng& random,ScreenEffects& screen,DamageRegions& damage,GameValues& v,u16& flags)
 :anm(a),renderer(r),values(v),replay_flags(flags),state(s),environment(e),transforms(random,a.timing),space(random,a.timing,e),geometry(r),bomb(screen,damage)
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
 ,host_damage(damage)
#endif
{
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    player_owners[0]={&environment.player,&host_damage,true};
#endif
}
const EffectDefinition& EffectSystem::definition(u32 kind){static const EffectDefinition empty{-1,nullptr,nullptr};return kind<66?definitions[kind]:empty;}
void EffectSystem::release(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(rollback_journal&&(rollback_journal->IsFrameOpen()||rollback_journal->FrameCount())){invalid=true;return;}
    familiar_effect_view.reset();
    for(i32 i=effect_pool_layout::active_pool_begin;i<effect_pool_layout::active_object_count;++i)state.objects[i].vertices=nullptr;
    for(auto& vertices:geometry_storage)vertices.reset();
#else
    for(i32 i=effect_pool_layout::active_pool_begin;i<effect_pool_layout::active_object_count;++i)EffectGeometry::release(state.objects[i]);
#endif
}
void EffectSystem::reset(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(rollback_journal&&(rollback_journal->IsFrameOpen()||rollback_journal->FrameCount())){invalid=true;return;}
#endif
    release();std::memset(&state,0,sizeof(state));invalid=false;
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool EffectSystem::capture_slot(EffectState& e){
    if(!multiplayer::before_write(rollback_journal,e)){invalid=true;return false;}
    if(rollback_journal&&rollback_journal->IsFrameOpen()&&e.vertices&&
       !rollback_journal->Touch(e.vertices,258*sizeof(SpriteVertex))){invalid=true;return false;}
    return true;
}
void EffectSystem::release_geometry(EffectState& e){
    if(!capture_slot(e))return;
    const auto address=reinterpret_cast<std::uintptr_t>(&e),base=reinterpret_cast<std::uintptr_t>(state.objects);
    if(address<base||(address-base)%sizeof(EffectState)||
       (address-base)/sizeof(EffectState)>=geometry_storage.size()){invalid=true;return;}
    if(e.vertices&&e.vertices!=geometry_storage[(address-base)/sizeof(EffectState)].get()){invalid=true;return;}
    // Retain allocation through this stage, including speculative deletion and
    // slot reuse. The EffectState pointer and its payload can then both rewind.
    e.vertices=nullptr;
}
i32 EffectSystem::initialize_geometry(EffectState& e,EffectDraw callback,bool alternative){
    if(!capture_slot(e))return -1;
    const auto address=reinterpret_cast<std::uintptr_t>(&e),base=reinterpret_cast<std::uintptr_t>(state.objects);
    if(address<base||(address-base)%sizeof(EffectState)||
       (address-base)/sizeof(EffectState)>=geometry_storage.size()){invalid=true;return -1;}
    auto& storage=geometry_storage[(address-base)/sizeof(EffectState)];
    if(!storage)storage.reset(new(std::nothrow) SpriteVertex[258]{});
    if(!storage){invalid=true;return -1;}
    if(rollback_journal&&rollback_journal->IsFrameOpen()&&
       !rollback_journal->Touch(storage.get(),258*sizeof(SpriteVertex))){invalid=true;return -1;}
    return EffectGeometry::initialize_borrowed(e,storage.get(),callback,alternative);
}
#endif
void EffectSystem::begin(EffectState& e,i32 kind,u32 color,bool depth){
    e.active=1;e.kind=u8(kind);const auto& d=definition(kind);e.scriptIndex=i16(d.script);
    auto* file=state.base_animation;
    if(!file||d.script<0||u32(d.script)>=file->scriptCount){invalid=true;e.active=0;return;}
    anm.start(*file,e,file->scripts[d.script]);if(depth)e.zWriteDisabled=1;e.color1.d3dColor=i32(color);e.pos2={};e.update=d.update;
}
void EffectSystem::initialize(EffectState& e,i32 kind){const auto callback=definition(kind).initialize;if(callback&&callback(e,*this)!=0)e.active=0;}
EffectState* EffectSystem::spawn(i32 kind,Vec3 position,i32 count,u32 color,const Vec3* parameters){
    if(u32(kind)>=66){invalid=true;return &state.objects[effect_pool_layout::dummy_index];}
    for(i32 attempt=0;attempt<effect_pool_layout::active_pool_end-effect_pool_layout::active_pool_begin;++attempt){auto& e=state.objects[state.cursor];state.cursor=(state.cursor+1)%effect_pool_layout::active_pool_end;if(e.active)continue;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(!capture_slot(e))return &state.objects[effect_pool_layout::dummy_index];
        release_geometry(e);if(invalid)return &state.objects[effect_pool_layout::dummy_index];
#else
        EffectGeometry::release(e);
#endif
        std::memset(&e,0,sizeof(e));e.position=position;if(parameters)e.parameters=*parameters;begin(e,kind,color,!parameters);initialize(e,kind);
        if(--count==0){replay_flags|=0x400;return &e;}
    }replay_flags|=0x400;return &state.objects[effect_pool_layout::dummy_index];
}
EffectState* EffectSystem::fixed(i32 kind,Vec3 position,i32 slot,u32 color,const Vec3* parameters){
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    auto* e=group(slot);if(!e||u32(kind)>=66){invalid=true;return &state.objects[effect_pool_layout::dummy_index];}
    return fixed_at(slot,slot,kind,position,color,parameters);
#else
    auto* e=group(slot);if(!e||u32(kind)>=66){invalid=true;return &state.objects[effect_pool_layout::dummy_index];}
    EffectGeometry::release(*e);std::memset(e,0,sizeof(*e));e->slot=slot;e->position=position;if(parameters)e->parameters=*parameters;begin(*e,kind,color,true);initialize(*e,kind);replay_flags|=0x400;return e;
#endif
}
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
EffectState* EffectSystem::fixed_player(i32 seat,i32 local_slot,i32 kind,Vec3 position,u32 color,const Vec3* parameters){
    const auto address=player_effect_slots::fixed_for_player(seat,local_slot);
    if(!address||u32(kind)>=66||(seat>0&&!player_owners[size_t(seat)].bound)){invalid=true;return &state.objects[effect_pool_layout::dummy_index];}
    return fixed_at(address.relative_slot,address.local_slot,kind,position,color,parameters);
}
EffectState* EffectSystem::fixed_at(i32 relative_slot,i32 local_slot,i32 kind,Vec3 position,u32 color,const Vec3* parameters){
    const auto address=player_effect_slots::fixed_for_storage_index(player_effect_slots::fixed_pool_begin+relative_slot);
    if(!address||address.local_slot!=local_slot||u32(kind)>=66){invalid=true;return &state.objects[effect_pool_layout::dummy_index];}
    auto* e=&state.objects[address.storage_index];
    if(!capture_slot(*e))return &state.objects[effect_pool_layout::dummy_index];
    release_geometry(*e);if(invalid)return &state.objects[effect_pool_layout::dummy_index];
    std::memset(e,0,sizeof(*e));e->slot=local_slot;e->position=position;if(parameters)e->parameters=*parameters;begin(*e,kind,color,true);initialize(*e,kind);replay_flags|=0x400;return e;
}
i32 EffectSystem::effect_seat(const EffectState& effect)const{
    const auto begin=reinterpret_cast<uintptr_t>(state.objects+player_effect_slots::fixed_pool_begin);
    const auto address=reinterpret_cast<uintptr_t>(&effect);
    const auto bytes=uintptr_t(player_effect_slots::fixed_count)*sizeof(EffectState);
    if(address<begin||address>=begin+bytes||(address-begin)%sizeof(EffectState)!=0)return -1;
    const auto fixed=player_effect_slots::fixed_for_storage_index(i32(player_effect_slots::fixed_pool_begin+(address-begin)/sizeof(EffectState)));
    return fixed?fixed.seat:-1;
}
bool EffectSystem::player_owner(const EffectState& effect,const Vec3*& position,DamageRegions*& damage)const{
    const i32 seat=effect_seat(effect);
    if(seat<0){position=&environment.player;damage=&host_damage;return true;}
    const auto& owner=player_owners[size_t(seat)];
    if(!owner.bound||!owner.position||!owner.damage)return false;
    position=owner.position;damage=owner.damage;return true;
}
EffectState* EffectSystem::replace_fixed(EffectState& current,i32 kind,Vec3 position,u32 color,const Vec3* parameters){
    const auto begin=reinterpret_cast<uintptr_t>(state.objects+player_effect_slots::fixed_pool_begin);
    const auto address=reinterpret_cast<uintptr_t>(&current);
    const auto bytes=uintptr_t(player_effect_slots::fixed_count)*sizeof(EffectState);
    if(address<begin||address>=begin+bytes||(address-begin)%sizeof(EffectState)!=0||u32(kind)>=66){invalid=true;return nullptr;}
    const auto fixed=player_effect_slots::fixed_for_storage_index(i32(player_effect_slots::fixed_pool_begin+(address-begin)/sizeof(EffectState)));
    if(!fixed||(fixed.seat>0&&!player_owners[size_t(fixed.seat)].bound)){invalid=true;return nullptr;}
    return fixed_at(fixed.relative_slot,fixed.local_slot,kind,position,color,parameters);
}
bool EffectSystem::bind_player(i32 seat,const Vec3& position,DamageRegions& damage){
    if(seat<0||seat>=player_effect_slots::seat_count){invalid=true;return false;}
    player_owners[size_t(seat)]={&position,&damage,true};return true;
}
i32 EffectSystem::follow_player(EffectState& effect){
    const Vec3* position=nullptr;DamageRegions* damage=nullptr;
    if(!player_owner(effect,position,damage)){invalid=true;return 0;}
    (void)damage;return EffectTransforms::follow(effect,*position);
}
i32 EffectSystem::burst(EffectState& effect,bool rotating){
    const Vec3* position=nullptr;DamageRegions* damage=nullptr;
    if(!player_owner(effect,position,damage)){invalid=true;return 0;}
    (void)position;return bomb.burst(effect,rotating,*damage);
}
#endif
EffectState* EffectSystem::overlay(i32 kind,Vec3 position,i32 count,u32 color){
    if(u32(kind)>=66){invalid=true;return &state.objects[effect_pool_layout::dummy_index];}
    for(i32 i=effect_pool_layout::overlay_pool_begin;i<effect_pool_layout::fixed_pool_begin;++i){auto& e=state.objects[i];if(e.active)continue;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(!capture_slot(e))return &state.objects[effect_pool_layout::dummy_index];
        release_geometry(e);if(invalid)return &state.objects[effect_pool_layout::dummy_index];
#else
        EffectGeometry::release(e);
#endif
        e.draw=nullptr;e.layer=0;e.position=position;begin(e,kind,color,false);
        e.age.set(0);e.dying=0;e.fade_frames=0;e.parameters={};initialize(e,kind);if(--count==0){replay_flags|=0x400;return &e;}
    }replay_flags|=0x400;return &state.objects[effect_pool_layout::dummy_index];
}
void EffectSystem::shift_glows(const Vec3& offset){for(i32 i=effect_pool_layout::active_pool_begin;i<effect_pool_layout::active_pool_end;++i)if(state.objects[i].kind==51){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(!capture_slot(state.objects[i]))return;
#endif
    add(state.objects[i].world_position,offset);
}}
void EffectSystem::snapshot_presentation(){if(!presentation_marker.capture())return;for(i32 i=0;i<effect_pool_layout::object_count;++i){const auto& e=state.objects[i];auto& before=presentation_previous[size_t(i)];before.active=e.active!=0;if(before.active){before.position=e.position;before.center=e.center;before.radius=e.radius;before.angle=e.angle;before.width=e.width;before.height=e.height;before.angle_y=e.angle_y;before.frequency=e.frequency;before.segments=e.segments;before.age=e.age.current;before.kind=e.kind;before.visual.capture(e);before.projected_offset=e.posFinal;}}}
void EffectSystem::presentation_visual(const EffectState& source,EffectState& draw)const{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(familiar_effect_view.apply(source,draw))return;
#endif
    const size_t index=size_t(&source-state.objects);if(index>=effect_pool_layout::object_count)return;const auto& before=presentation_previous[index];
    if(!before.active||before.kind!=source.kind||source.age.current<before.age)return;
    using V=presentation::VisualSample;u32 owner_fields=0;
    // Glow tint is a continuous modulation of ANM color1. Boss-tracking glow
    // kinds also low-pass pos2 toward the boss every logical tick, so their
    // owner-driven offset needs an explicit presentation endpoint as well.
    // The orbit's death callback owns its expanding scale/fade; neither is a
    // palette flash.
    if(source.flag17)owner_fields|=V::Rgb2|V::Opacity2;
    if(source.kind==51||source.kind==63)owner_fields|=V::Offset;
    if(source.dying)owner_fields|=V::Scale|V::Opacity;
    if(source.kind==19)owner_fields|=V::Rotation;
    before.visual.apply(source,draw,presentation::world_alpha,V::Attributes|V::Offset,owner_fields);
    if((source.kind==51||source.kind==63)&&presentation::active)
        draw.posFinal=V::vector(before.projected_offset,source.posFinal,presentation::world_alpha);
}
Vec3 EffectSystem::presentation_position(const EffectState& e)const{
    if(!presentation::active)return e.position;const size_t index=size_t(&e-state.objects);if(index>=effect_pool_layout::object_count)return e.position;const auto& before=presentation_previous[index];
    const float dx=e.position.x-before.position.x,dy=e.position.y-before.position.y;if(!before.active||before.kind!=e.kind||e.age.current<before.age||dx*dx+dy*dy>=16384.0f)return e.position;
    return {presentation::lerp_world(before.position.x,e.position.x),presentation::lerp_world(before.position.y,e.position.y),presentation::lerp_world(before.position.z,e.position.z)};
}
EffectState EffectSystem::presentation_copy(const EffectState& source)const{
    EffectState draw=source;
    if(!presentation::render_only)return draw;
    draw.position=presentation_position(source);presentation_visual(source,draw);
    if(presentation::active)presentation_geometry(source,draw);
    return draw;
}
void EffectSystem::presentation_geometry(const EffectState& source,EffectState& draw)const{
    const size_t index=size_t(&source-state.objects);if(index>=effect_pool_layout::object_count)return;const auto& before=presentation_previous[index];
    const float dx=source.position.x-before.position.x,dy=source.position.y-before.position.y;if(!before.active||before.kind!=source.kind||source.age.current<before.age||dx*dx+dy*dy>=16384.0f)return;
    draw.center={presentation::lerp_world(before.center.x,source.center.x),presentation::lerp_world(before.center.y,source.center.y),presentation::lerp_world(before.center.z,source.center.z)};
    // Branch selectors and vertex layout are discrete. Blending height while
    // taking frequency/segments from the current endpoint can create a third
    // geometry mode that exists at neither endpoint. Keep the authoritative
    // current shape across a topology boundary; placement may still be smooth.
    if(!EffectGeometry::interpolation_preserves_topology(before.height,before.frequency,before.segments,source.height,source.frequency,source.segments))return;
    draw.radius=presentation::lerp_world(before.radius,source.radius);draw.width=presentation::lerp_world(before.width,source.width);draw.height=presentation::lerp_world(before.height,source.height);
    constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;
    auto angle=[&](float a,float b){if(presentation::world_alpha>=1)return b;if(presentation::world_alpha<=0)return a;float d=b-a;if(d>pi)d-=tau;else if(d<-pi)d+=tau;return add_angle(a+d*presentation::world_alpha,0);};
    draw.angle=angle(before.angle,source.angle);draw.angle_y=angle(before.angle_y,source.angle_y);
}
JobResult EffectSystem::update(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const i32 familiar_form=familiar_view_form?familiar_view_form(player_view):-1;
#endif
    state.active_count=0;for(u32 i=0;i<5;++i){state.tails[i]=&state.sentinels[i];state.sentinels[i].next=nullptr;}
    for(i32 i=effect_pool_layout::active_pool_begin;i<effect_pool_layout::active_object_count;++i){auto& e=state.objects[i];if(!e.active){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(e.vertices){release_geometry(e);if(invalid)return JobResult::Error;}
#else
        EffectGeometry::release(e);
#endif
        continue;}++state.active_count;
        if(!paused||e.ignore_pause){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            if(familiar_view_form)familiar_effect_view.update(e,familiar_form,anm.timing);
#endif
            if((e.update&&e.update(e,*this)!=1)||anm.execute(e)){e.active=0;continue;}e.age.tick(anm.timing);}
        e.next=nullptr;if(e.kind==64)continue;
        const u32 list=(i8(e.layer)==1||i8(e.layer)>2)?1:e.layer==0?(e.alternative?3:e.blendMode==1?4:0):2;
        state.tails[list]->next=&e;state.tails[list]=&e;
    }
    state.frames=wrapping_add(state.frames,1);return state.frames%300==100&&values.tampered()?JobResult::Exit:JobResult::Continue;
}
void EffectSystem::draw_list(u32 index,float depth,bool offset_before_depth){
    for(auto* e=state.sentinels[index].next;e;e=e->next){TH08_AUDIT_SCOPE(Effect,e,e->age.current,u32(e->kind));
        u8 alpha=255;
        bool local_familiar=false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const i32 seat=effect_seat(*e);if(seat>=0&&player_view_alpha)alpha=player_view_alpha(player_view,seat);
        local_familiar=familiar_effect_view.contains(*e);
#endif
        const auto fade=[alpha](EffectState& vm){vm.color1.a=std::min(vm.color1.a,alpha);vm.color2.a=std::min(vm.color2.a,alpha);};
        bool isolated=presentation::render_only||alpha<255||local_familiar;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        // Every endpoint must leave the same authoritative effect state.
        // Previously local full-opacity effects drew in place, while remote
        // and spectator effects drew copies. Renderer position/geometry caches
        // then depended on which player was viewing the world.
        isolated=true;
#endif
        if(e->draw){
            if(isolated){
                EffectState copy=*e;std::array<SpriteVertex,258> vertices{};if(e->vertices){std::memcpy(vertices.data(),e->vertices,sizeof(vertices));copy.vertices=vertices.data();}
                copy.position=presentation_position(*e);presentation_visual(*e,copy);fade(copy);if(presentation::active)presentation_geometry(*e,copy);copy.geometry_dirty=1;const bool invalid_before=geometry.invalid;e->draw(copy,*this);geometry.invalid=invalid_before;
            }else e->draw(*e,*this);
            continue;
        }EffectState copy;EffectState* draw=e;
        if(isolated){copy=*e;copy.position=presentation_position(*e);presentation_visual(*e,copy);fade(copy);draw=&copy;}
        draw->pos=draw->position;draw->pos.x=Scalar::add(arcade.x,draw->pos.x);draw->pos.y=Scalar::add(arcade.y,draw->pos.y);
        if(offset_before_depth){add(draw->pos,draw->pos2);draw->pos.z=depth;}else{draw->pos.z=depth;add(draw->pos,draw->pos2);}renderer.draw_2d(*draw);
    }
}
JobResult EffectSystem::draw(){draw_list(0,.07f,false);for(auto* e=state.sentinels[2].next;e;e=e->next){TH08_AUDIT_SCOPE(Effect,e,e->age.current,u32(e->kind));EffectState copy;EffectState* draw=e;if(presentation::render_only){copy=*e;copy.position=presentation_position(*e);presentation_visual(*e,copy);draw=&copy;}draw->pos=draw->position;renderer.draw_facing_camera(*draw);}draw_list(4,.07f,false);return JobResult::Continue;}
JobResult EffectSystem::draw_alternative(){draw_list(3,.04f,true);return JobResult::Continue;}
void EffectSystem::projected(AnmVm& vm,Vec3& position,void* p){
    // The authoritative Draw has already integrated this attraction once.
    // Extra Draws read its endpoint instead of integrating it a second time.
    if(presentation::render_only){add(position,vm.posFinal);return;}
    static_cast<EffectSystem*>(p)->space.projected(vm,position);
}
JobResult EffectSystem::draw_background(){
    // Quality 1 returns before its first object, as in 4281e0; the odd/even
    // check is a return from the loop, not a skip to the next particle.
    if(quality<2)return JobResult::Continue;
    for(auto* e=state.sentinels[1].next;e;e=e->next){TH08_AUDIT_SCOPE(Effect,e,e->age.current,u32(e->kind));EffectState copy;EffectState* draw=e;if(presentation::render_only){copy=*e;copy.position=presentation_position(*e);presentation_visual(*e,copy);draw=&copy;}draw->pos=draw->position;if(draw->layer==4)renderer.draw_2d(*draw);else if(draw->layer==1)renderer.draw_facing_camera(*draw,(draw->kind==51||draw->kind==63)?projected:nullptr,this);else renderer.draw_world(*draw);}
    return JobResult::Continue;
}
#if defined(TH_PRESENTATION_AUDIT)
const float* EffectSystem::audit_presentation_sample(uintptr_t object)const{
    static float out[16];std::fill(out,out+16,0.0f);
    const auto* begin=state.objects;const auto* end=state.objects+effect_pool_layout::object_count;const auto* current=reinterpret_cast<const EffectState*>(object);
    if(current<begin||current>=end)return out;const size_t index=size_t(current-begin);const auto& before=presentation_previous[index];
    out[0]=before.active?1.0f:0.0f;out[1]=float(before.age);out[2]=float(before.kind);out[3]=before.visual.scale.x;out[4]=before.visual.scale.y;
    out[5]=float(before.visual.color1.a);out[6]=float(before.visual.continuous);out[7]=before.position.x;out[8]=before.position.y;
    out[9]=current->scale.x;out[10]=current->scale.y;out[11]=float(current->color1.a);out[12]=float(presentation::VisualSample::continuous_fields(*current));
    out[13]=current->position.x;out[14]=current->position.y;out[15]=float(presentation_marker.last_epoch&0xffffffu);return out;
}
#endif
}
