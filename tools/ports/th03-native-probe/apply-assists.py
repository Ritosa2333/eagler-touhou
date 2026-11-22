"""Apply the Eagler runtime adaptations to a pinned TH03 source copy."""
from pathlib import Path
import sys

root = Path(sys.argv[1])

def replace(path, old, new):
    file = root / path
    text = file.read_text(encoding='utf-8')
    if text.count(old) != 1:
        raise SystemExit(f'Unexpected source at {path}')
    file.write_text(text.replace(old, new), encoding='utf-8', newline='\n')

replace('src/audio/pmd_opna.cpp', '''		for(std::uint32_t elapsed = 0U; elapsed < clocks_per_sample; ++elapsed) {
			++chip_clocks;''', '''		const auto end_clock = chip_clocks + clocks_per_sample;
		while(chip_clocks < end_clock) {
			// Only timer IRQs can change state between sample boundaries.
			auto next_clock = end_clock;
			for(const auto deadline : timer_deadlines) {
				if(deadline >= 0) {
					next_clock = std::min(next_clock, static_cast<std::uint64_t>(deadline));
				}
			}
			// Re-read each timer in hardware order: its callback can change
			// either deadline, including scheduling work for the next clock.
			chip_clocks = std::max(chip_clocks + 1U, next_clock);''')

replace('src/core/frame.cpp', 'namespace th03::detail {', '''extern "C" {
int th03_assist_focus = 0;
int th03_assist_focus_p2 = 0;
int th03_touch_active[2] = {};
int th03_touch_unlimited[2] = {};
int th03_touch_x[2] = {};
int th03_touch_y[2] = {};
int th03_assist_x = -1;
int th03_assist_y = -1;
int th03_assist_x2 = -1;
int th03_assist_y2 = -1;
unsigned th03_assist_seen = 0;
int th03_netplay_paused = 0;
int th03_surrender_request = 0;
int th03_surrender_allowed = 0;
int th03_semantic_speculative = 0;
int th03_semantic_boundary = 0;
int th03_assist_charge = 0;
int th03_charge_enabled = 0;
int th03_charge_mode[2] = {};
int th03_charge_input[2][4] = {};
int th03_charge_output[2][4] = {};
int th03_charge_reset[2] = {};
int th03_charge_suppress = 0;
int th03_charge_overflow = 0;
}

namespace th03::detail {''')
replace('src/core/frame.cpp', '''	for(std::uint8_t player_id = 0U; player_id < player_count; ++player_id) {
		if(state.players[player_id].is_cpu != 0U) {
			player_update_cpu(state, player_id, observer, ruleset);''', '''	th03_surrender_allowed = state.game_mode == game_mode_vs_1p_2p &&
		state.players[0].is_cpu == 0U && state.players[1].is_cpu == 0U &&
		state.defeat_flag == DefeatFlag::none && state.players[0].lose_anim_time == 0U && state.players[1].lose_anim_time == 0U;
	if(th03_surrender_request && th03_surrender_allowed) {
		const auto loser = static_cast<std::uint8_t>((th03_surrender_request - 1) & 1);
		const auto winner = static_cast<std::uint8_t>(1U - loser);
		auto &player = state.players[loser];
		player.halfhearts = 0U; player.lose_anim_time = 0xffU;
		state.last_fatal_player = loser; state.winner = static_cast<std::int8_t>(winner);
		if(th03_surrender_request > 2) state.players[winner].rounds_won = 2U;
		else ++state.players[winner].rounds_won;
		state.defeat_flag = DefeatFlag::banner; state.round_or_result_frame = 0U;
		if(observer != nullptr) {
			observer->main_hud_player_value_changed(MainHudPlayerValueChanged{.player = static_cast<PlayerId>(loser), .kind = MainHudPlayerValueKind::halfhearts, .value_after = 0U});
			observer->main_hud_player_value_changed(MainHudPlayerValueChanged{.player = static_cast<PlayerId>(winner), .kind = MainHudPlayerValueKind::rounds_won, .value_after = state.players[winner].rounds_won});
		}
		th03_surrender_allowed = 0;
	}
	th03_surrender_request = 0;
	for(std::uint8_t player_id = 0U; player_id < player_count; ++player_id) {
		if(state.players[player_id].is_cpu != 0U) {
			player_update_cpu(state, player_id, observer, ruleset);''')
replace('src/core/frame/player_movement.inl',
        'std::int8_t move_human(PlayerState &player, std::uint16_t live_input) noexcept',
        'std::int8_t move_human(PlayerState &player, std::uint16_t live_input, bool focus) noexcept')
replace('src/core/frame/player_movement.inl',
        '\t\tif(result == MoveResult::valid) {\n\t\t\tapply_velocity(player, velocity);',
        '\t\tif(result == MoveResult::valid) {\n\t\t\tif(focus) { velocity.x = exact::arithmetic_shift_right(velocity.x, 1U); velocity.y = exact::arithmetic_shift_right(velocity.y, 1U); }\n\t\t\tapply_velocity(player, velocity);')
