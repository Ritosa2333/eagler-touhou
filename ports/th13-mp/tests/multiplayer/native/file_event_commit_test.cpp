// m1.unit_file_event_commit —— M1/T4 存档事件队列化独立回归（F1/F1′/F1″/F2）。
//
// 被测生产面（全部 public/struct 公开态，零生产钩子；实现=ApplicationState.hpp
// PendingFileEvent 三档注释、ApplicationGame.cpp 捕获点、ApplicationTitle.cpp
// persist_options/mark_heard 捕获点、Application.cpp:1675 record_music_unlocked
// 捕获点 + :1462 commit_pending_file_events 冲账 + TitleScene.hpp:132 非常量
// option_menu() 访问器（F1″ 微改））。
//
// 断言命题（工单步骤 (a)-(d) + 风险反例逐条落点）：
//   F1(b) 吞错档 mark_heard：save 注入失败（TH13_SAVE_PATH 指向不存在目录，
//     load_v6 缺档=新档、write_save fopen 失败——真实生产失败路径，非伪造
//     设备）下 commit 不失败、仅落 files.error、静默继续；update-only 段
//     files 字段不变；双 commit 无重复结算。反例钉面：若被统一为 commit
//     fail（帧失败档），commit 返回 false 即红——MusicRoom 由静默继续变帧
//     失败 = SP 行为变更（ApplicationTitle.cpp:283-287 原 `(void)files.save()`）。
//   F1′(c) 菜单错误档 persist_options：标题 Option 真实菜单链驱动
//     （Main→Option→音量←编辑→行4 やめる→substate4→leave）到生产捕获点；
//     捕获乐观放行（update-only files/设备音量不变）；commit 期经
//     TitleScene::option_menu() 非常量访问器对 public error set-if-empty
//     补记 "Title option persistence unavailable"、帧不失败、screen 回 Main
//     照常。反例钉面：统一为 commit fail（帧失败）或静默跳过（error 空）
//     任一即红。
//   F1(a) 帧失败档六处：SpellEvent/SpellPracticeScore/StageClear/PauseOption/
//     ResultEntry/MusicUnlocked——commit fail 终态同路径（返回 false、
//     failure 非空且与原调用点文案逐字、frames++ 未达、次帧入口守卫拦停）；
//     update-only 段 files 字段/设备音量不变；失败拍内存变更 = 拆分前原
//     表达式顺序（变更先于 save，SP 终态逐位同路径）。
//   F2 save_replay_slot 家族：确认拍宿主事件原位保留——不进待确认队列、
//     同步完成（catalog 导入+落盘）、无 commit 参与；无效槽确认拍即 fail。
//   固定序截断：存档队列失败 → pan 挂起队/播放帧/audio_device.update()/
//     frames++ 全部不执行；录制槽（第一环）已冲账；捕获序截断（失败点后
//     条目保留在队列）。
//
// 捕获点调用方式：八处捕获函数是宿主平台面（TitleOptionsMenu::leave→
// persist_options、MusicRoom→mark_heard、RunScene→record_spell_event 等的
// 生产入口）；除 persist_options 走真实菜单链外，其余直接调用公开捕获函数
// = 生产捕获面本身（RunScene/PauseMenu 侧驱动属其里程碑用例）。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "game/SpellState.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/TitleOptionsMenu.hpp"
#include "game/TitleState.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using Audio = th13::sdl::AudioDevice;
using th13::sdl::PendingFileEvent;
using EventKind = PendingFileEvent::Kind;
using th13::sdl::Destination;
using th13::TitleScreen;
using th13::TitleOptionVolumes;
using th13::SpellEvent;
using th13::SpellEventKind;
using th13::kSpellCapture;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;

// save 失败注入根：不存在目录下的路径（fopen 必败、对所有用户确定性失败；
// load_v6 缺档返回 true=新档默认值，boot 不受影响）。测试顺序执行共享进程
// 环境，pin_boot 每次重设 TH13_SAVE_PATH，互不泄漏。
constexpr const char* kFailDirA = "/nonexistent-t4fe-a/save.bin";
constexpr const char* kFailDirB = "/nonexistent-t4fe-b/save.bin";
constexpr const char* kFailDirC = "/nonexistent-t4fe-c/save.bin";

