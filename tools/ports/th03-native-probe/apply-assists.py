"""Apply the TH03 solo movement and position hooks to a pinned source copy."""
from pathlib import Path
import sys

root = Path(sys.argv[1])

def replace(path, old, new):
    file = root / path
    text = file.read_text(encoding='utf-8')
    if text.count(old) != 1:
        raise SystemExit(f'Unexpected source at {path}')
    file.write_text(text.replace(old, new), encoding='utf-8', newline='\n')

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
int th03_netplay_boot = 0;
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
replace('src/app/classic/op/classic_op_scene.cpp',
        '#include "classic/op/classic_op_selection_internal.hpp"',
        '#include "classic/op/classic_op_selection_internal.hpp"\nextern "C" int th03_netplay_boot;')
replace('src/app/classic/op/classic_op_scene.cpp',
        'ClassicOpTitleMenuScene::advance(ClassicOpInput input)\n{',
        '''ClassicOpTitleMenuScene::advance(ClassicOpInput input)
{
\tif(th03_netplay_boot > 0) {
\t\tinput = {};
\t\tif(phase_ == ClassicOpPhase::main_menu && th03_netplay_boot <= 4) {
\t\t\tif(th03_netplay_boot == 2) input.down = true;
\t\t\tif(th03_netplay_boot == 4) input.shot = true;
\t\t\tinput.raw_any = input.down || input.shot;
\t\t\t++th03_netplay_boot;
\t\t} else if(phase_ == ClassicOpPhase::vs_menu && th03_netplay_boot >= 5 && th03_netplay_boot <= 8) {
\t\t\tif(th03_netplay_boot == 6) input.down = true;
\t\t\tif(th03_netplay_boot == 8) input.shot = true;
\t\t\tinput.raw_any = input.down || input.shot;
\t\t\tif(++th03_netplay_boot == 9) th03_netplay_boot = 0;
\t\t} else if(th03_netplay_boot == 1) {
\t\t\tinput.shot = true;
\t\t\tinput.raw_any = true;
\t\t}
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

# Prototype a local-only semantic snapshot boundary for an active VS process.
# Network and audio-device state are deliberately outside this snapshot.
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
        "\tif(th03_semantic_speculative && stepped->status != StepStatus::running) { th03_semantic_boundary = 1; return MainProcessDriverTick{}; }\n\tconst bool deferred = operations_.deferred_frame_pending &&\n\t\toperations_.deferred_frame_pending(operations_.context);")
replace('src/app/classic/op/classic_op_session_backend.cpp',
        'struct ClassicOpSessionBackend final {',
        'extern "C" { int th03_match_winner = -1; unsigned th03_match_serial = 0; }\nstruct ClassicOpSessionBackend final {')
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
static std::array<Th03SemanticSnapshot, 8> th03_semantic_slots;
static std::unique_ptr<MainProcessRollbackSnapshot> th03_semantic_scratch;
extern "C" int th03_semantic_capture(int slot) {
	auto *backend = th03_active_vs_backend;
	if(slot < 0 || slot >= 8 || !backend || !backend->vs_process || !backend->vs_driver ||
		backend->vs_pause_menu || backend->vs_terminal ||
		backend->vs_driver->input_context() != InputContext::gameplay ||
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
	if(slot < 0 || slot >= 8 || !backend || !backend->vs_process || !backend->vs_driver) return 0;
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
