#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer netplay runtime must not enter an ordinary build
#endif

#include "SessionSetup.hpp"

#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>

#include <cstdint>

namespace th10::multiplayer {

// Title-owned deterministic netplay driver state. Transport and rollback byte
// storage stay in eagler-common; this owner binds the common session/core to
// TH10's session contract and exposes one frame decision at a time.
class NetplayRuntime {
public:
    bool Reset(const SessionSetup& setup, std::uint64_t sessionId) noexcept;
    void Clear() noexcept;

    Netplay::SessionPacket Hello() const { return gate_.BuildPacket(Netplay::SessionPhase::Hello); }
    Netplay::SessionPacket Ready() const { return gate_.BuildPacket(Netplay::SessionPhase::Ready); }
    Netplay::SessionPacketResult ApplySession(const Netplay::SessionPacket& packet) {
        return gate_.Apply(packet);
    }
    bool CanSendReady() const { return gate_.CanSendReady(); }
    void MarkLocalReady() { gate_.MarkLocalReady(); }
    bool CanStart() const { return configured_ && gate_.CanStart(); }

    bool CaptureLocal(std::uint32_t frame, const Netplay::FrameInput& input) {
        return configured_ && core_.ScheduleLocalInput(frame, input);
    }
    Netplay::RemoteInputResult SubmitRemote(std::uint8_t player, std::uint32_t frame,
                                            const Netplay::FrameInput& input) {
        return configured_ ? core_.SubmitRemoteInput(player, frame, input)
                           : Netplay::RemoteInputResult::InvalidPlayer;
    }
    Netplay::FrameDecision Prepare(std::uint32_t frame) const {
        return configured_ ? core_.PrepareFrame(frame) : Netplay::FrameDecision{};
    }
    bool MarkSimulated(std::uint32_t frame, const Netplay::FrameDecision& decision) {
        return configured_ && core_.MarkSimulated(frame, decision);
    }

    bool HasRollbackRequest() const { return configured_ && core_.HasRollbackRequest(); }
    std::uint32_t RollbackFrame() const { return core_.RollbackFrame(); }
    void ClearRollbackRequest() { core_.ClearRollbackRequest(); }
    std::uint32_t LastSimulatedFrame() const { return core_.LastSimulatedFrame(); }
    std::uint32_t NextFrame() const {
        const auto last=core_.LastSimulatedFrame();
        return last==Netplay::INVALID_FRAME?0:last+1;
    }
    bool HasLocalCapture(std::uint32_t frame) const { return core_.HasLocalCapture(frame); }
    bool InputPresent(std::uint8_t player,std::uint32_t frame) const {
        return core_.InputPresent(player,frame);
    }
    std::uint32_t ConfirmedThroughAllRemotes() const {
        return core_.ConfirmedThroughAllRemotes();
    }
    const Netplay::SessionConfig& Config() const { return gate_.Config(); }
    bool Configured() const { return configured_; }

private:
    Netplay::SessionGate gate_{};
    Netplay::RollbackCore core_{};
    bool configured_ = false;
};

} // namespace th10::multiplayer
