#include "../platform/Application.hpp"
#include <eagler/netplay/NetplayProtocol.hpp>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstddef>

using namespace th10;
namespace {
u32 hash_bytes(const void* data,std::size_t bytes,u32 hash=2166136261u){
    const auto* p=static_cast<const u8*>(data);
    for(std::size_t i=0;i<bytes;++i){hash^=p[i];hash*=16777619u;}
    return hash;
}
template<class T>u32 hash_value(const T& value,u32 hash=2166136261u){
    return hash_bytes(&value,sizeof(value),hash);
}
u32 hash_input_lanes(const multiplayer::InputLanes::State& state,u32 hash=2166136261u){
    for(const auto& seat:state.seats)hash=hash_value(seat,hash);
    // FrameInput has tail padding on wasm32; hash its fields so allocator or
    // copy details cannot masquerade as divergent input state.
    for(const auto& input:state.inputs){
        hash=hash_value(input.buttons,hash);hash=hash_value(input.analogMode,hash);
        hash=hash_value(input.x,hash);hash=hash_value(input.y,hash);
        hash=hash_value(input.unlimited,hash);hash=hash_value(input.touchUsed,hash);
        hash=hash_value(input.touchBomb,hash);
    }
    hash=hash_value(state.host,hash);
    return hash_value(state.pause_rising,hash);
}
u32 hash_team_economy(const multiplayer::TeamEconomy& source){
    auto value=source;
    // High score and bit 2 only track personal-record display/persistence.
    value.high_score=0;value.high_score_units=0;value.flags&=~4u;
    const float rate=value.faith_timer.rate?*value.faith_timer.rate:0.0f;
    value.faith_timer.rate=nullptr;
    return hash_value(rate,hash_value(value));
}
u32 hash_timer_state(const Timer& timer,u32 hash){
    hash=hash_value(timer.previous,hash);hash=hash_value(timer.current,hash);
    hash=hash_value(timer.fractional,hash);
    const float rate=timer.rate?*timer.rate:0.0f;
    return hash_value(rate,hash);
}
u32 hash_movement_state(const Movement& movement,u32 hash){
    hash=hash_value(movement.position,hash);hash=hash_value(movement.velocity,hash);
    hash=hash_value(movement.speed,hash);hash=hash_value(movement.angle,hash);
    hash=hash_value(movement.radius,hash);hash=hash_value(movement.radial_velocity,hash);
    return hash_value(movement.flags,hash);
}
u32 hash_shot_definition(const PlayerShotDefinition& definition,u32 hash){
    hash=hash_value(definition.fire_interval,hash);hash=hash_value(definition.fire_offset,hash);
    hash=hash_value(definition.damage,hash);hash=hash_value(definition.offset,hash);
    hash=hash_value(definition.hitbox,hash);hash=hash_value(definition.angle,hash);
    hash=hash_value(definition.speed,hash);hash=hash_value(definition.option,hash);
    hash=hash_value(definition.type,hash);hash=hash_value(definition.on_initialize,hash);
    hash=hash_value(definition.on_update,hash);return hash_value(definition.on_hit,hash);
}
u32 hash_player_shot(const PlayerShot& shot,u32 hash){
    hash=hash_value(shot.state,hash);hash=hash_timer_state(shot.timer,hash);
    hash=hash_value(shot.timer_flags,hash);hash=hash_movement_state(shot.motion,hash);
    hash=hash_value(shot.collided,hash);hash=hash_value(shot.first_collision,hash);
    const bool hasTarget=shot.homing_target!=nullptr;hash=hash_value(hasTarget,hash);
    const bool hasDefinition=shot.definition!=nullptr;hash=hash_value(hasDefinition,hash);
    if(shot.definition)hash=hash_shot_definition(*shot.definition,hash);
    return hash;
}
struct PlayerHashParts {
    u32 core=0,timers=0,shots=0,shotCount=0,options=0,damage=0;
    u32 activeLasers=0,optionSpeed=0,invulnerability=0,bounds=0,history=0;
};
PlayerHashParts hash_player_parts(const Player& player){
    PlayerHashParts parts;parts.core=2166136261u;parts.timers=2166136261u;
    parts.shots=2166136261u;parts.options=2166136261u;parts.damage=2166136261u;
    parts.activeLasers=parts.optionSpeed=parts.invulnerability=parts.bounds=parts.history=2166136261u;
    auto& core=parts.core;auto& timers=parts.timers;auto& shots=parts.shots;
    auto& options=parts.options;auto& damage=parts.damage;
    // Hash simulation fields explicitly. Player also owns ANM VMs, update-chain
    // links, callbacks and profile pointers whose addresses are not Replay state.
    core=hash_value(player.flags,core);core=hash_value(player.position,core);
    core=hash_value(player.fixed_position,core);core=hash_value(player.fast_speed,core);
    core=hash_value(player.slow_speed,core);core=hash_value(player.fast_diagonal,core);
    core=hash_value(player.slow_diagonal,core);core=hash_value(player.velocity,core);
    core=hash_value(player.collision_bounds,core);core=hash_value(player.hitbox_half_size,core);
    core=hash_value(player.fast_pickup_size,core);core=hash_value(player.slow_pickup_size,core);
    core=hash_value(player.death_position,core);core=hash_value(player.input_velocity,core);
    core=hash_value(player.direction,core);core=hash_value(player.state,core);
    const bool hasTarget=player.target!=nullptr;core=hash_value(hasTarget,core);
    core=hash_value(player.target_seen,core);
    timers=hash_timer_state(player.fire_timer,timers);timers=hash_value(player.fire_timer_flags,timers);
    timers=hash_timer_state(player.state_timer,timers);timers=hash_value(player.state_timer_flags,timers);
    timers=hash_timer_state(player.focus_timer,timers);timers=hash_value(player.focus_timer_flags,timers);
    for(u32 i=0;i<128;++i){
        const auto& shot=player.shots[i];if(!shot.state)continue;
        ++parts.shotCount;
        shots=hash_value(i,hash_player_shot(shot,shots));
    }
    for(const auto& option:player.options){
        options=hash_value(option.active,options);options=hash_value(option.target,options);
        options=hash_value(option.position,options);options=hash_value(option.offset,options);
        options=hash_value(option.focused_offset,options);options=hash_value(option.previous_focus,options);
        options=hash_value(option.index,options);options=hash_value(option.snap_next,options);
    }
    options=hash_value(player.option_count,options);
    for(u32 i=0;i<32;++i){
        const auto& area=player.damage_areas[i];if(!(area.flags&1u))continue;
        damage=hash_value(i,damage);damage=hash_value(area.radius,damage);
        damage=hash_value(area.radial_speed,damage);damage=hash_value(area.angle,damage);
        damage=hash_value(area.angular_velocity,damage);damage=hash_value(area.size,damage);
        damage=hash_movement_state(area.motion,damage);damage=hash_timer_state(area.timer,damage);
        damage=hash_value(area.timer_flags,damage);damage=hash_value(area.damage,damage);
        damage=hash_value(area.total_damage,damage);damage=hash_value(area.damage_limit,damage);
        damage=hash_value(area.interval,damage);damage=hash_value(area.flags,damage);
    }
    for(const auto laser:player.active_lasers)parts.activeLasers=hash_value(laser,parts.activeLasers);
    parts.optionSpeed=hash_value(player.option_follow_speed,parts.optionSpeed);
    parts.invulnerability=hash_timer_state(player.invulnerability,parts.invulnerability);
    parts.invulnerability=hash_value(player.invulnerability_flags,parts.invulnerability);
    parts.bounds=hash_value(player.pickup_bounds,parts.bounds);
    parts.bounds=hash_value(player.slow_pickup_bounds,parts.bounds);
    parts.bounds=hash_value(player.fast_pickup_bounds,parts.bounds);
    parts.history=hash_bytes(player.position_history,sizeof(player.position_history),parts.history);
    parts.history=hash_value(player.focused,parts.history);
    return parts;
}
u32 hash_player_state(const Player& player,u32 hash){
    const auto parts=hash_player_parts(player);
    hash=hash_value(parts.core,hash);hash=hash_value(parts.timers,hash);
    hash=hash_value(parts.shotCount,hash);hash=hash_value(parts.shots,hash);
    hash=hash_value(parts.options,hash);hash=hash_value(parts.damage,hash);
    hash=hash_value(parts.activeLasers,hash);hash=hash_value(parts.optionSpeed,hash);
    hash=hash_value(parts.invulnerability,hash);hash=hash_value(parts.bounds,hash);
    return hash_value(parts.history,hash);
}
u32 hash_bomb_state(const Bomb& bomb,u32 hash){
    hash=hash_value(bomb.flags,hash);hash=hash_timer_state(bomb.timer,hash);
    hash=hash_value(bomb.timer_flags,hash);hash=hash_value(bomb.active,hash);
    if(bomb.active){hash=hash_value(bomb.position,hash);hash=hash_value(bomb.radius,hash);
        hash=hash_value(bomb.radial_speed,hash);hash=hash_value(bomb.mode,hash);}
    return hash;
}
template<std::size_t BlockSize,std::size_t Capacity>
u32 hash_pool(const multiplayer::RollbackPool<BlockSize,Capacity>& pool,u32 hash=2166136261u){
    hash=hash_bytes(pool.occupied,sizeof(pool.occupied),hash);
    hash=hash_value(pool.cursor,hash);
    for(std::size_t i=0;i<Capacity;++i)if(pool.active(i)){
        hash=hash_value(u32(i),hash);hash=hash_bytes(pool.at(i),BlockSize,hash);
    }
    return hash;
}
u32 hash_bullets(const EnemyBulletManager* manager){
    if(!manager)return 0;u32 hash=2166136261u;
    // The manager survives a native stage transition, while direct Replay
    // bootstrap creates a fresh owner. Update/draw handles, draw-list links,
    // the animation-file pointer and the absolute pool cursor are graph
    // addresses, not gameplay identity. Hash the cursor as a pool index and
    // keep only scalar state that can affect later bullet operations.
    hash=hash_bytes(manager->manager_fields,sizeof(manager->manager_fields),hash);
    const u32 cursor=manager->cursor>=manager->pool&&manager->cursor<=manager->pool+2000?
        u32(manager->cursor-manager->pool):0xffffffffu;
    hash=hash_value(cursor,hash);
    hash=hash_value(manager->last_cancel_position,hash);
    hash=hash_value(manager->last_cancel_size,hash);
    hash=hash_value(manager->active_count,hash);
    for(u32 i=0;i<2000;++i)if(manager->pool[i].state){
        hash=hash_value(i,hash);hash=hash_value(manager->pool[i],hash);
    }
    return hash;
}
u32 hash_items(const ItemManager* manager){
    if(!manager)return 0;u32 hash=2166136261u;
    // Stage transitions retain this manager, while direct Replay creates a
    // fresh one. Callback/resource pointers are graph identity, not gameplay
    // state. Keep only scalar manager state plus active item contents.
    hash=hash_value(manager->flags,hash);
    hash=hash_value(manager->manager_state,hash);
    for(u32 i=0;i<150;++i)if(manager->regular[i].state){hash=hash_value(i,hash);hash=hash_value(manager->regular[i],hash);}
    for(u32 i=0;i<2048;++i)if(manager->faith[i].state){hash=hash_value(i,hash);hash=hash_value(manager->faith[i],hash);}
    hash=hash_value(manager->active_count,hash);hash=hash_value(manager->faith_cursor,hash);hash=hash_value(manager->faith_count,hash);return hash;
}
u32 hash_laser_semantic(const EnemyLaser& laser,u32 hash){
    auto value=laser;
    value.previous=value.next=nullptr;
    value.timer.rate=nullptr;value.acceleration_timer.rate=nullptr;
    value.vector_timer.rate=nullptr;value.angular_timer.rate=nullptr;
    value.turn_timer.rate=nullptr;value.feature_delay.rate=nullptr;
    value.size_timer.rate=nullptr;value.blend_timer.rate=nullptr;
    value.outside_delay.rate=nullptr;
    return hash_value(value,hash);
}
u32 hash_lasers_semantic(const LaserManager* manager){
    if(!manager)return 0;u32 hash=2166136261u;
    // The native manager is retained across stages, while direct Replay owns a
    // fresh graph. Linked-list addresses, tail, resource pointers and rollback
    // allocator slots are representation identity. count and last_id affect
    // subsequent creation and therefore remain authoritative.
    hash=hash_value(manager->count,hash);hash=hash_value(manager->last_id,hash);
    u32 traversed=0;
    for(const auto* laser=manager->sentinel.next;laser;laser=laser->next){
        ++traversed;hash=hash_laser_semantic(*laser,hash);
    }
    return hash_value(traversed,hash);
}
u32 hash_stage(const Stage* stage){
    if(!stage)return 0;u32 hash=hash_value(*stage);
    if(stage->object_animations&&stage->file&&stage->file->primitive_count>0)
        hash=hash_bytes(stage->object_animations,std::size_t(stage->file->primitive_count)*sizeof(AnmVm),hash);
    if(stage->objects&&stage->file)for(i32 i=0;i<stage->file->object_count;++i)
        if(stage->objects[i])hash=hash_value(stage->objects[i]->flags,hash);
    return hash;
}
void normalize_timer(Timer& timer){timer.rate=nullptr;}
template<class T>void normalize_interpolator(T& value){normalize_timer(value.timer);}
u32 hash_ecl_context_semantic(const EclContext& context,u32 hash=2166136261u){
    hash=hash_value(context.time,hash);
    // Instruction addresses refer to immutable ECL data. Hash the current
    // instruction contents instead of the allocation address.
    const bool hasInstruction=context.instruction!=nullptr;hash=hash_value(hasInstruction,hash);
    if(hasInstruction)hash=hash_bytes(context.instruction,sizeof(EclInstruction),hash);
    const i32 top=std::clamp(context.stack.top,0,4096);
    hash=hash_value(top,hash);hash=hash_value(context.stack.frame_base,hash);
    if(top)hash=hash_bytes(context.stack.data,std::size_t(top),hash);
    hash=hash_value(context.thread_id,hash);hash=hash_value(context.state_1018,hash);
    hash=hash_value(context.difficulty,hash);return hash_value(context.flags,hash);
}
u32 hash_enemy_semantic(const Enemy& enemy,u32 hash=2166136261u){
    hash=hash_ecl_context_semantic(enemy.script.root,hash);
    for(auto* node=enemy.script.threads.next;node;node=node->next)
        if(node->value)hash=hash_ecl_context_semantic(*node->value,hash);
    EnemyState state=enemy.state;
    // ANM ids and list/owner addresses are local identity, not authored game
    // state. visual_size remains included because movement/despawn logic uses
    // it, as do every other scalar/timer/ECL field in EnemyState.
    std::memset(state.animations,0,sizeof(state.animations));
    state.manager_node={};state.script_owner=nullptr;
    normalize_timer(state.lifetime);normalize_timer(state.damage_immunity);
    normalize_timer(state.collision_immunity);
    normalize_interpolator(state.absolute_position);normalize_interpolator(state.relative_position);
    normalize_interpolator(state.absolute_angle);normalize_interpolator(state.relative_angle);
    normalize_interpolator(state.absolute_radius);normalize_interpolator(state.relative_radius);
    return hash_value(state,hash);
}
u32 hash_enemies_semantic(const browser::World& world){
    const auto* manager=world.actors.enemies;if(!manager)return 0;
    u32 hash=2166136261u;hash=hash_value(manager->flags,hash);
    Timer lifetime=manager->lifetime;normalize_timer(lifetime);hash=hash_value(lifetime,hash);
    hash=hash_value(manager->lifetime_flags,hash);hash=hash_value(manager->count,hash);
    hash=hash_value(manager->spawn_count,hash);
    for(const auto* boss:manager->bosses)hash=hash_value(boss!=nullptr,hash);
    u32 traversed=0;for(auto* node=manager->head;node;node=node->next){
        if(!node->value)continue;++traversed;hash=hash_enemy_semantic(*node->value,hash);
    }
    return hash_value(traversed,hash);
}
u32 hash_anm_vm_authored(const AnmVm& vm,u32 hash){
    hash=hash_value(vm.id,hash);hash=hash_value(vm.owner_tag,hash);
    hash=hash_value(vm.rotation,hash);hash=hash_value(vm.angular_velocity,hash);
    hash=hash_value(vm.scale,hash);hash=hash_value(vm.scale_velocity,hash);
    hash=hash_value(vm.sprite_size,hash);hash=hash_value(vm.uv_offset,hash);
    hash=hash_value(vm.script_timer,hash);hash=hash_value(vm.script_timer_flags,hash);
    hash=hash_value(vm.position_interpolation,hash);hash=hash_value(vm.color_interpolation,hash);
    hash=hash_value(vm.alpha_interpolation,hash);hash=hash_value(vm.rotation_interpolation,hash);
    hash=hash_value(vm.scale_interpolation,hash);hash=hash_value(vm.color2_interpolation,hash);
    hash=hash_value(vm.alpha2_interpolation,hash);hash=hash_value(vm.uv_velocity,hash);
    hash=hash_value(vm.color,hash);hash=hash_value(vm.secondary_color,hash);
    hash=hash_value(vm.pending_interrupt,hash);hash=hash_value(vm.animation_file,hash);
    hash=hash_bytes(vm.integer_variables,sizeof(vm.integer_variables),hash);
    hash=hash_bytes(vm.float_variables,sizeof(vm.float_variables),hash);
    hash=hash_bytes(vm.extra_integer_variables,sizeof(vm.extra_integer_variables),hash);
    hash=hash_value(vm.script_position,hash);hash=hash_value(vm.position,hash);hash=hash_value(vm.child_position,hash);
    const u32 authoredFlags=vm.flags&~12u;hash=hash_value(authoredFlags,hash);
    hash=hash_value(vm.saved_timer,hash);hash=hash_value(vm.saved_timer_flags,hash);
    hash=hash_value(vm.saved_instruction,hash);hash=hash_value(vm.sprite_frame,hash);
    hash=hash_value(vm.sprite_index,hash);hash=hash_value(vm.file_index,hash);
    hash=hash_value(vm.script_index,hash);hash=hash_value(vm.script_begin,hash);
    hash=hash_value(vm.instruction,hash);hash=hash_value(vm.sprite,hash);return hash;
}
struct AnmHashes {u32 metadata=2166136261u,pooled=2166136261u,overflow=2166136261u;};
AnmHashes hash_anm_parts(const browser::AnimationEngine& engine){
    const auto& manager=engine.manager;AnmHashes result;
    result.metadata=hash_value(manager.started_scripts,result.metadata);
    result.metadata=hash_value(manager.processed_count,result.metadata);
    result.metadata=hash_bytes(manager.occupied,sizeof(manager.occupied),result.metadata);
    result.metadata=hash_value(manager.cursor,result.metadata);
    result.metadata=hash_value(manager.registry,result.metadata);
    result.metadata=hash_value(manager.last_id,result.metadata);
    for(u32 i=0;i<4096;++i)if(manager.occupied[i]){
        result.pooled=hash_value(i,result.pooled);
        result.pooled=hash_anm_vm_authored(manager.pool[i],result.pooled);
    }
    for(u32 i=0;i<2048;++i)if(engine.rollback_animation_overflow.active(i)){
        result.overflow=hash_value(i,result.overflow);
        result.overflow=hash_anm_vm_authored(*static_cast<const AnmVm*>(engine.rollback_animation_overflow.at(i)),result.overflow);
    }
    return result;
}
u32 hash_anm(const AnmHashes& parts){
    u32 hash=hash_value(parts.metadata);hash=hash_value(parts.pooled,hash);return hash_value(parts.overflow,hash);
}

// Cross-graph hashes used by read-only spectators. The rollback oracle above
// intentionally retains exact pointer/layout identity; these helpers instead
// describe authored state that must survive a different transport/allocation
// bootstrap.
u32 hash_cstring_portable(const char* value,u32 hash){
    const bool present=value!=nullptr;hash=hash_value(present,hash);
    if(!value)return hash;
    for(std::size_t i=0;i<512;++i){
        const u8 byte=static_cast<u8>(value[i]);hash=hash_value(byte,hash);
        if(!byte)return hash;
    }
    return hash_value(u8(0xff),hash);
}
const EclSubroutine* ecl_subroutine_for(const EclProgram* program,
                                        const EclInstruction* instruction){
    if(!program||!instruction||!program->sorted_subroutines||program->subroutine_count<=0)return nullptr;
    const auto target=reinterpret_cast<std::uintptr_t>(instruction);
    const EclSubroutine* best=nullptr;std::uintptr_t bestStart=0;
    for(i32 i=0;i<program->subroutine_count;++i){
        const auto& sub=program->sorted_subroutines[i];
        if(!sub.header)continue;
        const auto start=reinterpret_cast<std::uintptr_t>(sub.header+16);
        if(start<=target&&start>=bestStart){best=&sub;bestStart=start;}
    }
    return best;
}
u32 hash_ecl_instruction_portable(const EclProgram* program,
                                  const EclInstruction* instruction,u32 hash){
    const bool present=instruction!=nullptr;hash=hash_value(present,hash);
    if(!instruction)return hash;
    if(const auto* sub=ecl_subroutine_for(program,instruction)){
        hash=hash_cstring_portable(sub->name,hash);
        const auto offset=u32(reinterpret_cast<std::uintptr_t>(instruction)-
                              reinterpret_cast<std::uintptr_t>(sub->header+16));
        return hash_value(offset,hash);
    }
    // Defensive fallback for malformed diagnostic state. Valid gameplay ECL
    // instructions are always owned by one loaded subroutine.
    return hash_bytes(instruction,sizeof(EclInstruction),hash);
}
u32 hash_ecl_stack_portable(const EclContext& context,const EclProgram* program,u32 hash){
    const i32 top=std::clamp(context.stack.top,0,4096);
    const i32 frameBase=std::clamp(context.stack.frame_base,0,top);
    hash=hash_value(top,hash);hash=hash_value(frameBase,hash);
    bool returnPointer[1024]{};
    // A call saves [time, instruction] immediately before the callee's frame
    // base. The previous frame base lives four bytes before that caller top.
    // Mark only these structurally known pointer slots; local integer values
    // which happen to resemble Wasm addresses stay ordinary data.
    i32 base=frameBase;
    for(u32 depth=0;depth<64&&base>=8&&base<=top;++depth){
        const i32 pointerOffset=base-4;
        if((pointerOffset&3)==0)returnPointer[pointerOffset/4]=true;
        if(base<12)break;
        i32 previous=0;std::memcpy(&previous,context.stack.data+base-12,4);
        if(previous<=0||previous>=base)break;
        base=previous;
    }
    i32 offset=0;
    for(;offset+4<=top;offset+=4){
        u32 word=0;std::memcpy(&word,context.stack.data+offset,4);
        if(returnPointer[offset/4]){
            hash=hash_value(u32(0xec100001u),hash);
            hash=hash_ecl_instruction_portable(
                program,reinterpret_cast<const EclInstruction*>(std::uintptr_t(word)),hash);
        }else hash=hash_value(word,hash);
    }
    if(offset<top)hash=hash_bytes(context.stack.data+offset,std::size_t(top-offset),hash);
    return hash;
}
u32 hash_ecl_context_portable(const EclContext& context,const EclProgram* program,
                              u32 hash=2166136261u){
    hash=hash_value(context.time,hash);
    hash=hash_ecl_instruction_portable(program,context.instruction,hash);
    hash=hash_ecl_stack_portable(context,program,hash);
    hash=hash_value(context.thread_id,hash);hash=hash_value(context.state_1018,hash);
    // Native initialization deliberately writes only the low difficulty byte
    // and only bit zero of flags is consumed by ECL.
    hash=hash_value(reinterpret_cast<const u8*>(&context.difficulty)[0],hash);
    return hash_value(context.flags&1u,hash);
}
template<class T>u32 hash_interpolator_portable(const T& source,u32 hash){
    auto value=source;
    const float rate=value.timer.rate?*value.timer.rate:0.0f;
    value.timer.rate=nullptr;hash=hash_value(value,hash);return hash_value(rate,hash);
}
u32 hash_enemy_portable(const Enemy& enemy,const EclProgram* program,u32 hash=2166136261u){
    hash=hash_ecl_context_portable(enemy.script.root,program,hash);
    for(auto* node=enemy.script.threads.next;node;node=node->next)
        if(node->value)hash=hash_ecl_context_portable(*node->value,program,hash);
    EnemyState state=enemy.state;
    std::memset(state.animations,0,sizeof(state.animations));
    state.manager_node={};state.script_owner=nullptr;
    for(auto& interrupt:state.interrupts){
        hash=hash_cstring_portable(interrupt.health_subroutine,hash);
        hash=hash_cstring_portable(interrupt.time_subroutine,hash);
        interrupt.health_subroutine=interrupt.time_subroutine=nullptr;
    }
    normalize_timer(state.lifetime);normalize_timer(state.damage_immunity);
    normalize_timer(state.collision_immunity);
    normalize_interpolator(state.absolute_position);normalize_interpolator(state.relative_position);
    normalize_interpolator(state.absolute_angle);normalize_interpolator(state.relative_angle);
    normalize_interpolator(state.absolute_radius);normalize_interpolator(state.relative_radius);
    return hash_value(state,hash);
}
u32 hash_enemies_portable(const browser::World& world){
    const auto* manager=world.actors.enemies;if(!manager)return 0;
    u32 hash=2166136261u;hash=hash_value(manager->flags,hash);
    hash=hash_timer_state(manager->lifetime,hash);hash=hash_value(manager->lifetime_flags,hash);
    hash=hash_value(manager->count,hash);hash=hash_value(manager->spawn_count,hash);
    for(const auto* boss:manager->bosses)hash=hash_value(boss!=nullptr,hash);
    u32 traversed=0;for(auto* node=manager->head;node;node=node->next){
        if(!node->value)continue;++traversed;hash=hash_enemy_portable(*node->value,manager->program,hash);
    }
    return hash_value(traversed,hash);
}
u32 hash_anm_vm_portable(const AnmVm& vm,u32 hash){
    hash=hash_value(vm.id,hash);hash=hash_value(vm.owner_tag,hash);
    hash=hash_value(vm.rotation,hash);hash=hash_value(vm.angular_velocity,hash);
    hash=hash_value(vm.scale,hash);hash=hash_value(vm.scale_velocity,hash);
    hash=hash_value(vm.sprite_size,hash);hash=hash_value(vm.uv_offset,hash);
    hash=hash_timer_state(vm.script_timer,hash);hash=hash_value(vm.script_timer_flags,hash);
    hash=hash_interpolator_portable(vm.position_interpolation,hash);
    hash=hash_interpolator_portable(vm.color_interpolation,hash);
    hash=hash_interpolator_portable(vm.alpha_interpolation,hash);
    hash=hash_interpolator_portable(vm.rotation_interpolation,hash);
    hash=hash_interpolator_portable(vm.scale_interpolation,hash);
    hash=hash_interpolator_portable(vm.color2_interpolation,hash);
    hash=hash_interpolator_portable(vm.alpha2_interpolation,hash);
    hash=hash_value(vm.uv_velocity,hash);hash=hash_value(vm.color,hash);
    hash=hash_value(vm.secondary_color,hash);hash=hash_value(vm.pending_interrupt,hash);
    hash=hash_bytes(vm.integer_variables,sizeof(vm.integer_variables),hash);
    hash=hash_bytes(vm.float_variables,sizeof(vm.float_variables),hash);
    hash=hash_bytes(vm.extra_integer_variables,sizeof(vm.extra_integer_variables),hash);
    hash=hash_value(vm.script_position,hash);hash=hash_value(vm.position,hash);
    hash=hash_value(vm.child_position,hash);
    const u32 authoredFlags=vm.flags&~12u;hash=hash_value(authoredFlags,hash);
    hash=hash_timer_state(vm.saved_timer,hash);hash=hash_value(vm.saved_timer_flags,hash);
    const auto instructionOffset=[&](const AnmInstruction* instruction){
        if(!instruction)return i32(-1);
        if(!vm.script_begin)return i32(-2);
        return i32(reinterpret_cast<const u8*>(instruction)-
                   reinterpret_cast<const u8*>(vm.script_begin));
    };
    hash=hash_value(instructionOffset(vm.saved_instruction),hash);
    hash=hash_value(vm.sprite_frame,hash);hash=hash_value(vm.sprite_index,hash);
    hash=hash_value(vm.file_index,hash);hash=hash_value(vm.script_index,hash);
    return hash_value(instructionOffset(vm.instruction),hash);
}
u32 hash_anm_portable(const browser::AnimationEngine& engine){
    const auto& manager=engine.manager;u32 hash=2166136261u;
    hash=hash_value(manager.started_scripts,hash);hash=hash_value(manager.processed_count,hash);
    hash=hash_bytes(manager.occupied,sizeof(manager.occupied),hash);
    hash=hash_value(manager.cursor,hash);hash=hash_value(manager.last_id,hash);
    for(u32 i=0;i<4096;++i)if(manager.occupied[i]){
        hash=hash_value(i,hash);hash=hash_anm_vm_portable(manager.pool[i],hash);
    }
    for(u32 i=0;i<2048;++i)if(engine.rollback_animation_overflow.active(i)){
        hash=hash_value(i,hash);
        hash=hash_anm_vm_portable(
            *static_cast<const AnmVm*>(engine.rollback_animation_overflow.at(i)),hash);
    }
    return hash;
}
bool decode_frame_input(const u32* row,Netplay::FrameInput& input){
    if(!row||row[0]>65535||row[1]>4||row[4]>7)return false;
    input.buttons=u16(row[0]);input.analogMode=Netplay::AnalogMode(row[1]);
    std::memcpy(&input.x,row+2,4);std::memcpy(&input.y,row+3,4);
    input.unlimited=(row[4]&1)!=0;input.touchUsed=(row[4]&2)!=0;input.touchBomb=(row[4]&4)!=0;
    return std::isfinite(input.x)&&std::isfinite(input.y);
}
}
extern "C" __attribute__((export_name("multiplayer_commit_inputs")))
u32 multiplayer_commit_inputs(browser::Application* app,const u32* words,u32 count){
    if(!app||app->stopped||!app->state.multiplayer_session.configured||!words||
       count!=app->state.multiplayer_session.playerCount*5)return 0;
    if(app->state.multiplayer_session.sessionId&&app->multiplayer_active())return 0;
    Netplay::FrameInput inputs[3]{};
    const auto seats=app->state.multiplayer_session.playerCount;
    for(u32 seat=0;seat<seats;++seat){
        const auto* row=words+seat*5;
        if(!decode_frame_input(row,inputs[seat]))return 0;
    }
    return multiplayer::InputLanes::Commit(app->state.input_lanes,inputs,seats)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_configure")))
u32 multiplayer_configure(browser::Application* app,const u32* words,u32 count){
    if(!app||app->stopped||app->world||app->value.screen!=-2)return 0;
    multiplayer::SessionSetup next=app->state.multiplayer_session;
    if(!multiplayer::DecodeSessionSetup(next,words,count))return 0;
    if(next.sessionId&&!app->state.netplay_runtime.Reset(next,next.sessionId))return 0;
    if(!next.sessionId)app->state.netplay_runtime.Clear();
    app->state.input_lanes={};
    app->state.multiplayer_session=next;return 1;
}
extern "C" __attribute__((export_name("multiplayer_contract")))
u32 multiplayer_contract(browser::Application* app){
    if(!app||!app->state.multiplayer_session.configured)return 0;
    return multiplayer::GameplayContract(app->state.multiplayer_session);
}
extern "C" __attribute__((export_name("multiplayer_session_build")))
u32 multiplayer_session_build(browser::Application* app,u32 phase,u8* out,u32 capacity){
    if(!app||!app->state.netplay_runtime.Configured()||app->state.netplay_runtime.Spectator()||
       (phase!=1&&phase!=2))return 0;
    if(phase==2&&!app->state.netplay_runtime.LocalReady())return 0;
    std::vector<u8> wire;
    const auto packet=phase==1?app->state.netplay_runtime.Hello():app->state.netplay_runtime.Ready();
    if(!Netplay::EncodeSessionPacket(packet,&wire)||wire.size()>capacity||(!out&&!wire.empty()))return 0;
    if(!wire.empty())std::memcpy(out,wire.data(),wire.size());return u32(wire.size());
}
extern "C" __attribute__((export_name("multiplayer_session_apply")))
u32 multiplayer_session_apply(browser::Application* app,const u8* bytes,u32 size){
    if(!app||!app->state.netplay_runtime.Configured()||app->state.netplay_runtime.Spectator()||
       !bytes||!size)return 0;
    Netplay::SessionPacket packet{};if(!Netplay::DecodeSessionPacket(bytes,size,&packet))return 0;
    const auto result=app->state.netplay_runtime.ApplySession(packet);
    return result==Netplay::SessionPacketResult::Accepted||result==Netplay::SessionPacketResult::Duplicate?1:0;
}
extern "C" __attribute__((export_name("multiplayer_session_mark_ready")))
u32 multiplayer_session_mark_ready(browser::Application* app){
    if(!app||!app->state.netplay_runtime.Configured()||app->state.netplay_runtime.Spectator()||
       !app->state.netplay_runtime.CanSendReady())return 0;
    app->state.netplay_runtime.MarkLocalReady();return 1;
}
extern "C" __attribute__((export_name("multiplayer_session_can_start")))
u32 multiplayer_session_can_start(browser::Application* app){
    return app&&app->state.netplay_runtime.CanStart()?1:0;
}
extern "C" __attribute__((export_name("multiplayer_submit_remote")))
u32 multiplayer_submit_remote(browser::Application* app,u32 player,u32 frame,
                              const u32* words,u32 count){
    if(!app||!app->state.netplay_runtime.Configured()||count!=5||
       player>=app->state.multiplayer_session.playerCount||
       player==app->state.multiplayer_session.localPlayer)return 0;
    Netplay::FrameInput input{};if(!decode_frame_input(words,input))return 0;
    const auto result=app->state.netplay_runtime.SubmitRemote(
        static_cast<std::uint8_t>(player),frame,input);
    return static_cast<u32>(result)+1;
}
extern "C" __attribute__((export_name("multiplayer_capture_local")))
u32 multiplayer_capture_local(browser::Application* app,u32 frame,const u32* words,u32 count){
    if(!app||app->stopped||count!=5||!app->state.netplay_runtime.CanStart()||
       frame!=app->state.netplay_runtime.NextFrame())return 0;
    Netplay::FrameInput input{};if(!decode_frame_input(words,input))return 0;
    return app->state.netplay_runtime.CaptureLocal(frame,input)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_lifecycle_status")))
const i32* multiplayer_lifecycle_status(browser::Application* app){
    static i32 words[12]{};std::fill(words,words+12,0);if(!app)return words;
    words[0]=1;words[1]=app->value.screen;words[2]=app->value.pending_screen;
    if(auto* w=app->world){
        if(w->actors.session){words[3]=i32(w->actors.session->session_flags);words[6]=w->actors.session->elapsed.current;}
        if(w->actors.results){words[4]=w->actors.results->state;words[5]=w->actors.results->menu.selected;}
    }
    words[7]=app->multiplayer_waiting;words[8]=i32(app->multiplayer_rollbacks);
    words[9]=i32(app->multiplayer_resimulated_frames);
    words[10]=app->input.player_profiles[0].input.raw_pressed;
    words[11]=app->input.player_profiles[0].input.raw_repeat;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_netplay_status")))
const i32* multiplayer_netplay_status(browser::Application* app){
    static i32 words[11]{};std::fill(words,words+11,0);if(!app)return words;
    auto& runtime=app->state.netplay_runtime;
    words[0]=runtime.Configured();words[1]=runtime.CanStart();
    words[2]=i32(runtime.NextFrame());words[3]=i32(runtime.LastSimulatedFrame());
    words[4]=i32(runtime.ConfirmedThroughAllRemotes());
    words[5]=runtime.HasRollbackRequest()?i32(runtime.RollbackFrame()):-1;
    words[6]=app->world?i32(app->world->rollback.CapturedBytes(
        runtime.LastSimulatedFrame()==Netplay::INVALID_FRAME?0:runtime.LastSimulatedFrame())):0;
    const auto sessionId=app->state.multiplayer_session.sessionId;
    words[7]=i32(u32(sessionId));words[8]=i32(u32(sessionId>>32));
    words[9]=app->multiplayer_active()?1:0;
    words[10]=app->world&&app->world->actors.session?1:0;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_canonical_hashes")))
const u32* multiplayer_canonical_hashes(browser::Application* app){
    static u32 words[44]{};std::fill(words,words+44,0);if(!app||!app->world)return words;
    auto& w=*app->world;u32 economy=hash_team_economy(app->state.team_economy);
    economy=hash_bytes(app->state.pilot_economies,sizeof(app->state.pilot_economies),economy);
    economy=hash_value(w.cooperation,economy);economy=hash_input_lanes(app->state.input_lanes,economy);
    u32 rng=hash_value(app->engine.script_random);rng=hash_value(app->engine.visual_random,rng);
    u32 players=2166136261u;
    for(u32 seat=0;seat<w.player_count;++seat){
        players=hash_value(w.pilots[seat].input_keys,players);
        if(w.pilots[seat].player)players=hash_player_state(*w.pilots[seat].player,players);
        if(w.pilots[seat].bomb)players=hash_bomb_state(*w.pilots[seat].bomb,players);
    }
    const u32 enemies=hash_enemies_semantic(w);
    const u32 lasers=hash_lasers_semantic(w.actors.lasers);
    u32 items=hash_items(w.actors.items);items=hash_bytes(w.regular_item_owners,sizeof(w.regular_item_owners),items);items=hash_bytes(w.faith_item_owners,sizeof(w.faith_item_owners),items);
    u32 scene=2166136261u;if(w.actors.session)scene=hash_value(*w.actors.session,scene);if(w.actors.spell)scene=hash_value(*w.actors.spell,scene);if(w.actors.gui)scene=hash_value(*w.actors.gui,scene);if(w.actors.results)scene=hash_value(*w.actors.results,scene);if(w.actors.popups)scene=hash_value(*w.actors.popups,scene);if(w.actors.hints)scene=hash_value(*w.actors.hints,scene);if(w.actors.effects)scene=hash_value(*w.actors.effects,scene);scene=hash_value(hash_stage(w.backgrounds.current),scene);scene=hash_value(hash_stage(w.backgrounds.previous),scene);
    u32 chain=hash_value(app->engine.chain_value);chain=hash_pool(app->engine.callback_environment.rollback_entries,chain);chain=hash_pool(app->effects.rollback_effects,chain);
    const auto anm=hash_anm_parts(app->engine);
    words[0]=2;words[2]=economy;words[3]=rng;words[4]=players;words[5]=enemies;words[6]=hash_bullets(w.actors.bullets);words[7]=lasers;words[8]=items;words[9]=hash_anm(anm);words[10]=scene;words[11]=chain;
    u32 composite=2166136261u;for(u32 i=2;i<12;++i)composite=hash_value(words[i],composite);words[1]=composite;words[12]=app->state.multiplayer_session.started;
    words[13]=app->engine.script_random.seed;words[14]=app->engine.script_random.calls;
    words[15]=app->engine.visual_random.seed;words[16]=app->engine.visual_random.calls;
    words[17]=anm.metadata;words[18]=anm.pooled;words[19]=anm.overflow;
    auto& manager=app->engine.manager;u32 occupied=0,worldCount=0,uiCount=0,overflowCount=0;
    for(const auto value:manager.occupied)occupied+=value?1u:0u;
    for(auto* node=manager.registry.world_head;node;node=node->next)++worldCount;
    for(auto* node=manager.registry.ui_head;node;node=node->next)++uiCount;
    for(u32 i=0;i<2048;++i)overflowCount+=app->engine.rollback_animation_overflow.active(i)?1u:0u;
    words[20]=manager.started_scripts;words[21]=manager.processed_count;words[22]=u32(manager.cursor);words[23]=manager.last_id;
    words[24]=occupied;words[25]=worldCount;words[26]=uiCount;words[27]=overflowCount;
    const u32 enemyManager=w.actors.enemies?hash_value(*w.actors.enemies):0;
    const u32 enemyPool=hash_pool(w.rollback_enemies);
    const u32 eclPool=hash_pool(w.rollback_ecl);
    words[28]=enemyManager;words[29]=enemyPool;words[30]=eclPool;
    if(w.actors.enemies){
        words[31]=u32(w.actors.enemies->count);words[32]=w.actors.enemies->spawn_count;
        words[33]=u32(w.actors.enemies->lifetime.current);
        words[34]=w.actors.enemies->bosses[0]?1u:0u;
    }
    u32 enemyActive=0,eclActive=0;
    for(u32 i=0;i<512;++i)enemyActive+=w.rollback_enemies.active(i)?1u:0u;
    for(u32 i=0;i<1024;++i)eclActive+=w.rollback_ecl.active(i)?1u:0u;
    words[35]=enemyActive;words[36]=eclActive;words[37]=u32(app->engine.speed*65536.0f);
    // Schema 2 extends the semantic composite instead of allowing a new owner
    // to disappear behind diagnostic-only fields. Raw pointer-heavy pool
    // diagnostics 28..30 stay separate from semantic Enemy/ECL identity.
    u32 replay=2166136261u;
    if(const auto* r=app->state.replay){
        replay=hash_value(r->flags,replay);replay=hash_value(r->manager_state,replay);
        replay=hash_value(r->elapsed,replay);replay=hash_value(r->active_stage,replay);
        for(auto* snapshot:r->stages)if(snapshot)replay=hash_value(*snapshot,replay);
        if(r->info){auto info=*r->info;std::memset(info.name,0,sizeof(info.name));
            info.timestamp=0;info.slow_rate=0;replay=hash_value(info,replay);}
    }
    words[38]=replay;
    u32 common=2166136261u;
    if(const auto* c=w.common.value){common=hash_value(c->flags,common);common=hash_value(c->state,common);
        common=hash_value(c->frames,common);common=hash_value(c->introduction_animation,common);
        common=hash_value(c->loading_animation,common);}
    words[39]=common;
    words[40]=w.hud?hash_pool(w.hud->rollback_dialogues):0;
    words[41]=hash_pool(app->engine.rollback_geometry);
    u32 camera=hash_value(app->engine.world);camera=hash_value(app->engine.ui,camera);
    camera=hash_value(app->engine.tangent,camera);camera=hash_value(app->state.application.engine_flags,camera);
    camera=hash_value(app->state.quitting,camera);words[42]=camera;
    words[43]=w.scores.data?hash_value(w.scores.data->settings,hash_value(w.scores.data->characters)):0;
    for(u32 i=38;i<44;++i)words[1]=hash_value(words[i],words[1]);
    return words;
}
extern "C" __attribute__((export_name("multiplayer_portable_hashes")))
const u32* multiplayer_portable_hashes(browser::Application* app){
    // Pointer-independent spectator/cross-graph oracle. Keep the exact-layout
    // rollback oracle above unchanged: it intentionally catches local graph
    // identity drift that is meaningless for a separately bootstrapped,
    // receive-only spectator.
    static u32 words[12]{};std::fill(words,words+12,0);words[0]=1;
    if(!app||!app->world)return words;
    auto& w=*app->world;
    u32 economy=hash_team_economy(app->state.team_economy);
    economy=hash_bytes(app->state.pilot_economies,sizeof(app->state.pilot_economies),economy);
    economy=hash_value(w.cooperation,economy);
    economy=hash_input_lanes(app->state.input_lanes,economy);
    u32 rng=hash_value(app->engine.script_random);
    rng=hash_value(app->engine.visual_random,rng);
    u32 players=2166136261u;
    for(u32 seat=0;seat<w.player_count;++seat){
        players=hash_value(w.pilots[seat].input_keys,players);
        if(w.pilots[seat].player)players=hash_player_state(*w.pilots[seat].player,players);
        if(w.pilots[seat].bomb)players=hash_bomb_state(*w.pilots[seat].bomb,players);
    }
    const u32 enemies=hash_enemies_portable(w);
    const u32 bullets=hash_bullets(w.actors.bullets);
    const u32 lasers=hash_lasers_semantic(w.actors.lasers);
    u32 items=hash_items(w.actors.items);
    items=hash_bytes(w.regular_item_owners,sizeof(w.regular_item_owners),items);
    items=hash_bytes(w.faith_item_owners,sizeof(w.faith_item_owners),items);
    const u32 anm=hash_anm_portable(app->engine);
    u32 lifecycle=2166136261u;
    lifecycle=hash_value(app->state.game.stage,lifecycle);
    lifecycle=hash_value(app->state.game.stage_frames,lifecycle);
    lifecycle=hash_value(app->state.game.section_frames,lifecycle);
    lifecycle=hash_value(app->state.pending_screen,lifecycle);
    lifecycle=hash_value(app->state.multiplayer_session.started,lifecycle);
    lifecycle=hash_value(w.loading,lifecycle);
    if(const auto* session=w.actors.session){
        lifecycle=hash_value(session->flags,lifecycle);
        lifecycle=hash_value(session->stage_identifier,lifecycle);
        lifecycle=hash_timer_state(session->elapsed,lifecycle);
        lifecycle=hash_value(session->timer_flags,lifecycle);
        lifecycle=hash_bytes(session->configuration,sizeof(session->configuration),lifecycle);
        lifecycle=hash_value(session->session_flags,lifecycle);
        lifecycle=hash_value(session->replay_mode,lifecycle);
    }
    u32 replay=2166136261u;
    if(const auto* r=app->state.replay){
        replay=hash_value(r->flags,replay);replay=hash_value(r->manager_state,replay);
        replay=hash_value(r->elapsed,replay);replay=hash_value(r->active_stage,replay);
        for(auto* snapshot:r->stages)if(snapshot)replay=hash_value(*snapshot,replay);
        if(r->info){auto info=*r->info;std::memset(info.name,0,sizeof(info.name));
            info.timestamp=0;info.slow_rate=0;replay=hash_value(info,replay);}
    }
    words[2]=economy;words[3]=rng;words[4]=players;words[5]=enemies;
    words[6]=bullets;words[7]=lasers;words[8]=items;words[9]=anm;
    words[10]=lifecycle;words[11]=replay;
    u32 composite=2166136261u;
    for(u32 i=2;i<12;++i)composite=hash_value(words[i],composite);
    words[1]=composite;return words;
}
extern "C" __attribute__((export_name("multiplayer_enemy_debug")))
const u32* multiplayer_enemy_debug(browser::Application* app){
    // Read-only diagnostic words for the first four fixed-pool enemies.  Keep
    // raw ANM handles beside gameplay fields so rollback tests can distinguish
    // presentation identity drift from an actual Enemy/ECL divergence.
    static u32 words[81]{};std::fill(words,words+81,0);if(!app||!app->world)return words;
    auto& w=*app->world;u32 row=0;words[0]=1;
    for(u32 slot=0;slot<512&&row<4;++slot){
        if(!w.rollback_enemies.active(slot))continue;
        const auto& enemy=*static_cast<const Enemy*>(w.rollback_enemies.at(slot));
        const auto& s=enemy.state;auto* out=words+1+row*20;
        out[0]=slot;out[1]=hash_value(enemy.script);out[2]=hash_value(s);
        std::memcpy(out+3,&s.current.position.x,4);std::memcpy(out+4,&s.current.position.y,4);
        std::memcpy(out+5,&s.current.velocity.x,4);std::memcpy(out+6,&s.current.velocity.y,4);
        out[7]=u32(s.health);out[8]=s.flags;out[9]=u32(s.lifetime.current);
        out[10]=s.animations[0];out[11]=s.animations[1];out[12]=s.animations[2];
        out[13]=u32(s.animation_file);out[14]=u32(s.bound_animation_file);
        out[15]=u32(s.animation_script);out[16]=u32(s.base_animation);
        std::memcpy(out+17,&s.visual_size.x,4);std::memcpy(out+18,&s.visual_size.y,4);
        out[19]=u32(s.direction);++row;
    }
    words[80]=row;return words;
}
extern "C" __attribute__((export_name("multiplayer_status")))
const i32* multiplayer_status(browser::Application* app){
    static i32 words[44];std::memset(words,0,sizeof(words));
    if(!app)return words;
    const auto& setup=app->state.multiplayer_session;
    words[0]=1;words[1]=i32(setup.playerCount);words[2]=i32(setup.localPlayer);
    words[3]=setup.started;words[4]=app->world&&app->world->loading;
    words[5]=app->error;words[6]=app->state.game.stage;
    words[7]=i32(app->state.game.stage_frames);
    if(auto* world=app->world)for(u32 seat=0;seat<world->player_count;++seat){
        const auto& pilot=world->pilots[seat];auto* out=words+8+seat*12;
        out[0]=pilot.game.character;out[1]=pilot.game.shot_type;
        out[2]=pilot.game.lives;out[3]=pilot.game.power;
        if(pilot.player){
            out[4]=pilot.player->state;out[5]=pilot.player->fixed_position.x;
            out[6]=pilot.player->fixed_position.y;out[7]=pilot.player->focused;
        }
        out[8]=pilot.bomb?pilot.bomb->active:0;
        out[9]=i32(world->cooperation.seats[seat].lifeState);
        out[10]=world->cooperation.seats[seat].rescueTicks;
        out[11]=i32(pilot.input_keys);
    }
    return words;
}