replace('src/core/frame/player_collision.inl',
        '\t\tfinal_velocity_x_write = move_human(player, live_input);',
        '''\t\tconst bool assist = state.players[player_id].is_cpu == 0U &&
\t\t\t((player_id == 0U && (state.game_mode == game_mode_story || state.game_mode == game_mode_vs_1p_cpu)) || state.game_mode == game_mode_vs_1p_2p) &&
\t\t\tstate.defeat_flag == DefeatFlag::none && player.lose_anim_time == 0U;
\t\tconst bool focus = assist && (player_id == 0U ? th03_assist_focus : th03_assist_focus_p2) != 0;
\t\tif(assist && th03_touch_active[player_id]) {
\t\t\tint &dx = th03_touch_x[player_id], &dy = th03_touch_y[player_id];
\t\t\tint x = focus && !th03_touch_unlimited[player_id] ? (dx >> 1) : dx;
\t\t\tint y = focus && !th03_touch_unlimited[player_id] ? (dy >> 1) : dy;
\t\t\tconst double length = std::sqrt(static_cast<double>(x)*x + static_cast<double>(y)*y);
\t\t\tconst int limit = (focus ? player.speed_aligned_x / 2 : player.speed_aligned_x);
\t\t\tif(!th03_touch_unlimited[player_id] && length > limit && length > 0) { x = static_cast<int>(x * limit / length); y = static_cast<int>(y * limit / length); }
\t\t\tconst auto oldx=player.center_x.raw(), oldy=player.center_y.raw();
\t\t\tapply_velocity(player, Velocity{static_cast<std::int16_t>(x),static_cast<std::int16_t>(y)});
\t\t\tconst int usedx=player.center_x.raw()-oldx, usedy=player.center_y.raw()-oldy;
\t\t\tdx -= focus && !th03_touch_unlimited[player_id] ? usedx * 2 : usedx;
\t\t\tdy -= focus && !th03_touch_unlimited[player_id] ? usedy * 2 : usedy;
\t\t\tif(player.center_x.raw()==player_x_min || player.center_x.raw()==player_x_max) dx=0;
\t\t\tif(player.center_y.raw()==player_y_min || player.center_y.raw()==player_y_max) dy=0;
\t\t\tfinal_velocity_x_write = static_cast<std::int8_t>(std::clamp(usedx,-128,127));
\t\t} else { final_velocity_x_write = move_human(player, live_input, focus); }
\t\tth03_touch_x[player_id]=std::clamp(th03_touch_x[player_id],-8192,8191);
\t\tth03_touch_y[player_id]=std::clamp(th03_touch_y[player_id],-8192,8191);''')
replace('src/core/frame.cpp', '#include <cstdint>', '#include <cstdint>\n#include <cmath>')
charge_updates = root / 'src/core/frame/characters/charge_update.inl'
charge_text = charge_updates.read_text(encoding='utf-8')
for target in ('state.players[player_id].gauge_charged = 0U;', 'player.gauge_charged = 0U;'):
    if target not in charge_text:
        raise SystemExit('Unexpected charge shot source')
    charge_text = charge_text.replace(target, '(th03_charge_reset[player_id] = 1, ' + target[:-1] + ');')
charge_updates.write_text(charge_text, encoding='utf-8', newline='\n')
replace('src/core/game_internal.hpp',
        '\tstd::uint16_t gauge_charged = 0;\n\tstd::uint16_t gauge_avail = 0;',
        '\tstd::uint16_t gauge_charged = 0;\n\tstd::array<std::uint16_t, 8> th03_pending_charge{};\n\tstd::uint8_t th03_pending_charge_count = 0;\n\tstd::uint16_t gauge_avail = 0;')
replace('src/core/rollback_snapshot.cpp',
        '\tvalue.knockback_active, value.gauge_charged, value.gauge_avail,',
        '\tvalue.knockback_active, value.gauge_charged, value.th03_pending_charge, value.th03_pending_charge_count, value.gauge_avail,')
replace('src/core/state_hash.cpp',
        '\thash = hash_u16(hash, player.gauge_charged);',
        '\thash = hash_u16(hash, player.gauge_charged);\n\thash = hash_u8(hash, player.th03_pending_charge_count);\n\tfor(auto value : player.th03_pending_charge) hash = hash_u16(hash, value);')
