// PCM-level audio diagnostics for the original TH13 sound engine.
// Original anchors:
//   00460bc0 start voice (12 frame channels, count 0..0x3c per channel)
//   00460c50 start voice with pan argument (same channel table)
//   00460d40 stop voice / stop all (negative count marker)
//   00460dd0 per-update drain: one voice object per ID is restarted with the
//            averaged channel entries; duplicate requests coalesce.
//   004613c0 SE voice start: cubic (1-(1-v/100)^3) f32 curve + 0x4971f0
//            truncation, applied once per start from global DAT_004e4770.
// These tests consume the real original WAV assets; no synthetic PCM is
// substituted for the original tables.
#include "../harness/test_framework.hpp"

#ifndef TH13_AUDIO_MIXER_ONLY
#error "audio tests require -DTH13_AUDIO_MIXER_ONLY (use tests/audio/run.sh)"
#endif

#include "game/SoundEffects.hpp"
#include "sdl/AudioDevice.hpp"

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace {

using namespace th13;
using namespace th13::sdl;

constexpr const char* kAssetRoot = "artifacts/reference/raw";
constexpr const char* kMusicArchive = "games/th13/thbgmogg.dat";

const SoundDefinition* find_definition(i32 id) {
    for (const auto& row : sound_definitions)
        if (row.id == id) return &row;
    return nullptr;
}

bool has_nonzero(const std::vector<float>& pcm) {
    for (float sample : pcm)
        if (sample != 0.0f) return true;
    return false;
}

bool channel_nonzero(const std::vector<float>& pcm, size_t channel) {
    for (size_t i = channel; i < pcm.size(); i += 2)
        if (pcm[i] != 0.0f) return true;
    return false;
}

std::vector<float> render(AudioDevice& audio, u32 frames) {
    std::vector<float> pcm(size_t(frames) * 2, 0.0f);
    CHECK(audio.mix(pcm.data(), frames));
    return pcm;
}

i32 attenuation_for(i32 id, i32 volume) {
    AudioDevice audio;
    CHECK(audio.initialize_effects_for_test(kAssetRoot));
    audio.effect_volume = volume;
    CHECK(audio.enqueue(id));
    audio.update();
    return audio.voice_attenuation(id);
}

}  // namespace

TEST(request_observer_preserves_raw_order_and_does_not_change_queue) {
    AudioDevice audio;
    CHECK(audio.initialize_effects_for_test(kAssetRoot));
    std::vector<AudioDevice::RequestObservation> observed;
    audio.request_observer = [&](const auto& request) { observed.push_back(request); };
    CHECK(audio.enqueue(60, -96.f));
    CHECK(audio.enqueue(63));
    CHECK(audio.stop(60));
    CHECK(audio.queue_music(5, 3, "fade"));
    CHECK_EQ(observed.size(), size_t(4));
    CHECK(observed[0].kind == AudioDevice::RequestObservation::Kind::Voice);
    CHECK_EQ(observed[0].id, 60); CHECK(observed[0].positional); CHECK_EQ(observed[0].x, -96.f);
    CHECK_EQ(observed[1].id, 63); CHECK(!observed[1].positional);
    CHECK(observed[2].kind == AudioDevice::RequestObservation::Kind::Stop);
    CHECK(observed[3].kind == AudioDevice::RequestObservation::Kind::Music);
    CHECK_EQ(observed[3].id, 5); CHECK_EQ(observed[3].value, 3);
    CHECK_EQ(observed[3].text, std::string("fade"));
    audio.update();
    CHECK_EQ(audio.statistics().started, u64(1)); // 60 stop marker wins
}

