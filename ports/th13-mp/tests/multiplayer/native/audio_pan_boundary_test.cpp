#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::u16;
using th13::u32;
using th13::u64;
using th13::i32;
using App = th13::sdl::ApplicationState;
using Audio = th13::sdl::AudioDevice;
using Observation = Audio::RequestObservation;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kShift = 0x08;

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

const char* asset_root() {
    const char* root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    return root && *root ? root : "";
}

bool init_effects(Audio& audio) {
    if (!*asset_root()) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    return audio.initialize_effects_for_test(asset_root());
}

bool init_full(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    if (!*asset_root()) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    if (!app.audio_device.initialize_for_test(
            asset_root(), std::string(repo_root()) + "/games/th13/thbgmogg.dat")) {
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
    if (!init_full(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(th13::sdl::Destination::Run) && app.failure.empty();
}

struct TapLog {
    std::vector<Observation> items;
    void attach(Audio& audio) {
        audio.request_observer = [this](const Observation& obs) { items.push_back(obs); };
    }
    size_t mark() const { return items.size(); }
    size_t count_kind(Audio::RequestObservation::Kind kind, size_t begin, size_t end) const {
        size_t n = 0;
        for (size_t i = begin; i < end && i < items.size(); ++i)
            if (items[i].kind == kind) ++n;
        return n;
    }
    size_t count_voice(size_t begin, size_t end) const {
        return count_kind(Observation::Kind::Voice, begin, end);
    }
    size_t count_pan(size_t begin, size_t end) const {
        return count_kind(Observation::Kind::Pan, begin, end);
    }
};

bool pcm_energy(Audio& audio, float& left, float& right) {
    float pcm[441 * 2] = {};
    if (!audio.mix(pcm, 441)) {
        std::printf("  失败 mix: %s\n", audio.error.c_str());
        return false;
    }
    left = right = 0;
    for (int i = 0; i < 441; ++i) {
        left += pcm[i * 2] < 0 ? -pcm[i * 2] : pcm[i * 2];
        right += pcm[i * 2 + 1] < 0 ? -pcm[i * 2 + 1] : pcm[i * 2 + 1];
    }
    return true;
}

TEST(audio_pan_boundary_observe_apply_split_device_semantics) {
    {
        Audio audio;
        CHECK(init_effects(audio));
        TapLog log;
        log.attach(audio);
        const Audio::Statistics pre = audio.statistics();
        CHECK(audio.observe_voice_pan(55, 192.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
        CHECK_EQ(log.items[0].kind, Observation::Kind::Pan);
        CHECK_EQ(log.items[0].id, 55);
        CHECK(log.items[0].positional);
        CHECK(log.items[0].x == 192.f);
        const Audio::Statistics mid = audio.statistics();
        CHECK_EQ(mid.requests, pre.requests);
        CHECK_EQ(mid.started, pre.started);
        CHECK_EQ(u32(mid.pending_voices), 0u);
        CHECK_EQ(mid.mixed_frames, pre.mixed_frames);
        CHECK(!audio.observe_voice_pan(99, -5.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(2));
        CHECK_EQ(log.items[1].id, 99);
        CHECK_EQ(audio.error, std::string("Invalid or uninitialized TH13 sound pan: 99"));
        CHECK(!audio.observe_voice_pan(30, 0.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(3));
        CHECK_EQ(audio.error, std::string("Invalid or uninitialized TH13 sound pan: 99"));
    }
    {
        Audio audio;
        CHECK(!audio.observe_voice_pan(0, 0.f));
        CHECK_EQ(audio.error, std::string("Invalid or uninitialized TH13 sound pan: 0"));
    }
    {
        Audio audio;
        CHECK(init_effects(audio));
        TapLog log;
        log.attach(audio);
        CHECK(audio.enqueue(55));
        audio.update();
        float left = 0, right = 0;
        CHECK(pcm_energy(audio, left, right));
        CHECK(left > 0 && right > 0);
        const size_t taps_before = log.mark();
        audio.apply_voice_pan(55, 192.f);
        CHECK_EQ(log.mark(), taps_before);
        CHECK(pcm_energy(audio, left, right));
        CHECK(left == 0.f && right > 0.f);
        audio.apply_voice_pan(55, -192.f);
        CHECK_EQ(log.mark(), taps_before);
        CHECK(pcm_energy(audio, left, right));
        CHECK(right == 0.f && left > 0.f);
        const Audio::Statistics pre = audio.statistics();
        audio.apply_voice_pan(99, 5.f);
        audio.apply_voice_pan(-1, 5.f);
        CHECK_EQ(log.mark(), taps_before);
        const Audio::Statistics post = audio.statistics();
        CHECK_EQ(post.requests, pre.requests);
        CHECK_EQ(post.started, pre.started);
        CHECK_EQ(u32(post.pending_voices), u32(pre.pending_voices));
        CHECK(audio.error.empty());
        CHECK(audio.set_voice_pan(55, 0.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
        CHECK(pcm_energy(audio, left, right));
        CHECK(left > 0 && right > 0);
    }
    {
        Audio audio;
        CHECK(init_effects(audio));
        CHECK(audio.enqueue(55, 1.0f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), 5);
        CHECK(audio.enqueue(55, 192.f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), 1000);
        CHECK(audio.enqueue(55, -192.f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), -1000);
        CHECK(audio.enqueue(55, -1.0f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), -5);
    }
}

TEST(audio_pan_boundary_host_capture_sync_and_commit_apply) {
    TapLog log;
    App app;
    CHECK(init_full(app, "t4pan-host.bin"));
    if (!app.failure.empty()) return;
    log.attach(app.audio_device);
    CHECK(app.audio_device.finish_music_requests());
    CHECK(app.audio_device.queue_music(3, 0, ""));
    CHECK(app.audio_device.finish_music_requests());
    CHECK(app.audio_device.enqueue(55));
    const u64 requests_after_enqueue = app.audio_device.statistics().requests;
    CHECK_EQ(requests_after_enqueue, 1ull);
    const u32 frames_boot = app.frames;
    CHECK(app.commit(kFps));
    CHECK_EQ(app.frames, frames_boot + 1u);
    float left = 0, right = 0;
    CHECK(pcm_energy(app.audio_device, left, right));
    CHECK(left > 0 && right > 0);
    const u32 capture_frame = app.recorder.capture_frame;
    CHECK(app.set_voice_pan(55, 192.f));
    CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
    CHECK_EQ(app.pending_voice_pans_[0].capture_frame, capture_frame);
    CHECK_EQ(app.pending_voice_pans_[0].id, 55);
    CHECK(app.pending_voice_pans_[0].pan == 192.f);
    CHECK(pcm_energy(app.audio_device, left, right));
    CHECK(left > 0 && right > 0);
    CHECK(app.commit(kFps));
    CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
    CHECK(app.pending_voice_pans_.empty());
    CHECK(pcm_energy(app.audio_device, left, right));
    CHECK(left == 0.f && right > 0.f);
    CHECK(app.commit(kFps));
    CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
    CHECK(app.pending_voice_pans_.empty());
    CHECK_EQ(app.audio_device.statistics().requests, requests_after_enqueue);
    const size_t taps_before_bad = log.mark();
    CHECK(!app.set_voice_pan(99, 0.f));
    CHECK_EQ(app.failure, std::string("Invalid or uninitialized TH13 sound pan: 99"));
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(0));
    CHECK_EQ(log.count_pan(taps_before_bad, log.mark()), size_t(1));
    CHECK_EQ(log.items[taps_before_bad].id, 99);
}

TEST(audio_pan_boundary_live_run_charge_pan_capture_not_commit) {
    TapLog log;
    App app;
    CHECK(boot_live_run(app, "t4pan-live.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    log.attach(app.audio_device);
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    int charge_frames = 0;
    size_t pan_total = 0;
    size_t commit_pan_taps = 0;
    for (int f = 0; f < 40; ++f) {
        const u32 held = (f >= 10 ? (kShift | kZ) : kZ);
        input.update(held);
        const u32 frames_pre = app.frames;
        const size_t mark_pre = log.mark();
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        const size_t mark_mid = log.mark();
        const size_t pan_in_update = log.count_pan(mark_pre, mark_mid);
        if (pan_in_update > 0) {
            CHECK_EQ(pan_in_update, size_t(2));
            CHECK_EQ(log.items[mark_mid - 2].kind, Observation::Kind::Pan);
            CHECK_EQ(log.items[mark_mid - 2].id, 30);
            CHECK_EQ(log.items[mark_mid - 1].kind, Observation::Kind::Pan);
            CHECK_EQ(log.items[mark_mid - 1].id, 55);
            for (const auto& obs : log.items) {
                if (obs.kind == Observation::Kind::Pan)
                    CHECK(obs.x >= -192.f && obs.x <= 192.f);
            }
            CHECK_EQ(app.pending_voice_pans_.size(), size_t(2));
            CHECK_EQ(app.pending_voice_pans_[0].capture_frame, frames_pre);
            CHECK_EQ(app.pending_voice_pans_[1].capture_frame, frames_pre);
            ++charge_frames;
            pan_total += pan_in_update;
        } else {
            CHECK(app.pending_voice_pans_.empty());
        }
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        const size_t mark_post = log.mark();
        commit_pan_taps += log.count_pan(mark_mid, mark_post);
        CHECK(app.pending_voice_pans_.empty());
        CHECK(app.present());
    }
    CHECK(charge_frames >= 8);
    CHECK(pan_total >= 16);
    CHECK_EQ(commit_pan_taps, size_t(0));
    const Audio::Statistics final_stats = app.audio_device.statistics();
    CHECK_EQ(final_stats.requests, u64(log.count_voice(0, log.mark())));
    CHECK(final_stats.requests > 0);
}

TEST(audio_pan_boundary_natural_queue_statistics_pinned) {
    Audio audio;
    CHECK(*asset_root() != 0);
    CHECK(audio.initialize_for_test(asset_root(),
                                    std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
    TapLog log;
    log.attach(audio);
    CHECK(audio.enqueue(30));
    CHECK_EQ(log.count_voice(0, log.mark()), size_t(1));
    CHECK_EQ(audio.statistics().requests, 1ull);
    CHECK_EQ(u32(audio.statistics().pending_voices), 1u);
    CHECK_EQ(audio.statistics().started, 0ull);
    audio.update();
    CHECK_EQ(u32(audio.statistics().pending_voices), 0u);
    CHECK_EQ(audio.statistics().started, 1ull);
    CHECK(audio.enqueue(30));
    CHECK(audio.enqueue(30));
    CHECK_EQ(audio.statistics().requests, 3ull);
    CHECK_EQ(u32(audio.statistics().pending_voices), 1u);
    audio.update();
    CHECK_EQ(audio.statistics().started, 2ull);
    CHECK(audio.stop(30));
    CHECK_EQ(log.count_kind(Observation::Kind::Stop, 0, log.mark()), size_t(1));
    CHECK_EQ(u32(audio.statistics().pending_voices), 1u);
    audio.update();
    CHECK_EQ(audio.statistics().stopped, 1ull);
    CHECK_EQ(u32(audio.statistics().pending_voices), 0u);
    CHECK(audio.stop(30));
    audio.update();
    CHECK_EQ(audio.statistics().stopped, 1ull);
    CHECK(audio.enqueue(21));
    audio.update();
    CHECK_EQ(audio.statistics().started, 3ull);
    CHECK(audio.stop(-1));
    CHECK_EQ(audio.statistics().stopped, 2ull);
    CHECK_EQ(log.items.back().kind, Observation::Kind::Stop);
    CHECK_EQ(log.items.back().id, -1);
    for (i32 id = 0; id < 12; ++id) CHECK(audio.enqueue(id));
    CHECK_EQ(u32(audio.statistics().pending_voices), 12u);
    const u64 dropped_pre = audio.statistics().dropped_requests;
    const u64 requests_pre = audio.statistics().requests;
    CHECK(audio.enqueue(12));
    CHECK_EQ(audio.statistics().requests, requests_pre + 1ull);
    CHECK_EQ(audio.statistics().dropped_requests, dropped_pre + 1ull);
    CHECK_EQ(u32(audio.statistics().pending_voices), 12u);
    audio.update();
    CHECK_EQ(u32(audio.statistics().pending_voices), 0u);
    CHECK_EQ(audio.statistics().started, 15ull);
    CHECK(audio.enqueue(40));
    audio.update();
    CHECK_EQ(audio.statistics().started, 16ull);
    const u64 dropped_before_marker = audio.statistics().dropped_requests;
    CHECK(audio.stop(40));
    CHECK(audio.enqueue(40));
    CHECK_EQ(audio.statistics().dropped_requests, dropped_before_marker + 1ull);
    audio.update();
    CHECK_EQ(audio.statistics().started, 16ull);
    CHECK_EQ(audio.statistics().stopped, 3ull);
    CHECK(audio.queue_music(1, 0, "th13_00.wav"));
    CHECK(audio.queue_music(2, -1, "th13_00.wav"));
    CHECK_EQ(log.count_kind(Observation::Kind::Music, 0, log.mark()), size_t(2));
    CHECK_EQ(log.items[log.items.size() - 2].text, std::string("th13_00.wav"));
    CHECK(audio.finish_music_requests());
    CHECK(audio.statistics().track >= 0);
    CHECK(audio.queue_music(3, 0, ""));
    CHECK(audio.finish_music_requests());
    CHECK_EQ(audio.statistics().track, -1);
    CHECK(audio.error.empty());
    {
        Audio bad;
        CHECK(bad.initialize_for_test(asset_root(),
                                      std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
        TapLog bad_log;
        bad_log.attach(bad);
        CHECK(!bad.stop(99));
        CHECK_EQ(bad_log.count_kind(Observation::Kind::Stop, 0, bad_log.mark()), size_t(1));
        CHECK_EQ(bad.error, std::string("Invalid TH13 sound stop request: 99"));
    }
}

}

int main() { return th13test::run_all("m1.unit_audio_pan_boundary"); }