replace('src/core/frame/player_collision.inl',
        '''\tauto &player = state.players[player_id];
\tbool acted = false;
\tif(player.knockback_active) {''',
        '''\tauto &player = state.players[player_id];
\tbool acted = false;
\tconst bool th03_was_knocked_back = player.knockback_active;
\tif(th03_charge_enabled && th03_charge_mode[player_id] == 2 && !th03_charge_reset[player_id] && !player.knockback_active) {
\t\tplayer.gauge_charged = std::min<std::uint16_t>(static_cast<std::uint16_t>(th03_charge_input[player_id][0]), player.gauge_avail);
\t\tplayer.spell_ready_frames = static_cast<std::uint8_t>(th03_charge_input[player_id][1]);
\t}
\tif(player.knockback_active) {''')
replace('src/core/frame/player_actions.inl',
        '\tconst auto charged = player.gauge_charged;\n\tif(charged >= gauge_64) {',
        '''\tconst auto charged = player.gauge_charged;
\tif(th03_charge_enabled && !th03_charge_suppress) { th03_charge_output[player_id][2] = charged; th03_charge_output[player_id][3] |= 1; }
\tif(charged >= gauge_64) {''')
replace('src/core/frame/player_collision.inl',
        '''\t} else if(
\t\tstate.autofire_shot_gate[player_id] <= 2U &&
\t\tplayer.hyper_active == 0U''',
        '''\t} else if(th03_charge_enabled && th03_charge_mode[player_id] == 1 && (th03_charge_input[player_id][3] & 4)) {
\t\tconst auto *sample = th03_charge_input[player_id];
\t\tplayer.gauge_charged = std::min<std::uint16_t>(static_cast<std::uint16_t>(sample[0]), player.gauge_avail);
\t\tplayer.spell_ready_frames = static_cast<std::uint8_t>(sample[1]);
\t\tif(sample[3] & 2) observe_main_feedback(observer, MainFeedbackCueSite::gauge_spell_ready, player_id);
\t} else if(
\t\tstate.autofire_shot_gate[player_id] <= 2U &&
\t\tplayer.hyper_active == 0U''')
replace('src/core/frame/player_collision.inl',
        '''\t\t\tif(player.gauge_charged == 0x180U) {
\t\t\t\tobserve_main_feedback(''',
        '''\t\t\tif(player.gauge_charged == 0x180U) {
\t\t\t\tif(th03_charge_enabled) th03_charge_output[player_id][3] |= 2;
\t\t\t\tobserve_main_feedback(''')
replace('src/core/frame/player_collision.inl',
        '''\tif(player.shot_active == 2U) {
\t\tlive_input = 0U;''',
        '''\tif(!th03_was_knocked_back && player.hyper_active == 0U && player.th03_pending_charge_count != 0U && !(th03_charge_enabled && th03_charge_mode[player_id] != 1 && (th03_charge_output[player_id][3] & 1))) {
\t\tconst auto pending = player.th03_pending_charge[0];
\t\tfor(std::size_t i=1; i<player.th03_pending_charge_count; ++i) player.th03_pending_charge[i-1] = player.th03_pending_charge[i];
\t\tplayer.th03_pending_charge[--player.th03_pending_charge_count] = 0U;
\t\tconst auto saved = player.gauge_charged;
\t\tplayer.gauge_charged = std::min<std::uint16_t>(pending, player.gauge_avail);
\t\tacted = acted || player.gauge_charged >= gauge_64;
\t\tth03_charge_suppress = 1; release_gauge(state, player_id, observer); th03_charge_suppress = 0;
\t\tplayer.gauge_charged = std::min<std::uint16_t>(saved, player.gauge_avail);
\t}
\tif(th03_charge_enabled) {
\t\tth03_charge_output[player_id][0] = player.gauge_charged;
\t\tth03_charge_output[player_id][1] = player.spell_ready_frames;
\t\tth03_charge_output[player_id][3] |= 4;
\t}
\tif(player.shot_active == 2U) {
\t\tlive_input = 0U;''')
replace('src/core/frame/player_collision.inl',
        '''\tauto &player = state.players[player_id];
\tstd::optional<std::int8_t> final_velocity_x_write;
\tif(!player_update_begin(state, player_id, observer)) {''',
        '''\tauto &player = state.players[player_id];
\tif(th03_charge_enabled && th03_charge_mode[player_id] == 1 && (th03_charge_input[player_id][3] & 1) && th03_charge_input[player_id][2] >= gauge_64) {
\t\tif(player.th03_pending_charge_count < player.th03_pending_charge.size()) player.th03_pending_charge[player.th03_pending_charge_count++] = static_cast<std::uint16_t>(th03_charge_input[player_id][2]);
\t\telse th03_charge_overflow = 1;
\t}
\tstd::optional<std::int8_t> final_velocity_x_write;
\tif(!player_update_begin(state, player_id, observer)) {''')
replace('src/core/frame/player_collision.inl',
        '\tplayer_update_finish(\n\t\tstate, player_id, live_input, final_velocity_x_write, observer\n\t);',
        '''\tif(player.is_cpu == 0U && (static_cast<unsigned>(th03_assist_charge) & (1U << player_id))) {
\t\tstate.autofire_shot_gate[player_id] = std::min<std::uint8_t>(state.autofire_shot_gate[player_id], 2U);
\t}
\tplayer_update_finish(
\t\tstate, player_id, live_input, final_velocity_x_write, observer
\t);
\tif(player_id == 0U) {
\t\t++th03_assist_seen;
\t\tif(state.players[0].is_cpu == 0U && (state.game_mode == game_mode_story || state.game_mode == game_mode_vs_1p_cpu || state.game_mode == game_mode_vs_1p_2p) && state.defeat_flag == DefeatFlag::none && player.lose_anim_time == 0U) {
\t\t\tth03_assist_x = (player.center_x.raw() >> 4) + state.playfield_fg_shift_x[0] + 14;
\t\t\tth03_assist_y = ((player.center_y.raw() >> 5) + 7) * 2;
\t\t} else { th03_assist_x = -1; th03_assist_y = -1; }
\t}
\tif(player_id == 1U) {
\t\tif(player.is_cpu == 0U && state.game_mode == game_mode_vs_1p_2p && state.defeat_flag == DefeatFlag::none && player.lose_anim_time == 0U) {
\t\t\tth03_assist_x2 = (player.center_x.raw() >> 4) + state.playfield_fg_shift_x[1] + 334;
\t\t\tth03_assist_y2 = ((player.center_y.raw() >> 5) + 7) * 2;
\t\t} else { th03_assist_x2 = -1; th03_assist_y2 = -1; }
\t}''')
replace('include/th03/app/classic/classic_runner.hpp',
        '\tClassicContentPaths content{};',
        '\tClassicContentPaths content{};\n\tbool th03_direct_versus = false;')
replace('src/app/classic/classic_runner.cpp',
        '\tauto resident = initial_resident(request.seed, request.gameplay_ruleset);',
        '''\tauto resident = initial_resident(request.seed, request.gameplay_ruleset);
\tif(request.th03_direct_versus) {
\t\tresident.game_mode = game_mode::vs_1p_2p;
\t\tresident.demo_number = 0U;
\t\tresident.is_cpu = {0U, 0U};
\t\tresident.paletted_character = {1, 1};
\t}''')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '#include "th03/app/new_classic/new_classic_pause.hpp"',
        '#include "th03/app/new_classic/new_classic_pause.hpp"\nextern "C" int th03_netplay_paused;')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '\t\t\tvs_pause_menu.reset();\n\t\t\tif(resume) {',
        '\t\t\tvs_pause_menu.reset();\n\t\t\tth03_netplay_paused = 0;\n\t\t\tif(resume) {')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '\t\t\t\tvs_pause_menu.emplace(\n\t\t\t\t\treplay_recorder && replay_recorder->recording(), patch_assets',
        '\t\t\t\tth03_netplay_paused = 1;\n\t\t\t\tvs_pause_menu.emplace(\n\t\t\t\t\treplay_recorder && replay_recorder->recording(), patch_assets')

