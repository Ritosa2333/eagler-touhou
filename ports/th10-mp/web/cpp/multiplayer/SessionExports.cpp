#include "../platform/Application.hpp"
#include "../platform/CameraMathCache.hpp"
#include <eagler/netplay/NetplayProtocol.hpp>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstddef>
#include <cstring>

using namespace th10;
extern "C" __attribute__((export_name("sdl_room_return"))) u32 sdl_room_return(browser::Application* app){
    if(!app||app->stopped||app->error)return 0;
    app->engine.rollback_state=nullptr;app->engine.netplay_frame=Netplay::INVALID_FRAME;
    if(app->world){
        app->world->rollback.reset();app->world->audio_events.Reset();
        app->world->audio.manager.command_sink=nullptr;
    }
    app->state.netplay_runtime.Clear();app->state.multiplayer_session={};app->state.input_lanes={};
    app->state.multiplayer_replay.Clear();
    app->multiplayer_replay_scope=app->multiplayer_frame_open=app->multiplayer_waiting=app->multiplayer_generation_pending=false;
    app->state.pending_screen=4;app->room_preview=true;return 1;
}
namespace {
u32 hash_bytes(const void* data,std::size_t bytes,u32 hash=2166136261u){
    const auto* p=static_cast<const u8*>(data);
    for(std::size_t i=0;i<bytes;++i){hash^=p[i];hash*=16777619u;}
    return hash;
}
u32 float_bits(float value){u32 result=0;std::memcpy(&result,&value,sizeof(result));return result;}
template<class T>u32 hash_value(const T& value,u32 hash=2166136261u){
    return hash_bytes(&value,sizeof(value),hash);
}
u32 hash_anm_vm_portable(const AnmVm& vm,u32 hash);
u32 hash_cstring_portable(const char* value,u32 hash);
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

template<std::size_t BlockSize,std::size_t Capacity>
void describe_graph_pool(u32* out,u32 id,
                         const multiplayer::RollbackPool<BlockSize,Capacity>& pool) noexcept {
    // Descriptor inputs exactly match hash_pool's coverage. Storage may have
    // alignment tail bytes, so its stride must not be inferred from BlockSize.
    out[0]=id;
    out[1]=u32(reinterpret_cast<std::uintptr_t>(&pool.blocks[0]));
    out[2]=u32(BlockSize);
    out[3]=u32(sizeof(pool.blocks[0]));
    out[4]=u32(Capacity);
    out[5]=u32(reinterpret_cast<std::uintptr_t>(&pool.occupied[0]));
    out[6]=u32(sizeof(pool.occupied));
    out[7]=u32(reinterpret_cast<std::uintptr_t>(&pool.cursor));
    out[8]=pool.cursor;
    out[9]=0;
    for(std::size_t i=0;i<Capacity;++i)if(pool.active(i))++out[9];
}

u32 hash_bullets(const EnemyBulletManager* manager){
    if(!manager)return 0;u32 hash=2166136261u;
    // The manager survives a native stage transition, while direct Replay
    // bootstrap creates a fresh owner. Update/draw handles, draw-list links,
    // animation geometry, reserved bytes and draw-derived ANM script_position
    // are presentation/graph state. Item-independent ANM state, bullet
    // commands, modifiers, motion and all timers remain part of the hash.
    hash=hash_bytes(manager->manager_fields,sizeof(manager->manager_fields),hash);
    const u32 cursor=manager->cursor>=manager->pool&&manager->cursor<=manager->pool+2000?
        u32(manager->cursor-manager->pool):0xffffffffu;
    hash=hash_value(cursor,hash);
    hash=hash_value(manager->last_cancel_position,hash);
    hash=hash_value(manager->last_cancel_size,hash);
    hash=hash_value(manager->active_count,hash);
    for(u32 i=0;i<2000;++i)if(manager->pool[i].state){
        const auto& bullet=manager->pool[i];hash=hash_value(i,hash);
        hash=hash_value(bullet.flags,hash);hash=hash_value(bullet.cancel_protection,hash);
        AnmVm animation=bullet.animation;animation.script_position={};
        hash=hash_anm_vm_portable(animation,hash);
        hash=hash_cstring_portable(bullet.animation.animation_file?bullet.animation.animation_file->name:nullptr,hash);
        hash=hash_value(bullet.motion.position,hash);hash=hash_value(bullet.motion.velocity,hash);
        hash=hash_value(bullet.motion.speed,hash);hash=hash_value(bullet.motion.angle,hash);
        hash=hash_value(bullet.cancel_size,hash);hash=hash_value(bullet.hitbox_height,hash);
        hash=hash_timer_state(bullet.cancel_timer,hash);hash=hash_value(bullet.cancel_timer_flags,hash);
        hash=hash_timer_state(bullet.secondary_timer,hash);hash=hash_value(bullet.secondary_timer_flags,hash);
        hash=hash_value(bullet.id,hash);hash=hash_value(bullet.outside_delay,hash);
        hash=hash_value(bullet.cancel_script,hash);hash=hash_value(bullet.active_features,hash);
        hash=hash_value(bullet.spawn_flags,hash);hash=hash_value(bullet.state,hash);
        hash=hash_value(bullet.turn_sound,hash);hash=hash_value(bullet.command_index,hash);
        hash=hash_value(bullet.draw_layer,hash);
        for(const auto& command:bullet.commands){
            for(const auto argument:command.arguments)hash=hash_value(argument,hash);
            hash=hash_value(command.type,hash);hash=hash_value(command.concurrent,hash);
        }
        for(const auto& modifier:bullet.modifiers){
            hash=hash_timer_state(modifier.timer,hash);hash=hash_value(modifier.timer_flags,hash);
            hash=hash_value(modifier.first,hash);hash=hash_value(modifier.second,hash);
            hash=hash_value(modifier.vector,hash);hash=hash_value(modifier.duration,hash);
            hash=hash_value(modifier.count,hash);hash=hash_value(modifier.iteration,hash);
        }
        hash=hash_value(bullet.sprite_type,hash);hash=hash_value(bullet.color,hash);
    }
    return hash;
}
u32 hash_items(const ItemManager* manager){
    if(!manager)return 0;u32 hash=2166136261u;
    // Stage transitions retain this manager, while direct Replay creates a
    // fresh one. Callback/resource pointers are graph identity, not gameplay
    // state. Item::draw derives animation.script_position from the visible
    // item position every draw, so omit that presentation cache from the
    // semantic VM hash while retaining every authored animation field.
    hash=hash_value(manager->flags,hash);
    hash=hash_value(manager->manager_state,hash);
    const auto add=[&](const Item& item,u32 slot,u32 pool){
        if(!item.state)return;
        hash=hash_value(pool,hash);hash=hash_value(slot,hash);
        hash=hash_value(item.state,hash);hash=hash_value(item.kind,hash);
        hash=hash_value(item.sprite_kind,hash);hash=hash_value(item.position,hash);
        hash=hash_value(item.velocity,hash);hash=hash_value(item.motion_3c4,hash);
        hash=hash_timer_state(item.timer,hash);hash=hash_value(item.timer_flags,hash);
        hash=hash_value(item.attraction_speed,hash);hash=hash_value(item.value_variant,hash);
        AnmVm animation=item.animation;animation.script_position={};
        hash=hash_anm_vm_portable(animation,hash);
    };
    for(u32 i=0;i<150;++i)add(manager->regular[i],i,0);
    for(u32 i=0;i<2048;++i)add(manager->faith[i],i,1);
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
    for(u32 i=0;i<multiplayer::kAnimationOverflowCapacity;++i)if(engine.rollback_animation_overflow.active(i)){
        result.overflow=hash_value(i,result.overflow);
        result.overflow=hash_anm_vm_authored(*static_cast<const AnmVm*>(engine.rollback_animation_overflow.at(i)),result.overflow);
    }
    return result;
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
bool loading_tree_contains(const AnmVm& root,const AnmVm* candidate,u32 depth){
    if(&root==candidate)return true;
    if(depth>=32)return false;
    for(auto* node=root.child_node.next;node;node=node->next){
        if(!node->value)continue;
        if(node->value==candidate||loading_tree_contains(*node->value,candidate,depth+1))return true;
    }
    return false;
}
bool is_startup_loading_vm(const browser::AnimationEngine& engine,const AnmVm& candidate){
    const auto* candidateFile=candidate.animation_file;
    if(!candidateFile||std::strcmp(candidateFile->name,"ascii.anm")!=0)return false;
    const auto& registry=engine.manager.registry;
    const auto contains=[&](ListNode<AnmVm>* node){
        for(;node;node=node->next){
            if(!node->value)continue;const auto& root=*node->value;const auto* file=root.animation_file;
            if(file&&std::strcmp(file->name,"ascii.anm")==0&&root.script_index==6&&
               loading_tree_contains(root,&candidate,0))return true;
        }
        return false;
    };
    return contains(registry.world_head)||contains(registry.ui_head);
}
u32 hash_anm_portable(const browser::AnimationEngine& engine){
    const auto& manager=engine.manager;u32 hash=2166136261u;
    // Pool slots, ANM ids, and file slots are local handles. Preserve the
    // authored update order and resource/script identity across both lists.
    // Only the ASCII script-6 loading root and its actual child tree are
    // omitted: the host retains that startup graph after the viewer joins.
    for(u32 listIndex=0;listIndex<2;++listIndex){
        hash=hash_value(listIndex,hash);u32 count=0;
        auto* node=listIndex?manager.registry.ui_head:manager.registry.world_head;
        for(;node;node=node->next){
            if(!node->value)continue;const auto& source=*node->value;
            if(is_startup_loading_vm(engine,source))continue;
            const auto* file=source.animation_file;
            hash=hash_cstring_portable(file?file->name:nullptr,hash);
            auto vm=source;vm.id=0;vm.file_index=0;
            hash=hash_anm_vm_portable(vm,hash);++count;
        }
        hash=hash_value(count,hash);
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
    if(!app||app->stopped)return 0;
    const bool preview=app->room_preview&&app->value.screen==4&&app->title&&!app->title->loading&&
        app->world&&!app->world->actors.session;
    if(!preview&&(app->world||app->value.screen!=-2))return 0;
    multiplayer::SessionSetup next=app->state.multiplayer_session;
    if(!multiplayer::DecodeSessionSetup(next,words,count))return 0;
    if(next.sessionId&&!app->state.netplay_runtime.Reset(next,next.sessionId))return 0;
    if(!next.sessionId)app->state.netplay_runtime.Clear();
    app->state.input_lanes={};
    app->room_preview=false;app->state.inactive_frames=app->state.demo_index=0;
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
    if(!app||app->stopped||count!=5||!app->state.netplay_runtime.CanCapture()||
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
extern "C" __attribute__((export_name("multiplayer_spectator_load_debug")))
const i32* multiplayer_spectator_load_debug(browser::Application* app){
    static i32 words[29]{};std::fill(words,words+29,0);if(!app)return words;
    words[0]=app->error;
    words[24]=app->startup?app->startup->error:0;words[25]=app->title?app->title->error:0;
    words[26]=app->credits?app->credits->error:0;words[27]=app->value.screen;words[28]=app->value.pending_screen;
    auto* world=app->world;if(!world)return words;
    words[1]=world->error;words[2]=world->backgrounds.error;words[3]=world->loading;
    words[4]=i32(world->loading_progress.phase);words[5]=world->new_game;
    words[6]=world->state.game.flags;words[7]=world->state.game.stage;
    words[8]=world->actors.session!=nullptr;words[9]=world->state.current_stage!=nullptr;
    words[10]=world->backgrounds.current!=nullptr;words[11]=world->actors.gui!=nullptr;
    words[12]=world->actors.player!=nullptr;words[13]=world->actors.bullets!=nullptr;
    words[14]=world->actors.items!=nullptr;words[15]=world->actors.lasers!=nullptr;
    words[16]=world->actors.results!=nullptr;words[17]=world->actors.hints!=nullptr;
    words[18]=world->actors.popups!=nullptr;words[19]=world->actors.enemies!=nullptr;
    words[20]=world->actors.effects!=nullptr;words[21]=world->actors.bomb!=nullptr;
    words[22]=world->actors.spell!=nullptr;words[23]=world->state.replay!=nullptr;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_calibration_status")))
const u32* multiplayer_calibration_status(browser::Application* app){static u32 empty[32]{};
    return app?app->state.netplay_runtime.CalibrationStatus():empty;}
extern "C" __attribute__((export_name("multiplayer_netplay_status")))
const i32* multiplayer_netplay_status(browser::Application* app){
    static i32 words[11]{};std::fill(words,words+11,0);if(!app)return words;
    auto& runtime=app->state.netplay_runtime;
    words[0]=runtime.Configured();words[1]=runtime.CanStart();
    words[2]=i32(runtime.NextFrame());words[3]=i32(runtime.LastSimulatedFrame());
    words[4]=i32(runtime.ConfirmedThroughAllRemotes());
    words[5]=runtime.HasRollbackRequest()?i32(runtime.RollbackFrame()):-1;
    words[6]=app->world&&app->world->rollback?i32(app->world->rollback->CapturedBytes(
        runtime.LastSimulatedFrame()==Netplay::INVALID_FRAME?0:runtime.LastSimulatedFrame())):0;
    const auto sessionId=app->state.multiplayer_session.sessionId;
    words[7]=i32(u32(sessionId));words[8]=i32(u32(sessionId>>32));
    words[9]=app->multiplayer_active()?1:0;
    words[10]=app->world&&app->world->actors.session?1:0;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_smoothness_status")))
const i32* multiplayer_smoothness_status(browser::Application* app){
    // Host-owned observations only: no journal writes, clock updates, input
    // consumption, or semantic hashes in the presentation callback probe.
    static i32 words[10]{};std::fill(words,words+10,0);words[0]=1;
    if(!app)return words;
    auto& runtime=app->state.netplay_runtime;
    words[1]=i32(app->multiplayer_recovery_yields);
    words[2]=runtime.Correcting()?1:0;
    words[3]=runtime.Correcting()?i32(runtime.CorrectionEnd()):-1;
    words[4]=i32(runtime.PredictionFrontier());
    words[5]=i32(runtime.MissingRemoteInputs());
    words[6]=i32(runtime.IntervalScale()*1000.0+0.5);
    words[7]=app->multiplayer_forward_advanced?1:0;
    words[8]=app->multiplayer_corrected_presentation_pending?1:0;
    words[9]=runtime.HasLocalCapture(runtime.NextFrame())?1:0;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_projection_cache_status")))
const double* multiplayer_projection_cache_status(browser::Application* app){
    // Read only after the measured run: derived whole-world-lifetime counters.
    static double words[5]{};std::fill(words,words+5,0);words[0]=1;
    if(!app||!app->world)return words;
    const auto& count=app->world->backgrounds.projection_cache.counters();
    words[1]=double(count.hits);words[2]=double(count.misses);
    words[3]=double(count.evictions);words[4]=double(count.fallbacks);
    return words;
}
extern "C" __attribute__((export_name("multiplayer_camera_math_cache_set_enabled")))
u32 multiplayer_camera_math_cache_set_enabled(u32 enabled){
    return browser::camera_math_cache().set_enabled(enabled!=0)?1u:0u;
}
extern "C" __attribute__((export_name("multiplayer_camera_math_cache_set_verify_every")))
u32 multiplayer_camera_math_cache_set_verify_every(u32 every){
    auto& cache=browser::camera_math_cache();cache.set_verify_every(every);
    return cache.verify_every();
}
extern "C" __attribute__((export_name("multiplayer_camera_math_cache_status")))
const double* multiplayer_camera_math_cache_status(){
    // Fixed derived counters, sampled outside the measured callback hot path.
    // schema/enabled/supported/verifyEvery/invalidations, then two 7-word sets:
    // calls/hits/misses/evictions/bypasses/verified/mismatches.
    static double words[19]{};auto& cache=browser::camera_math_cache();
    const auto& count=cache.counters();
    words[0]=1;words[1]=cache.enabled();words[2]=browser::CameraMathCache::supported;
    words[3]=cache.verify_every();words[4]=double(count.invalidations);
    const auto copy=[&](u32 at,const browser::CameraMathCache::FunctionCounters& c){
        words[at]=double(c.calls);words[at+1]=double(c.hits);words[at+2]=double(c.misses);
        words[at+3]=double(c.evictions);words[at+4]=double(c.bypasses);
        words[at+5]=double(c.verified);words[at+6]=double(c.mismatches);
    };
    copy(5,count.look_at);copy(12,count.perspective);return words;
}
extern "C" __attribute__((export_name("multiplayer_camera_math_cache_verification")))
const u32* multiplayer_camera_math_cache_verification(u32 first_mismatch){
    // schema, serial lo/hi, function, keyWords, matched, output flags/errno,
    // 14 key words, then the cached and original full 16-word matrices.
    static u32 words[56]{};std::fill(words,words+56,0);words[0]=1;
    const auto& cache=browser::camera_math_cache();
    const auto& sample=first_mismatch?cache.first_mismatch():cache.last_verification();
    words[1]=u32(sample.number);words[2]=u32(sample.number>>32);
    words[3]=u32(sample.function);words[4]=sample.key_words;words[5]=sample.matched;
    words[6]=sample.cached_flags;words[7]=sample.original_flags;
    std::memcpy(words+8,&sample.cached_errno,sizeof(sample.cached_errno));
    std::memcpy(words+9,&sample.original_errno,sizeof(sample.original_errno));
    std::memcpy(words+10,sample.key.data(),sizeof(sample.key));
    std::memcpy(words+24,&sample.cached,sizeof(sample.cached));
    std::memcpy(words+40,&sample.original,sizeof(sample.original));return words;
}
extern "C" __attribute__((export_name("multiplayer_packed_occupancy_scan_set_enabled")))
u32 multiplayer_packed_occupancy_scan_set_enabled(u32 enabled){
    return multiplayer::RollbackState::SetPackedOccupancyScan(enabled!=0)?1u:0u;
}
extern "C" __attribute__((export_name("multiplayer_packed_occupancy_scan_status")))
u32 multiplayer_packed_occupancy_scan_status(){
    return multiplayer::RollbackState::PackedOccupancyScanEnabled()?1u:0u;
}
extern "C" __attribute__((export_name("multiplayer_initial_capture_batch_set_enabled")))
u32 multiplayer_initial_capture_batch_set_enabled(u32 enabled){
    return multiplayer::RollbackState::SetInitialCaptureBatch(enabled!=0)?1u:0u;
}
extern "C" __attribute__((export_name("multiplayer_initial_capture_batch_status")))
const double* multiplayer_initial_capture_batch_status(browser::Application* app){
    // Fixed counters only; read outside the measured callback hot path.
    // schema/enabled/supported/open/pendingBlocks/pendingBytes, then
    // scopes/deferredBlocks/deferredBytes/copiedBlocks/copiedBytes/
    // flushes/batchImports/maxBlocksPerFlush.
    static double words[14]{};std::fill(words,words+14,0.);
    words[0]=1;words[1]=multiplayer::RollbackState::InitialCaptureBatchEnabled();
    words[2]=multiplayer::RollbackState::InitialCaptureBatchSupported();
    if(!app||!app->world||!app->world->rollback)return words;
    const auto& rollback=*app->world->rollback;
    const auto& stats=rollback.InitialCaptureBatchStats();
    words[3]=rollback.InitialCaptureBatchOpen();
    words[4]=double(rollback.PendingInitialCaptureBlocks());
    words[5]=double(rollback.PendingInitialCaptureBytes());
    words[6]=double(stats.scopes);words[7]=double(stats.deferredBlocks);
    words[8]=double(stats.deferredBytes);words[9]=double(stats.copiedBlocks);
    words[10]=double(stats.copiedBytes);words[11]=double(stats.flushes);
    words[12]=double(stats.batchImports);words[13]=double(stats.maxBlocksPerFlush);
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
    // Include transitive hint contents and allocator topology, not merely the
    // list-head pointers. This strengthens the same-layout rollback oracle.
    scene=hash_pool(w.rollback_hints,scene);
    scene=hash_pool(w.rollback_hint_owners,scene);
    u32 chain=hash_value(app->engine.chain_value);chain=hash_pool(app->engine.callback_environment.rollback_entries,chain);chain=hash_pool(app->effects.rollback_effects,chain);
    // These stable owner pools are part of the callback graph. Preserve the
    // original word-11 prefix and also verify their allocator and raw payload.
    chain=hash_pool(w.rollback_laser_owners,chain);
    chain=hash_pool(w.rollback_bomb_owners,chain);
    chain=hash_pool(w.rollback_session_owners,chain);
    chain=hash_pool(w.rollback_game_effect_owners,chain);
    const auto anm=hash_anm_parts(app->engine);
    words[0]=2;words[2]=economy;words[3]=rng;words[4]=players;words[5]=enemies;words[6]=hash_bullets(w.actors.bullets);words[7]=lasers;words[8]=items;words[9]=hash_anm_portable(app->engine);words[10]=scene;words[11]=chain;
    u32 composite=2166136261u;for(u32 i=2;i<12;++i)composite=hash_value(words[i],composite);words[1]=composite;words[12]=app->state.multiplayer_session.started;
    words[13]=app->engine.script_random.seed;words[14]=app->engine.script_random.calls;
    words[15]=app->engine.visual_random.seed;words[16]=app->engine.visual_random.calls;
    words[17]=anm.metadata;words[18]=anm.pooled;words[19]=anm.overflow;
    auto& manager=app->engine.manager;u32 occupied=0,worldCount=0,uiCount=0,overflowCount=0;
    for(const auto value:manager.occupied)occupied+=value?1u:0u;
    for(auto* node=manager.registry.world_head;node;node=node->next)++worldCount;
    for(auto* node=manager.registry.ui_head;node;node=node->next)++uiCount;
    for(u32 i=0;i<multiplayer::kAnimationOverflowCapacity;++i)overflowCount+=app->engine.rollback_animation_overflow.active(i)?1u:0u;
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
// Complete raw storage descriptors for the scene/chain graph investigation.
// Schema 4 adds the stable GameSession and GameEffects owner pools to the u32 table,
// copied synchronously with the pointed-to
// bytes before any runtime step or heap growth. This function allocates no
// heap storage, changes no owner, and does not replace the raw 44-word oracle.
//
// Header (10 words):
//   0 version=4, 1 headerWords=10, 2 poolDescriptorWords=10, 3 poolCount,
//   4 pointerBytes, 5 chain_value address, 6 sizeof(UpdateChain),
//   7 sizeof(UpdateChainEntry), 8 offsetof(update), 9 offsetof(draw).
// poolCount=0 means no valid wasm32 application/world; callers must reject it.
// Pool IDs: 1 world.rollback_hints, 2 callback_environment.rollback_entries,
//           3 effects.rollback_effects, 4 world.rollback_hint_owners,
//           5 world.rollback_laser_owners, 6 world.rollback_bomb_owners,
//           7 world.rollback_session_owners, 8 world.rollback_game_effect_owners.
// Each pool descriptor (10 words):
//   0 id, 1 blocks address, 2 BlockSize, 3 sizeof(Storage), 4 Capacity,
//   5 occupied address, 6 occupied bytes, 7 cursor address, 8 cursor value,
//   9 activeCount. Copy every occupied byte and the cursor, then every nonzero
//   occupied slot's BlockSize bytes at blocks + slot * sizeof(Storage), keeping
//   its original slot index. Do not include alignment tails, remap slot order,
//   normalize occupied values, or omit active slots from the raw comparison.
extern "C" __attribute__((export_name("multiplayer_graph_storage")))
const u32* multiplayer_graph_storage(browser::Application* app){
    constexpr u32 headerWords=10,poolWords=10,poolCount=8;
    static u32 words[headerWords+poolWords*poolCount]{};
    std::fill(words,words+headerWords+poolWords*poolCount,0u);
    words[0]=4;words[1]=headerWords;words[2]=poolWords;words[4]=u32(sizeof(void*));
    if(sizeof(std::uintptr_t)>sizeof(u32)||!app||!app->world)return words;
    const auto& chain=app->engine.chain_value;const auto& world=*app->world;
    words[3]=poolCount;
    words[5]=u32(reinterpret_cast<std::uintptr_t>(&chain));
    words[6]=u32(sizeof(chain));words[7]=u32(sizeof(UpdateChainEntry));
    words[8]=u32(offsetof(UpdateChain,update));words[9]=u32(offsetof(UpdateChain,draw));
    describe_graph_pool(words+headerWords,1,world.rollback_hints);
    describe_graph_pool(words+headerWords+poolWords,2,app->engine.callback_environment.rollback_entries);
    describe_graph_pool(words+headerWords+2*poolWords,3,app->effects.rollback_effects);
    describe_graph_pool(words+headerWords+3*poolWords,4,world.rollback_hint_owners);
    describe_graph_pool(words+headerWords+4*poolWords,5,world.rollback_laser_owners);
    describe_graph_pool(words+headerWords+5*poolWords,6,world.rollback_bomb_owners);
    describe_graph_pool(words+headerWords+6*poolWords,7,world.rollback_session_owners);
    describe_graph_pool(words+headerWords+7*poolWords,8,world.rollback_game_effect_owners);
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
namespace {
u32 hash_stage_scene(const Stage* s){
    u32 h=hash_value(s!=nullptr);if(!s)return h;
    h=hash_value(s->stage_number,h);h=hash_cstring_portable(s->file?s->file->animation_name:nullptr,h);
    h=hash_value(s->flags,h);h=hash_value(s->state,h);h=hash_value(s->camera_effect,h);
    h=hash_timer_state(s->effect_timer,h);h=hash_value(s->effect_timer_flags,h);
    h=hash_timer_state(s->script_timer,h);h=hash_value(s->script_timer_flags,h);
    const i32 pc=s->instruction&&s->script_begin?i32(reinterpret_cast<const u8*>(s->instruction)-
        reinterpret_cast<const u8*>(s->script_begin)):-1;h=hash_value(pc,h);
    h=hash_interpolator_portable(s->target_interpolation,h);
    h=hash_interpolator_portable(s->position_interpolation,h);
    h=hash_interpolator_portable(s->fog_interpolation,h);
    h=hash_value(s->reserved_174,h);h=hash_value(s->draw_distance_squared,h);
    h=hash_value(s->effects_enabled,h);h=hash_value(s->fade_color,h);h=hash_value(s->frame_effect,h);
    h=hash_value(s->effect_position,h);h=hash_value(s->effect_size,h);
    h=hash_value(s->draw_flags,h);h=hash_timer_state(s->fade_timer,h);
    h=hash_value(s->fade_timer_flags,h);h=hash_value(s->frame_count,h);
    // StageResources inherits this render basis from the previous engine
    // camera. Every world draw recomputes the active basis in configure_world
    // before billboard use; Stage simulation never reads this cached right.
    // Keep its raw value in diagnostics and on the wire, outside this oracle.
    auto camera=s->camera;camera.right={};h=hash_value(camera,h);
    const bool initialized=s->instruction!=nullptr;h=hash_value(initialized,h);
    // StageResources allocates the primitive VM array before Stage::restart
    // initializes it. A viewer can expose this valid loading state while its
    // checkpoint is buffered; those raw allocations contain no VM state yet.
    if(!initialized)return h;
    const auto vmHash=[&](const AnmVm& original){
        auto vm=original;vm.id=vm.owner_tag=0;vm.file_index=0;
        h=hash_cstring_portable(vm.animation_file?vm.animation_file->name:nullptr,h);
        h=hash_anm_vm_portable(vm,h);
    };
    if(s->file)for(i32 i=0;i<s->file->object_count;++i){
        const auto* object=s->objects[i];h=hash_value(object->id,h);h=hash_value(object->layer,h);
        h=hash_value(object->flags,h);
        for(auto* p=const_cast<StageObject*>(object)->primitives();p->type>=0;p=p->next()){
            h=hash_value(p->animation,h);vmHash(s->object_animations[p->animation]);
        }
    }
    for(const auto& vm:s->script_animations)vmHash(vm);
    for(const auto& vm:s->effect_animations)vmHash(vm);
    return h;
}
u32 hash_vec2_fields(const Vec2& value,u32 hash=2166136261u){
    hash=hash_value(float_bits(value.x),hash);return hash_value(float_bits(value.y),hash);
}
u32 hash_vec3_fields(const Vec3& value,u32 hash=2166136261u){
    hash=hash_value(float_bits(value.x),hash);hash=hash_value(float_bits(value.y),hash);
    return hash_value(float_bits(value.z),hash);
}
u32 hash_matrix_fields(const Matrix4& value){
    u32 hash=2166136261u;
    for(const auto& row:value.elements)for(const auto component:row)
        hash=hash_value(float_bits(component),hash);
    return hash;
}
u32 hash_camera_fog_fields(const AnmFog& value,u32 hash=2166136261u){
    hash=hash_value(float_bits(value.near_distance),hash);
    hash=hash_value(float_bits(value.far_distance),hash);
    for(const auto component:value.color)hash=hash_value(float_bits(component),hash);
    return hash_value(value.packed_color,hash);
}
template<class T,unsigned N>u32 hash_interpolator_fields(const Interpolator<T,N>& value){
    u32 hash=2166136261u;
    for(const auto component:value.start)hash=hash_value(component,hash);
    for(const auto component:value.end)hash=hash_value(component,hash);
    for(const auto component:value.initial_tangent)hash=hash_value(component,hash);
    for(const auto component:value.final_tangent)hash=hash_value(component,hash);
    hash=hash_timer_state(value.timer,hash);hash=hash_value(value.flags,hash);
    hash=hash_value(value.duration,hash);return hash_value(value.mode,hash);
}
u32 hash_fog_interpolator_fields(const FogInterpolator& value){
    u32 hash=2166136261u;
    for(const auto* fog:{&value.start,&value.end,&value.initial_tangent,&value.final_tangent})
        hash=hash_camera_fog_fields(*fog,hash);
    hash=hash_timer_state(value.timer,hash);hash=hash_value(value.flags,hash);
    hash=hash_value(value.duration,hash);return hash_value(value.mode,hash);
}
u32 hash_stage_vm_fields(const AnmVm& original,u32 hash=2166136261u){
    auto vm=original;vm.id=vm.owner_tag=0;vm.file_index=0;
    hash=hash_cstring_portable(vm.animation_file?vm.animation_file->name:nullptr,hash);
    return hash_anm_vm_portable(vm,hash);
}
bool write_stage_scene_fields(const Stage& s,u32* out){
    const bool initialized=s.instruction!=nullptr;
    const auto instructionOffset=initialized&&s.script_begin?
        i32(reinterpret_cast<const u8*>(s.instruction)-reinterpret_cast<const u8*>(s.script_begin)):-1;
    const auto timerRate=[](const Timer& timer){return float_bits(timer.rate?*timer.rate:0.0f);};
    // Per-row word map (64 words):
    // 0 initialized, 1 stage_number, 2 flags, 3 state, 4 camera_effect;
    // 5-9 effect timer (prev,current,fraction,rate,flags), 10-14 script timer;
    // 15 instruction PC, 16-18 target/position/fog interpolation hashes;
    // 19 reserved_174, 20 draw_distance, 21 effects_enabled, 22 fade_color,
    // 23 frame_effect, 24 effect_position, 25 effect_size, 26-29 draw counts/flags;
    // 30-34 fade timer, 35 frame_count, 36 object flags, 37 primitive descriptors,
    // 38 primitive VM group, 39 script VM group, 40 effect VM group;
    // 41-46 Camera named vectors, 47 FOV, 48 view, 49 projection,
    // 50-55 viewport fields, 56 screen_space, 57 draw_offset, 58 animation_delta,
    // 59 fog, 60 resource name, 61 object count, 62 primitive count, 63 source size.
    out[0]=initialized;out[1]=u32(s.stage_number);out[2]=s.flags;out[3]=s.state;
    out[4]=s.camera_effect;
    out[5]=u32(s.effect_timer.previous);out[6]=u32(s.effect_timer.current);
    out[7]=float_bits(s.effect_timer.fractional);out[8]=timerRate(s.effect_timer);out[9]=s.effect_timer_flags;
    out[10]=u32(s.script_timer.previous);out[11]=u32(s.script_timer.current);
    out[12]=float_bits(s.script_timer.fractional);out[13]=timerRate(s.script_timer);out[14]=s.script_timer_flags;
    out[15]=u32(instructionOffset);out[16]=hash_interpolator_fields(s.target_interpolation);
    out[17]=hash_interpolator_fields(s.position_interpolation);
    out[18]=hash_fog_interpolator_fields(s.fog_interpolation);out[19]=s.reserved_174;
    out[20]=float_bits(s.draw_distance_squared);out[21]=s.effects_enabled;out[22]=s.fade_color;
    out[23]=u32(s.frame_effect);out[24]=hash_vec3_fields(s.effect_position);
    out[25]=hash_vec3_fields(s.effect_size);out[26]=s.drawn_objects;out[27]=s.culled_objects;
    out[28]=s.drawn_primitives;out[29]=s.draw_flags;
    out[30]=u32(s.fade_timer.previous);out[31]=u32(s.fade_timer.current);
    out[32]=float_bits(s.fade_timer.fractional);out[33]=timerRate(s.fade_timer);out[34]=s.fade_timer_flags;
    out[35]=s.frame_count;
    out[60]=hash_cstring_portable(s.file?s.file->animation_name:nullptr,2166136261u);
    out[61]=s.file?u32(s.file->object_count):0;out[62]=s.file?u32(s.file->primitive_count):0;
    out[63]=s.source_size;
    const auto& camera=s.camera;
    out[41]=hash_vec3_fields(camera.position);out[42]=hash_vec3_fields(camera.target_offset);
    out[43]=hash_vec3_fields(camera.up);out[44]=hash_vec3_fields(camera.reserved_024);
    out[45]=hash_vec3_fields(camera.right);out[46]=hash_vec3_fields(camera.eye_offset);
    out[47]=float_bits(camera.field_of_view);out[48]=hash_matrix_fields(camera.view);
    out[49]=hash_matrix_fields(camera.projection);out[50]=camera.viewport.x;out[51]=camera.viewport.y;
    out[52]=camera.viewport.width;out[53]=camera.viewport.height;
    out[54]=float_bits(camera.viewport.near_depth);out[55]=float_bits(camera.viewport.far_depth);
    out[56]=camera.screen_space;out[57]=hash_vec2_fields(camera.draw_offset);
    out[58]=hash_vec3_fields(camera.animation_delta);out[59]=hash_camera_fog_fields(camera.fog);
    // StageResources allocates primitive VM storage before restart initializes it.
    // Its raw bytes are not readable until an instruction PC has been installed.
    if(!initialized)return true;
    if(!s.file||!s.objects||(s.file->primitive_count>0&&!s.object_animations))return false;
    u32 objectFlags=2166136261u,primitiveDescriptors=2166136261u;
    u32 primitiveVms=2166136261u,scriptVms=2166136261u,effectVms=2166136261u;
    u32 primitiveCount=0;
    for(i32 object=0;object<s.file->object_count;++object){
        const auto* owner=s.objects[object];if(!owner)return false;
        objectFlags=hash_value(u32(object),objectFlags);objectFlags=hash_value(owner->id,objectFlags);
        objectFlags=hash_value(owner->layer,objectFlags);objectFlags=hash_value(owner->flags,objectFlags);
        for(auto* primitive=const_cast<StageObject*>(owner)->primitives();primitive->type>=0;
            primitive=primitive->next()){
            if(primitiveCount>=u32(s.file->primitive_count)||primitive->animation<0||
               primitive->animation>=s.file->primitive_count)return false;
            primitiveDescriptors=hash_value(u32(object),primitiveDescriptors);
            primitiveDescriptors=hash_value(primitive->type,primitiveDescriptors);
            primitiveDescriptors=hash_value(primitive->length,primitiveDescriptors);
            primitiveDescriptors=hash_value(primitive->script,primitiveDescriptors);
            primitiveDescriptors=hash_value(primitive->animation,primitiveDescriptors);
            primitiveDescriptors=hash_vec3_fields(primitive->position,primitiveDescriptors);
            primitiveDescriptors=hash_vec2_fields(primitive->size,primitiveDescriptors);
            primitiveVms=hash_value(primitiveCount,primitiveVms);
            primitiveVms=hash_value(primitive->animation,primitiveVms);
            primitiveVms=hash_stage_vm_fields(s.object_animations[primitive->animation],primitiveVms);
            ++primitiveCount;
        }
    }
    if(primitiveCount!=u32(s.file->primitive_count))return false;
    for(const auto& vm:s.script_animations)scriptVms=hash_stage_vm_fields(vm,scriptVms);
    for(const auto& vm:s.effect_animations)effectVms=hash_stage_vm_fields(vm,effectVms);
    out[36]=objectFlags;out[37]=primitiveDescriptors;out[38]=primitiveVms;
    out[39]=scriptVms;out[40]=effectVms;return true;
}
u32 hash_screen_effects(browser::World& w){
    const auto ownerHash=[&](const ScreenEffect& e){
        u32 h=hash_value(e.flags);h=hash_value(e.state,h);h=hash_value(e.kind,h);
        h=hash_value(e.reserved_014,h);h=hash_value(e.alpha,h);h=hash_value(e.duration,h);
        for(auto p:e.parameters)h=hash_value(p,h);
        h=hash_value(e.releasing,h);h=hash_timer_state(e.timer,h);return hash_value(e.timer_flags,h);
    };
    u32 h=2166136261u;
    for(auto* sentinel:{&w.chain->update,&w.chain->draw}){
        u32 count=0;
        for(auto* node=sentinel->node.next;node;node=node->next){
            const auto* entry=node->value;
            if(!entry||!w.effects.rollback_effects.owns(entry->owner))continue;
            h=hash_value(entry->priority,h);h=hash_value(entry->flags,h);
            h=hash_value(ownerHash(*static_cast<const ScreenEffect*>(entry->owner)),h);++count;
        }
        h=hash_value(count,h);
    }
    std::vector<u32> ownerHashes;
    for(u32 slot=0;slot<128;++slot)if(w.effects.rollback_effects.active(slot))
        ownerHashes.push_back(ownerHash(*static_cast<const ScreenEffect*>(w.effects.rollback_effects.at(slot))));
    std::sort(ownerHashes.begin(),ownerHashes.end());h=hash_value(u32(ownerHashes.size()),h);
    for(auto value:ownerHashes)h=hash_value(value,h);
    return h;
}
}
extern "C" __attribute__((export_name("multiplayer_stage_scene_hashes")))
const u32* multiplayer_stage_scene_hashes(browser::Application* app){
    // Extend the cross-world oracle to the STD timeline and embedded VMs.
    // The existing twelve-word oracle remains independently checked.
    static u32 out[5]{};out[0]=1;out[1]=out[2]=out[3]=out[4]=0;
    if(!app||!app->world)return out;auto& w=*app->world;
    out[1]=hash_stage_scene(w.backgrounds.current);out[2]=hash_stage_scene(w.backgrounds.previous);
    u32 h=2166136261u;
    for(u32 seat=0;seat<w.player_count;++seat)if(w.pilots[seat].player){
        auto vm=w.pilots[seat].player->animation;vm.id=vm.owner_tag=0;vm.file_index=0;
        h=hash_cstring_portable(vm.animation_file?vm.animation_file->name:nullptr,h);
        h=hash_anm_vm_portable(vm,h);
    }
    out[3]=h;out[4]=hash_screen_effects(w);return out;
}
extern "C" __attribute__((export_name("multiplayer_stage_scene_fields")))
const u32* multiplayer_stage_scene_fields(browser::Application* app){
    // Diagnostic-only field groups; the existing canonical StageScene hash
    // remains the strict parity gate.
    constexpr u32 headerWords=4,rowWords=64;
    static u32 words[headerWords+rowWords*2]{};
    std::fill(words,words+headerWords+rowWords*2,0u);
    words[0]=1;words[3]=rowWords;
    if(!app||!app->world)return words;
    auto& world=*app->world;
    const auto* current=world.backgrounds.current;const auto* previous=world.backgrounds.previous;
    words[1]=current!=nullptr;words[2]=previous!=nullptr;
    if((current&&!write_stage_scene_fields(*current,words+headerWords))||
       (previous&&!write_stage_scene_fields(*previous,words+headerWords+rowWords)))
        words[0]=0;
    return words;
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
extern "C" __attribute__((export_name("multiplayer_ecl_context_debug")))
const u32* multiplayer_ecl_context_debug(browser::Application* app){
    // Read-only, allocation-independent ECL timing diagnostics for the first
    // four active enemies. Instruction addresses are represented as an offset
    // within the owning subroutine, never as a native pointer.
    static u32 words[225]{};std::fill(words,words+225,0);if(!app||!app->world)return words;
    const auto context_words=[](const EclContext& context,const EclProgram* program,u32* out){
        std::memcpy(out,&context.time,sizeof(context.time));
        const auto* instruction=context.instruction;
        out[1]=instruction?u32(instruction->time):0xffffffffu;
        out[2]=instruction?u32(instruction->opcode):0xffffffffu;
        out[3]=instruction?u32(instruction->length):0xffffffffu;
        out[4]=u32(context.stack.top);out[5]=u32(context.stack.frame_base);
        out[6]=u32(context.thread_id);out[7]=context.state_1018;
        out[8]=context.difficulty;out[9]=context.flags;
        out[10]=0;out[11]=0xffffffffu;
        for(u32 i=0;i<4;++i){out[12+i]=0;if(i*4+4<=u32(std::clamp(context.stack.top,0,4096)))
            std::memcpy(out+12+i,context.stack.data+i*4,4);}
        if(instruction){
            if(const auto* sub=ecl_subroutine_for(program,instruction)){
                out[10]=hash_cstring_portable(sub->name,2166136261u);
                out[11]=u32(reinterpret_cast<std::uintptr_t>(instruction)-
                           reinterpret_cast<std::uintptr_t>(sub->header+16));
            }
        }
    };
    auto& world=*app->world;u32 row=0;words[0]=1;
    for(u32 slot=0;slot<512&&row<4;++slot){
        if(!world.rollback_enemies.active(slot))continue;
        const auto& enemy=*static_cast<const Enemy*>(world.rollback_enemies.at(slot));
        auto* out=words+1+row*56;out[0]=slot;
        const auto* program=enemy.script.program;out[1]=program?u32(program->file_count):0;
        out[2]=program?u32(program->subroutine_count):0;
        context_words(enemy.script.root,program,out+3);
        const auto* active=enemy.script.active?enemy.script.active:&enemy.script.root;
        context_words(*active,program,out+19);
        u32 threads=0;for(auto* node=enemy.script.threads.next;node;node=node->next)++threads;
        out[35]=threads;out[36]=u32(enemy.state.lifetime.current);
        out[37]=u32(enemy.state.lifetime.previous);out[38]=u32(enemy.state.boss_slot);
        out[39]=u32(enemy.state.health);
        u32 contextHash=hash_ecl_context_portable(enemy.script.root,program);
        for(auto* node=enemy.script.threads.next;node;node=node->next)
            if(node->value)contextHash=hash_ecl_context_portable(*node->value,program,contextHash);
        out[40]=contextHash;out[41]=hash_enemy_portable(enemy,program);
        const auto& state=enemy.state;
        u32 hash=2166136261u;
        hash=hash_movement_state(state.previous,hash);hash=hash_movement_state(state.current,hash);
        hash=hash_movement_state(state.absolute,hash);hash=hash_movement_state(state.relative,hash);out[42]=hash;
        hash=hash_value(state.hitbox);hash=hash_value(state.collision_box,hash);out[43]=hash;
        hash=hash_value(state.animation_file);hash=hash_value(state.bound_animation_file,hash);
        hash=hash_value(state.animation_script,hash);hash=hash_value(state.base_animation,hash);
        hash=hash_value(state.direction,hash);out[44]=hash;
        hash=hash_bytes(state.integer_variables,sizeof(state.integer_variables));
        hash=hash_bytes(state.float_variables,sizeof(state.float_variables),hash);out[45]=hash;
        hash=hash_timer_state(state.lifetime,2166136261u);hash=hash_value(state.lifetime_flags,hash);out[46]=hash;
        hash=hash_interpolator_portable(state.absolute_position,2166136261u);
        hash=hash_interpolator_portable(state.relative_position,hash);
        hash=hash_interpolator_portable(state.absolute_angle,hash);
        hash=hash_interpolator_portable(state.relative_angle,hash);
        hash=hash_interpolator_portable(state.absolute_radius,hash);
        hash=hash_interpolator_portable(state.relative_radius,hash);out[47]=hash;
        hash=hash_bytes(state.emitters,sizeof(state.emitters));out[48]=hash;
        hash=hash_bytes(state.emitter_offsets,sizeof(state.emitter_offsets));out[49]=hash;
        hash=hash_value(state.visual_size);hash=hash_value(state.clamp_center,hash);
        hash=hash_value(state.clamp_size,hash);out[50]=hash;
        hash=hash_value(state.score);hash=hash_value(state.health,hash);
        hash=hash_value(state.maximum_health,hash);hash=hash_value(state.health_to_interrupt,hash);out[51]=hash;
        hash=hash_value(state.drops);hash=hash_value(state.death_sound,hash);
        hash=hash_value(state.death_animation,hash);hash=hash_value(state.death_animation_file,hash);
        hash=hash_value(state.flash_frames,hash);hash=hash_value(state.reserved_1418,hash);out[52]=hash;
        hash=hash_timer_state(state.damage_immunity,2166136261u);hash=hash_value(state.damage_immunity_flags,hash);
        hash=hash_timer_state(state.collision_immunity,hash);hash=hash_value(state.collision_immunity_flags,hash);out[53]=hash;
        hash=hash_value(state.flags);hash=hash_value(state.alternate_animation,hash);
        hash=hash_value(state.saved_animation,hash);hash=hash_value(state.boss_slot,hash);
        hash=hash_value(state.shot_exclusion_distance_squared,hash);out[54]=hash;
        hash=2166136261u;for(const auto& interrupt:state.interrupts){
            hash=hash_value(interrupt.health,hash);hash=hash_value(interrupt.time,hash);
            hash=hash_cstring_portable(interrupt.health_subroutine,hash);
            hash=hash_cstring_portable(interrupt.time_subroutine,hash);
        }out[55]=hash;++row;
    }
    words[224]=row;return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_parity_debug")))
const u32* multiplayer_spectator_parity_debug(browser::Application* app){
    // Narrow diagnostics for cross-graph spectator parity. Player state is
    // split into the same components used by the semantic hash; ANM list
    // hashes separate world-owned scripts from UI history and allocator IDs.
    static u32 words[96]{};std::fill(words,words+96,0u);words[0]=1;
    if(!app||!app->world)return words;
    auto& world=*app->world;auto& manager=app->engine.manager;
    words[1]=world.player_count;words[2]=manager.started_scripts;
    words[3]=manager.processed_count;words[4]=u32(manager.cursor);words[5]=manager.last_id;
    u32 worldCount=0,uiCount=0,worldHash=2166136261u,uiHash=2166136261u;
    for(auto* node=manager.registry.world_head;node;node=node->next){
        if(!node->value)continue;++worldCount;
        worldHash=hash_anm_vm_portable(*node->value,worldHash);
    }
    for(auto* node=manager.registry.ui_head;node;node=node->next){
        if(!node->value)continue;++uiCount;
        uiHash=hash_anm_vm_portable(*node->value,uiHash);
    }
    words[6]=worldCount;words[7]=uiCount;words[8]=worldHash;words[9]=uiHash;
    for(u32 seat=0;seat<world.player_count&&seat<3;++seat){
        const auto& pilot=world.pilots[seat];const auto parts=pilot.player?hash_player_parts(*pilot.player):PlayerHashParts{};
        auto* out=words+12+seat*20;out[0]=pilot.input_keys;out[1]=pilot.player?1u:0u;
        out[2]=parts.core;out[3]=parts.timers;out[4]=parts.shotCount;out[5]=parts.shots;
        out[6]=parts.options;out[7]=parts.damage;out[8]=parts.activeLasers;
        out[9]=parts.optionSpeed;out[10]=parts.invulnerability;out[11]=parts.bounds;
        out[12]=parts.history;out[13]=u32(pilot.game.character);out[14]=u32(pilot.game.shot_type);
        out[15]=u32(pilot.game.lives);out[16]=u32(pilot.game.power);
        if(pilot.player){out[17]=u32(pilot.player->state);out[18]=u32(pilot.player->fixed_position.x);
            out[19]=u32(pilot.player->fixed_position.y);}
    }
    return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_options_debug")))
const u32* multiplayer_spectator_options_debug(browser::Application* app){
    // Expose the authored option-follow state behind the player hash component.
    static u32 words[256]{};std::fill(words,words+256,0u);words[0]=1;
    if(!app||!app->world)return words;
    const auto& world=*app->world;words[1]=world.player_count;
    for(u32 seat=0;seat<world.player_count&&seat<3;++seat){
        const auto& pilot=world.pilots[seat];auto* out=words+4+seat*84;
        if(!pilot.player)continue;
        out[0]=pilot.input_keys;out[1]=u32(pilot.player->option_count);
        for(u32 index=0;index<4;++index){
            const auto& option=pilot.player->options[index];auto* row=out+2+index*20;
            row[0]=u32(option.active);row[1]=u32(option.target.x);row[2]=u32(option.target.y);
            row[3]=u32(option.position.x);row[4]=u32(option.position.y);
            row[5]=u32(option.offset.x);row[6]=u32(option.offset.y);
            row[7]=u32(option.focused_offset.x);row[8]=u32(option.focused_offset.y);
            row[9]=option.animations[0];row[10]=option.animations[1];
            row[11]=u32(option.previous_focus);row[12]=u32(option.index);
            row[13]=u32(option.snap_next);row[14]=option.on_update;row[15]=option.on_draw;
        }
    }
    return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_entity_counts")))
const u32* multiplayer_spectator_entity_counts(browser::Application* app){
    static u32 words[8]{};std::fill(words,words+8,0u);words[0]=1;
    if(!app||!app->world)return words;
    const auto& world=*app->world;u32 enemies=0,regularItems=0,faithItems=0;
    for(u32 slot=0;slot<512;++slot)if(world.rollback_enemies.active(slot))++enemies;
    if(world.actors.items){
        for(const auto& item:world.actors.items->regular)if(item.state)++regularItems;
        for(const auto& item:world.actors.items->faith)if(item.state)++faithItems;
    }
    words[1]=enemies;
    words[2]=world.actors.bullets?u32(std::max(0,world.actors.bullets->active_count)):0;
    words[3]=world.actors.lasers?u32(std::max(0,world.actors.lasers->count)):0;
    words[4]=regularItems;words[5]=faithItems;
    words[6]=u32(app->state.game.stage_frames);words[7]=u32(app->state.game.section_frames);
    return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_item_debug")))
const u32* multiplayer_spectator_item_debug(browser::Application* app){
    // Header: schema, manager flags/state/count/cursor/faith count,
    // regular/faith active counts, row count, truncated rows. Each row names
    // the item pool and slot, then records every gameplay scalar separately
    // from process-local animation/geometry handles.
    static u32 words[10+512*64]{};std::fill(words,words+10+512*64,0u);words[0]=1;
    if(!app||!app->world||!app->world->actors.items)return words;
    const auto& world=*app->world;const auto& manager=*world.actors.items;
    words[1]=manager.flags;words[2]=manager.manager_state;
    words[3]=u32(manager.active_count);words[4]=u32(manager.faith_cursor);words[5]=u32(manager.faith_count);
    for(const auto& item:manager.regular)words[6]+=item.state?1u:0u;
    for(const auto& item:manager.faith)words[7]+=item.state?1u:0u;
    u32 row=0,truncated=0;
    const auto emit=[&](u32 pool,u32 slot,const Item& item,const multiplayer::ItemOwnership& owner){
        if(!item.state)return;
        if(row>=512){++truncated;return;}
        auto* out=words+10+row*64;out[0]=pool;out[1]=slot;out[2]=u32(item.state);
        out[3]=u32(item.kind);out[4]=u32(item.sprite_kind);
        out[5]=float_bits(item.position.x);out[6]=float_bits(item.position.y);out[7]=float_bits(item.position.z);
        out[8]=float_bits(item.velocity.x);out[9]=float_bits(item.velocity.y);out[10]=float_bits(item.velocity.z);
        out[11]=float_bits(item.motion_3c4);out[12]=u32(item.timer.previous);out[13]=u32(item.timer.current);
        out[14]=float_bits(item.timer.fractional);out[15]=float_bits(item.timer.rate?*item.timer.rate:0.0f);
        out[16]=item.timer_flags;out[17]=float_bits(item.attraction_speed);out[18]=u32(item.value_variant);
        out[19]=u32(i32(owner.homing));out[20]=u32(i32(owner.recipient));
        AnmVm semanticAnimation=item.animation;semanticAnimation.script_position={};
        out[21]=hash_anm_vm_portable(semanticAnimation,2166136261u);out[22]=item.animation.id;out[23]=item.animation.owner_tag;
        out[24]=u32(i32(item.animation.script_index));out[25]=item.animation.file_index;
        out[26]=item.animation.instruction&&item.animation.script_begin?
            u32(reinterpret_cast<const u8*>(item.animation.instruction)-reinterpret_cast<const u8*>(item.animation.script_begin)):
            item.animation.instruction?0xfffffffEu:0xffff'ffffu;
        out[27]=u32(item.animation.script_timer.previous);out[28]=u32(item.animation.script_timer.current);
        out[29]=hash_cstring_portable(item.animation.animation_file?item.animation.animation_file->name:nullptr,2166136261u);
        out[30]=u32(reinterpret_cast<std::uintptr_t>(item.animation.geometry));out[31]=hash_value(item);
        const auto& vm=item.animation;u32 group=2166136261u;
        group=hash_value(vm.rotation,group);out[32]=hash_value(vm.angular_velocity,group);
        group=hash_value(vm.scale,2166136261u);out[33]=hash_value(vm.scale_velocity,group);
        group=hash_value(vm.sprite_size,2166136261u);group=hash_value(vm.uv_offset,group);out[34]=hash_value(vm.uv_velocity,group);
        out[35]=hash_timer_state(vm.script_timer,2166136261u);out[36]=vm.script_timer_flags;
        out[37]=hash_interpolator_portable(vm.position_interpolation,2166136261u);
        out[38]=hash_interpolator_portable(vm.color_interpolation,2166136261u);
        out[39]=hash_interpolator_portable(vm.alpha_interpolation,2166136261u);
        out[40]=hash_interpolator_portable(vm.rotation_interpolation,2166136261u);
        out[41]=hash_interpolator_portable(vm.scale_interpolation,2166136261u);
        out[42]=hash_interpolator_portable(vm.color2_interpolation,2166136261u);
        out[43]=hash_interpolator_portable(vm.alpha2_interpolation,2166136261u);
        group=hash_value(vm.color,2166136261u);group=hash_value(vm.secondary_color,group);
        out[44]=hash_value(vm.pending_interrupt,group);
        out[45]=hash_bytes(vm.integer_variables,sizeof(vm.integer_variables));
        out[46]=hash_bytes(vm.float_variables,sizeof(vm.float_variables));
        out[47]=hash_bytes(vm.extra_integer_variables,sizeof(vm.extra_integer_variables));
        out[48]=hash_value(vm.script_position);out[49]=hash_value(vm.position);out[50]=hash_value(vm.child_position);
        out[51]=vm.flags&~12u;out[52]=hash_timer_state(vm.saved_timer,2166136261u);
        out[53]=vm.saved_timer_flags;
        out[54]=vm.saved_instruction&&vm.script_begin?
            u32(reinterpret_cast<const u8*>(vm.saved_instruction)-reinterpret_cast<const u8*>(vm.script_begin)):
            vm.saved_instruction?0xffff'fffeu:0xffff'ffffu;
        out[55]=u32(vm.sprite_frame);group=hash_value(vm.sprite_index,2166136261u);
        group=hash_value(vm.file_index,group);out[56]=hash_value(vm.script_index,group);
        group=hash_value(vm.sprite_matrix);group=hash_value(vm.transform_matrix,group);
        out[57]=hash_value(vm.uv_matrix,group);out[58]=hash_cstring_portable(vm.animation_file?vm.animation_file->name:nullptr,2166136261u);
        out[59]=float_bits(vm.script_timer.rate?*vm.script_timer.rate:0.0f);
        ++row;
    };
    for(u32 i=0;i<150;++i)emit(0,i,manager.regular[i],world.regular_item_owners[i]);
    for(u32 i=0;i<2048;++i)emit(1,i,manager.faith[i],world.faith_item_owners[i]);
    words[8]=row;words[9]=truncated;return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_bullet_debug")))
const u32* multiplayer_spectator_bullet_debug(browser::Application* app){
    // Header: schema, manager fields, cursor slot, last cancel position/size,
    // active count, live slot count, rows and truncation. Rows expose authored
    // motion/timers/commands/modifiers and separate item-independent ANM state
    // from draw-derived coordinates and local geometry addresses.
    static u32 words[10+512*48]{};std::fill(words,words+10+512*48,0u);words[0]=1;
    if(!app||!app->world||!app->world->actors.bullets)return words;
    const auto& manager=*app->world->actors.bullets;
    words[1]=manager.manager_fields[0];words[2]=manager.manager_fields[1];
    words[3]=manager.cursor>=manager.pool&&manager.cursor<=manager.pool+2000?
        u32(manager.cursor-manager.pool):0xffff'ffffu;
    words[4]=hash_value(manager.last_cancel_position);words[5]=hash_value(manager.last_cancel_size);
    words[6]=u32(manager.active_count);
    for(u32 i=0;i<2000;++i)if(manager.pool[i].state)++words[7];
    u32 row=0,truncated=0;
    for(u32 i=0;i<2000;++i){
        const auto& bullet=manager.pool[i];if(!bullet.state)continue;
        if(row>=512){++truncated;continue;}
        auto* out=words+10+row*48;out[0]=i;out[1]=bullet.state;out[2]=u32(bullet.id);
        out[3]=bullet.flags;out[4]=bullet.cancel_protection;
        out[5]=float_bits(bullet.motion.position.x);out[6]=float_bits(bullet.motion.position.y);
        out[7]=float_bits(bullet.motion.position.z);out[8]=float_bits(bullet.motion.velocity.x);
        out[9]=float_bits(bullet.motion.velocity.y);out[10]=float_bits(bullet.motion.velocity.z);
        out[11]=float_bits(bullet.motion.speed);out[12]=float_bits(bullet.motion.angle);
        out[13]=float_bits(bullet.cancel_size);out[14]=float_bits(bullet.hitbox_height);
        out[15]=hash_timer_state(bullet.cancel_timer,2166136261u);out[16]=bullet.cancel_timer_flags;
        out[17]=hash_timer_state(bullet.secondary_timer,2166136261u);out[18]=bullet.secondary_timer_flags;
        out[19]=u32(bullet.outside_delay);out[20]=u32(bullet.cancel_script);
        out[21]=bullet.active_features;out[22]=bullet.spawn_flags;out[23]=u32(bullet.command_index);
        out[24]=u32(bullet.draw_layer);out[25]=u32(std::int32_t(bullet.sprite_type));
        out[26]=u32(std::int32_t(bullet.color));u32 group=2166136261u;
        for(const auto& command:bullet.commands){
            group=hash_bytes(command.arguments,sizeof(command.arguments),group);
            group=hash_value(command.type,group);group=hash_value(command.concurrent,group);
        }
        out[27]=group;group=2166136261u;
        for(const auto& modifier:bullet.modifiers){
            group=hash_timer_state(modifier.timer,group);group=hash_value(modifier.timer_flags,group);
            group=hash_value(modifier.first,group);group=hash_value(modifier.second,group);
            group=hash_value(modifier.vector,group);group=hash_value(modifier.duration,group);
            group=hash_value(modifier.count,group);group=hash_value(modifier.iteration,group);
        }
        out[28]=group;
        AnmVm semantic=bullet.animation;semantic.script_position={};
        out[29]=hash_anm_vm_portable(semantic,2166136261u);out[30]=bullet.animation.id;
        out[31]=bullet.animation.owner_tag;out[32]=u32(std::int32_t(bullet.animation.script_index));
        out[33]=bullet.animation.file_index;
        out[34]=bullet.animation.instruction&&bullet.animation.script_begin?
            u32(reinterpret_cast<const u8*>(bullet.animation.instruction)-reinterpret_cast<const u8*>(bullet.animation.script_begin)):
            bullet.animation.instruction?0xffff'fffeu:0xffff'ffffu;
        out[35]=hash_timer_state(bullet.animation.script_timer,2166136261u);
        out[36]=hash_cstring_portable(bullet.animation.animation_file?bullet.animation.animation_file->name:nullptr,2166136261u);
        out[37]=hash_value(bullet);out[38]=u32(reinterpret_cast<std::uintptr_t>(bullet.animation.geometry));
        out[39]=float_bits(bullet.animation.script_position.x);out[40]=float_bits(bullet.animation.script_position.y);
        out[41]=float_bits(bullet.animation.script_position.z);out[42]=bullet.animation.flags;
        out[43]=float_bits(bullet.animation.position.x);out[44]=float_bits(bullet.animation.position.y);
        out[45]=float_bits(bullet.animation.position.z);out[46]=u32(std::int32_t(bullet.animation.sprite_index));
        out[47]=u32(bullet.animation.sprite_frame);++row;
    }
    words[8]=row;words[9]=truncated;return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_common_debug")))
const u32* multiplayer_spectator_common_debug(browser::Application* app){
    static u32 words[7]{};std::fill(words,words+7,0u);words[0]=1;
    if(!app||!app->world||!app->world->common.value)return words;
    const auto& common=*app->world->common.value;
    words[1]=common.flags;words[2]=common.state;words[3]=u32(common.frames);
    words[4]=common.introduction_animation;words[5]=common.loading_animation;
    words[6]=common.text_count;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_scores_debug")))
const u32* multiplayer_spectator_scores_debug(browser::Application* app){
    // Field-group hashes split canonical word 43 so score records and random
    // settings can be distinguished from their containing object identity.
    static u32 words[41]{};std::fill(words,words+41,0u);words[0]=1;
    if(!app||!app->world||!app->world->scores.data)return words;
    const auto& scores=*app->world->scores.data;
    words[1]=hash_value(scores.settings.header);
    words[2]=hash_bytes(scores.settings.last_name,sizeof(scores.settings.last_name));
    words[3]=hash_bytes(scores.settings.statistics,sizeof(scores.settings.statistics));
    words[4]=hash_bytes(scores.settings.random_words,sizeof(scores.settings.random_words));
    words[5]=hash_value(scores.settings.reserved_446);
    for(u32 index=0;index<7;++index){
        const auto& character=scores.characters[index];const u32 base=6+index*5;
        words[base]=hash_value(character.header);
        words[base+1]=hash_value(character.character);
        words[base+2]=hash_bytes(character.high_scores,sizeof(character.high_scores));
        words[base+3]=hash_bytes(character.statistics,sizeof(character.statistics));
        words[base+4]=hash_bytes(character.spells,sizeof(character.spells));
    }
    return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_anm_debug")))
const u32* multiplayer_spectator_anm_debug(browser::Application* app){
    // Stable resource/script/timer/position rows identify whether ANM hash
    // divergence is authored state or only process-local handles.
    constexpr u32 header=8,rowWords=17,capacity=40;
    static u32 words[header+capacity*rowWords*2]{};
    std::fill(words,words+header+capacity*rowWords*2,0u);words[0]=1;
    if(!app)return words;const auto& manager=app->engine.manager;
    words[3]=manager.started_scripts;words[4]=manager.processed_count;
    words[5]=u32(manager.cursor);words[6]=manager.last_id;
    const auto writeList=[&](ListNode<AnmVm>* node,u32 base,u32& count){
        for(;node&&count<capacity;node=node->next){
            if(!node->value)continue;const auto& vm=*node->value;
            auto* out=words+base+count*rowWords;
            out[0]=vm.id;out[1]=vm.owner_tag;
            out[2]=hash_cstring_portable(vm.animation_file?vm.animation_file->name:nullptr,2166136261u);
            out[3]=vm.file_index;out[4]=u32(vm.script_index);
            out[5]=vm.instruction&&vm.script_begin?
                u32(reinterpret_cast<std::uintptr_t>(vm.instruction)-
                    reinterpret_cast<std::uintptr_t>(vm.script_begin)):0xffffffffu;
            out[6]=u32(vm.script_timer.previous);out[7]=u32(vm.script_timer.current);
            std::memcpy(out+8,&vm.script_timer.fractional,sizeof(vm.script_timer.fractional));
            out[9]=vm.script_timer_flags;out[10]=u32(vm.sprite_index);
            out[11]=u32(vm.sprite_frame);
            std::memcpy(out+12,&vm.position.x,4);std::memcpy(out+13,&vm.position.y,4);
            std::memcpy(out+14,&vm.position.z,4);
            auto semantic=vm;semantic.id=0;semantic.owner_tag=0;
            out[15]=hash_anm_vm_portable(semantic,2166136261u);
            out[16]=is_startup_loading_vm(app->engine,vm)?1u:0u;++count;
        }
    };
    writeList(manager.registry.world_head,header,words[1]);
    writeList(manager.registry.ui_head,header+capacity*rowWords,words[2]);
    return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_anm_fields")))
const u32* multiplayer_spectator_anm_fields(browser::Application* app){
    // Read-only named diagnostics. Keep the existing portable hash as the gate.
    constexpr u32 header=4,rowWords=80,capacity=128;
    static u32 words[header+capacity*rowWords]{};
    std::fill(words,words+header+capacity*rowWords,0u);
    words[0]=1;words[2]=rowWords;words[3]=capacity;
    if(!app)return words;
    const auto bits=[](float value){u32 result;std::memcpy(&result,&value,4);return result;};
    const auto pc=[](const AnmVm& vm,const AnmInstruction* instruction){
        return instruction&&vm.script_begin?u32(reinterpret_cast<std::uintptr_t>(instruction)-
            reinterpret_cast<std::uintptr_t>(vm.script_begin)):0xffffffffu;
    };
    for(u32 list=0;list<2;++list){
        auto* node=list?app->engine.manager.registry.ui_head:app->engine.manager.registry.world_head;
        u32 order=0;
        for(;node;node=node->next){
            if(!node->value||is_startup_loading_vm(app->engine,*node->value))continue;
            if(words[1]>=capacity){words[0]=0;return words;}
            const auto& vm=*node->value;auto* out=words+header+words[1]++*rowWords;u32 at=0;
            const auto put=[&](u32 value){out[at++]=value;};
            const auto vec2=[&](const auto& value){put(bits(value.x));put(bits(value.y));};
            const auto vec3=[&](const auto& value){vec2(value);put(bits(value.z));};
            const auto timer=[&](const Timer& value,u32 flags){put(u32(value.previous));put(u32(value.current));
                put(bits(value.fractional));put(bits(value.rate?*value.rate:0.0f));put(flags);};
            const auto interp=[&](const auto& value){put(hash_interpolator_portable(value,2166136261u));
                put(bits(value.timer.rate?*value.timer.rate:0.0f));};
            put(list);put(order++);put(hash_cstring_portable(vm.animation_file?vm.animation_file->name:nullptr,2166136261u));
            put(u32(vm.script_index));put(pc(vm,vm.instruction));put(vm.owner_tag);
            vec3(vm.rotation);vec3(vm.angular_velocity);vec2(vm.scale);vec2(vm.scale_velocity);
            vec2(vm.sprite_size);vec2(vm.uv_offset);timer(vm.script_timer,vm.script_timer_flags);
            interp(vm.position_interpolation);interp(vm.color_interpolation);interp(vm.alpha_interpolation);
            interp(vm.rotation_interpolation);interp(vm.scale_interpolation);interp(vm.color2_interpolation);interp(vm.alpha2_interpolation);
            vec2(vm.uv_velocity);put(vm.color);put(vm.secondary_color);put(u32(vm.pending_interrupt));
            for(auto value:vm.integer_variables)put(u32(value));for(auto value:vm.float_variables)put(bits(value));
            for(auto value:vm.extra_integer_variables)put(u32(value));
            vec3(vm.script_position);vec3(vm.position);vec3(vm.child_position);put(vm.flags&~12u);
            timer(vm.saved_timer,vm.saved_timer_flags);put(pc(vm,vm.saved_instruction));
            put(u32(vm.sprite_frame));put(u32(vm.sprite_index));
            put(hash_value(vm.sprite_matrix));put(hash_value(vm.transform_matrix));put(hash_value(vm.uv_matrix));
            put(vm.update_callback);put(vm.draw_callback);put(hash_bytes(vm.text_settings,sizeof(vm.text_settings)));
        }
    }
    return words;
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
