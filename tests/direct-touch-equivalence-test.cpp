#include <eagler/netplay/DirectTouchEquivalence.hpp>

#include <array>
#include <cassert>
#include <cstdio>

using namespace Netplay;

static DirectTouchFrameTrace Trace(std::uint32_t frame, float applied, float remaining,
                                   float left = 200.0f, float right = 200.0f)
{
    DirectTouchFrameTrace trace{};
    trace.frame = frame;
    trace.used.analogMode = AnalogMode::DirectTouchDelta;
    trace.used.touchUsed = true;
    trace.applied = {applied, 0.0f, true};
    trace.remaining = {remaining, 0.0f, true};
    trace.leftMargin = left;
    trace.rightMargin = right;
    trace.topMargin = trace.bottomMargin = 200.0f;
    return trace;
}

int main()
{
    FrameInput actual{};
    actual.analogMode = AnalogMode::DirectTouchDelta;
    actual.touchUsed = true;
    actual.x = 5.0f;

    std::array<DirectTouchFrameTrace, 3> traces{
        Trace(10, 24.0f, 20.0f), Trace(11, 20.0f, 16.0f), Trace(12, 16.0f, 12.0f)};
    auto result = ProveDirectTouchEquivalent(traces.data(), traces.size(), actual);
    assert(result.equivalent && result.carryX == 5.0f && result.carryY == 0.0f);
    assert(result.framesChecked == 3);

    traces[0].remaining.x = 0.0f;
    assert(!ProveDirectTouchEquivalent(traces.data(), traces.size(), actual).equivalent);
    traces[0] = Trace(10, 24.0f, 20.0f);
    traces[1].rightMargin = 22.0f;
    assert(!ProveDirectTouchEquivalent(traces.data(), traces.size(), actual).equivalent);
    traces[1] = Trace(11, 20.0f, 16.0f);

    actual.y = 1.0f;
    assert(!ProveDirectTouchEquivalent(traces.data(), traces.size(), actual).equivalent);
    actual.y = 0.0f;
    actual.buttons = 4;
    assert(!ProveDirectTouchEquivalent(traces.data(), traces.size(), actual).equivalent);
    actual.buttons = 0;

    traces[2].used.analogMode = AnalogMode::DirectTouchBegin;
    result = ProveDirectTouchEquivalent(traces.data(), traces.size(), actual);
    assert(result.equivalent && result.carryX == 0.0f && result.framesChecked == 2);

    std::puts("direct touch equivalence: PASS saturated axis/boundary/reset guards");
}
