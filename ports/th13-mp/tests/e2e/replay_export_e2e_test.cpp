// Native 端到端总验收（Tier 1）：原版 th13_01.rpy 的输入流以「玩家物理输入」
// 身份驱动生产游戏全流程通关，走生产保存路径导出 Replay，与原文件逐字节比对。
//
// 流程（全部生产路径，无平行状态）：
//   boot：ApplicationState::initialize(false)（真实原版资产；AudioDevice 走
//         TH13_AUDIO_MIXER_ONLY 的 initialize_for_test 真资产入口；字体走
//         native_stub 的 freetype 真栅格）→ 确定性 boot 种子（等价 web
//         window.__th13_boot_seed 钩子，game_random/visual ← 原版 stage1 戳）；
//   标题：物理输入驱动 Main→難度(Normal)→角色(Reimu)→start_game →
//         apply_destination → begin_run（生产开局链，含 begin_record_stage）；
//   对局：逐游戏帧把该关 replay 的 held 16 位经 GameInput::update（与键盘同
//         一条转换路径）喂 step()；run->record_input（Application.cpp:808）
//         自然录制 6 字节帧、沿差分与 30 帧慢速率事件。慢事件字节按原文件
//         事件流在步前写 slow_window_fps_（原保存机器的实时 fps 时间线——
//         10/58/59 字节是保存当机的 OS 卡顿，健康 native 机的窗口机制无法
//         再现；与 pinned date/num/den 同属「保存时刻实时量」注入面）；
//   对齐：每关 recorded frame 0 = 生产 begin_record_stage 帧。录制块归属：
//         关间 0xc 门内帧（stage_transition_active）属递增前关卡（录制块仍
//         为旧块，progress.stage 已递增）；activate 帧（age==30）的场景更新
//         已属新关 frame 0。边界断言上一关录制帧数 == 原文件该关帧数；
//   结算：stage6 clear → Ending（Z 连打推进）→ AfterEnding → begin_title(3)
//         → ResultName（0x454050，END 录名）→ result_replay_save_available
//         门 → ResultReplaySlot（0x4549d0）→ save_replay_slot(1, "ARINATEI",
//         pinned)（B 项注入缝；date/slow 自原文件反解）；
//   验收：读回 CWD 的 th13_01.rpy 与 games/replays/th13_01.rpy 整文件逐字节
//         比对（首个分歧给偏移+两侧 hex+字段域）；断言 FileStore v6 榜表经
//         record_clear_result_name 落了本局成绩面。
#include "../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "game/ReplayRecorder.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

