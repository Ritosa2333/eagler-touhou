// 对局期平台服务（th15 ApplicationGame.cpp 裁剪：life_hud/bomb_hud/game_over
// 保留，battle/敌机/符卡回调随分期接入）。
#include "ApplicationState.hpp"
#include "../game/AnmText.hpp"
#include "../game/RunScene.hpp"
#include "../game/SpellPresentation.hpp"
#include "../game/SpellElapsedTime.hpp"

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

void ApplicationState::update_spell_time(bool active) {
    const auto now = std::chrono::steady_clock::now();
    if (spell_time_stage != progress.stage) {
        spell_time_stage = progress.stage;
        spell_time_clock.reset();
        spell_time_packed = 0;
    }
    // 00412ee0 edges are latched at the 00413270/00413a60 instruction, not by
    // sampling manager+0x7c bit0 at Update 26: a same-tick Begin+End still
    // ends once and advances manager+0x8c, keeping replay slots aligned.
    if (active) {
        spell_time_clock.begin(now);
        return;
    }
    SpellTimeClock::End end;
    if (!spell_time_clock.end(now, end)) return;
    spell_time_packed = encode_spell_elapsed_time(end.seconds);
    if (replay_active || pending_replay_stage) {
        const ReplayStage* stage = pending_replay_stage;
        if (!stage) {
            const i32 index = ReplaySession::stage_index(replay_file, progress.stage);
            if (index >= 0) stage = &replay_file.stages[u32(index)];
        }
        const size_t offset = 0x70 + size_t(end.index) * 4;
        // 0x41302c-0x413041: read the stage slot, validate with 0x412760 and
        // store the 0x6ad1584 sentinel when invalid. 0x427910 decodes that
        // sentinel to 999.99, the same failed-decode display as native.
        spell_time_packed = i32(0x6ad1584);
        if (stage && offset + 4 <= stage->header.size()) {
            i32 stored = 0;
            std::memcpy(&stored, stage->header.data() + offset, 4);
            if (spell_time_packed_valid(stored)) spell_time_packed = stored;
        }
    } else if (recorder.ready() &&
               !recorder.record_spell_time(u32(end.index), spell_time_packed)) {
        fail("Replay spell time array unavailable");
    }
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

bool ApplicationState::dialogue_begin() {
    // 0x4282b0 构造：open 时立即建四条 text.anm VM（尾插 + render_flags
    // 0x1000），不等首行提交。sink 只做 graphics.texture() 查询，不写像素。
    DeviceDialogueSink sink{graphics};
    std::string error;
    if (!dialogue_text.begin(animations, sink, error))
        return fail("Dialogue text begin: " + error);
    return true;
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
    // 0043f570: mode0 → 0xe (GameOver/continue, script 142); mode≠0 → 0x18
    // (BGameOver, script 147, three items, default cursor 2, no Continue).
    const bool results_menu = (progress.run_mode_flags & 0x30u) != 0;
    return pause && pause->open(results_menu ? PauseEntrance::BGameOver
                                             : PauseEntrance::GameOver) ||
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
bool ApplicationState::hud_sound(i32 id, float x) { return audio_device.enqueue(id, x) || fail(audio_device.error); }
bool ApplicationState::stop_sound(i32 id) { return audio_device.stop(id) || fail(audio_device.error); }
bool ApplicationState::set_voice_pan(i32 id, float x) { return audio_device.set_voice_pan(id, x) || fail(audio_device.error); }

// 0x42a430 = jmp [eax*4+0x42abd4] 分发器：case2（0x42ee28 小P /
// 0x43d23e 大P 满灵力）→ front61；case4（0x43d3a6，FUN_0043d330 碎片
// 达标）→ front63（EXTENDED）。宿主路径 ApplicationState::run →
// RunScene::show_notice 窄接口 → 私有 spell_presentation；空 run 或
// 资源未就绪返回 true（不制造新失败）。1UP（0x43d2f0）不发横幅，不在此列。
bool ApplicationState::hud_notice(i32 front_script) {
    if (!run) return true;
    return run->show_notice(front_script);
}
// 0x43d530：吸灵满 1 整槽 level ∈ 1..3 → front 108/109/110（×1/×2/×MAX）。
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
    return pause && pause->open(PauseEntrance::PracticeEnd) ||
           fail(pause ? pause->error : "Practice end has no overlay");
}

bool ApplicationState::request_extra_results() {
    return pause && pause->open(PauseEntrance::ExtraEnd) ||
           fail(pause ? pause->error : "Extra results have no overlay");
}

bool ApplicationState::restart_run() {
    // Retry (0xa ordinary / 0xb practice, 0x440960 state 0x1d cursor 2) is a
    // fresh run, not the pending 0xc continuation. A pause opened while the
    // 0xc transition was active must discard that pending stage and resume
    // state, or begin_run would keep environment.paused/stage_transition_active
    // and skip the replay restore.
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
    // Retry（0x43c1c0 case0xa/0xb）置 dcc30=1，与普通新局同走 0x42b990 的
    // dcc30!=0 复位分支（0x42bb5a→0x42d3c0→0x42d404 写 200）——
    // start_game 内部的 trance_reset_fresh_entry 已覆盖，不再重复调用。
    // 关间 0xc 延续（dcc30=0）不经过 start_game，灵界计跨关保留。
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
        // 0x43fa90:548-552：continue 置 be808=200 后 0x42b030 无条件刷新。
        !run->hud.trance_label_refresh()) return fail(run->hud.error);
    return true;
}