# The web audio host supplies FM and Off; upstream MIDI requires a native device.
replace('src/app/classic/op/classic_op_options_scene.cpp',
        '''\tcase ClassicOpOptionsChoice::music_driver:
\t\tvalue = config_.bgm_mode == ClassicBgmMode::midi ?
\t\t\tmessages.text(NewClassicMessageId::value_midi_sc88) :
\t\t\tmessages.text(NewClassicMessageId::value_fm86);''',
        '''\tcase ClassicOpOptionsChoice::music_driver:
\t\tvalue = config_.bgm_mode == ClassicBgmMode::off ?
\t\t\tmessages.text(NewClassicMessageId::value_off) :
\t\t\tmessages.text(NewClassicMessageId::value_fm86);''')
replace('src/app/classic/op/classic_op_options_scene.cpp',
        '''\t\tconst auto current = static_cast<std::uint8_t>(
\t\t\tconfig_.bgm_mode == ClassicBgmMode::midi ? 1U : 0U
\t\t);
\t\tconfig_.bgm_mode = ring_step(current, direction, 1U) == 0U ?
\t\t\tClassicBgmMode::fm : ClassicBgmMode::midi;''',
        '''\t\tconfig_.bgm_mode = config_.bgm_mode == ClassicBgmMode::off ?
\t\t\tClassicBgmMode::fm : ClassicBgmMode::off;''')
replace('src/app/classic/op/classic_op_options_scene.cpp',
        '''\t\t}) || !push_audio_command({
\t\t\t.type = ClassicAudioCommandType::play_song,
\t\t\t.cue = ClassicMusicCue::menu_main,
\t\t}))) {
\t\t\treturn false;
\t\t}
\t\tconfig_changed = true;
\t\tbreak;
\t}
\tcase ClassicOpOptionsChoice::sfx:''',
        '''\t\t}) || (config_.bgm_mode != ClassicBgmMode::off && !push_audio_command({
\t\t\t.type = ClassicAudioCommandType::play_song,
\t\t\t.cue = ClassicMusicCue::menu_main,
\t\t})))) {
\t\t\treturn false;
\t\t}
\t\tconfig_changed = true;
\t\tbreak;
\t}
\tcase ClassicOpOptionsChoice::sfx:''')

# Raising music volume must not turn an explicitly disabled driver back on.
replace('src/app/classic/op/classic_op_options_scene.cpp',
        '''\t\t\t\tif(config_.bgm_mode == ClassicBgmMode::off) {
\t\t\t\t\tconfig_.bgm_mode = ClassicBgmMode::fm;
\t\t\t\t}
\t\t\t\tif(!push_audio_command({''',
        '''\t\t\t\tif(!push_audio_command({''')
replace('src/app/classic/op/classic_op_options_scene.cpp',
        '''\t\t\t\t}) || !push_audio_command({
\t\t\t\t\t.type = ClassicAudioCommandType::play_song,
\t\t\t\t\t.cue = ClassicMusicCue::menu_main,
\t\t\t\t})) {
\t\t\t\t\treturn false;
\t\t\t\t}
\t\t\t} else if(!push_audio_command({''',
        '''\t\t\t\t}) || (config_.bgm_mode != ClassicBgmMode::off && !push_audio_command({
\t\t\t\t\t.type = ClassicAudioCommandType::play_song,
\t\t\t\t\t.cue = ClassicMusicCue::menu_main,
\t\t\t\t}))) {
\t\t\t\t\treturn false;
\t\t\t\t}
\t\t\t} else if(!push_audio_command({''')

# Direct network matches take the shared room rank instead of local settings.
replace('src/app/classic/op/classic_op_session_backend.cpp',
        'struct ClassicOpSessionBackend final {',
        'extern "C" int th03_room_rank;\nstruct ClassicOpSessionBackend final {')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '''\t\t\tresident.rank = static_cast<std::uint8_t>(config.rank);
\t\t\tresident.key_mode = static_cast<std::uint8_t>(config.key_mode);
\t\t\tauto direct = ClassicOpTitleMenuScene::direct_vs_reentry(''',
        '''\t\t\tresident.rank = th03_room_rank >= 0 ? static_cast<std::uint8_t>(th03_room_rank) : static_cast<std::uint8_t>(config.rank);
\t\t\tresident.key_mode = static_cast<std::uint8_t>(config.key_mode);
\t\t\tauto direct = ClassicOpTitleMenuScene::direct_vs_reentry(''')

# The web build exposes the native game only; omit the unavailable Classic switch.
for size in (6, 7):
    replace('src/app/classic/op/classic_op_options_scene.cpp',
            f'constexpr std::array<ClassicOpOptionsChoice, {size}U> ' +
            ('system_choices' if size == 6 else 'system_update_choices') +
            '{{\n\tClassicOpOptionsChoice::switch_classic,\n',
            f'constexpr std::array<ClassicOpOptionsChoice, {size - 1}U> ' +
            ('system_choices' if size == 6 else 'system_update_choices') + '{{\n')