namespace {

using namespace th13;
using th13::sdl::ApplicationState;

// ---- 样本定位（同 tests/replay 的向上探测；CWD = run.sh 的 scratch 目录，
//       仓根以 TH13_NATIVE_REPO_ROOT 注入）----
const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

bool read_repo_file(const std::string& relative, std::vector<u8>& out) {
    std::FILE* file = std::fopen((std::string(repo_root()) + "/" + relative).c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

u32 read_u32(const u8* p) {
    return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24;
}
i32 read_i32(const u8* p) { return i32(read_u32(p)); }

// 原保存机器时区（与 replay_encode_roundtrip_test 同法）：USER Date 行与
// +0x0C time_t 的差反推 POSIX TZ，进程内 setenv+tzset 建模。
bool set_replay_timezone(const std::string& date_line, std::time_t stamp) {
    if (date_line.size() < 14) return false;
    std::tm tm{};
    tm.tm_year = 100 + std::atoi(date_line.substr(0, 2).c_str());
    tm.tm_mon = std::atoi(date_line.substr(3, 2).c_str()) - 1;
    tm.tm_mday = std::atoi(date_line.substr(6, 2).c_str());
    tm.tm_hour = std::atoi(date_line.substr(9, 2).c_str());
    tm.tm_min = std::atoi(date_line.substr(12, 2).c_str());
    tm.tm_isdst = -1;
    const std::time_t as_utc = timegm(&tm);
    if (as_utc == std::time_t(-1)) return false;
    const long long minute = static_cast<long long>(stamp) -
                             static_cast<long long>(stamp) % 60;
    const long long offset = static_cast<long long>(as_utc) - minute;
    if (offset % 3600 != 0) return false;
    const long long hours = offset / 3600;
    char tz[32];
    const char sign = hours > 0 ? '-' : '+';
    const long long abs_hours = hours < 0 ? -hours : hours;
    std::snprintf(tz, sizeof tz, "UTC%c%lld", sign, abs_hours);
    setenv("TZ", tz, 1);
    ::tzset();
    return true;
}

// USER 摘要（SJIS，ASCII 行可按字节扫）行值。
std::string user_line(const std::string& text, const char* key) {
    const std::string k = std::string(key) + " ";
    size_t at = text.find(k);
    if (at != 0) {
        at = text.find(std::string("\r\n") + k);
        if (at == std::string::npos) return "";
        at += 2;
    }
    size_t end = text.find("\r\n", at + k.size());
    if (end == std::string::npos) end = text.size();
    return text.substr(at + k.size(), end - (at + k.size()));
}

constexpr u32 kZ = 0x01;  // 0x4e49f0 键位 bit0 = Z 射击/确认

// live 臂逐帧状态转储（TH13_E2E_STATE_LOG；诊断用，不影响验收面）。
std::FILE* g_state_log = nullptr;
bool g_last_battle = false;  // 本步镜像是否计为录制帧（诊断列）

// ---- 对局录制镜像：每场景更新恰一录制帧（RunScene::update 四早退门同条
//       件），录制块归属按关间门态映射。----
struct Driver {
    ApplicationState* state = nullptr;
    const ReplayFile* original = nullptr;
    std::array<u32, 8> recorded{};   // 按关号 1..6
    bool overrun = false;
    u32 overrun_stage = 0;
    // 对齐探针（TH13_E2E_SHIFT）：喂入帧 = 镜像帧 + shift。0 = 生产对齐语义。
    int shift = 0;
    // 步前观察（feed 与记账共用）。boot_ = 本步 complete_loading 将创建 run
    //（pending_load 由上一帧尾的 destination Run 路径置位）——该步的场景更新
    // 即 frame 0，喂入与记账必须在本步生效。
    bool run_ = false, boot_ = false, ending_apply_ = false, replay_ = false;
    bool paused_ = false;
    bool overlays_ = false, battle_ = false, activate_ = false;
    i32 block_ = 0;  // 本步场景更新归属的录制块（关号）

    void observe() {
        run_ = state->run != nullptr;
        // boot_ = 本步 complete_loading 将创建 run 并录 frame 0（pending_load
        // 由上一帧尾 destination Run 路径置位）。ending 收尾步（pending_load
        // + destination Ending + run 仍在观察点）在本步 complete_loading 内
        // release_run，场景更新不再发生，不录制。
        boot_ = !run_ && state->pending_load != 0 &&
                state->pending_destination == i32(sdl::Destination::Run);
        ending_apply_ = run_ && state->pending_load != 0 &&
                        state->pending_destination == i32(sdl::Destination::Ending);
        replay_ = state->replay_active;
        paused_ = state->environment.paused;
        const u32 flags = state->progress.scene_flags;
        overlays_ = (flags & 0x70u) != 0u || (flags & 0x4010u) == 0x4010u;
        battle_ = (run_ || boot_) && !ending_apply_ && !replay_ && !paused_ && !overlays_;
        activate_ = run_ && battle_ &&
                    state->stage_transition_active && !state->stage_transition_loading &&
                    state->stage_startup_age == 30;
        // 门内帧录制块 = 递增前关卡。门首步（prepare_next_stage 在本步开头
        // 跑，观察点 stage_transition_active 尚为 false）以
        // stage_transition_loading 识别；其后门内步以 stage_transition_active
        // 识别。activate 步的场景更新已属新关 frame 0，不递减。
        block_ = state->progress.stage;
        if (!activate_ && block_ >= 2 &&
            (state->stage_transition_active || state->stage_transition_loading))
            --block_;
    }

    // 步前喂入：对局录制帧 → 该关 replay 的 held 16 位；慢事件字节写
    // slow_window_fps_（生产 record_input 读取面）。非对局步返回 0。
    u32 feed() {
        if (!battle_ || block_ < 1 || block_ > 6) return 0;
        const ReplayStage& source = original->stages[u32(block_ - 1)];
        const u32 fc = activate_ ? 0u : recorded[u32(block_)];
        const i64 probe = i64(fc) + shift;
        if (probe < 0 || u64(probe) >= source.frame_count) {
            if (!overrun) {
                overrun = true;
                overrun_stage = u32(block_);
                std::printf("# overrun: stage %u recorded %u >= original %u; "
                            "keeping zero input to measure the overflow\n",
                            overrun_stage, fc, source.frame_count);
            }
            return 0;
        }
        const u32 tick = u32(probe) / 30;
        if (tick < source.events.size())
            state->slow_window_fps_ = float(source.events[tick]);
        return source.frame(u32(probe)).state;
    }

    void bookkeep() {
        if (battle_ && block_ >= 1 && block_ <= 7) recorded[u32(block_)]++;
    }

    // 诊断面：不改状态地看一眼本步将喂入的 held。
    u32 feed_probe() {
        observe();
        return feed();
    }
};

// 驱动一步（物理输入 → GameInput::update 生产转换 → ApplicationState::step）。
// 与浏览器宿主同构：全程恰一个 GameInput 实例（0x4e49f0 等价），held 逐帧
// update 产生沿——不得每帧新建（否则每帧都是全量按下沿，作战/对话语义全变）。
// TH13_E2E_STATE_LOG 打开时逐帧转储对局状态（与 replay 诊断臂同格式）。
bool pump(ApplicationState& state, u32 held, u32& steps) {
    static GameInput input;  // 唯一实例（BrowserRuntime 的 controls 同构）
    input.update(held);
    ++steps;
    if (!state.step(input, 60.0f, false)) {
        std::printf("# step %u failed: %s\n", steps, state.failure.c_str());
        return false;
    }
    if (g_state_log && state.run) {
        std::fprintf(g_state_log, "%d %u %u %d %d %u %x %d %d %zu\n",
                     state.progress.stage_frame,
                     float_to_bits(state.run->battle.player_x),
                     float_to_bits(state.run->battle.player_y),
                     state.selection_score.score, state.selection_player.extra_lives,
                     state.game_random.seed(), state.progress.scene_flags,
                     g_last_battle ? 1 : 0, state.pending_destination,
                     state.ending_fades.size());
        std::fflush(g_state_log);
    }
    return true;
}

// 标题菜单物理导航：Main→難度(Normal)→角色(Reimu)→开局（生产 start_game 链）。
bool navigate_to_run(ApplicationState& state, u32& steps) {
    for (int i = 0; i < 600; ++i) {
        if (state.title && state.title->state.screen == TitleScreen::Main &&
            state.title->state.substate == 2)
            break;
        if (!pump(state, 0, steps)) return false;
    }
    if (!state.title || state.title->state.screen != TitleScreen::Main ||
        state.title->state.substate != 2) {
        std::printf("# title main menu never became interactive\n");
        return false;
    }
    const auto confirm_and_wait = [&](TitleScreen next) {
        if (!pump(state, kZ, steps)) return false;  // 确认脉冲（一帧按下沿）
        for (int i = 0; i < 240; ++i) {
            // pending_load 置位 = 下一帧 complete_loading 建 run 并录 frame 0，
            // 必须把这一帧让给主循环的对局喂入（提前一步退出，不吃掉 frame 0）。
            if (state.pending_load != 0 || (next == TitleScreen::StartGame && state.run))
                return true;
            if (state.title && state.title->state.screen == next &&
                state.title->state.substate == 2)
                return true;
            if (!pump(state, 0, steps)) return false;
        }
        std::printf("# menu wait for screen %d timed out (screen=%d substate=%d)\n",
                    int(next),
                    state.title ? int(state.title->state.screen) : -1,
                    state.title ? state.title->state.substate : -1);
        return false;
    };
    if (!confirm_and_wait(TitleScreen::Difficulty)) return false;  // 開始 → 難度
    if (!confirm_and_wait(TitleScreen::Character)) return false;   // Normal(默认) → 角色
    if (!confirm_and_wait(TitleScreen::StartGame)) return false;   // Reimu(默认) → 开局
    if (!state.run && state.pending_load == 0) {
        std::printf("# run never started\n");
        return false;
    }
    return true;
}

// Ending 场景（run==null、ending!=null）：Z 连打推进对话/滚屏直至战后标题。
bool pump_ending(ApplicationState& state, u32& steps) {
    u32 toggle = 0;
    for (int i = 0; i < 240000; ++i) {
        if (state.title) return true;  // AfterEnding → begin_title(3)
        if (!state.ending) {
            std::printf("# ending vanished without title (dest=%d failure=%s)\n",
                        state.pending_destination, state.failure.c_str());
            return false;
        }
        const u32 held = (toggle++ & 1) ? kZ : 0u;  // 隔帧按下沿，快进对话
        if (!pump(state, held, steps)) return false;
    }
    std::printf("# ending never finished\n");
    return false;
}

TEST(native_th13_01_input_stream_replays_and_exports_byte_identical_file) {
    // ---- 0) 样本装载与注入量反解 ----
    std::vector<u8> bytes;
    CHECK(read_repo_file("games/replays/th13_01.rpy", bytes));
    if (bytes.empty()) return;
    ReplayFile original;
    CHECK(original.decode(bytes.data(), bytes.size()));
    if (original.stages.size() != 6) {
        std::printf("# unexpected stage count %zu\n", original.stages.size());
        return;
    }
    u64 stamp = 0;
    for (int i = 0; i < 8; ++i) stamp |= u64(original.top_header[0x0C + i]) << (8 * i);
    const i32 stage_field = read_i32(original.top_header.data() + 0x68);
    const u16 boot_seed = u16(original.stages[0].header[2] |
                              (original.stages[0].header[3] << 8));
    const std::string summary = original.user_blocks.empty() ? std::string()
                                                             : original.user_blocks[0];
    CHECK(set_replay_timezone(user_line(summary, "Date"), std::time_t(stamp)));

    // +0x54 f32 反解 (num, den)（replay_encode_roundtrip_test 同法）。
    ReplaySaveMeta pinned;
    pinned.date = std::time_t(stamp);
    pinned.stage_field = stage_field;
    pinned.version = "1.00c";
    {
        const double x = double(original.slow_rate);
        const float r = float((100.0 - x) / 100.0);
        constexpr double kScale = 1099511627776.0;  // 2^40
        pinned.slow_numerator = double((long long)(r * kScale + (r >= 0 ? 0.5 : -0.5)));
        pinned.slow_denominator = kScale;
    }
    std::array<u32, 8> original_frames{};
    for (size_t i = 0; i < original.stages.size() && i < 7; ++i)
        original_frames[i + 1] = original.stages[i].frame_count;

    // ---- 1) 生产宿主 boot（真实资产；确定性 boot 种子）----
    setenv("TH13_SAVE_PATH", "e2e-th13-save.bin", 1);  // 缺档 = 新档默认值
    ApplicationState state;
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    CHECK(asset_root && *asset_root);
    if (!(asset_root && *asset_root)) return;
    // B' 符卡经过时间钉住：0x412ee0 的经过时间取实时钟（QPC），原保存机器
    // 的墙钟值与 date/slow 同属不可再现实时量 → 自原文件 stage 头 +0x70..
    // 逐槽置换（verbatim 含 0x6ad1584 哨兵）。
    state.spell_time_pin_enabled_ = true;
    // B' 顶层头未映射域 verbatim（0x4484d0 只覆写已名字段）。
    state.replay_top_pin_ = original.top_header;
    state.replay_top_pin_enabled_ = state.replay_top_pin_.size() == 0x74;
    for (size_t s = 0; s < original.stages.size() && s < 7; ++s) {
        const u32 stage_no = original.stages[s].number;
        if (stage_no < 1 || stage_no > 7) continue;
        for (u32 k = 0; k < 21; ++k) {
            const size_t off = 0x70 + size_t(k) * 4;
            i32 value = 0;
            if (off + 4 <= original.stages[s].header.size())
                std::memcpy(&value, original.stages[s].header.data() + off, 4);
            state.spell_time_pin_[stage_no][k] = value;
        }
    }
    CHECK(state.audio_device.initialize_for_test(
        asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
    if (!state.audio_device.error.empty())
        std::printf("# audio init error: %s\n", state.audio_device.error.c_str());
    // 等价 web window.__th13_boot_seed 确定性钩子：boot 双流 ← 原版 stage1 戳
    // （visual 另按 th13_debug_replay 同款补 0x45ac90 噪声表 256 步 + boot 2 抽，
    //  对局开始前菜单期不消费 game_random；具体落拍在 initialize 之后——
    //  prepare_platform 会以 SDL_GetTicks 桩值重播种子）。
    CHECK(state.initialize(false));
    if (!state.failure.empty()) {
        std::printf("# initialize failed: %s\n", state.failure.c_str());
        return;
    }
    std::printf("# fonts_failure='%s'\n", state.fonts_failure.c_str());
    // initialize（prepare_platform）以 SDL_GetTicks 桩值重播双流种子，这里
    // 再落确定性 boot 种子（= web __th13_boot_seed 钩子的 native 等价拍）。
    state.visual.reseed_restore(boot_seed);
    for (int i = 0; i < 256; ++i) (void)state.visual.step16();
    (void)state.visual.next32();
    (void)state.visual.next32();
    state.game_random.reseed(boot_seed);

    // ---- 2) 标题物理导航 → 生产开局链 ----
    u32 steps = 0;
    CHECK(navigate_to_run(state, steps));
    if (!state.run && state.pending_load == 0) return;
    std::printf("# run started at step %u (stage %d seed %u want %u)\n", steps,
                state.progress.stage, state.game_random.seed(), unsigned(boot_seed));

    // ---- 3) 全流程驱动：6 关对局 → Ending → 战后结算 ----
    Driver driver;
    driver.state = &state;
    driver.original = &original;
    if (const char* shift_env = std::getenv("TH13_E2E_SHIFT"))
        driver.shift = std::atoi(shift_env);
    u32 max_steps = 480000u;
    if (const char* max_env = std::getenv("TH13_E2E_MAX_STEPS"))
        max_steps = u32(std::atoi(max_env));
    std::printf("# driver shift=%d max_steps=%u\n", driver.shift, max_steps);
    if (const char* log_path = std::getenv("TH13_E2E_STATE_LOG");
        log_path && *log_path)
        g_state_log = std::fopen(log_path, "w");
    enum class Phase { Battle, Ending, Result } phase = Phase::Battle;
    int result_waits = 0;
    bool saved = false;
    while (steps < max_steps) {
        if (steps % 20000u == 0u)
            std::printf("# step %u stage=%d sf=%d flags=%x paused=%d run=%d end=%d "
                        "title=%d rec=[%u %u %u %u %u %u] feed_held=0x%x\n",
                        steps, state.progress.stage, state.progress.stage_frame,
                        state.progress.scene_flags, int(state.environment.paused),
                        state.run != nullptr, state.ending != nullptr,
                        state.title != nullptr, driver.recorded[1], driver.recorded[2],
                        driver.recorded[3], driver.recorded[4], driver.recorded[5],
                        driver.recorded[6], driver.feed_probe());
        driver.observe();
        g_last_battle = driver.battle_;
        if (driver.activate_ && driver.block_ >= 2 && driver.block_ <= 6) {
            // activate 帧：旧关块已定帧数 → 关边界对齐断言。
            std::printf("# stage %d recorded %u frames (original %u)\n",
                        driver.block_ - 1, driver.recorded[u32(driver.block_ - 1)],
                        original_frames[u32(driver.block_ - 1)]);
            CHECK_EQ(driver.recorded[u32(driver.block_ - 1)],
                     original_frames[u32(driver.block_ - 1)]);
        }
        u32 held = 0;
        if (phase == Phase::Battle) held = driver.feed();
        if (!pump(state, held, steps)) return;
        driver.bookkeep();
        if (!state.failure.empty()) {
            std::printf("# failure at step %u: %s\n", steps, state.failure.c_str());
            CHECK(state.failure.empty());
            return;
        }
        if (driver.overrun && state.progress.stage == i32(driver.overrun_stage)) {
            // 超帧后每 300 步报一次（量化溢出；若关界仍未到则由步数上限兜底）。
            if ((steps / 300u) != (u32(steps - 1) / 300u) &&
                state.progress.stage_frame % 300u == 0)
                std::printf("# overrun pending: stage %d stage_frame=%d score=%d "
                            "lives=%d paused=%d\n",
                            state.progress.stage, state.progress.stage_frame,
                            state.selection_score.score,
                            state.selection_player.extra_lives,
                            int(state.environment.paused));
        }
        if (phase == Phase::Battle && state.environment.paused && state.run &&
            !state.replay_active) {
            // 对局中覆盖层（1CC 无暂停/续关）：录制死亡/终局点并停止，
            // 不空转至步数上限。
            std::printf("# battle overlay at step %u: stage=%d recorded=%u "
                        "stage_frame=%d shift=%d (player died / game over)\n",
                        steps, state.progress.stage,
                        driver.recorded[size_t(std::max(1, state.progress.stage))],
                        state.progress.stage_frame, driver.shift);
            std::printf("# player at %.2f,%.2f score=%d lives=%d\n",
                        state.run->battle.player_x, state.run->battle.player_y,
                        state.selection_score.score, state.selection_player.extra_lives);
            CHECK(false);
            return;
        }
        if (phase == Phase::Battle && state.ending) {
            std::printf("# stage 6 recorded %u frames (original %u)\n",
                        driver.recorded[6], original_frames[6]);
            CHECK_EQ(driver.recorded[6], original_frames[6]);
            phase = Phase::Ending;
        }
        if (phase == Phase::Ending) {
            if (!pump_ending(state, steps)) return;
            phase = Phase::Result;
            continue;
        }
        if (phase == Phase::Result) {
            if (state.title && state.title->state.screen == TitleScreen::ResultName) {
                // 0x454050 case2：END（cursor 90 默认）一拍确认录名（空名）。
                if (state.title->state.substate == 2) {
                    if (!pump(state, kZ, steps)) return;
                    continue;
                }
                if (!pump(state, 0, steps)) return;  // case1/case3 过渡
                continue;
            }
            if (state.title && state.title->state.screen == TitleScreen::ResultReplaySlot) {
                if (state.title->state.substate < 2) {
                    if (!pump(state, 0, steps)) return;
                    continue;
                }
                // 0x4549d0 门（续关数 0 + 录像就绪）确认后走 B 项注入缝保存。
                CHECK(state.result_replay_save_available());
                CHECK_EQ(state.title->state.result_slot, 0);  // 默认槽 → th13_01.rpy
                saved = state.save_replay_slot(1, "ARINATEI", &pinned);
                if (!saved) std::printf("# save failed: %s\n", state.failure.c_str());
                CHECK(saved);
                break;
            }
            if (++result_waits > 4000) {
                std::printf("# result flow stalled (screen=%d substate=%d)\n",
                            state.title ? int(state.title->state.screen) : -1,
                            state.title ? state.title->state.substate : -1);
                CHECK(false);
                return;
            }
            if (!pump(state, 0, steps)) return;
        }
    }
    CHECK(saved);
    if (!saved) return;
    std::printf("# total steps %u\n", steps);
    if (g_state_log) { std::fclose(g_state_log); g_state_log = nullptr; }

    // ---- 4) v6 得分面断言（record_clear_result_name → FileStore 榜表）----
    {
        bool found = false;
        int stage_byte = -1;
        const auto& ranking = state.files.ordinary_records[0].ranking[1];  // Reimu/Normal
        for (const auto& entry : ranking) {
            if (entry.available && entry.score == original.score) {
                found = true;
                stage_byte = entry.stage;
                break;
            }
        }
        std::printf("# v6 record face: found=%d stage=%d (score %d)\n", int(found),
                    stage_byte, original.score);
        CHECK(found);
        CHECK_EQ(stage_byte, 8);  // 全通条目 stage 字节 = 8（0x454050 → "Clear"）
    }

    // ---- 5) 导出文件整字节比对（验收线）。生产 native 保存路径写在 CWD。----
    std::vector<u8> exported;
    {
        std::FILE* file = std::fopen("th13_01.rpy", "rb");
        CHECK(file != nullptr);
        if (file) {
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            exported.resize(size_t(size < 0 ? 0 : size));
            const size_t got = exported.empty()
                                   ? 1
                                   : std::fread(exported.data(), 1, exported.size(), file);
            std::fclose(file);
            CHECK(got == exported.size());
        }
    }
    if (exported.empty()) {
        std::printf("# exported th13_01.rpy missing in CWD\n");
        return;
    }
    const size_t common = exported.size() < bytes.size() ? exported.size() : bytes.size();
    size_t divergence = common;
    for (size_t b = 0; b < common; ++b) {
        if (exported[b] != bytes[b]) {
            divergence = b;
            break;
        }
    }
    const bool identical = exported.size() == bytes.size() && divergence == exported.size();
    std::printf("# export %zu bytes vs original %zu bytes, first_diff=%zu %s\n",
                exported.size(), bytes.size(), divergence,
                identical ? "BYTE-IDENTICAL" : "DIVERGED");
    if (!identical) {
        const size_t at = divergence;
        std::printf("#   got  @%zu:", at);
        for (size_t b = at; b < exported.size() && b < at + 16; ++b)
            std::printf(" %02X", exported[b]);
        std::printf("\n#   want @%zu:", at);
        for (size_t b = at; b < bytes.size() && b < at + 16; ++b)
            std::printf(" %02X", bytes[b]);
        std::printf("\n");
        const u32 user_off = read_u32(bytes.data() + 0x0C);
        const char* region = at < 0x24 ? "file header"
                                       : at < size_t(user_off)
                                             ? "cipher blob (plain replay stream)"
                                             : "USER section";
        std::printf("#   region: %s (user_off=0x%X)\n", region, user_off);
        ReplayFile produced;
        if (produced.decode(exported.data(), exported.size())) {
            std::printf("#   produced name=%s score=%d slow=%.7f stages=%zu\n",
                        produced.player_name.c_str(), produced.score,
                        double(produced.slow_rate), produced.stages.size());
            for (size_t i = 0; i < produced.stages.size(); ++i)
                std::printf("#   stage %u frames %u (orig %u) score %u (orig %u)\n",
                            produced.stages[i].number, produced.stages[i].frame_count,
                            original.stages[i].frame_count, produced.stages[i].score,
                            original.stages[i].score);
        } else {
            std::printf("#   produced file failed to decode: %s\n",
                        produced.error().c_str());
        }
    }
    CHECK(identical);
}

// 诊断臂（TH13_E2E_MODE=replay 时运行）：同一二进制内走生产 begin_replay_run
// 回放路径（0x448c40 装载序 + 0x449070 恢复 + 流内输入），逐帧转储对局状态，
// 与 live 驱动臂的转储对拍定位分歧源。非验收路径。
TEST(replay_mode_state_probe) {
    if (const char* mode = std::getenv("TH13_E2E_MODE"); !mode || std::string(mode) != "replay")
        return;
    const char* log_path = std::getenv("TH13_E2E_STATE_LOG");
    if (!log_path || !*log_path) return;
    std::FILE* log = std::fopen(log_path, "w");
    CHECK(log != nullptr);
    if (!log) return;

    std::vector<u8> bytes;
    CHECK(read_repo_file("games/replays/th13_01.rpy", bytes));
    if (bytes.empty()) { std::fclose(log); return; }
    ReplayFile original;
    CHECK(original.decode(bytes.data(), bytes.size()));
    const u16 boot_seed = u16(original.stages[0].header[2] |
                              (original.stages[0].header[3] << 8));

    setenv("TH13_SAVE_PATH", "e2e-th13-save.bin", 1);
    ApplicationState state;
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    CHECK(asset_root && *asset_root);
    if (!(asset_root && *asset_root)) { std::fclose(log); return; }
    CHECK(state.audio_device.initialize_for_test(
        asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
    CHECK(state.initialize(false));
    if (!state.failure.empty()) {
        std::printf("# initialize failed: %s\n", state.failure.c_str());
        std::fclose(log);
        return;
    }
    // th13_debug_replay 同款直入序（native boot 血统 + 装载幕 + 3 装载帧）。
    state.visual.reseed_restore(boot_seed);
    for (int i = 0; i < 256; ++i) (void)state.visual.step16();
    (void)state.visual.next32();
    (void)state.visual.next32();
    state.game_random.reseed(boot_seed);
    CHECK(state.draw_loading(false));
    for (unsigned frame = 0;
         frame < th13::sdl::ApplicationState::kDebugLoadingFrames; ++frame)
        CHECK(state.advance_loading_caption());
    CHECK(state.begin_replay_run(bytes.data(), bytes.size(), 0));
    if (!state.failure.empty()) {
        std::printf("# begin_replay_run failed: %s\n", state.failure.c_str());
        std::fclose(log);
        return;
    }

    GameInput input;
    input.update(0);
    const u32 total = original.stages[0].frame_count + 120u;
    i32 last_stage = 1;
    u32 consumed_snapshot = 0;
    for (u32 f = 0; f < total; ++f) {
        if (!state.step(input, 60.0f, false)) {
            std::fprintf(log, "fail %u %s\n", f, state.failure.c_str());
            break;
        }
        if (state.progress.stage != last_stage) {
            // 关界：上一关回放游标实耗 = 该关实录制帧数（0x448200 每录制帧
            // 恰一消费；activate 已把 replay_consumed 清零，取切换前快照）。
            std::printf("# probe: stage %d -> %d at step %u, consumed=%u\n",
                        last_stage, state.progress.stage, f, consumed_snapshot);
            last_stage = state.progress.stage;
            consumed_snapshot = 0;
        }
        if (state.progress.stage == last_stage)
            consumed_snapshot = state.replay_consumed;
        if (state.run) {
            const u32 px_bits = float_to_bits(state.run->battle.player_x);
            const u32 py_bits = float_to_bits(state.run->battle.player_y);
            std::fprintf(log, "%d %u %u %d %d %u\n", state.progress.stage_frame,
                         px_bits, py_bits, state.selection_score.score,
                         state.selection_player.extra_lives, state.game_random.seed());
        } else {
            std::fprintf(log, "norun %u\n", f);
            break;
        }
    }
    std::fclose(log);
    std::printf("# state log written to %s\n", log_path);
}

}  // namespace

int main() { return th13test::run_all("native e2e: th13_01 输入流全流程→导出逐字节复刻"); }