// commit 期临时切换存档根（工单"临时只读存档根"探针机制）：构造即切坏路径，
// 析构恢复——boot 用可写路径、捕获后冲账前注入失败，两段语义同帧并存。
struct SavePathGuard {
    std::string previous;
    bool had = false;
    explicit SavePathGuard(const char* bad_path) {
        const char* old = std::getenv("TH13_SAVE_PATH");
        had = old != nullptr;
        if (had) previous = old;
        ::setenv("TH13_SAVE_PATH", bad_path, 1);
    }
    ~SavePathGuard() {
        if (had) ::setenv("TH13_SAVE_PATH", previous.c_str(), 1);
        else ::unsetenv("TH13_SAVE_PATH");
    }
};

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

bool pin_boot(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    if (!asset_root || !*asset_root) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    if (!app.audio_device.initialize_for_test(
            asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat")) {
        std::printf("  失败 boot: audio initialize_for_test: %s\n", app.audio_device.error.c_str());
        return false;
    }
    if (!app.initialize(false)) {
        std::printf("  失败 boot: initialize: %s\n", app.failure.c_str());
        return false;
    }
    app.visual.reseed_restore(kBootSeed);
    for (int i = 0; i < 256; ++i) (void)app.visual.step16();
    (void)app.visual.next32();
    (void)app.visual.next32();
    app.game_random.reseed(kBootSeed);
    return true;
}

bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

bool drive(App& app, th13::GameInput& input, u32 held) {
    input.update(held);
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    return app.present();
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t4fe", 0, meta);
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

SpellEvent begin_event(i32 slot, const char* name) {
    SpellEvent event;
    event.kind = SpellEventKind::Begin;
    event.state.id = slot;
    event.state.name = name;
    return event;
}

SpellEvent captured_end_event(i32 slot) {
    SpellEvent event;
    event.kind = SpellEventKind::End;
    event.state.id = slot;
    event.state.flags |= kSpellCapture;  // captured() 才入队（捕获拍纯判定）
    return event;
}

// ---- F1(b) 吞错档：mark_heard save 失败仅落 files.error、帧不失败 ----
TEST(file_event_commit_mark_heard_swallow_tier) {
    App app;
    CHECK(pin_boot(app, kFailDirA));  // 不存在目录：每次 save 必失败
    if (!app.failure.empty()) return;
    CHECK(!app.files.music_heard(3));
    const Audio::Statistics pre = app.audio_device.statistics();
    app.mark_heard(3);  // 捕获（TitleMusicRoomMenu.cpp:90 同面，void）
    // update-only 段：files 字段不变、设备统计不变、队列恰一条带帧身份。
    CHECK(!app.files.music_heard(3));
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.pending_file_events_[0].kind == EventKind::MarkHeard);
    CHECK_EQ(app.pending_file_events_[0].capture_frame, app.recorder.capture_frame);
    CHECK_EQ(app.pending_file_events_[0].track, 3);
    const Audio::Statistics mid = app.audio_device.statistics();
    CHECK_EQ(mid.requests, pre.requests);
    CHECK_EQ(mid.started, pre.started);
    // 吞错档冲账：commit 不失败（反例钉面：统一为帧失败档此处即红）。
    const u32 frames_pre = app.frames;
    CHECK(app.commit(kFps));
    CHECK(app.files.music_heard(3));        // 变更已应用
    CHECK(!app.files.error.empty());        // save 失败如实落 files.error
    CHECK(app.failure.empty());             // 帧失败面为空（原 (void)save 终态）
    CHECK_EQ(app.frames, frames_pre + 1u);  // frames 正常推进
    CHECK(app.pending_file_events_.empty());
    // 双 commit：队列空、无重复结算（heard 位不回退、不重计数）。
    CHECK(app.commit(kFps));
    CHECK(app.files.music_heard(3));
    CHECK_EQ(app.frames, frames_pre + 2u);
}

// ---- F1′(c) 菜单错误档：真实标题 Option 菜单链捕获 + commit 期补记 ----
TEST(file_event_commit_persist_options_menu_driven_capture_commit) {
    App app;
    CHECK(pin_boot(app, kFailDirB));
    if (!app.failure.empty()) return;
    CHECK(app.title != nullptr);
    CHECK(app.title->option_menu().error.empty());
    CHECK_EQ(app.files.bgm_volume, 100);
    CHECK_EQ(app.files.effect_volume, 80);
    th13::GameInput input;
    // Main 菜单可交互（age≥180+11 → substate 2；boot 标题线程真实推进）。
    int guard = 0;
    while ((app.title->state.screen != TitleScreen::Main ||
            app.title->state.substate != 2) &&
           guard++ < 600) {
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK(guard < 600);
    // 光标 → オプション（行 7；Extra 无存档禁用行 1 由 MenuCursor 跳过）。
    guard = 0;
    while (app.title->state.menu.cursor != 7 && guard++ < 20) {
        CHECK(drive(app, input, 0x20));  // ↓
        if (!app.failure.empty()) return;
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(app.title->state.menu.cursor, 7);
    CHECK(drive(app, input, 0x01));  // Z 确认
    if (!app.failure.empty()) return;
    guard = 0;
    while (app.title->state.screen != TitleScreen::Option && guard++ < 60) {
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(i32(app.title->state.screen), i32(TitleScreen::Option));
    // case0→1（>6 帧）→2：空闲 10 帧后必可交互。
    for (int f = 0; f < 10; ++f) {
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    // 行 0 BGM ←×4：菜单共享真值 100→80（refresh_volumes 只写设备）。
    for (int k = 0; k < 4; ++k) {
        CHECK(drive(app, input, 0x40));
        if (!app.failure.empty()) return;
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(app.title->option_menu().volumes_ref().bgm, 80);
    CHECK_EQ(app.files.bgm_volume, 100);  // 捕获前 files 音量未变（编辑不入档）
    // X：行≠4 → 光标跳行 4（0x44dd00 case2 同拍）。update/commit 分调以便
    // 观察 leave 拍的 update-only 窗口。两次 X 之间必须释放一帧（pressed=
    // changed&held，held 不释放则第二次 X 无按下沿）。
    input.update(0x02);
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK_EQ(app.title->option_menu().row(), 4);
    input.update(0);  // 释放 X
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    input.update(0x02);  // 行 4 X → substate4（>9 帧后 leave）
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK(app.pending_file_events_.empty());  // substate4 未离屏：尚无捕获
    // 空闲帧到 leave（case4 substate_frames_>9 → leave(false)→
    // host.persist_options——生产捕获点），逐帧观察 update-only 段。
    u32 leave_frame = 0;
    guard = 0;
    while (app.pending_file_events_.empty() && guard++ < 20) {
        leave_frame = app.frames;
        input.update(0);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
    }
    CHECK(!app.pending_file_events_.empty());
    if (app.pending_file_events_.empty()) return;  // 失败路径不越界索引
    CHECK(!app.pending_file_events_.empty());
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.pending_file_events_[0].kind == EventKind::PersistOptions);
    CHECK_EQ(app.pending_file_events_[0].capture_frame, leave_frame);  // 帧身份
    CHECK_EQ(app.pending_file_events_[0].bgm, 80);
    CHECK_EQ(app.pending_file_events_[0].effect, 80);
    // update-only 段：files 音量不变（乐观放行——变更+save 延至 commit）。
    // 设备音量≠files 基线：菜单编辑期 refresh_volumes→apply_volumes 已即编
    // 即效写设备（原 0044ed70 同拍，非本票延账面）。
    CHECK_EQ(app.files.bgm_volume, 100);
    CHECK_EQ(app.files.effect_volume, 80);
    CHECK_EQ(app.audio_device.music_volume, 80);   // 编辑拍 apply_volumes 已写
    CHECK_EQ(app.audio_device.effect_volume, 80);
    CHECK(app.title->option_menu().error.empty());  // 捕获拍不补记
    // commit：菜单错误档——帧不失败、音量落档值、error set-if-empty 补记、
    // screen 回 Main 照常（leave 无条件回 Main，失败补记不阻断）。
    CHECK(app.commit(kFps));
    CHECK(app.failure.empty());
    CHECK_EQ(app.files.bgm_volume, 80);
    CHECK_EQ(app.files.effect_volume, 80);
    CHECK_EQ(app.title->option_menu().error,
             std::string("Title option persistence unavailable"));
    CHECK_EQ(i32(app.title->state.screen), i32(TitleScreen::Main));
    CHECK(app.pending_file_events_.empty());
    CHECK_EQ(app.frames, leave_frame + 1u);
    CHECK(app.present());
    // set-if-empty 不覆盖（复刻 TitleOptionsMenu::fail 语义）：error 已非空
    // 时第二次失败 commit 保持原文案。捕获点走生产 persist_options。
    // 此轮基线 = 第一轮 commit 后的落档值 {80, 80}。
    th13::TitleOptionVolumes volumes;
    volumes.bgm = 55;
    volumes.effect = 66;
    CHECK(app.persist_options(volumes));
    CHECK_EQ(app.files.bgm_volume, 80);  // update-only：仍为第一轮落档值
    CHECK_EQ(app.files.effect_volume, 80);
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.commit(kFps));
    CHECK(app.failure.empty());
    CHECK_EQ(app.files.bgm_volume, 55);
    CHECK_EQ(app.files.effect_volume, 66);
    CHECK_EQ(app.title->option_menu().error,
             std::string("Title option persistence unavailable"));  // 不覆盖
    // 双 commit：无重复结算（队列空、音量不再变化）。
    const u32 frames_after = app.frames;
    CHECK(app.commit(kFps));
    CHECK(app.pending_file_events_.empty());
    CHECK_EQ(app.files.bgm_volume, 55);
    CHECK_EQ(app.frames, frames_after + 1u);
}

// ---- F1(a) 帧失败档：SpellEvent 捕获延账 + save 失败/无效卡反例 ----
TEST(file_event_commit_spell_event_capture_defer_frame_fail) {
    // 正控臂（可写存档根）：Begin/captured 入队、非 captured/replay 不入队、
    // commit 按原表达式落计数（character 列 + aggregate 列、practice 位）。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-spell-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        const i32 attempts_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->attempts[0] : -1;
        const i32 agg_before =
            app.files.spell_counters(4, 2) ? app.files.spell_counters(4, 2)->attempts[0] : -1;
        CHECK(attempts_before >= 0 && agg_before >= 0);
        CHECK(app.record_spell_event(begin_event(2, "T4spellA")));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::SpellEvent);
        CHECK_EQ(app.pending_file_events_[0].capture_frame, app.recorder.capture_frame);
        CHECK_EQ(app.pending_file_events_[0].spell_event.state.id, 2);
        CHECK_EQ(app.pending_file_events_[0].character, 0);
        // update-only 段：计数不变（变更+save 全延至 commit）。
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before);
        CHECK_EQ(app.files.spell_counters(4, 2)->attempts[0], agg_before);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());  // 正控 save 成功
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before + 1);
        CHECK_EQ(app.files.spell_counters(4, 2)->attempts[0], agg_before + 1);
        // captured End → capture 列 +1；非 captured End → 不入队。
        CHECK(app.record_spell_event(captured_end_event(2)));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        const i32 captures_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->captures[0] : -1;
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.spell_counters(0, 2)->captures[0], captures_before + 1);
        SpellEvent missed;
        missed.kind = SpellEventKind::End;
        missed.state.id = 2;  // flags 无 kSpellCapture → captured() false
        CHECK(app.record_spell_event(missed));
        CHECK(app.pending_file_events_.empty());
        // replay_active 门（捕获拍纯判定，与拆分前 Update 拍同源）。
        app.replay_active = true;
        CHECK(app.record_spell_event(begin_event(2, "T4spellA")));
        CHECK(app.pending_file_events_.empty());
        app.replay_active = false;
        // practice 位快照：run_mode_flags bit0x20 → attempts[mode1] 列。
        const i32 practice_before =
            app.files.spell_counters(0, 3) ? app.files.spell_counters(0, 3)->attempts[1] : -1;
        app.progress.run_mode_flags =
            (app.progress.run_mode_flags & ~0x30u) | 0x20u;
        CHECK(app.record_spell_event(begin_event(3, "T4spellP")));
        CHECK(app.pending_file_events_[0].spell_practice);
        CHECK_EQ(app.files.spell_counters(0, 3)->attempts[1], practice_before);
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.spell_counters(0, 3)->attempts[1], practice_before + 1);
        CHECK(app.files.error.empty());
    }
    // save 失败臂：commit fail 终态同路径（消息逐字=files.error）。
    {
        App app;
        CHECK(pin_boot(app, kFailDirA));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        const i32 attempts_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->attempts[0] : -1;
        CHECK(app.record_spell_event(begin_event(2, "T4spellF")));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirA);
        // 原表达式顺序：record_spell_attempt 先于 save——内存计数已 +1
        //（与拆分前 Update 拍失败终态逐位同路径）。
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before + 1);
        CHECK_EQ(app.frames, 0u);  // frames++ 未达（同帧截断）
        th13::GameInput input{};
        CHECK(!app.update(input, false));  // 次帧入口守卫拦停
    }
    // 无效卡臂：record_spell_attempt false → "Invalid spell record event"
    //（files.error 空档文案，逐字同原 :50-51）。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-spell-c.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        CHECK(app.record_spell_event(begin_event(-1, "T4bad")));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Invalid spell record event"));
        CHECK_EQ(app.frames, 0u);
    }
}