# The host provides multiplayer through the launcher, not the upstream online mode.
replace('src/app/classic/op/classic_op_scene.cpp',
        'constexpr std::array<ClassicOpVsChoice, 4U> new_classic_vs_choices{\n\tClassicOpVsChoice::one_player_cpu,\n\tClassicOpVsChoice::one_player_two_player,\n\tClassicOpVsChoice::cpu_cpu,\n\tClassicOpVsChoice::online_match,',
        'constexpr std::array<ClassicOpVsChoice, 3U> new_classic_vs_choices{\n\tClassicOpVsChoice::one_player_cpu,\n\tClassicOpVsChoice::one_player_two_player,\n\tClassicOpVsChoice::cpu_cpu,')

# Preserve the nonblocking charge cut-in across prediction and rollback. The
# presenter state already captures pixels and palette; retain its two counters
# instead of treating the whole visual effect as a network synchronization barrier.
replace('include/th03/present/main_rollback.hpp',
        '\tstd::uint8_t warning_next_interstitial_sample = 0U;',
        '\tstd::uint8_t warning_next_interstitial_sample = 0U;\n\tstd::uint8_t warning_nonblocking_frames = 0U;\n\tstd::uint8_t warning_nonblocking_mask = 0U;')
replace('src/present/main_rollback.cpp',
        '\t\twarning_gauge.nonblocking_interstitial_frames_ == 0U &&\n', '')
replace('src/present/main_rollback.cpp',
        '\tstate.warning_interstitial_active = warning_gauge.interstitial_active_;',
        '\tstate.warning_nonblocking_frames = warning_gauge.nonblocking_interstitial_frames_;\n\tstate.warning_nonblocking_mask = warning_gauge.nonblocking_interstitial_player_mask_;\n\tstate.warning_interstitial_active = warning_gauge.interstitial_active_;')
replace('src/present/main_rollback.cpp',
        '\twarning_gauge.nonblocking_interstitial_frames_ = 0U;\n\twarning_gauge.nonblocking_interstitial_player_mask_ = 0U;',
        '\twarning_gauge.nonblocking_interstitial_frames_ = state.warning_nonblocking_frames;\n\twarning_gauge.nonblocking_interstitial_player_mask_ = state.warning_nonblocking_mask;')

# Warning cut-ins can be predicted because presenter and round state are saved.
replace('src/present/main_rollback.cpp', '\t\tstate.warning_interstitial_active ||\n', '')
replace('src/present/main_rollback.cpp', '\t\tstate.hud_late_state_pending ||\n', '')
for condition in ('!warning_gauge.interstitial_active_', '!hud_result.hud_late_state_pending_',
                  '!round.warning_interstitial_active_', '!round.hud_start_step_seen_'):
    replace('src/present/main_rollback.cpp', '\t\t' + condition + ' &&\n', '')

# Resolve the 16 graphics and 8 text colors once per surface, rather than
# expanding and ordering channels for every pixel. Keep the source validation.
replace('src/app/runtime/host_surface.cpp', '#include <cstdint>\n', '#include <cstdint>\n#include <cstring>\n')
replace('src/app/runtime/host_surface.cpp', '''	for(std::size_t y = 0U; y < source_height; ++y) {
		const auto row_offset = y * target.pitch_bytes;
		const auto source_offset = y * source_width;
		for(std::size_t x = 0U; x < source_width; ++x) {
			const auto pixel = source.pixels[source_offset + x];
			const auto color = (pixel.layer ==
				present::ComposedPixelLayer::graphics) ?
				graphics_color(source.graphics_palette.colors[pixel.value]) :
				text_color(pixel.value);
			const auto bytes = ordered_bytes(color, target.byte_order);
			const auto destination = row_offset + (x * channel_count);
			for(std::size_t channel = 0U; channel < channel_count; ++channel) {
				target.bytes[destination + channel] =
					static_cast<std::byte>(bytes[channel]);
			}
		}
	}''', '''	std::array<std::array<std::uint8_t, channel_count>, 16> graphics_lut;
	std::array<std::array<std::uint8_t, channel_count>, 8> text_lut;
	for(std::size_t i = 0; i < graphics_lut.size(); ++i) {
		graphics_lut[i] = ordered_bytes(graphics_color(source.graphics_palette.colors[i]), target.byte_order);
	}
	for(std::size_t i = 0; i < text_lut.size(); ++i) {
		text_lut[i] = ordered_bytes(text_color(static_cast<std::uint8_t>(i)), target.byte_order);
	}
	for(std::size_t y = 0; y < source_height; ++y) {
		auto *row = target.bytes.data() + y * target.pitch_bytes;
		const auto *pixels = source.pixels.data() + y * source_width;
		for(std::size_t x = 0; x < source_width; ++x) {
			const auto pixel = pixels[x];
			const auto &bytes = pixel.layer == present::ComposedPixelLayer::graphics ? graphics_lut[pixel.value] : text_lut[pixel.value];
			std::memcpy(row + x * channel_count, bytes.data(), channel_count);
		}
	}''')

# Prototype a local-only semantic snapshot boundary for an active VS process.
# Network and audio-device state are deliberately outside this snapshot.
# The visual snapshots belong to this runtime and never cross the network. Use
# a distinct marker and checksum that processes four bytes at a time.
replace('src/present/rollback_snapshot.cpp',
        "std::byte{'V'}, std::byte{'0'}, std::byte{'0'}, std::byte{'1'},",
        "std::byte{'L'}, std::byte{'0'}, std::byte{'0'}, std::byte{'1'},")
