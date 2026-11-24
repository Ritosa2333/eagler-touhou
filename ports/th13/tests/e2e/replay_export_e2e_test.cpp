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

u32 read_u32(const u8* p) { return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24; }
i32 read_i32(const u8* p) { return i32(read_u32(p)); }

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
    const long long minute = static_cast<long long>(stamp) - static_cast<long long>(stamp) % 60;
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

constexpr u32 kZ = 0x01;

std::FILE* g_state_log = nullptr;
bool g_last_battle = false;

struct Driver {
    ApplicationState* state = nullptr;
    const ReplayFile* original = nullptr;
    std::array<u32, 8> recorded{};
    bool overrun = false;
    u32 overrun_stage = 0;
    int shift = 0;
    bool run_ = false, boot_ = false, ending_apply_ = false, replay_ = false;
    bool paused_ = false;
    bool overlays_ = false, battle_ = false, activate_ = false;
    i32 block_ = 0;

    void observe() {
        run_ = state->run != nullptr;
        boot_ = !run_ && state->pending_load != 0 && state->pending_destination == i32(sdl::Destination::Run);
        ending_apply_ = run_ && state->pending_load != 0 && state->pending_destination == i32(sdl::Destination::Ending);
        replay_ = state->replay_active;
        paused_ = state->environment.paused;
        const u32 flags = state->progress.scene_flags;
        overlays_ = (flags & 0x70u) != 0u || (flags & 0x4010u) == 0x4010u;
        battle_ = (run_ || boot_) && !ending_apply_ && !replay_ && !paused_ && !overlays_;
        activate_ = run_ && battle_ && state->stage_transition_active && !state->stage_transition_loading &&
                    state->stage_startup_age == 30;
        block_ = state->progress.stage;
        if (!activate_ && block_ >= 2 && (state->stage_transition_active || state->stage_transition_loading)) --block_;
    }

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
        if (tick < source.events.size()) state->slow_window_fps_ = float(source.events[tick]);
        return source.frame(u32(probe)).state;
    }

    void bookkeep() {
        if (battle_ && block_ >= 1 && block_ <= 7) recorded[u32(block_)]++;
    }

    u32 feed_probe() {
        observe();
        return feed();
    }
};

bool pump(ApplicationState& state, u32 held, u32& steps) {
    static GameInput input;
    input.update(held);
    ++steps;
    if (!state.step(input, 60.0f, false)) {
        std::printf("# step %u failed: %s\n", steps, state.failure.c_str());
        return false;
    }
    if (g_state_log && state.run) {
        std::fprintf(g_state_log, "%d %u %u %d %d %u %x %d %d %zu\n", state.progress.stage_frame,
                     float_to_bits(state.run->battle.player_x), float_to_bits(state.run->battle.player_y),
                     state.selection_score.score, state.selection_player.extra_lives, state.game_random.seed(),
                     state.progress.scene_flags, g_last_battle ? 1 : 0, state.pending_destination,
                     state.ending_fades.size());
        std::fflush(g_state_log);
    }
    return true;
}

bool navigate_to_run(ApplicationState& state, u32& steps) {
    for (int i = 0; i < 600; ++i) {
        if (state.title && state.title->state.screen == TitleScreen::Main && state.title->state.substate == 2) break;
        if (!pump(state, 0, steps)) return false;
    }
    if (!state.title || state.title->state.screen != TitleScreen::Main || state.title->state.substate != 2) {
        std::printf("# title main menu never became interactive\n");
        return false;
    }
    const auto confirm_and_wait = [&](TitleScreen next) {
        if (!pump(state, kZ, steps)) return false;
        for (int i = 0; i < 240; ++i) {
            if (state.pending_load != 0 || (next == TitleScreen::StartGame && state.run)) return true;
            if (state.title && state.title->state.screen == next && state.title->state.substate == 2) return true;
            if (!pump(state, 0, steps)) return false;
        }
        std::printf("# menu wait for screen %d timed out (screen=%d substate=%d)\n", int(next),
                    state.title ? int(state.title->state.screen) : -1, state.title ? state.title->state.substate : -1);
        return false;
    };
    if (!confirm_and_wait(TitleScreen::Difficulty)) return false;
    if (!confirm_and_wait(TitleScreen::Character)) return false;
    if (!confirm_and_wait(TitleScreen::StartGame)) return false;
    if (!state.run && state.pending_load == 0) {
        std::printf("# run never started\n");
        return false;
    }
    return true;
}