// ---- F1(a) 帧失败档：SpellPracticeScore ----
TEST(file_event_commit_spell_practice_score_capture_defer_frame_fail) {
    // 正控臂：捕获延账 → commit 落 practice_best_score（内部分 /10*10）。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-sps-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        app.progress.selected_spell_id = 0;
        const i32 best_before =
            app.files.spell_counters(0, 0) ? app.files.spell_counters(0, 0)->practice_best_score : -1;
        CHECK(app.record_spell_practice_score(12345));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::SpellPracticeScore);
        CHECK_EQ(app.pending_file_events_[0].score, 12345);
        CHECK_EQ(app.files.spell_counters(0, 0)->practice_best_score, best_before);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.spell_counters(0, 0)->practice_best_score,
                 best_before > 12340 ? best_before : 12340);
        // replay_active 门。
        app.replay_active = true;
        CHECK(app.record_spell_practice_score(1));
        CHECK(app.pending_file_events_.empty());
        app.replay_active = false;
    }
    // save 失败臂：fail(files.error)——空文案走 "Application operation
    // failed"（原 :101-102 同语义）；此处坏路径 → files.error 非空逐字。
    {
        App app;
        CHECK(pin_boot(app, kFailDirB));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        app.progress.selected_spell_id = 0;
        CHECK(app.record_spell_practice_score(500));
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirB);
        CHECK_EQ(app.frames, 0u);
    }
}

