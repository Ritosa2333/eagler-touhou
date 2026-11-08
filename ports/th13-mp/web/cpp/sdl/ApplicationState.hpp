// Application 聚合根（th15 ApplicationState 裁剪：destination 驱动的场景所有权，
// 装载挡板与 16 槽 projection 保留；practice/checkpoint/ending/replay 不做，
// docs/plans/ui-base.md §四）。
#pragma once

#include "../game/TitlePlayerDataMenu.hpp"
#include "../game/TitleSpellPracticeMenu.hpp"
#include "../game/SpellCatalog.hpp"
#include "../game/SpellElapsedTime.hpp"

namespace th13::sdl {

// ---- 标题 PlayerData / SpellPractice 最小宿主服务（0x453170 / 0x4560a0）----
// 两个平台接口都声明了无参 character_count()，但语义不同：
//   プレイヤーデータ = 固定 5 列（0..4，+0x30=4 的原始列号），
//   符卡练习       = 固定 4 角色（0..3，0x4564c0 的角色切换面）。
// 同一个最派生类无法用单个 override 同时满足两个虚槽。TitleScenePlatform
// 当前把两个接口都作为基类聚合，因此 ApplicationState 只能提供一个
// character_count()（取 5 以满足 0x453170 开屏）；两套数据服务在这里
// 分开实现，Spell 侧角色上限由 TitleSpellPracticeMenu 构造器钳制。
//
// 数据口径（缺档与旧档兼容）：
//   - scoreth13.dat 的 0x7f 条记录表（姓名/分数/★）尚未进 FileStore，
//     故 PlayerData 为真实空表；不生成记录、不补零值冒充。
//   - 符卡 catalog（DAT_004bbb28 / DAT_004be7d4）与 score 解锁位未落库，
//     故符卡全部锁定、无 id/rank/名称；菜单可浏览但不可启动。
// 两服务只读、无状态，不写档、不改 FileStore 版本。
struct TitlePlayerDataHost : TitlePlayerDataPlatform {
    std::function<i32(i32)> count_records;
    std::function<bool(i32, i32, i32, TitlePlayerDataRecord&)> read_record;
    std::function<bool(i32, i32, TitlePlayerDataSummary&)> read_summary;
    bool open_player_data() override { return true; }  // 空表也可开屏（原版缺档）
    void close_player_data() override {}
    i32 difficulty_count() const override { return kPlayerDataDifficulties; }
    i32 character_count() const override { return kPlayerDataCharacters; }
    i32 record_count(i32 difficulty) const override {
        // 记录表未持久化：所有难度都是真实的 0 条，而不是 127 个空槽。
        if (difficulty < 0 || difficulty >= kPlayerDataDifficulties) return 0;
        return count_records ? count_records(difficulty) : 0;
    }
    bool record(i32 difficulty, i32 character, i32 ordinal,
                TitlePlayerDataRecord& out) const override {
        return read_record && read_record(difficulty, character, ordinal, out);
    }
    bool summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const override {
        return read_summary && read_summary(difficulty, character, out);
    }
};

struct TitleSpellPracticeHost : TitleSpellPracticePlatform {
    const SpellCatalog* catalog = nullptr;
    SpellCatalog::Check encountered;
    SpellCatalog::Check captured;
    SpellCatalog::Check named;
    i32 stage_count() const override { return kSpellPracticeStages; }
    bool stage_available(i32 stage) const override {
        // 0x4560a0 case0 固定 +0x30=7，stage 行不按存档设锁。
        return stage >= 1 && stage <= kSpellPracticeStages;
    }
    std::string stage_name(i32 stage) const override {
        // 0x4bb944 名字表（尾随空格是原版列对齐的一部分，原样保留）。
        static constexpr const char* kNames[kSpellPracticeStages + 1] = {
            nullptr, "Stage 1", "Stage 2", "Stage 3", "Stage 4",
            "Stage 5", "Stage 6", "Extra  "};
        if (stage < 1 || stage > kSpellPracticeStages) return {};
        return kNames[stage];
    }
    i32 spell_count(i32 stage) const override {
        // 0x456650 local_24 = {4,4,4,4,5,7,13}（stage 1..6 + Extra）。
        if (stage < 1 || stage > kSpellPracticeStages) return 0;
        return kSpellPracticeSlots[stage - 1];
    }
    bool spell_unlocked(i32 stage, i32 slot) const override {
        return catalog && catalog->spell_unlocked(stage, slot, encountered);
    }
    std::string spell_name(i32 stage, i32 slot) const override {
        return catalog ? catalog->spell_name(stage, slot, encountered) : std::string{};
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* entry = catalog ? catalog->variant(stage, slot, difficulty) : nullptr;
        return entry ? entry->id : -1;
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* entry = catalog ? catalog->variant(stage, slot, difficulty) : nullptr;
        return entry ? entry->rank : -1;
    }
    bool spell_variant_available(i32 stage, i32 slot, i32 difficulty) const override {
        return catalog && catalog->variant_available(stage, slot, difficulty,
                                                     encountered, captured);
    }
    std::string spell_variant_name(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* entry = catalog ? catalog->variant(stage, slot, difficulty) : nullptr;
        return entry && named && named(entry->id)
                   ? entry->name : std::string{};
    }
    i32 character_count() const override { return kSpellPracticeCharacters; }
    std::string character_name(i32 character) const override {
        // 0x4bb904 名字表（同 TitleReplayMenu 的原版列对齐字符串）。
        static constexpr const char* kNames[kSpellPracticeCharacters] = {
            "Reimu  ", "Marisa ", "Sanae  ", "Youmu  "};
        if (character < 0 || character >= kSpellPracticeCharacters) return {};
        return kNames[character];
    }
};

// 0x442960 属原生 player update（0x443240 的移动分支）：environment.paused
// 或 scene_flags&0x70 覆盖层帧不执行玩家更新，隐藏计数 player+0x1469c 不
// 推进，计数 >0x1d 的 option 根 pending=1 也不触发。Application 的门计数
// 与中断检查必须用同一条件；否则门内按 Esc/失焦暂停 >=30 帧会让 port 在
// 暂停中提前触发 interrupt，恢复后 option 树比原版更早消失。
// （抽成可测纯函数：tests/ui/transition_gate_pause_test.cpp。）
inline bool transition_gate_player_update_runs(bool paused, u32 scene_flags) noexcept {
    return !paused && (scene_flags & 0x70u) == 0;
}

inline bool transition_gate_should_interrupt(u32 frames, bool paused,
                                             u32 scene_flags) noexcept {
    return frames >= 29 && transition_gate_player_update_runs(paused, scene_flags);
}

}  // namespace th13::sdl