bool pump_ending(ApplicationState& state, u32& steps) {
    u32 toggle = 0;
    for (int i = 0; i < 240000; ++i) {
        if (state.title) return true;
        if (!state.ending) {
            std::printf("# ending vanished without title (dest=%d failure=%s)\n", state.pending_destination,
                        state.failure.c_str());
            return false;
        }
        const u32 held = (toggle++ & 1) ? kZ : 0u;
        if (!pump(state, held, steps)) return false;
    }
    std::printf("# ending never finished\n");
    return false;
}

TEST(native_th13_01_input_stream_replays_and_exports_byte_identical_file) {
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
    for (int i = 0; i < 8; ++i)
        stamp |= u64(original.top_header[0x0C + i]) << (8 * i);
    const i32 stage_field = read_i32(original.top_header.data() + 0x68);
    const u16 boot_seed = u16(original.stages[0].header[2] | (original.stages[0].header[3] << 8));
    const std::string summary = original.user_blocks.empty() ? std::string() : original.user_blocks[0];
    CHECK(set_replay_timezone(user_line(summary, "Date"), std::time_t(stamp)));

    ReplaySaveMeta pinned;
    pinned.date = std::time_t(stamp);
    pinned.stage_field = stage_field;
    pinned.version = "1.00c";
    {
        const double x = double(original.slow_rate);
        const float r = float((100.0 - x) / 100.0);
        constexpr double kScale = 1099511627776.0;
        pinned.slow_numerator = double((long long)(r * kScale + (r >= 0 ? 0.5 : -0.5)));
        pinned.slow_denominator = kScale;
    }
    std::array<u32, 8> original_frames{};
    for (size_t i = 0; i < original.stages.size() && i < 7; ++i)
        original_frames[i + 1] = original.stages[i].frame_count;

    setenv("TH13_SAVE_PATH", "e2e-th13-save.bin", 1);
    ApplicationState state;
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    CHECK(asset_root && *asset_root);
    if (!(asset_root && *asset_root)) return;
    state.spell_time_pin_enabled_ = true;
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
    CHECK(state.audio_device.initialize_for_test(asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
    if (!state.audio_device.error.empty()) std::printf("# audio init error: %s\n", state.audio_device.error.c_str());
    CHECK(state.initialize(false));
    if (!state.failure.empty()) {
        std::printf("# initialize failed: %s\n", state.failure.c_str());
        return;
    }
    std::printf("# fonts_failure='%s'\n", state.fonts_failure.c_str());
    state.visual.reseed_restore(boot_seed);
    for (int i = 0; i < 256; ++i)
        (void)state.visual.step16();
    (void)state.visual.next32();
    (void)state.visual.next32();
    state.game_random.reseed(boot_seed);

    u32 steps = 0;
    CHECK(navigate_to_run(state, steps));
    if (!state.run && state.pending_load == 0) return;
    std::printf("# run started at step %u (stage %d seed %u want %u)\n", steps, state.progress.stage,
                state.game_random.seed(), unsigned(boot_seed));

    Driver driver;
    driver.state = &state;
    driver.original = &original;
    if (const char* shift_env = std::getenv("TH13_E2E_SHIFT")) driver.shift = std::atoi(shift_env);
    u32 max_steps = 480000u;
    if (const char* max_env = std::getenv("TH13_E2E_MAX_STEPS")) max_steps = u32(std::atoi(max_env));
    std::printf("# driver shift=%d max_steps=%u\n", driver.shift, max_steps);
    if (const char* log_path = std::getenv("TH13_E2E_STATE_LOG"); log_path && *log_path)
        g_state_log = std::fopen(log_path, "w");
    enum class Phase { Battle, Ending, Result } phase = Phase::Battle;
    int result_waits = 0;
    bool saved = false;
    while (steps < max_steps) {
        if (steps % 20000u == 0u)
            std::printf("# step %u stage=%d sf=%d flags=%x paused=%d run=%d end=%d "
                        "title=%d rec=[%u %u %u %u %u %u] feed_held=0x%x\n",
                        steps, state.progress.stage, state.progress.stage_frame, state.progress.scene_flags,
                        int(state.environment.paused), state.run != nullptr, state.ending != nullptr,
                        state.title != nullptr, driver.recorded[1], driver.recorded[2], driver.recorded[3],
                        driver.recorded[4], driver.recorded[5], driver.recorded[6], driver.feed_probe());
        driver.observe();
        g_last_battle = driver.battle_;
        if (driver.activate_ && driver.block_ >= 2 && driver.block_ <= 6) {
            std::printf("# stage %d recorded %u frames (original %u)\n", driver.block_ - 1,
                        driver.recorded[u32(driver.block_ - 1)], original_frames[u32(driver.block_ - 1)]);
            CHECK_EQ(driver.recorded[u32(driver.block_ - 1)], original_frames[u32(driver.block_ - 1)]);
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
            if ((steps / 300u) != (u32(steps - 1) / 300u) && state.progress.stage_frame % 300u == 0)
                std::printf("# overrun pending: stage %d stage_frame=%d score=%d "
                            "lives=%d paused=%d\n",
                            state.progress.stage, state.progress.stage_frame, state.selection_score.score,
                            state.selection_player.extra_lives, int(state.environment.paused));
        }
        if (phase == Phase::Battle && state.environment.paused && state.run && !state.replay_active) {
            std::printf("# battle overlay at step %u: stage=%d recorded=%u "
                        "stage_frame=%d shift=%d (player died / game over)\n",
                        steps, state.progress.stage, driver.recorded[size_t(std::max(1, state.progress.stage))],
                        state.progress.stage_frame, driver.shift);
            std::printf("# player at %.2f,%.2f score=%d lives=%d\n", state.run->battle.player_x,
                        state.run->battle.player_y, state.selection_score.score, state.selection_player.extra_lives);
            CHECK(false);
            return;
        }
        if (phase == Phase::Battle && state.ending) {
            std::printf("# stage 6 recorded %u frames (original %u)\n", driver.recorded[6], original_frames[6]);
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
                if (state.title->state.substate == 2) {
                    if (!pump(state, kZ, steps)) return;
                    continue;
                }
                if (!pump(state, 0, steps)) return;
                continue;
            }
            if (state.title && state.title->state.screen == TitleScreen::ResultReplaySlot) {
                if (state.title->state.substate < 2) {
                    if (!pump(state, 0, steps)) return;
                    continue;
                }
                CHECK(state.result_replay_save_available());
                CHECK_EQ(state.title->state.result_slot, 0);
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
    if (g_state_log) {
        std::fclose(g_state_log);
        g_state_log = nullptr;
    }

    {
        bool found = false;
        int stage_byte = -1;
        const auto& ranking = state.files.ordinary_records[0].ranking[1];
        for (const auto& entry : ranking) {
            if (entry.available && entry.score == original.score) {
                found = true;
                stage_byte = entry.stage;
                break;
            }
        }
        std::printf("# v6 record face: found=%d stage=%d (score %d)\n", int(found), stage_byte, original.score);
        CHECK(found);
        CHECK_EQ(stage_byte, 8);
    }

    std::vector<u8> exported;
    {
        std::FILE* file = std::fopen("th13_01.rpy", "rb");
        CHECK(file != nullptr);
        if (file) {
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            exported.resize(size_t(size < 0 ? 0 : size));
            const size_t got = exported.empty() ? 1 : std::fread(exported.data(), 1, exported.size(), file);
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
    std::printf("# export %zu bytes vs original %zu bytes, first_diff=%zu %s\n", exported.size(), bytes.size(),
                divergence, identical ? "BYTE-IDENTICAL" : "DIVERGED");
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
        const char* region = at < 0x24               ? "file header"
                             : at < size_t(user_off) ? "cipher blob (plain replay stream)"
                                                     : "USER section";
        std::printf("#   region: %s (user_off=0x%X)\n", region, user_off);
        ReplayFile produced;
        if (produced.decode(exported.data(), exported.size())) {
            std::printf("#   produced name=%s score=%d slow=%.7f stages=%zu\n", produced.player_name.c_str(),
                        produced.score, double(produced.slow_rate), produced.stages.size());
            for (size_t i = 0; i < produced.stages.size(); ++i)
                std::printf("#   stage %u frames %u (orig %u) score %u (orig %u)\n", produced.stages[i].number,
                            produced.stages[i].frame_count, original.stages[i].frame_count, produced.stages[i].score,
                            original.stages[i].score);
        } else {
            std::printf("#   produced file failed to decode: %s\n", produced.error().c_str());
        }
    }
    CHECK(identical);
}

TEST(replay_mode_state_probe) {
    if (const char* mode = std::getenv("TH13_E2E_MODE"); !mode || std::string(mode) != "replay") return;
    const char* log_path = std::getenv("TH13_E2E_STATE_LOG");
    if (!log_path || !*log_path) return;
    std::FILE* log = std::fopen(log_path, "w");
    CHECK(log != nullptr);
    if (!log) return;

    std::vector<u8> bytes;
    CHECK(read_repo_file("games/replays/th13_01.rpy", bytes));
    if (bytes.empty()) {
        std::fclose(log);
        return;
    }
    ReplayFile original;
    CHECK(original.decode(bytes.data(), bytes.size()));
    const u16 boot_seed = u16(original.stages[0].header[2] | (original.stages[0].header[3] << 8));

    setenv("TH13_SAVE_PATH", "e2e-th13-save.bin", 1);
    ApplicationState state;
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    CHECK(asset_root && *asset_root);
    if (!(asset_root && *asset_root)) {
        std::fclose(log);
        return;
    }
    CHECK(state.audio_device.initialize_for_test(asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
    CHECK(state.initialize(false));
    if (!state.failure.empty()) {
        std::printf("# initialize failed: %s\n", state.failure.c_str());
        std::fclose(log);
        return;
    }
    state.visual.reseed_restore(boot_seed);
    for (int i = 0; i < 256; ++i)
        (void)state.visual.step16();
    (void)state.visual.next32();
    (void)state.visual.next32();
    state.game_random.reseed(boot_seed);
    CHECK(state.draw_loading(false));
    for (unsigned frame = 0; frame < th13::sdl::ApplicationState::kDebugLoadingFrames; ++frame)
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
            std::printf("# probe: stage %d -> %d at step %u, consumed=%u\n", last_stage, state.progress.stage, f,
                        consumed_snapshot);
            last_stage = state.progress.stage;
            consumed_snapshot = 0;
        }
        if (state.progress.stage == last_stage) consumed_snapshot = state.replay_consumed;
        if (state.run) {
            const u32 px_bits = float_to_bits(state.run->battle.player_x);
            const u32 py_bits = float_to_bits(state.run->battle.player_y);
            std::fprintf(log, "%d %u %u %d %d %u\n", state.progress.stage_frame, px_bits, py_bits,
                         state.selection_score.score, state.selection_player.extra_lives, state.game_random.seed());
        } else {
            std::fprintf(log, "norun %u\n", f);
            break;
        }
    }
    std::fclose(log);
    std::printf("# state log written to %s\n", log_path);
}

}

int main() { return th13test::run_all("native e2e: th13_01 输入流全流程→导出逐字节复刻"); }