TEST(direct_buffer_pan_preserves_playback_and_start_pan) {
    AudioDevice audio, reference;
    CHECK(audio.initialize_effects_for_test(kAssetRoot));
    CHECK(reference.initialize_effects_for_test(kAssetRoot));
    CHECK(audio.enqueue(30, -96.f)); CHECK(reference.enqueue(30, -96.f));
    audio.update(); reference.update();
    CHECK(render(audio, 128) == render(reference, 128));
    const auto before = audio.statistics();
    std::vector<AudioDevice::RequestObservation> events;
    audio.request_observer = [&](const auto& event) { events.push_back(event); };
    CHECK(audio.set_voice_pan(30, 192.f));
    CHECK_EQ(audio.voice_pan(30), -500);
    CHECK_EQ(audio.statistics().requests, before.requests);
    CHECK_EQ(audio.statistics().started, before.started);
    CHECK_EQ(audio.statistics().pending_voices, before.pending_voices);
    CHECK_EQ(events.size(), size_t(1));
    CHECK(events[0].kind == AudioDevice::RequestObservation::Kind::Pan);
    CHECK_EQ(events[0].id, 30); CHECK_EQ(events[0].x, 192.f);
    const auto changed = render(audio, 512);
    // Reference at -500 has half right gain. Doubling that channel checks
    // the same playback cursor while the direct setter mutes the left.
    const auto expected = render(reference, 512);
    CHECK(channel_nonzero(expected, 1));
    for (size_t i = 0; i < changed.size(); i += 2) {
        CHECK_EQ(changed[i], 0.f);
        CHECK_EQ(changed[i + 1], expected[i + 1] * 2.f);
    }
    CHECK(audio.enqueue(30, 0.f)); audio.update();
    CHECK_EQ(audio.voice_pan(30), 0);
    CHECK(channel_nonzero(render(audio, 512), 0));
}

TEST(audio_failure_paths_are_explicit_and_not_silent_devices) {
    AudioDevice uninitialized;
    float buffer[8] = {};
    CHECK(!uninitialized.enqueue(0));
    CHECK(!uninitialized.error.empty());
    CHECK(!uninitialized.stop(0));
    CHECK(!uninitialized.mix(buffer, 4));
    CHECK(!uninitialized.queue_music(2, -1, "th13_01.wav"));
    CHECK(!uninitialized.trance_music(true));
    CHECK(!uninitialized.seek_music_frames(0));

    AudioDevice missing;
    CHECK(!missing.initialize_effects_for_test("artifacts/reference/raw/__absent__"));
    CHECK(!missing.error.empty());
    CHECK(!missing.statistics().ready);
    CHECK(!missing.enqueue(0));

    // Mixer-only binaries cannot open an SDL device; the failure is retained.
    AudioDevice no_device;
    CHECK(!no_device.initialize(true));
    CHECK(!no_device.error.empty());
    CHECK(!no_device.statistics().ready);

    AudioDevice effects;
    CHECK(effects.initialize_effects_for_test(kAssetRoot));
    CHECK(effects.statistics().ready);
    CHECK(!effects.statistics().device);
    CHECK(!effects.enqueue(-1));
    CHECK(effects.error.find("Invalid") != std::string::npos);

    AudioDevice invalid_high;
    CHECK(invalid_high.initialize_effects_for_test(kAssetRoot));
    CHECK(!invalid_high.enqueue(71));
    CHECK(invalid_high.error.find("Invalid") != std::string::npos);

    // The highest real definition ID (70) is valid.
    AudioDevice highest;
    CHECK(highest.initialize_effects_for_test(kAssetRoot));
    CHECK(highest.enqueue(70));
    CHECK(highest.stop(70));
    highest.update();
    CHECK(!highest.stop(71));
    CHECK(highest.error.find("Invalid") != std::string::npos);

    // Music commands fail cleanly before initialization and for invalid codes.
    AudioDevice commands;
    CHECK(commands.initialize_effects_for_test(kAssetRoot));
    CHECK(!commands.queue_music(0, 0, ""));
    CHECK(commands.error.find("Invalid") != std::string::npos);

    AudioDevice code;
    CHECK(code.initialize_effects_for_test(kAssetRoot));
    CHECK(!code.queue_music(10, 0, ""));
    CHECK(code.error.find("Invalid") != std::string::npos);

    AudioDevice long_text;
    CHECK(long_text.initialize_effects_for_test(kAssetRoot));
    CHECK(!long_text.queue_music(1, 0, std::string(256, 'x')));
    CHECK(long_text.error.find("Invalid") != std::string::npos);

    AudioDevice full;
    CHECK(full.initialize_for_test(kAssetRoot, kMusicArchive));
    for (int n = 0; n < 31; ++n) CHECK(full.queue_music(1, 0, "th13_01.wav"));
    CHECK(full.finish_music_requests());
    for (int n = 0; n < 31; ++n) CHECK(full.queue_music(1, 0, "th13_01.wav"));
    CHECK(!full.queue_music(1, 0, "th13_01.wav"));
    CHECK(full.error.find("full") != std::string::npos);
}

