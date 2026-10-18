#include <eagler/netplay/DirectTouchEquivalence.hpp>

#include <array>
#include <cassert>
#include <cstdio>
#include <limits>

using namespace Netplay;

static void ExpectReject(const DirectTouchEquivalenceResult &result,
                         DirectTouchEquivalenceReject reject,
                         std::size_t index = 0)
{
    assert(!result.equivalent);
    assert(result.reject == reject);
    assert(result.rejectIndex == index);
}

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
    ExpectReject(ProveDirectTouchEquivalent(nullptr, 0, actual),
                 DirectTouchEquivalenceReject::Metadata);
    auto result = ProveDirectTouchEquivalent(traces.data(), traces.size(), actual);
    assert(result.equivalent && result.carryX == 5.0f && result.carryY == 0.0f);
    assert(result.framesChecked == 3);

    traces[0].remaining.x = 0.0f;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::DirectionOrBoundary);
    traces[0] = Trace(10, 24.0f, 20.0f);
    traces[1].rightMargin = 22.0f;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::DirectionOrBoundary, 1);
    traces[1] = Trace(11, 20.0f, 16.0f);

    traces[1].frame = 13;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::TraceGap, 1);
    traces[1] = Trace(11, 20.0f, 16.0f);

    traces[1].remaining.active = false;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::UnlimitedOrInactive, 1);
    traces[1] = Trace(11, 20.0f, 16.0f);

    traces[1].applied.y = 1.0f;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::CrossAxis, 1);
    traces[1] = Trace(11, 20.0f, 16.0f);

    actual.y = 1.0f;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::DeltaShape);
    actual.y = 0.0f;
    actual.x = std::numeric_limits<float>::infinity();
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::DeltaShape);
    actual.x = 5.0f;
    actual.buttons = 4;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::Metadata);
    actual.buttons = 0;

    actual.unlimited = true;
    ExpectReject(ProveDirectTouchEquivalent(traces.data(), traces.size(), actual),
                 DirectTouchEquivalenceReject::Metadata);
    actual.unlimited = false;

    traces[2].used.analogMode = AnalogMode::DirectTouchBegin;
    result = ProveDirectTouchEquivalent(traces.data(), traces.size(), actual);
    assert(result.equivalent && result.carryX == 0.0f && result.framesChecked == 2);

    DirectTouchFrameTrace yTrace{};
    yTrace.frame = 20;
    yTrace.used.analogMode = AnalogMode::DirectTouchDelta;
    yTrace.used.touchUsed = true;
    yTrace.applied = {0.0f, -12.0f, true};
    yTrace.remaining = {0.0f, -8.0f, true};
    yTrace.leftMargin = yTrace.rightMargin = yTrace.topMargin = yTrace.bottomMargin = 200.0f;
    actual.x = 0.0f;
    actual.y = -3.0f;
    result = ProveDirectTouchEquivalent(&yTrace, 1, actual);
    assert(result.equivalent && result.carryX == 0.0f && result.carryY == -3.0f);

    std::puts("direct touch equivalence: PASS axis/boundary/reset guards");
}
