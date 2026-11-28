#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "sdl/ApplicationState.hpp"
#include "state_digest.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using Audio = th13::sdl::AudioDevice;
using th13::sdl::Destination;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kX = 0x02;
constexpr u32 kShift = 0x08;

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

bool obs_equal(const Audio::RequestObservation& a, const Audio::RequestObservation& b) {
    return a.kind == b.kind && a.id == b.id && a.value == b.value &&
           a.positional == b.positional && a.x == b.x && a.text == b.text;
}

struct TapLog {
    std::vector<Audio::RequestObservation> items;
    void attach(App& app) {
        app.audio_device.request_observer = [this](const Audio::RequestObservation& obs) {
            items.push_back(obs);
        };
    }
    size_t mark() const { return items.size(); }
};

struct ChannelModel {
    std::set<i32> playing;
    struct Expect {
        size_t pending = 0, started = 0, marker_stopped = 0;
        size_t immediate_stopped = 0;
        size_t voice_requests = 0;
        size_t stopped_total() const { return immediate_stopped + marker_stopped; }
    };
    Expect observe(const Audio::RequestObservation* taps, size_t count) const {
        std::set<i32> voice_ids, stop_ids;
        Expect e;
        bool all_stop = false;
        for (size_t i = 0; i < count; ++i) {
            const Audio::RequestObservation& obs = taps[i];
            if (obs.kind == Audio::RequestObservation::Kind::Voice) {
                ++e.voice_requests;
                if (obs.id >= 0) voice_ids.insert(obs.id);
            } else if (obs.kind == Audio::RequestObservation::Kind::Stop) {
                if (obs.id < 0) {
                    ++e.immediate_stopped;
                    all_stop = true;
                } else {
                    stop_ids.insert(obs.id);
                }
            }
        }
        std::set<i32> touched(voice_ids);
        touched.insert(stop_ids.begin(), stop_ids.end());
        e.pending = touched.size();
        for (i32 id : voice_ids)
            if (!stop_ids.count(id)) ++e.started;
        if (!all_stop)
            for (i32 id : stop_ids)
                if (playing.count(id)) ++e.marker_stopped;
        return e;
    }
    void drain(const Audio::RequestObservation* taps, size_t count) {
        std::set<i32> voice_ids, stop_ids;
        bool all_stop = false;
        for (size_t i = 0; i < count; ++i) {
            const Audio::RequestObservation& obs = taps[i];
            if (obs.kind == Audio::RequestObservation::Kind::Voice && obs.id >= 0)
                voice_ids.insert(obs.id);
            else if (obs.kind == Audio::RequestObservation::Kind::Stop) {
                if (obs.id < 0) all_stop = true;
                else stop_ids.insert(obs.id);
            }
        }
        if (all_stop) playing.clear();
        for (i32 id : voice_ids)
            if (!stop_ids.count(id)) playing.insert(id);
        for (i32 id : stop_ids) playing.erase(id);
    }
};

i32 total_frames_at(const App& app, i32 character) {
    return app.files.ordinary_records[size_t(character)].total_frames;
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t3ab", 0, meta);
}

struct DrawProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;
    int fires = 0;
    static i32 run(void* p) {
        auto& probe = *static_cast<DrawProbe*>(p);
        ++probe.fires;
        probe.callback.enabled = false;
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target) {
        app = &target;
        callback.owner = this;
        callback.enabled = true;
        callback.run = &DrawProbe::run;
        if (target.scheduler.add(callback, th13::FramePass::Draw, 73) < 0) return false;
        attached_scheduler = &target.scheduler;
        return true;
    }
    void detach() {
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~DrawProbe() { detach(); }
};

}