TEST(se_volume_curve_matches_0x4613c0_cubic_truncation) {
    struct Row { i32 volume, id, definition_volume, expected; };
    // Expected values from
    // artifacts/verification/codex-recovery-20261029/fixes/audio/curve_reference.py
    // (f32 store points:
    // x, 1-x, (1-x)^3, 1-(1-x)^3; then trunc(curve*(def+5000))-5000).
    // id 0 = se_plst00.wav definition volume -1900.
    // id 20 = se_lazer02.wav definition volume -3000.
    const Row rows[] = {
        {0, 0, -1900, -10000}, {20, 0, -1900, -3488}, {50, 0, -1900, -2288},
        {80, 0, -1900, -1925}, {100, 0, -1900, -1900},
        {0, 20, -3000, -10000}, {20, 20, -3000, -4025},
        {50, 20, -3000, -3250}, {80, 20, -3000, -3017},
        {100, 20, -3000, -3000},
    };
    for (const auto& row : rows) {
        const SoundDefinition* definition = find_definition(row.id);
        CHECK(definition != nullptr);
        if (definition) CHECK_EQ(definition->volume, row.definition_volume);
        CHECK_EQ(attenuation_for(row.id, row.volume), row.expected);
    }
    // The pre-fix square/lround formula gave -3080 for (80, id 20), not -3017.
    // -3017 also distinguishes the f32 timing from exact real arithmetic
    // (-3016); see
    // artifacts/verification/codex-recovery-20261029/fixes/audio/curve_reference.py.
}

TEST(global_effect_volume_is_read_at_voice_start_not_initialize) {
    AudioDevice audio;
    CHECK_EQ(audio.effect_volume, 80);  // original default config 0x5064 high byte
    audio.effect_volume = 20;           // options write point before device init
    CHECK(audio.initialize_effects_for_test(kAssetRoot));
    CHECK_EQ(audio.effect_volume, 20);  // initialize must not freeze a startup value
    CHECK(audio.enqueue(0));
    audio.update();
    CHECK_EQ(audio.voice_attenuation(0), -3488);

    // Changing the global while the voice plays does not retroactively change
    // it: 0x4613c0 only runs when a voice starts.
    audio.effect_volume = 100;
    CHECK_EQ(audio.voice_attenuation(0), -3488);
    const auto fixed = render(audio, 512);

    AudioDevice reference;
    CHECK(reference.initialize_effects_for_test(kAssetRoot));
    reference.effect_volume = 20;
    CHECK(reference.enqueue(0));
    reference.update();
    const auto expected = render(reference, 512);
    CHECK(fixed == expected);

    // Restarting the same ID after the write reads the new global.
    audio.stop(-1);
    audio.update();
    CHECK(audio.enqueue(0));
    audio.update();
    CHECK_EQ(audio.voice_attenuation(0), -1900);

    // A write after initialize but before enqueue is honored as well.
    AudioDevice late;
    CHECK(late.initialize_effects_for_test(kAssetRoot));
    late.effect_volume = 50;
    CHECK(late.enqueue(0));
    late.update();
    CHECK_EQ(late.voice_attenuation(0), -2288);
}

TEST(se_volume_curve_reaches_pcm_gain) {
    AudioDevice loud;
    CHECK(loud.initialize_effects_for_test(kAssetRoot));
    loud.effect_volume = 100;
    CHECK(loud.enqueue(20));
    loud.update();
    const auto loud_pcm = render(loud, 2048);

    AudioDevice soft;
    CHECK(soft.initialize_effects_for_test(kAssetRoot));
    soft.effect_volume = 80;
    CHECK(soft.enqueue(20));
    soft.update();
    const auto soft_pcm = render(soft, 2048);

    CHECK(has_nonzero(loud_pcm));
    CHECK(has_nonzero(soft_pcm));
    double loud_energy = 0.0, soft_energy = 0.0;
    for (size_t i = 0; i < loud_pcm.size(); ++i) {
        loud_energy += double(loud_pcm[i]) * double(loud_pcm[i]);
        soft_energy += double(soft_pcm[i]) * double(soft_pcm[i]);
    }
    CHECK(loud_energy > 0.0);
    const double ratio = std::sqrt(soft_energy / loud_energy);
    const double expected = std::pow(10.0, (-3017.0 - (-3000.0)) / 2000.0);
    CHECK_NEAR(ratio, expected, 1e-6);
    // The old square curve would give about 0.912 here.
    CHECK(ratio > 0.97);
}

