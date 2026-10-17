#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer netplay runtime must not enter an ordinary build
#endif

#include "SessionSetup.hpp"
#include "NetworkInputGapPacing.hpp"

#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/AdonisConnection.hpp>
#include <eagler/netplay/AdonisSpectatorTiming.hpp>
#include <eagler/netplay/InputGapPacing.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>

#include <cstdint>
#include <array>
#include <deque>
#include <string>
#include <vector>

namespace th10::multiplayer {

// Title-owned deterministic netplay driver state. Transport and rollback byte
// storage stay in eagler-common; this owner binds the common session/core to
// TH10's session contract and exposes one frame decision at a time.
class NetplayRuntime {
public:
    enum class WireResult:std::uint32_t {Accepted=1,IgnoredSession=2,Malformed=3,ContractMismatch=4,InvalidPeer=5};
    bool Reset(const SessionSetup& setup, std::uint64_t sessionId) noexcept;
    bool BeginPlayback(SessionSetup& setup) noexcept;
    bool FeedPlayback(std::uint32_t frame,const Netplay::FrameInput* inputs,std::size_t count);
    bool Playback()const{return playback_;}
    bool Spectator()const{return spectator_;}
    bool SpectatorRetired()const{return spectator_retired_;}
    std::size_t SpectatorBacklog()const{
        if(!spectator_stage_playback_.History().Segments().empty()){
            const auto received=spectator_stage_playback_.History().Received();
            const auto position=spectator_stage_playback_.Position();
            return received>position?received-position:0;
        }
        return spectator_frames_.size();
    }
    bool StagePlaybackReady()const{return spectator_&&spectator_stage_playback_.Ready();}
    Netplay::SpectatorStagePlayback& StagePlaybackController(){return spectator_stage_playback_;}
    const Netplay::SpectatorStagePlayback& StagePlaybackController()const{return spectator_stage_playback_;}
    const char* SpectatorPlaybackState();
    bool SpectatorPlaybackControl(const char* run,const char* action,std::uint32_t frame);
    bool SpectatorCheckpointRequired(std::uint32_t frame,std::uint32_t stage)const;
    void StopSpectatorPublication(){
        if(!spectator_&&!playback_&&setup_.localPlayer==0){
            spectator_publish_failed_=true;transport_.StopSpectators();
        }
    }
    bool RecordSpectatorFrame(std::uint32_t frame,std::uint32_t stage,
                              const std::vector<std::uint8_t>& initializer,
                              const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs,
                              std::uint8_t bossMask);
    void PublishConfirmedSpectatorHistoryAfterReconcile(){
        if(!Correcting()&&!HasRollbackRequest())PublishConfirmedSpectatorFrames();
    }
    bool FeedSpectator(std::uint32_t frame);
    void Clear() noexcept;
    // The title must have reconciled/confirmed the restart fence. A real
    // transport additionally finishes its peer ACK fence before retirement.
    bool PrepareTransitionFence();
    bool RetireRun() noexcept;
    bool CanRetireRun() const;
    bool BeginNextRun(SessionSetup& setup,std::uint32_t seed) noexcept;
    std::uint32_t Generation()const{return generation_;}
    bool Retired()const{return retired_;}
    WireResult ApplyWire(const std::uint8_t* bytes,std::size_t size);
    bool BuildInputWire(std::uint8_t peer,std::uint32_t latest,std::uint32_t sequence,
                        std::uint32_t ack,std::vector<std::uint8_t>& out)const;

    Netplay::SessionPacket Hello() const { return gate_.BuildPacket(Netplay::SessionPhase::Hello); }
    Netplay::SessionPacket Ready() const { return gate_.BuildPacket(Netplay::SessionPhase::Ready); }
    Netplay::SessionPacketResult ApplySession(const Netplay::SessionPacket& packet) {
        if(spectator_)return Netplay::SessionPacketResult::InvalidPeer;
        return gate_.Apply(packet);
    }
    bool CanSendReady() const { return !spectator_&&gate_.CanSendReady(); }
    bool LocalReady() const { return configured_&&!spectator_&&gate_.LocalReady(); }
    void MarkLocalReady() { if(!spectator_)gate_.MarkLocalReady(); }
    bool CanStart() const { return configured_ && (spectator_?spectator_timing_ready_:playback_ || gate_.CanStart()); }