TEST(commit_audio_boundary_update_only_queue_commit_drain) {
    App app;
    CHECK(boot_live_run(app, "t3ab-drain-save.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    TapLog log;
    log.attach(app);
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    ChannelModel model;
    const Audio::Statistics boot_stats = app.audio_device.statistics();
    size_t window_begin = log.mark();
    int windows_with_pending = 0, windows_with_starts = 0;
    size_t max_pending = 0;
    for (int f = 0; f < 30; ++f) {
        const u32 held = (f == 10 ? (kShift | kZ | kX) : (f >= 10 ? (kShift | kZ) : kZ));
        input.update(held);
        const u32 frames_pre = app.frames;
        const Audio::Statistics pre = app.audio_device.statistics();
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        const size_t window_end = log.mark();
        const ChannelModel::Expect e =
            model.observe(log.items.data() + window_begin, window_end - window_begin);
        const Audio::Statistics mid = app.audio_device.statistics();
        CHECK_EQ(mid.requests, pre.requests + e.voice_requests);
        CHECK_EQ(u32(mid.pending_voices), u32(e.pending));
        CHECK_EQ(mid.started, pre.started);
        CHECK_EQ(mid.stopped, pre.stopped + e.immediate_stopped);
        CHECK_EQ(mid.mixed_frames, pre.mixed_frames);
        CHECK_EQ(mid.dropped_requests, pre.dropped_requests);
        if (e.pending > 0) ++windows_with_pending;
        if (e.started > 0) ++windows_with_starts;
        max_pending = std::max(max_pending, e.pending);
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        const Audio::Statistics post = app.audio_device.statistics();
        CHECK_EQ(u32(post.pending_voices), 0u);
        CHECK_EQ(post.started, pre.started + e.started);
        CHECK_EQ(post.stopped, pre.stopped + e.stopped_total());
        CHECK_EQ(post.mixed_frames, pre.mixed_frames);
        CHECK_EQ(post.dropped_requests, pre.dropped_requests);
        CHECK_EQ(app.frames, frames_pre + 1u);
        model.drain(log.items.data() + window_begin, window_end - window_begin);
        CHECK(app.present());
        if (!app.failure.empty()) return;
        window_begin = log.mark();
    }
    CHECK(windows_with_pending >= 1);
    CHECK(windows_with_starts >= 1);
    CHECK(max_pending >= 2);
    CHECK_EQ(app.audio_device.statistics().mixed_frames, boot_stats.mixed_frames);
}

TEST(commit_audio_boundary_double_commit_single_beat) {
    App app;
    CHECK(boot_live_run(app, "t3ab-double-save.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    TapLog log;
    log.attach(app);
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    ChannelModel model;
    size_t window_begin = log.mark();
    int guarded_at = -1;
    for (int f = 0; f < 30 && guarded_at < 0; ++f) {
        const u32 held = (f == 10 ? (kShift | kZ | kX) : (f >= 10 ? (kShift | kZ) : kZ));
        input.update(held);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        const size_t window_end = log.mark();
        const ChannelModel::Expect e =
            model.observe(log.items.data() + window_begin, window_end - window_begin);
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        model.drain(log.items.data() + window_begin, window_end - window_begin);
        CHECK(app.present());
        window_begin = log.mark();
        if (e.started >= 2) {
            const Audio::Statistics after_first = app.audio_device.statistics();
            const u32 frames_after_first = app.frames;
            CHECK(app.commit(kFps));
            const Audio::Statistics after_second = app.audio_device.statistics();
            CHECK_EQ(after_second.started, after_first.started);
            CHECK_EQ(after_second.stopped, after_first.stopped);
            CHECK_EQ(u32(after_second.pending_voices), 0u);
            CHECK_EQ(after_second.requests, after_first.requests);
            CHECK_EQ(after_second.dropped_requests, after_first.dropped_requests);
            CHECK_EQ(after_second.mixed_frames, after_first.mixed_frames);
            CHECK_EQ(app.frames, frames_after_first + 1u);
            guarded_at = f;
        }
    }
    CHECK(guarded_at >= 0);
}

TEST(commit_audio_boundary_split_step_tap_sequence_identical) {
    constexpr int kFrames = 30;
    App arm_s, arm_d;
    CHECK(boot_live_run(arm_s, "t3ab-taps-save.bin"));
    CHECK(boot_live_run(arm_d, "t3ab-tapd-save.bin"));
    if (!arm_s.failure.empty() || !arm_d.failure.empty()) return;
    arm_s.progress.character = 3;
    arm_d.progress.character = 3;
    TapLog log_s, log_d;
    log_s.attach(arm_s);
    log_d.attach(arm_d);
    th13::GameInput in_s, in_d;
    in_s.update(0);
    in_d.update(0);
    CHECK(arm_s.update(in_s, false));
    CHECK(arm_d.update(in_d, false));
    CHECK(arm_s.commit(kFps));
    CHECK(arm_d.commit(kFps));
    CHECK(arm_s.present());
    CHECK(arm_d.present());
    for (int f = 0; f < kFrames; ++f) {
        const u32 held = (f == 10 ? (kShift | kZ | kX) : (f >= 10 ? (kShift | kZ) : kZ));
        in_s.update(held);
        in_d.update(held);
        CHECK(arm_s.step(in_s, kFps, false));
        if (!arm_s.failure.empty()) {
            std::printf("  失败臂S f%d: %s\n", f, arm_s.failure.c_str());
            return;
        }
        CHECK(arm_d.update(in_d, false));
        if (!arm_d.failure.empty()) {
            std::printf("  失败臂D f%d: %s\n", f, arm_d.failure.c_str());
            return;
        }
        if (f != 8 && f != 9) {
            CHECK(arm_d.commit(kFps));
            CHECK(arm_d.present());
        }
        if (!arm_d.failure.empty()) return;
    }
    if (log_s.items.size() != log_d.items.size()) {
        std::printf("  失败 tap 数量 S=%zu D=%zu\n", log_s.items.size(), log_d.items.size());
    }
    CHECK_EQ(log_s.items.size(), log_d.items.size());
    for (size_t i = 0; i < log_s.items.size() && i < log_d.items.size(); ++i) {
        if (!obs_equal(log_s.items[i], log_d.items[i])) {
            std::printf("  失败 tap[%zu] 不一致\n", i);
        }
        CHECK(obs_equal(log_s.items[i], log_d.items[i]));
    }
    const Audio::Statistics s = arm_s.audio_device.statistics();
    const Audio::Statistics d = arm_d.audio_device.statistics();
    CHECK_EQ(s.started, d.started);
    CHECK_EQ(s.requests, d.requests);
    CHECK_EQ(s.dropped_requests, d.dropped_requests);
    CHECK_EQ(s.stopped, d.stopped);
    std::vector<u8> enc_s, enc_d;
    CHECK(encode_pinned(arm_s, enc_s));
    CHECK(encode_pinned(arm_d, enc_d));
    CHECK(bytes_equal(enc_s, enc_d));
}

TEST(commit_audio_boundary_commit_order_truncation_play_frames) {
    {
        App control;
        CHECK(boot_live_run(control, "t3ab-ordctl-save.bin"));
        if (!control.failure.empty()) return;
        control.progress.character = 3;
        TapLog log;
        log.attach(control);
        th13::GameInput input;
        input.update(0);
        CHECK(control.update(input, false));
        CHECK(control.commit(kFps));
        CHECK(control.present());
        ChannelModel model;
        size_t window_begin = log.mark();
        for (int f = 0; f <= 10; ++f) {
            input.update(f == 9 ? (kZ | kX) : kZ);
            const Audio::Statistics pre_update = control.audio_device.statistics();
            CHECK(control.update(input, false));
            if (!control.failure.empty()) return;
            const size_t window_end = log.mark();
            const ChannelModel::Expect e =
                model.observe(log.items.data() + window_begin, window_end - window_begin);
            const Audio::Statistics pre_commit = control.audio_device.statistics();
            CHECK_EQ(control.progress.stage_frame, f + 1);
            CHECK_EQ(control.pending_play_frames_, 1u);
            CHECK_EQ(u32(pre_commit.pending_voices), u32(e.pending));
            CHECK_EQ(pre_commit.started, pre_update.started);
            CHECK_EQ(pre_commit.stopped,
                     pre_update.stopped + e.immediate_stopped);
            CHECK(control.commit(kFps));
            if (!control.failure.empty()) return;
            const Audio::Statistics post = control.audio_device.statistics();
            CHECK_EQ(u32(post.pending_voices), 0u);
            CHECK_EQ(post.started, pre_update.started + e.started);
            CHECK_EQ(post.stopped, pre_update.stopped + e.stopped_total());
            CHECK_EQ(post.mixed_frames, pre_update.mixed_frames);
            CHECK_EQ(total_frames_at(control, 3), f + 1);
            CHECK_EQ(control.ordinary_records_dirty, true);
            CHECK_EQ(control.frames, u32(f) + 2u);
            if (f == 10) {
                CHECK(control.loading_prayer == 0);
            }
            model.drain(log.items.data() + window_begin, window_end - window_begin);
            window_begin = log.mark();
            CHECK(control.present());
        }
        CHECK_EQ(control.frames, 12u);
    }
    App app;
    CHECK(boot_live_run(app, "t3ab-ordfail-save.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    TapLog log;
    log.attach(app);
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    ChannelModel model;
    size_t window_begin = log.mark();
    for (int f = 0; f <= 10; ++f) {
        input.update(f == 9 ? (kZ | kX) : kZ);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        if (f == 10) break;
        const size_t window_end = log.mark();
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        model.drain(log.items.data() + window_begin, window_end - window_begin);
        window_begin = log.mark();
        CHECK(app.present());
    }
    const size_t window_end = log.mark();
    const ChannelModel::Expect e =
        model.observe(log.items.data() + window_begin, window_end - window_begin);
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(1));
    CHECK_EQ(app.pending_play_frames_, 1u);
    CHECK_EQ(app.progress.stage_frame, 11);
    CHECK_EQ(app.loading_prayer != 0, true);
    CHECK(e.pending >= 1);
    const Audio::Statistics pre_stats = app.audio_device.statistics();
    CHECK_EQ(u32(pre_stats.pending_voices), u32(e.pending));
    const u32 pre_frames = app.frames;
    const double pre_window_time = app.slow_window_time_;
    const u32 pre_window_frames = app.slow_window_frames_;
    const uint64_t pre_caption = mpdigest::digest_loading_caption(app);
    const i32 pre_mirror = total_frames_at(app, 3);
    const bool pre_dirty = app.ordinary_records_dirty;
    std::vector<u8> enc_pre;
    CHECK(encode_pinned(app, enc_pre));
    app.progress.character = 99;
    CHECK(!app.commit(kFps));
    CHECK_EQ(app.failure, std::string("Invalid play time record"));
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    std::vector<u8> enc_post;
    CHECK(encode_pinned(app, enc_post));
    CHECK(!bytes_equal(enc_post, enc_pre));
    CHECK_EQ(total_frames_at(app, 3), pre_mirror);
    CHECK_EQ(app.ordinary_records_dirty, pre_dirty);
    CHECK_EQ(app.pending_play_frames_, 0u);
    const Audio::Statistics post_stats = app.audio_device.statistics();
    CHECK_EQ(post_stats.pending_voices, pre_stats.pending_voices);
    CHECK_EQ(post_stats.started, pre_stats.started);
    CHECK_EQ(post_stats.requests, pre_stats.requests);
    CHECK_EQ(post_stats.dropped_requests, pre_stats.dropped_requests);
    CHECK_EQ(post_stats.mixed_frames, pre_stats.mixed_frames);
    CHECK_EQ(app.frames, pre_frames);
    CHECK_EQ(app.slow_window_time_, pre_window_time);
    CHECK_EQ(app.slow_window_frames_, pre_window_frames);
    CHECK_EQ(app.loading_prayer != 0, true);
    CHECK_EQ(mpdigest::digest_loading_caption(app), pre_caption);
    DrawProbe probe;
    CHECK(probe.attach(app));
    input.update(0);
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(probe.fires, 0);
    CHECK_EQ(app.frames, pre_frames);
    CHECK_EQ(app.failure, std::string("Invalid play time record"));
    probe.detach();
}

TEST(commit_audio_boundary_commit_order_truncation_recorder_flush) {
    App app;
    CHECK(boot_live_run(app, "t3ab-flushfail-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    for (int f = 0; f < 3; ++f) {
        input.update(0);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(3));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    const Audio::Statistics pre_stats = app.audio_device.statistics();
    CHECK_EQ(u32(pre_stats.pending_voices), 2u);
    const u32 pre_frames = app.frames;
    const i32 pre_mirror = total_frames_at(app, 0);
    const bool pre_dirty = app.ordinary_records_dirty;
    const double pre_window_time = app.slow_window_time_;
    app.recorder.stream.clear();
    CHECK(!app.recorder.ready());
    CHECK(!app.commit(kFps));
    CHECK_EQ(app.failure, std::string("Replay input recording failed"));
    CHECK_EQ(total_frames_at(app, 0), pre_mirror);
    CHECK_EQ(app.ordinary_records_dirty, pre_dirty);
    CHECK_EQ(app.frames, pre_frames);
    CHECK_EQ(app.slow_window_time_, pre_window_time);
    const Audio::Statistics post_stats = app.audio_device.statistics();
    CHECK_EQ(post_stats.pending_voices, pre_stats.pending_voices);
    CHECK_EQ(post_stats.started, pre_stats.started);
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(2));
    input.update(0);
    CHECK(!app.update(input, false));
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(app.frames, pre_frames);
}

int main() { return th13test::run_all("m1.unit_commit_audio_boundary"); }