bool ApplicationState::can_save_replay() const {
    return !replay_active && recorder.ready();
}

bool ApplicationState::result_replay_save_available() const {
    // 0x454050 state3：DAT_004be7c8（续关数）==0 才进 0x4549d0（回放保存）；
    // 网页侧叠加 recorder.ready()（无录像数据时保存无意义）。
    return !replay_active && recorder.ready() && progress.continues == 0;
}

bool ApplicationState::result_screen_data(TitleResultScreenData& out) const {
    if (progress.character < 0 || progress.character >= kSaveCharacters ||
        progress.difficulty < 0 || progress.difficulty >= kSaveDifficulties)
        return false;
    out = {};
    out.character = progress.character;
    out.difficulty = progress.difficulty;
    out.rank = -1;
    out.current.score = selection_score.score;
    out.current.stage = progress.stage;
    out.current.continues = progress.continues;
    out.current.timestamp = i64(std::time(nullptr));
    out.current.available = true;
    const auto& ranking = files.ordinary_records[size_t(progress.character)]
                              .ranking[size_t(progress.difficulty)];
    for (size_t i = 0; i < out.records.size(); ++i) out.records[i] = ranking[i];
    // Preview the same ties-before-old insertion as FileStore::record_high_score,
    // without persisting a nameless score before the editor is confirmed.
    if (!replay_active && progress.practice_mode == 0 &&
        (progress.run_mode_flags & 0x30u) == 0 && selection_score.score >= 0) {
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
    // 0x43e3f0:0x43e588 → 0x429d00 / 0x43e700:0x43e749 → 0x429f10：
    // 对话对象 9 槽 VM+0x18 bit1 清/置。立绘/名牌在 RunScene 的
    // MsgPresentation；正文四行与气泡根在宿主 DialogueText。
    if (run && !run->set_dialogue_visible(visible))
        return fail(run->error.empty() ? "Dialogue visibility failed" : run->error);
    std::string error;
    if (!dialogue_text.set_visible(animations, visible, error)) return fail(error);
    return true;
}

bool ApplicationState::save_replay() {
    return save_replay_slot(1, "PLAYER");
}

bool ApplicationState::save_replay_slot(i32 slot, const std::string& name) {
    if (slot < 1 || slot > 25) return fail("Invalid replay slot");
    std::vector<u8> bytes;
    if (!recorder.encode(bytes, name, selection_score.score))
        return fail("Replay recording unavailable");
    // 0x4484d0 写 replay/th13_NN.rpy 后，原版 0x4525b0 重扫目录即可在标题
    // Replay 菜单看到；网页版没有目录重扫，这里直接走既有 import_replay_bytes
    // 入 catalog（th13_NN.rpy → 编号槽 NN-1），标题 Replay 菜单立即可选。
    char catalog_name[32];
    std::snprintf(catalog_name, sizeof catalog_name, "th13_%02d.rpy", slot);
    if (!import_replay_bytes(bytes.data(), bytes.size(), catalog_name))
        return fail(replay_import_error_.empty() ? std::string("Replay catalog import failed")
                                                 : replay_import_error_);
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

bool ApplicationState::record_result_entry(const std::string& name, i32 stage) {
    if (replay_active || progress.practice_mode != 0) return true;
    TitlePlayerDataScore result;
    result.score=selection_score.score; result.stage=stage;
    result.continues=progress.continues; result.name=name;
    result.timestamp=i64(std::time(nullptr));
    // 0x43d6b0:42-44：float@24 = 100 − 慢速率分子/分母×100（0x424990 累加
    // 器，关首 0x42b990 清零 → 末段值）；分母为 0（无有效帧）时记 0。
    result.progress_percent = slow_budget_ > 0.0
        ? float(100.0 - slow_actual_ / slow_budget_ * 100.0) : 0.0f;
    result.available=true;
    files.record_high_score(progress.character, progress.difficulty, result);
    return files.save_score() || fail(files.error);
}

bool ApplicationState::record_result_name(const std::string& name) {
    return record_result_entry(name, progress.stage);
}

bool ApplicationState::record_clear_result_name(const std::string& name) {
    return record_result_entry(name, 8);
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