// ---- F1(a) 帧失败档：StageClear（gate 纯判定 + 快照 + dirty 清位拍）----
TEST(file_event_commit_stage_clear_gate_capture_defer_frame_fail) {
    // gate 纯判定臂：非 stage6/7、practice、mode 位 → 不入队（捕获拍同源）。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-sc-g.bin"));
        if (!app.failure.empty()) return;
        CHECK(app.record_stage_clear());
        CHECK(app.pending_file_events_.empty());  // stage=1 不满足
        app.progress.stage = 6;
        app.progress.practice_mode = 1;
        CHECK(app.record_stage_clear());
        CHECK(app.pending_file_events_.empty());
        app.progress.practice_mode = 0;
        app.progress.run_mode_flags |= 0x30u;
        CHECK(app.record_stage_clear());
        CHECK(app.pending_file_events_.empty());
        app.progress.run_mode_flags &= ~0x30u;
        CHECK(app.record_stage_clear());
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::StageClear);
        CHECK_EQ(app.pending_file_events_[0].character, app.progress.character);
        CHECK_EQ(app.pending_file_events_[0].difficulty, app.progress.difficulty);
        CHECK_EQ(app.pending_file_events_[0].no_continue, app.progress.continues == 0);
    }
    // 正控臂（可写）：commit 落 clear_count/no_continue/cleared 位，成功拍
    // 清 ordinary_records_dirty（序同原帧内：record+save 成功后才清）。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-sc-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 1;
        app.progress.difficulty = 2;
        app.progress.continues = 0;
        app.progress.stage = 6;
        const u32 clear_before = app.files.ordinary_records[1].clear_count[2];
        const u32 nc_before = app.files.ordinary_records[1].no_continue_clear_count[2];
        CHECK(app.record_stage_clear());
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK_EQ(app.files.ordinary_records[1].clear_count[2], clear_before);
        app.ordinary_records_dirty = true;
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.ordinary_records[1].clear_count[2], clear_before + 1u);
        CHECK_EQ(app.files.ordinary_records[1].no_continue_clear_count[2], nc_before + 1u);
        CHECK(app.files.cleared(1, 2));
        CHECK(!app.ordinary_records_dirty);  // 成功拍清位
    }
    // save 失败臂：commit fail；clear_count 内存已 +1（原表达式顺序）、
    // dirty 不清（清位在 save 成功之后）、消息逐字。
    {
        App app;
        CHECK(pin_boot(app, kFailDirC));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        app.progress.difficulty = 1;
        app.progress.continues = 3;  // no_continue=false
        app.progress.stage = 7;
        const u32 clear_before = app.files.ordinary_records[0].clear_count[1];
        app.ordinary_records_dirty = true;
        CHECK(app.record_stage_clear());
        SavePathGuard guard(kFailDirC);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirC);
        CHECK_EQ(app.files.ordinary_records[0].clear_count[1], clear_before + 1u);
        CHECK(!app.files.cleared(0, 1));
        CHECK(app.ordinary_records_dirty);  // 失败拍不清位
        CHECK_EQ(app.frames, 0u);
    }
}

