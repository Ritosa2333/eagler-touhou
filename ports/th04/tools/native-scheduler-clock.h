#pragma once

// Keep the clock and frame deadline in ordinary WASM memory so snapshots also
// restore them. Host accessors bind to this block once, before startup.
struct THSchedulerClock {
    double now;
    double deadline;
    double mode; // 0: wall clock; 1: virtual clock; 2: virtual clock + batching
};
extern THSchedulerClock thSchedulerClock;

