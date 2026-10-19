#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace Netplay
{
// Allocation-free per-peer frame-advantage smoothing. This preserves the
// existing TH06/TH07 64-sample trimmed-mean policy while keeping each remote
// endpoint in an independent window.
struct FrameAdvantageWindow
{
    double averageLead = 0.0;
    bool ready = false;

    bool AddSample(std::int32_t lead)
    {
        samples_[cursor_] = lead;
        cursor_ = (cursor_ + 1) % samples_.size();
        count_ = std::min(count_ + 1, samples_.size());
        if (count_ < 20)
            return false;

        auto sorted = samples_;
        std::sort(sorted.begin(), sorted.begin() + count_);
        const std::size_t trim = std::min<std::size_t>(4, count_ / 8);
        std::int64_t sum = 0;
        for (std::size_t index = trim; index < count_ - trim; ++index)
            sum += sorted[index];
        averageLead = static_cast<double>(sum) /
                      static_cast<double>(count_ - trim * 2);
        ready = true;
        return true;
    }

private:
    std::array<std::int32_t, 64> samples_{};
    std::size_t count_ = 0;
    std::size_t cursor_ = 0;
};
} // namespace Netplay
