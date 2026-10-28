// Application 聚合根（th15 ApplicationState 裁剪：destination 驱动的场景所有权，
// 装载挡板与 16 槽 projection 保留；practice/checkpoint/ending/replay 不做，
// docs/plans/ui-base.md §四）。
#pragma once

#include "../game/TitlePlayerDataMenu.hpp"
#include "../game/TitleSpellPracticeMenu.hpp"
#include "../game/SpellCatalog.hpp"

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
#include "../game/TitleMusicRoomMenu.hpp"
#include "../game/TitleOptionsMenu.hpp"
#include "../game/DialogueText.hpp"
#include "../game/RunScene.hpp"
#include "../game/RunPause.hpp"
#include "../game/PlayfieldTargets.hpp"

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
    AnmEnvironment environment;
    AnmManager animations{visual, environment};
    DialogueText dialogue_text;  // text.anm scripts0..3 四行 VM + texture0 动态正文
    AnmRenderer renderer{graphics};
    ScreenViews views{renderer, environment};
    AsciiText captions{animations, environment};
    FrameScheduler scheduler;
    GameRandom loading_random;
    AnmEnvironment loading_environment;
    AnmManager loading_animations{loading_random, loading_environment};
    bool platform_prepared = false;
    u32 loading_signature = 0, loading_prayer = 0;
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
    bool stage_transition_loading = false, stage_transition_active = false;
    i32 stage_startup_age = -1;
    i32 next_scenery_bank = 26;
    bool prepare_next_stage();
    bool activate_next_stage();
    bool begin_record_stage();
    std::unique_ptr<ReplayInputCursor> replay_cursor;
    bool replay_active = false;
    i32 replay_catalog_index = 0; // 4525b0 preserves DAT_004dc674 for the return
    bool replay_finished = false;
    u32 replay_consumed = 0;
    GameInput replay_input; // recorded input history, frozen with the stage
    // 回放恢复挂点：begin_replay_run 备好 stage 指针，begin_run 在关首表
    // 重置（0x42b990:103-110 等价）之后、场景构造/ECL 第 0 帧之前应用
    // 0x449070 恢复（种子 + 22 dword）。正常开局/换关为空。
    const ReplayStage* pending_replay_stage = nullptr;
    u32 pending_replay_stage_index = 0;
    bool initialized = false, exiting = false;
    std::array<i32, 16> projected{};
    std::string failure;
    std::unordered_map<std::string, std::vector<u8>> resource_cache;

    bool loading_waiting() const;
    bool complete_loading();
    bool render_loading();
    bool advance_loading(double);
    bool fail(const std::string&);
    bool prepare_platform();
    bool prepare_loading();
    bool draw_loading(bool signature);
    bool initialize(bool);
    ~ApplicationState();
    // Physical input drives menus; Replay gameplay consumes its own stream
    // at the RunScene update gate (00448200/00414f30).
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
    bool record_spell_practice_score(i32 score) override;
    bool record_stage_clear() override;
    bool ordinary_records_dirty = false;
    bool rasterize_spell_name(SpellPresentation&) override;
    void spell_counts(i32, i32& captures, i32& attempts) const override;
    bool hud_sound(i32) override;
    // 对话正文 → text.anm texture0（0x4282b0 四 VM / 0x428ae0 行块）
    bool dialogue_line(i32 row, const MsgLine& line, float box_x, float box_y) override;
    bool dialogue_fade(i32 label) override;
    bool dialogue_anchor_y(float) override;
    void dialogue_clear() override;
    void dialogue_end() override;
    bool read_asset(const std::string& name, std::vector<u8>& bytes, bool required = true) override {
        return read(name, bytes, required);
    }
    bool camera(DrawCamera index, bool refresh) override { return views.camera(index, refresh); }
    ZunGraphics& stage_graphics() override { return graphics; }
    bool preload_scenery_animation(i32 bank, const std::string& name) override;
    bool queue_music(i32 code, i32 value, const std::string& name) override {
        return audio_device.queue_music(code, value, name) || fail(audio_device.error);
    }
    bool trance_music(bool active) override {
        return audio_device.trance_music(active) || fail(audio_device.error);
    }
    // RunPausePlatform
    bool suspend_music() override;
    bool resume_music() override;
    bool restart_run() override;
    bool return_to_title() override;
    bool continue_run() override;
    bool save_replay() override;
    bool can_save_replay() const override;
    bool save_replay_slot(i32, const std::string&) override;
    bool can_open_pause_options() const override;
    bool open_pause_options() override;
    bool change_pause_option(i32, i32) override;
    i32 pause_option_value(i32) const override;
    bool record_result_name(const std::string&) override;
    bool result_needs_name() const override;
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
