#include "../harness/test_framework.hpp"

#ifndef TH13_AUDIO_MIXER_ONLY
#error "audio tests require -DTH13_AUDIO_MIXER_ONLY (use tests/audio/run.sh)"
#endif

#include "sdl/AudioDevice.hpp"

#include <cmath>
#include <string>
#include <vector>

namespace {

using namespace th13;
using namespace th13::sdl;

constexpr const char* kAssetRoot = "artifacts/reference/raw";
constexpr const char* kMusicArchive = "games/th13/thbgmogg.dat";

bool has_nonzero(const std::vector<float>& pcm) {
    for (float sample : pcm)
        if (sample != 0.0f) return true;
    return false;
}

std::vector<float> render(AudioDevice& audio, u32 frames) {
    std::vector<float> pcm(size_t(frames) * 2, 0.0f);
    CHECK(audio.mix(pcm.data(), frames));
    return pcm;
}

void start(AudioDevice& audio, const char* track) {
    CHECK(audio.queue_music(2, -1, track));
    CHECK(audio.finish_music_requests());
}

}

TEST(music_pause_and_resume_preserve_position) {
    AudioDevice audio;
    CHECK(audio.initialize_for_test(kAssetRoot, kMusicArchive));
    start(audio, "th13_01.wav");
    CHECK_EQ(audio.statistics().track, 1);
    CHECK_EQ(audio.statistics().music_cursor, 0u);
    CHECK(audio.seek_music_frames(44100));
    CHECK(has_nonzero(render(audio, 4096)));
    const u32 before_pause = audio.statistics().music_cursor;
    CHECK(before_pause > 0);

    audio.pause_music(true);
    CHECK(audio.statistics().music_paused);
    CHECK(!has_nonzero(render(audio, 4096)));
    CHECK_EQ(audio.statistics().music_cursor, before_pause);

    audio.pause_music(false);
    CHECK(!audio.statistics().music_paused);
    CHECK(has_nonzero(render(audio, 4096)));
    CHECK(audio.statistics().music_cursor > before_pause);

    CHECK(audio.queue_music(6, 0, ""));
    CHECK(audio.finish_music_requests());
    CHECK(audio.statistics().music_paused);
    const u32 paused = audio.statistics().music_cursor;
    CHECK(!has_nonzero(render(audio, 2048)));
    CHECK_EQ(audio.statistics().music_cursor, paused);

    CHECK(audio.queue_music(7, 0, ""));
    CHECK(audio.finish_music_requests());
    CHECK(!audio.statistics().music_paused);
    CHECK(has_nonzero(render(audio, 2048)));
    CHECK(audio.statistics().music_cursor > paused);
}

TEST(music_seek_stop_and_error_paths) {
    AudioDevice audio;
    CHECK(audio.initialize_for_test(kAssetRoot, kMusicArchive));
    start(audio, "th13_01.wav");
    CHECK(audio.seek_music_frames(44100));
    CHECK_EQ(audio.statistics().music_cursor, 44100u);
    CHECK(has_nonzero(render(audio, 1024)));
    CHECK_EQ(audio.statistics().music_cursor, 45124u);

    CHECK(audio.queue_music(3, 0, ""));
    CHECK(audio.finish_music_requests());
    CHECK_EQ(audio.statistics().track, -1);
    CHECK(!has_nonzero(render(audio, 2048)));
    CHECK(!audio.seek_music_frames(0));
    CHECK(audio.error.find("outside track") != std::string::npos);

    AudioDevice stop4;
    CHECK(stop4.initialize_for_test(kAssetRoot, kMusicArchive));
    start(stop4, "th13_01.wav");
    CHECK(stop4.queue_music(4, 0, ""));
    CHECK(stop4.finish_music_requests());
    CHECK_EQ(stop4.statistics().track, -1);
    CHECK(!has_nonzero(render(stop4, 1024)));

    AudioDevice no_track;
    CHECK(no_track.initialize_for_test(kAssetRoot, kMusicArchive));
    CHECK(!no_track.seek_music_frames(0));
    CHECK(no_track.error.find("outside track") != std::string::npos);

    AudioDevice out_of_range;
    CHECK(out_of_range.initialize_for_test(kAssetRoot, kMusicArchive));
    start(out_of_range, "th13_01.wav");
    CHECK(!out_of_range.seek_music_frames(0xffffffffu));
    CHECK(out_of_range.error.find("outside track") != std::string::npos);
}