replace('src/present/rollback_snapshot.cpp',
        '''\tauto hash = fnv_offset_basis;
\tfor(const auto value : bytes) {
\t\thash ^= std::to_integer<std::uint8_t>(value);
\t\thash *= fnv_prime;
\t}
\treturn hash;''',
        '''\tauto hash = fnv_offset_basis;
\tstd::size_t at = 0;
\tfor(; at + sizeof(std::uint32_t) <= bytes.size(); at += sizeof(std::uint32_t)) {
\t\tstd::uint32_t word;
\t\tstd::memcpy(&word, bytes.data() + at, sizeof(word));
\t\thash = (hash ^ word) * fnv_prime;
\t}
\tfor(; at < bytes.size(); ++at) hash = (hash ^ std::to_integer<std::uint8_t>(bytes[at])) * fnv_prime;
\treturn hash;''')
replace('tests/test_present.cpp',
        '''\tstd::uint32_t hash = 2166136261U;
\tfor(const auto value : bytes) {
\t\thash ^= std::to_integer<std::uint8_t>(value);
\t\thash *= 16777619U;
\t}
\treturn hash;''',
        '''\tstd::uint32_t hash = 2166136261U;
\tstd::size_t at = 0;
\tfor(; at + 4 <= bytes.size(); at += 4) {
\t\tstd::uint32_t word = 0;
\t\tfor(unsigned i = 0; i < 4; ++i) word |= std::uint32_t(std::to_integer<std::uint8_t>(bytes[at + i])) << (8 * i);
\t\thash = (hash ^ word) * 16777619U;
\t}
\tfor(; at < bytes.size(); ++at) hash = (hash ^ std::to_integer<std::uint8_t>(bytes[at])) * 16777619U;
\treturn hash;''')
replace('tests/test_present.cpp', ')) == 1721220782U,', ')) == 1006282275U,')
replace('include/th03/present/main_round.hpp',
        'MainRoundPresenter(const MainRoundPresenter &) = delete;\n\tMainRoundPresenter &operator=(const MainRoundPresenter &) = delete;',
        'MainRoundPresenter(const MainRoundPresenter &) = default;\n\tMainRoundPresenter &operator=(const MainRoundPresenter &) = default;')
replace('include/th03/app/main/main_process.hpp',
        'class MainProcess final {\npublic:',
        'class MainProcess final {\npublic:\n\t[[nodiscard]] std::unique_ptr<present::MainRoundPresenter> th03_capture_round() const;\n\tvoid th03_restore_round(const present::MainRoundPresenter &round);')
replace('src/app/main/main_process.cpp',
        'bool MainProcess::rollback_boundary_available() const noexcept',
        'std::unique_ptr<present::MainRoundPresenter> MainProcess::th03_capture_round() const { return std::make_unique<present::MainRoundPresenter>(*impl_->round); }\nvoid MainProcess::th03_restore_round(const present::MainRoundPresenter &round) { *impl_->round = round; }\nbool MainProcess::rollback_boundary_available() const noexcept')
replace('include/th03/app/new_classic/replay/new_classic_replay.hpp',
        '\t[[nodiscard]] bool recording() const noexcept;',
        '''	struct Th03ReplayRollbackCheckpoint {
		bool active = false;
		std::size_t records = 0;
		std::uint32_t samples = 0, source_vsync_clock = 0, round_vsync_last = 0, round_real_frames = 0, round_timed_frames = 0;
		std::size_t checkpoints = 0, splits = 0;
		std::optional<formats::ReplayMetadata> metadata;
		std::optional<formats::ReplaySlowdown> slowdown;
		std::optional<CanonicalState> last_terminal;
		bool round_timer_active = false, slow_sample_pending = false;
	};
	[[nodiscard]] Th03ReplayRollbackCheckpoint th03_capture() const noexcept {
		Th03ReplayRollbackCheckpoint result{};
		if(!document_ || completed_ || !document_->metadata) return result;
		result.active = true; result.records = document_->session.records.size(); result.samples = document_->session.sample_count;
		result.checkpoints = document_->checkpoints.size(); result.splits = document_->expected_round_splits.size();
		result.metadata = document_->metadata; result.slowdown = document_->slowdown; result.last_terminal = last_terminal_;
		result.source_vsync_clock = source_vsync_clock_; result.round_vsync_last = round_vsync_last_;
		result.round_real_frames = round_real_frames_; result.round_timed_frames = round_timed_frames_;
		result.round_timer_active = round_timer_active_; result.slow_sample_pending = slow_sample_pending_;
		return result;
	}
	[[nodiscard]] bool th03_can_restore(const Th03ReplayRollbackCheckpoint &saved) const noexcept {
		if(!saved.active) return !document_ && !completed_;
		return document_ && !completed_ && saved.metadata && saved.records <= document_->session.records.size() &&
			saved.checkpoints <= document_->checkpoints.size() && saved.splits <= document_->expected_round_splits.size();
	}
	[[nodiscard]] bool th03_restore(const Th03ReplayRollbackCheckpoint &saved) noexcept {
		if(!th03_can_restore(saved)) return false;
		if(!saved.active) return true;
		document_->session.records.resize(saved.records); document_->session.sample_count = saved.samples;
		document_->checkpoints.resize(saved.checkpoints); document_->expected_round_splits.resize(saved.splits);
		document_->metadata = saved.metadata; document_->slowdown = saved.slowdown; last_terminal_ = saved.last_terminal;
		source_vsync_clock_ = saved.source_vsync_clock; round_vsync_last_ = saved.round_vsync_last;
		round_real_frames_ = saved.round_real_frames; round_timed_frames_ = saved.round_timed_frames;
		round_timer_active_ = saved.round_timer_active; slow_sample_pending_ = saved.slow_sample_pending; return true;
	}
	[[nodiscard]] bool recording() const noexcept;''')
replace('src/app/main/main_process_driver_internal.hpp',
        'MainProcessDriver(const MainProcessDriver &) = delete;\n\tMainProcessDriver &operator=(const MainProcessDriver &) = delete;',
        'MainProcessDriver(const MainProcessDriver &) = default;\n\tMainProcessDriver &operator=(const MainProcessDriver &) = default;')
