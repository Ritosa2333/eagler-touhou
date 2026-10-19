#include "DynamicState.hpp"
#include "../game/Player.hpp"
#include <algorithm>

namespace th09::multiplayer {
namespace {
// Preserve the complete byte representation of named POD blocks, including
// padding used by the independent exact restore oracle. Owning types still
// use their explicit deep-copy path.
template<class T>void copy_value(T& to,const T& from){
    if constexpr(std::is_trivially_copyable_v<T>)std::memcpy(&to,&from,sizeof(T));
    else to=from;
}
template<class T>void copy_owner(std::unique_ptr<T>& to,const std::unique_ptr<T>& from) {
    if(!from){to.reset();return;}
    if(!to)to=std::make_unique<T>();
    copy_value(*to,*from);
}
}
bool EnemyState::Copy(EclVm& out,const EclVm& in) {
    // An ECL executor temporarily moves an asynchronous context out of the
    // owner while executing it. A checkpoint there is invalid; at a tick
    // boundary active is null, primary, or an owned asynchronous context.
    int active=-2;
    if(in.active==&in.primary)active=-1;
    else if(in.active){
        for(int i=0;i<4;++i)if(in.active==in.asynchronous[i].get())active=i;
        if(active==-2)return false;
    }
    int locals=-2;
    if(in.values.locals==&in.primary.locals)locals=-1;
    else if(in.values.locals){
        for(int i=0;i<4;++i)if(in.asynchronous[i]&&in.values.locals==&in.asynchronous[i]->locals)locals=i;
        if(locals==-2)return false;
    }
#define COPY(field) copy_value(out.field,in.field)
    COPY(values);COPY(program);COPY(primary);
    for(unsigned i=0;i<4;++i){copy_owner(out.asynchronous[i],in.asynchronous[i]);out.generations[i]=in.generations[i];}
    COPY(active_slot);COPY(scratch_depth);COPY(behavior_flags);COPY(difficulty_flags);
    COPY(pending_interrupt);std::copy_n(in.interrupt_subroutines,32,out.interrupt_subroutines);
    COPY(position_offset);COPY(velocity);COPY(movement);COPY(emitter);COPY(lasers);
    COPY(animation);COPY(status);COPY(trail);COPY(finished);COPY(invalid);COPY(failed_opcode);
#undef COPY
    out.active=active==-2?nullptr:active==-1?&out.primary:out.asynchronous[active].get();
    // values.locals must never point into the snapshot or an allocation that
    // a later frame deleted. External world/field/program pointers stay fixed.
    out.values.locals=locals==-2?nullptr:locals==-1?&out.primary.locals:&out.asynchronous[locals]->locals;
    return true;
}
bool EnemyState::Save(const EclVm& source) { valid=Copy(saved,source);return valid; }
bool EnemyState::Restore(EclVm& destination) const { return valid&&Copy(destination,saved); }
std::size_t EnemyState::Bytes() const {
    std::size_t n=sizeof(saved);for(const auto& c:saved.asynchronous)if(c)n+=sizeof(*c);return n;
}
void EffectState::Copy(EffectActor& out,const EffectActor& in) {
#define COPY(field) copy_value(out.field,in.field)
    COPY(position);COPY(arguments);COPY(velocity);COPY(acceleration);COPY(start_tangent);
    COPY(anchor);COPY(origin);COPY(destination);COPY(auxiliary);COPY(auxiliary_scalar);
    COPY(radius);COPY(angle);COPY(spread);COPY(thickness);COPY(segments);COPY(slot);
    COPY(distortion);COPY(rotation);COPY(cycle);COPY(transfer);COPY(time);COPY(reserved);
    COPY(active);COPY(kind);COPY(flags0);COPY(flags1);COPY(layer);COPY(dirty);
    COPY(upper_layer);COPY(flags2);COPY(hidden);COPY(side);COPY(colors);COPY(texture);
#undef COPY
    copy_owner(out.animation,in.animation);copy_owner(out.burst,in.burst);
}
std::size_t EffectState::Bytes() const {
    return sizeof(saved)+(saved.animation?sizeof(AnmVm):0)+(saved.burst?sizeof(EffectBurst):0)+
        saved.colors.size()*sizeof(AttackColorVertex)+saved.texture.size()*sizeof(AttackTextureVertex);
}
void AttackStateSnapshot::Copy(AttackActor& out,const AttackActor& in) {
#define COPY(field) copy_value(out.field,in.field)
    COPY(layer);COPY(destination_side);COPY(source_side);COPY(active);COPY(time);
    COPY(position);COPY(animations);COPY(behavior);
#undef COPY
    std::memcpy(&out.color,&in.color,sizeof(out.color));
    if(in.state)in.state->copy_into(out.state);else out.state.reset();
    out.extra_position=nullptr;out.parent=nullptr;
}
bool AttackStateSnapshot::Save(const AttackActor& source) {
    // Init-only pointers may refer to caller stack storage. Sakuya's durable
    // parent link lives inside its state and points to a stable actor slot.
    valid=!source.extra_position&&!source.parent;
    if(valid)Copy(saved,source);
    return valid;
}
bool AttackStateSnapshot::Restore(AttackActor& destination) const {
    if(!valid)return false;Copy(destination,saved);return true;
}
u32 EnemyState::Fingerprint(const EclVm& e){
    StateChecksum h;const auto& v=e.values;
    h.Add(v.world,v.field,v.opponent,v.shared_integer,v.shared_real,v.position,v.resolved_position,v.origin,v.target,v.last_delta,v.direction,v.angular_velocity,v.speed,v.acceleration,v.orbit_radius,v.orbit_angle,v.orbit_velocity,v.lifetime,v.life,v.last_damage,v.life_thresholds,v.item_reward,v.score_reward,v.drop_count,v.drop_item,v.flags,v.boss_id,e.program,e.primary);
    for(auto& c:e.asynchronous){h.Add(bool(c));if(c)h.Add(*c);}
    int active=-2,locals=-2;if(e.active==&e.primary)active=-1;if(e.values.locals==&e.primary.locals)locals=-1;
    for(int i=0;i<4;++i)if(e.asynchronous[i]){if(e.active==e.asynchronous[i].get())active=i;if(e.values.locals==&e.asynchronous[i]->locals)locals=i;}
    h.Add(active,locals,e.generations,e.active_slot,e.scratch_depth,e.behavior_flags,e.difficulty_flags,e.pending_interrupt,e.interrupt_subroutines,e.position_offset,e.velocity,e.movement,e.emitter,e.lasers,e.animation,e.status,e.trail,e.finished,e.invalid,e.failed_opcode);return h.value;
}
u32 EffectState::Fingerprint(const EffectActor& e){
    StateChecksum h;h.Add(e.position,e.arguments,e.velocity,e.acceleration,e.start_tangent,e.anchor,e.origin,e.destination,e.auxiliary,e.auxiliary_scalar,e.radius,e.angle,e.spread,e.thickness,e.segments,e.slot,e.distortion,e.rotation,e.cycle,e.transfer,e.time,e.reserved,e.active,e.kind,e.flags0,e.flags1,e.layer,e.dirty,e.upper_layer,e.flags2,e.hidden,e.side);
    h.Add(bool(e.animation),bool(e.burst));if(e.animation)h.Add(*e.animation);if(e.burst)h.Add(*e.burst);for(auto& v:e.colors)h.Add(v);for(auto& v:e.texture)h.Add(v);return h.value;
}
u32 AttackStateSnapshot::Fingerprint(const AttackActor& a){
    StateChecksum h;h.Add(a.layer,a.destination_side,a.source_side,a.active,a.time,a.position,a.color,a.behavior,a.extra_position,a.parent);for(auto& v:a.animations)h.Add(v);h.Add(bool(a.state));if(a.state)h.Add(a.state->checksum());return h.value;
}
}

namespace th09 {
void StateChecksum::One(const FrameTiming& x){Add(x.rate,x.force_step);}
void StateChecksum::One(const PlayerFrameContext& x){Add(x.timing,x.limits,x.geometry,x.computer,x.game_flags,x.field_flags,x.dialogue,x.automatic_focus,x.extra_mode,x.hide_players,x.scene_state,x.ending_frames,x.opponent_level,x.opponent_survival_time);}
void StateChecksum::One(const CpuContext& x){Add(x.side,x.player,x.speeds,x.base_scale,x.effect_scale,x.limits,x.health,x.level,x.opponent_pending,x.spirit_count,x.normal_enemies,x.attack_areas,x.field_flags,x.scene_locked,x.dialogue,x.extra_mode,x.item_target,x.priority_target,x.first_target,x.has_priority,x.has_first,x.target_flags,x.extra_damage);}
}