TEST(trance_b_variant_and_restore) {
    AudioDevice audio;
    CHECK(audio.initialize_for_test(kAssetRoot, kMusicArchive));
    start(audio, "th13_01.wav");
    CHECK_EQ(audio.statistics().track, 1);
    CHECK(audio.seek_music_frames(44100));
    CHECK_EQ(audio.statistics().music_cursor, 44100u);

    CHECK(audio.trance_music(true));
    CHECK(audio.finish_music_requests());
    CHECK_EQ(audio.statistics().track, 2);
    CHECK_EQ(audio.statistics().music_cursor, 22050u);
    CHECK(has_nonzero(render(audio, 8192)));
    CHECK_EQ(audio.statistics().music_cursor, 22050u + 4096u);

    CHECK(audio.trance_music(false));
    CHECK(audio.finish_music_requests());
    CHECK_EQ(audio.statistics().track, 1);
    CHECK_EQ(audio.statistics().music_cursor, 52292u);
    CHECK(has_nonzero(render(audio, 8192)));
    CHECK_EQ(audio.statistics().music_cursor, 52292u + 8192u);

    AudioDevice round_trip;
    CHECK(round_trip.initialize_for_test(kAssetRoot, kMusicArchive));
    start(round_trip, "th13_01.wav");
    CHECK(round_trip.seek_music_frames(44100));
    CHECK(round_trip.trance_music(true));
    CHECK(round_trip.finish_music_requests());
    CHECK_EQ(round_trip.statistics().music_cursor, 22050u);
    CHECK(round_trip.trance_music(false));
    CHECK(round_trip.finish_music_requests());
    CHECK_EQ(round_trip.statistics().music_cursor, 44100u);
}

TEST(trance_missing_variant_and_missing_base_are_errors) {
    AudioDevice missing_variant;
    CHECK(missing_variant.initialize_for_test(kAssetRoot, kMusicArchive));
    start(missing_variant, "th13_18.wav");
    CHECK_EQ(missing_variant.statistics().track, 24);
    CHECK(missing_variant.trance_music(true));
    CHECK(!missing_variant.finish_music_requests());
    CHECK(missing_variant.error.find("Missing TH13 alternate music track") != std::string::npos);

    AudioDevice no_base;
    CHECK(no_base.initialize_effects_for_test(kAssetRoot));
    CHECK(!no_base.trance_music(true));
    CHECK(no_base.error.find("no current base track") != std::string::npos);
}

