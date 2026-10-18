#include "SpectatorStageActivation.hpp"
#include "SpectatorStageState.hpp"
#include "../platform/World.hpp"
#include <algorithm>
#include <cmath>
#include <type_traits>

namespace th10::multiplayer {
namespace {
using Netplay::SpectatorScalars::Reader;
using Netplay::SpectatorScalars::Writer;
constexpr u32 kActivationSchema=1,kMaxActivationBytes=256*1024;
template<class C> constexpr bool reading=std::is_same_v<C,Reader>;
template<class C> bool vec(C& c,Vec3& v){return c.Values(v.x,v.y,v.z);}
template<class C> bool vec(C& c,Vec2& v){return c.Values(v.x,v.y);}
template<class C> bool point(C& c,PlayerOption::FixedPoint& p){return c.Values(p.x,p.y);}
template<class C> bool bounds(C& c,PlayerBounds& b){return vec(c,b.minimum)&&vec(c,b.maximum);}
template<class C> bool timer(C& c,Timer& t,browser::World& w){
    bool hasRate=t.rate!=nullptr;
    if(!c.Values(t.previous,t.current,t.fractional,hasRate))return false;
    if constexpr(reading<C>)t.rate=hasRate?&w.engine.speed:nullptr;
    return true;
}
template<class C> bool movement(C& c,Movement& m){
    return vec(c,m.position)&&vec(c,m.velocity)&&c.Values(m.speed,m.angle,m.radius,m.radial_velocity,m.flags);
}
template<class C> bool handle(C& c,u32& id,browser::World& w,const SpectatorStageState& saved){
    u32 source=id;if(!c.Value(source))return false;
    if constexpr(reading<C>)id=ResolveSpectatorStageAnm(w,saved,source);
    return true;
}
template<class C> bool replayStage(C& c,ReplayStage& s){
    if(!c.Values(s.stage,s.seed,s.frames,s.stream_bytes,s.score,s.power,s.reserved_012,
        s.item_value,s.faith,s.lives,s.rank)||!point(c,s.position))return false;
    for(auto& p:s.history)if(!point(c,p))return false;
    for(auto* list:{s.option_targets,s.option_positions,s.option_offsets,s.option_focused_offsets})
        for(unsigned i=0;i<4;++i)if(!point(c,list[i]))return false;
    return c.Values(s.score_units,s.extend_index,s.focused,s.flags);
}
template<class C> bool replay(C& c,browser::World& w){
    auto* r=w.state.replay;if(!r||!r->info||r->mode!=0)return false;
    if(!c.Values(r->flags,r->manager_state,r->elapsed,r->active_stage))return false;
    for(auto*& stage:r->stages){
        bool present=stage!=nullptr;if(!c.Value(present))return false;
        if constexpr(reading<C>){
            if(present&&!stage)stage=reinterpret_cast<ReplayStage*>(w.replay_memory.allocate(sizeof(ReplayStage)));
            if(!present&&stage){
                if(!w.replay_memory.owns(stage))return false;
                w.replay_memory.release(stage);stage=nullptr;
            }
        }
        if(present&&(!stage||!replayStage(c,*stage)))return false;
    }
    auto& i=*r->info;
    // Names, time and measured frame rate are local recording metadata.
    return c.Values(i.score)&&c.Array(i.configuration)&&c.Values(i.stage_count,i.character,
        i.shot_type,i.difficulty,i.last_stage,i.score_units);
}
u32 definitionIndex(const PlayerProfile& profile,const PlayerShotDefinition* wanted){
    if(!wanted)return 0;
    for(u32 p=0;p<std::min<u32>(profile.pattern_count,10);++p){
        const auto* defs=profile.patterns[p].definitions;if(!defs)continue;
        for(u32 i=0;i<128&&defs[i].fire_interval>=0;++i)
            if(defs+i==wanted)return 1+(p<<8)+i;
    }
    return 0xffffffffu;
}
PlayerShotDefinition* definitionAt(PlayerProfile& profile,u32 value){
    if(!value)return nullptr;--value;const u32 p=value>>8,i=value&255;
    if(p>=profile.pattern_count||p>=10||i>=128||!profile.patterns[p].definitions)return nullptr;
    auto* defs=profile.patterns[p].definitions;
    for(u32 n=0;n<=i;++n)if(defs[n].fire_interval<0)return nullptr;
    return defs+i;
}
template<class C> bool player(C& c,Player& p,browser::World& w,const SpectatorStageState& saved){
    if(!p.profile||(!reading<C>&&p.target))return false;
    if(!c.Values(p.flags,p.manager_state)||!vec(c,p.position)||!point(c,p.fixed_position)||
       !c.Values(p.fast_speed,p.slow_speed,p.fast_diagonal,p.slow_diagonal)||!point(c,p.velocity)||
       !bounds(c,p.collision_bounds)||!vec(c,p.hitbox_half_size)||!vec(c,p.fast_pickup_size)||
       !vec(c,p.slow_pickup_size)||!vec(c,p.death_position)||!point(c,p.input_velocity)||
       !c.Values(p.direction,p.state,p.target_seen)||!timer(c,p.fire_timer,w)||!c.Value(p.fire_timer_flags)||
       !timer(c,p.state_timer,w)||!c.Value(p.state_timer_flags)||!timer(c,p.focus_timer,w)||
       !c.Value(p.focus_timer_flags))return false;
    if constexpr(reading<C>)p.target=nullptr;
    for(auto& s:p.shots){
        if constexpr(reading<C>)s={};
        if(!c.Value(s.state))return false;
        if(!s.state)continue;
        if(!reading<C>&&s.homing_target)return false;
        u32 definition=reading<C>?0:definitionIndex(*p.profile,s.definition);
        if(definition==0xffffffffu||!timer(c,s.timer,w)||!c.Value(s.timer_flags)||!movement(c,s.motion)||
           !handle(c,s.animation,w,saved)||!handle(c,s.secondary_animation,w,saved)||
           !c.Values(s.collided,s.first_collision,definition))return false;
        if constexpr(reading<C>){
            s.homing_target=nullptr;s.definition=definitionAt(*p.profile,definition);
            if(definition&&!s.definition)return false;
        }
    }
    if(!handle(c,p.focus_animation,w,saved))return false;
    for(auto& o:p.options){
        if(!c.Value(o.active)||!point(c,o.target)||!point(c,o.position)||!point(c,o.offset)||
           !point(c,o.focused_offset)||!handle(c,o.animations[0],w,saved)||
           !handle(c,o.animations[1],w,saved)||!c.Values(o.previous_focus,o.index,o.snap_next,o.on_update,o.on_draw))return false;
    }
    if(!c.Value(p.option_count)||p.option_count<0||p.option_count>4)return false;
    for(auto& a:p.damage_areas){
        if constexpr(reading<C>)a={};
        if(!c.Value(a.flags))return false;
        if(!(a.flags&1u))continue;
        if(!c.Values(a.radius,a.radial_speed,a.angle,a.angular_velocity)||!vec(c,a.size)||
           !movement(c,a.motion)||!timer(c,a.timer,w)||!c.Values(a.timer_flags,a.damage,
                a.total_damage,a.damage_limit,a.interval))return false;
    }
    if(!c.Array(p.active_lasers)||!c.Value(p.option_follow_speed)||!timer(c,p.invulnerability,w)||
       !c.Values(p.invulnerability_flags,p.reserved_4320)||!bounds(c,p.pickup_bounds)||
       !bounds(c,p.slow_pickup_bounds)||!bounds(c,p.fast_pickup_bounds))return false;
    for(auto& pos:p.position_history)if(!point(c,pos))return false;
    return c.Value(p.focused);
}
template<class C> bool bomb(C& c,Bomb& b,browser::World& w,const SpectatorStageState& saved){
    return c.Values(b.flags,b.manager_state,b.reserved_010)&&timer(c,b.timer,w)&&
        c.Values(b.timer_flags,b.active)&&handle(c,b.animation,w,saved)&&vec(c,b.position)&&
        c.Values(b.radius,b.radial_speed,b.mode);
}
struct EffectRecord {
    ScreenEffect value{};
    bool update=false,draw=false;
    i32 updatePriority=14,drawPriority=0;
    u32 updateFlags=0,drawFlags=0,updateOrder=0,drawOrder=0;
};
UpdateChainEntry* effectEntry(UpdateChainEntry& sentinel,const ScreenEffect* owner,u32& order){
    order=0;
    for(auto* node=sentinel.node.next;node;node=node->next,++order)
        if(node->value&&node->value->owner==owner)return node->value;
    return nullptr;
}
template<class C> bool screenEffects(C& c,browser::World& w){
    auto& env=w.effects;std::vector<EffectRecord> records;
    if constexpr(!reading<C>){
        for(u32 slot=0;slot<128;++slot)if(env.rollback_effects.active(slot)){
            const auto* e=static_cast<const ScreenEffect*>(env.rollback_effects.at(slot));EffectRecord rec;
            rec.value=*e;
            if(auto* entry=effectEntry(w.chain->update,e,rec.updateOrder)){
                rec.update=true;rec.updatePriority=entry->priority;rec.updateFlags=entry->flags;
            }
            if(auto* entry=effectEntry(w.chain->draw,e,rec.drawOrder)){
                rec.draw=true;rec.drawPriority=entry->priority;rec.drawFlags=entry->flags;
            }
            records.push_back(rec);
        }
        std::stable_sort(records.begin(),records.end(),[](const auto& a,const auto& b){
            return a.update!=b.update?a.update>b.update:a.updateOrder<b.updateOrder;
        });
    }
    u32 count=u32(records.size());if(!c.Value(count)||count>128)return false;
    if constexpr(reading<C>)records.resize(count);
    for(auto& rec:records){auto& e=rec.value;
        if(!c.Values(e.flags,e.state,e.kind,e.reserved_014,e.alpha,e.duration)||!c.Array(e.parameters)||
           !c.Value(e.releasing)||!timer(c,e.timer,w)||!c.Values(e.timer_flags,rec.update,rec.draw,
            rec.updatePriority,rec.drawPriority,rec.updateFlags,rec.drawFlags,rec.updateOrder,rec.drawOrder)||
           u32(e.kind)>8||(rec.update&&rec.updatePriority!=14)||rec.updateOrder>65536||rec.drawOrder>65536)
            return false;
    }
    if constexpr(reading<C>){
        // Inspect live chain entries rather than a removed effect's stale
        // callback pointer. Native flashes can retain an owner after return 0.
        for(u32 slot=0;slot<128;++slot)if(env.rollback_effects.active(slot)){
            auto* e=static_cast<ScreenEffect*>(env.rollback_effects.at(slot));u32 order=0;
            if(auto* entry=effectEntry(w.chain->update,e,order))w.chain->remove_locked(entry,w.engine.callback_environment);
            if(auto* entry=effectEntry(w.chain->draw,e,order))w.chain->remove_locked(entry,w.engine.callback_environment);
            env.destroy(e);
        }
        std::vector<std::pair<EffectRecord*,ScreenEffect*>> restored;
        // Equal-priority insertion is before the existing entry. Recreate the
        // captured update sequence in reverse to preserve RNG consumption.
        for(auto it=records.rbegin();it!=records.rend();++it){
            auto& rec=*it;auto* e=env.allocate();if(!e)return false;
            e->initialize();*e=rec.value;e->update_entry=e->draw_entry=nullptr;
            const auto kind=u32(e->kind);
            if(rec.update){
                e->update_entry=w.chain->add(env.update_callbacks[kind],e,rec.updatePriority,false,true,w.engine.callback_environment);
                if(!e->update_entry)return false;
                e->update_entry->flags=rec.updateFlags;
                e->update_entry->notify_callback=env.delete_callback;
#ifdef TH_NATIVE_PLATFORM
                e->update_entry->notification=w.engine.callback_environment.resolve(env.delete_callback);
#endif
            }
            restored.push_back({&rec,e});
        }
        std::stable_sort(restored.begin(),restored.end(),[](const auto& a,const auto& b){return a.first->drawOrder>b.first->drawOrder;});
        for(auto& pair:restored){auto& rec=*pair.first;auto* e=pair.second;if(!rec.draw)continue;
            const auto callback=env.draw_callbacks[u32(e->kind)];if(!callback)return false;
            e->draw_entry=w.chain->add(callback,e,rec.drawPriority,true,true,w.engine.callback_environment);
            if(!e->draw_entry)return false;e->draw_entry->flags=rec.drawFlags;
        }
    }
    return true;
}
template<class C> bool visit(C& c,browser::World& w,const SpectatorStageState& saved){
    auto* s=w.actors.session;auto* e=w.actors.enemies;auto* b=w.actors.bullets;auto* items=w.actors.items;
    if(!s||!e||!b||!items||e->head||e->tail||e->count)return false;
    for(auto* boss:e->bosses)if(boss)return false;
    for(u32 i=0;i<2000;++i)if(b->pool[i].state)return false;
    for(const auto& i:items->regular)if(i.state)return false;
    for(const auto& i:items->faith)if(i.state)return false;
    u32 schema=kActivationSchema,count=w.player_count;
    if(!c.Values(schema,count)||schema!=kActivationSchema||count!=w.player_count||count<2||count>3||
       !c.Values(s->flags,s->stage_identifier)||!timer(c,s->elapsed,w)||!c.Value(s->timer_flags)||
       !c.Array(s->configuration)||!c.Values(s->session_flags,s->replay_mode)||!replay(c,w)||
       !c.Values(e->flags,e->reserved_004)||!timer(c,e->lifetime,w)||
       !c.Values(e->lifetime_flags,e->count,e->spawn_count)||e->count!=0)return false;
    u32 cursor=reading<C>?0:(b->cursor>=b->pool&&b->cursor<=b->pool+2000?u32(b->cursor-b->pool):0xffffffffu);
    if(!c.Array(b->manager_fields)||!c.Value(cursor)||cursor>2000||!vec(c,b->last_cancel_position)||
       !vec(c,b->last_cancel_size)||!c.Value(b->active_count)||!c.Values(items->flags,items->manager_state,
        items->active_count,items->faith_cursor,items->faith_count))return false;
    if constexpr(reading<C>)b->cursor=b->pool+cursor;
    for(auto* owners:{w.regular_item_owners,w.faith_item_owners}){
        const u32 n=owners==w.regular_item_owners?150:2048;
        for(u32 i=0;i<n;++i){
            if(!c.Values(owners[i].homing,owners[i].recipient)||owners[i].homing< -1||
               owners[i].homing>=i32(count)||owners[i].recipient< -1||owners[i].recipient>=i32(count))return false;
        }
    }
    for(u32 seat=0;seat<count;++seat){
        auto& pilot=w.pilots[seat];
        if(!pilot.player||!pilot.bomb||!player(c,*pilot.player,w,saved)||!bomb(c,*pilot.bomb,w,saved))return false;
    }
    return screenEffects(c,w);
}
bool valid(const SpectatorStageActivationState& a){
    return !a.present?(a.uncompressedBytes==0&&a.compressed.empty()):
        (a.elapsedCurrent==30&&std::isfinite(a.elapsedFractional)&&a.uncompressedBytes>0&&
         a.uncompressedBytes<=kMaxActivationBytes&&!a.compressed.empty()&&
         a.compressed.size()<=Netplay::SpectatorStageMaxStateBytes);
}
}
bool CaptureSpectatorStageActivation(browser::World& w,SpectatorStageState& saved){
    auto* s=w.actors.session;if(!s||s->elapsed.current!=30)return false;
    auto& a=saved.activation;a={};a.present=true;
    a.elapsedPrevious=s->elapsed.previous;a.elapsedCurrent=s->elapsed.current;
    a.elapsedFractional=s->elapsed.fractional;a.timerFlags=s->timer_flags;a.sessionFlags=s->session_flags;
    Writer writer(kMaxActivationBytes);
    return visit(writer,w,saved)&&writer.Good()&&
        PackSpectatorStageFields(writer.Bytes(),a.uncompressedBytes,a.checksum,a.compressed)&&valid(a);
}
bool PrepareSpectatorStageActivation(browser::World& w,const SpectatorStageState& saved){
    const auto& a=saved.activation;if(!a.present)return true;
    if(!valid(a)||!w.actors.session)return false;
    auto& s=*w.actors.session;s.elapsed.previous=a.elapsedPrevious;s.elapsed.current=a.elapsedCurrent;
    s.elapsed.fractional=a.elapsedFractional;s.elapsed.rate=&w.engine.speed;
    s.timer_flags=a.timerFlags;s.session_flags=a.sessionFlags|0x800u;
    return true;
}
bool RestoreSpectatorStageActivation(browser::World& w,const SpectatorStageState& saved){
    if(!valid(saved.activation)||!saved.activation.present)return false;
    std::vector<u8> raw;
    if(!UnpackSpectatorStageFields(saved.activation.uncompressedBytes,saved.activation.checksum,
        saved.activation.compressed,raw)||raw.size()>kMaxActivationBytes)return false;
    Reader reader(raw);return visit(reader,w,saved)&&reader.Done();
}
bool WriteSpectatorStageActivation(Writer& c,const SpectatorStageActivationState& a){
    if(!valid(a)||!c.Value(a.present))return false;if(!a.present)return true;
    const u32 size=u32(a.compressed.size());
    return c.Values(a.elapsedPrevious,a.elapsedCurrent,a.elapsedFractional,a.timerFlags,a.sessionFlags,
        a.uncompressedBytes,a.checksum,size)&&c.Array(a.compressed);
}
bool ReadSpectatorStageActivation(Reader& c,SpectatorStageActivationState& a){
    a={};if(!c.Value(a.present))return false;if(!a.present)return true;
    u32 size=0;
    if(!c.Values(a.elapsedPrevious,a.elapsedCurrent,a.elapsedFractional,a.timerFlags,a.sessionFlags,
        a.uncompressedBytes,a.checksum,size)||size==0||size>Netplay::SpectatorStageMaxStateBytes||
        a.uncompressedBytes==0||a.uncompressedBytes>kMaxActivationBytes)return false;
    a.compressed.resize(size);return c.Array(a.compressed)&&valid(a);
}
}
