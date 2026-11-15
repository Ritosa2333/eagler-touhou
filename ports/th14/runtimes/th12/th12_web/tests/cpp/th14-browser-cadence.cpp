#include "../../../portable/sdl/FrameCadence.hpp"
#include <cassert>

int main() {
    using touhou::sdl::FrameCadence;
    FrameCadence clock;
    constexpr double interval = FrameCadence::interval;
    assert(clock.advance_display(0, true) == 0);
    for (int i = 0; i < 120; ++i)
        assert(clock.advance_display(interval + (i % 2 ? .001 : -.001), true) == 1);
    clock.reset();
    for (int i = 0; i < 60; ++i)
        assert(clock.advance_display(2 * interval, true) == 2);
    assert(clock.advance_display(1, true) == 2);
    assert(clock.advance_display(2 * interval, false) == 1);
    clock.reset();
    unsigned updates = 0;
    for (int i = 0; i < 120; ++i) {
        const auto ticks = clock.advance_display(interval / 2, true);
        assert(ticks <= 1);
        updates += ticks;
    }
    assert(updates >= 59 && updates <= 61);
}