TEST(pan_entries_aggregate_like_0x460c50) {
    struct Row { float pan; i32 expected; };
    const Row rows[] = {
        {192.0f, 1000}, {-192.0f, -1000}, {96.0f, 500}, {-96.0f, -500},
        {1.0f, 5}, {-1.0f, -5}, {0.0f, 0}, {191.9f, 999},
    };
    for (const auto& row : rows) {
        AudioDevice audio;
        CHECK(audio.initialize_effects_for_test(kAssetRoot));
        CHECK(audio.enqueue(0, row.pan));
        audio.update();
        CHECK_EQ(audio.voice_pan(0), row.expected);
    }

    // 0x460dd0 averages all entries for the ID with signed integer division.
    {
        AudioDevice audio;
        CHECK(audio.initialize_effects_for_test(kAssetRoot));
        CHECK(audio.enqueue(0, 192.0f));
        CHECK(audio.enqueue(0, -192.0f));
        audio.update();
        CHECK_EQ(audio.voice_pan(0), 0);
        CHECK_EQ(audio.statistics().started, u64(1));
    }
    {
        AudioDevice audio;
        CHECK(audio.initialize_effects_for_test(kAssetRoot));
        CHECK(audio.enqueue(0, 192.0f));
        CHECK(audio.enqueue(0, 96.0f));
        audio.update();
        CHECK_EQ(audio.voice_pan(0), 750);
    }
    {
        AudioDevice audio;
        CHECK(audio.initialize_effects_for_test(kAssetRoot));
        CHECK(audio.enqueue(0, 192.0f));
        CHECK(audio.enqueue(0));
        audio.update();
        // 0x460bc0 stores 0 for the pan-less request; (1000+0)/2 = 500.
        CHECK_EQ(audio.voice_pan(0), 500);
    }
    {
        AudioDevice audio;
        CHECK(audio.initialize_effects_for_test(kAssetRoot));
        for (int n = 0; n < 61; ++n) CHECK(audio.enqueue(0, 192.0f));
        CHECK_EQ(audio.statistics().dropped_requests, u64(1));
        audio.update();
        CHECK_EQ(audio.statistics().started, u64(1));
        CHECK_EQ(audio.voice_pan(0), 1000);
    }

    // 12 channels x 60 entries: all 720 entries are accepted, the 13th ID is
    // discarded, and every channel averages its own 60 entries.
    {
        AudioDevice audio;
        CHECK(audio.initialize_effects_for_test(kAssetRoot));
        for (i32 id = 0; id < 12; ++id)
            for (int n = 0; n < 60; ++n) CHECK(audio.enqueue(id, 96.0f));
        CHECK(audio.enqueue(12));
        CHECK_EQ(audio.statistics().dropped_requests, u64(1));
        audio.update();
        const auto stats = audio.statistics();
        CHECK_EQ(stats.started, u64(12));
        CHECK_EQ(stats.active_voices, 12u);
        for (i32 id = 0; id < 12; ++id) CHECK_EQ(audio.voice_pan(id), 500);
    }
}

