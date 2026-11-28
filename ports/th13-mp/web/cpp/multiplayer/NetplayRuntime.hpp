#pragma once

#include "DigestWireFilter.hpp"
#include "SessionSetup.hpp"

#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

namespace th13::multiplayer {

class NetplayRuntime {
public:
    struct DigestSlot {
        std::uint32_t frame = 0;
        std::uint32_t local = 0, remote = 0;
        bool hasLocal = false, hasRemote = false;
    };
    static constexpr std::size_t kDigestSlots = 8;
    static constexpr std::uint32_t kDigestIntervalFrames =
        DigestWireFilter::kIntervalFrames;

    NetplayRuntime();
    ~NetplayRuntime();
    NetplayRuntime(const NetplayRuntime&) = delete;
    NetplayRuntime& operator=(const NetplayRuntime&) = delete;

    bool Reset(const SessionSetup& setup);
    void Clear();

    static Netplay::CoreConfig CoreConfigFor(const SessionSetup& setup);

    bool Connect(const char* relay_url);
    void Close() { transport_.Close(); }

    bool ConnectSpectator(const char* relay_url, const char* spectator_id);
    bool Spectator() const { return spectator_; }
    bool StagePlaybackReady() const {
        return spectator_ && stage_received_ && stage_playback_.Ready();
    }
    Netplay::SpectatorStagePlayback& StagePlayback() { return stage_playback_; }
    const char* SpectatorState() {
        return StagePlaybackReady() ? stage_view_.State(stage_playback_) : nullptr;
    }
    bool SpectatorControl(const char* run, const char* action, std::uint32_t frame) {
        return StagePlaybackReady() &&
               stage_view_.Control(stage_playback_, run, action, frame);
    }
    bool RecordSpectatorFrame(std::uint32_t frame, std::uint32_t stage,
                              const Netplay::FrameInput* inputs, unsigned count,
                              const std::vector<std::uint8_t>* initializer);
    bool PublishSpectator(std::uint32_t confirmed, std::uint32_t simulated);
    bool FinishSpectator(std::uint32_t end_frame);
    bool SpectatorPublishFailed() const { return publish_failed_; }
    auto SpectatorCheckpointNeeded(std::uint32_t frame, std::uint32_t stage) const {
        using Requirement = Netplay::SpectatorStagePublisher::CheckpointRequirement;
        if (publish_failed_ || spectator_ || setup_.local_player != 0)
            return Requirement::NotRequired;
        if (!publish_started_) return Requirement::Required;
        return stage_publisher_.NeedsCheckpoint(frame, stage);
    }
    std::uint32_t SpectatorBacklog() const {
        return static_cast<std::uint32_t>(spectator_frames_.size());
    }
    bool SpectatorStreamEnded() const {
        return stage_received_ && stage_playback_.History().Ended();
    }

    bool Pump(bool world_ready, std::uint64_t now_ms);
    bool PumpTerminal(std::uint64_t now_ms);
    bool TerminalReleased() const { return terminal_released_; }
    void NoteInputStarved();

    bool Capture(std::uint32_t capture_frame, const Netplay::FrameInput& input,
                 std::uint64_t now_ms);
    std::uint32_t CaptureFrame() const { return next_capture_; }
    void AdvanceCapturePhase() { ++next_capture_; }

    bool HasRollbackRequest() const { return core_.HasRollbackRequest(); }
    std::uint32_t RollbackFrame() const { return core_.RollbackFrame(); }
    void ClearRollbackRequest() { core_.ClearRollbackRequest(); }
    void SetLocalSimulation(bool enabled) { local_simulation_ = enabled; }
    bool BeginCorrection(std::uint32_t first);
    bool RestartCorrection(std::uint32_t first);
    bool EndCorrection();
    bool Correcting() const { return correction_end_ != Netplay::INVALID_FRAME; }
    std::uint32_t CorrectionEnd() const { return correction_end_; }
    std::uint32_t NextFrame() const { return next_frame_; }

    bool CanStart() const { return gate_.CanStart(); }
    bool AllPeersHello() const { return gate_.AllPeersHello(); }
    void MarkLocalReady() { gate_.MarkLocalReady(); }
    bool LocalReady() const { return gate_.LocalReady(); }

    Netplay::FrameDecision Prepare(std::uint32_t frame) const {
        return core_.PrepareFrame(frame);
    }
    bool MarkSimulated(std::uint32_t frame, const Netplay::FrameDecision& decision) {
        if (!core_.MarkSimulated(frame, decision)) return false;
        if (!Correcting()) ++next_frame_;
        else if (next_frame_ < correction_end_) ++next_frame_;
        return true;
    }
    bool ConfirmedInputs(std::uint32_t frame,
                         std::array<Netplay::FrameInput, 3>& out) const {
        return core_.ConfirmedInputs(frame, &out);
    }

    const SessionSetup& Setup() const { return setup_; }
    Netplay::RemoteInputResult SubmitRemote(unsigned seat, std::uint32_t frame,
                                            const Netplay::FrameInput& input) {
        return core_.SubmitRemoteInput(std::uint8_t(seat), frame, input);
    }
    double IntervalScale() const { return channel_.IntervalScale(); }

