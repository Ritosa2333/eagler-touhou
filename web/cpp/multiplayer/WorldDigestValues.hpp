// Stable value serialization for rollback state. Resource and callback
// identities are fixed within a confirmed scene epoch; pointers never hash.
#pragma once
#include "../game/AnmOverlay.hpp"
#include <tuple>
#include <type_traits>
#include <map>
#include <optional>

namespace th13::multiplayer {
inline auto state_fields(const Timer& v) { return std::tie(v.previous, v.current, v.fractional, v.rate_index, v.flags); }
inline auto state_fields(const Vec2& v) { return std::tie(v.x, v.y); }
inline auto state_fields(const Vec3& v) { return std::tie(v.x, v.y, v.z); }
inline auto state_fields(const Matrix4& v) { return std::tie(v.m); }
inline auto state_fields(const AnmVariables& v) { return std::tie(v.integers, v.floats, v.extra_integers, v.random_scale, v.random_angle, v.random_bound, v.position, v.rotation); }
inline auto state_fields(const AnmVisualState& v) { return std::tie(v.flags, v.render_flags, v.layer, v.sprite, v.child_anchor, v.angular_velocity, v.scale, v.secondary_scale, v.scale_velocity, v.uv_scale, v.uv_offset, v.uv_velocity, v.size, v.color, v.secondary_color, v.uv, v.sprite_size, v.sprite_matrix, v.sprite_frame, v.translation, v.transform_matrix, v.uv_matrix, v.inherited_color); }
inline auto state_fields(const AnmInterpolators& v) { return std::tie(v.position, v.color, v.alpha, v.rotation, v.angle, v.scale, v.secondary_scale, v.uv_scale, v.secondary_color, v.secondary_alpha, v.uv_x, v.uv_y); }
inline auto state_fields(const AnmGeometryVertex& v) { return std::tie(v.position, v.reciprocal_w, v.color, v.uv); }
inline auto state_fields(const AnmWorldVertex& v) { return std::tie(v.position, v.color, v.uv); }
inline auto state_fields(const AnmDistortion& v) { return std::tie(v.vertices, v.radius, v.radial_speed, v.uv_speed); }
inline auto state_fields(const AnmTrail& v) { return std::tie(v.points, v.colors, v.angle, v.age, v.segment_count); }
inline auto state_fields(const AnmCircle& v) { return std::tie(v.vertices, v.center, v.radius, v.angle, v.color, v.rim_color, v.age); }
inline auto state_fields(const AnmGatherEffect& v) { return std::tie(v.handles, v.points, v.tangents, v.phase, v.centers, v.reserved, v.age); }
inline auto state_fields(const AnmRibbon& v) { return std::tie(v.segments, v.vertices, v.angle, v.age); }
inline auto state_fields(const AnmWindTrail& v) { return std::tie(v.vertices, v.points, v.tangents, v.start, v.end, v.start_tangent, v.end_tangent, v.drift, v.interpolation, v.age, v.duration, v.color); }
inline auto state_fields(const AnmOverlay& v) { return std::tie(v.panels, v.mode, v.frames); }
inline auto state_fields(const AnmGeometry& v) { return std::tie(v.screen_vertices, v.world_vertices, v.distortion, v.allocation_bytes, v.trail, v.circle, v.gather, v.ribbon, v.wind, v.overlay); }
inline auto state_fields(const SpellSnapshot& v) { return std::tie(v.id, v.name, v.flags, v.current_bonus, v.initial_bonus, v.duration_frames, v.age, v.active_frames); }
inline auto state_fields(const MsgLine& v) { return std::tie(v.text_cp932, v.text_utf8, v.has_ruby, v.ruby_x, v.ruby_spacing, v.ruby_cp932, v.ruby_utf8, v.bubble_width, v.block, v.side, v.text_attr); }
inline auto state_fields(const MsgDialogState& v) { return std::tie(v.active, v.done, v.entry, v.side, v.box_x, v.box_y, v.auto_advance, v.text_attr, v.skip_request, v.bubble_width, v.block, v.lines, v.lines_meta); }
inline auto state_fields(const MsgPlayer::Snapshot& v) { return std::tie(v.cursor_offset, v.state, v.clock, v.line_index, v.block_open, v.wait_timeout, v.command_delay, v.pending_ruby, v.pending_ruby_x, v.pending_ruby_spacing, v.pending_ruby_cp932, v.pending_ruby_utf8); }
inline auto state_fields(const MsgAdvance& v) { return std::tie(v.z_pressed, v.held, v.z_frames, v.ctrl_frames, v.pressed); }
inline auto state_fields(const ScreenShake& v) { return std::tie(v.duration, v.start, v.end, v.envelope, v.amplitude, v.rise, v.hold, v.fall, v.age); }
inline auto state_fields(const StageFog& v) { return std::tie(v.near_distance, v.far_distance, v.channels, v.color); }
inline auto state_fields(const StageFogInterpolation& v) { return std::tie(v.start, v.end, v.control1, v.control2, v.current, v.timer, v.duration, v.mode); }
inline auto state_fields(const StageCamera& v) { return std::tie(v.position, v.direction, v.up, v.eye_offset, v.target_offset, v.animation_delta, v.fov, v.fog); }
inline auto state_fields(const StageBackgroundAnm& v) { return std::tie(v.script, v.visible); }
inline auto state_fields(const StageScriptState& v) { return std::tie(v.timer, v.instruction_offset, v.camera_effect, v.effect_timer, v.direction, v.position, v.up, v.fog, v.camera, v.culling_distance_squared, v.background); }
inline auto state_fields(const StageScenery::ExtraSnapshot& v) { return std::tie(v.primitives, v.background_slots, v.object_flags, v.instance_flags, v.lifecycle_flags, v.lifecycle_timer, v.fade_in_pending, v.transition_color, v.rate, v.view_direction, v.frames); }
inline auto state_fields(const ScorePopups::Entry& v) { return std::tie(v.digits, v.count, v.x, v.y, v.z, v.color, v.scale, v.age, v.active); }
inline auto state_fields(const ScorePopups::RollbackState& v) { return std::tie(v.glyph, v.pool, v.cursor, v.available); }
inline auto state_fields(const LaserPresentation::SnapshotData::Entry& v) { return std::tie(v.generation, v.type, v.color, v.curve, v.body, v.orb, v.tip); }
inline auto state_fields(const LaserPresentation::SnapshotData& v) { return std::tie(v.entries); }
inline auto state_fields(const RunScene::RollbackState& v) { return std::tie(v.post, v.hud, v.spell, v.portraits, v.popups, v.lasers, v.camera_origin, v.screen_translation, v.reference_position, v.background_delta, v.stage_direction, v.shake_offset, v.environment_rng, v.animation_rate, v.paused, v.dialogue_rows, v.age, v.player_activation_pending, v.transition_wait, v.scenery_started, v.team_wipe_frames, v.team_wipe_game_over); }
inline auto state_fields(const PostGameFlow::RollbackState& v) { return std::tie(v.state_, v.age_, v.clear_sequence_, v.clear_frame_); }
inline auto state_fields(const SpellPresentation::RollbackState& v) { return std::tie(v.flags_, v.spell_id_, v.name_, v.bonus_base_, v.bonus_cap_, v.duration_, v.background_, v.anchor_, v.snapshot_, v.displays_, v.stage_background_, v.decoration_, v.banner_, v.notice_, v.spirit_notice_, v.spirit_notices_, v.bonus_digits_, v.bonus_million_, v.bonus_thousand_, v.bonus_capture_effect_, v.capture_frames_, v.production_, v.retiring_displays_); }
inline auto state_fields(const BossPresentation::RollbackState& v) { return std::tie(v.gate_, v.displayed_ratio_, v.target_ratio_, v.timer_seconds_, v.timer_displayed_, v.timer_frames_, v.timer_centiseconds_, v.remaining_star_handles_, v.warn_state_, v.move_state_, v.escalate_state_, v.proximity_hidden_, v.stars_created_, v.ring_created_, v.marker_glyph_, v.digit_tens_, v.digit_ones_, v.ring_arc_, v.ring_overlays_, v.star_handles_, v.marker_handle_, v.name_handle_); }
inline auto state_fields(const GameHud::RollbackState& v) { return std::tie(v.life_icons, v.bomb_icons, v.root, v.difficulty_notice, v.difficulty_label, v.flags, v.intro_age, v.frame_age, v.clear_age, v.clear_banner, v.displayed_score, v.score_increment, v.gauge_main, v.trance_skull_label, v.opening_signature, v.opening_signature_right, v.stage_logo_bank, v.boss, v.spell); }
inline auto state_fields(const MsgPresentation::RollbackState& v) { return std::tie(v.player_, v.side_, v.nameplate_); }
inline auto state_fields(const StagePresentation::SnapshotData::EnemyVisualData& v) { return std::tie(v.serial, v.script, v.has_vm, v.vm); }
inline auto state_fields(const StagePresentation::SnapshotData::BulletSlotData& v) { return std::tie(v.has_vm, v.vm, v.script, v.color, v.source_type, v.source_color, v.generation, v.active); }
inline auto state_fields(const StagePresentation::SnapshotData::ShotFadeData& v) { return std::tie(v.handle, v.generation, v.x, v.y, v.angle, v.drift, v.sine_moving, v.age, v.sine_motion); }
inline auto state_fields(const StagePresentation::PlayerEffectsState& v) { return std::tie(v.youmu_slash_handles, v.youmu_slash_extra, v.option_handles, v.full_option_handles, v.bomb_orb_handles, v.bomb_orb_exploding, v.bomb_ray_handles, v.focus_handle, v.option_character, v.option_count, v.option_anchor_deferred, v.option_rebuild_pending, v.options_hidden, v.death_options_queued, v.full_options, v.bomb_generation, v.bomb_background, v.bomb_beam); }
inline auto state_fields(const StagePresentation::SnapshotData& v) { return std::tie(v.enemies, v.bullets, v.shot_handles, v.shots, v.shot_generations, v.shot_fades, v.shot_faded_generations, v.has_player, v.player, v.player_script, v.player_tilt, v.teammate_players, v.teammate_tilts, v.spirit_handles, v.spirit_serials, v.spirit_scripts, v.item_vm_present, v.item_side_vm_present, v.item_vms, v.item_side_vms, v.youmu_slash_handles, v.youmu_slash_extra, v.option_handles, v.full_option_handles, v.bomb_orb_handles, v.bomb_orb_exploding, v.bomb_ray_handles, v.trance_handle, v.trance_was_active, v.bullet_cancel_handles, v.focus_handle, v.youmu_charge_handles, v.fog_handles, v.option_character, v.option_count, v.option_anchor_deferred, v.option_rebuild_pending, v.options_hidden, v.death_options_queued, v.full_options, v.frame_synced, v.bomb_generation, v.bomb_background, v.bomb_beam, v.player_effect_handles, v.teammate_effects); }
template<class... T> auto state_fields(const std::tuple<T...>& v) { return v; }
template<u32 N, bool A> auto state_fields(const FloatInterpolation<N, A>& v) {
    return std::tie(v.start,v.end,v.control1,v.control2,v.current,v.timer,v.duration,v.mode);
}
template<u32 N> auto state_fields(const IntegerInterpolation<N>& v) {
    return std::tie(v.start,v.end,v.control1,v.control2,v.current,v.timer,v.duration,v.mode);
}
inline auto state_fields(const StagePolarItp& v) { return std::tie(v.from_a,v.from_b,v.to_a,v.to_b,v.duration,v.elapsed,v.mode); }
inline auto state_fields(const StagePosItp& v) { return std::tie(v.start,v.target,v.p1,v.p2,v.duration,v.elapsed,v.mode,v.per_axis,v.axis_mode); }
inline auto state_fields(const StageMover& v) {
    return std::tie(v.pos,v.speed,v.angle,v.radius,v.growth,v.ellipse_angle,v.ellipse_ratio,
                    v.origin,v.sine_phase,v.aux,v.mode,v.polar,v.radial,v.postime);
}
template<class T> void digest_value(DigestSink& d, const T& v);
template<class A,class B> void digest_value(DigestSink&,const std::pair<A,B>&);
template<class K,class V> void digest_value(DigestSink&,const std::unordered_map<K,V>&);
template<class T> void digest_range(DigestSink& d, const T& v) {
    d.u64(v.size()); for (const auto& item : v) digest_value(d,item);
}
template<class T> struct IsOptional : std::false_type {};
template<class T> struct IsOptional<std::optional<T>> : std::true_type {};
template<class T> struct IsOwned : std::false_type {};
template<class T> struct IsOwned<std::unique_ptr<T>> : std::true_type {};
template<class T, class=void> struct IsRange : std::false_type {};
template<class T> struct IsRange<T,std::void_t<decltype(std::declval<T>().begin())>> : std::true_type {};
template<class T> void digest_value(DigestSink& d, const T& v) {
    if constexpr(std::is_same_v<T,bool>) d.boolean(v);
    else if constexpr(std::is_enum_v<T>) d.u32(static_cast<u32>(v));
    else if constexpr(std::is_integral_v<T>) {
        if constexpr(sizeof(T)>4) d.u64(v); else d.u32(v);
    } else if constexpr(std::is_floating_point_v<T>) d.f32(v);
    else if constexpr(std::is_same_v<T,AnmVm>) d.vm(v);
    else if constexpr(std::is_same_v<T,GameRandom>) { d.u32(v.seed());d.u32(v.index()); }
    else if constexpr(IsOptional<T>::value || IsOwned<T>::value) {
        d.boolean(bool(v)); if(v) digest_value(d,*v);
    } else if constexpr(std::is_array_v<T>) {
        d.u64(std::extent_v<T>);for(const auto& item:v) digest_value(d,item);
    } else if constexpr(IsRange<T>::value) digest_range(d,v);
    else std::apply([&](const auto&... fields) { (digest_value(d,fields),...); },state_fields(v));
}
template<class A, class B> void digest_value(DigestSink& d,const std::pair<A,B>& v) {
    digest_value(d,v.first);digest_value(d,v.second);
}
template<class K,class V> void digest_value(DigestSink& d,const std::unordered_map<K,V>& v) {
    std::map<K,V> ordered(v.begin(),v.end());digest_range(d,ordered);
}
} // namespace th13::multiplayer