TEST(pan_reaches_pcm_balance) {
    // Verifies the 0x460c50/0x460dd0 aggregation and the host linear balance
    // only. The original sound-buffer vtable+0x40 pan law is not decompiled;
    // this is not an original-PCM pan certification.
    AudioDevice plain;
    CHECK(plain.initialize_effects_for_test(kAssetRoot));
    CHECK(plain.enqueue(0));
    plain.update();
    const auto centered = render(plain, 1024);
    CHECK(channel_nonzero(centered, 0));
    CHECK(channel_nonzero(centered, 1));

    // enqueue(id, 0) is the 0x460c50 path with a zero pan entry; it must be
    // indistinguishable from the 0x460bc0 pan-less path.
    AudioDevice zero_pan;
    CHECK(zero_pan.initialize_effects_for_test(kAssetRoot));
    CHECK(zero_pan.enqueue(0, 0.0f));
    zero_pan.update();
    CHECK(render(zero_pan, 1024) == centered);

    AudioDevice right;
    CHECK(right.initialize_effects_for_test(kAssetRoot));
    CHECK(right.enqueue(0, 192.0f));
    right.update();
    const auto right_pcm = render(right, 1024);
    CHECK(!channel_nonzero(right_pcm, 0));
    CHECK(channel_nonzero(right_pcm, 1));

    AudioDevice left;
    CHECK(left.initialize_effects_for_test(kAssetRoot));
    CHECK(left.enqueue(0, -192.0f));
    left.update();
    const auto left_pcm = render(left, 1024);
    CHECK(channel_nonzero(left_pcm, 0));
    CHECK(!channel_nonzero(left_pcm, 1));

    // Opposite pan entries average back to center.
    AudioDevice averaged;
    CHECK(averaged.initialize_effects_for_test(kAssetRoot));
    CHECK(averaged.enqueue(0, 192.0f));
    CHECK(averaged.enqueue(0, -192.0f));
    averaged.update();
    CHECK(render(averaged, 1024) == centered);
}

TEST(duplicate_voice_requests_coalesce_into_one_voice_per_update) {
    auto single = [](u32 requests) {
        AudioDevice audio;
        CHECK(audio.initialize_effects_for_test(kAssetRoot));
        for (u32 n = 0; n < requests; ++n) CHECK(audio.enqueue(0));
        audio.update();
        const auto stats = audio.statistics();
        CHECK_EQ(stats.requests, u64(requests));
        CHECK_EQ(stats.started, u64(1));  // 00460dd0: one start per ID
        CHECK_EQ(stats.active_voices, 1u);
        CHECK_EQ(stats.dropped_requests, 0u);
        return render(audio, 1024);
    };
    const auto once = single(1);
    const auto repeated = single(5);
    CHECK(has_nonzero(once));
    // If duplicates were separate instances the summed PCM would differ.
    CHECK(once == repeated);
}

TEST(twelve_frame_channels_sixty_entries_and_stop_markers) {
    // 12 distinct IDs occupy all channels; the 13th distinct ID is discarded.
    AudioDevice audio;
    CHECK(audio.initialize_effects_for_test(kAssetRoot));
    for (i32 id = 0; id < 12; ++id) CHECK(audio.enqueue(id));
    CHECK(audio.enqueue(12));
    auto stats = audio.statistics();
    CHECK_EQ(stats.dropped_requests, u64(1));
    audio.update();
    stats = audio.statistics();
    CHECK_EQ(stats.started, u64(12));
    CHECK_EQ(stats.active_voices, 12u);
    CHECK_EQ(stats.pending_voices, 0u);

    // A new frame gets a fresh channel table. The same ID accepts 60 queued
    // entries; the 61st is discarded (00460bc0: count >= 0x3c).
    const u64 started_before = stats.started;
    for (int n = 0; n < 61; ++n) CHECK(audio.enqueue(0));
    stats = audio.statistics();
    CHECK_EQ(stats.dropped_requests, u64(2));
    audio.update();
    stats = audio.statistics();
    CHECK_EQ(stats.started, started_before + 1);  // still one coalesced start

    // A stop marker queued earlier this frame wins over later starts.
    AudioDevice marker;
    CHECK(marker.initialize_effects_for_test(kAssetRoot));
    CHECK(marker.enqueue(0));
    CHECK(marker.stop(0));
    CHECK(marker.enqueue(0));
    marker.update();
    CHECK_EQ(marker.statistics().started, u64(0));
    CHECK_EQ(marker.statistics().active_voices, 0u);
    CHECK_EQ(marker.statistics().dropped_requests, u64(1));

    // Stopping a playing looping ID retires it and mutes the mix.
    AudioDevice playing;
    CHECK(playing.initialize_effects_for_test(kAssetRoot));
    CHECK(playing.enqueue(55));  // se_msl2.wav, definition flags bit0 = loop
    playing.update();
    CHECK_EQ(playing.statistics().active_voices, 1u);
    CHECK(playing.stop(55));
    CHECK(playing.enqueue(55));
    playing.update();
    CHECK_EQ(playing.statistics().stopped, u64(1));
    CHECK_EQ(playing.statistics().active_voices, 0u);
    CHECK(!has_nonzero(render(playing, 2048)));

    // Negative ID stops every original voice object (00460d40 negative path).
    AudioDevice all;
    CHECK(all.initialize_effects_for_test(kAssetRoot));
    CHECK(all.enqueue(0));
    CHECK(all.enqueue(20));
    all.update();
    CHECK_EQ(all.statistics().active_voices, 2u);
    CHECK(all.stop(-1));
    all.update();
    CHECK_EQ(all.statistics().active_voices, 0u);
    CHECK(!has_nonzero(render(all, 2048)));

    // All 12 channels occupied: a stop for a 13th ID has no free slot.
    AudioDevice full;
    CHECK(full.initialize_effects_for_test(kAssetRoot));
    for (i32 id = 0; id < 12; ++id) CHECK(full.enqueue(id));
    CHECK(full.stop(12));
    CHECK_EQ(full.statistics().dropped_requests, u64(1));
    full.update();
    CHECK_EQ(full.statistics().active_voices, 12u);
}