// ---- F1(a) 帧失败档：PauseOption（设备音量写随冲账延至 commit）----
TEST(file_event_commit_pause_option_capture_defer_frame_fail) {
    // 正控臂（可写）：无效行原样 false 不入队；有效行捕获拍 files/设备音量
    // 双不变；commit 钳制读改写 + apply_volumes 设备写 + save。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-po-a.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.change_pause_option(-1, 5));
        CHECK(!app.change_pause_option(2, 5));
        CHECK(app.pending_file_events_.empty());
        CHECK(app.failure.empty());  // 无效行不 fail
        app.files.bgm_volume = 50;
        const i32 device_bgm_before = app.audio_device.music_volume;
        CHECK(app.change_pause_option(0, 30));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::PauseOption);
        CHECK_EQ(app.pending_file_events_[0].row, 0);
        CHECK_EQ(app.pending_file_events_[0].delta, 30);
        CHECK_EQ(app.files.bgm_volume, 50);              // update-only 不变
        CHECK_EQ(app.audio_device.music_volume, device_bgm_before);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.bgm_volume, 80);
        CHECK_EQ(app.audio_device.music_volume, 80);     // apply_volumes 已写
        // 行 1 SE + 低钳 0。
        app.files.effect_volume = 10;
        CHECK(app.change_pause_option(1, -30));
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.effect_volume, 0);
        CHECK_EQ(app.audio_device.effect_volume, 0);
        // 高钳 100。
        app.files.bgm_volume = 90;
        CHECK(app.change_pause_option(0, 30));
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.bgm_volume, 100);
    }
    // save 失败臂：内存与设备音量已在 save 前写（原 :463-469 表达式顺序），
    // 帧失败同路径。
    {
        App app;
        CHECK(pin_boot(app, kFailDirA));
        if (!app.failure.empty()) return;
        app.files.bgm_volume = 50;
        CHECK(app.change_pause_option(0, 30));
        SavePathGuard guard(kFailDirA);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirA);
        CHECK_EQ(app.files.bgm_volume, 80);           // 钳制读改写已发生
        CHECK_EQ(app.audio_device.music_volume, 80);  // 设备写已发生（随冲账）
        CHECK_EQ(app.frames, 0u);
        th13::GameInput input{};
        CHECK(!app.update(input, false));
    }
}

