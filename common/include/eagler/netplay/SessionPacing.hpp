#pragma once

#include <eagler/netplay/FrameAdvantageWindow.hpp>
#include <eagler/netplay/FramePacingPolicy.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>

namespace Netplay
{
// Wall-clock advice only. Input frames, predictions and rollback never depend
// on this state; each remote contributes a separate latency-compensated window.
class SessionPacing
{
public:
    void Reset() { *this = SessionPacing{}; }
    void Observe(std::uint32_t localFrame, const InputPacket &packet)
    {
        const auto peer = packet.senderPlayer;
        if (peer >= MAX_PLAYERS || packet.senderFrame == INVALID_FRAME) return;
        if (seen_[peer] && packet.senderFrame <= lastFrame_[peer]) return;
        seen_[peer] = true;
        lastFrame_[peer] = packet.senderFrame;
        const auto lead = FramePacingPolicy::InferLead(localFrame, packet.senderFrame,
                                                       packet.frameAdvantage);
        if (!FramePacingPolicy::AcceptLead(lead) || !peers_[peer].AddSample(lead)) return;
        bool have = false;
        double recommendation = 0;
        for (const auto &window : peers_) if (window.ready)
        {
            if (!have || window.averageLead > recommendation) recommendation = window.averageLead;
            have = true;
        }
        lead_ = recommendation;
        scale_ = FramePacingPolicy::UpdateScale(scale_, lead_);
    }
    double IntervalScale() const { return scale_; }
    double Lead() const { return lead_; }

private:
    std::array<FrameAdvantageWindow, MAX_PLAYERS> peers_{};
    std::array<std::uint32_t, MAX_PLAYERS> lastFrame_{};
    std::array<bool, MAX_PLAYERS> seen_{};
    double scale_ = 1.0, lead_ = 0;
};
}
