#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "sdl/ApplicationState.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
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

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t3rec", 0, meta);
}

i32 total_frames(const App& app) {
    return app.files.ordinary_records[size_t(app.progress.character)].total_frames;
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

u8 expected_slow(const App& app) {
    return u8(std::min(255.0f, std::max(0.0f, app.slow_window_fps_ + 0.5f)));
}

u32 pattern(u32 f) {
    u32 held = 0;
    if (f % 9 < 5) held |= kZ;
    if (f % 17 >= 10 && f % 17 < 14) held |= kRight;
    if (f % 23 == 20) held |= kDown;
    if (f % 31 >= 25) held |= kShift;
    return held;
}

struct CaptureLog {
    th13::GameInput input{};
    u8 slow = 0;
    bool edge_reset = false;
    u32 frame = 0;
};

}

TEST(recorder_pending_dfix_first_frame_slot) {
    App app;
    CHECK(boot_live_run(app, "t3rec-dfix-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(kShift);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    CHECK_EQ(app.frames, 1u);
    input.update(kShift | kZ);
    const u32 frames_pre = app.frames;
    CHECK(app.update(input, false));
    CHECK(app.recorder.ready());
    CHECK_EQ(app.record_edge_reset_pending_, false);
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(1));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    if (app.recorder.pending_inputs.empty()) return;
    const auto& slot = app.recorder.pending_inputs.front();
    CHECK(slot.edge_reset);
    CHECK_EQ(slot.frame, frames_pre);
    CHECK_EQ(slot.frame, app.recorder.capture_frame);
    CHECK_EQ(u32(slot.input.held), kShift | kZ);
    CHECK_EQ(u32(slot.input.pressed), kZ);
    CHECK_EQ(u32(slot.input.released), 0u);
    CHECK_EQ(slot.slow, expected_slow(app));
    CHECK_EQ(slot.slow, u8(60));
    std::vector<u8> before;
    CHECK(encode_pinned(app, before));
    CHECK_EQ(total_frames(app), 0);
    CHECK_EQ(app.frames, frames_pre);

    CHECK(app.commit(kFps));
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
    CHECK_EQ(app.frames, frames_pre + 1u);
    std::vector<u8> after;
    CHECK(encode_pinned(app, after));
    CHECK(!bytes_equal(after, before));
    th13::ReplayFile decoded;
    CHECK(decoded.decode(after.data(), after.size()));
    CHECK_EQ(decoded.stages.size(), size_t(1));
    if (decoded.stages.empty()) return;
    const th13::ReplayStage& stage = decoded.stages.front();
    CHECK_EQ(stage.frame_count, 1u);
    CHECK_EQ(stage.number, u16(1));
    const th13::ReplayFrame f0 = stage.frame(0);
    CHECK_EQ(u32(f0.state), kShift | kZ);
    CHECK_EQ(u32(f0.pressed), kShift | kZ);
    CHECK_EQ(u32(f0.released), 0u);
    CHECK_EQ(stage.events.size(), size_t(1));
    if (!stage.events.empty()) CHECK_EQ(stage.events[0], u8(60));
}

TEST(recorder_pending_split_step_byte_identity) {
    constexpr int kFrames = 24;
    App arm_s, arm_d;
    CHECK(boot_live_run(arm_s, "t3rec-arms-save.bin"));
    CHECK(boot_live_run(arm_d, "t3rec-armd-save.bin"));
    if (!arm_s.failure.empty() || !arm_d.failure.empty()) return;
    th13::GameInput in_s, in_d;
    in_s.update(0);
    in_d.update(0);
    CHECK(arm_s.update(in_s, false));
    CHECK(arm_d.update(in_d, false));
    CHECK(arm_s.commit(kFps));
    CHECK(arm_d.commit(kFps));
    CHECK(arm_s.present());
    CHECK(arm_d.present());
    CHECK_EQ(arm_s.pending_load, 1u);
    CHECK_EQ(arm_d.pending_load, 1u);

    std::vector<CaptureLog> log_d;
    std::vector<u8> enc_last_commit;
    for (int f = 0; f < kFrames; ++f) {
        const u32 held = pattern(u32(f));
        in_s.update(held);
        in_d.update(held);
        const i32 stage_pre_s = arm_s.progress.stage_frame;
        const i32 stage_pre_d = arm_d.progress.stage_frame;
        CHECK(arm_s.step(in_s, kFps, false));
        if (!arm_s.failure.empty()) {
            std::printf("  失败臂S f%d: %s\n", f, arm_s.failure.c_str());
            return;
        }
        CHECK_EQ(arm_s.progress.stage_frame, stage_pre_s + 1);
        CHECK(arm_d.update(in_d, false));
        if (!arm_d.failure.empty()) {
            std::printf("  失败臂D f%d: %s\n", f, arm_d.failure.c_str());
            return;
        }
        CHECK_EQ(arm_d.progress.stage_frame, stage_pre_d + 1);
        if (arm_d.recorder.pending_inputs.empty()) {
            std::printf("  失败臂D f%d: update 后挂起槽为空（record_input 捕获丢失）\n", f);
        }
        CHECK(!arm_d.recorder.pending_inputs.empty());
        if (!arm_d.recorder.pending_inputs.empty()) {
            const auto& slot = arm_d.recorder.pending_inputs.back();
            CaptureLog entry;
            entry.input = slot.input;
            entry.slow = slot.slow;
            entry.edge_reset = slot.edge_reset;
            entry.frame = slot.frame;
            log_d.push_back(entry);
            CHECK_EQ(slot.frame, arm_d.frames);
            CHECK_EQ(slot.slow, expected_slow(arm_d));
        }
        const i32 mirror_pre = total_frames(arm_d);
        std::vector<u8> stream_pre;
        CHECK(encode_pinned(arm_d, stream_pre));
        if (f >= 8 && f <= 10) {
            CHECK_EQ(total_frames(arm_d), 8);
            CHECK_EQ(mirror_pre, 8);
            CHECK_EQ(arm_d.ordinary_records_dirty, true);
            CHECK_EQ(arm_d.pending_play_frames_, u32(f - 7));
            CHECK_EQ(u32(arm_d.recorder.pending_inputs.size()), u32(f - 7));
            CHECK_EQ(arm_d.frames, 9u);
            std::vector<u8> stream_now;
            CHECK(encode_pinned(arm_d, stream_now));
            CHECK(bytes_equal(stream_now, enc_last_commit));
        } else {
            const int commits = f == 16 ? 2 : (f == 20 ? 3 : 1);
            std::vector<u8> after_first;
            for (int c = 0; c < commits; ++c) {
                CHECK(arm_d.commit(kFps));
                if (!arm_d.failure.empty()) return;
                if (c == 0) {
                    CHECK(encode_pinned(arm_d, after_first));
                    CHECK(!bytes_equal(after_first, stream_pre));
                    CHECK_EQ(arm_d.recorder.pending_inputs.size(), size_t(0));
                    CHECK_EQ(arm_d.pending_play_frames_, 0u);
                    CHECK_EQ(total_frames(arm_d), f + 1);
                    CHECK_EQ(arm_d.ordinary_records_dirty, true);
                    enc_last_commit = after_first;
                } else {
                    std::vector<u8> after_again;
                    CHECK(encode_pinned(arm_d, after_again));
                    CHECK(bytes_equal(after_again, after_first));
                    CHECK_EQ(total_frames(arm_d), f + 1);
                    CHECK_EQ(arm_d.recorder.pending_inputs.size(), size_t(0));
                    CHECK_EQ(arm_d.pending_play_frames_, 0u);
                }
            }
            CHECK(arm_d.present());
        }
    }
    CHECK_EQ(size_t(log_d.size()), size_t(kFrames));
    std::vector<u8> enc_s, enc_d;
    CHECK(encode_pinned(arm_s, enc_s));
    CHECK(encode_pinned(arm_d, enc_d));
    if (!bytes_equal(enc_s, enc_d)) {
        std::printf("  失败两臂 encode 长度 S=%zu D=%zu\n", enc_s.size(), enc_d.size());
    }
    CHECK(bytes_equal(enc_s, enc_d));
    CHECK_EQ(total_frames(arm_s), kFrames);
    CHECK_EQ(total_frames(arm_d), kFrames);
    th13::ReplayFile dec_s, dec_d;
    CHECK(dec_s.decode(enc_s.data(), enc_s.size()));
    CHECK(dec_d.decode(enc_d.data(), enc_d.size()));
    CHECK_EQ(dec_d.stages.size(), size_t(1));
    CHECK_EQ(dec_s.stages.size(), size_t(1));
    if (dec_s.stages.empty() || dec_d.stages.empty()) return;
    CHECK_EQ(dec_d.stages.front().frame_count, u32(kFrames));
    CHECK(dec_d.stages.front().input == dec_s.stages.front().input);
    CHECK(dec_d.stages.front().events == dec_s.stages.front().events);
    CHECK(dec_d.stages.front().header == dec_s.stages.front().header);
    const th13::ReplayStage& stage = dec_d.stages.front();
    for (size_t i = 0; i < log_d.size(); ++i) {
        const th13::ReplayFrame frame = stage.frame(u32(i));
        u32 pressed = log_d[i].input.pressed;
        u32 released = log_d[i].input.released;
        if (log_d[i].edge_reset) {
            pressed = log_d[i].input.held;
            released = 0;
        }
        if (u32(frame.state) != log_d[i].input.held || u32(frame.pressed) != pressed ||
            u32(frame.released) != released) {
            std::printf("  失败帧%zu: state=%u pressed=%u released=%u（期望 held=%u pressed=%u released=%u edge=%d）\n",
                        i, frame.state, frame.pressed, frame.released,
                        log_d[i].input.held, pressed, released, int(log_d[i].edge_reset));
        }
        CHECK_EQ(u32(frame.state), log_d[i].input.held);
        CHECK_EQ(u32(frame.pressed), pressed);
        CHECK_EQ(u32(frame.released), released);
    }
}

TEST(recorder_pending_slow_source_capture_window) {
    constexpr int kFrames = 135;
    App app;
    CHECK(boot_live_run(app, "t3rec-slow-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    std::vector<CaptureLog> log;
    std::vector<u8> enc_pre_window;
    bool settled_low = false;
    for (int f = 0; f < kFrames; ++f) {
        const float fps = (f <= 64) ? 60.0f : (f <= 67 ? 0.5f : 60.0f);
        input.update(f % 2 == 0 ? kZ : 0u);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        CHECK(!app.recorder.pending_inputs.empty());
        if (app.recorder.pending_inputs.empty()) return;
        const auto& slot = app.recorder.pending_inputs.back();
        CaptureLog entry;
        entry.input = slot.input;
        entry.slow = slot.slow;
        entry.edge_reset = slot.edge_reset;
        entry.frame = slot.frame;
        log.push_back(entry);
        CHECK_EQ(slot.slow, expected_slow(app));
        if (f == 64) CHECK(encode_pinned(app, enc_pre_window));
        CHECK(app.commit(fps));
        if (!app.failure.empty()) return;
        if (f == 65 && app.slow_window_fps_ < 60.0f) settled_low = true;
        CHECK(app.present());
    }
    CHECK(settled_low);
    CHECK_NEAR(app.slow_window_fps_, 60.0, 0.01);
    CHECK_EQ(size_t(log.size()), size_t(kFrames));
    std::vector<u8> enc;
    CHECK(encode_pinned(app, enc));
    CHECK(!bytes_equal(enc, enc_pre_window));
    th13::ReplayFile decoded;
    CHECK(decoded.decode(enc.data(), enc.size()));
    CHECK_EQ(decoded.stages.size(), size_t(1));
    if (decoded.stages.empty()) return;
    const th13::ReplayStage& stage = decoded.stages.front();
    CHECK_EQ(stage.frame_count, u32(kFrames));
    CHECK_EQ(stage.events.size(), size_t((kFrames + 29) / 30));
    for (size_t t = 0; t < stage.events.size() && size_t(30 * t) < log.size(); ++t)
        CHECK_EQ(stage.events[t], log[size_t(30 * t)].slow);
    CHECK_EQ(stage.events[0], u8(60));
    CHECK_EQ(stage.events[1], u8(60));
    CHECK_EQ(stage.events[2], u8(60));
    CHECK_EQ(log[65].slow, u8(60));
    CHECK(log[66].slow < u8(60));
    CHECK(stage.events[3] != u8(60));
    CHECK(stage.events[4] != u8(60));
    CHECK_EQ(log.back().slow, u8(60));
}

TEST(recorder_pending_spell_time_slot_captured_once) {
    App app;
    CHECK(boot_live_run(app, "t3rec-spell-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    for (int f = 0; f < 2; ++f) {
        input.update(kZ);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
    }
    CHECK(app.recorder.ready());
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    std::vector<u8> baseline;
    CHECK(encode_pinned(app, baseline));

    app.update_spell_time(true);
    app.update_spell_time(false);
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(1));
    if (app.recorder.pending_spell_times.size() != 1) return;
    const auto& entry = app.recorder.pending_spell_times.front();
    CHECK_EQ(entry.index, 0u);
    CHECK(entry.index < 21u);
    CHECK_EQ(entry.packed, app.spell_time_packed);
    CHECK(th13::spell_time_packed_valid(entry.packed));
    CHECK_EQ(entry.frame, app.recorder.capture_frame);
    std::vector<u8> before;
    CHECK(encode_pinned(app, before));
    CHECK(bytes_equal(before, baseline));

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(app.commit(kFps));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    CHECK_EQ(app.spell_time_packed, entry.packed);
    std::vector<u8> after;
    CHECK(encode_pinned(app, after));
    CHECK(!bytes_equal(after, baseline));
    th13::ReplayFile decoded;
    CHECK(decoded.decode(after.data(), after.size()));
    CHECK_EQ(decoded.stages.size(), size_t(1));
    if (decoded.stages.empty()) return;
    const th13::ReplayStage& stage = decoded.stages.front();
    CHECK(stage.header.size() >= size_t(0xC4));
    if (stage.header.size() < 0xC4) return;
    const u32 stored = u32(stage.header[0x70]) | u32(stage.header[0x71]) << 8 |
                       u32(stage.header[0x72]) << 16 | u32(stage.header[0x73]) << 24;
    CHECK_EQ(i32(stored), entry.packed);
    app.update_spell_time(true);
    app.update_spell_time(false);
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(1));
    if (app.recorder.pending_spell_times.size() == 1) {
        CHECK_EQ(app.recorder.pending_spell_times.front().index, 1u);
        CHECK_EQ(app.recorder.pending_spell_times.front().packed,
                 app.spell_time_packed);
    }
    std::vector<u8> after_first;
    CHECK(app.commit(kFps));
    CHECK(encode_pinned(app, after_first));
    CHECK(!bytes_equal(after_first, after));
    CHECK(app.commit(kFps));
    std::vector<u8> after_double;
    CHECK(encode_pinned(app, after_double));
    CHECK(bytes_equal(after_double, after_first));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
}

TEST(recorder_pending_capture_gate_sync_semantics) {
    App app;
    CHECK(boot_live_run(app, "t3rec-gate-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    input.update(kZ);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK(app.recorder.ready());
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));

    app.recorder.stream.clear();
    CHECK(!app.recorder.ready());
    input.update(kZ);
    const u32 frames_pre = app.frames;
    const i32 mirror_pre = total_frames(app);
    CHECK(app.update(input, false));
    CHECK_EQ(app.failure, std::string("Replay input recording failed"));
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    CHECK(app.commit(kFps));
    CHECK_EQ(app.frames, frames_pre + 1u);
    CHECK_EQ(total_frames(app), mirror_pre + 1);
    input.update(0);
    CHECK(!app.update(input, false));
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(app.frames, frames_pre + 1u);
}

int main() { return th13test::run_all("m1.unit_recorder_pending_once"); }
