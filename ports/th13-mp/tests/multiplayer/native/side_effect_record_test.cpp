#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "sdl/ApplicationState.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using th13::sdl::Destination;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kShift = 0x08;
constexpr u32 kRight = 0x80;
constexpr u32 kDown = 0x20;

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

bool boot_replay_battle(App& app, const std::vector<u8>& rpy, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.draw_loading(false)) {
        std::printf("  失败 boot: draw_loading: %s\n", app.failure.c_str());
        return false;
    }
    for (unsigned frame = 0; frame < App::kDebugLoadingFrames; ++frame)
        if (!app.advance_loading_caption()) {
            std::printf("  失败 boot: advance_loading_caption: %s\n", app.failure.c_str());
            return false;
        }
    if (!app.begin_replay_run(rpy.data(), rpy.size(), 0)) {
        std::printf("  失败 boot: begin_replay_run: %s\n", app.failure.c_str());
        return false;
    }
    return app.run != nullptr && app.failure.empty();
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t3se", 0, meta);
}

i32 total_frames(const App& app) {
    return app.files.ordinary_records[size_t(app.progress.character)].total_frames;
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

u32 pattern(u32 f) {
    u32 held = 0;
    if (f % 12 < 7) held |= kZ;
    if (f % 19 >= 11 && f % 19 < 15) held |= kRight;
    if (f % 29 >= 24) held |= kDown;
    return held;
}

struct CaptureLog {
    th13::GameInput input{};
    u8 slow = 0;
    bool edge_reset = false;
    u32 frame = 0;
};

}

