#include <eagler/presentation/FrameDeadline.hpp>

#include <cassert>

int main()
{
    using eagler::presentation::DueFrameSlots;
    using eagler::presentation::SingleFrameRemainingTime;
    using eagler::presentation::WebFrameEarlyTolerance;
    constexpr double period = 1.0 / 60.0;
    const double tolerance = WebFrameEarlyTolerance(period);
    double previous = 0.0;
    double browserDebt = 0.0;
    double exactDebt = 0.0;
    unsigned exactPictures = 0;

    for (unsigned frame = 1; frame <= 600; ++frame)
    {
        const double offset = frame % 2 ? -0.0014 : 0.0006;
        const double now = frame * period + offset;
        const double elapsed = now - previous;
        previous = now;
        browserDebt += elapsed;
        exactDebt += elapsed;

        const auto browserSlots = DueFrameSlots(browserDebt, period, tolerance);
        assert(browserSlots == 1);
        browserDebt -= browserSlots * period;
        assert(browserDebt >= -tolerance);

        const auto exactSlots = DueFrameSlots(exactDebt, period, 0.0);
        exactDebt -= exactSlots * period;
        exactPictures += exactSlots > 0;
    }
    assert(exactPictures < 600);

    // The initial phase can place every other callback just beyond the
    // early tolerance. Expiring both due slots on the next callback traps
    // the loop at 30 FPS even though the browser delivers 60 callbacks.
    double jitterDebt = -tolerance;
    unsigned jitterPictures = 0;
    for (unsigned frame = 0; frame < 600; ++frame)
    {
        const double elapsed = period + (frame % 2 ? 0.001 : -0.001);
        jitterDebt += elapsed;
        const auto due = DueFrameSlots(jitterDebt, period, tolerance);
        if (due == 0)
            continue;
        jitterDebt = SingleFrameRemainingTime(jitterDebt, elapsed, period, tolerance, due, true);
        ++jitterPictures;
    }
    assert(jitterPictures == 599);

    double clusteredDebt = 0.0;
    double duration = 0.0;
    unsigned clusteredPictures = 0;
    for (unsigned frame = 0; frame < 720; ++frame)
    {
        const double elapsed = frame % 3 == 2 ? 0.0238 : 0.0089;
        duration += elapsed;
        clusteredDebt += elapsed;
        const auto due = DueFrameSlots(clusteredDebt, period, tolerance);
        if (due == 0)
            continue;
        clusteredDebt = SingleFrameRemainingTime(clusteredDebt, elapsed, period, tolerance, due, true);
        ++clusteredPictures;
    }
    // Arrival and completion can each fall on either side of a deadline.
    assert(std::abs(clusteredPictures - duration / period) <= 2.0);

    clusteredDebt = duration = 0.0;
    clusteredPictures = 0;
    for (unsigned frame = 0; frame < 14 * 36; ++frame)
    {
        const double elapsed = (frame % 14 == 13 ? 5.0 : 1.0) / 144.0;
        duration += elapsed;
        clusteredDebt += elapsed;
        const auto due = DueFrameSlots(clusteredDebt, period, tolerance);
        if (due == 0)
            continue;
        clusteredDebt = SingleFrameRemainingTime(clusteredDebt, elapsed, period, tolerance, due, true);
        ++clusteredPictures;
    }
    assert(std::abs(clusteredPictures - duration / period) <= 2.0);

    const double stalled = 0.11;
    const auto stalledSlots = DueFrameSlots(stalled, period, tolerance);
    assert(SingleFrameRemainingTime(stalled, stalled, period, tolerance, stalledSlots, true) < period);

    for (const unsigned refresh : {60u, 90u, 120u, 144u, 165u})
    {
        double debt = 0.0;
        unsigned ticks = 0;
        for (unsigned frame = 0; frame < refresh * 10; ++frame)
        {
            debt += 1.0 / refresh;
            const auto due = DueFrameSlots(debt, period, tolerance, 4);
            assert(due <= 1);
            debt -= due * period;
            ticks += due;
        }
        assert(ticks == 600);
    }

    assert(DueFrameSlots(-tolerance, period, tolerance) == 0);
    assert(DueFrameSlots(0.12, period, tolerance, 4) == 4);
    assert(DueFrameSlots(0.12, period, tolerance) > 4);
}