// ---- F1(a) 帧失败档：ResultEntry（捕获拍结算行 + commit 固定序截断）----
// 截断臂用 live run：存档队列失败 → 录制槽已冲账（第一环先于存档队列）、
// pan 挂起队/播放帧/audio 排空/frames++ 全部不执行、次帧守卫拦停。
TEST(file_event_commit_result_entry_capture_defer_and_commit_truncation) {
    // 正控臂（可写，独立 app）：捕获拍构建结算行，commit 落榜表+save_score。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-re-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 2;
        app.progress.difficulty = 1;
        app.progress.continues = 1;
        // 1000000 > 空表哨兵行[0]=100000（0x449760 原版空表显式行）→ 插入
        // 落 row 0，断言可直接读 [0]。
        app.selection_score.score = 1000000;
        const th13::TitlePlayerDataScore baseline =
            app.files.ordinary_records[2].ranking[1][0];
        CHECK(app.record_result_entry("T4R", 1));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::ResultEntry);
        CHECK_EQ(app.pending_file_events_[0].result.score, 1000000);
        CHECK_EQ(app.pending_file_events_[0].result.stage, 1);
        CHECK_EQ(app.pending_file_events_[0].result.name, std::string("T4R"));
        CHECK_EQ(app.pending_file_events_[0].result.available, true);
        CHECK(app.pending_file_events_[0].result.timestamp > 0);  // 捕获拍实时量
        // update-only 段：榜表不变（空表哨兵行原样）。
        CHECK(app.files.ordinary_records[2].ranking[1][0].score == baseline.score &&
              app.files.ordinary_records[2].ranking[1][0].available == baseline.available);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.ordinary_records[2].ranking[1][0].score, 1000000);
        CHECK_EQ(app.files.ordinary_records[2].ranking[1][0].name, std::string("T4R"));
    }
    // 截断臂（live run + 临时坏存档根）：固定序逐环证据。
    App app;
    CHECK(boot_live_run(app, "t4fe-re-trunc.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    th13::GameInput input;
    input.update(0);  // 屏障帧：present 置 pending_load
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    input.update(0);  // battle f1：录制槽 1 条 + 播放帧计数 1
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(1));
    CHECK_EQ(app.pending_play_frames_, 1u);
    std::vector<u8> enc_pre;
    CHECK(encode_pinned(app, enc_pre));
    const i32 mirror_pre = app.files.ordinary_records[3].total_frames;
    const u32 frames_pre = app.frames;
    // 同帧多类捕获：ResultEntry（将失败）+ pan（将不被冲账）+ 设备请求
    //（将不被排空）。pending_voices 用增量断言（对局自身 SE 请求同窗排队）。
    app.selection_score.score = 999999;  // > 空表哨兵 100000 → 落 row 0
    CHECK(app.record_result_entry("T4X", 1));
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.set_voice_pan(55, 192.f));
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
    const u32 pending_pre = app.audio_device.statistics().pending_voices;
    CHECK(app.audio_device.enqueue(44));
    CHECK_EQ(u32(app.audio_device.statistics().pending_voices), pending_pre + 1u);
    // 临时只读存档根注入：commit 存档队列失败（真实生产 IO 失败路径）。
    {
        SavePathGuard guard(kFailDirB);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirB);
    }
    // 环 1：录制槽已冲账（先于存档队列——流落盘、槽清空）。
    CHECK(app.recorder.pending_inputs.empty());
    std::vector<u8> enc_post;
    CHECK(encode_pinned(app, enc_post));
    CHECK(!bytes_equal(enc_post, enc_pre));
    // 环 2 失败点：结算行内存已插入（原表达式顺序，SP 同终态）。
    const i32 run_difficulty = app.progress.difficulty;
    CHECK(run_difficulty >= 0 && run_difficulty < 5);
    CHECK_EQ(app.files.ordinary_records[3].ranking[size_t(run_difficulty)][0].score, 999999);
    // 环 3：pan 挂起队未冲账（存档队列先于 pan 队）。
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
    // 环 4：播放帧未冲账（镜像/计数不变）。
    CHECK_EQ(app.pending_play_frames_, 1u);
    CHECK_EQ(app.files.ordinary_records[3].total_frames, mirror_pre);
    // 环 5：audio_device.update() 未执行（请求未排空）、frames++ 未达。
    CHECK_EQ(u32(app.audio_device.statistics().pending_voices), pending_pre + 1u);
    CHECK_EQ(app.frames, frames_pre);
    // 次帧入口守卫拦停（update/step 双入口）。
    input.update(0);
    CHECK(!app.update(input, false));
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(app.frames, frames_pre);
}

