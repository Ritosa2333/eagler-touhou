// fps 读数：0x424990 口径（逻辑帧 / 帧钟入账时间，含两端相位）+ 半帧相位起步。
#include "../harness/test_framework.hpp"

#include "../../portable/sdl/FrameRateMeter.hpp"

#include <algorithm>

namespace {
using touhou::sdl::FrameCadence;
using touhou::sdl::FrameRateMeter;

double jitter(unsigned i, double amplitude) {
    const unsigned h = (i * 2654435761u) >> 16;
    return ((h % 2001) / 1000. - 1.) * amplitude;
}
// Callbacks are late around vsync: the delta is the difference of two bounded offsets.
double callback_delta(unsigned i, double period, double amplitude) {
    return period + jitter(i + 1, amplitude) - jitter(i, amplitude);
}
struct Run { float low = 99, high = 0; unsigned uneven = 0; };
Run run(double period, double amplitude, unsigned callbacks, unsigned stall_at = ~0u) {
    FrameCadence cadence;
    cadence.debt = FrameCadence::interval / 2;
    FrameRateMeter meter;
    Run result;
    for (unsigned i = 0; i < callbacks; ++i) {
        const double delta = i == stall_at ? .5 : callback_delta(i, period, amplitude);
        const float fps = meter.sample(cadence.debt);
        if (i > 120) { result.low = std::min(result.low, fps); result.high = std::max(result.high, fps); }
        meter.elapse(delta);
        const unsigned ticks = cadence.advance(delta);
        if (i > 0 && i != stall_at && i != stall_at + 1) result.uneven += ticks != 1;
        meter.frame(ticks);
    }
    return result;
}
}  // namespace

TEST(fps_readout_is_exact_on_a_jittery_60hz_display) {
    const auto steady = run(1. / 60., .003, 1200);
    CHECK(steady.low >= 59.95f && steady.high <= 60.f);
    CHECK_EQ(steady.uneven, 0u);  // half-phase start: one logic frame per refresh
    const auto rough = run(1. / 60., .007, 1200);
    CHECK(rough.low >= 59.95f && rough.high <= 60.f);
}

TEST(fps_readout_follows_wall_time_not_refresh_rate) {
    const auto ntsc = run(1001. / 60000., .002, 1800);
    CHECK(ntsc.low >= 59.95f && ntsc.high <= 60.f);
    const auto fifty = run(1. / 50., 0, 600);
    CHECK_NEAR(fifty.low, 60.f, .01f);
}

TEST(fps_readout_shows_time_the_cadence_drops) {
    CHECK(run(1. / 60., .002, 600, 300).low < 58.f);
}

int main() { return th13test::run_all("fps 读数"); }
