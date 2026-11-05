#include <eagler/netplay/FramePacingPolicy.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>

namespace
{
double LegacyUpdate(double currentScale, double recommendedLead)
{
    const double deadbandLead = std::abs(recommendedLead) < 0.5 ? 0.0 : recommendedLead;
    const double desiredScale = std::clamp(1.0 + deadbandLead * 0.003, 0.98, 1.02);
    currentScale += (desiredScale - currentScale) * 0.08;
    if (std::abs(currentScale - 1.0) < 0.0002)
        currentScale = 1.0;
    return currentScale;
}
}

int main()
{
    using Netplay::FramePacingPolicy;

    static_assert(FramePacingPolicy::MaxAbsoluteLeadFrames == 30);
    static_assert(FramePacingPolicy::SignedFrameDelta(100, 90) == 10);
    static_assert(FramePacingPolicy::SignedFrameDelta(2, 0xfffffffeu) == 4);
    static_assert(FramePacingPolicy::InferLead(100, 90, 2) == 4);
    static_assert(FramePacingPolicy::AcceptLead(30));
    static_assert(FramePacingPolicy::AcceptLead(-30));
    static_assert(!FramePacingPolicy::AcceptLead(31));
    static_assert(!FramePacingPolicy::AcceptLead(-31));

    assert(FramePacingPolicy::UpdateScale(1.0, 0.4) == 1.0);
    assert(FramePacingPolicy::UpdateScale(1.0, 10.0) == 1.0016);
    assert(FramePacingPolicy::UpdateScale(1.0, -10.0) == 0.9984);

    std::uint32_t random = 0x58f172adu;
    for (unsigned i = 0; i < 10000; ++i)
    {
        random = random * 1664525u + 1013904223u;
        const double current = 0.95 + static_cast<double>(random % 10001u) / 100000.0;
        random = random * 1664525u + 1013904223u;
        const double lead = static_cast<double>(static_cast<int>(random % 801u) - 400) / 10.0;
        assert(FramePacingPolicy::UpdateScale(current, lead) == LegacyUpdate(current, lead));
    }

    std::puts("frame pacing policy: PASS legacy constants/formula and wraparound");
}
