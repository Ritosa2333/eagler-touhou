#include "ReplayPlayback.hpp"
#include <cassert>

int main() {
    static th08::ReplayPlayback playback;
    unsigned maximum = 0;
    for (unsigned flags : {0u, 4u, 512u, 516u})
        for (int mode : {0, 1, 2})
            for (bool dialogue : {false, true})
                for (bool waiting : {false, true})
                    for (bool boss : {false, true})
                        for (unsigned start = 0; start < 30; ++start) {
                            playback.stream.frame = start;
                            unsigned ticks = 0;
                            do {
                                ++playback.stream.frame;
                                ++ticks;
                                assert(ticks <= 15);
                            } while (playback.after_update(flags, mode, dialogue, waiting, boss)
                                     == th08::JobResult::Restart);
                            if (!(flags & 4) || (flags & 512)) assert(ticks == 1);
                            if (ticks > maximum) maximum = ticks;
                        }
    // Dialogue's period 3 and fast playback's period 5 can both be active.
    assert(maximum == 15);
}
