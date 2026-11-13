// M6：WorldState 实现——capture/restore/digest（头文件注释即契约）。
#include "WorldState.hpp"
#include "HostState.hpp"
#include "DigestSink.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <sstream>

namespace th13::multiplayer {


} // namespace th13::multiplayer
#include "WorldDigestValues.hpp"
namespace th13::multiplayer {
void DigestSink::vm(const AnmVm& v) {
    digest_value(*this,std::tie(v.shot_effect_owner,v.hide_hit_projectile,v.presentation_motion,v.presentation_part,
        v.variables,v.visual,v.interpolators,v.timer,v.age,v.instruction_offset,v.visible,
        v.pending_interrupt,v.geometry,v.slowdown,v.source_script,v.ordering,v.saved_offset,v.saved_timer));
    tag("anm.bindings");
    boolean(v.resource!=nullptr);
    if(v.resource)u64(v.resource->content_identity());
    boolean(v.sprite_resource!=nullptr);
    if(v.sprite_resource)u64(v.sprite_resource->content_identity());
    // Use the actual bound pointers, not source_script/visual.sprite: those
    // retain the original file-local fields, including fallback selections.
    const auto script_index=v.resource?v.resource->script_index(v.script):(v.script?-2:-1);
    i32(script_index);
    if(script_index==-2 && v.script)u64(anm_script_identity(*v.script));
    const auto* sprite_source=v.sprite_resource?v.sprite_resource:v.resource;
    const auto sprite_index=sprite_source?sprite_source->sprite_index(v.sprite):(v.sprite?-2:-1);
    i32(sprite_index);
    if(sprite_index==-2 && v.sprite) {
        const auto& s=*v.sprite;
        digest_value(*this,std::tie(s.source_id,s.texture,s.x,s.y,s.width,s.height,s.u0,s.v0,s.u1,s.v1));
    }
}

uint64_t WorldState::digest_dialogue(const DialogueText::RollbackState& state) {
    DigestSink d;
    d.tag("host.dialogue");
    digest_value(d, std::tie(state.handles, state.text, state.block, state.cached,
        state.lines, state.box, state.anchor_y, state.anchor_y_set));
    for (const auto& region : state.regions)
        digest_value(d, std::tie(region.x, region.y, region.width, region.height));
    d.boolean(state.texture != 0);
    d.boolean(state.pixel_texture != 0);
    d.boolean(bool(state.pixels));
    if (state.pixels) digest_value(d, *state.pixels);
    return d.value();
}

void digest_timer(DigestSink& d, const char* tag, const Timer& timer) {
    d.tag(tag);
    d.i32(timer.previous); d.i32(timer.current);
    d.f32(timer.fractional);
    d.u32(timer.rate_index); d.u32(timer.flags);
}

void digest_input(DigestSink& d, const char* tag, const GameInput& input) {
    d.tag(tag);
    d.u32(input.held); d.u32(input.previous); d.u32(input.repeated);
    d.u32(input.pressed); d.u32(input.released); d.u32(input.long_held);
    for (u32 value : input.repeat_age) d.u32(value);
    for (u32 value : input.duration) d.u32(value);
}

void digest_player_state(DigestSink& d, const StagePlayerState& state) {
    d.tag("seat");
    d.boolean(state.initialized); d.f32(state.x); d.f32(state.y);
    d.i32(state.fixed_x); d.i32(state.fixed_y);
    d.boolean(state.focus); d.boolean(state.dead); d.boolean(state.ghost);
    d.boolean(state.final_death); d.boolean(state.ghost_just_entered);
    d.u32(state.ghost_random); d.u32(state.ghost_clock);
    d.i32(state.ghost_dx); d.i32(state.ghost_dy);
    d.u8(state.life_transfer_state);
    d.u8(state.power_transfer_state);
    d.i32(static_cast<i32>(state.phase));
    d.i32(state.phase_frame); d.i32(state.invulnerable);
    d.i32(state.phase_frame_previous);
    digest_timer(d, "seat.phase_clock", state.phase_clock);
    digest_timer(d, "seat.fire_clock", state.fire_clock);
    digest_timer(d, "seat.startup_clock", state.startup_clock);
    digest_timer(d, "seat.charge_clock", state.charge_clock);
    d.f32(state.charge_origin.x); d.f32(state.charge_origin.y);
    d.f32(state.charge_origin.z);
    d.u64(state.shots.size());
    for (const PlayerShot& shot : state.shots) {
        d.tag("shot");
        d.f32(shot.x); d.f32(shot.y); d.f32(shot.damage); d.f32(shot.angle);
        d.i32(shot.script); d.boolean(shot.alive); d.boolean(shot.hit_fade);
        d.i32(shot.state); d.boolean(shot.animation_finished);
        d.f32(shot.speed); d.f32(shot.width); d.f32(shot.height);
        d.f32(shot.beam_visual_width);
        d.i32(shot.age); d.i32(shot.behavior); d.i32(shot.type);
        d.i32(shot.anchor); d.i32(shot.anchor_power);
        d.f32(shot.target_angle); d.f32(shot.damage_x); d.f32(shot.damage_y);
        digest_timer(d, "shot.age_clock", shot.age_clock);
        digest_timer(d, "shot.damage_clock", shot.damage_clock);
        d.u32(shot.target_serial); d.u32(shot.generation);
        d.i32(shot.hit_behavior); d.boolean(shot.sine_moving);
        digest_value(d, shot.sine_motion);
        d.i32(shot.registration_slot);
        d.boolean(shot.beam_contact); d.boolean(shot.beam_contact_seen);
        d.i32(shot.beam_interrupt); d.i32(shot.owner_seat);
    }
    d.u64(state.damage_registrations.size());
    for (const DamageRegistration& registration : state.damage_registrations) {
        d.tag("dmg");
        d.boolean(registration.active); d.u32(registration.flags);
        d.i32(static_cast<i32>(registration.owner)); d.i32(registration.orb);
        d.f32(registration.radius); d.f32(registration.radius_growth);
        d.f32(registration.center_x); d.f32(registration.center_y);
        d.f32(registration.center_z);
        d.f32(registration.travel_speed); d.f32(registration.travel_angle);
        d.f32(registration.width); d.f32(registration.height);
        d.f32(registration.angle);
        digest_timer(d, "dmg.clock", registration.clock);
        d.i32(registration.interval); d.f32(registration.damage);
        d.i32(registration.accumulated_damage); d.i32(registration.threshold);
        d.u32(registration.generation); d.i32(registration.owner_seat);
    }
    d.i32(state.damage_registration_cursor);
    d.u64(state.shot_geometry.size());
    for (const PlayerShotGeometrySnapshot& snapshot : state.shot_geometry) {
        d.tag("geom");
        d.boolean(snapshot.valid); d.i32(snapshot.slot); d.i32(snapshot.age);
        d.i32(snapshot.script); d.u32(snapshot.generation);
        for (const Vec3& corner : snapshot.quad) {
            d.f32(corner.x); d.f32(corner.y); d.f32(corner.z);
        }
    }
    d.u32(state.shot_generation);
    d.u64(state.options.size());
    for (const PlayerOption& option : state.options) {
        d.f32(option.x); d.f32(option.y); d.f32(option.angle);
        d.boolean(option.active);
    }
    d.i32(state.option_count);
    for (const auto& fixed : state.option_fixed)
        for (i32 value : fixed) d.i32(value);
    for (const auto& history : state.movement_history)
        for (i32 value : history) d.i32(value);
    d.i32(state.option_power);
    d.boolean(state.option_reset_pending); d.boolean(state.option_movement_tick);
    d.boolean(state.death_option_rebuild);
    d.boolean(state.respawn_damage.active);
    d.f32(state.respawn_damage.center.x); d.f32(state.respawn_damage.center.y);
    d.f32(state.respawn_damage.center.z); d.f32(state.respawn_damage.radius);
    digest_timer(d, "respawn.remaining", state.respawn_damage.remaining);
    d.i32(state.respawn_damage.registration_slot);
    d.f32(state.respawn_origin.x); d.f32(state.respawn_origin.y);
    d.f32(state.respawn_origin.z);
    d.i32(state.bomb_timer); d.i32(state.bomb_protection); d.i32(state.bomb_age);
    d.f32(state.bomb_age_accumulator); d.u32(state.bomb_generation);
    for (const StagePlayerState::BombRay& ray : state.bomb_rays) {
        d.f32(ray.origin.x); d.f32(ray.origin.y); d.f32(ray.origin.z);
        d.f32(ray.center.x); d.f32(ray.center.y); d.f32(ray.center.z);
        d.f32(ray.angle);
    }
    d.f32(state.bomb_angle); d.f32(state.bomb_center_x); d.f32(state.bomb_center_y);
    d.f32(state.bomb_rotation_speed); d.f32(state.bomb_visual_angle);
    d.i32(state.bomb_duration);
    for (const PlayerBombOrb& orb : state.bomb_orbs) {
        d.f32(orb.x); d.f32(orb.y); d.f32(orb.angle);
        d.boolean(orb.active); d.boolean(orb.exploding);
        d.i32(orb.explosion_frame);
        digest_timer(d, "orb.clock", orb.explosion_damage_clock);
        d.f32(orb.radius); d.f32(orb.speed);
        d.f32(orb.velocity_x); d.f32(orb.velocity_y);
        d.f32(orb.accumulated_damage); d.i32(orb.registration_slot);
    }
    d.u32(state.input_state);
    d.i32(state.touch.mode); d.f32(state.touch.x); d.f32(state.touch.y);
    d.i32(state.speed_orth); d.i32(state.speed_diag);
    d.i32(state.speed_orth_focus); d.i32(state.speed_diag_focus);
    for (const auto& mode : state.option_offsets)
        for (const auto& entry : mode)
            for (i32 value : entry) d.i32(value);
    d.boolean(state.sht_shots_loaded);
    for (const auto& block : state.sht_shots) {
        d.u64(block.size());
        for (const ShtShotRecord& record : block) {
            d.i32(record.interval); d.i32(record.phase); d.f32(record.damage);
            d.i32(record.script); d.i32(record.anchor); d.i32(record.sound);
            d.f32(record.x); d.f32(record.y); d.f32(record.angle);
            d.f32(record.speed); d.f32(record.width); d.f32(record.height);
            d.i32(record.type); d.i32(record.behavior);
            d.i32(record.initializer); d.i32(record.hit_behavior);
            d.f32(record.motion_phase);
        }
    }
    d.i32(state.character); d.f32(state.hit_size);
    d.boolean(state.trance_protection);
    state.youmu_bomb.visit_state([&](const auto&... values) {
        digest_value(d, std::tie(values...));
    });
}

void digest_hud_player(DigestSink& d, const HudPlayerState& hud) {
    d.i32(hud.extra_lives); d.i32(hud.life_pieces);
    d.i32(hud.life_piece_threshold_index);
    d.i32(hud.bombs); d.i32(hud.bomb_pieces);
    d.i32(hud.power); d.i32(hud.max_power); d.i32(hud.power_step);
    d.i32(hud.deaths); d.u32(hud.mode_flags);
}

static void digest_registry(DigestSink&, const AnmRegistry::SnapshotData&);

uint64_t WorldState::digest_host(const HostState& state) {
    DigestSink d;
    d.tag("host.application");
    const auto& s = state.state_fields;
    digest_value(d, std::tie(s.frames, s.pending_destination, s.pending_load,
        s.record_edge_reset_pending, s.spell_time_stage, s.spell_clock_active,
        s.spell_clock_index, s.stage_transition_loading, s.stage_transition_active,
        s.stage_startup_age, s.transition_gate_active, s.transition_gate_frames));
    digest_hud_player(d, s.selection_player);
    digest_value(d, s.game_over_music);
    const auto& e = s.loading_environment;
    digest_value(d, std::tie(e.game_rng, e.camera_origin, e.screen_translation,
        e.reference_position, e.background_delta, e.paused, e.screen_shake_offset,
        e.stage_direction, e.stage_difficulty, e.render_target_has_alpha,
        e.playfield_origin));
    d.boolean(e.shared_game_rng != nullptr);
    d.boolean(e.chapter != nullptr);
    if (e.chapter) d.i32(*e.chapter);
    d.boolean(e.fallback_sprite_resource != nullptr);
    if (e.fallback_sprite_resource) d.u64(e.fallback_sprite_resource->content_identity());
    // Resolution, raster scale, screen offsets and physical dimensions belong
    // to each display. Their logical simulation values above remain compared.
    digest_value(d, std::tie(s.loading_animation_rate, s.loading_signature, s.loading_prayer));
    d.boolean(bool(s.loading_registry));
    if (s.loading_registry) digest_registry(d, *s.loading_registry);
    d.boolean(s.has_pause);
    if (s.has_pause) {
        const auto& p = s.pause;
        const auto& m = p.menu;
        digest_value(d, std::tie(p.age, p.selection_age, m.cursor, m.previous, m.count,
            m.history_cursor, m.history_count, m.disabled, m.depth, m.wrapping,
            m.disabled_count, m.error, p.screen, p.previous, p.phase, p.flags,
            p.menu_animation, p.snapshot_animation, p.front_bank, p.practice,
            p.can_continue, p.can_save_replay, p.can_open_options, p.selected_action,
            p.replay_slot, p.name_cursor, p.option_row, p.replay_name, p.name_is_result,
            p.result_needs_name, p.saved_rate, s.pause_frame.input.pressed,
            s.pause_frame.input.repeated, s.pause_frame.input.lost_focus,
            s.pause_frame.enabled));
    }
    d.u64(s.fades.size());
    for (const auto& f : s.fades)
        digest_value(d, std::tie(f.kind, f.duration, f.time, f.alpha, f.playfield_only));
    d.u64(state.dialogue_digest);
    return d.value();
}

std::string WorldState::diagnostic_host_detail(const HostState& state) {
    std::ostringstream out;
    const auto& s = state.state_fields;
    const auto write = [&](const char* name, const auto& value) {
        DigestSink d; digest_value(d, value);
        out << name << '=' << std::hex << d.value() << ' ';
    };
    write("frames", s.frames); write("destination", s.pending_destination);
    write("load", s.pending_load); write("edge", s.record_edge_reset_pending);
    write("spell", std::tie(s.spell_time_stage, s.spell_clock_active, s.spell_clock_index));
    write("transition", std::tie(s.stage_transition_loading, s.stage_transition_active,
        s.stage_startup_age, s.transition_gate_active, s.transition_gate_frames));
    { DigestSink d; digest_hud_player(d, s.selection_player); out << "selection=" << std::hex << d.value() << ' '; }
    write("music", s.game_over_music);
    const auto& e = s.loading_environment;
    write("loading_environment", std::tie(e.game_rng, e.camera_origin, e.screen_translation,
        e.reference_position, e.background_delta, e.paused, e.screen_shake_offset,
        e.stage_direction, e.stage_difficulty, e.render_target_has_alpha, e.playfield_origin));
    write("loading_handles", std::tie(s.loading_signature, s.loading_prayer));
    write("loading_rate", s.loading_animation_rate);
    { DigestSink d; if (s.loading_registry) digest_registry(d, *s.loading_registry);
      out << "loading_registry=" << std::hex << d.value() << ' '; }
    write("pause", std::tie(s.has_pause, s.pause.age, s.pause.selection_age,
        s.pause.screen, s.pause.previous, s.pause.phase, s.pause.flags,
        s.pause.menu_animation, s.pause.snapshot_animation, s.pause.front_bank,
        s.pause.practice, s.pause.can_continue, s.pause.can_save_replay,
        s.pause.can_open_options, s.pause.selected_action, s.pause.replay_slot,
        s.pause.name_cursor, s.pause.option_row, s.pause.replay_name, s.pause.name_is_result,
        s.pause.result_needs_name, s.pause.saved_rate, s.pause_frame.input.pressed,
        s.pause_frame.input.repeated, s.pause_frame.input.lost_focus, s.pause_frame.enabled));
    const auto& m = s.pause.menu;
    write("menu", std::tie(m.cursor, m.previous, m.count, m.history_cursor,
        m.history_count, m.disabled, m.depth, m.wrapping, m.disabled_count, m.error));
    write("dialogue", state.dialogue_digest);
    return out.str();
}

void digest_bullet(DigestSink& d, const StageBullet& bullet) {
        d.i32(bullet.pool_slot);
        d.f32(bullet.x); d.f32(bullet.y); d.f32(bullet.z);
        d.f32(bullet.angle); d.f32(bullet.speed);
        d.f32(bullet.vel[0]); d.f32(bullet.vel[1]); d.f32(bullet.vel[2]);
        d.u32(bullet.sprite); d.u32(bullet.subtype);
        d.f32(bullet.hit_size[0]); d.f32(bullet.hit_size[1]);
        d.u32(bullet.flags); d.f32(bullet.hit_scale);
        d.boolean(bullet.alive); d.boolean(bullet.grazed);
        d.u8(bullet.grazed_seats);
        d.i32(bullet.ex_cursor); d.i32(bullet.native_trigger);
        d.i32(bullet.transform_sound); d.u32(bullet.behavior_flags);
        d.f32(bullet.b98_mirror); d.u32(bullet.state);
        d.u32(bullet.source_enemy); d.u32(bullet.generation);
        d.i32(bullet.delay); d.boolean(bullet.animation_ready);
        d.i32(bullet.animation_interrupt); d.i32(bullet.animation_blend);
        d.i32(bullet.state_age); d.i32(bullet.cancel_protection);
        d.f32(bullet.state_time); d.boolean(bullet.collision_ready);
        d.boolean(bullet.animation_geometry);
        d.f32(bullet.sprite_width); d.f32(bullet.sprite_height);
        d.i32(bullet.cull_grace);
        for (const StageEt::Ex& ex : bullet.ex) {
            d.u32(ex.op); d.u32(ex.flags);
            for (float value : ex.args) d.f32(value);
            for (i32 value : ex.integer_arguments) d.i32(value);
        }
        d.f32(bullet.steer[0]); d.f32(bullet.steer[1]); d.f32(bullet.steer[2]);
        d.f32(bullet.ex4_acceleration); d.i32(bullet.ex4_duration);
        d.i32(bullet.ex4_timer); d.f32(bullet.ex4_time);
        d.i32(bullet.ex1_timer); d.f32(bullet.ex1_time);
        d.f32(bullet.ex8_speed_delta); d.f32(bullet.ex8_angle_delta);
        d.i32(bullet.ex8_duration); d.i32(bullet.ex8_timer); d.f32(bullet.ex8_time);
        d.f32(bullet.ex10_speed); d.f32(bullet.ex10_angle);
        d.i32(bullet.ex10_duration); d.i32(bullet.ex10_repeats);
        d.i32(bullet.ex10_count); d.i32(bullet.ex10_mode);
        d.i32(bullet.ex10_timer); d.f32(bullet.ex10_time);
        d.f32(bullet.ex40_speed); d.f32(bullet.ex40_w); d.f32(bullet.ex40_h);
        d.i32(bullet.ex40_bounces); d.i32(bullet.ex40_limit); d.u32(bullet.ex40_mode);
        d.i32(bullet.offscreen_timer); d.f32(bullet.offscreen_time);
        d.i32(bullet.offscreen_directional);
        d.i32(bullet.ex1000_count); d.i32(bullet.ex1000_limit);
        d.u32(bullet.ex1000_mask);
        d.i32(bullet.wait_timer); d.f32(bullet.wait_time);
        d.f32(bullet.saved_speed); d.f32(bullet.saved_angle);
        d.f32(bullet.saved_pos[0]); d.f32(bullet.saved_pos[1]);
        d.f32(bullet.saved_pos[2]);
        d.i32(bullet.target_speed_time); d.i32(bullet.target_speed_duration);
        d.f32(bullet.target_speed_fraction); d.f32(bullet.target_speed_step);
        for (float value : bullet.target_speed_velocity) d.f32(value);
}

// P3.2：激光全值域 digest（缺口 3——此前仅 u64(size)，三处 sink 同补）。
void digest_laser(DigestSink& d, const StageLaser& laser) {
    d.tag("laser");
    d.i32(i32(laser.kind)); d.i32(laser.id); d.u32(laser.generation);
    for (float value : laser.origin) d.f32(value);
    for (float value : laser.velocity) d.f32(value);
    d.f32(laser.angle); d.f32(laser.rotation);
    d.f32(laser.length); d.f32(laser.target_length); d.f32(laser.length_speed);
    d.f32(laser.width); d.f32(laser.target_width); d.f32(laser.start_offset);
    for (i32 value : laser.timing) d.i32(value);
    d.i32(laser.time); d.i32(laser.previous_time); d.f32(laser.time_fraction);
    d.i32(laser.phase);
    d.u32(laser.flags); d.u32(laser.sprite); d.u32(laser.subtype);
    d.u32(laser.beam_descriptor_flags);
    d.i32(laser.sound); d.i32(laser.transform_sound);
    for (const StageEt::Ex& ex : laser.ex) {
        d.u32(ex.op); d.u32(ex.flags);
        for (float value : ex.args) d.f32(value);
        for (i32 value : ex.integer_arguments) d.i32(value);
    }
    d.i32(laser.ex_cursor); d.i32(laser.cancel_protection);
    d.u32(laser.behavior_flags); d.u32(laser.anm_flags);
    d.boolean(laser.active);
    d.boolean(laser.pending_remove); d.boolean(laser.has_updated);
    d.f32(laser.line_limit);
    d.f32(laser.line_descriptor_angle); d.f32(laser.line_initial_length);
    // 曲线激光路径段 + 采样点（vector——先尺寸后逐点，稳定序）。
    d.u64(laser.curve_path.segments.size());
    for (const CurveLaserSegment& segment : laser.curve_path.segments) {
        d.f32(segment.start_time); d.f32(segment.end_time); d.i32(segment.mode);
        d.f32(segment.direction.x); d.f32(segment.direction.y);
        d.f32(segment.direction.z);
        d.f32(segment.origin.x); d.f32(segment.origin.y); d.f32(segment.origin.z);
        d.f32(segment.angle); d.f32(segment.speed);
        d.f32(segment.speed_delta); d.f32(segment.angle_delta);
    }
    d.u64(laser.curve_samples.size());
    for (const CurveLaserSample& sample : laser.curve_samples) {
        d.f32(sample.position.x); d.f32(sample.position.y);
        d.f32(sample.position.z);
        d.f32(sample.delta.x); d.f32(sample.delta.y); d.f32(sample.delta.z);
        d.f32(sample.angle); d.f32(sample.speed);
    }
    d.i32(laser.acceleration_time); d.i32(laser.acceleration_duration);
    d.f32(laser.acceleration_fraction);
    d.f32(laser.acceleration);
    for (float value : laser.acceleration_velocity) d.f32(value);
    d.i32(laser.wait_remaining); d.f32(laser.wait_fraction);
    d.i32(laser.cull_timer); d.i32(laser.graze_timer);
}

void digest_stage_enemy(DigestSink& d, const StageEnemy& enemy,
                        bool include_animation_ids = true) {
    d.tag("enemy");
    d.i32(enemy.native_function); d.i32(enemy.native_function_mode);
    d.u32(enemy.serial); d.i32(enemy.script_id); d.i32(enemy.trace_id);
    d.u64(enemy.routine.size());
    for (char value : enemy.routine) d.u8(static_cast<uint8_t>(value));
    d.f32(enemy.life); d.i32(enemy.spawn_score);
    d.f32(enemy.bullet_protect_radius2);
    d.f32(enemy.vel.pos[0]); d.f32(enemy.vel.pos[1]); d.f32(enemy.vel.pos[2]);
    d.f32(enemy.vel.speed); d.f32(enemy.vel.angle);
    d.f32(enemy.vel.radius); d.f32(enemy.vel.growth);
    d.f32(enemy.vel.ellipse_angle); d.f32(enemy.vel.ellipse_ratio);
    d.f32(enemy.vel.origin[0]); d.f32(enemy.vel.origin[1]);
    d.f32(enemy.vel.origin[2]); d.f32(enemy.vel.sine_phase);
    d.f32(enemy.vel.aux[0]); d.f32(enemy.vel.aux[1]); d.f32(enemy.vel.aux[2]);
    d.u32(enemy.vel.mode);
    d.f32(enemy.pos.pos[0]); d.f32(enemy.pos.pos[1]); d.f32(enemy.pos.pos[2]);
    d.f32(enemy.pos.speed); d.f32(enemy.pos.angle);
    d.f32(enemy.pos.radius); d.f32(enemy.pos.growth);
    d.f32(enemy.pos.ellipse_angle); d.f32(enemy.pos.ellipse_ratio);
    d.f32(enemy.pos.origin[0]); d.f32(enemy.pos.origin[1]);
    d.f32(enemy.pos.origin[2]); d.f32(enemy.pos.sine_phase);
    d.f32(enemy.pos.aux[0]); d.f32(enemy.pos.aux[1]); d.f32(enemy.pos.aux[2]);
    d.u32(enemy.pos.mode);
    // P3.2：mover 嵌套插值器（缺口 3——polar/radial/postime 此前全漏，
    // 活动插值中的敌机在回滚重放后会按旧端点继续插值而 digest 不报）。
    for (const StageMover* mover : {&enemy.vel, &enemy.pos}) {
        d.f32(mover->polar.from_a); d.f32(mover->polar.from_b);
        d.f32(mover->polar.to_a); d.f32(mover->polar.to_b);
        d.f32(mover->polar.duration); d.f32(mover->polar.elapsed);
        d.i32(mover->polar.mode);
        for (float value : mover->radial.start) d.f32(value);
        for (float value : mover->radial.end) d.f32(value);
        for (float value : mover->radial.control1) d.f32(value);
        for (float value : mover->radial.control2) d.f32(value);
        for (float value : mover->radial.current) d.f32(value);
        digest_timer(d, "mover.radial", mover->radial.timer);
        d.i32(mover->radial.duration); d.i32(mover->radial.mode);
        for (float value : mover->postime.start) d.f32(value);
        for (float value : mover->postime.target) d.f32(value);
        for (float value : mover->postime.p1) d.f32(value);
        for (float value : mover->postime.p2) d.f32(value);
        d.f32(mover->postime.duration); d.f32(mover->postime.elapsed);
        d.i32(mover->postime.mode); d.boolean(mover->postime.per_axis);
        for (i32 value : mover->postime.axis_mode) d.i32(value);
    }
    d.f32(enemy.position[0]); d.f32(enemy.position[1]); d.f32(enemy.position[2]);
    d.f32(enemy.last_hit_position[0]); d.f32(enemy.last_hit_position[1]);
    d.f32(enemy.motion_delta[0]); d.f32(enemy.motion_delta[1]);
    d.f32(enemy.motion_delta[2]);
    d.i32(enemy.death_sound); d.i32(enemy.death_effect);
    d.i32(enemy.death_effect_file);
    d.f32(enemy.age); d.u32(enemy.flags);
    d.f32(enemy.hurtbox[0]); d.f32(enemy.hurtbox[1]);
    d.f32(enemy.hitbox[0]); d.f32(enemy.hitbox[1]);
    d.f32(enemy.body_angle);
    d.f32(enemy.limit_center[0]); d.f32(enemy.limit_center[1]);
    d.f32(enemy.limit_half[0]); d.f32(enemy.limit_half[1]);
    for (const StageEt& et : enemy.et) {
        d.f32(et.spawn[0]); d.f32(et.spawn[1]); d.f32(et.spawn[2]);
        d.f32(et.laser_width);
        d.i32(et.sound); d.i32(et.transform_sound);
        for (i32 value : et.laser_timing) d.i32(value);
        d.f32(et.angle1); d.f32(et.angle2);
        d.f32(et.speed1); d.f32(et.speed2); d.f32(et.spin);
        d.i32(et.count); d.i32(et.ways); d.i32(et.aim);
        d.f32(et.offset[0]); d.f32(et.offset[1]); d.f32(et.offset[2]);
        d.f32(et.absolute[0]); d.f32(et.absolute[1]); d.f32(et.absolute[2]);
        for (const StageEt::Ex& ex : et.ex) {
            d.u32(ex.op); d.u32(ex.flags);
            for (float value : ex.args) d.f32(value);
            for (i32 value : ex.integer_arguments) d.i32(value);
        }
        d.u32(et.ex_next);
    }
    for (i32 value : enemy.i_slots) d.i32(value);
    for (float value : enemy.f_slots) d.f32(value);
    d.boolean(enemy.dead); d.boolean(enemy.tick_gate);
    d.u64(enemy.interrupt_label.size());
    for (char value : enemy.interrupt_label) d.u8(static_cast<uint8_t>(value));
    for (const EclContext::Interrupt& interrupt : enemy.interrupts) {
        d.i32(interrupt.life); d.i32(interrupt.timeout);
        d.u64(interrupt.health_label.size());
        for (char value : interrupt.health_label) d.u8(static_cast<uint8_t>(value));
        d.u64(interrupt.timeout_label.size());
        for (char value : interrupt.timeout_label) d.u8(static_cast<uint8_t>(value));
    }
    d.i32(enemy.invulnerable);
    digest_timer(d, "enemy.invuln_clock", enemy.invulnerable_clock);
    d.i32(enemy.body_invulnerable);
    digest_timer(d, "enemy.body_clock", enemy.body_invulnerable_clock);
    d.i32(enemy.damage_function); d.i32(enemy.pending_damage);
    d.i32(enemy.mp_boss_damage_remainder);
    d.boolean(enemy.spell_active);
    d.i32(enemy.spell_base_life); d.i32(enemy.spell_life); d.i32(enemy.spell_id);
    d.u64(enemy.spell_name.size());
    for (unsigned char value : enemy.spell_name) d.u8(value);
    d.i32(enemy.phase_life_remaining); d.i32(enemy.phase_life_threshold);
    d.boolean(enemy.phase_timed_out);
    d.i32(enemy.drop_main);
    for (i32 value : enemy.drop_extra) d.i32(value);
    d.f32(enemy.drop_area[0]); d.f32(enemy.drop_area[1]);
    d.f32(enemy.spirit_accum); d.i32(enemy.spirit_threshold);
    d.i32(enemy.hit_anim_timer); d.i32(enemy.hit_feedback_cooldown);
    d.i32(enemy.hit_sound); d.boolean(enemy.periodic_flash);
    d.i32(enemy.death_spirit_limit); d.i32(enemy.death_spirit_count);
    d.boolean(enemy.damage_spirits_disabled);
    d.u64(enemy.death_routine.size());
    for (char value : enemy.death_routine) d.u8(static_cast<uint8_t>(value));
    d.i32(enemy.anm_file); d.i32(enemy.anm_base_script);
    d.i32(enemy.anm_slot0_bank); d.i32(enemy.anm_slot0_script_offset);
    for (i32 value : enemy.anm_script) d.i32(value);
    for (u32 value : enemy.anm_slot_handle) {
        // Title and loading animations can allocate different local IDs before
        // the same combat starts. Keep slot occupancy in the wire checksum.
        d.u32(include_animation_ids ? value : u32(value != 0));
    }
    for (const Vec3& offset : enemy.anm_slot_offset) {
        d.f32(offset.x); d.f32(offset.y); d.f32(offset.z);
    }
    d.f32(enemy.anm_size_a[0]); d.f32(enemy.anm_size_a[1]);
    d.f32(enemy.anm_size_b[0]); d.f32(enemy.anm_size_b[1]);
    d.i32(enemy.mirror_dir); d.f32(enemy.mirror_velocity_x);
    d.boolean(enemy.mirror_ready); d.f32(enemy.mirror_prev_x);
    d.i32(enemy.mirror_script_base);
}

void digest_enemy_fog(DigestSink& d, const EnemySnapshot& enemy) {
    d.boolean(enemy.has_fog);
    if (!enemy.has_fog) return;
    const auto& fog = enemy.fog;
    digest_value(d, std::tie(fog.target, fog.radius, fog.angle_x, fog.angle_y, fog.color, fog.vertices));
}

void digest_ecl_threads(DigestSink& d, const EclThreadsSnapshot& threads) {
    const auto targets = [&](const EclContext& context) {
        d.tag("ecl.interpolation_targets");
        for (const EclInstruction* target : context.interpolation_targets) {
            if (!target) { d.u8(0); continue; }
            bool found = false;
            if (context.program) {
                const auto pointer = reinterpret_cast<std::uintptr_t>(target);
                for (u32 routine = 0; routine < context.program->definitions.size(); ++routine) {
                    const auto& definition = context.program->definitions[routine];
                    const auto& sub = context.program->subroutine(routine);
                    if (sub.size < 32) continue;
                    const auto* code = definition.file->bytes.data() + sub.offset + 16;
                    const auto base = reinterpret_cast<std::uintptr_t>(code);
                    if (pointer < base || pointer - base > sub.size - 32) continue;
                    const u32 offset = u32(pointer - base);
                    if (offset % 4) break;
                    u32 cursor = 0;
                    while (cursor < offset) {
                        const auto* instruction = reinterpret_cast<const EclInstruction*>(code + cursor);
                        if (instruction->length < 16 || instruction->length % 4 ||
                            instruction->length > sub.size - 16 - cursor) break;
                        cursor += instruction->length;
                    }
                    if (cursor == offset) {
                        d.u8(1); d.u32(routine); d.u32(offset);
                        found = true;
                    }
                    break;
                }
            }
            // Non-null foreign or non-boundary pointers have no legal program
            // identity. Distinguish them from null without reading that storage.
            if (!found) d.u8(2);
        }
    };
    d.tag("ecl.main");
    const EclContext& main = threads.main;
    d.f32(main.time); d.i32(main.routine); d.i32(main.instruction_offset);
    d.u64(main.stack.size());
    for (u8 value : main.stack) d.u8(value);
    d.i32(main.stack_pointer); d.i32(main.local_base);
    d.u8(main.difficulty);
    d.i32(main.thread_id); d.i32(main.thread_state); d.u32(main.thread_flags);
    for (const EclInterpolation& channel : main.interpolators) {
        d.i32(channel.duration); d.i32(channel.mode);
        d.f32(channel.start); d.f32(channel.end);
        d.f32(channel.control1); d.f32(channel.control2); d.f32(channel.current);
        digest_timer(d, "ecl.itp", channel.timer);
    }
    targets(main);
    d.u64(threads.children.size());
    for (const EclContext& child : threads.children) {
        d.tag("ecl.child");
        d.f32(child.time); d.i32(child.routine); d.i32(child.instruction_offset);
        d.u64(child.stack.size());
        for (u8 value : child.stack) d.u8(value);
        d.i32(child.stack_pointer); d.i32(child.local_base);
        d.u8(child.difficulty);
        d.i32(child.thread_id); d.i32(child.thread_state);
        d.u32(child.thread_flags);
        for (const EclInterpolation& channel : child.interpolators) {
            d.i32(channel.duration); d.i32(channel.mode);
            d.f32(channel.start); d.f32(channel.end);
            d.f32(channel.control1); d.f32(channel.control2);
            d.f32(channel.current);
            digest_timer(d, "ecl.itp", channel.timer);
        }
        targets(child);
    }
}

bool WorldState::capture(StageBattle& battle, GameRandom& visual, const ReplayView* replay,
                         StagePresentation* presentation, RunScene* scene,
                         u32 transition_handle) {
    ready_ = false;
    StageSim& sim = battle.sim;
    // ---- StageBattle 玩家面 ----
    battle_.seat_count = battle.seat_count;
    battle_.active_seat = battle.active_seat;
    if (battle.seat_count < 2) {
        battle_.single_player = battle.player_state_;
        battle_.single_hud = battle.player_ ? *battle.player_ : HudPlayerState{};
    }
    battle_.seats = battle.seats;
    battle_.seat_huds = battle.seat_huds;
    battle_.seat_characters = battle.seat_characters;
    battle_.seat_inputs = battle.seat_inputs;
    battle_.trance_mode = battle.trance_mode;
    battle_.trance_owner_seat = battle.trance_owner_seat;
    battle_.trance_generation = battle.trance_generation;
    battle_.death_trance_generation = battle.death_trance_generation;
    battle_.trance_task_frame = battle.trance_task_frame_;
    // ---- StageSim ----
    sim_.frame = sim.frame;
    // P4：resize 复用槽内 EnemySnapshot（fields 的 string 与 threads 的
    // vector 容量跨帧保留；capture_fields 的拷贝赋值复用既有 string 容量）。
    sim_.enemies.resize(sim.enemies.size());
    for (size_t index = 0; index < sim.enemies.size(); ++index) {
        EnemySnapshot& snapshot = sim_.enemies[index];
        const auto& enemy = *sim.enemies[index];
        enemy.capture_fields(snapshot.fields);
        if (enemy.threads) snapshot.threads = enemy.threads->snapshot();
        if (enemy.fog) { snapshot.has_fog = true; snapshot.fog = *enemy.fog; }
        else snapshot.has_fog = false;
    }
    // P4：assign 复用既有容量（稳态零堆分配；值语义与整拷赋值等价）。
    sim_.bullets.assign(sim.bullets.begin(), sim.bullets.end());
    sim_.lasers.assign(sim.lasers.begin(), sim.lasers.end());
    sim_.gvars_writable_ints.assign(sim.gvars.writable_ints_.begin(), sim.gvars.writable_ints_.end());
    sim_.shared_integers = sim.shared_integers;
    sim_.shared_floats = sim.shared_floats;
    for (int slot = 0; slot < 3; ++slot) sim_.boss_slot[slot] = sim.boss_slot[slot];
    sim_.boss_timer_seconds = sim.boss_timer_seconds;
    sim_.boss_timer_centiseconds = sim.boss_timer_centiseconds;
    sim_.boss_leaving = sim.boss_leaving;
    for (int slot = 0; slot < 3; ++slot)
        for (int marker = 0; marker < 4; ++marker)
            sim_.boss_markers[slot][marker] = sim.boss_markers[size_t(slot)][size_t(marker)];
    sim_.boss_remaining_stars = sim.boss_remaining_stars;
    sim_.boss_nonspell_frame = sim.boss_nonspell_frame;
    sim_.boss_main_serial = sim.boss_main_serial;
    sim_.last_created_enemy_id = sim.last_created_enemy_id;
    sim_.laser_serial = sim.laser_serial;
    sim_.next_serial = sim.mp_next_serial();
    sim_.next_bullet_generation = sim.mp_next_bullet_generation();
    sim_.player_x = sim.player_x;
    sim_.player_y = sim.player_y;
    sim_.seat_count = sim.seat_count;
    sim_.seat_x = sim.seat_x;
    sim_.seat_y = sim.seat_y;
    sim_.seat_eligible = sim.seat_eligible;
    sim_.bullet_bounce_size[0] = sim.bullet_bounce_size[0];
    sim_.bullet_bounce_size[1] = sim.bullet_bounce_size[1];
    sim_.active_bullet_protect_radius2 = sim.active_bullet_protect_radius2;
    sim_.mode_flags = sim.mode_flags;
    sim_.difficulty_rank = sim.difficulty_rank;
    sim_.diff = sim.diff;
    sim_.rank = sim.rank;
    sim_.spawned = sim.spawned_;
    sim_.bullet_free_slots.assign(sim.mp_bullet_free_slots().begin(), sim.mp_bullet_free_slots().end());
    sim_.anm_effects.assign(sim.anm_effects.begin(), sim.anm_effects.end());
    sim_.bomb_active = sim.bomb_active;
    sim_.spell = sim.spell.state();
    // ---- ItemManager ----
    items_.items = battle.item_manager.items();
    items_.spirits = battle.item_manager.spirits();
    items_.ordinary_free.assign(battle.item_manager.ordinary_free_.begin(), battle.item_manager.ordinary_free_.end());
    items_.cancel_free.assign(battle.item_manager.cancel_free_.begin(), battle.item_manager.cancel_free_.end());
    items_.events.assign(battle.item_manager.events().begin(), battle.item_manager.events().end());
    items_.last_player = battle.item_manager.last_player_;
    items_.spirit_serial = battle.item_manager.spirit_serial_;
    items_.spirit_overflow = battle.item_manager.spirit_overflow_;
    items_.spirit_window = battle.item_manager.spirit_window_;
    items_.spirit_window_clock = battle.item_manager.spirit_window_clock_;
    items_.cancel_total = battle.item_manager.cancel_total_;
    items_.cancel_wave = battle.item_manager.cancel_wave_;
    items_.point_items = battle.item_manager.point_items_;
    // ---- 会话/分数 ----
    if (sim.session) progress_ = *sim.session;
    else progress_ = SessionState{};
    score_ = battle.score_ ? *battle.score_ : HudScoreState{};
    // ---- 双 RNG ----
    game_rng_.seed = sim.random.seed();
    game_rng_.index = sim.random.index();
    visual_rng_.seed = visual.seed();
    visual_rng_.index = visual.index();
    // ---- 表现层手账（生产路径：registry 快照同批次的句柄映射 + 自有
    // VM 值。无头测试无 presentation → 空手账，restore 跳过）----
    has_present_ = presentation != nullptr;
    if (presentation) {
        if (!presentation->capture_presentation(present_)) return false;
    } else {
        present_ = StagePresentation::SnapshotData{};
    }
    // Capture the owner identities first: draw-only item VMs have no registry
    // node, but their semantic slot and restore binding belong to presentation.
    if (battle.mp_registry()) {
        if (!battle.mp_registry()->capture(anm_, present_.embedded_identities)) return false;
    } else {
        anm_ = AnmRegistry::SnapshotData{};
    }
    // ---- P3.2 宿主域（RunScene：震动/对话/STD；转场句柄由调用方供给）。
    // 无头测试不传 scene = 该组域为空，restore 跳过（与 presentation 同门）。
    has_scene_ = scene != nullptr;
    if (scene) {
        scene_state_ = scene->capture_rollback();
        scene->capture_shakes(shakes_);
        const auto dialog = scene->capture_dialog();
        dialog_ = dialog.player;
        dialog_advance_ = dialog.advance;
        dialog_ready_ = dialog.ready;
        if (!scene->capture_scenery(scenery_extra_, scenery_script_, has_scenery_))
            return false;
        has_previous_scenery_ = scene->previous_scenery != nullptr;
        previous_scenery_bank_ = scene->previous_scenery_bank;
        if (has_previous_scenery_) {
            previous_scenery_script_ = scene->previous_scenery->script.state;
            previous_scenery_extra_ = scene->previous_scenery->capture_extra();
        }
        transition_handle_ = transition_handle;
    } else {
        scene_state_ = RunScene::RollbackState{};
        shakes_.clear();
        dialog_ = MsgPlayer::Snapshot{};
        dialog_advance_ = MsgAdvance{};
        dialog_ready_ = false;
        has_scenery_ = false;
        scenery_script_ = StageScriptState{};
        scenery_extra_ = StageScenery::ExtraSnapshot{};
        has_previous_scenery_ = false;
        previous_scenery_bank_ = -1;
        previous_scenery_script_ = StageScriptState{};
        previous_scenery_extra_ = StageScenery::ExtraSnapshot{};
        transition_handle_ = 0;
    }
    // ---- 宿主回放面（回放会话输入游标；live run 不传 = 不快照）----
    has_replay_ = replay != nullptr;
    if (replay) replay_ = *replay;
    ready_ = true;
    return true;
}

bool WorldState::restore(StageBattle& battle, GameRandom& visual, ReplayView* replay,
                         StagePresentation* presentation, RunScene* scene,
                         u32* transition_handle_out) {
    if (!ready_) return false;
    StageSim& sim = battle.sim;
    // Embedded VMs are recreated by presentation restore. Rebind registry
    // child chains to those new objects instead of retaining old addresses.
    std::unordered_map<const AnmVm*, AnmVm*> remapped;
    if (presentation && has_present_ &&
        !presentation->restore_presentation(present_, &remapped)) return false;
    if (scene && has_scene_) scene->restore_rollback(scene_state_, remapped);
    if (battle.mp_registry() && !battle.mp_registry()->restore(anm_, remapped)) return false;
    // ---- P3.2 宿主域恢复（敌机池/RNG 已恢复之后：震动回调的 run 引用
    // scene.battle.sim.random，battle 必须先就位；重建经 start_shake 生产
    // 路径保持注册序。capture 无 scene 时跳过（无头测试路径）。----
    if (scene && has_scene_) {
        if (!scene->restore_shakes(shakes_)) return false;
        scene->restore_dialog({dialog_, dialog_advance_, dialog_ready_});
        if (!scene->restore_scenery(scenery_extra_, scenery_script_, has_scenery_))
            return false;
        if (has_previous_scenery_ != (scene->previous_scenery != nullptr) ||
            previous_scenery_bank_ != scene->previous_scenery_bank) return false;
        if (has_previous_scenery_) {
            scene->previous_scenery->script.state = previous_scenery_script_;
            scene->previous_scenery->restore_extra(previous_scenery_extra_);
        }
        if (transition_handle_out) *transition_handle_out = transition_handle_;
    }
    // ---- StageSim 敌机池（数量对齐 → 逐只值拷贝 + 线程恢复）----
    while (sim.enemies.size() > sim_.enemies.size()) sim.enemies.pop_back();
    while (sim.enemies.size() < sim_.enemies.size()) {
        // 新敌机对象：threads 由 snapshot 恢复时重建（EclThreads::restore
        // 校验 program 指针一致——同一 sim 的 program 不变）。
        sim.enemies.push_back(std::make_unique<StageEnemy>());
    }
    for (size_t i = 0; i < sim_.enemies.size(); ++i) {
        StageEnemy& enemy = *sim.enemies[i];
        const EnemySnapshot& snapshot = sim_.enemies[i];
        // 值域替换（restore_fields 保留 threads/fog 对象身份）。
        enemy.restore_fields(snapshot.fields);
        if (!enemy.threads) {
            // 新建线程组必须复刻生产接线（StageSim 敌机生成处）：
            // EclThreads 构造只设 program/variables/random，commands 由
            // 宿主补接；EclThreads::restore 的 rebind 以目标对象自身的
            // commands 指针为准——不补接则游戏指令（弹幕 spawn 等）静默
            // 失效，restore 后第一帧权威态即分歧。
            enemy.threads = std::make_unique<EclThreads>(
                sim.program, &sim.gvars, &sim.random);
            enemy.threads->main.commands = &sim.commands;
        }
        if (!enemy.threads->restore(snapshot.threads)) return false;
        if (snapshot.has_fog) {
            if (!enemy.fog) enemy.fog = std::make_unique<EnemyFog>();
            *enemy.fog = snapshot.fog;
        } else {
            enemy.fog.reset();
        }
    }
    sim.frame = sim_.frame;
    sim.bullets = sim_.bullets;
    sim.lasers = sim_.lasers;
    sim.gvars.writable_ints_ = sim_.gvars_writable_ints;
    sim.shared_integers = sim_.shared_integers;
    sim.shared_floats = sim_.shared_floats;
    for (int slot = 0; slot < 3; ++slot) sim.boss_slot[slot] = sim_.boss_slot[slot];
    sim.boss_timer_seconds = sim_.boss_timer_seconds;
    sim.boss_timer_centiseconds = sim_.boss_timer_centiseconds;
    sim.boss_leaving = sim_.boss_leaving;
    for (int slot = 0; slot < 3; ++slot)
        for (int marker = 0; marker < 4; ++marker)
            sim.boss_markers[size_t(slot)][size_t(marker)] = sim_.boss_markers[slot][marker];
    sim.boss_remaining_stars = sim_.boss_remaining_stars;
    sim.boss_nonspell_frame = sim_.boss_nonspell_frame;
    sim.boss_main_serial = sim_.boss_main_serial;
    sim.last_created_enemy_id = sim_.last_created_enemy_id;
    sim.laser_serial = sim_.laser_serial;
    sim.mp_next_serial() = sim_.next_serial;
    sim.mp_next_bullet_generation() = sim_.next_bullet_generation;
    sim.player_x = sim_.player_x;
    sim.player_y = sim_.player_y;
    sim.seat_count = sim_.seat_count;
    sim.seat_x = sim_.seat_x;
    sim.seat_y = sim_.seat_y;
    sim.seat_eligible = sim_.seat_eligible;
    sim.bullet_bounce_size[0] = sim_.bullet_bounce_size[0];
    sim.bullet_bounce_size[1] = sim_.bullet_bounce_size[1];
    sim.active_bullet_protect_radius2 = sim_.active_bullet_protect_radius2;
    sim.mode_flags = sim_.mode_flags;
    sim.difficulty_rank = sim_.difficulty_rank;
    sim.diff = sim_.diff;
    sim.rank = sim_.rank;
    sim.spawned_ = sim_.spawned;
    sim.mp_bullet_free_slots() = sim_.bullet_free_slots;
    sim.anm_effects = sim_.anm_effects;
    sim.bomb_active = sim_.bomb_active;
    sim.spell.restore(sim_.spell);
    // ---- ItemManager ----
    battle.item_manager.items() = items_.items;
    battle.item_manager.spirits() = items_.spirits;
    battle.item_manager.ordinary_free_ = items_.ordinary_free;
    battle.item_manager.cancel_free_ = items_.cancel_free;
    battle.item_manager.events() = items_.events;
    battle.item_manager.last_player_ = items_.last_player;
    battle.item_manager.spirit_serial_ = items_.spirit_serial;
    battle.item_manager.spirit_overflow_ = items_.spirit_overflow;
    battle.item_manager.spirit_window_ = items_.spirit_window;
    battle.item_manager.spirit_window_clock_ = items_.spirit_window_clock;
    battle.item_manager.cancel_total_ = items_.cancel_total;
    battle.item_manager.cancel_wave_ = items_.cancel_wave;
    battle.item_manager.point_items_ = items_.point_items;
    // ---- 会话/分数 ----
    if (sim.session) *sim.session = progress_;
    if (battle.score_) *battle.score_ = score_;
    // ---- 双 RNG ----
    sim.random.reseed(game_rng_.seed);
    sim.random.set_index(game_rng_.index);
    visual.reseed(visual_rng_.seed);
    visual.set_index(visual_rng_.index);
    // ---- StageBattle 玩家面 ----
    battle.seat_count = battle_.seat_count;
    battle.active_seat = battle_.active_seat;
    battle.seats = battle_.seats;
    battle.seat_huds = battle_.seat_huds;
    battle.seat_characters = battle_.seat_characters;
    battle.seat_inputs = battle_.seat_inputs;
    battle.trance_mode = battle_.trance_mode;
    battle.trance_owner_seat = battle_.trance_owner_seat;
    battle.trance_generation = battle_.trance_generation;
    battle.death_trance_generation = battle_.death_trance_generation;
    battle.trance_task_frame_ = battle_.trance_task_frame;
    // M6 复核修复：restore 与下一帧 step_mp 的 copy-in 之间，宿主可能读
    // 活动席窗口（end_trance 的 owner 定向死亡读 player_x/bomb_timer 等
    // 发散期旧值）。多席时用权威槽位刷新窗口，消除读窗；单席窗口即
    // 权威（enable_seats 前不触 seats），保持不动。
    if (battle.seat_count >= 2) {
        battle.player_state_ = battle.seats[size_t(battle.active_seat)];
        battle.player_ = &battle.seat_huds[size_t(battle.active_seat)];
        battle.character_ = battle.seat_characters[size_t(battle.active_seat)];
    }
    else {
        battle.player_state_ = battle_.single_player;
        if (battle.player_) *battle.player_ = battle_.single_hud;
    }
    // ---- 宿主回放面（capture 时带游标的会话才恢复——live run 的
    // replay_consumed/cursor 不在快照边界内）----
    if (replay && has_replay_) *replay = replay_;
    // HUD pointers and draw opacity are views of the restored world. A render
    // may follow restoration before any new update, including a paused frame.
    if (scene && has_scene_) scene->sync_mp_hud_view();
    return true;
}

void WorldState::discard() noexcept {
    ready_ = false;
    battle_ = BattleData{};
    sim_ = SimData{};
    items_ = ItemData{};
    progress_ = SessionState{};
    score_ = HudScoreState{};
    game_rng_ = RngData{};
    visual_rng_ = RngData{};
    anm_ = AnmRegistry::SnapshotData{};
    present_ = StagePresentation::SnapshotData{};
    has_present_ = false;
    replay_ = ReplayView{};
    has_replay_ = false;
}

const char* WorldState::field_ledger() {
    // M6 交付台账：快照覆盖的权威域（测试断言逐条引用；新权威域落地时
    // 必须同步登记——ledger 即快照完整性契约）。
    return "battle.seats|battle.seat_huds|battle.seat_characters|"
           "battle.seats.bomb_protection|battle.seats.final_death|"
           "battle.seats.ghost_drift|battle.seats.ghost_just_entered|"
           "battle.seat_inputs|battle.trance_mode|battle.trance_owner_seat|"
           "battle.trance_generation|battle.trance_task_frame|"
           "battle.death_trance_generation|"
           "sim.frame|sim.enemies(+threads+fog)|sim.bullets|sim.lasers|"
           "sim.gvars_writable|sim.shared_integers|sim.shared_floats|"
           "sim.boss_slots|sim.boss_timers|sim.boss_markers|sim.serials|"
           "sim.next_bullet_generation|"
           "sim.seat_targets|sim.bullet_pool|sim.anm_effects|sim.spell|"
           "items.pool|items.spirits|items.free_lists|items.events|"
           "items.counters|session.state|score.view|"
           "rng.game|rng.visual|anm.registry|"
           // P3.2 宿主域（scene 非空时有效；transition 句柄经接线传入）。
           "scene.shakes|scene.dialog|scene.scenery|scene.transition";
}

size_t WorldState::retained_bytes() const noexcept {
    size_t total = sizeof(*this);
    total += sim_.enemies.size() * sizeof(EnemySnapshot);
    total += sim_.bullets.size() * sizeof(StageBullet);
    total += sim_.lasers.size() * sizeof(StageLaser);
    total += sim_.bullet_free_slots.size() * sizeof(i32);
    total += sim_.anm_effects.size() * sizeof(AnmEffectRequest);
    total += anm_.entries.size() * sizeof(AnmRegistry::SnapshotData::Entry);
    total += anm_.free_slots.size() * sizeof(u32);
    for (const auto& layer : anm_.layers)
        total += layer.size() * sizeof(AnmRegistry::SnapshotData::DrawReference);
    return total;
}

std::string WorldState::diagnostic_detail() const {
    std::ostringstream out;
    const auto write=[&](const char* name,const auto& value) {
        DigestSink d;digest_value(d,value);
        out << name << '=' << std::hex << d.value() << ' ';
    };
    size_t index=0;
    std::apply([&](const auto&... fields) {
        (write(("presentation."+std::to_string(index++)).c_str(),fields),...);
    },state_fields(present_));
    index=0;
    std::apply([&](const auto&... fields) {
        (write(("scene."+std::to_string(index++)).c_str(),fields),...);
    },state_fields(scene_state_));
    return out.str();
}


namespace {
struct ItemFixedStream {
    uint64_t factor = 1;
    std::array<uint64_t, 256> adjustment{};
};
// F_S(h) = h * P^|S| + C_S[h & 255]. XOR only changes the low byte;
// multiplication makes the remaining high 56 input bits a linear contribution.
const std::array<ItemFixedStream, 12>& default_item_streams() {
    static const auto streams = [] {
        std::array<ItemFixedStream, 12> result{};
        std::array<uint8_t, 69> bytes{};
        // The unchanged Item serializer has 17 little-endian 32-bit fields
        // followed by alive. Only target, tracking and anm_frame_prev are -1.
        for (size_t word : {size_t(3), size_t(4), size_t(14)})
            for (size_t byte = 0; byte < 4; ++byte) bytes[word * 4 + byte] = 255;
        auto& first = result[0];
        for (uint8_t byte : bytes) { (void)byte; first.factor *= 0x100000001b3ull; }
        for (size_t low = 0; low < 256; ++low) {
            uint64_t h = low;
            for (uint8_t byte : bytes) h = (h ^ byte) * 0x100000001b3ull;
            first.adjustment[low] = h - uint64_t(low) * first.factor;
        }
        for (size_t power = 1; power < result.size(); ++power) {
            const auto& old = result[power - 1];
            auto& next = result[power];
            next.factor = old.factor * old.factor;
            for (size_t low = 0; low < 256; ++low) {
                const auto after = uint8_t(uint64_t(low) * old.factor + old.adjustment[low]);
                next.adjustment[low] = old.adjustment[low] * old.factor + old.adjustment[after];
            }
        }
        return result;
    }();
    return streams;
}
bool is_default_item(const Item& item) {
    auto float_bits = [](float value) { uint32_t bits = 0; std::memcpy(&bits, &value, sizeof bits); return bits; };
    return item.state == 0 && item.type == 0 && item.type2 == 0 && item.target_seat == -1 && item.tracking_seat == -1 &&
        float_bits(item.x) == 0 && float_bits(item.y) == 0 && float_bits(item.z) == 0 &&
        float_bits(item.vx) == 0 && float_bits(item.vy) == 0 && float_bits(item.vz) == 0 &&
        float_bits(item.homing_speed) == 0 && item.sprite == 0 && item.anm_frame == 0 &&
        item.anm_frame_prev == -1 && float_bits(item.anm_time) == 0 && item.flags == 0 && !item.alive;
}
void append_default_items(DigestSink& d, size_t count) {
    const auto& streams = default_item_streams();
    for (size_t power = 0; count; ++power, count >>= 1)
        if (count & 1) d.fixed_stream(streams[power].factor, streams[power].adjustment);
}
template<size_t N> void digest_items_exact(DigestSink& d, const std::array<Item, N>& items) {
    static_assert(N < (size_t(1) << 12));
    size_t defaults = 0;
    for (const Item& item : items) {
        if (is_default_item(item)) { ++defaults; continue; }
        if (defaults) { append_default_items(d, defaults); defaults = 0; }

            d.i32(item.state); d.i32(item.type); d.i32(item.type2);
            d.i32(item.target_seat);
            d.i32(item.tracking_seat);
            d.f32(item.x); d.f32(item.y); d.f32(item.z);
            d.f32(item.vx); d.f32(item.vy); d.f32(item.vz);
            d.f32(item.homing_speed); d.i32(item.sprite);
            d.i32(item.anm_frame); d.i32(item.anm_frame_prev);
            d.f32(item.anm_time); d.u32(item.flags); d.boolean(item.alive);

    }
    if (defaults) append_default_items(d, defaults);
}
} // namespace

uint64_t WorldState::digest_core() const {
    DigestSink d;
    digest_core_sink(d, false);
    return d.value();
}

uint64_t WorldState::digest() const {
    DigestSink d;
    digest_core_sink(d, true);
    digest_anm_sink(d);
    digest_value(d, has_present_);
    if (has_present_) digest_value(d, present_);
    digest_scene_sink(d, true);
    return d.value();
}
// 分域 digest（诊断面）：core 各域独立摘要（首差域定位）。
void WorldState::digest_domains(char* buffer, size_t capacity) const {
    bool first = true;
    auto write = [&](const char* name, uint64_t value) {
        if (!buffer || capacity == 0) return;
        const int written = std::snprintf(buffer, capacity, "%s%s=%016llx",
                                          first ? "" : " ", name,
                                          static_cast<unsigned long long>(value));
        if (written < 0 || size_t(written) >= capacity) {
            capacity = 0;
            return;
        }
        first = false;
        buffer += written;
        capacity -= size_t(written);
    };
    {
        DigestSink d; d.tag("battle");
        d.i32(battle_.seat_count); d.i32(battle_.active_seat);
        if (battle_.seat_count < 2) {
            digest_player_state(d, battle_.single_player);
            digest_hud_player(d, battle_.single_hud);
        }
        for (const StagePlayerState& seat : battle_.seats) digest_player_state(d, seat);
        for (const HudPlayerState& hud : battle_.seat_huds) digest_hud_player(d, hud);
        for (i32 value : battle_.seat_characters) d.i32(value);
        for (const GameInput& input : battle_.seat_inputs)
            digest_input(d, "seat_input", input);
        d.i32(static_cast<i32>(battle_.trance_mode));
        d.i32(battle_.trance_owner_seat); d.u32(battle_.trance_generation);
        d.u32(battle_.death_trance_generation);
        d.i32(battle_.trance_task_frame);
        write("battle", d.value());
    }
    for (size_t seat_index = 0; seat_index < battle_.seats.size(); ++seat_index) {
        DigestSink d; d.tag("seat");
        digest_player_state(d, battle_.seats[seat_index]);
        char name[16];
        std::snprintf(name, sizeof name, "seat%zu", seat_index);
        write(name, d.value());
    }
    {
        DigestSink d; d.tag("hud");
        for (const HudPlayerState& hud : battle_.seat_huds) digest_hud_player(d, hud);
        write("hud", d.value());
    }
    {
        DigestSink d; d.tag("trance");
        d.i32(static_cast<i32>(battle_.trance_mode));
        d.i32(battle_.trance_owner_seat); d.u32(battle_.trance_generation);
        d.i32(battle_.trance_task_frame);
        write("trance", d.value());
    }
    {
        DigestSink d; d.tag("bmeta");
        d.i32(battle_.seat_count); d.i32(battle_.active_seat);
        for (i32 value : battle_.seat_characters) d.i32(value);
        write("bmeta", d.value());
    }
    {
        DigestSink d; d.tag("binputs");
        for (const GameInput& input : battle_.seat_inputs)
            digest_input(d, "seat_input", input);
        write("binputs", d.value());
    }
    {
        DigestSink d; d.tag("sim");
        d.u32(sim_.frame);
        d.u64(sim_.enemies.size());
        for (const EnemySnapshot& enemy : sim_.enemies) {
            digest_stage_enemy(d, enemy.fields);
            digest_enemy_fog(d, enemy);
            digest_ecl_threads(d, enemy.threads);
        }
        d.u64(sim_.bullets.size());
        for (const StageBullet& bullet : sim_.bullets) { d.tag("bullet"); digest_bullet(d, bullet); }
        d.u64(sim_.lasers.size());
        for (const StageLaser& laser : sim_.lasers) digest_laser(d, laser);
        d.u64(sim_.gvars_writable_ints.size());
        for (const auto& [id, value] : sim_.gvars_writable_ints) { d.i32(id); d.i32(value); }
        for (i32 value : sim_.shared_integers) d.i32(value);
        for (float value : sim_.shared_floats) d.f32(value);
        for (int slot = 0; slot < 3; ++slot) d.i32(sim_.boss_slot[slot]);
        d.i32(sim_.boss_timer_seconds); d.i32(sim_.boss_timer_centiseconds);
        d.boolean(sim_.boss_leaving);
        for (int slot = 0; slot < 3; ++slot)
            for (int marker = 0; marker < 4; ++marker) {
                d.f32(sim_.boss_markers[slot][marker].ratio);
                d.i32(sim_.boss_markers[slot][marker].color);
            }
        d.i32(sim_.boss_remaining_stars); d.i32(sim_.boss_nonspell_frame);
        d.u32(sim_.boss_main_serial); d.i32(sim_.last_created_enemy_id);
        d.u32(sim_.laser_serial);
    d.u32(sim_.next_serial);
        d.f32(sim_.player_x); d.f32(sim_.player_y);
        d.i32(sim_.seat_count);
        for (float value : sim_.seat_x) d.f32(value);
        for (float value : sim_.seat_y) d.f32(value);
        for (bool value : sim_.seat_eligible) d.boolean(value);
        d.f32(sim_.bullet_bounce_size[0]); d.f32(sim_.bullet_bounce_size[1]);
        d.f32(sim_.active_bullet_protect_radius2);
        d.u32(sim_.mode_flags); d.u32(sim_.difficulty_rank);
        d.i32(sim_.diff); d.i32(sim_.rank); d.u32(sim_.spawned);
        d.u64(sim_.bullet_free_slots.size());
        for (i32 value : sim_.bullet_free_slots) d.i32(value);
        d.u64(sim_.anm_effects.size());
        for (const AnmEffectRequest& request : sim_.anm_effects) {
            d.i32(request.file); d.i32(request.script);
            d.f32(request.x); d.f32(request.y); d.f32(request.z);
            d.u8(request.kind); d.u32(request.opcode);
            d.i32(request.slot); d.u32(request.serial);
        }
        d.boolean(sim_.bomb_active);
        d.i32(sim_.spell.id); d.u32(sim_.spell.flags);
        d.i32(sim_.spell.current_bonus); d.i32(sim_.spell.initial_bonus);
        d.i32(sim_.spell.duration_frames); d.i32(sim_.spell.active_frames);
        digest_timer(d, "spell.age", sim_.spell.age);
        d.u64(sim_.spell.name.size());
        for (unsigned char value : sim_.spell.name) d.u8(value);
        write("sim", d.value());
    }
    {
        DigestSink d; d.tag("simenemies");
        for (const EnemySnapshot& enemy : sim_.enemies) {
            digest_stage_enemy(d, enemy.fields);
            digest_enemy_fog(d, enemy);
            digest_ecl_threads(d, enemy.threads);
        }
        write("simenemies", d.value());
    }
    {
        DigestSink d; d.tag("enemyfields");
        d.u64(sim_.enemies.size());
        for (const EnemySnapshot& enemy : sim_.enemies) {
            digest_stage_enemy(d, enemy.fields); digest_enemy_fog(d, enemy);
        }
        write("enemyfields", d.value());
    }
    {
        DigestSink values, handles, feedback;
        for (const EnemySnapshot& enemy : sim_.enemies) {
            const auto& fields = enemy.fields;
            for (u32 handle : fields.anm_slot_handle) handles.u32(handle);
            digest_stage_enemy(values, fields, false);
            digest_enemy_fog(values, enemy);
            feedback.i32(fields.hit_anim_timer);
            feedback.i32(fields.hit_feedback_cooldown);
            feedback.boolean(fields.periodic_flash);
        }
        write("enemyvalues", values.value());
        write("enemyhandles", handles.value());
        write("enemyfeedback", feedback.value());
    }
    {
        DigestSink d; d.tag("enemythreads");
        for (const EnemySnapshot& enemy : sim_.enemies) digest_ecl_threads(d, enemy.threads);
        write("enemythreads", d.value());
    }
    {
        DigestSink d; d.tag("simbullets");
        d.u64(sim_.bullets.size());
        for (const StageBullet& bullet : sim_.bullets) { d.tag("bullet"); digest_bullet(d, bullet); }
        write("simbullets", d.value());
    }
    {
        DigestSink d; d.tag("simmisc");
        d.u32(sim_.frame);
        d.u64(sim_.lasers.size());
        for (const StageLaser& laser : sim_.lasers) digest_laser(d, laser);
        d.u64(sim_.gvars_writable_ints.size());
        for (const auto& [id, value] : sim_.gvars_writable_ints) { d.i32(id); d.i32(value); }
        for (i32 value : sim_.shared_integers) d.i32(value);
        for (float value : sim_.shared_floats) d.f32(value);
        for (int slot = 0; slot < 3; ++slot) d.i32(sim_.boss_slot[slot]);
        d.i32(sim_.boss_timer_seconds); d.i32(sim_.boss_timer_centiseconds);
        d.boolean(sim_.boss_leaving);
        d.i32(sim_.boss_remaining_stars); d.i32(sim_.boss_nonspell_frame);
        d.u32(sim_.boss_main_serial); d.i32(sim_.last_created_enemy_id);
        d.u32(sim_.laser_serial);
    d.u32(sim_.next_serial);
        d.f32(sim_.player_x); d.f32(sim_.player_y);
        d.i32(sim_.seat_count);
        for (float value : sim_.seat_x) d.f32(value);
        for (float value : sim_.seat_y) d.f32(value);
        for (bool value : sim_.seat_eligible) d.boolean(value);
        d.f32(sim_.bullet_bounce_size[0]); d.f32(sim_.bullet_bounce_size[1]);
        d.f32(sim_.active_bullet_protect_radius2);
        d.u32(sim_.mode_flags); d.u32(sim_.difficulty_rank);
        d.i32(sim_.diff); d.i32(sim_.rank); d.u32(sim_.spawned);
        d.u64(sim_.bullet_free_slots.size());
        for (i32 value : sim_.bullet_free_slots) d.i32(value);
        d.u64(sim_.anm_effects.size());
        for (const AnmEffectRequest& request : sim_.anm_effects) {
            d.i32(request.file); d.i32(request.script);
            d.f32(request.x); d.f32(request.y); d.f32(request.z);
            d.u8(request.kind); d.u32(request.opcode);
            d.i32(request.slot); d.u32(request.serial);
        }
        d.boolean(sim_.bomb_active);
        d.i32(sim_.spell.id); d.u32(sim_.spell.flags);
        d.i32(sim_.spell.current_bonus); d.i32(sim_.spell.initial_bonus);
        d.i32(sim_.spell.duration_frames); d.i32(sim_.spell.active_frames);
        digest_timer(d, "spell.age", sim_.spell.age);
        d.u64(sim_.spell.name.size());
        for (unsigned char value : sim_.spell.name) d.u8(value);
        write("simmisc", d.value());
    }
    {
        DigestSink d; d.tag("items");
        digest_items_exact(d, items_.items);
        for (const Spirit& spirit : items_.spirits) {
            d.i32(spirit.state); d.i32(spirit.kind); d.u32(spirit.serial);
            d.i32(spirit.tracking_seat);
            d.f32(spirit.speed); d.f32(spirit.move_speed);
            d.f32(spirit.x); d.f32(spirit.y); d.f32(spirit.z);
            d.f32(spirit.angle); d.i32(spirit.anm_frame);
            d.i32(spirit.anm_frame_prev); d.f32(spirit.anm_time);
            d.i32(spirit.frame);
            digest_timer(d, "spirit.clock", spirit.lifetime_clock);
            d.u32(spirit.flags); d.boolean(spirit.fade_requested);
            d.boolean(spirit.alive);
        }
        d.u64(items_.ordinary_free.size());
        for (i32 value : items_.ordinary_free) d.i32(value);
        d.u64(items_.cancel_free.size());
        for (i32 value : items_.cancel_free) d.i32(value);
        d.u64(items_.events.size());
        for (const ItemEvent& event : items_.events) {
            d.i32(static_cast<i32>(event.kind)); d.i32(event.value);
            d.f32(event.x); d.f32(event.y); d.u32(event.color);
        }
        d.f32(items_.last_player.x); d.f32(items_.last_player.y);
        d.i32(items_.last_player.state); d.boolean(items_.last_player.focus);
        d.boolean(items_.last_player.dialog); d.f32(items_.last_player.homing_speed);
        for (float value : items_.last_player.box) d.f32(value);
        for (float value : items_.last_player.box_auto) d.f32(value);
        for (float value : items_.last_player.box_auto_focus) d.f32(value);
        d.u32(items_.spirit_serial); d.i32(items_.spirit_overflow);
        d.i32(items_.spirit_window);
        digest_timer(d, "items.window_clock", items_.spirit_window_clock);
        d.i32(items_.cancel_total); d.i32(items_.cancel_wave);
        d.i32(items_.point_items);
        write("items", d.value());
    }
    {
        DigestSink d; d.tag("progress");
        d.i32(progress_.stage); d.i32(progress_.stage_frame);
        d.i32(progress_.character); d.i32(progress_.difficulty);
        d.i32(progress_.continues); d.i32(progress_.continue_stock);
        d.i32(progress_.high_score); d.i32(progress_.high_score_extra);
        d.i32(progress_.configured_lives); d.i32(progress_.starting_stage);
        d.i32(progress_.chapter); d.i32(progress_.chapter_frame);
        d.f32(progress_.rate); d.u32(progress_.scene_flags);
        d.boolean(progress_.new_run); d.u32(progress_.run_mode_flags);
        d.i32(progress_.practice_mode); d.i32(progress_.selected_spell_id);
        d.i32(progress_.ending_points); d.i32(progress_.good_endings);
        d.i32(progress_.stage_clear_bonus); d.u32(progress_.stage_cleared_bits);
        d.i32(progress_.point_items); d.i32(progress_.max_point);
        d.i32(progress_.max_point_cap); d.i32(progress_.graze);
        d.i32(progress_.life_piece_threshold_index);
        write("progress", d.value());
    }
    {
        DigestSink d; d.tag("score");
        d.i32(score_.score); d.i32(score_.life_piece_tier);
        write("score", d.value());
    }
    {
        DigestSink d; d.tag("rng");
        d.u32(game_rng_.seed); d.u32(game_rng_.index);
        d.u32(visual_rng_.seed); d.u32(visual_rng_.index);
        write("rng", d.value());
    }
    { DigestSink d; digest_scene_sink(d, false); write("scene", d.value()); }
    { DigestSink d; digest_scene_sink(d, true); write("scene_full", d.value()); }
    { DigestSink d; digest_value(d, has_present_); if (has_present_) digest_value(d, present_); write("presentation", d.value()); }
}

void WorldState::digest_scene_sink(DigestSink& d, bool full) const {
    d.tag("scene");
    digest_value(d, has_scene_);
    if (!has_scene_) return;
    digest_value(d, shakes_); digest_value(d, dialog_);
    digest_value(d, dialog_advance_); digest_value(d, dialog_ready_);
    digest_value(d, has_scenery_);
    if (has_scenery_) digest_value(d, scenery_script_);
    digest_value(d, has_previous_scenery_);
    digest_value(d, previous_scenery_bank_);
    if (has_previous_scenery_) digest_value(d, previous_scenery_script_);
    digest_value(d, transition_handle_);
    const auto& s = scene_state_;
    digest_value(d, std::tie(s.post,s.age,s.player_activation_pending,s.transition_wait,s.scenery_started,
        s.team_wipe_frames,s.team_wipe_game_over,
        s.hud.flags,s.hud.intro_age,s.hud.frame_age,s.hud.clear_age,s.hud.displayed_score,s.hud.score_increment,
        s.camera_origin,s.screen_translation,s.reference_position,s.background_delta,s.stage_direction,
        s.shake_offset,s.environment_rng,s.animation_rate,s.paused,s.dialogue_rows));
    if (full) {
        digest_value(d, s);
        if (has_scenery_) digest_value(d, scenery_extra_);
        if (has_previous_scenery_) digest_value(d, previous_scenery_extra_);
    }
}

void WorldState::digest_core_sink(DigestSink& d, bool include_animation_ids) const {
    d.tag("worldstate");
    d.i32(battle_.seat_count); d.i32(battle_.active_seat);
    if (battle_.seat_count < 2) {
        digest_player_state(d, battle_.single_player);
        digest_hud_player(d, battle_.single_hud);
    }
    for (const StagePlayerState& seat : battle_.seats) digest_player_state(d, seat);
    for (const HudPlayerState& hud : battle_.seat_huds) digest_hud_player(d, hud);
    for (i32 value : battle_.seat_characters) d.i32(value);
    for (const GameInput& input : battle_.seat_inputs)
        digest_input(d, "seat_input", input);
    d.i32(static_cast<i32>(battle_.trance_mode));
    d.i32(battle_.trance_owner_seat); d.u32(battle_.trance_generation);
    d.u32(battle_.death_trance_generation);
    d.i32(battle_.trance_task_frame);
    d.u32(sim_.frame);
    d.u32(sim_.next_bullet_generation);
    d.u64(sim_.enemies.size());
    for (const EnemySnapshot& enemy : sim_.enemies) {
        digest_stage_enemy(d, enemy.fields, include_animation_ids);
        digest_enemy_fog(d, enemy);
        digest_ecl_threads(d, enemy.threads);
    }
    d.u64(sim_.bullets.size());
    for (const StageBullet& bullet : sim_.bullets) {
        digest_bullet(d, bullet);
    }
    d.u64(sim_.lasers.size());
    for (const StageLaser& laser : sim_.lasers) digest_laser(d, laser);
    d.u64(sim_.gvars_writable_ints.size());
    for (const auto& [id, value] : sim_.gvars_writable_ints) {
        d.i32(id); d.i32(value);
    }
    for (i32 value : sim_.shared_integers) d.i32(value);
    for (float value : sim_.shared_floats) d.f32(value);
    for (int slot = 0; slot < 3; ++slot) d.i32(sim_.boss_slot[slot]);
    d.i32(sim_.boss_timer_seconds); d.i32(sim_.boss_timer_centiseconds);
    d.boolean(sim_.boss_leaving);
    for (int slot = 0; slot < 3; ++slot)
        for (int marker = 0; marker < 4; ++marker) {
            d.f32(sim_.boss_markers[slot][marker].ratio);
            d.i32(sim_.boss_markers[slot][marker].color);
        }
    d.i32(sim_.boss_remaining_stars); d.i32(sim_.boss_nonspell_frame);
    d.u32(sim_.boss_main_serial); d.i32(sim_.last_created_enemy_id);
    d.u32(sim_.laser_serial);
    d.u32(sim_.next_serial);
    d.f32(sim_.player_x); d.f32(sim_.player_y);
    d.i32(sim_.seat_count);
    for (float value : sim_.seat_x) d.f32(value);
    for (float value : sim_.seat_y) d.f32(value);
    for (bool value : sim_.seat_eligible) d.boolean(value);
    d.f32(sim_.bullet_bounce_size[0]); d.f32(sim_.bullet_bounce_size[1]);
    d.f32(sim_.active_bullet_protect_radius2);
    d.u32(sim_.mode_flags); d.u32(sim_.difficulty_rank);
    d.i32(sim_.diff); d.i32(sim_.rank); d.u32(sim_.spawned);
    d.u64(sim_.bullet_free_slots.size());
    for (i32 value : sim_.bullet_free_slots) d.i32(value);
    d.u64(sim_.anm_effects.size());
    for (const AnmEffectRequest& request : sim_.anm_effects) {
        d.i32(request.file); d.i32(request.script);
        d.f32(request.x); d.f32(request.y); d.f32(request.z);
        d.u8(request.kind); d.u32(request.opcode);
        d.i32(request.slot); d.u32(request.serial);
    }
    d.boolean(sim_.bomb_active);
    d.i32(sim_.spell.id); d.u32(sim_.spell.flags);
    d.i32(sim_.spell.current_bonus); d.i32(sim_.spell.initial_bonus);
    d.i32(sim_.spell.duration_frames); d.i32(sim_.spell.active_frames);
    digest_timer(d, "spell.age", sim_.spell.age);
    d.u64(sim_.spell.name.size());
    for (unsigned char value : sim_.spell.name) d.u8(value);
    // items 池摘要：只 digest 活跃槽（2648 全量按位也行——但池语义是
    // state!=0 即活；空槽是常量默认值，等价全量）。
    digest_items_exact(d, items_.items);
    for (const Spirit& spirit : items_.spirits) {
        d.i32(spirit.state); d.i32(spirit.kind); d.u32(spirit.serial);
        d.i32(spirit.tracking_seat);
        d.f32(spirit.speed); d.f32(spirit.move_speed);
        d.f32(spirit.x); d.f32(spirit.y); d.f32(spirit.z);
        d.f32(spirit.angle); d.i32(spirit.anm_frame);
        d.i32(spirit.anm_frame_prev); d.f32(spirit.anm_time);
        d.i32(spirit.frame);
        digest_timer(d, "spirit.clock", spirit.lifetime_clock);
        d.u32(spirit.flags); d.boolean(spirit.fade_requested);
        d.boolean(spirit.alive);
    }
    d.u64(items_.ordinary_free.size());
    for (i32 value : items_.ordinary_free) d.i32(value);
    d.u64(items_.cancel_free.size());
    for (i32 value : items_.cancel_free) d.i32(value);
    d.u64(items_.events.size());
    for (const ItemEvent& event : items_.events) {
        d.i32(static_cast<i32>(event.kind)); d.i32(event.value);
        d.f32(event.x); d.f32(event.y); d.u32(event.color);
    }
    d.f32(items_.last_player.x); d.f32(items_.last_player.y);
    d.i32(items_.last_player.state); d.boolean(items_.last_player.focus);
    d.boolean(items_.last_player.dialog); d.f32(items_.last_player.homing_speed);
    for (float value : items_.last_player.box) d.f32(value);
    for (float value : items_.last_player.box_auto) d.f32(value);
    for (float value : items_.last_player.box_auto_focus) d.f32(value);
    d.u32(items_.spirit_serial); d.i32(items_.spirit_overflow);
    d.i32(items_.spirit_window);
    digest_timer(d, "items.window_clock", items_.spirit_window_clock);
    d.i32(items_.cancel_total); d.i32(items_.cancel_wave);
    d.i32(items_.point_items);
    // session/score。
    d.i32(progress_.stage); d.i32(progress_.stage_frame);
    d.i32(progress_.character); d.i32(progress_.difficulty);
    d.i32(progress_.continues); d.i32(progress_.continue_stock);
    d.i32(progress_.high_score); d.i32(progress_.high_score_extra);
    d.i32(progress_.configured_lives); d.i32(progress_.starting_stage);
    d.i32(progress_.chapter); d.i32(progress_.chapter_frame);
    d.f32(progress_.rate); d.u32(progress_.scene_flags);
    d.boolean(progress_.new_run); d.u32(progress_.run_mode_flags);
    d.i32(progress_.practice_mode); d.i32(progress_.selected_spell_id);
    d.i32(progress_.ending_points); d.i32(progress_.good_endings);
    d.i32(progress_.stage_clear_bonus); d.u32(progress_.stage_cleared_bits);
    d.i32(progress_.point_items); d.i32(progress_.max_point);
    d.i32(progress_.max_point_cap); d.i32(progress_.graze);
    d.i32(progress_.life_piece_threshold_index);
    d.i32(score_.score); d.i32(score_.life_piece_tier);
    // 双 RNG。
    d.u32(game_rng_.seed); d.u32(game_rng_.index);
    d.u32(visual_rng_.seed); d.u32(visual_rng_.index);
    digest_scene_sink(d, false);
}

// ANM registry：序号化摘要（不 hash 指针——VM 值 + 链关系 + 句柄）。
// digest_core_sink 尾接调用（全量 digest 专用——表现层 VM 池面）。
void WorldState::digest_anm_sink(DigestSink& d) const {
    digest_registry(d, anm_);
}

static void digest_registry(DigestSink& d, const AnmRegistry::SnapshotData& anm_) {
    d.u32(anm_.generation); d.u32(anm_.active_count);
    d.u64(anm_.next_serialization_ordinal);
    d.u64(anm_.entries.size());
    for (const AnmRegistry::SnapshotData::Entry& entry : anm_.entries) {
        d.tag("anm");
        d.u32(entry.slot); d.u32(entry.handle);
        d.u64(entry.serialization_ordinal);
        d.boolean(entry.active); d.boolean(entry.registered);
        d.boolean(entry.alternate); d.boolean(entry.borrowed);
        d.boolean(entry.frozen_root); d.boolean(entry.queued_deletion);
        d.u32(entry.parent); d.u32(entry.creation_parent); d.u32(entry.rotation_parent);
        d.u64(entry.children.size());
        for (u32 child : entry.children) d.u32(child);
        d.u32(entry.chain_index[0]); d.u32(entry.chain_index[1]);
        d.vm(entry.vm);
    }
    d.u64(anm_.free_slots.size());
    for (u32 value : anm_.free_slots) d.u32(value);
    for (const auto& layer : anm_.layers) {
        d.u64(layer.size());
        for (const auto& reference : layer) {
            d.u32(reference.entry); d.u32(reference.embedded);
        }
    }
}

}  // namespace th13::multiplayer
