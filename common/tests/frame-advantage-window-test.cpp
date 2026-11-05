#include <eagler/netplay/FrameAdvantageWindow.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <vector>

int main()
{
    Netplay::FrameAdvantageWindow window;
    Netplay::FrameAdvantageWindow otherPeer;
    std::vector<std::int32_t> reference;
    std::uint32_t random = 0x7135abcdu;

    // Compare against the former dynamically allocated implementation across
    // warm-up, full-window wraparound, outliers and reset.
    for (unsigned index = 0; index < 10000; ++index)
    {
        if (index == 5000)
        {
            window = {};
            reference.clear();
            assert(!window.ready && window.averageLead == 0.0);
        }
        random = random * 1664525u + 1013904223u;
        const std::int32_t value = index % 127 == 0
            ? std::numeric_limits<std::int32_t>::max()
            : index % 131 == 0 ? std::numeric_limits<std::int32_t>::min()
            : static_cast<std::int32_t>(random % 61) - 30;
        reference.push_back(value);
        if (reference.size() > 64)
            reference.erase(reference.begin());

        const bool ready = window.AddSample(value);
        assert(ready == (reference.size() >= 20));
        assert(window.ready == ready);
        assert(!otherPeer.ready && otherPeer.averageLead == 0.0);
        if (!ready)
            continue;

        auto sorted = reference;
        std::sort(sorted.begin(), sorted.end());
        const std::size_t trim = std::min<std::size_t>(4, sorted.size() / 8);
        std::int64_t sum = 0;
        for (std::size_t i = trim; i < sorted.size() - trim; ++i)
            sum += sorted[i];
        const double expected = static_cast<double>(sum) /
                                static_cast<double>(sorted.size() - trim * 2);
        assert(window.averageLead == expected);
    }

    for (unsigned index = 0; index < 64; ++index)
        otherPeer.AddSample(-9);
    assert(otherPeer.ready && otherPeer.averageLead == -9.0);
    std::puts("frame advantage window: PASS 10000 reference comparisons");
}
