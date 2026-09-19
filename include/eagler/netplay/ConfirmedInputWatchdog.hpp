#pragma once

#include <cstdint>

namespace Netplay
{
// Per-peer liveness guard for the authoritative confirmed-input frontier.
// The watchdog owns only progress timing. Session eligibility, required peer
// membership and the action taken on timeout stay with the title driver.
class ConfirmedInputWatchdog
{
public:
    static constexpr std::uint64_t DefaultTimeoutMs = 15'000;

    void Disarm()
    {
        armed_ = false;
    }

    bool Observe(std::uint32_t confirmedFrame, std::uint64_t nowMs,
                 std::uint64_t timeoutMs = DefaultTimeoutMs)
    {
        if (!armed_)
        {
            armed_ = true;
            lastConfirmedFrame_ = confirmedFrame;
            lastAdvanceMs_ = nowMs;
            return false;
        }

        if (confirmedFrame != lastConfirmedFrame_)
        {
            lastConfirmedFrame_ = confirmedFrame;
            lastAdvanceMs_ = nowMs;
            return false;
        }

        return nowMs - lastAdvanceMs_ >= timeoutMs;
    }

    bool Armed() const { return armed_; }
    std::uint32_t LastConfirmedFrame() const { return lastConfirmedFrame_; }
    std::uint64_t LastAdvanceMs() const { return lastAdvanceMs_; }

private:
    bool armed_ = false;
    std::uint32_t lastConfirmedFrame_ = 0;
    std::uint64_t lastAdvanceMs_ = 0;
};
} // namespace Netplay
