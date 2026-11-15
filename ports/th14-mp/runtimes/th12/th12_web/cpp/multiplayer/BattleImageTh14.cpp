#include "FrameImageTh14.hpp"
#include "../game/GameSession.hpp"

namespace th11 {
namespace {
void save_deformation(FrameImageTh14& image,ScreenDeformation& mesh){
    image.save(mesh.columns,mesh.rows,mesh.vertices,mesh.positions,mesh.animations,mesh.control_animation);
}
}

void AnmManager::capture_room_frame(FrameImageTh14& image){
    image.save(rate,script_rng,visual_rng,texture_high_precision,reference_positions,camera_delta,
        default_tangent,render_space,th14_native_stack,th14_effect_stop,deferred_captures);
    if(auto* control=th14_control.get())image.save(control->flags,control->callbacks_enabled);
    image.save(storage,available,active,geometry_sizes,geometry_owners,geometry_addresses,heads,tails,layers,
        next_id,spawn_depth,th14_draw_tail,th14_draw_epoch,th14_native_slots,th14_unused_slots);
    struct Animation {AnmVm* target;AnmVm value;};
    const auto count=active.size();
    auto* saved=image.plain_storage<Animation>(FrameImageTh14::Array::ActiveAnimations,count);
    if(!image.ready())return;
    std::size_t index=0;
    for(const auto& [id,vm]:active)::new(saved+index++) Animation{vm,*vm};
    image.after_restore([saved,count]{
        for(std::size_t n=0;n<count;++n)*saved[n].target=saved[n].value;
    });
    // initialize() preserves these two words even on idle pool entries.
    // Preserve them without copying a full VM for every unused slot.
    struct Idle {AnmVm* vm;Vec3 position;u32 layer;};
    const auto free=available.size();
    auto* idle=image.plain_storage<Idle>(FrameImageTh14::Array::IdleAnimations,free);
    if(!image.ready())return;
    index=0;
    for(auto* vm:available)::new(idle+index++) Idle{vm,vm->position,vm->layer};
    image.after_restore([idle,free]{
        for(std::size_t n=0;n<free;++n){
            const auto& entry=idle[n];
            entry.vm->position=entry.position;entry.vm->layer=entry.layer;entry.vm->initialize();
        }
    });
    struct Geometry {void* target;std::vector<u8> bytes;};
    std::vector<Geometry> buffers;
    for(const auto& [vm,address]:geometry_addresses){
        const auto* p=static_cast<const u8*>(address);
        const auto bytes=geometry_sizes.at(vm);
        buffers.push_back({address,{p,p+bytes}});
    }
    image.after_restore([buffers=std::move(buffers)]{
        for(const auto& b:buffers)std::memcpy(b.target,b.bytes.data(),b.bytes.size());
    });
}

void EnemyManager::capture_room_frame(FrameImageTh14& image){
    image.save(entries,first,last,count,total_created,timer,spawn_capacity,last_error,script_error,program,spawn_depth);
    for(const auto& [enemy,entry]:entries){
        image.save(*enemy,entry->script.error,entry->script.thread_storage);
        for(auto* node=enemy->script.threads.next;node;node=node->next)image.save(*node,*node->value);
    }
}

void LaserManager::capture_room_frame(FrameImageTh14& image){
    image.save(room_lasers,rate,player,active_count,next_id,last_error,last_center,last_size,sentinel,tail,updating);
    for(auto* laser=first();laser;laser=laser->next)switch(laser->type){
        case 0:image.save(*reinterpret_cast<Th14LaserLine*>(laser));break;
        case 1:image.save(*reinterpret_cast<Th14LaserInfinite*>(laser));break;
        case 2:image.save(*reinterpret_cast<Th14LaserCurve*>(laser));break;
        case 3:image.save(*reinterpret_cast<Th14LaserShell*>(laser));break;
    }
}

void BulletManager::capture_room_frame(FrameImageTh14& image,RoomSlots& slots){
    image.save(rate,player,cursor,active_count,exclusion_squared,last_error,cancel_center,cancel_size,
        heads,tails,fire_depth,th14_free,th14_active,th14_reward_count);
    slots.base=storage.get();
    for(auto* node=th14_active;node;node=node->th14_active_next)slots.save(*node);
}

void ItemManager::capture_room_frame(FrameImageTh14& image,RoomSlots& slots){
    image.save(player,cancel_cursor,cancel_spawn_count,active_count,last_error,th14_pilots,
        th14_pilot_live,th14_pilot_count,th14_collector,th14_poc_owner,th14_stage_attract,th14_spawn_count,
        th14_cancel_count,th14_generation,th14_last_collect_type,th14_cancel_free,th14_ordinary_free,
        th14_occupied,th14_slots_exposed);
    slots.base=storage.get();
    for(u32 page=0;page<th14_occupied.size();++page)if(th14_page_occupied(page))
        for(u32 n=page*th14_word_bits;n<std::min(capacity,(page+1)*th14_word_bits);++n)
            if(storage[n].state)slots.save(storage[n]);
}

void BulletManager::room_digest(u32& hash)const{
    const auto put=[&](u32 v){hash=(hash^v)*16777619u;};
    for(auto* b=th14_active;b;b=b->th14_active_next){
        put(u32(b-storage.get()));put(u32(b->state));put(b->flags);
        put(float_bits(b->position.x));put(float_bits(b->position.y));
        put(float_bits(b->angle));put(float_bits(b->speed));put(b->active_transforms);
    }
}
void ItemManager::room_digest(u32& hash)const{
    const auto put=[&](u32 v){hash=(hash^v)*16777619u;};
    for(u32 page=0;page<th14_occupied.size();++page)if(th14_page_occupied(page))
        for(u32 n=page*th14_word_bits;n<std::min(capacity,(page+1)*th14_word_bits);++n){
            const auto& i=storage[n];if(!i.state)continue;
            put(n);put(u32(i.state));put(u32(i.type));put(u32(i.th14_owner));put(u32(i.th14_attractor));put(u32(i.th14_combo_owner));
            put(float_bits(i.position.x));put(float_bits(i.position.y));put(u32(i.lifetime.current));
        }
}
u32 ShotManager::room_state_digest()const{
    u32 hash=2166136261u;
    const auto put=[&](u32 v){hash=(hash^v)*16777619u;};
    for(const auto& shot:shots)if(shot.state){
        put(u32(shot.state));put(u32(shot.damage));put(u32(shot.timer.current));
        put(float_bits(shot.movement.position.x));put(float_bits(shot.movement.position.y));
        put(shot.th14_target_id);put(shot.th14_area_slot);
    }
    return hash;
}

void Stage::capture_room_frame(FrameImageTh14& image){
    if(mesh)save_deformation(image,*mesh);
    image.save(mesh,state,active_camera,error);
    // The scene topology is fixed between stage boundaries. Only the flags
    // change; the primitive arrays and their allocations can stay in place.
    std::vector<u8> object_flags;
    object_flags.reserve(resource.objects.size());
    for(const auto& object:resource.objects)object_flags.push_back(object.flags);
    std::vector<i16> instance_flags;
    instance_flags.reserve(resource.instances.size());
    for(const auto& instance:resource.instances)instance_flags.push_back(instance.flags);
    image.after_restore([this,objects=std::move(object_flags),instances=std::move(instance_flags)]{
        for(std::size_t i=0;i<objects.size();++i)resource.objects[i].flags=objects[i];
        for(std::size_t i=0;i<instances.size();++i)resource.instances[i].flags=instances[i];
    });
    // The scene keeps one fixed-size VM buffer while a stage is active.
    image.save_block(FrameImageTh14::Array::Stage,object_animations);
}

void BombController::capture_room_frame(FrameImageTh14& image){
    if(mesh)save_deformation(image,*mesh);
    image.save(mesh,state,last_error,th14_orbs,th14_orbs_allocated,th14_reimu_areas,
        th14_reimu_positions,th14_reimu_angles,th14_reimu_shift,th14_reimu_allocated,
        create_script,create_opcode,create_depth,create_scripts);
}

void GameBattle::capture_room_frame(FrameImageTh14& image,BulletManager::RoomSlots& bullet_slots,ItemManager::RoomSlots& item_slots){
    image.save(player_state,special_active,spell_id,player_flags,spell_flags,bomb_animation_flags,
        special_ending,target_locked,target,countdown_seconds,countdown_hundredths,spell_elapsed,
        spell_bonus,shared_spell_state,camera_position,LaserWorld::rate,LaserWorld::player,
        th14_drop_counter,th14_combo,completion.mode,completion.state,transition_timer,
        stage_active,result_animation,fades,enemy_environment,enemy_commands,hud.score,
        hud.lives,hud.digits,hud.communication,hud.indicator,hud.elapsed,hud.previous_seconds,
        hud.displayed_health,hud.target_health,hud.health,hud.frame_animation,hud.boss_name,
        hud.stars,hud.difficulty_intro,hud.difficulty_label,hud.bonus_digits,hud.spell_notice,
        hud.item_notice,hud.spell_time_animation,hud.bonus_marks,hud.th14_overlay,hud.digit_handles,
        hud.icon_handles,hud.coop_icon_handles,hud.th14_boss,hud.show_spell_time,
        popups,spell_timing,enemy_animations.resources,enemy_animations.stage_logo_resource,
        player_input,cancellation,communication,shakes,spell_backgrounds,callback_shared_visual,
        spell_titles,spell_circle,ghosts,life_taps,power_taps,coop_held,coop_input,coop_actor,
        coop_skip,coop_grazer,coop_bomb_invincibility,coop_marisa_b_cooldown,coop_knife_owner,
        th14_dialogue_skip_frames,th14_dialogue_fire_frames,
        th14_spell_background,
        th14_spell_result_requested,th14_current_music,frame,th14_control_clock,th14_control_counted,
        damage_total,player_live,game_over_requested,last_error,error,events,dialogue_text_requests,
        initial_control_activation);
    image.save(spells.timer,spells.initial_bonus,spells.timeout,spells.frame_count,spells.circle_position,
        spells.name,spells.selection,spells.stage,spells.replay,spells.bomb_active,
        spells.run_flags,spells.show_full_capture_history);
    if(dialogue)image.save(dialogue->text_pair_194,dialogue->th14_text_background,dialogue->th14_text_width,
        dialogue->th14_text_layout,dialogue->th14_line_widths,dialogue->th14_enemy_portraits,
        dialogue->state,dialogue->active,dialogue->last_opcode,dialogue->error,dialogue->end);
    for(u32 seat=0;seat<coop_seats;++seat){
        auto& p=pilot(seat);auto& s=p.shots;
        image.save(pilot_economy(seat),p.state,p.input,p.spell,p.last_error,p.th14_scale,
            p.motion.state,p.motion.options,p.motion.body,p.motion.focus_animation,p.motion.last_error);
        image.save(s.player,s.schedule,s.laser_active,s.damage_areas,s.locked_target,s.target_locked,
            s.last_error,s.th14_beam_failure,s.shots,s.converted_laser_spec,s.th14_areas,s.th14_shot_pool,
            s.th14_area_owners,s.th14_area_cursor,s.th14_damage_target);
        if(auto* b=pilot_bomb(seat))b->capture_room_frame(image);
    }
    enemies.capture_room_frame(image);
    bullets.capture_room_frame(image,bullet_slots);
    lasers.capture_room_frame(image);
    items.capture_room_frame(image,item_slots);
    for(auto& [ptr,mesh]:room_deformations)save_deformation(image,*mesh);
    image.save(room_deformations);
    stage->capture_room_frame(image);
    image.after_restore([this]{ascii.clear();ShotWorld::enemies=enemies.first;});
}

bool GameSession::can_predict_room()const{
    return state.phase==GameSessionPhase::stage&&battle_attached&&battle&&battle->stage&&
        battle->stage_active&&!battle->transitioning&&!battle->pending_stage&&!battle->game_over_requested&&
        battle->completion.state.exit==StageExit::None&&(!battle->dialogue||!battle->dialogue->active)&&
        animations.deferred_captures.first_file_index()<0;
}

void GameSession::capture_room_frame(FrameImageTh14& image,BulletManager::RoomSlots& bullets,ItemManager::RoomSlots& items){
    image.save(state,game_input,last_held,last_pause,room_menu,error,shared_loading,spell_records,clear_records,purple_retry_misses,
        scores.settings,scores.characters[state.character*2+state.subtype]);
    image.save(compositor.copies,compositor.full_camera,compositor.play_camera,compositor.world_camera,
        compositor.scaled_play_camera,compositor.region,compositor.clear_color,compositor.last_error);
    // Owner replacement can run finalizers that mark current ANMs. Restore
    // the animation graph last, after the battle and its owners are restored.
    battle->capture_room_frame(image,bullets,items);
    if(image.ready())animations.capture_room_frame(image);
}
}
