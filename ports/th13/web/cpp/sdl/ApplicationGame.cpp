// 对局期平台服务（th15 ApplicationGame.cpp 裁剪：life_hud/bomb_hud/game_over
// 保留，battle/敌机/符卡回调随分期接入）。
#include "ApplicationState.hpp"
#include "../game/AnmText.hpp"

#include <array>
#include <algorithm>
#include <cstdio>
#include <ctime>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
EM_JS(bool, th13_download_recorded_replay, (const unsigned char* bytes, int size, int slot), {
    try {
        const blob = new Blob([HEAPU8.slice(bytes, bytes + size)], {type: 'application/octet-stream'});
        const url = URL.createObjectURL(blob);
        const link = document.createElement('a'); link.href = url;
        link.download = 'th13_' + String(slot).padStart(2, '0') + '.rpy';
        link.click(); setTimeout(() => URL.revokeObjectURL(url), 1000); return true;
    } catch (_) { return false; }
});
#endif

namespace th13::sdl {

bool ApplicationState::record_spell_event(const SpellEvent& event) {
    // 00413270/00413a60 exclude replay sessions, for both columns and aggregate.
    if (replay_active || !event.emitted()) return true;
    const bool practice = (progress.run_mode_flags & 0x30u) == 0x20u;
    bool changed = false;
    if (event.kind == SpellEventKind::Begin)
        changed = files.record_spell_attempt(progress.character, event.state.id,
                                               event.state.name, practice);
    else if (event.captured())
        changed = files.record_spell_capture(progress.character, event.state.id, practice);
    else
        return true;
    return (changed && files.save()) || fail(files.error.empty() ?
                                                "Invalid spell record event" : files.error);
}

bool ApplicationState::record_spell_practice_score(i32 score) {
    if (replay_active) return true;
    return (files.record_spell_practice_score(progress.character, progress.selected_spell_id,
                                               score) && files.save()) || fail(files.error);
}

bool ApplicationState::record_stage_clear() {
    // 0042cd40 only increments ordinary all-clear counters on stage6/Extra.
    if (replay_active || progress.practice_mode != 0 ||
        (progress.run_mode_flags & 0x30u) != 0 ||
        (progress.stage != 6 && progress.stage != 7)) return true;
    if (!files.record_run_clear(progress.character, progress.difficulty,
                                 progress.continues == 0) || !files.save())
        return fail(files.error.empty() ? "Invalid clear record" : files.error);
    ordinary_records_dirty = false;
    return true;
}

void ApplicationState::spell_counts(i32 id, i32& captures, i32& attempts) const {
    const auto* counts = files.spell_counters(progress.character, id);
    const size_t mode = (progress.run_mode_flags & 0x30u) == 0x20u ? 1 : 0;
    captures = counts ? counts->captures[mode] : 0;
    attempts = counts ? counts->attempts[mode] : 0;
}

namespace {
// DialogueText 的设备边界：ANM bank 0 text.anm 的 texture0 + changed 上传，
// 与 th15 FontDevice::text 走同一个 graphics.texture/pixels/changed 生产链。
struct DeviceDialogueSink final : DialogueTextureSink {
    explicit DeviceDialogueSink(GraphicsDevice& device) : graphics(device) {}
    GraphicsDevice& graphics;
    u32 texture(const AnmResource& resource, u32 index) override {
        return graphics.texture(resource, index);
    }
    TextureImage* pixels(u32 handle) override { return graphics.pixels(handle); }
    void changed(u32 handle, const std::array<u32, 4>& region) override {
        graphics.changed(handle, region);
    }
};
}  // namespace

bool ApplicationState::rasterize_spell_name(SpellPresentation& presentation) {
    DeviceDialogueSink sink{graphics};
    return presentation.rasterize_name(sink,
        [this](TextureImage& image, i32 x, i32 y, u32 color, const std::string& text) {
            return fonts.draw_text(image, x, y, 0, color, text);
        }) || fail(presentation.error.empty() ? fonts.error : presentation.error);
}

bool ApplicationState::rasterize_title_text(AnmVm& vm, u32 color,
                                            const std::string& text, AnmTextAlignment alignment) {
    DeviceDialogueSink sink{graphics};
    std::string error;
    return rasterize_anm_text(vm, sink,
        [this](TextureImage& image, i32 x, i32 y, u32 ink, const std::string& cp932) {
            return fonts.draw_text(image, x, y, 0, ink, cp932);
        }, color, text, alignment, 17, &error) ||
        fail(error.empty() ? fonts.error : error);
}

bool ApplicationState::dialogue_line(i32 row, const MsgLine& line, float box_x,
                                     float box_y) {
    DeviceDialogueSink sink{graphics};
    // 0x428ae0 case 0x11（429198-4291bb 反汇编）：正文样式 = (+0x178>>1)&1，
    // 即样式 0/1 = CreateFontA(0x20,400/600) 32px（0x45ac90/0x45aef0 字体表）；
    // 样式 4/5（40px）只用于 case 0xf/0x10 宣言大字。ruby 保持样式 2=15px/700。
    // FontDevice 槽位映射：样式0→槽0(32/400)、样式1→槽2(32/600)、样式2→槽4。
    const i32 body_profile = (line.text_attr & 2u) ? 2 : 0;
    const DialogueRasterizer raster = [this, body_profile](TextureImage& image, i32 x, i32 y, bool ruby,
                                              u32 color, const std::string& text) {
        // 004282b0 sets VM+0x1c bit0x1000:0046e810 selects the
        // plain0045b1b0 path, not the outlined0045aef0 text path.
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
    if (!dialogue_text.set_anchor_y(animations, y, error))
        return fail("Dialogue anchor: " + error);
    return true;
}

void ApplicationState::dialogue_clear() {
    DeviceDialogueSink sink{graphics};
    std::string error;
    if (!dialogue_text.clear(sink, error)) fail("Dialogue text clear: " + error);
}

void ApplicationState::dialogue_end() { dialogue_text.release(animations); }

bool ApplicationState::life_hud(i32 stock, i32 pieces) {
    return run && run->hud.life(stock, pieces) || fail(run ? run->hud.error : "Life HUD has no scene");
}

bool ApplicationState::bomb_hud(i32 stock, i32 pieces) {
    return run && run->hud.bombs(stock, pieces) ||
           fail(run ? run->hud.error : "Bomb HUD has no scene");
}

bool ApplicationState::game_over() {
    return pause && pause->open(PauseEntrance::GameOver) ||
           fail(pause ? pause->error : "Game-over menu has no owner");
}

bool ApplicationState::request_pause() {
    return pause && pause->open(PauseEntrance::Pause) || fail(pause ? pause->error : "Pause has no owner");
}

bool ApplicationState::request_game_over() {
    return game_over();
}

bool ApplicationState::request_player_game_over() {
    // 00443240: exhausted lives in Replay (manager+10 == 1) call43e5a0;
    // ordinary deaths call43f570. Spell timeout calls43f570 separately.
    return replay_active ? request_practice_end() : request_game_over();
}

bool ApplicationState::hud_sound(i32 id) { return sound(id); }

bool ApplicationState::suspend_music() {
    audio_device.pause_music(true);
    return true;
}

bool ApplicationState::resume_music() {
    audio_device.pause_music(false);
    return true;
}

bool ApplicationState::request_practice_end() {
    return pause && pause->open(PauseEntrance::PracticeEnd) ||
           fail(pause ? pause->error : "Practice end has no overlay");
}

bool ApplicationState::request_extra_results() {
    return pause && pause->open(PauseEntrance::ExtraEnd) ||
           fail(pause ? pause->error : "Extra results have no overlay");
}

bool ApplicationState::restart_run() {
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
    // 43fa90 state 0x15, cursor 0: continue in place, not a new run.
    if (!run || progress.continue_stock <= 0) return fail("Continue unavailable");
    if (progress.stage == 7) return restart_run();
    selection_player.extra_lives = 2;
    selection_player.life_pieces = 0;
    selection_player.bombs = 2;
    selection_player.bomb_pieces = 0;
    progress.ending_points = 200;
    selection_player.power = selection_player.max_power < 0 ? selection_player.max_power :
        (selection_player.power_step > 0 ? selection_player.power_step : 0);
    // 4406f3 passes four power steps to 43d210, which caps at max_power.
    if (selection_player.power < selection_player.max_power)
        selection_player.power = std::min(selection_player.max_power,
            wrapping_add(selection_player.power, wrapping_mul(selection_player.power_step, 4)));
    progress.continues = std::min(progress.continues + 1, 9);
    --progress.continue_stock;
    selection_score.score = 0;
    if (!run->battle.continue_player()) return fail("Player continuation failed");
    if (!run->hud.life(selection_player.extra_lives, selection_player.life_pieces) ||
        !run->hud.bombs(selection_player.bombs, selection_player.bomb_pieces) ||
        !run->hud.update_trance()) return fail(run->hud.error);
    return true;
}

bool ApplicationState::can_save_replay() const {
    return !replay_active && recorder.ready();
}

bool ApplicationState::save_replay() {
    return save_replay_slot(1, "PLAYER");
}

bool ApplicationState::save_replay_slot(i32 slot, const std::string& name) {
    if (slot < 1 || slot > 25) return fail("Invalid replay slot");
    std::vector<u8> bytes;
    if (!recorder.encode(bytes, name, selection_score.score))
        return fail("Replay recording unavailable");
#ifdef __EMSCRIPTEN__
    if (!th13_download_recorded_replay(bytes.data(), int(bytes.size()), slot))
        return fail("Replay download failed");
#else
    char filename[32]; std::snprintf(filename,sizeof filename,"th13_%02d.rpy",slot);
    FILE* file = std::fopen(filename, "wbx");
    if (!file) return fail("Replay file open failed");
    const bool written = std::fwrite(bytes.data(), 1, bytes.size(), file) == bytes.size();
    const bool closed = std::fclose(file) == 0;
    if (!written || !closed) return fail("Replay file write failed");
#endif
    return true;
}

bool ApplicationState::can_open_pause_options() const { return true; }
bool ApplicationState::open_pause_options() { return true; }
i32 ApplicationState::pause_option_value(i32 row) const {
    return row == 0 ? files.bgm_volume : files.effect_volume;
}
bool ApplicationState::change_pause_option(i32 row, i32 delta) {
    if (row < 0 || row > 1) return false;
    i32& value = row == 0 ? files.bgm_volume : files.effect_volume;
    value = std::max(0,std::min(100,value+delta));
    apply_volumes(files.bgm_volume,files.effect_volume);
    return files.save() || fail(files.error);
}

bool ApplicationState::record_result_name(const std::string& name) {
    if (replay_active || progress.practice_mode != 0) return true;
    TitlePlayerDataScore result;
    result.score=selection_score.score; result.stage=progress.stage;
    result.continues=progress.continues; result.name=name;
    result.timestamp=i64(std::time(nullptr)); result.progress_percent=0; result.available=true;
    files.record_high_score(progress.character, progress.difficulty, result);
    return files.save_score() || fail(files.error);
}

bool ApplicationState::result_needs_name() const {
    if (replay_active || progress.practice_mode != 0 ||
        (progress.run_mode_flags & 0x30u) != 0 || progress.character < 0 ||
        progress.character >= kSaveCharacters || progress.difficulty < 0 ||
        progress.difficulty >= kSaveDifficulties || selection_score.score < 0) return false;
    const auto& ranking = files.ordinary_records[size_t(progress.character)].ranking[size_t(progress.difficulty)];
    for (const auto& entry : ranking) if (entry.score <= selection_score.score) return true;
    return false;
}

}  // namespace th13::sdl
