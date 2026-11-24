#include "../game/AnmText.hpp"
#include "../game/ResultRecord.hpp"
#include "../game/RunScene.hpp"
#include "../game/SpellElapsedTime.hpp"
#include "../game/SpellPresentation.hpp"
#include "../multiplayer/Session.hpp"
#include "ApplicationState.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <ctime>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// clang-format off
EM_JS(bool, th13_download_recorded_replay, (const unsigned char* bytes, int size, int slot), {
    try {
        const name = 'th13_' + String(slot).padStart(2, '0') + '.rpy';
        const data = HEAPU8.slice(bytes, bytes + size);
        try { Module.onReplaySaved?.(name, data); }
        catch (error) { console.warn('[th13] onReplaySaved failed:', error); }
        const blob = new Blob([data], {type: 'application/octet-stream'});
        const url = URL.createObjectURL(blob);
        const link = document.createElement('a'); link.href = url;
        link.download = name;
        link.click(); setTimeout(() => URL.revokeObjectURL(url), 1000); return true;
    } catch (_) { return false; }
});
// clang-format on
#endif

namespace th13::sdl {

bool ApplicationState::record_spell_event(const SpellEvent& event) {
    if (replay_active || !event.emitted()) return true;
    if (event.kind != SpellEventKind::Begin && !event.captured()) return true;
    PendingFileEvent entry;
    entry.kind = PendingFileEvent::Kind::SpellEvent;
    entry.capture_frame = recorder.capture_frame;
    entry.spell_event = event;
    entry.spell_practice = (progress.run_mode_flags & 0x30u) == 0x20u;
    entry.character = progress.character;
    pending_file_events_.push_back(entry);
    return true;
}

void ApplicationState::update_spell_time(bool active) {
    const auto now = std::chrono::steady_clock::now();
    if (spell_time_stage != progress.stage) {
        spell_time_stage = progress.stage;
        spell_time_clock.reset();
        spell_time_packed = 0;
    }
    if (active) {
        spell_time_clock.begin(now);
        return;
    }
    SpellTimeClock::End end;
    if (!spell_time_clock.end(now, end)) return;
    spell_time_packed = encode_spell_elapsed_time(end.seconds);
    if (spell_time_pin_enabled_ && progress.stage >= 1 && progress.stage <= 7 && end.index >= 0 && end.index < 21) {
        spell_time_packed = spell_time_pin_[u32(progress.stage)][u32(end.index)];
    }
    if (replay_active || pending_replay_stage) {
        const ReplayStage* stage = pending_replay_stage;
        if (!stage) {
            const i32 index = ReplaySession::stage_index(replay_file, progress.stage);
            if (index >= 0) stage = &replay_file.stages[u32(index)];
        }
        const size_t offset = 0x70 + size_t(end.index) * 4;
        spell_time_packed = i32(0x6ad1584);
        if (stage && offset + 4 <= stage->header.size()) {
            i32 stored = 0;
            std::memcpy(&stored, stage->header.data() + offset, 4);
            if (spell_time_packed_valid(stored)) spell_time_packed = stored;
        }
    } else if (recorder.ready() && !recorder.record_spell_time(u32(end.index), spell_time_packed)) {
        fail("Replay spell time array unavailable");
    }
}

bool ApplicationState::record_spell_practice_score(i32 score) {
    if (replay_active) return true;
    PendingFileEvent entry;
    entry.kind = PendingFileEvent::Kind::SpellPracticeScore;
    entry.capture_frame = recorder.capture_frame;
    entry.character = progress.character;
    entry.spell_id = progress.selected_spell_id;
    entry.score = score;
    pending_file_events_.push_back(entry);
    return true;
}

bool ApplicationState::record_stage_clear() {
    if (replay_active || progress.practice_mode != 0 || (progress.run_mode_flags & 0x30u) != 0 ||
        (progress.stage != 6 && progress.stage != 7))
        return true;
    PendingFileEvent entry;
    entry.kind = PendingFileEvent::Kind::StageClear;
    entry.capture_frame = recorder.capture_frame;
    entry.character = progress.character;
    entry.difficulty = progress.difficulty;
    entry.no_continue = progress.continues == 0;
    pending_file_events_.push_back(entry);
    return true;
}

void ApplicationState::spell_counts(i32 id, i32& captures, i32& attempts) const {
    const auto* counts = files.spell_counters(progress.character, id);
    const size_t mode = (progress.run_mode_flags & 0x30u) == 0x20u ? 1 : 0;
    captures = counts ? counts->captures[mode] : 0;
    attempts = counts ? counts->attempts[mode] : 0;
}

namespace {
struct DeviceDialogueSink final : DialogueTextureSink {
    explicit DeviceDialogueSink(GraphicsDevice& device) : graphics(device) {}
    GraphicsDevice& graphics;
    u32 texture(const AnmResource& resource, u32 index) override { return graphics.texture(resource, index); }
    TextureImage* pixels(u32 handle) override { return graphics.pixels(handle); }
    void changed(u32 handle, const std::array<u32, 4>& region) override { graphics.changed(handle, region); }
};
}

bool ApplicationState::rasterize_spell_name(SpellPresentation& presentation) {
    DeviceDialogueSink sink{graphics};
    return presentation.rasterize_name(sink, [this](TextureImage& image, i32 x, i32 y, u32 color,
                                                    const std::string& text) {
        return fonts.draw_text(image, x, y, 0, color, text);
    }) || fail(presentation.error.empty() ? fonts.error : presentation.error);
}

bool ApplicationState::rasterize_title_text(AnmVm& vm, u32 color, const std::string& text, AnmTextAlignment alignment) {
    DeviceDialogueSink sink{graphics};
    std::string error;
    return rasterize_anm_text(
               vm, sink,
               [this](TextureImage& image, i32 x, i32 y, u32 ink, const std::string& cp932) {
                   return fonts.draw_text(image, x, y, 0, ink, cp932);
               },
               color, text, alignment, 17, &error) ||
           fail(error.empty() ? fonts.error : error);
}

bool ApplicationState::dialogue_begin() {
    DeviceDialogueSink sink{graphics};
    std::string error;
    if (!dialogue_text.begin(animations, sink, error)) return fail("Dialogue text begin: " + error);
    return true;
}

std::function<bool()> ApplicationState::capture_dialogue_rollback() {
    DeviceDialogueSink sink{graphics};
    const auto state = dialogue_text.capture_rollback(animations, sink);
    return [this, state] {
        DeviceDialogueSink sink{graphics};
        std::string error;
        return dialogue_text.restore_rollback(state, animations, sink, error) || fail(error);
    };
}

u64 ApplicationState::digest_dialogue_rollback() {
    DeviceDialogueSink sink{graphics};
    return multiplayer::WorldState::digest_dialogue(dialogue_text.capture_rollback(animations, sink));
}

bool ApplicationState::dialogue_line(i32 row, const MsgLine& line, float box_x, float box_y) {
    DeviceDialogueSink sink{graphics};
    const i32 body_profile = (line.text_attr & 2u) ? 2 : 0;
    const DialogueRasterizer raster = [this, body_profile](TextureImage& image, i32 x, i32 y, bool ruby, u32 color,
                                                           const std::string& text) {
        return fonts.draw_text(image, x, y, ruby ? 4 : body_profile, color, text);
    };
    std::string error;
    if (!dialogue_text.submit(animations, sink, row, line, box_x, box_y, raster, error))
        return fail("Dialogue text submission: " + error);
    return true;
}

bool ApplicationState::dialogue_fade(i32 label) {
    std::string error;
    if (!dialogue_text.fade(label, error)) return fail("Dialogue text fade: " + error);
    return true;
}

bool ApplicationState::dialogue_anchor_y(float y) {
    std::string error;
    if (!dialogue_text.set_anchor_y(animations, y, error)) return fail("Dialogue anchor: " + error);
    return true;
}

void ApplicationState::dialogue_clear() {
    DeviceDialogueSink sink{graphics};
    std::string error;
    if (!dialogue_text.clear(sink, error)) fail("Dialogue text clear: " + error);
}

void ApplicationState::dialogue_end() { dialogue_text.release(animations); }

bool ApplicationState::life_hud(i32 stock, i32 pieces) {
    return (run && run->hud.life(stock, pieces)) || fail(run ? run->hud.error : "Life HUD has no scene");
}

bool ApplicationState::bomb_hud(i32 stock, i32 pieces) {
    return (run && run->hud.bombs(stock, pieces)) || fail(run ? run->hud.error : "Bomb HUD has no scene");
}

bool ApplicationState::life_hud_seat(i32 seat, i32 lives, i32 pieces) {
    if (!run) return fail("Life HUD has no scene");
    if (run->hud.mp_seat_count >= 2) {
        const HudPlayerState* seat_state = run->battle.seat_hud(seat);
        if (!seat_state) return fail("Multiplayer seat HUD state missing");
        return run->hud.life_seat(seat, lives, pieces, seat_state->life_piece_threshold_index) || fail(run->hud.error);
    }
    return life_hud(lives, pieces);
}

bool ApplicationState::bomb_hud_seat(i32 seat, i32 stock, i32 pieces) {
    if (!run) return fail("Bomb HUD has no scene");
    if (seat > 0 && run->hud.mp_seat_count >= 2)
        return run->hud.bombs_seat(seat, stock, pieces) || fail(run->hud.error);
    return bomb_hud(stock, pieces);
}

bool ApplicationState::game_over() {
    if (mp_seat_count >= 2) {
        game_over_music.clear();
        game_over_music_cursor = 0;
    } else {
        game_over_music = audio_device.music_filename();
        game_over_music_cursor = audio_device.statistics().music_cursor;
    }
    const bool results_menu = mp_seat_count >= 2 || (progress.run_mode_flags & 0x30u) != 0;
    if (!pause || !pause->open(results_menu ? PauseEntrance::BGameOver : PauseEntrance::GameOver))
        return fail(pause ? pause->error : "Game-over menu has no owner");
    return audio_device.queue_music(2, -1, "th128_08.wav") || fail(audio_device.error);
}

bool ApplicationState::request_pause() {
    return (pause && pause->open(PauseEntrance::Pause)) || fail(pause ? pause->error : "Pause has no owner");
}

bool ApplicationState::request_game_over() { return game_over(); }

bool ApplicationState::request_player_game_over() {
    return replay_active ? request_practice_end() : request_game_over();
}

bool ApplicationState::hud_sound(i32 id) { return sound(id); }
bool ApplicationState::hud_sound(i32 id, float x) { return audio_device.enqueue(id, x) || fail(audio_device.error); }
bool ApplicationState::stop_sound(i32 id) { return audio_device.stop(id) || fail(audio_device.error); }
bool ApplicationState::set_voice_pan(i32 id, float x) {
    if (!audio_device.observe_voice_pan(id, x)) return fail(audio_device.error);
    if (!audio_device.defer_output) pending_voice_pans_.push_back({recorder.capture_frame, id, x});
    return true;
}

bool ApplicationState::hud_notice(i32 front_script) {
    if (!run) return true;
    return run->show_notice(front_script);
}
bool ApplicationState::hud_spirit_notice(i32 level, float x, float y, float z) {
    if (!run) return true;
    return run->show_spirit_notice(level, x, y, z);
}

bool ApplicationState::suspend_music() {
    audio_device.pause_music(true);
    return true;
}

bool ApplicationState::resume_music() {
    audio_device.pause_music(false);
    return true;
}

bool ApplicationState::request_practice_end() {
    return (pause && pause->open(PauseEntrance::PracticeEnd)) ||
           fail(pause ? pause->error : "Practice end has no overlay");
}

bool ApplicationState::request_extra_results() {
    return (pause && pause->open(PauseEntrance::ExtraEnd)) ||
           fail(pause ? pause->error : "Extra results have no overlay");
}

bool ApplicationState::restart_run() {
    if (multiplayer::session.Active() && !multiplayer::session.Simulating()) return return_to_title();
    stage_transition_loading = false;
    stage_transition_active = false;
    stage_startup_age = -1;
    pending_replay_stage = nullptr;
    pending_replay_stage_index = 0;
    environment.paused = false;
    if (replay_active) {
        const i32 stage_index = ReplaySession::stage_index(replay_file, progress.stage);
        if (stage_index < 0) return fail("Replay restart stage missing");
        pending_replay_stage_index = u32(stage_index);
        pending_destination = i32(Destination::Run);
        return true;
    }
    return start_game(progress.starting_stage);
}

bool ApplicationState::return_to_title() {
    pending_destination = i32(Destination::Title);
    return true;
}

bool ApplicationState::continue_run() {
    if (!run || progress.continue_stock <= 0) return fail("Continue unavailable");
    if (progress.stage == 7) return restart_run();
    if (!game_over_music.empty() &&
        (!audio_device.queue_music(2, -1, game_over_music) || !audio_device.finish_music_requests() ||
         !audio_device.seek_music_frames(game_over_music_cursor)))
        return fail(audio_device.error);
    selection_player.extra_lives = 2;
    selection_player.life_pieces = 0;
    selection_player.bombs = 2;
    selection_player.bomb_pieces = 0;
    progress.ending_points = 200;
    selection_player.power = selection_player.max_power < 0
                                 ? selection_player.max_power
                                 : (selection_player.power_step > 0 ? selection_player.power_step : 0);
    if (selection_player.power < selection_player.max_power)
        selection_player.power =
            std::min(selection_player.max_power,
                     wrapping_add(selection_player.power, wrapping_mul(selection_player.power_step, 4)));
    progress.continues = std::min(progress.continues + 1, 9);
    --progress.continue_stock;
    selection_score.score = 0;
    if (!run->battle.continue_player()) return fail("Player continuation failed");
    if (!run->hud.life(selection_player.extra_lives, selection_player.life_pieces) ||
        !run->hud.bombs(selection_player.bombs, selection_player.bomb_pieces) || !run->hud.trance_label_refresh())
        return fail(run->hud.error);
    return true;
}

bool ApplicationState::can_save_replay() const {
    if (multiplayer_replay_available) return multiplayer_replay_available();
    return mp_seat_count < 2 && !replay_active && recorder.ready();
}

bool ApplicationState::result_replay_save_available() const {
    if (multiplayer_replay_available) return multiplayer_replay_available();
    return mp_seat_count < 2 && !replay_active && recorder.ready() && progress.continues == 0;
}

bool ApplicationState::result_ranking_available() const { return mp_seat_count < 2; }

bool ApplicationState::result_screen_data(TitleResultScreenData& out) const {
    if (progress.character < 0 || progress.character >= kSaveCharacters || progress.difficulty < 0 ||
        progress.difficulty >= kSaveDifficulties)
        return false;
    out = {};
    out.character = progress.character;
    out.difficulty = progress.difficulty;
    out.rank = -1;
    out.current.score = selection_score.score;
    const bool clear_result = !run && title &&
                              (title->state.return_reason == 3 || title->state.screen == TitleScreen::ResultName ||
                               title->state.screen == TitleScreen::ResultReplaySlot);
    out.current.stage = result_record_stage(clear_result, extra_results_open(), progress.stage);
    out.current.continues = progress.continues;
    out.current.timestamp = i64(std::time(nullptr));
    out.current.available = true;
    out.current.progress_percent = result_slowdown_percent(slow_actual_, slow_budget_);
    const auto& ranking = files.ordinary_records[size_t(progress.character)].ranking[size_t(progress.difficulty)];
    for (size_t i = 0; i < out.records.size(); ++i)
        out.records[i] = ranking[i];
    if (mp_seat_count < 2 && !replay_active && progress.practice_mode == 0 && (progress.run_mode_flags & 0x30u) == 0 &&
        selection_score.score >= 0) {
        for (size_t i = 0; i < out.records.size(); ++i) {
            if (out.records[i].score > selection_score.score) continue;
            out.rank = i32(i);
            for (size_t next = out.records.size() - 1; next > i; --next)
                out.records[next] = out.records[next - 1];
            out.records[i] = out.current;
            break;
        }
    }
    return true;
}

bool ApplicationState::dialogue_visible(bool visible) {
    if (run && !run->set_dialogue_visible(visible))
        return fail(run->error.empty() ? "Dialogue visibility failed" : run->error);
    std::string error;
    if (!dialogue_text.set_visible(animations, visible, error)) return fail(error);
    return true;
}

bool ApplicationState::save_replay() { return save_replay_slot(1, "PLAYER"); }

bool ApplicationState::save_replay_slot(i32 slot, const std::string& name) {
    return save_replay_slot(slot, name, nullptr);
}

bool ApplicationState::save_replay_slot(i32 slot, const std::string& name, const ReplaySaveMeta* pinned) {
    if (slot < 1 || slot > 25) return fail("Invalid replay slot");
    if (multiplayer_replay_save) {
        if (!multiplayer_replay_save(slot, name)) return fail("Multiplayer replay save failed");
        files.remember_name(name);
        return true;
    }
    if (mp_seat_count >= 2) return fail("Multiplayer replay save unavailable");
    std::vector<u8> bytes;
    if (pinned) {
        if (!recorder.encode(bytes, name, selection_score.score, *pinned)) return fail("Replay recording unavailable");
    } else {
        ReplaySaveMeta meta;
        meta.slow_numerator = slow_num_;
        meta.slow_denominator = slow_den_;
        if (!recorder.encode(bytes, name, selection_score.score, meta)) return fail("Replay recording unavailable");
    }
    char catalog_name[32];
    std::snprintf(catalog_name, sizeof catalog_name, "th13_%02d.rpy", slot);
    if (!import_replay_bytes(bytes.data(), bytes.size(), catalog_name))
        return fail(replay_import_error_.empty() ? std::string("Replay catalog import failed") : replay_import_error_);
#ifdef __EMSCRIPTEN__
    if (!th13_download_recorded_replay(bytes.data(), int(bytes.size()), slot)) return fail("Replay download failed");
#else
    char filename[32];
    std::snprintf(filename, sizeof filename, "th13_%02d.rpy", slot);
    FILE* file = std::fopen(filename, "wbx");
    if (!file) return fail("Replay file open failed");
    const bool written = std::fwrite(bytes.data(), 1, bytes.size(), file) == bytes.size();
    const bool closed = std::fclose(file) == 0;
    if (!written || !closed) return fail("Replay file write failed");
#endif
    files.remember_name(name);
    return files.save() || fail(files.error);
}

bool ApplicationState::can_open_pause_options() const { return true; }
bool ApplicationState::open_pause_options() { return true; }
i32 ApplicationState::pause_option_value(i32 row) const { return row == 0 ? files.bgm_volume : files.effect_volume; }
bool ApplicationState::change_pause_option(i32 row, i32 delta) {
    if (row < 0 || row > 1) return false;
    PendingFileEvent entry;
    entry.kind = PendingFileEvent::Kind::PauseOption;
    entry.capture_frame = recorder.capture_frame;
    entry.row = row;
    entry.delta = delta;
    pending_file_events_.push_back(entry);
    return true;
}

bool ApplicationState::record_result_entry(const std::string& name, i32 stage) {
    if (replay_active || progress.practice_mode != 0) return true;
    TitlePlayerDataScore result;
    result.score = selection_score.score;
    result.stage = stage;
    result.continues = progress.continues;
    result.name = name;
    result.timestamp = i64(std::time(nullptr));
    result.progress_percent = result_slowdown_percent(slow_actual_, slow_budget_);
    result.available = true;
    PendingFileEvent entry;
    entry.kind = PendingFileEvent::Kind::ResultEntry;
    entry.capture_frame = recorder.capture_frame;
    entry.character = progress.character;
    entry.difficulty = progress.difficulty;
    entry.result = result;
    pending_file_events_.push_back(entry);
    return true;
}

bool ApplicationState::record_result_name(const std::string& name) {
    return record_result_entry(name, result_record_stage(false, extra_results_open(), progress.stage));
}

bool ApplicationState::extra_results_open() const {
    return run && pause && pause->state.screen == PauseScreen::ExtraEnd;
}

std::string ApplicationState::last_result_name() const { return files.last_name; }

bool ApplicationState::record_clear_result_name(const std::string& name) { return record_result_entry(name, 8); }

bool ApplicationState::result_needs_name() const {
    if (mp_seat_count >= 2 || replay_active || progress.practice_mode != 0 || (progress.run_mode_flags & 0x30u) != 0 ||
        progress.character < 0 || progress.character >= kSaveCharacters || progress.difficulty < 0 ||
        progress.difficulty >= kSaveDifficulties || selection_score.score < 0)
        return false;
    const auto& ranking = files.ordinary_records[size_t(progress.character)].ranking[size_t(progress.difficulty)];
    for (const auto& entry : ranking)
        if (entry.score <= selection_score.score) return true;
    return false;
}

i32 ApplicationState::result_ranking(TitlePlayerDataScore rows[10]) const {
    TitleResultScreenData data;
    if (!result_screen_data(data) || data.rank < 0) return -1;
    for (size_t i = 0; i < data.records.size(); ++i)
        rows[i] = data.records[i];
    return data.rank;
}

}
