#include "../platform/BrowserRuntime.hpp"
#include "CanonicalContext.hpp"
#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

namespace th08::multiplayer {
namespace {
#ifdef TH_MULTIPLAYER_FIXTURES
std::array<std::vector<u8>,11> canonical_traces;
#ifdef TH_MULTIPLAYER_FIXTURES
struct CanonicalResourceTraceSpan {u32 begin=0,size=0,kind=0; i32 resource=-1;};
std::vector<CanonicalResourceTraceSpan> canonical_resource_trace_spans;
void CanonicalResourceTraceMark(u32 kind,i32 resource,u32 begin){
    const auto end=canonical_traces[9].size();
    if(end>begin)canonical_resource_trace_spans.push_back({begin,u32(end-begin),kind,resource});
}
#endif
struct CanonicalPlayerRegion {std::size_t begin=0,size=0;};
std::vector<CanonicalPlayerRegion> canonical_player_regions,canonical_player_regions_before;
u32 CanonicalPlayerFieldInternal(u32 trace_offset){
    u32 local=~u32(0);
    for(const auto& region:canonical_player_regions_before)if(trace_offset>=region.begin&&trace_offset-region.begin<region.size){local=u32(trace_offset-region.begin);break;}
    if(local==~u32(0))return 90; // PlayerState trace metadata, not object bytes.
#define PLAYER_FIELD(member,id) if(local>=offsetof(PlayerSimulationState,member)&&local<offsetof(PlayerSimulationState,member)+sizeof(((PlayerSimulationState*)nullptr)->member))return id
    PLAYER_FIELD(analog,1);PLAYER_FIELD(touch_remainder,2);PLAYER_FIELD(unlimited_movement_used,3);
    PLAYER_FIELD(motion,4);PLAYER_FIELD(life,5);PLAYER_FIELD(bomb,7);
    if(local>=offsetof(PlayerSimulationState,bomb_input)&&local<offsetof(PlayerSimulationState,bomb_input)+sizeof(((PlayerSimulationState*)nullptr)->bomb_input)){
        const u32 nested=local-u32(offsetof(PlayerSimulationState,bomb_input));
#define BOMB_INPUT_FIELD(member,id) if(nested>=offsetof(PlayerBombContext,member)&&nested<offsetof(PlayerBombContext,member)+sizeof(((PlayerBombContext*)nullptr)->member))return id
        BOMB_INPUT_FIELD(buttons,101);BOMB_INPUT_FIELD(previous_buttons,102);BOMB_INPUT_FIELD(gui_blocked,103);
        BOMB_INPUT_FIELD(tampered,104);BOMB_INPUT_FIELD(cheats,105);BOMB_INPUT_FIELD(reserved,106);
        BOMB_INPUT_FIELD(regular_bombs,107);BOMB_INPUT_FIELD(last_spells,108);
        if(nested>=offsetof(PlayerBombContext,bosses)&&nested<offsetof(PlayerBombContext,bosses)+sizeof(((PlayerBombContext*)nullptr)->bosses))
            return 109+(nested-u32(offsetof(PlayerBombContext,bosses)))/sizeof(PlayerBombBoss*);
#undef BOMB_INPUT_FIELD
        return 8;
    }
    if(local>=offsetof(PlayerSimulationState,context)&&local<offsetof(PlayerSimulationState,context)+sizeof(((PlayerSimulationState*)nullptr)->context)){
        const u32 nested=local-u32(offsetof(PlayerSimulationState,context));
#define LIFE_CONTEXT_FIELD(member,id) if(nested>=offsetof(PlayerLifeContext,member)&&nested<offsetof(PlayerLifeContext,member)+sizeof(((PlayerLifeContext*)nullptr)->member))return id
        LIFE_CONTEXT_FIELD(game_flags,201);LIFE_CONTEXT_FIELD(hud_flags,202);LIFE_CONTEXT_FIELD(pause,203);
        LIFE_CONTEXT_FIELD(bombs,204);LIFE_CONTEXT_FIELD(lives,205);LIFE_CONTEXT_FIELD(power,206);
        LIFE_CONTEXT_FIELD(time_orbs,207);LIFE_CONTEXT_FIELD(last_spell_requirement,208);LIFE_CONTEXT_FIELD(extent,209);
        LIFE_CONTEXT_FIELD(replay_flags,210);LIFE_CONTEXT_FIELD(character,211);LIFE_CONTEXT_FIELD(focused,212);
        LIFE_CONTEXT_FIELD(time_spell,213);LIFE_CONTEXT_FIELD(game_over,214);LIFE_CONTEXT_FIELD(miss_control,215);
        LIFE_CONTEXT_FIELD(cheats,216);LIFE_CONTEXT_FIELD(gauge,217);LIFE_CONTEXT_FIELD(padding,218);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        LIFE_CONTEXT_FIELD(challenge_mode,219);
#endif
#undef LIFE_CONTEXT_FIELD
        return 6;
    }
    PLAYER_FIELD(shots,9);PLAYER_FIELD(bomb_objects,10);PLAYER_FIELD(frame,11);
    PLAYER_FIELD(input,12);PLAYER_FIELD(enemy_origin,13);PLAYER_FIELD(cancel_item,14);PLAYER_FIELD(item_gauge_lock,15);
    PLAYER_FIELD(stage_play_frames,16);
#undef PLAYER_FIELD
    return 0xffffffffu;
}
#endif
struct Hash {
    u32 value=2166136261u;
#ifdef TH_MULTIPLAYER_FIXTURES
    std::vector<u8>* trace=nullptr;
#endif
    void bytes(const void* p,std::size_t n){auto* b=static_cast<const u8*>(p);
#ifdef TH_MULTIPLAYER_FIXTURES
        if(trace)trace->insert(trace->end(),b,b+n);
#endif
        for(std::size_t i=0;i<n;++i){value^=b[i];value*=16777619u;}}
    template<class T>void add(const T& v){static_assert(std::is_trivially_copyable_v<T>);bytes(&v,sizeof(v));}
};
// Diagnostic pointer tokens only. These copies are never executed or restored.
// Object identities are native pool slots; resource identities are file indices
// and byte offsets. Invalid references have a distinct sentinel, not an address.
template<class T>T* token(u32 v){return reinterpret_cast<T*>(std::uintptr_t(v));}
u32 offset(const void* p,const void* base,std::size_t size){
    if(!p)return 0;const auto a=std::uintptr_t(p),b=std::uintptr_t(base);
    return base&&a>=b&&a-b<size?u32(a-b)+1:~u32(0);
}
template<class T,std::size_t N>u32 slot(const void* p,const T (&a)[N]){return offset(p,a,sizeof(a));}
void anm(AnmVm& v){
    const auto* file=v.anmFile;const auto* beginning=v.beginningOfScript;
    const auto instruction=[&](const void* p){
        if(!p)return u32(0);if(!beginning)return ~u32(0);
        return u32(std::uintptr_t(p)-std::uintptr_t(beginning))+1;
    };
    v.currentInstruction=token<AnmRawInstr>(instruction(v.currentInstruction));
    v.interruptReturnInstruction=token<AnmRawInstr>(instruction(v.interruptReturnInstruction));
    v.loadedSprite=token<AnmLoadedSprite>(file?offset(v.loadedSprite,file->sprites,std::size_t(file->spriteCount)*sizeof(AnmLoadedSprite)):v.loadedSprite?~u32(0):0);
    v.anmFile=token<AnmLoaded>(file?u32(file->anmIdx)+1:0);
    v.beginningOfScript=token<AnmRawInstr>(beginning?1:0);
}
struct StateHash {
    GameplayScene& g;
    u32 effect(const void* p)const{
        if(!p)return 0;auto id=slot(p,g.effect_pool.objects);
        return id!=~u32(0)?id:0x10000000u+slot(p,g.effect_pool.sentinels);
    }
    u32 enemy(const EclVm* p)const{return p?u32(p->pool_index)+1:0;}
    void definition(const ShotDefinition*& p,Hash& h){const bool present=p;h.add(present);if(p)h.add(*p);p=nullptr;}
    void player(const PlayerSimulationState& source,Hash& h){
        auto storage=std::make_unique<PlayerSimulationState>(source);auto& p=*storage;
        anm(p.motion.animation);for(auto& o:p.motion.options)anm(o.animation);
        p.motion.form.focus_effect=token<AnmVm>(effect(p.motion.form.focus_effect));
        p.motion.gauge.effect=token<EffectState>(effect(p.motion.gauge.effect));
        p.life.invincible_effect=token<EffectState>(effect(p.life.invincible_effect));
        p.life.predead_effect=token<EffectState>(effect(p.life.predead_effect));
        for(auto& b:p.bomb_input.bosses){const bool present=b;h.add(present);if(b)h.add(*b);b=nullptr;}
        for(auto& shot:p.shots.shots){anm(shot.animation);definition(shot.definition,h);}
        for(auto& l:p.shots.lasers)l.shot=token<PlayerShot>(slot(l.shot,source.shots.shots));
        for(auto& d:p.shots.laser_definitions)definition(d,h);
        for(auto& b:p.bomb_objects.objects){
            for(auto& v:b.animation)anm(v);
            b.effect=token<EffectState>(effect(b.effect));
            b.damage=token<DamageRegion>(slot(b.damage,source.shots.regions.damaging));
            b.cancel=token<DamageRegion>(slot(b.cancel,source.shots.regions.cancelling));
        }
        p.frame.target_reference=token<void>(enemy(static_cast<const EclVm*>(p.frame.target_reference)));
#ifdef TH_MULTIPLAYER_FIXTURES
        const auto trace_begin=h.trace?h.trace->size():0;
#endif
        h.add(p);
#ifdef TH_MULTIPLAYER_FIXTURES
        if(h.trace)canonical_player_regions.push_back({trace_begin,sizeof(p)});
#endif
    }
    void context(EclContext& c){
        NormalizeContextPadding(c);
        const auto instruction=[&](const EclInstruction* p){return token<EclInstruction>(offset(p,g.program.data(),g.program.size()));};
        c.instruction=instruction(c.instruction);c.branch_instruction=instruction(c.branch_instruction);
        c.native_instruction=instruction(c.native_instruction);
        for(auto& p:c.call_stack)p=instruction(p);
        for(auto& frame:c.call_frames)frame.native_instruction=instruction(frame.native_instruction);
    }
    void ecl(const EclVm& source,Hash& h){
        EclVm e=source;
        // EclVm has non-trivial asynchronous owners. Its C++ copy constructor
        // does not copy padding (notably the byte after `invalid`), so hashing
        // that copy can hash stack residue left by an extra presentation.
        // Preserve the native representation of the two trivially-owned
        // ranges, without byte-copying the shared_ptr ownership words. Only
        // pointer tokens below differ from the observed world.
        constexpr auto prefix=offsetof(EclVm,asynchronous);
        constexpr auto tail=offsetof(EclVm,asynchronous_generations);
        std::memcpy(&e,&source,prefix);
        std::memcpy(reinterpret_cast<u8*>(&e)+tail,reinterpret_cast<const u8*>(&source)+tail,sizeof(e)-tail);
        // Allocation/reuse also leaves padding in the source indeterminate.
        // Clear only layout gaps, never an ECL variable or animation field.
        const auto gap=[&](std::size_t first,std::size_t last){std::memset(reinterpret_cast<u8*>(&e)+first,0,last-first);};
        gap(offsetof(EclVm,failure)+sizeof(e.failure),offsetof(EclVm,program));
        gap(offsetof(EclVm,pose_direction)+sizeof(e.pose_direction),offsetof(EclVm,emitter));
        gap(offsetof(EclVm,difficulty_flags)+sizeof(e.difficulty_flags),offsetof(EclVm,angular_velocity));
        gap(offsetof(EclVm,invalid)+sizeof(e.invalid),offsetof(EclVm,damage_protection));
        gap(offsetof(EclVm,death_effects)+sizeof(e.death_effects),offsetof(EclVm,previous_familiar));
        gap(offsetof(EclVm,hit_flash)+sizeof(e.hit_flash),offsetof(EclVm,trail));
        context(e.main_context);e.program=nullptr;e.random=nullptr;e.environment=nullptr;e.active_context=nullptr;
        e.parent=token<EclVm>(enemy(e.parent));e.next_familiar=token<EclVm>(enemy(e.next_familiar));
        e.previous_familiar=token<EclVm>(enemy(e.previous_familiar));e.next_in_layer=token<EclVm>(enemy(e.next_in_layer));
        for(auto& v:e.animation)anm(v);
        e.emitter.type=token<BulletTemplate>(slot(e.emitter.type,g.projectile_pool.templates.types));
        e.laser_emitter.type=token<BulletTemplate>(slot(e.laser_emitter.type,g.projectile_pool.templates.types));
        for(auto& l:e.laser_slots)l=token<LaserState>(slot(l,g.projectile_pool.lasers));
        for(auto& v:e.effects)v=token<EffectState>(effect(v));
        e.familiar_effect=token<AnmVm>(effect(e.familiar_effect));
        h.bytes(&e,offsetof(EclVm,asynchronous));
        h.bytes(reinterpret_cast<const u8*>(&e)+tail,sizeof(e)-tail);
        for(const auto& p:source.asynchronous){const bool present=bool(p);h.add(present);if(p){auto c=*p;context(c);h.add(c);}}
    }
};
void pilot_resources(Hash& h,const PilotResources& p){
    h.add(p.lives);h.add(p.bombs);h.add(p.power);
    h.add(p.gauge);h.add(p.gauge_copy);
    h.add(p.deaths);h.add(p.deaths_stage);h.add(p.bombs_used);h.add(p.bombs_used_stage);
}
void cooperation(Hash& h,const CooperativeState& c){
    h.add(c.count);h.add(c.wipe_progress);h.add(c.retry_pending);
    for(const auto& s:c.seats){
        h.add(s.spirit);h.add(s.waiting_for_focus_release);
        h.add(s.progress);h.add(s.power_taps);h.add(s.power_window);
        h.add(s.target);h.add(s.drift_x);h.add(s.drift_y);
    }
}
}
}
namespace th08 {
#ifdef TH_MULTIPLAYER_FIXTURES
extern "C" __attribute__((export_name("mp_fixture_enemy_canonical")))
const u32* mp_fixture_enemy_canonical(BrowserRuntime* runtime,u32 index){
    using namespace multiplayer;
    static std::vector<u32> out;out.assign(16,0);
    if(!runtime||!runtime->app.in_game()||index>481)return out.data();
    auto& g=runtime->app.game;StateHash world{g};Hash h;std::vector<u8> bytes;h.trace=&bytes;
    if(index==481){
        auto manager=g.enemies.state;
        for(auto& layer:manager.layers)layer=token<EclVm>(world.enemy(layer));
        for(auto& timeline:manager.timelines){timeline.program=nullptr;timeline.instruction=token<EclTimelineInstruction>(offset(timeline.instruction,g.program.data(),g.program.size()));}
        h.add(manager);
    }else{
        const auto* enemy=g.enemies.population.at(index);if(!enemy)return out.data();
        world.ecl(*enemy,h);
    }
    out[0]=1;out[1]=bytes.size();out[2]=h.value;out[3]=offsetof(EclVm,asynchronous);
    out[4]=offsetof(EclVm,asynchronous_generations);out[5]=sizeof(EclVm);
    out[6]=offsetof(EclVm,animation);out[7]=sizeof(AnmVm);out[8]=offsetof(EclVm,resolved_position);
    out[9]=offsetof(EclVm,position);out[10]=offsetof(EclVm,trail);
    out.resize(16+(bytes.size()+3)/4);std::memcpy(out.data()+16,bytes.data(),bytes.size());return out.data();
}
#endif
extern "C" __attribute__((export_name("multiplayer_canonical_state")))
const u32* multiplayer_canonical_state(BrowserRuntime* runtime){
    using namespace multiplayer;
    static u32 result[13]{};std::fill(result,result+13,0);result[0]=1;
#ifdef TH_MULTIPLAYER_FIXTURES
    for(auto& trace:canonical_traces)trace.clear();
    canonical_resource_trace_spans.clear();
    canonical_player_regions.clear();
#endif
    if(!runtime||!runtime->app.in_game())return result;
    auto& a=runtime->app;auto& g=a.game;StateHash world{g};Hash h[11];
#ifdef TH_MULTIPLAYER_FIXTURES
    for(u32 i=0;i<11;++i)h[i].trace=&canonical_traces[i];
#endif
    auto economy=a.session.numbers;economy.high_score=0;economy.high_score_retries=0;
    h[0].add(economy);
    for(const auto& p:a.session.pilot_resources)pilot_resources(h[0],p);
    h[0].add(a.session.rank);cooperation(h[0],g.cooperation);h[0].add(a.session.multiplayer_route_state);
    h[0].add(g.committed_buttons);h[0].add(g.previous_buttons);
    h[0].add(a.session.multiplayer_cheat_movement_used);
    for(u32 seat=0;seat<a.session.player_count;++seat)world.player(g.pilot(seat).status(),h[1]);
    h[2].add(a.session.random);
    auto manager=g.enemies.state;
    for(auto& layer:manager.layers)layer=token<EclVm>(world.enemy(layer));
    for(auto& timeline:manager.timelines){timeline.program=nullptr;timeline.instruction=token<EclTimelineInstruction>(offset(timeline.instruction,g.program.data(),g.program.size()));}
    h[3].add(manager);
    for(u32 i=0;i<481;++i){const auto* e=g.enemies.population.at(i);const bool exists=e!=nullptr;h[3].add(exists);if(e)world.ecl(*e,h[3]);}
    auto& bullets=g.projectile_pool;
    h[4].add(bullets.active_count);h[4].add(bullets.timer);h[4].add(bullets.cancel_frames);h[4].add(bullets.unknown_counter);
    h[4].add(slot(bullets.next_slot,bullets.bullets));
    for(auto* layer:bullets.layers)h[4].add(slot(layer,bullets.bullets));
    for(const auto& source:bullets.bullets){h[4].add(source.state);if(source.state){auto b=source;for(auto& v:b.sprites.animation)anm(v);b.next_in_layer=token<BulletState>(slot(b.next_in_layer,bullets.bullets));h[4].add(b);}}
    for(const auto& source:bullets.lasers){h[5].add(source.in_use);if(source.in_use){auto l=source;for(auto& v:l.animation)anm(v);h[5].add(l);}}
    const auto& pool=g.items.status();h[6].add(pool.count);h[6].add(pool.next_index);
    for(const auto& source:pool.items){h[6].add(source.active);if(source.active){auto i=source;anm(i.animation);i.next=token<ItemState>(slot(i.next,pool.items));i.previous=token<ItemState>(slot(i.previous,pool.items));h[6].add(i);}}
    for(u32 seat=0;seat<3;++seat)h[6].add(g.items.assigned_gifts(seat));
    const auto& effects=g.effect_pool;h[7].add(effects.cursor);h[7].add(effects.active_count);h[7].add(effects.frames);
    for(const auto* tail:effects.tails)h[7].add(world.effect(tail));
    for(const auto& source:effects.objects){h[7].add(source.active);if(source.active){auto e=source;anm(e);e.vertices=nullptr;e.next=token<EffectState>(world.effect(e.next));h[7].add(e);if(source.vertices)h[7].bytes(source.vertices,258*sizeof(SpriteVertex));}}
    h[8].add(a.supervisor.state.active);h[8].add(a.supervisor.state.target);h[8].add(g.globals.stage);
    h[8].add(g.control.state);h[8].add(g.time_stopped);h[8].add(g.paused);h[8].add(g.retrying);h[8].add(a.supervisor.input);
    const auto trace_h9=[&](u32 kind,i32 resource,const void* bytes,std::size_t count){
#ifdef TH_MULTIPLAYER_FIXTURES
        const auto begin=u32(canonical_traces[9].size());
#endif
        h[9].bytes(bytes,count);
#ifdef TH_MULTIPLAYER_FIXTURES
        CanonicalResourceTraceMark(kind,resource,begin);
#endif
    };
    trace_h9(1,-1,g.program.data(),g.program.size());
    auto& program=g.background_script.program;trace_h9(2,-1,program.header(),program.bytes_size());
    trace_h9(3,-1,g.dialogue.program.bytes(),g.dialogue.program.size());
    for(i32 index=0;index<256;++index)if(const auto* resource=a.library.resource(index)){
        trace_h9(4,index,&index,sizeof(index));
        const auto& bytes=resource->data();trace_h9(5,index,bytes.data(),bytes.size());
        if(const auto* loaded=a.library.get(index)){
            if(loaded->spriteCount){
                std::vector<AnmLoadedSprite> sprites(loaded->sprites,loaded->sprites+loaded->spriteCount);
                // TextureStore handles are instance-local presentation IDs. Keep
                // every ANM sprite scalar in the canonical trace except this
                // renderer-only binding; AnmRenderer uses it for bind/readback.
                static_assert(offsetof(AnmLoadedSprite,texture)==4);
                for(auto& sprite:sprites)sprite.texture=0;
                trace_h9(6,index,sprites.data(),sprites.size()*sizeof(AnmLoadedSprite));
            }
        }
    }
    auto spell=static_cast<const SpellState&>(g.globals);
    spell.spell_enemy=token<EclVm>(world.enemy(spell.spell_enemy));
    spell.spell_effect=token<EffectState>(world.effect(spell.spell_effect));spell.spell_reward_effect=token<EffectState>(world.effect(spell.spell_reward_effect));
    for(auto& v:spell.spell_vms)anm(v);
    const auto file=[](AnmLoaded* v){return token<AnmLoaded>(v?u32(v->anmIdx)+1:0);};
    spell.spell_human_face=file(spell.spell_human_face);spell.spell_youkai_face=file(spell.spell_youkai_face);
    spell.spell_enemy_face=file(spell.spell_enemy_face);spell.spell_enemy_face2=file(spell.spell_enemy_face2);spell.spell_banners=file(spell.spell_banners);
    h[10].add(spell);h[10].add(g.globals.game_flags);h[10].add(g.background.camera);h[10].add(g.background.fog);
    Hash composite;for(u32 i=0;i<11;++i){result[i+2]=h[i].value;composite.add(result[i+2]);}result[1]=composite.value;return result;
}
#ifdef TH_MULTIPLAYER_FIXTURES
namespace multiplayer {
const std::vector<u8>* CanonicalTrace(u32 group){return group<canonical_traces.size()?&canonical_traces[group]:nullptr;}
void SnapshotCanonicalPlayerRegions(){canonical_player_regions_before=canonical_player_regions;}
u32 CanonicalPlayerField(u32 offset){return CanonicalPlayerFieldInternal(offset);}
u32 CanonicalPlayerSeat(u32 trace_offset){
    for(u32 seat=0;seat<canonical_player_regions_before.size();++seat){
        const auto& region=canonical_player_regions_before[seat];
        if(trace_offset>=region.begin&&trace_offset-region.begin<region.size)return seat;
    }
    return ~u32(0);
}
u32 CanonicalPlayerFieldByteOffset(u32 trace_offset){
    u32 local=~u32(0);
    for(const auto& region:canonical_player_regions_before)if(trace_offset>=region.begin&&trace_offset-region.begin<region.size){local=u32(trace_offset-region.begin);break;}
    if(local==~u32(0))return 0xffu;
#define PLAYER_MEMBER_OFFSET(member) if(local>=offsetof(PlayerSimulationState,member)&&local<offsetof(PlayerSimulationState,member)+sizeof(((PlayerSimulationState*)nullptr)->member))return local-u32(offsetof(PlayerSimulationState,member))
    PLAYER_MEMBER_OFFSET(analog);PLAYER_MEMBER_OFFSET(touch_remainder);PLAYER_MEMBER_OFFSET(unlimited_movement_used);
    PLAYER_MEMBER_OFFSET(motion);PLAYER_MEMBER_OFFSET(life);PLAYER_MEMBER_OFFSET(bomb);
    PLAYER_MEMBER_OFFSET(shots);PLAYER_MEMBER_OFFSET(bomb_objects);PLAYER_MEMBER_OFFSET(frame);PLAYER_MEMBER_OFFSET(input);
    PLAYER_MEMBER_OFFSET(enemy_origin);PLAYER_MEMBER_OFFSET(cancel_item);PLAYER_MEMBER_OFFSET(item_gauge_lock);PLAYER_MEMBER_OFFSET(stage_play_frames);
#undef PLAYER_MEMBER_OFFSET
    if(local>=offsetof(PlayerSimulationState,bomb_input)&&local<offsetof(PlayerSimulationState,bomb_input)+sizeof(((PlayerSimulationState*)nullptr)->bomb_input))
        return local-u32(offsetof(PlayerSimulationState,bomb_input));
    if(local>=offsetof(PlayerSimulationState,context)&&local<offsetof(PlayerSimulationState,context)+sizeof(((PlayerSimulationState*)nullptr)->context)){
        const u32 nested=local-u32(offsetof(PlayerSimulationState,context));
#define LIFE_CONTEXT_OFFSET(member) if(nested>=offsetof(PlayerLifeContext,member)&&nested<offsetof(PlayerLifeContext,member)+sizeof(((PlayerLifeContext*)nullptr)->member))return nested-u32(offsetof(PlayerLifeContext,member))
        LIFE_CONTEXT_OFFSET(game_flags);LIFE_CONTEXT_OFFSET(hud_flags);LIFE_CONTEXT_OFFSET(pause);LIFE_CONTEXT_OFFSET(bombs);
        LIFE_CONTEXT_OFFSET(lives);LIFE_CONTEXT_OFFSET(power);LIFE_CONTEXT_OFFSET(time_orbs);LIFE_CONTEXT_OFFSET(last_spell_requirement);
        LIFE_CONTEXT_OFFSET(extent);LIFE_CONTEXT_OFFSET(replay_flags);LIFE_CONTEXT_OFFSET(character);LIFE_CONTEXT_OFFSET(focused);
        LIFE_CONTEXT_OFFSET(time_spell);LIFE_CONTEXT_OFFSET(game_over);LIFE_CONTEXT_OFFSET(miss_control);LIFE_CONTEXT_OFFSET(cheats);
        LIFE_CONTEXT_OFFSET(gauge);LIFE_CONTEXT_OFFSET(padding);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        LIFE_CONTEXT_OFFSET(challenge_mode);
#endif
#undef LIFE_CONTEXT_OFFSET
    }
    return 0;
}
std::array<u32,2> CanonicalSupervisorField(u32 trace_offset){
    using State=GameplayControlState;
    std::size_t base=0;
    const auto field=[&](std::size_t size,u32 id)->std::array<u32,2>{
        if(trace_offset>=base&&trace_offset-base<size)return {id,u32(trace_offset-base)};
        base+=size;return {0,0};
    };
    if(auto v=field(sizeof(i32),1);v[0])return v; // supervisor.active
    if(auto v=field(sizeof(i32),2);v[0])return v; // supervisor.target
    if(auto v=field(sizeof(u32),3);v[0])return v; // game.stage
    if(trace_offset>=base&&trace_offset-base<sizeof(State)){
        const auto local=trace_offset-base;
#define CONTROL_FIELD(member,id) if(local>=offsetof(State,member)&&local<offsetof(State,member)+sizeof(((State*)nullptr)->member))return {id,u32(local)-u32(offsetof(State,member))}
        CONTROL_FIELD(frames,10);CONTROL_FIELD(restore_viewport,11);CONTROL_FIELD(next_scene,12);CONTROL_FIELD(load_state,13);
        CONTROL_FIELD(load_frames,14);CONTROL_FIELD(start_music,15);CONTROL_FIELD(slow_frames,16);CONTROL_FIELD(play_frames,17);
        CONTROL_FIELD(demo_frames,18);CONTROL_FIELD(demo_index,19);CONTROL_FIELD(replay_mode,20);CONTROL_FIELD(stage_mask,21);
        CONTROL_FIELD(sticky_input,22);CONTROL_FIELD(loading_images_setup,23);
#undef CONTROL_FIELD
        return {90,u32(local)}; // Unnamed bytes within GameplayControlState.
    }
    base+=sizeof(State);
    if(auto v=field(sizeof(bool),30);v[0])return v; // game.time_stopped
    if(auto v=field(sizeof(bool),31);v[0])return v; // game.paused
    if(auto v=field(sizeof(bool),32);v[0])return v; // game.retrying
    using Input=InputFrame;
    if(trace_offset>=base&&trace_offset-base<sizeof(Input)){
        const auto local=trace_offset-base;
#define INPUT_FIELD(member,id) if(local>=offsetof(Input,member)&&local<offsetof(Input,member)+sizeof(((Input*)nullptr)->member))return {id,u32(local)-u32(offsetof(Input,member))}
        INPUT_FIELD(current,40);INPUT_FIELD(previous,41);INPUT_FIELD(scrolling,42);INPUT_FIELD(held,43);
#undef INPUT_FIELD
        return {90,u32(local)};
    }
    return {90,trace_offset>=base?u32(trace_offset-base):trace_offset};
}
u32 CanonicalResourceTraceLocation(u32 trace_offset,u32* out){
    if(!out)return 0;std::fill(out,out+4,~u32(0));
    for(const auto& span:canonical_resource_trace_spans){
        if(trace_offset<span.begin||trace_offset-span.begin>=span.size)continue;
        const auto relative=trace_offset-span.begin;out[0]=span.kind;out[1]=u32(span.resource);out[2]=relative;
        if(span.kind==6){
            const auto within=relative%sizeof(AnmLoadedSprite);
#define ANM_SPRITE_FIELD(member,id) if(within>=offsetof(AnmLoadedSprite,member)&&within<offsetof(AnmLoadedSprite,member)+sizeof(((AnmLoadedSprite*)nullptr)->member))out[3]=id
            ANM_SPRITE_FIELD(anmIdx,1);ANM_SPRITE_FIELD(texture,2);ANM_SPRITE_FIELD(startPixelInclusive,3);
            ANM_SPRITE_FIELD(endPixelInclusive,4);ANM_SPRITE_FIELD(height,5);ANM_SPRITE_FIELD(width,6);
            ANM_SPRITE_FIELD(uvStart,7);ANM_SPRITE_FIELD(uvEnd,8);ANM_SPRITE_FIELD(heightPx,9);
            ANM_SPRITE_FIELD(widthPx,10);ANM_SPRITE_FIELD(scaleFactor,11);ANM_SPRITE_FIELD(unk0x40,12);
#undef ANM_SPRITE_FIELD
        }
        return 1;
    }
    return 0;
}
}
#endif
#ifdef TH_MULTIPLAYER_FIXTURES
extern "C" __attribute__((export_name("mp_fixture_canonical_group_bytes")))
const u32* mp_fixture_canonical_group_bytes(BrowserRuntime* runtime,u32 group){
    using namespace multiplayer;
    static std::vector<u32> words;words.clear();
    if(!runtime||group>=11){words.push_back(0);return words.data();}
    if(!multiplayer_canonical_state(runtime)||!CanonicalTrace(group)){words.push_back(0);return words.data();}
    const auto& bytes=*CanonicalTrace(group);words.resize(1+(bytes.size()+3)/4);words[0]=u32(bytes.size());
    if(!bytes.empty())std::memcpy(words.data()+1,bytes.data(),bytes.size());return words.data();
}
extern "C" __attribute__((export_name("mp_fixture_canonical_field_info")))
const u32* mp_fixture_canonical_field_info(BrowserRuntime* runtime,u32 group,u32 offset){
    using namespace multiplayer;
    static u32 words[4]{};std::fill(words,words+4,~u32(0));
    if(!runtime||group>=11)return words;
    if(!multiplayer_canonical_state(runtime))return words;SnapshotCanonicalPlayerRegions();
    if(group==1){words[0]=CanonicalPlayerField(offset);words[1]=CanonicalPlayerFieldByteOffset(offset);words[2]=CanonicalPlayerSeat(offset);}
    else if(group==8){const auto field=CanonicalSupervisorField(offset);words[0]=field[0];words[1]=field[1];words[2]=~u32(0);}
    return words;
}
#endif
}
