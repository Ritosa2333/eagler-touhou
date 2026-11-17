#include "../../harness/test_framework.hpp"
#include "sdl/AudioDevice.hpp"
#include "game/SoundEffects.hpp"
#include <eagler/audio/WebSfx.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
using namespace th13;
namespace output = eagler::audio::web_sfx;
namespace {
const void* started_voice() {
    for (const auto& row : output::voices) if (row.second.starts) return row.first;
    return nullptr;
}
}
TEST(immediate_output_preserves_frame_coalescing_pan_attenuation_and_does_not_double_mix) {
    sdl::AudioDevice audio;
    CHECK(audio.initialize_effects_for_test("artifacts/reference/raw"));
    CHECK_EQ(output::voices.size(), size_t(71));
    CHECK(audio.enqueue(30, -192.f));
    CHECK(audio.enqueue(30, 0.f));
    CHECK_EQ(audio.statistics().started, u64(0));
    audio.update();
    const auto* key = started_voice();
    CHECK(key != nullptr);
    if (!key) return;
    auto& v = output::voices.at(key);
    CHECK_EQ(v.starts, 1u);
    CHECK_EQ(v.balance, -.5f);
    CHECK(std::abs(v.gain - std::pow(10.f, float(audio.voice_attenuation(30)) / 2000.f)) < 1e-7f);
    CHECK(std::any_of(v.pcm.begin(), v.pcm.end(), [](float value) { return value != 0.f; }));
    std::vector<float> pcm(8192, 1.f);
    CHECK(audio.mix(pcm.data(), 4096));
    CHECK(std::all_of(pcm.begin(), pcm.end(), [](float value) { return value == 0.f; }));
    CHECK(audio.set_voice_pan(30, 192.f));
    CHECK_EQ(v.balance, 1.f);
    CHECK_EQ(v.starts, 1u);
    CHECK_EQ(audio.voice_pan(30), -500);
    audio.suspend(true); CHECK(output::paused);
    audio.suspend(false); CHECK(!output::paused);
    CHECK(audio.stop(30)); CHECK(audio.enqueue(30));
    audio.update();
    CHECK(!v.active); CHECK_EQ(v.starts, 1u);
    CHECK(audio.enqueue(30)); audio.update();
    CHECK_EQ(v.starts, 2u); CHECK_EQ(v.cursor, 0u);
    CHECK(audio.stop(-1)); CHECK(!v.active);
    audio.close(); CHECK(output::voices.empty());
}
TEST(immediate_output_reuploads_after_context_replacement_and_reports_output_failures) {
    sdl::AudioDevice audio;
    CHECK(audio.initialize_effects_for_test("artifacts/reference/raw"));
    output::voices.clear();
    CHECK(audio.enqueue(30)); audio.update();
    CHECK(audio.error.empty()); CHECK(started_voice() != nullptr);
    const auto* key = started_voice();
    if (!key) return;
    output::voices.at(key).cursor = 128;
    float pcm[2] = {};
    CHECK(audio.mix(pcm, 1));
    output::voices.clear();  // Context replacement while this voice is live.
    CHECK(audio.mix(pcm, 1));
    CHECK(output::contains(key));
    CHECK(output::playing(key));
    CHECK_EQ(output::cursor(key), 128u);
    output::reject_play = true;
    CHECK(audio.enqueue(30)); audio.update();
    CHECK(!audio.error.empty());
    output::reject_play = false;
    audio.close(); CHECK(output::voices.empty());
    output::reject_upload = true;
    CHECK(!audio.initialize_effects_for_test("artifacts/reference/raw"));
    CHECK(!audio.error.empty()); CHECK(!audio.statistics().ready);
    output::reject_upload = false;
}
int main() { return th13test::run_all("TH13 immediate SFX output"); }
