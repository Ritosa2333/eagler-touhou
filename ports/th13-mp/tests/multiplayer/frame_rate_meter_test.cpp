#include "../../web/cpp/multiplayer/FrameSchedule.hpp"
#include "../../portable/sdl/FrameRateMeter.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>

namespace {
using touhou::sdl::FrameCadence;
using touhou::sdl::FrameRateMeter;
using th13::multiplayer::FrameSchedule;

double jitter(unsigned i, double amplitude) {
    const unsigned h = (i * 2654435761u) >> 16;
    return ((h % 2001) / 1000. - 1.) * amplitude;
}
double callback_delta(unsigned i, double period, double amplitude) {
    return period + jitter(i + 1, amplitude) - jitter(i, amplitude);
}

struct CadenceRun { float low = 99, high = 0; unsigned zero = 0, double_ = 0; };

CadenceRun run_cadence(double period, double amplitude, unsigned callbacks, unsigned stall_at = ~0u) {
    FrameCadence cadence;
    cadence.debt = FrameCadence::interval / 2;
    FrameRateMeter meter;
    CadenceRun run;
    for (unsigned i = 0; i < callbacks; ++i) {
        const double delta = i == stall_at ? .5 : callback_delta(i, period, amplitude);
        const float fps = meter.sample(cadence.debt);
        if (i > 120) { run.low = std::min(run.low, fps); run.high = std::max(run.high, fps); }
        meter.elapse(delta);
        const unsigned ticks = cadence.advance(delta);
        if (i > 0 && i != stall_at && i != stall_at + 1) {
            run.zero += ticks == 0;
            run.double_ += ticks >= 2;
        }
        meter.frame(ticks);
    }
    return run;
}
}

int main() {
    const auto steady = run_cadence(1. / 60., .003, 1200);
    assert(steady.low >= 59.95f && steady.high <= 60.f);
    assert(steady.zero == 0 && steady.double_ == 0);
    const auto rough = run_cadence(1. / 60., .007, 1200);
    assert(rough.low >= 59.95f && rough.high <= 60.f);
    const auto ntsc = run_cadence(1001. / 60000., .002, 1800);
    assert(ntsc.low >= 59.95f && ntsc.high <= 60.f);
    const auto stall = run_cadence(1. / 60., .002, 600, 300);
    assert(stall.low < 58.f);
    const auto slow = run_cadence(1. / 50., 0, 600);
    assert(std::fabs(slow.low - 60.f) < .01f);

    FrameSchedule schedule;
    schedule.align_phase();
    FrameRateMeter meter;
    float low = 99;
    unsigned uneven = 0;
    for (unsigned i = 0; i < 1200; ++i) {
        const double delta = callback_delta(i, 1. / 60., .003);
        const float fps = meter.sample(schedule.credit());
        if (i > 120) low = std::min(low, fps);
        meter.elapse(delta);
        const unsigned owed = schedule.advance_gap(delta);
        uneven += owed != 1;
        for (unsigned f = 0; f < owed; ++f) schedule.complete();
        meter.frame(schedule.take_completed());
    }
    assert(uneven == 0);
    assert(low >= 59.95f);
    const auto network = [](bool gap) {
        FrameSchedule held;
        held.align_phase();
        FrameRateMeter readout;
        float lowest = 99;
        for (unsigned i = 0; i < 600; ++i) {
            const float fps = readout.sample(held.credit());
            if (i > 120) lowest = std::min(lowest, fps);
            readout.elapse(1. / 60.);
            const unsigned owed = gap ? held.advance_gap(1. / 60.) : held.advance(1. / 60.);
            if (i % 4 == 0) { held.network_wait(); continue; }
            for (unsigned f = 0; f < owed; ++f) held.complete();
            readout.frame(held.take_completed());
        }
        return lowest;
    };
    assert(network(true) >= 59.f);
    assert(network(false) < 50.f);
    std::puts("frame rate meter ok");
    return 0;
}