TEST(music_commands_preload_play_and_fade) {
    AudioDevice audio;
    CHECK(audio.initialize_for_test(kAssetRoot, kMusicArchive));

    CHECK(audio.queue_music(1, 0, "th13_01.wav"));
    CHECK(audio.finish_music_requests());
    CHECK(audio.queue_music(2, 0, ""));
    CHECK(audio.finish_music_requests());
    CHECK_EQ(audio.statistics().track, 1);
    CHECK_EQ(audio.statistics().music_cursor, 0u);
    CHECK(audio.seek_music_frames(44100));
    CHECK(has_nonzero(render(audio, 2048)));

    CHECK(audio.queue_music(1, 1, "th13_03.wav"));
    CHECK(audio.finish_music_requests());
    CHECK(audio.queue_music(2, 1, ""));
    CHECK(audio.finish_music_requests());
    CHECK_EQ(audio.statistics().track, 3);

    AudioDevice bad_preload_slot;
    CHECK(bad_preload_slot.initialize_for_test(kAssetRoot, kMusicArchive));
    CHECK(bad_preload_slot.queue_music(1, 16, "th13_01.wav"));
    CHECK(!bad_preload_slot.finish_music_requests());
    CHECK(bad_preload_slot.error.find("preload") != std::string::npos);

    AudioDevice bad_preload_name;
    CHECK(bad_preload_name.initialize_for_test(kAssetRoot, kMusicArchive));
    CHECK(bad_preload_name.queue_music(1, 0, "th13_99.wav"));
    CHECK(!bad_preload_name.finish_music_requests());
    CHECK(bad_preload_name.error.find("preload") != std::string::npos);

    AudioDevice bad_play_slot;
    CHECK(bad_play_slot.initialize_for_test(kAssetRoot, kMusicArchive));
    CHECK(bad_play_slot.queue_music(2, 16, ""));
    CHECK(!bad_play_slot.finish_music_requests());
    CHECK(bad_play_slot.error.find("play slot") != std::string::npos);

    AudioDevice missing_track;
    CHECK(missing_track.initialize_for_test(kAssetRoot, kMusicArchive));
    CHECK(missing_track.queue_music(2, -1, "th13_99.wav"));
    CHECK(!missing_track.finish_music_requests());
    CHECK(missing_track.error.find("Missing TH13 music track") != std::string::npos);

    CHECK(audio.queue_music(8, 0, ""));
    CHECK(audio.finish_music_requests());
    CHECK(audio.seek_music_frames(44100));
    CHECK(has_nonzero(render(audio, 1024)));

    CHECK(audio.queue_music(5, 1, ""));
    CHECK(audio.finish_music_requests());
    for (int n = 0; n < 60; ++n)
        audio.update();
    CHECK(!has_nonzero(render(audio, 2048)));

    AudioDevice immediate;
    CHECK(immediate.initialize_for_test(kAssetRoot, kMusicArchive));
    start(immediate, "th13_01.wav");
    CHECK(immediate.queue_music(5, 0, ""));
    CHECK(immediate.finish_music_requests());
    CHECK(!has_nonzero(render(immediate, 1024)));
}

TEST(music_real_asset_pcm_positive_negative_and_suspend) {
    AudioDevice audio;
    CHECK(audio.initialize_for_test(kAssetRoot, kMusicArchive));
    start(audio, "th13_00.wav");
    CHECK(audio.seek_music_frames(44100));
    const auto pcm = render(audio, 4096);
    CHECK(has_nonzero(pcm));
    for (float sample : pcm)
        CHECK(sample >= -1.0f && sample <= 1.0f);

    audio.music_volume = 0;
    const u32 before_zero = audio.statistics().music_cursor;
    CHECK(!has_nonzero(render(audio, 4096)));
    CHECK(audio.statistics().music_cursor > before_zero);

    audio.music_volume = 100;
    CHECK(has_nonzero(render(audio, 4096)));

    audio.suspend(true);
    CHECK(audio.statistics().suspended);
    const u32 before_suspend = audio.statistics().music_cursor;
    CHECK(!has_nonzero(render(audio, 2048)));
    CHECK_EQ(audio.statistics().music_cursor, before_suspend);
    audio.suspend(false);
    CHECK(!audio.statistics().suspended);
    CHECK(has_nonzero(render(audio, 2048)));
    CHECK(audio.statistics().music_cursor > before_suspend);

    AudioDevice idle;
    CHECK(idle.initialize_for_test(kAssetRoot, kMusicArchive));
    CHECK(!has_nonzero(render(idle, 4096)));
}

TEST(music_archive_validation_and_errors) {
    AudioDevice bad_archive;
    CHECK(!bad_archive.initialize_for_test(kAssetRoot, "artifacts/reference/raw/thbgm.ogg"));
    CHECK(!bad_archive.error.empty());
    CHECK(!bad_archive.statistics().ready);
    CHECK(!bad_archive.enqueue(0));

    AudioDevice absent_archive;
    CHECK(!absent_archive.initialize_for_test(kAssetRoot, "artifacts/reference/raw/__absent__.ogg"));
    CHECK(!absent_archive.error.empty());

    AudioDevice absent_layout;
    CHECK(!absent_layout.initialize_for_test("artifacts/reference/raw/__absent__", kMusicArchive));
    CHECK(!absent_layout.error.empty());

    AudioDevice effects_only;
    CHECK(effects_only.initialize_effects_for_test(kAssetRoot));
    CHECK(effects_only.queue_music(2, -1, "th13_01.wav"));
    CHECK(!effects_only.finish_music_requests());
    CHECK(effects_only.error.find("Missing TH13 music track") != std::string::npos);
}

int main() { return th13test::run_all("audio music PCM"); }
