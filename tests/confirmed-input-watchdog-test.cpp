#include <eagler/netplay/ConfirmedInputWatchdog.hpp>

#include <cassert>
#include <cstdio>

int main()
{
    using Netplay::ConfirmedInputWatchdog;

    static_assert(ConfirmedInputWatchdog::DefaultTimeoutMs == 15'000);

    ConfirmedInputWatchdog watchdog;
    assert(!watchdog.Armed());
    assert(!watchdog.Observe(100, 1'000));
    assert(watchdog.Armed());
    assert(watchdog.LastConfirmedFrame() == 100);
    assert(watchdog.LastAdvanceMs() == 1'000);

    assert(!watchdog.Observe(100, 15'999));
    assert(watchdog.Observe(100, 16'000));
    assert(watchdog.Observe(100, 20'000));

    // Any frontier change restarts the liveness window.
    assert(!watchdog.Observe(101, 20'000));
    assert(watchdog.LastConfirmedFrame() == 101);
    assert(watchdog.LastAdvanceMs() == 20'000);
    assert(!watchdog.Observe(101, 34'999));
    assert(watchdog.Observe(101, 35'000));

    // A title-owned harness may choose a wider window without changing the
    // production 15-second default.
    watchdog.Disarm();
    assert(!watchdog.Armed());
    assert(!watchdog.Observe(7, 5'000, 60'000));
    assert(!watchdog.Observe(7, 64'999, 60'000));
    assert(watchdog.Observe(7, 65'000, 60'000));

    std::puts("confirmed input watchdog: PASS arm/progress/default/override");
}