// ---- F1(a) 帧失败档：MusicUnlocked（索引校验留捕获拍同步 fail）----
TEST(file_event_commit_music_unlocked_capture_validate_frame_fail) {
    // 正控臂（可写）：commit 落 heard 位 + save。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-mu-a.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.files.music_heard(5));
        CHECK(app.record_music_unlocked(5));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::MusicUnlocked);
        CHECK_EQ(app.pending_file_events_[0].track, 5);
        CHECK(!app.files.music_heard(5));  // update-only 不变
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK(app.files.music_heard(5));
    }
    // 捕获拍索引校验（原 Application.cpp:1466 同步 fail 语义留捕获拍）。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-mu-b.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.record_music_unlocked(32));
        CHECK_EQ(app.failure, std::string("Ending music index out of range"));
        CHECK(app.pending_file_events_.empty());
        CHECK(!app.record_music_unlocked(-1));
        CHECK(app.pending_file_events_.empty());
    }
    // save 失败臂：commit fail；heard 位内存已置（原表达式顺序）。
    {
        App app;
        CHECK(pin_boot(app, kFailDirC));
        if (!app.failure.empty()) return;
        CHECK(app.record_music_unlocked(7));
        SavePathGuard guard(kFailDirC);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirC);
        CHECK(app.files.music_heard(7));
        CHECK_EQ(app.frames, 0u);
    }
}