// tests/ui/run.sh 只链接 game/*.cpp，不构建 SDL 设备层。该宏仅让宿主服务
// 单测避开 SDL3 设备头；生产构建从不定义它（ApplicationState 仍完整编译）。
#ifndef TH13_APPLICATION_PLATFORMS_ONLY

#include "Application.hpp"
#include "SceneDisplay.hpp"
#include "FontDevice.hpp"
#include "AudioDevice.hpp"
#include "FileStore.hpp"
#include "../game/EndingScene.hpp"
#include "../game/ReplayFile.hpp"
#include "../game/ReplaySession.hpp"
#include "../game/ReplayRecorder.hpp"
#include "../game/TitleScene.hpp"
#include "../game/ManualViewer.hpp"
#include "../game/TitleMusicRoomMenu.hpp"
#include "../game/TitleOptionsMenu.hpp"
#include "../game/DialogueText.hpp"
#include "../game/RunScene.hpp"
#include "../game/RunPause.hpp"
#include "../game/PlayfieldTargets.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace th13::sdl {

// 原版 supervisor destination 语义（数值沿用 th15 对照：13 开局、2/4/14 回标题
// 变体、3 退出、15 结局；th13 侧锚定后在 AddressMap.hpp 登记回填）。
// NextStage = th13 DAT_004dcc24 的 0xc 换关请求（0x42cd40 关 1-5 主干）——
// 延续对局、按已递增的 progress.stage 装载，与 13 的全新开局区分。
enum class Destination : i32 {
    Quit = 3,
    Title = 4,
    TitleFromGameOver = 2,
    TitleFromReplay = 14,
    Run = 13,
    NextStage = 100,
    Ending = 15,
    // 原版 DAT_004dcc24 的 0x10：Ending 场景脚本 END（0x416610）→
    // 0x43c1c0 case0x10 → 0x4165f0 拆卸 → 目的地 4 + DAT_004dc670=3。
    AfterEnding = 16
};