    bool CaptureLocal(std::uint32_t frame, const Netplay::FrameInput& input);
    Netplay::RemoteInputResult SubmitRemote(std::uint8_t player, std::uint32_t frame,
                                            const Netplay::FrameInput& input) {
        return configured_&&!playback_&&!spectator_
            ? core_.SubmitRemoteInput(player, frame, input)
            : Netplay::RemoteInputResult::InvalidPlayer;
    }
    Netplay::FrameDecision Prepare(std::uint32_t frame) const {
        return CanStart() ? core_.PrepareFrame(frame) : Netplay::FrameDecision{};
    }
    bool MarkSimulated(std::uint32_t frame, const Netplay::FrameDecision& decision) {
        if(!CanStart()||!core_.MarkSimulated(frame,decision))return false;
        network_gap_pacing_locked_=true;return true;
    }

    bool HasRollbackRequest() const { return configured_ && core_.HasRollbackRequest(); }
    std::uint32_t RollbackFrame() const { return core_.RollbackFrame(); }
    void ClearRollbackRequest() { core_.ClearRollbackRequest(); }
    std::uint32_t LastSimulatedFrame() const { return core_.LastSimulatedFrame(); }
    bool RewindSimulationTo(std::uint32_t frame) {
        return configured_&&core_.RewindSimulationTo(frame);
    }
    std::uint32_t NextFrame() const {
        const auto last=core_.LastSimulatedFrame();
        return last==Netplay::INVALID_FRAME?0:last+1;
    }
    bool HasLocalCapture(std::uint32_t frame) const { return core_.HasLocalCapture(frame); }
    // Recovery progress is host-owned, outside the rewindable world. Keep
    // the old exclusive frontier even when the simulation cursor rewinds.
    bool Correcting() const { return correction_end_!=Netplay::INVALID_FRAME; }
    std::uint32_t CorrectionEnd() const { return correction_end_; }
    std::uint32_t PredictionFrontier() const {
        const auto next=NextFrame();
        return Correcting()&&correction_end_>next?correction_end_:next;
    }
    void BeginCorrection(std::uint32_t end) {
        if(!Correcting()||end>correction_end_)correction_end_=end;
    }
    void EndCorrection() { correction_end_=Netplay::INVALID_FRAME; }
    bool CanCapture() const {
        return CanStart()&&!playback_&&!spectator_&&!retired_&&
            (!Correcting()||NextFrame()>=correction_end_);
    }
    std::uint32_t MissingRemoteInputs() const {
        const auto through=ConfirmedThroughAllRemotes();
        const auto actual=through==Netplay::INVALID_FRAME?0:through+1;
        const auto frontier=PredictionFrontier();
        return frontier>actual?frontier-actual:0;
    }
    bool InputPresent(std::uint8_t player,std::uint32_t frame) const {
        return core_.InputPresent(player,frame);
    }
    std::uint32_t ConfirmedThroughAllRemotes() const {
        return core_.ConfirmedThroughAllRemotes();
    }
    std::uint32_t ConfirmedThrough(std::uint8_t seat) const {
        return core_.ConfirmedThrough(seat);
    }
    std::uint32_t AcknowledgedLocalThroughAllRemotes() const {
        return core_.AcknowledgedLocalThroughAllRemotes();
    }
    bool ConfirmedInputs(std::uint32_t frame,std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>* out)const {
        return core_.ConfirmedInputs(frame,out);
    }
    const Netplay::SessionConfig& Config() const { return gate_.Config(); }
    bool Configured() const { return configured_; }

