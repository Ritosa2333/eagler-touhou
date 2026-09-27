#pragma once

#include <eagler/netplay/ConfirmedInputWatchdog.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/PeerTransport.hpp>
#include <eagler/netplay/SessionPacing.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Netplay
{
struct SessionChannelConfig
{
    std::uint64_t connectTimeoutMs = 45'000;
    std::uint64_t confirmedTimeoutMs = ConfirmedInputWatchdog::DefaultTimeoutMs;
    std::uint64_t controlResendMs = 200;
    std::uint64_t inputResendMs = 32;
    std::uint64_t repairIntervalMs = 250;
    std::size_t bufferedLimit = 256 * 1024;
    std::size_t receiveBudget = 256;
};

// Network-only protocol lifecycle. All clock, queue, sequence and liveness
// state is explicitly outside the title's rollback journal. The title owns
// SessionGate/Core, chooses its gameplay contract, samples input once and
// performs every restore/update/draw. This component never simulates a title.
class SessionChannel
{
public:
    enum class Failure : std::uint32_t
    {
        None, InvalidConfiguration, Transport, HandshakeTimeout,
        MalformedPacket, ContractMismatch, InvalidPeer, InputConflict,
        ConfirmedTimeout, ClockReversed, InvalidCapture, InvalidRetirement,
    };

    explicit SessionChannel(PeerTransport &transport);
    SessionChannel(const SessionChannel &) = delete;
    SessionChannel &operator=(const SessionChannel &) = delete;

    void Clear();
    bool BeginSession(const SessionConfig &session, std::uint64_t nowMs,
                      const SessionChannelConfig &config = {});
    bool Pump(SessionGate &gate, RollbackCore &core, std::uint64_t nowMs,
              bool expectsInput);
    // Notify only after ScheduleLocalInput succeeds. A retry never samples
    // again; the same captured frame may be sent any number of times.
    bool LocalCaptured(const RollbackCore &core, std::uint32_t captureFrame,
                       std::uint64_t nowMs);

    bool CanRetire(const RollbackCore &core, std::uint32_t terminalFrame) const;
    bool Retire(const RollbackCore &core, std::uint32_t terminalFrame,
                std::uint64_t nowMs);

    Failure Error() const { return failure_; }
    bool Active() const { return active_; }
    bool Retiring() const { return previousSession_ != 0; }
    std::uint32_t LatestCapture() const { return latestCapture_; }
    std::uint32_t PacketsSent() const { return sent_; }
    std::uint32_t PacketsReceived() const { return received_; }
    std::uint32_t PacketsIgnored() const { return ignored_; }
    std::uint32_t RepairsSent() const { return repairs_; }
    std::uint32_t PeerFrame(std::uint8_t peer) const;
    // Optional wall-clock advice. The consumer owns whether to apply it;
    // this channel never changes logical input or simulation frames.
    double IntervalScale() const { return pacing_.IntervalScale(); }
    double FrameLead() const { return pacing_.Lead(); }
    const char *ErrorText() const;

private:
    bool Fail(Failure failure);
    bool ObserveClock(std::uint64_t nowMs);
    bool SendSession(const SessionGate &gate);
    bool SendInputs(const RollbackCore &core, std::uint64_t nowMs, bool force);
    bool SendRetired(std::uint64_t nowMs);
    bool Receive(SessionGate &gate, RollbackCore &core, std::uint64_t nowMs);

    PeerTransport &transport_;
    SessionChannelConfig policy_{};
    SessionConfig session_{};
    bool active_ = false, observedClock_ = false, helloSent_ = false;
    bool forceControl_ = false, forceInputs_ = false;
    Failure failure_ = Failure::None;
    std::uint64_t lastClock_ = 0, beginTime_ = 0, nextControl_ = 0, nextInput_ = 0;
    std::uint32_t latestCapture_ = INVALID_FRAME;
    std::array<std::uint32_t, MAX_PLAYERS> sentSequence_{}, receivedSequence_{};
    std::array<std::uint32_t, MAX_PLAYERS> peerFrame_{}, repairFrontier_{};
    std::array<std::uint64_t, MAX_PLAYERS> lastRepair_{};
    std::array<bool, MAX_PLAYERS> peerNeedsAck_{};
    std::array<ConfirmedInputWatchdog, MAX_PLAYERS> watchdog_{};
    SessionPacing pacing_;
    std::vector<std::uint8_t> incoming_, outgoing_;

    // One retired epoch's terminal ACKs remain answerable until each peer
    // sends HELLO for the next epoch. All peers had acknowledged our terminal
    // input before this cache was made, so no old gameplay history is needed.
    // Without this echo a lost final ACK strands the slower peer forever.
    std::uint64_t previousSession_ = 0, nextRetiredSend_ = 0;
    std::uint8_t previousCount_ = 0, previousLocal_ = 0;
    std::array<bool, MAX_PLAYERS> previousNeeded_{};
    std::array<std::vector<std::uint8_t>, MAX_PLAYERS> terminalAcks_{};
    std::uint32_t sent_ = 0, received_ = 0, ignored_ = 0, repairs_ = 0;
};
} // namespace Netplay