struct ApplicationState final : TitleScenePlatform, RunSceneServices, RunPausePlatform,
                                EndingPlatform {
    GraphicsDevice graphics;
    FontDevice fonts{graphics};
    // 字体设备初始化失败的显式记录（原版 MS Gothic 启动资产缺席 → 对话日文
    // 渲染 BLOCKED；空 = 字体可用）。标题/HUD ASCII 文本不依赖字体设备。
    std::string fonts_failure;
    AudioDevice audio_device;
    FileStore files;
    GameRandom visual;
    u16 boot_seed = 0;
    std::array<u8, 256> visual_font_noise_table{};
    AnmEnvironment environment;
    AnmManager animations{visual, environment};
    DialogueText dialogue_text;  // text.anm scripts0..3 四行 VM + texture0 动态正文
    AnmRenderer renderer{graphics};
    ScreenViews views{renderer, environment};
    AsciiText captions{animations, environment};
    FrameScheduler scheduler;
    AnmEnvironment loading_environment;
    AnmManager loading_animations{visual, loading_environment};
    bool platform_prepared = false;
    u32 loading_signature = 0, loading_prayer = 0;
    // 00417370 creates this in the main manager on destination0xc
    // (0042bfa0:0x42bffd/0x42c02e/0x42c061); 004172e0 -> 0046fc30 retires
    // the whole caption tree at stage activation (0042c550:0x42c881/0x42c9fb).
    u32 transition_prayer = 0;
    // Next-stage STD scenery Load/Initialize is part of the loader gate, not
    // the 0042bfa0 door: Stage 3's scenery ANM scripts read one visual RNG
    // value per primitive at their first tick (st03wl scripts 11-16, 12 index
    // units). The original consumes those draws at the gate's fade phase
    // (native continuous th13_01: door input 27673 = 0 draws, start_transition
    // 27793 = 6 draws); doing it at the door shifts visual_index for the whole
    // gate. Keep the door teardown immediate and build the scenery when the
    // ordinary start_transition trigger fires.
    i32 transition_scenery_bank = -1;
    std::string transition_scenery_std;
    bool transition_scenery_pending = false;
    // Debug replay boot handoff: the 00439ce0/00417370 loading caption is
    // advanced by loader frames (native f0 create, f1 first ordinary update =
    // 24 visual index units, f2/f3 no draws, battle f4). th13_debug_replay
    // replays those three loader frames through this same primitive instead of
    // carrying its own 24-unit compensation; production startup uses it from
    // advance_loading(). The VM timer ends at f3 exactly like the original.
    static constexpr unsigned kDebugLoadingFrames = 3;
    // Battle-start handoff of the loading caption fore-pool (draw gate 2):
    // native keeps the AsciiInf-bound manager's layer31 ascii/loading.png VMs
    // alive and drawn for battle frames 0..9 while they fade (H evidence:
    // artifacts/round-16/agent-h-draw/phase3/loading-op-ownership.json), then
    // retires the tree (0x4172e0/0x46fc30). The port's loading_animations
    // registers no scene draw callbacks, so this callback reproduces the
    // fore-pool draw at the layer31 schedule position (Draw priority 72, after
    // the main manager's layer31 at 71 and before the ASCII text at 78).
    FrameCallback loading_battle_draw{};
    // 原版 FPSInf 的场内整数帧率文本（font0 "60" @391.5,449.5）与调试叠加
    // "%2.1ffps"（font1 @588,470）在同一 Draw 阶段按 HUD(76) → 77 → ASCII(78)
    // 顺序入队；旧实现把 fps 文本在 step 里提前入队，导致字形顺序/首个字形
    // 颜色对拍错位，并缺场内 "60" 两个字形。
    FrameCallback fps_text_draw{};
    float last_fps_ = 60;
    // 捕获 harness 用：原生 oracle 的实测 fps≈0.1 决定 FPS 文本颜色；
    // 生产路径不设置（-1 = 用真实 last_fps_）。
    float fps_color_override_ = -1;
    void set_fps_color_override(float value) { fps_color_override_ = value; }
    // 0x424990 慢速率累加器（结算条目 float@24 与回放 slow% 同源）：每 ≥1s
    // 窗采样 fps；游戏态有效（未装载/未暂停/无覆盖层，原生 +0x60&0x14 门）
    // 时分母 +60.0，fps≤57.0 分母计实际值否则计 60.0。0x42b990 关首清零 →
    // 结算百分比反映最后一段关卡。录像 slow 事件字节（回放 FPS 显示直读）
    // 取最近采样窗的整数 fps。
    double slow_actual_ = 0.0, slow_budget_ = 0.0;
    double slow_window_time_ = 0.0;
    u32 slow_window_frames_ = 0;
    float slow_window_fps_ = 60.0f;
    // 0x424990:37-50 fps 管理器计数器（对象 0x4c218c +0x24 num / +0x2c den 的
    // port 对应物）：每个有效采样窗（游戏态门与 slow_actual_ 同一分支）den+=60、
    // 窗 fps≤57 计实测值、否则计满 60。0x42b990 关首清零 → 保存时 +0x54 反映
    // 末段关卡。save_replay_slot 默认 ReplaySaveMeta 的实时慢速率来源。
    double slow_num_ = 0.0, slow_den_ = 0.0;
    void reset_slow_rate() {
        slow_actual_ = 0.0; slow_budget_ = 0.0;
        slow_window_time_ = 0.0; slow_window_frames_ = 0;
        slow_num_ = 0.0; slow_den_ = 0.0;
    }
    unsigned pending_load = 0, loading_frames = 0;
    bool loading_startup = false;
    double loading_debt = 0;
    SessionState progress;
    HudPlayerState selection_player;
    HudScoreState selection_score;
    TitleSelectionSettings selection;
    // 0x4dc324 等（game RNG）：ECL gvars RAND 族（0x420380/0x420d00）、敌机
    // 指令（0x41ce10/0x417b10）、弹幕出生/etEx（0x40e840/0x40f030）、自机弹
    // 出膛（0x445c40）共用此对象；0x4dc31c（visual）是 ANM 指令/畸变侧的
    // 独立第二对象——反汇编逐调用点核验见 artifacts/verification/r3/R。
    // StageSim/StageBattle 消费此实例的构造注入属 StageBattle 所有者（I）
    // 的接线（R3 已在 begin_replay_run 恢复本实例种子）。
    GameRandom game_random;
    std::unique_ptr<SceneDisplay> display;
    std::array<FrameCallback, 2> animation_updates;
    std::unique_ptr<ManualViewer> manual_viewer;
    std::unique_ptr<TitleScene> title;
    // 0042bfa0 keeps DAT_004c2190 on destination0xc. Its references point
    // to application-owned state, independently of the stage scene.
    std::unique_ptr<GameHud> run_hud;
    std::unique_ptr<StagePlayerState> run_player;
    std::unique_ptr<RunScene> run;
    std::unique_ptr<RunPause> pause;
    // Ending 场景（0x416570 家族；0xf 进入、0x10 拆卸回标题）。结局字节区
    // （score 缓冲 0x1b26a..0x1b276）直接引用 FileStore 的共享内存。
    std::unique_ptr<EndingScene> ending;
    std::array<u32, 5> ending_text_vms{};
    std::array<u32, 24> ending_anm_vms{};
    struct EndingFade {
        ApplicationState* owner = nullptr;
        i32 kind = 0, duration = 0, time = 0, alpha = 0;
        bool playfield_only = false;
        FrameCallback update, draw;
    };
    std::vector<std::unique_ptr<EndingFade>> ending_fades;
    // Original two-surface game composition; no gameplay-coordinate shortcut.
    std::unique_ptr<PlayfieldTargets> playfield_targets;
    i32 pending_destination = -1, selected_stage = 1, current_destination = 13;
    u32 frames = 0;
    // 回放调试会话（th13_debug_* 帧泵驱动；正常标题流不经过）。输入流以
    // 0x448200 节奏逐帧接管 controls——装载态/难度/角色取自录像头，直入对局。
    ReplayFile replay_file;
    ReplayRecorder recorder;
    // D-fix（0x448ec0 关首输入沿复位）：0x448ec0 在关首清 0x4e4c08/0x4e4c0c/
    // 0x4e4c14/0x4e4c18 与 duration/repeat 阵列 → 关内 frame 0 的 pressed =
    // 全量 held、released = 0（真机五处关界实录一致）。置位后首条录制帧按
    // 该语义落盘，随后自动清除。
    bool record_edge_reset_pending_ = false;
    // B' 端到端注入缝：顶层头 +0x18..+0x53 未映射域在原版承接保存时刻的
    // 内存底（0x4484d0 只覆写名字/日期/分/慢速率/槽计数等已名字段），属
    // 不可再现快照。enabled 时 recorder.start 以该 verbatim 顶层头替代
    // top_snapshot（名字/日期/分等仍由 encode 按生产语义覆写）。
    std::vector<u8> replay_top_pin_;
    bool replay_top_pin_enabled_ = false;
    // 00412ee0 edges are latched from 00413270/00413a60 events, not by
    // sampling manager+0x7c bit0 at Update 26: a same-tick Begin+End must
    // still advance manager+0x8c once. Clock owns start stamp + slot index.
    SpellTimeClock spell_time_clock;
    i32 spell_time_stage = 0, spell_time_packed = 0;
    // B' 端到端注入缝：符卡经过时间钉住表（stage 1..7 → 0x70 + 4·slot 的
    // 21 dword）。原版 0x412ee0 的经过时间取实时钟（FUN_0045d880 = QPC），
    // 与保存 date/slow 同属「保存时刻实时量」；表由端到端测试自原 .rpy
    // stage 头注入，enabled=false（默认）= 生产实时钟语义不变。
    std::array<std::array<i32, 21>, 8> spell_time_pin_{};
    bool spell_time_pin_enabled_ = false;
    bool stage_transition_loading = false, stage_transition_active = false;
    i32 stage_startup_age = -1;
    // 0xc 换关门内帧计数（prepare_next_stage 当帧=0，门内每 step 尾 +1）。
    // 原版门首 0x42bfa0 当帧由 0x42b6d0 下发 option 根 label3；0x442960
    // 隐藏支路每帧对 player+0x1469c 计数，计数 29（门内第 30 帧，input
    // 12214）时对 option 根置 label1 并清槽。计数只在原生 player update
    // 会跑的帧推进（environment.paused / scene_flags&0x70 时跳过，见
    // transition_gate_player_update_runs），activate_next_stage 清零。
    u32 transition_gate_frames = 0;
    bool transition_gate_active = false;
    i32 next_scenery_bank = 26;
    bool prepare_next_stage();
    bool activate_next_stage();
    bool begin_record_stage();
    std::unique_ptr<ReplayInputCursor> replay_cursor;
    bool replay_active = false;
    i32 replay_catalog_index = 0; // 4525b0 preserves DAT_004dc674 for the return
    bool replay_finished = false;
    // 0x44c0d0 标题线程尾段已跑：TitleInf+0x5e4>=0xB4 或 DAT_004dcc5c&0x180
    // 时经 0x46e620 卸载 ANM 槽 1（sig.anm）。标题场景跑过即置位；加速直入
    // （th13_debug_replay / oracle）不经过标题，保持 false——此时 sig 开场
    // 签名幕存续到整场（原生 gate-01 全窗逐帧 2 op 实证）。
    bool title_flow_ran = false;
    u32 replay_consumed = 0;
    FrameCallback replay_fast_forward{}, ending_update{};
    const GameInput* physical_input = nullptr;
    u32 replay_pass_start = 0;
    bool restart_update = false;
    GameInput replay_input; // recorded input history, frozen with the stage
    // 回放恢复挂点：begin_replay_run 备好 stage 指针，begin_run 在关首表
    // 重置（0x42b990:103-110 等价）之后、场景构造/ECL 第 0 帧之前应用
    // 0x449070 恢复（种子 + 22 dword）。正常开局/换关为空。
    const ReplayStage* pending_replay_stage = nullptr;
    u32 pending_replay_stage_index = 0;
    // 换关装载帧的 0x449070 恢复延迟到该帧全部动画抽取之后执行（原版装载
    // 器在帧内 ANM 更新之后触达 449070：连续参考 frame 12185 先消费 24 个
    // index 单位的旧血缘抽取，随后 vs/gs 重打 stamp、gi 清零）。
    bool pending_transition_restore = false;
    bool initialized = false, exiting = false;
    std::array<i32, 16> projected{};
    std::string failure;
    std::unordered_map<std::string, std::vector<u8>> resource_cache;

    bool loading_waiting() const;
    bool complete_loading();
    bool render_loading();
    bool advance_loading(double);
    // One loader frame: 0x46f170 main chain + 0x46f240 secondary chain over
    // the loading ANM manager. Shared by the production 180-frame startup
    // (advance_loading) and the debug replay's three-frame boot handoff.
    bool advance_loading_caption();
    // Battle frames 0..9: tick+draw loading_animations' layer31 fore-pool
    // (native caption handoff) and retire it at frame 10. Draw-phase only;
    // the loader gate's own tick count and RNG draws are untouched.
    bool draw_loading_battle();
    // 原版 FPSInf：场内整数帧率（font0）+ 调试 "%2.1ffps"（font1），在
    // Draw 优先级 77（HUD 76 之后、ASCII drain 78 之前）入队。
    bool draw_fps_text();
    // 0x45bf10/0x45d7d0 Present 前截取队列消费（暂停黑幕）：取
    // AnmManager::take_captures()，逐个把本帧 screen 战场矩形点采样进目标
    // ANM Blank 纹理。SceneDisplay::before_present 钩子调用。
    bool flush_animation_captures();
    bool fail(const std::string&);
    bool prepare_platform();
    bool prepare_loading();
    bool draw_loading(bool signature);
    bool initialize(bool);
    ~ApplicationState();
    // Physical input drives menus; Replay gameplay consumes its own stream
    // at the RunScene update gate (00448200/00414f30).
    bool update_scene_pass(const GameInput& input);
    bool step(const GameInput& input, float fps, bool lost_focus);
    bool apply_destination();
    bool quit(); // original global score cleanup/save; reports storage failure
    bool begin_title(i32 reason);
    // 正常开局/换关由 pending stage / replay_session 设置 replay_active 与恢复状态
    bool begin_run();
    // 回放会话直入：解码 .rpy → 按录像头配置 → begin_run（跳过标题与装载挡板）。
    bool begin_replay_run(const u8* data, size_t size, u32 stage_index);
    // 回放跨关：保留既有 replay_file/session 状态，按录像阶段索引配置新 stage
    bool begin_replay_stage(u32 stage_index, bool stage_transition = false);
    u32 replay_take_input();  // 每游戏帧恰一次：取当前输入态并推进游标
    void release_run(bool preserve_hud = false);
    const i32* projection();
    bool read(const std::string&, std::vector<u8>&, bool required = true);
    // ---- [R3-M] 标题 Replay 菜单（TitleReplayPlatform 实现，0x452300 槽位语义）----
    // 槽 0-24 = 编号（导入名 th13_NN.rpy 同名落槽）、25-73 = 导入追加、
    // 74 = 网页「导入本地 .rpy」入口行（占用原版第 50 用户槽，网页保留）。
    // decoded 供菜单借用（只读），bytes 供 start_replay 帧尾交给生产解码
    // begin_replay_run（0x448c40 装载序重走，不建平行状态）。
    struct ReplayCatalogSlot {
        std::string name;
        std::vector<u8> bytes;
        std::shared_ptr<ReplayFile> decoded;
    };
    std::array<ReplayCatalogSlot, 75> replay_slots{};
    std::string replay_import_error_;
    i32 pending_replay_file = -1, pending_replay_stage_slot = -1;  // start_replay 排队
    bool import_replay_bytes(const u8* data, size_t size, const std::string& name);
    // TitleReplayPlatform
    i32 replay_count() const override;
    const ReplayFile* replay_entry(i32 index) const override;
    std::string replay_filename(i32 index) const override;
    bool request_replay_file() override;
    std::string replay_import_error() const override;
    bool start_replay(i32 file_index, i32 stage_index) override;
    // TitleScenePlatform
    bool clear_title_overlay() override;
    bool queue_title_music(i32 code, i32 value, const std::string&) override;
    bool clear_current_wave() override;
    bool title_exit(i32 destination) override;
    bool fade_out_title_music() override;
    bool sound(i32) override;
    bool begin_transition(u32&) override;
    bool start_game(i32 stage) override;
    bool start_spell_practice(const SpellPracticeCommand&) override;
    bool can_start_spell_practice(const SpellPracticeCommand&) const override;
    bool record_menu_views() const override { return true; }
    bool open_manual() override;
    bool update_manual(u32 pressed, u32 repeated) override;
    bool manual_finished() const override;
    void close_manual() override;
    bool rasterize_title_text(AnmVm&, u32, const std::string&,
                              AnmTextAlignment = AnmTextAlignment::Left) override;
    bool request_destination(i32 raw) override;  // supervisor 0x4dcc24 真值 → destination
    bool hud_text(const HudTextDraw&) override;
    // TitleMenuServices：clear-tag 真实存档门（0x451450 case0 score 块
    // +0x56dc*char+0x7e4+diff*4 != 0；MenuExtras FileStore 数据源，缺档全隐）。
    bool character_cleared(i32 character, i32 difficulty) override;
    bool extra_unlocked(i32 character) override;
    bool practice_stage_played(i32 character, i32 difficulty, i32 stage) override;
    // TitleOptionPlatform（Options 屏宿主面，r4/menu-extras 契约 §三）
    void apply_volumes(i32 bgm, i32 effect) override;
    bool persist_options(const TitleOptionVolumes& volumes) override;
    bool open_key_config() override;  // 键配置屏（0x44fcc0 screen 4）分期后接
    // Pad bindings 与手柄按钮状态（0044fcc0 实证）
    u32 raw_pad_buttons = 0;
    const std::array<i16, 5>& pad_bindings() const noexcept { return files.pad_bindings; }
    bool persist_pad_bindings(const std::array<i16, 5>& bindings) {
        files.pad_bindings = bindings;
        return files.save();
    }
    // PlayerData / SpellPractice 宿主服务（0x453170 / 0x4560a0）。服务本体
    // 见文件顶部；TitleScenePlatform 已聚合两个平台接口，这里转发到对应
    // 服务，缺档/旧档返回空表/全锁定而不是 fatal。
    TitlePlayerDataHost player_data_host;
    TitleSpellPracticeHost spell_practice_host;
    SpellCatalog spell_catalog;
    void connect_record_menus();
    // TitlePlayerDataPlatform / TitleSpellPracticePlatform（0x453170 / 0x4560a0）
    bool open_player_data() override { return player_data_host.open_player_data(); }
    void close_player_data() override { player_data_host.close_player_data(); }
    i32 difficulty_count() const override { return player_data_host.difficulty_count(); }
    i32 record_count(i32 difficulty) const override {
        return player_data_host.record_count(difficulty);
    }
    bool record(i32 difficulty, i32 character, i32 ordinal,
                TitlePlayerDataRecord& out) const override {
        return player_data_host.record(difficulty, character, ordinal, out);
    }
    bool summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const override {
        return player_data_host.summary(difficulty, character, out);
    }
    i32 stage_count() const override { return spell_practice_host.stage_count(); }
    bool stage_available(i32 stage) const override {
        return spell_practice_host.stage_available(stage);
    }
    std::string stage_name(i32 stage) const override {
        return spell_practice_host.stage_name(stage);
    }
    i32 spell_count(i32 stage) const override { return spell_practice_host.spell_count(stage); }
    bool spell_unlocked(i32 stage, i32 slot) const override {
        return spell_practice_host.spell_unlocked(stage, slot);
    }
    std::string spell_name(i32 stage, i32 slot) const override {
        return spell_practice_host.spell_name(stage, slot);
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_id(stage, slot, difficulty);
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_rank(stage, slot, difficulty);
    }
    bool spell_variant_available(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_variant_available(stage, slot, difficulty);
    }
    std::string spell_variant_name(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_variant_name(stage, slot, difficulty);
    }
    bool spell_record(i32 character, i32 id, SpellPracticeRecord& out) const override {
        const auto* own = files.spell_counters(character, id);
        const auto* all = files.spell_counters(4, id);
        if (!own || !all) return false;
        for (i32 mode = 0; mode < 2; ++mode) {
            out.captures[mode] = own->captures[mode];
            out.attempts[mode] = own->attempts[mode];
        }
        out.best_score = own->practice_best_score;
        out.encountered = all->attempts[0] > 0 || all->attempts[1] > 0;
        return true;
    }
    // 接口冲突（SpellPractice 未完成/ BLOCKED）：两个平台的无参
    // character_count() 落到同一个虚槽。
    // 0x453170 开屏硬性要求 5 列，故返回 5；0x456650 的角色数由
    // TitleSpellPracticeMenu 构造器自行钳到 kSpellPracticeCharacters(4)。
    // 已知残留：Spell 浏览时 cycle 仍以 5 为模，会多出一个 phantom 角色；
    // 彻底修复需 game 层给两个接口分型适配（本卡不改 game 文件）。
    i32 character_count() const override { return kPlayerDataCharacters; }
    std::string character_name(i32 character) const override {
        return spell_practice_host.character_name(character);
    }
    // TitleMusicRoomPlatform（musicroom-host.md 八回调；r4/menu-extras §六）
    i32 track_count() const override;
    std::string track_title(i32 track) override;
    std::string track_comment(i32 track, i32 line) override;
    bool track_heard(i32 track) const override;
    void mark_heard(i32 track) override;      // 0x4554f0 Z 路径落档
    bool play_track(i32 track) override;      // 0x43adc0(0,曲名)+0x461830(2,0)
    bool play_locked_response() override;     // 0x461830(3/4,0)
    bool resume_title_music() override;       // 0x43adc0(0,"th13_00")
    // MusicRoom 曲目目录（musiccmt.txt '@bgm/名' 块 = 播放名 + 曲名行 + 8
    // 注释行；cp932 字节原样，显示归菜单/字体通道，不在此转码）。空 = 资产
    // 缺席（菜单如实空目录，不伪造曲目）。
    struct MusicRoomTrack {
        std::string name;
        std::string title;
        std::array<std::string, 8> comment;
    };
    std::vector<MusicRoomTrack> music_room;
    void load_music_room_catalog();
    // RunSceneServices
    bool request_pause() override;
    bool request_game_over() override;
    bool request_player_game_over() override;
    bool request_practice_end() override;
    bool request_stage6_fade() override;
    bool request_extra_results() override;
    bool record_spell_event(const SpellEvent&) override;
    void update_spell_time(bool) override;
    i32 spell_elapsed_time() const override { return spell_time_packed; }
    bool record_spell_practice_score(i32 score) override;
    bool record_stage_clear() override;
    bool ordinary_records_dirty = false;
    bool rasterize_spell_name(SpellPresentation&) override;
    void spell_counts(i32, i32& captures, i32& attempts) const override;
    bool hud_sound(i32) override;
    bool hud_sound(i32, float) override;
    bool stop_sound(i32) override;
    bool set_voice_pan(i32, float) override;
    // 对话正文 → text.anm texture0（0x4282b0 四 VM / 0x428ae0 行块）
    bool dialogue_begin() override;
    bool dialogue_line(i32 row, const MsgLine& line, float box_x, float box_y) override;
    bool dialogue_fade(i32 label) override;
    bool dialogue_anchor_y(float) override;
    void dialogue_clear() override;
    void dialogue_end() override;
    bool read_asset(const std::string& name, std::vector<u8>& bytes, bool required = true) override {
        return read(name, bytes, required);
    }
    bool camera(DrawCamera index, bool refresh) override { return views.camera(index, refresh); }
    bool camera_if_changed(DrawCamera index, bool refresh) override {
        return views.camera_if_changed(index, refresh);
    }
    ZunGraphics& stage_graphics() override { return graphics; }
    bool preload_scenery_animation(i32 bank, const std::string& name) override;
    bool queue_music(i32 code, i32 value, const std::string& name) override {
        return audio_device.queue_music(code, value, name) || fail(audio_device.error);
    }
    bool trance_music(bool active) override {
        return audio_device.trance_music(active) || fail(audio_device.error);
    }
    // 0x42b030 的 HUD 侧：无条件重写 script172 标签（Update pass 内调用）。
    bool trance_gauge_refresh() override {
        if (!run) return true;
        return run->hud.trance_label_refresh() || fail(run->hud.error);
    }
    // 0x44ad60 道具得分弹字入池。
    bool score_popup(i32 value, float x, float y, u32 color) override {
        if (!run) return true;
        return run->spawn_score_popup(value, x, y, color);
    }
    // 0x42a430 分发器 case2/case4（front61/63，0x42ee28/0x43d23e/0x43d3a6）
    // 与 0x43d530 吸灵满槽提示（front108/109/110）：经
    // RunScene::spell_presentation 转发到 SpellPresentation 的横幅面；
    // 空 run / 空 spell_presentation 由转发实现返回 true（不制造新失败）。
    bool hud_notice(i32 front_script) override;
    bool hud_spirit_notice(i32 level, float x, float y, float z) override;
    // RunPausePlatform
    bool suspend_music() override;
    bool resume_music() override;
    bool restart_run() override;
    bool return_to_title() override;
    bool continue_run() override;
    bool save_replay() override;
    bool can_save_replay() const override;
    bool save_replay_slot(i32, const std::string&) override;
    // 0x4484d0 保存元数据注入缝（端到端逐字节复刻用）：pinned 非空时其
    // date/slow/stage_field/EOF/version 原样进 encode；nullptr = 生产默认
    // （真实时钟 + 实时 slow_num_/slow_den_ 计数，A 项语义）。生产调用点
    // （RunPause/TitleResultEntry 虚槽）不变，仍走 2 参重载。
    bool save_replay_slot(i32, const std::string&, const ReplaySaveMeta* pinned);
    bool can_open_pause_options() const override;
    bool open_pause_options() override;
    bool change_pause_option(i32, i32) override;
    i32 pause_option_value(i32) const override;
    bool record_result_name(const std::string&) override;
    // 全通结算（0x454050）专用：条目 stage 字节 = 8（表 0x4bb944[8]="Clear"，
    // Extra 全通同 8）；对局内 GameOver 插入（0x43f8f0）仍走 record_result_name
    // 的当前关。共享实现 record_result_entry(name, stage)。
    bool record_clear_result_name(const std::string&) override;
    bool record_result_entry(const std::string& name, i32 stage);
    bool result_needs_name() const override;
    // 0x43f8f0 排名门 + 0x43d6b0 插入预览（复用 result_screen_data 的
    // ties-before-old 插入）：返回 0 基入榜行，不合格 -1。
    i32 result_ranking(TitlePlayerDataScore rows[10]) const override;
    // 0x43e3f0/0x43e700：暂停激活/恢复对对话 VM 面 +0x18 bit1 清/置。
    bool dialogue_visible(bool visible) override;
    // 0x454050 state3：DAT_004be7c8（续关数）==0 → 流程 0xf（0x4549d0
    // 回放保存）；标题战后流程的门（TitleScenePlatform 同名虚槽）。
    bool result_replay_save_available() const override;
    bool result_screen_data(TitleResultScreenData&) const override;
    // EndingPlatform（0x416570 场景宿主面；read_asset/request_destination/
    // queue_music 复用既有同型 override，这里只声明新增面）
    EndingSaveBytes* save_bytes() override { return &ending_save_view; }
    bool fade_out_music(i32 frames) override;
    bool record_music_unlocked(i32 index) override;
    bool advance_sound() override;
    bool load_ending_anm(i32 slot, const std::string& name) override;
    bool cue_ending_anm(i32 vm_slot, i32 bank_slot, i32 script) override;
    void release_ending_anm(i32 slot) override;
    bool initialize_ending_text();
    bool ending_page_reset() override;
    bool ending_page_line(i32 row, u32 color, const std::string& cp932) override;
    bool ending_page_clear() override;
    void release_ending_page() override;
    bool open_ending_window(i32 kind, i32 duration) override;
    bool open_game_fade(bool entering, i32 duration, bool playfield_only) override;
    void clear_ending_windows();
    // 结局字节区视图（0x1b26a..0x1b276 → files.ending_seen_bytes[0..12]）。
    struct EndingSaveMirror final : EndingSaveBytes {
        explicit EndingSaveMirror(std::array<u8, 13>* storage) : bytes(storage) {}
        u8 read(i32 index) const override {
            return bytes && index >= 0 && index < i32(bytes->size())
                       ? (*bytes)[u32(index)] : u8(0);
        }
        void write(i32 index, u8 value) override {
            if (bytes && index >= 0 && index < i32(bytes->size()))
                (*bytes)[u32(index)] = value;
        }
        std::array<u8, 13>* bytes;
    } ending_save_view{&files.ending_seen_bytes};
    // 对局期平台服务（battle 分期前的 HUD 直通；StageBattleServices 直通面）
    bool life_hud(i32, i32) override;
    bool bomb_hud(i32, i32) override;
    bool game_over();
};

}  // namespace th13::sdl

#endif  // TH13_APPLICATION_PLATFORMS_ONLY