replace('src/app/main/main_process_driver.cpp',
        '#include "main/main_process_driver_internal.hpp"',
        '#include "main/main_process_driver_internal.hpp"\nextern "C" { extern int th03_semantic_speculative, th03_semantic_boundary; }')
replace('src/app/main/main_process.cpp',
        '#include "th03/app/main/main_process.hpp"',
        '#include "th03/app/main/main_process.hpp"\nextern "C" int th03_semantic_boundary;')
replace('src/app/main/main_process.cpp',
        '\tif(!rollback_boundary_available()) {\n\t\treturn std::unexpected(MainProcessRollbackError{',
        '\tif(!rollback_boundary_available() && !th03_semantic_boundary) {\n\t\treturn std::unexpected(MainProcessRollbackError{')
replace('src/app/main/main_process_driver.cpp',
        "\tconst bool deferred = operations_.deferred_frame_pending &&\n\t\toperations_.deferred_frame_pending(operations_.context);",
        "\tif(th03_semantic_speculative && stepped->status != StepStatus::running && (stepped->status != StepStatus::phase_transition || context_before == InputContext::pause || (operations_.deferred_frame_pending && operations_.deferred_frame_pending(operations_.context)))) { th03_semantic_boundary = 1; return MainProcessDriverTick{}; }\n\tconst bool deferred = operations_.deferred_frame_pending &&\n\t\toperations_.deferred_frame_pending(operations_.context);")
replace('src/app/classic/op/classic_op_session_backend.cpp',
        'struct ClassicOpSessionBackend final {',
        'extern "C" { int th03_match_winner = -1, th03_room_replay = 0; unsigned th03_match_serial = 0; }\nstruct ClassicOpSessionBackend final {')
replace('include/th03/app/new_classic/replay/new_classic_replay.hpp',
        '\t\tformats::ReplayEndReason reason,\n\t\tconst GameInit &resident\n\t);',
        '\t\tformats::ReplayEndReason reason,\n\t\tconst GameInit &resident,\n\t\tbool encode_in_host = false\n\t);')
replace('src/app/new_classic/replay/new_classic_replay.cpp',
        '\tformats::ReplayEndReason reason,\n\tconst GameInit &resident\n)',
        '\tformats::ReplayEndReason reason,\n\tconst GameInit &resident,\n\tbool encode_in_host\n)')
replace('src/app/new_classic/replay/new_classic_replay.cpp',
        '\tauto candidate = *document_;',
        '\tconst bool handoff = encode_in_host && !extension_provider_.finish;\n\tauto candidate = handoff ? std::move(*document_) : *document_;')
replace('src/app/new_classic/replay/new_classic_replay.cpp',
        '\t\t\t*last_terminal_, document_->practice',
        '\t\t\t*last_terminal_, candidate.practice')
replace('src/app/new_classic/replay/new_classic_replay.cpp',
        '\tauto encoded = formats::write_v14_replay(candidate);',
        '''\t// The web room bridge serializes the completed document once when it
\t// exports it. There is no native Replay extension to finalize here.
\tif(handoff) {
\t\tcompleted_ = std::move(candidate);
\t\tclear_memory();
\t\treturn {};
\t}
\tauto encoded = formats::write_v14_replay(candidate);''')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '''\t\t\t\t\tformats::ReplayEndReason::menu_return,
\t\t\t\t\tvs_terminal->transition.resident
\t\t\t\t); !finished) {
\t\t\t\t\treturn std::unexpected(detail::replay_capture_error());''',
        '''\t\t\t\t\tformats::ReplayEndReason::menu_return,
\t\t\t\t\tvs_terminal->transition.resident, th03_room_replay
\t\t\t\t); !finished) {
\t\t\t\t\tif(!th03_room_replay) return std::unexpected(detail::replay_capture_error());
\t\t\t\t\treplay_recorder->discard();''')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '''\t\t\tif(!vs_terminal || !replay_recorder->finish(
\t\t\t\tformats::ReplayEndReason::game_over,
\t\t\t\tvs_terminal->transition.resident
\t\t\t)) {
\t\t\t\treturn std::unexpected(detail::replay_capture_error());
\t\t\t}''',
        '''\t\t\tif(!vs_terminal) return std::unexpected(detail::replay_capture_error());
\t\t\tif(!replay_recorder->finish(
\t\t\t\tformats::ReplayEndReason::game_over,
\t\t\t\tvs_terminal->transition.resident, th03_room_replay
\t\t\t)) {
\t\t\t\tif(!th03_room_replay) return std::unexpected(detail::replay_capture_error());
\t\t\t\treplay_recorder->discard();
\t\t\t}''')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        'struct ClassicOpSessionBackend final {',
        '''struct ClassicOpSessionBackend;
static ClassicOpSessionBackend *th03_active_vs_backend = nullptr;
struct ClassicOpSessionBackend final {''')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        'start_selection(\n\t\tClassicOpSelectionMode mode\n\t)\n\t{',
        'start_selection(\n\t\tClassicOpSelectionMode mode\n\t)\n\t{\n\t\tth03_active_vs_backend = this;')