    bool Connect(const char* relayUrl);
    bool ConnectSpectator(const char* relayUrl,const char* spectatorId);
    bool PumpNetwork(bool expectsInput,bool captureFollows=false);
    bool FlushNetworkInputs();
    bool NetworkEnabled() const { return network_enabled_; }
    bool PreparingWorld()const{return configured_&&setup_.version>=4&&setup_.adonis_mode!=0&&!playback_&&!spectator_;}
    bool AllowsRollback()const{return !playback_&&!spectator_&&setup_.adonis_mode!=unsigned(Netplay::AdonisMode::Delay);}
    Netplay::AdonisMode Mode()const{return Netplay::AdonisMode(setup_.adonis_mode);}
    const SessionSetup& Setup()const{return setup_;}
    const std::uint32_t* CalibrationStatus(){return calibration_.Status();}
    double PacedElapsedSeconds(double elapsed);
    // Independent startup choices, outside the wire/gameplay contract. A new
    // live Rollback session defaults to deep-gap pacing ON and extra Draw OFF.
    // A real capture or commit locks both across rewind and run generations;
    // Clear is neutral. No setter is used by the product UI.
    bool SetNetworkGapTailEnabled(bool enabled);
    bool NetworkGapTailEnabled()const{return network_gap_tail_enabled_;}
    bool NetworkGapTailLocked()const{return network_gap_pacing_locked_;}
    bool SetNetworkTailPresentationEnabled(bool enabled);
    bool NetworkTailPresentationEnabled()const{return network_tail_presentation_enabled_;}
    bool NetworkTailPresentationLocked()const{return network_gap_pacing_locked_;}
    double InputGapScale()const{
        return network_gap_tail_enabled_?network_input_gap_tail_.Scale():input_gap_pacing_.Scale();
    }
    double IntervalScale() const {
        const auto gap=InputGapScale(),phase=channel_.IntervalScale();
        return gap>1.?(gap>phase?gap:phase):phase;
    }
    // The HELLO/READY gate agrees on the session, not on completion of each
    // browser's title/resource loading. First input proves the world is ready.
    bool InitialInputsReady() const;
    const Netplay::SessionChannel& Channel() const { return channel_; }
    const Netplay::BrowserPeerTransport& Transport() const { return transport_; }
    const char* NetworkError() const;

private:
    bool Configure(const SessionSetup& setup,std::uint64_t id) noexcept;
    bool FeedAuthoritative(std::uint32_t frame,const Netplay::FrameInput* inputs,
                           std::size_t count);
    bool DrainSpectatorFrames();
    void PublishConfirmedSpectatorFrames();
    bool ReceiveSpectatorStageWire(const std::uint8_t*,std::size_t);
    Netplay::SessionGate gate_{};
    Netplay::RollbackCore core_{};
    SessionSetup setup_{};
    std::uint64_t base_session_id_=0;
    std::uint32_t generation_=0;
    bool retired_=false;
    bool configured_ = false;
    Netplay::BrowserPeerTransport transport_{};
    Netplay::AdonisConnection calibration_{transport_};
    Netplay::SessionChannel channel_{calibration_};
    double phase_debt_ms_=0;
    Netplay::InputGapPacing input_gap_pacing_{};
    NetworkInputGapPacing network_input_gap_tail_{};
    bool network_gap_tail_enabled_=false,network_tail_presentation_enabled_=false,network_gap_pacing_locked_=false;
    std::uint32_t correction_end_=Netplay::INVALID_FRAME;
    std::uint64_t network_now_=0;
    bool network_enabled_=false;
    bool initial_wait_started_=false;
    std::uint64_t initial_wait_since_=0;
    std::string initial_wait_error_{};
    bool playback_=false;
    bool spectator_=false,spectator_retired_=false;
    bool spectator_timing_ready_=true,spectator_timing_sent_=false,spectator_publish_failed_=false;
    std::uint64_t spectator_deadline_=0;
    std::uint32_t spectator_publish_frame_=0,spectator_receive_frame_=0;
    std::deque<Netplay::SpectatorFramePacket> spectator_frames_{};
    Netplay::SpectatorStagePublisher spectator_stage_publisher_{};
    Netplay::SpectatorStagePlayback spectator_stage_playback_{};
    Netplay::SpectatorStageView spectator_stage_view_{};
    struct SpectatorRecordedFrame {std::uint32_t frame=Netplay::INVALID_FRAME,stage=0;};
    std::array<SpectatorRecordedFrame,Netplay::SpectatorStagePublisher::Capacity> spectator_recorded_frames_{};
    std::string spectator_error_{};
};

} // namespace th10::multiplayer