// ---- 捕获序冲账 + 截断：失败点后条目保留队列、失败点前已应用 ----
TEST(file_event_commit_capture_order_truncation) {
    App app;
    CHECK(pin_boot(app, "t4fe-order.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 0;
    // 捕获序：[1] mark_heard(2)（吞错档恒不失败）→ [2] SpellEvent 无效卡
    //（commit fail）→ [3] mark_heard(3)。
    app.mark_heard(2);
    CHECK(app.record_spell_event(begin_event(-1, "T4order")));
    app.mark_heard(3);
    CHECK_EQ(app.pending_file_events_.size(), size_t(3));
    CHECK_EQ(app.pending_file_events_[0].kind, EventKind::MarkHeard);
    CHECK(app.pending_file_events_[1].kind == EventKind::SpellEvent);
    CHECK_EQ(app.pending_file_events_[2].kind, EventKind::MarkHeard);
    CHECK(!app.commit(kFps));
    // [1] 已应用（捕获序先于失败点）。
    CHECK(app.files.music_heard(2));
    // [2] 失败：消息逐字（files.error 空档 → 原文案）。
    CHECK_EQ(app.failure, std::string("Invalid spell record event"));
    // [3] 未应用且保留在队列（捕获序截断，非丢弃非跳过）。
    CHECK(!app.files.music_heard(3));
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.pending_file_events_[0].kind == EventKind::MarkHeard);
    CHECK_EQ(app.pending_file_events_[0].track, 3);
    CHECK_EQ(app.frames, 0u);
}

// ---- F2：save_replay_slot 确认拍原位保留（不进待确认队列）----
TEST(file_event_commit_save_replay_slot_confirmed_tier_in_place) {
    // 无效槽：确认拍即 fail（调用点 `|| fail` 原位语义），不入队。
    {
        App app;
        CHECK(pin_boot(app, "t4fe-srs-neg.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.save_replay_slot(0, "PLAYER"));
        CHECK_EQ(app.failure, std::string("Invalid replay slot"));
        CHECK(!app.save_replay_slot(26, "PLAYER"));
        CHECK(app.pending_file_events_.empty());
        CHECK_EQ(app.frames, 0u);  // 无 commit 参与
    }
    // 正控（live run）：同步完成——catalog 导入 + 落盘 + 队列空 + frames 不变。
    App app;
    CHECK(boot_live_run(app, "t4fe-srs-a.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    input.update(0);
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK(app.recorder.ready());
    const u32 frames_pre = app.frames;
    CHECK(app.pending_file_events_.empty());
    CHECK(app.save_replay_slot(1, "PLAYER"));
    CHECK(app.failure.empty());
    // 原位保留：同步生效（无 commit 即已可观测），且不产生待确认条目。
    CHECK(app.pending_file_events_.empty());
    CHECK(app.replay_slots[0].decoded != nullptr);  // th13_01.rpy → 槽 0
    CHECK_EQ(app.replay_filename(0), std::string("th13_01.rpy"));
    CHECK_EQ(app.frames, frames_pre);  // 确认拍无 commit 参与（帧身份=当前帧）
    if (FILE* file = std::fopen("th13_01.rpy", "rb")) {
        std::fclose(file);
    } else {
        std::printf("  失败: 确认拍未落盘 th13_01.rpy（cwd=%s）\n", ".");
        CHECK(false);
    }
}

}  // namespace

int main() { return th13test::run_all("m1.unit_file_event_commit"); }