    std::uint32_t PacketsSent() const { return channel_.PacketsSent(); }
    std::uint32_t PacketsReceived() const { return channel_.PacketsReceived(); }
    std::uint32_t RepairsSent() const { return channel_.RepairsSent(); }
    std::uint32_t PacketsIgnored() const { return channel_.PacketsIgnored(); }
    bool TransportOpen() const { return transport_.IsOpen(); }
    std::uint32_t RouteCode() const {
        const char* mode = transport_.Mode();
        if (!mode) return 0;
        const std::string text(mode);
        return text == "rtc" ? 1 : text == "relay" ? 2 : 0;
    }

    bool SendDigestControl(std::uint32_t frame, std::uint32_t digest);
    void NoteRemote(std::uint32_t frame, std::uint32_t digest);
    bool DigestMismatched() const { return digest_mismatch_; }
    std::uint32_t DigestMismatchFrame() const { return digest_mismatch_frame_; }
    const DigestSlot& MismatchDigests() const { return mismatch_digests_; }
    std::uint64_t DigestsSent() const { return digests_sent_; }
    std::uint64_t DigestsReceived() const { return digests_received_; }
    std::uint64_t DesyncDetected() const { return desync_detected_; }
    struct DigestWireStats {
        std::uint64_t sent = 0;
        std::uint64_t received = 0;
        std::uint64_t desync = 0;
        bool mismatched = false;
    };
    static DigestWireStats DebugDigestStats();

    const char* Error() const;
    bool Connected() const { return transport_.IsOpen(); }
    bool TransportFailed() const { return transport_.Failed(); }
    bool TransportDisconnected() const { return transport_.Disconnected(); }
    std::uint32_t ConfirmedThroughAll() const {
        return core_.ConfirmedThroughAllRemotes();
    }
    std::uint32_t LastSimulated() const { return core_.LastSimulatedFrame(); }
    struct TerminalReport {
        std::uint32_t last_simulated, confirmed, acknowledged;
        bool channel_active, retiring, released, spectators_settled;
        std::uint64_t started_ms, release_ms;
    };
    TerminalReport Terminal() const {
        return {core_.LastSimulatedFrame(), core_.ConfirmedThroughAllRemotes(),
                core_.AcknowledgedLocalThroughAllRemotes(), channel_.Active(),
                channel_.Retiring(), terminal_released_, SpectatorsSettled(),
                terminal_started_ms_, terminal_release_ms_};
    }
    std::uint8_t LocalSeat() const { return setup_.local_player; }
    std::uint32_t GameplayAbiValue() const { return multiplayer::GameplayAbi(setup_); }

private:
    void MarkDesync(std::uint32_t frame);
    bool DrainSpectator(std::uint64_t now_ms);
    void ReleaseTerminalTransport();
    bool InputHistoryComplete() const;
    bool SpectatorsSettled() const {
        return publish_finished_ || publish_failed_ || !publish_started_ || setup_.local_player != 0;
    }
    static constexpr std::uint64_t kTerminalLingerMs = 1000;
    static constexpr std::uint64_t kTerminalTailMs = 8000;

    SessionSetup setup_{};
    bool configured_ = false;
    bool local_simulation_ = false;
    std::uint32_t next_capture_ = 0;
    Netplay::RollbackCore core_;
    std::uint32_t next_frame_ = 0;
    std::uint32_t correction_end_ = Netplay::INVALID_FRAME;
    Netplay::SessionGate gate_;
    Netplay::BrowserPeerTransport transport_;
    DigestWireFilter digest_filter_{transport_};
    Netplay::SessionChannel channel_{digest_filter_};
    std::array<DigestSlot, kDigestSlots> digest_slots_{};
    DigestSlot mismatch_digests_{};
    bool digest_mismatch_ = false;
    std::uint32_t digest_mismatch_frame_ = 0;
    std::uint64_t digests_sent_ = 0, digests_received_ = 0, desync_detected_ = 0;
    std::string error_;
    bool transport_lost_ = false;
    bool terminal_released_ = false;
    bool publish_finished_ = false;
    std::uint64_t terminal_started_ms_ = 0;
    std::uint64_t terminal_release_ms_ = 0;
    bool spectator_ = false;
    bool stage_received_ = false;
    bool publish_started_ = false;
    bool publish_failed_ = false;
    std::uint64_t spectator_deadline_ms_ = 0;
    std::uint32_t spectator_recorded_ = 0;
    std::uint32_t receive_frame_ = 0;
    Netplay::SpectatorStagePublisher stage_publisher_{};
    Netplay::SpectatorStagePlayback stage_playback_{
        Netplay::SpectatorStagePlayback::RestoreMode::Run,
        Netplay::SpectatorStageHistory::FullRunMaxFrames};
    Netplay::SpectatorStageView stage_view_{};
    std::deque<Netplay::SpectatorFramePacket> spectator_frames_{};
};

}
