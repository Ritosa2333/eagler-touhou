#include "PurpleInput.hpp"
#include <eagler/thprac/PracticeSpeed.hpp>
#include <cassert>
#include <cstdio>

int main()
{
    // Every combination, including force-Shift overriding disable-Shift.
    for (unsigned options = 0; options < 32; ++options)
    {
        PracticeRuntime::PurpleInput input;
        input.disable_xkey = options & 1;
        input.disable_shiftkey = options & 2;
        input.disable_zkey = options & 4;
        input.force_shiftkey = options & 8;
        input.disable_Ckey_at_same_time = options & 16;
        for (unsigned buttons = 0; buttons < 65536; ++buttons)
        {
            assert(input.filter(buttons, false, false) == buttons);
            assert(input.filter(buttons, true, true) == buttons);
            auto result = input.filter(buttons, true, false);
            assert((result & ~7u) == (buttons & ~7u));
            assert(bool(result & 1) == (bool(buttons & 1) && !input.disable_zkey));
            assert(bool(result & 2) == (bool(buttons & 2) && !input.disable_xkey));
            assert(bool(result & 4) == (input.force_shiftkey || (bool(buttons & 4) && !input.disable_shiftkey)));
        }
        std::uint8_t keys[256]{};
        keys[67] = 0x80;
        input.apply(keys);
        assert(bool(keys[67]) == !(input.disable_xkey && input.disable_Ckey_at_same_time));
    }
    std::puts("TH07 purple input: 2097152 logical samples, OFF/replay passthrough, C/Shift precedence PASS");
    PracticeRuntime::PurpleInput retry;
    retry.enable_fast_retry = true;
    retry.begin_retry(0);
    assert(!retry.fast_retry_count_down);
    retry.begin_retry(1);
    for (int tick = 15; tick > 0; --tick) {
        assert(retry.filter(0, true, true) == 0);
        assert(retry.filter(0, false, false) == 0);
        const auto result = retry.filter(0, true, false);
        assert(result & 8); // Escape
        assert(bool(result & 0x4000) == (tick == 1)); // TH07 R, not Q
        assert(!(result & 0x200));
        retry.gui_tick();
    }
    assert(retry.filter(0, true, false) == 0);
    std::puts("TH07 purple fast retry: native 15-tick Escape/R, OFF/replay passthrough PASS");
    eagler::thprac::PracticeSpeed speed;
    assert(speed.interval(false, false, false, false) == 1.0 / 60.0);
    speed.fps = 120;
    speed.fps_replay_fast = 240;
    speed.fps_replay_slow = 15;
    assert(speed.interval(false, false, false, false) == 1.0 / 120.0);
    assert(speed.interval(true, true, false, false) == 1.0 / 240.0);
    assert(speed.interval(true, false, true, false) == 1.0 / 15.0);
    speed.fps_debug_acc = 1;
    assert(speed.interval(true, false, false, true) == 1.0 / 9999.0);
    std::puts("TH07 shared purple logical speed: live/replay/slow/fast/debug PASS");
}