TEST(side_effect_record_300_frame_mirror_record_once) {
    constexpr int kFrames = 300;
    App app;
    CHECK(boot_live_run(app, "t3se-300-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(kShift);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    CHECK_EQ(app.frames, 1u);
    CHECK_EQ(total_frames(app), 0);
    CHECK_EQ(app.ordinary_records_dirty, false);
    CHECK_EQ(app.pending_play_frames_, 0u);
    CHECK_EQ(app.recorder.ready(), false);

    std::vector<CaptureLog> log;
    int advanced_total = 0;
    int commit_total = 0;
    std::vector<u8> enc_last_commit;
    for (int f = 0; f < kFrames; ++f) {
        input.update(f == 0 ? (kShift | kZ) : pattern(u32(f)));
        const i32 stage_pre = app.progress.stage_frame;
        const u32 frames_pre = app.frames;
        const i32 mirror_pre = total_frames(app);
        const bool dirty_pre = app.ordinary_records_dirty;
        const u32 pending_pre = app.pending_play_frames_;
        CHECK(app.update(input, false));
        if (!app.failure.empty()) {
            std::printf("  失败 f%d update: %s\n", f, app.failure.c_str());
            return;
        }
        const bool advanced = app.progress.stage_frame != stage_pre;
        CHECK_EQ(total_frames(app), mirror_pre);
        CHECK_EQ(app.ordinary_records_dirty, dirty_pre);
        CHECK_EQ(app.frames, frames_pre);
        CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
        CHECK_EQ(app.pending_play_frames_, pending_pre + (advanced ? 1u : 0u));
        if (advanced) {
            ++advanced_total;
            CHECK(!app.recorder.pending_inputs.empty());
            if (app.recorder.pending_inputs.empty()) return;
            const auto& slot = app.recorder.pending_inputs.back();
            CaptureLog entry;
            entry.input = slot.input;
            entry.slow = slot.slow;
            entry.edge_reset = slot.edge_reset;
            entry.frame = slot.frame;
            log.push_back(entry);
            CHECK_EQ(slot.frame, frames_pre);
            CHECK_EQ(slot.slow, u8(60));
            CHECK_EQ(slot.edge_reset, f == 0);
            CHECK_EQ(slot.input.held, input.held);
            CHECK_EQ(slot.input.pressed, input.pressed);
            CHECK_EQ(slot.input.released, input.released);
        }
        std::vector<u8> stream_pre;
        CHECK(encode_pinned(app, stream_pre));

        int commits = 1;
        if (f >= 100 && f <= 102) commits = 0;
        if (f == 200) commits = 2;
        commit_total += commits;
        std::vector<u8> after_first;
        for (int c = 0; c < commits; ++c) {
            CHECK(app.commit(kFps));
            if (!app.failure.empty()) {
                std::printf("  失败 f%d commit: %s\n", f, app.failure.c_str());
                return;
            }
            if (c == 0) {
                CHECK(encode_pinned(app, after_first));
                CHECK(!bytes_equal(after_first, stream_pre));
                CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
                CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
                CHECK_EQ(app.pending_play_frames_, 0u);
                CHECK_EQ(total_frames(app), advanced_total);
                CHECK_EQ(app.ordinary_records_dirty, true);
                CHECK_EQ(app.frames, frames_pre + 1u);
                enc_last_commit = after_first;
            } else {
                std::vector<u8> after_again;
                CHECK(encode_pinned(app, after_again));
                CHECK(bytes_equal(after_again, after_first));
                CHECK_EQ(total_frames(app), advanced_total);
                CHECK_EQ(app.pending_play_frames_, 0u);
                CHECK_EQ(app.frames, frames_pre + 2u);
            }
        }
        if (commits == 0) {
            CHECK(bytes_equal(stream_pre, enc_last_commit));
            CHECK_EQ(app.pending_play_frames_, u32(f - 99));
            CHECK_EQ(app.frames, frames_pre);
        }
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 f%d present: %s\n", f, app.failure.c_str());
            return;
        }
    }
    CHECK_EQ(advanced_total, kFrames);
    CHECK_EQ(total_frames(app), kFrames);
    CHECK_EQ(commit_total, kFrames - 3 + 1);
    CHECK_EQ(app.frames, 1u + u32(commit_total));
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));

    std::vector<u8> enc;
    CHECK(encode_pinned(app, enc));
    th13::ReplayFile decoded;
    CHECK(decoded.decode(enc.data(), enc.size()));
    CHECK_EQ(decoded.stages.size(), size_t(1));
    if (decoded.stages.empty()) return;
    const th13::ReplayStage& stage = decoded.stages.front();
    CHECK_EQ(stage.number, u16(1));
    CHECK_EQ(stage.frame_count, u32(advanced_total));
    CHECK_EQ(stage.frame_count, u32(log.size()));
    CHECK_EQ(stage.events.size(), size_t((advanced_total + 29) / 30));
    for (size_t i = 0; i < log.size(); ++i) {
        const th13::ReplayFrame frame = stage.frame(u32(i));
        u32 pressed = log[i].input.pressed;
        u32 released = log[i].input.released;
        if (log[i].edge_reset) {
            pressed = log[i].input.held;
            released = 0;
        }
        if (u32(frame.state) != log[i].input.held || u32(frame.pressed) != pressed ||
            u32(frame.released) != released) {
            std::printf("  失败帧%zu: state=%u pressed=%u released=%u（期望 held=%u pressed=%u released=%u edge=%d）\n",
                        i, frame.state, frame.pressed, frame.released,
                        log[i].input.held, pressed, released, int(log[i].edge_reset));
        }
        CHECK_EQ(u32(frame.state), log[i].input.held);
        CHECK_EQ(u32(frame.pressed), pressed);
        CHECK_EQ(u32(frame.released), released);
    }
    for (size_t t = 0; t < stage.events.size() && size_t(30 * t) < log.size(); ++t)
        CHECK_EQ(stage.events[t], log[size_t(30 * t)].slow);
    if (!log.empty() && log.front().edge_reset) {
        const th13::ReplayFrame f0 = stage.frame(0);
        CHECK_EQ(u32(f0.pressed), log.front().input.held);
        CHECK_EQ(u32(f0.released), 0u);
    }
}

TEST(side_effect_record_replay_session_no_mirror_no_record) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t3se-replay-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    CHECK(app.replay_active);
    CHECK_EQ(app.recorder.ready(), false);
    const i32 mirror_baseline = total_frames(app);
    const bool dirty_baseline = app.ordinary_records_dirty;
    th13::GameInput input;
    for (int f = 0; f < 12; ++f) {
        input.update(f % 3 == 0 ? kZ : 0u);
        const i32 stage_pre = app.progress.stage_frame;
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        CHECK(app.commit(kFps));
        CHECK(app.present());
        if (!app.failure.empty()) return;
        CHECK(app.progress.stage_frame > stage_pre);
        CHECK_EQ(app.pending_play_frames_, 0u);
        CHECK_EQ(total_frames(app), mirror_baseline);
        CHECK_EQ(app.ordinary_records_dirty, dirty_baseline);
        CHECK_EQ(app.recorder.ready(), false);
        CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
        CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    }
    CHECK_EQ(app.frames, 12u);
}

int main() { return th13test::run_all("m1.unit_side_effect_record_once"); }
