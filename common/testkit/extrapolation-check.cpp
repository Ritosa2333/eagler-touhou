#include <eagler/presentation/Extrapolation.hpp>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
using eagler::presentation::ExtrapolationWeight;
int main() {
    // A constant-speed entity's visual age equals elapsed display time, rather
    // than elapsed time minus one 60 Hz step. Logic still advances at 60 Hz.
    for (int hz : {60, 75, 90, 120, 144, 165, 180, 240}) {
        for (int refresh = hz; refresh < hz * 4; ++refresh) {
            double ticks = double(refresh) * 60 / hz;
            double latest = std::floor(ticks + 1e-9);
            double phase = std::max(0., ticks - latest);
            float current = float(latest * 4), previous = current - 4;
            float shown = previous + (current - previous) * ExtrapolationWeight(phase);
            assert(std::abs(shown - ticks * 4) < 1e-3);
            assert(shown >= current && shown <= current + 4);
        }
    }
    assert(ExtrapolationWeight(-1) == 1);
    assert(ExtrapolationWeight(1000) == 2);
    assert(ExtrapolationWeight(std::numeric_limits<double>::quiet_NaN()) == 1);
    assert(ExtrapolationWeight(std::numeric_limits<double>::infinity()) == 1);
    eagler::presentation::ExtrapolationClock clock;
    assert(clock.observe(0, 100, 1, .5) == 1);
    assert(clock.observe(1./120, 101, 1, .5) == 1.5f);
    // No new endpoint: a clock's modulo phase must not rewind the picture.
    assert(clock.observe(1./120, 101, 1, 0) == 2);
    for (int n = 0; n < 120; ++n) assert(clock.observe(1./120, 101, 1, 0) == 2);
    assert(clock.observe(1./120, 102, 1, .25) == 1.25f);
    // Rollback correction, generation change and rewind invalidate motion age.
    assert(clock.observe(.1, 102, 2, .5) == 1);
    assert(clock.observe(.1, 50, 2, .5) == 1);
    clock.reset();assert(clock.observe(.1, 50, 2, .5) == 1);
    std::puts("extrapolation: PASS (60-240 Hz, no retained-frame lag, bounded waits, correction reset)");
}