TEST(looping_voice_repeats_until_original_stop_marker) {
    AudioDevice audio;
    CHECK(audio.initialize_effects_for_test(kAssetRoot));
    CHECK(audio.enqueue(55));  // se_msl2.wav, definition flags bit0 = loop
    audio.update();
    CHECK_EQ(audio.statistics().active_voices, 1u);

    const auto first = render(audio, 44100);
    const auto looped = render(audio, 44100);
    CHECK(has_nonzero(first));
    CHECK(has_nonzero(looped));  // 1.056 s sample is still looping at 2 s

    CHECK(audio.stop(55));  // 00460d40 positive ID: per-channel stop marker
    audio.update();
    const auto stats = audio.statistics();
    CHECK_EQ(stats.stopped, u64(1));
    CHECK_EQ(stats.active_voices, 0u);
    const auto silent = render(audio, 2048);
    CHECK(!has_nonzero(silent));
}

TEST(real_asset_pcm_positive_and_negative_controls) {
    // Positive control: the decoded original SE is audible at volume 100.
    AudioDevice audible;
    CHECK(audible.initialize_effects_for_test(kAssetRoot));
    audible.effect_volume = 100;
    CHECK(audible.enqueue(0));
    audible.update();
    const auto pcm = render(audible, 4096);
    CHECK(has_nonzero(pcm));
    CHECK(channel_nonzero(pcm, 0));
    CHECK(channel_nonzero(pcm, 1));
    for (float sample : pcm) CHECK(sample >= -1.0f && sample <= 1.0f);

    // Negative control: volume 0 is exact silence (0x4613c0 -10000 path).
    AudioDevice muted;
    CHECK(muted.initialize_effects_for_test(kAssetRoot));
    muted.effect_volume = 0;
    CHECK(muted.enqueue(0));
    muted.update();
    CHECK_EQ(muted.voice_attenuation(0), -10000);
    CHECK(!has_nonzero(render(muted, 4096)));

    // Negative control: no queued voice is silence.
    AudioDevice idle;
    CHECK(idle.initialize_effects_for_test(kAssetRoot));
    CHECK(!has_nonzero(render(idle, 4096)));

    // A non-looping voice retires at the decoded real sample length
    // (se_plst00.wav: 3552 frames at 22050 Hz -> 7104 frames at 44100 Hz).
    AudioDevice retire;
    CHECK(retire.initialize_effects_for_test(kAssetRoot));
    CHECK(retire.enqueue(0));
    retire.update();
    const auto long_pcm = render(retire, 8192);
    CHECK(has_nonzero(long_pcm));
    CHECK_EQ(retire.statistics().active_voices, 0u);
    bool tail_nonzero = false;
    for (size_t i = 7200 * 2; i < long_pcm.size(); ++i)
        tail_nonzero |= long_pcm[i] != 0.0f;
    CHECK(!tail_nonzero);
}

int main() { return th13test::run_all("audio device PCM"); }