# The web port exposes the complete character roster in every mode without
# modifying a player's stored scores or unlock progress.
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '\t\tauto available = available_character_count();',
        '\t\tstd::expected<std::uint8_t, ClassicOpSessionError> available = std::uint8_t{9U};')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '\t\tconst auto input_context = vs_driver->input_context();\n\t\tconst auto normalized = detail::normalize_vs_cpu_main_input(',
        '\t\tth03_active_vs_backend = this;\n\t\tconst auto input_context = vs_driver->input_context();\n\t\tconst auto normalized = detail::normalize_vs_cpu_main_input(')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        '\t\treturn {};\n\t}\n\n\t[[nodiscard]] std::expected<void, ClassicOpSessionError>\n\tcreate_direct_vs_selection(GameInit resident)',
        '\t\tif(vs_terminal && (vs_terminal->terminal.winner == 0 || vs_terminal->terminal.winner == 1)) { th03_match_winner = vs_terminal->terminal.winner; ++th03_match_serial; }\n\t\treturn {};\n\t}\n\n\t[[nodiscard]] std::expected<void, ClassicOpSessionError>\n\tcreate_direct_vs_selection(GameInit resident)')
replace('src/app/classic/op/classic_op_session_backend.cpp',
        'std::expected<detail::ClassicOpSessionProductionBackend, ClassicOpSessionError>\ndetail::create_classic_op_session_production_backend(',
        '''struct Th03SemanticSnapshot {
	std::unique_ptr<MainProcessRollbackSnapshot> process;
	std::unique_ptr<MainProcessDriver> driver;
	std::unique_ptr<present::MainRoundPresenter> round;
	NewClassicReplayRecorder::Th03ReplayRollbackCheckpoint replay;
	bool has_replay = false;
	MainProcess *owner = nullptr;
};
static std::array<Th03SemanticSnapshot, 16> th03_semantic_slots;
static std::unique_ptr<MainProcessRollbackSnapshot> th03_semantic_scratch;
extern "C" int th03_semantic_capture(int slot) {
	auto *backend = th03_active_vs_backend;
	if(slot < 0 || slot >= 16 || !backend || !backend->vs_process || !backend->vs_driver ||
		backend->vs_pause_menu || backend->vs_terminal ||
		(backend->vs_driver->input_context() != InputContext::gameplay &&
		 backend->vs_driver->input_context() != InputContext::interstitial) ||
		backend->vs_driver->status() != MainProcessDriverStatus::running ||
		!backend->vs_process->rollback_boundary_available()) return 0;
	auto &saved = th03_semantic_slots[static_cast<std::size_t>(slot)];
	if(!saved.process) saved.process = std::make_unique<MainProcessRollbackSnapshot>();
	if(!backend->vs_process->capture_rollback(*saved.process)) return 0;
	saved.driver = std::make_unique<MainProcessDriver>(*backend->vs_driver);
	saved.round = backend->vs_process->th03_capture_round();
	saved.has_replay = backend->replay_recorder != nullptr;
	if(saved.has_replay) saved.replay = backend->replay_recorder->th03_capture();
	saved.owner = backend->vs_process.get();
	return 1;
}
extern "C" int th03_semantic_restore(int slot) {
	auto *backend = th03_active_vs_backend;
	if(slot < 0 || slot >= 16 || !backend || !backend->vs_process || !backend->vs_driver) return 0;
	auto &saved = th03_semantic_slots[static_cast<std::size_t>(slot)];
	if(!saved.process || !saved.driver || saved.owner != backend->vs_process.get()) return 0;
	if(saved.has_replay && (!backend->replay_recorder || !backend->replay_recorder->th03_can_restore(saved.replay))) return 0;
	if(!th03_semantic_scratch) th03_semantic_scratch = std::make_unique<MainProcessRollbackSnapshot>();
	if(!backend->vs_process->restore_rollback(*saved.process, *th03_semantic_scratch)) return 0;
	backend->vs_process->th03_restore_round(*saved.round);
	if(saved.has_replay && (!backend->replay_recorder || !backend->replay_recorder->th03_restore(saved.replay))) return 0;
	*backend->vs_driver = *saved.driver;
	return 1;
}
extern "C" int th03_selection_index(int seat) {
 auto *backend=th03_active_vs_backend;
 return backend && backend->selection && seat>=0 && seat<2 ? backend->selection->local_selection()[static_cast<std::size_t>(seat)] : -1;
}
extern "C" int th03_semantic_interstitial() {
 auto *backend=th03_active_vs_backend;
 return backend && backend->vs_driver && backend->vs_driver->input_context() == InputContext::interstitial;
}
extern "C" int th03_match_speed() {
 auto *backend=th03_active_vs_backend;
 return backend && backend->vs_process ? backend->vs_process->canonical_state().round_speed : -1;
}
extern "C" int th03_semantic_round() {
 auto *backend=th03_active_vs_backend;
 return backend && backend->vs_process ? static_cast<int>(backend->vs_process->canonical_state().round_id) : -1;
}
extern "C" unsigned th03_semantic_state_hash() {
	auto *backend = th03_active_vs_backend;
	return backend && backend->vs_process ? backend->vs_process->canonical_state().state_hash : 0U;
}
extern "C" unsigned th03_semantic_replay_samples() {
	auto *backend = th03_active_vs_backend;
	auto *document = backend && backend->replay_recorder ? backend->replay_recorder->document() : nullptr;
	return document ? document->session.sample_count : 0U;
}
std::expected<detail::ClassicOpSessionProductionBackend, ClassicOpSessionError>
detail::create_classic_op_session_production_backend(''')

backend_path = root / 'src/app/classic/op/classic_op_session_backend.cpp'
backend_text = backend_path.read_text(encoding='utf-8')
if backend_text.count('\t\tvs_process.reset();') != 3:
    raise SystemExit('Unexpected VS process retirement sites')
backend_path.write_text(backend_text.replace('\t\tvs_process.reset();',
    '\t\tth03_active_vs_backend = nullptr;\n\t\tvs_process.reset();'), encoding='utf-8', newline='\n')
