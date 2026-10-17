#include "../platform/BrowserRuntime.hpp"
#include "CanonicalContext.hpp"
#include <cstddef>
#include <memory>

namespace th08::multiplayer {
namespace {
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
        h.add(p);
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
    if(!runtime||!runtime->app.in_game())return result;
    auto& a=runtime->app;auto& g=a.game;StateHash world{g};Hash h[11];
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
    h[9].bytes(g.program.data(),g.program.size());
    auto& program=g.background_script.program;h[9].bytes(program.header(),program.bytes_size());
    h[9].bytes(g.dialogue.program.bytes(),g.dialogue.program.size());
    for(i32 index=0;index<256;++index)if(const auto* resource=a.library.resource(index)){
        h[9].add(index);const auto& bytes=resource->data();h[9].bytes(bytes.data(),bytes.size());
        if(const auto* loaded=a.library.get(index))h[9].bytes(loaded->sprites,std::size_t(loaded->spriteCount)*sizeof(AnmLoadedSprite));
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
}
