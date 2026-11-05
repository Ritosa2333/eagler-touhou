#pragma once
#include "SessionSetup.hpp"
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/ConfirmedInputClock.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/AdonisConnection.hpp>
#include <eagler/netplay/AdonisSpectatorTiming.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorPublicationTail.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>
#include <deque>
#include <string>
namespace th11::multiplayer {
// Title-owned admission/frontiers over the unchanged shared wire/core. World
// restore and checkpoint retirement belong to the driver, never packet delivery.
class NetplayRuntime {
public:
    bool Reset(const SessionSetup&);
    void Clear();
    bool Connect(const char*);
    bool ConnectSpectator(const char*,const char*);
    bool BeginPlayback(const SessionSetup&);
    bool Pump(bool world_ready,bool capture_follows=false);
    bool FlushDeferredInputs();
    bool Capture(std::uint32_t,const Netplay::FrameInput&);
    bool NeedsCapture()const;
    void SetCapturePhase(bool exactUi,std::uint64_t nowUs){
        if(exactUi&&!capture_ui_)capture_clock_.BeginPhase(nowUs);
        capture_ui_=exactUi;capture_now_us_=nowUs;
    }
    std::uint32_t CaptureFrame()const{return next_capture_;}
    bool InitialInputsReady()const;
    bool CanStart()const;
    bool FeedPlayback(std::uint32_t,const Netplay::FrameInput*,unsigned);
    bool FeedSpectator(std::uint32_t);
    bool StagePlaybackReady()const{return spectator_&&spectator_stage_playback_.Ready();}
    bool SpectatorPublisherOwner()const{return network_&&!spectator_&&!playback_&&setup_.local_player==0;}
    Netplay::SpectatorStagePlayback& StagePlayback(){return spectator_stage_playback_;}
    const Netplay::SpectatorStagePlayback& StagePlayback()const{return spectator_stage_playback_;}
    const char* SpectatorPlaybackState(){return spectator_stage_view_.State(spectator_stage_playback_);}
    // Read-only timeout diagnostics: publisher frontier, transport admission,
    // receiver frontier, and native playback/restore state are distinct.
    std::array<std::uint32_t,21> SpectatorDiagnostics()const{
        return {{1u,std::uint32_t(SpectatorPublisherOwner()),spectator_stage_publisher_.Published(),publish_frame_,
            std::uint32_t(publish_failed_),std::uint32_t(transport_.HasSpectators()),std::uint32_t(transport_.SpectatorState()+1),
            std::uint32_t(transport_.Failed()),std::uint32_t(transport_.Disconnected()),std::uint32_t(timing_sent_),
            std::uint32_t(timing_ready_),receive_frame_,std::uint32_t(spectator_frames_.size()),
            std::uint32_t(spectator_stage_playback_.Ready()),spectator_stage_playback_.History().Received(),
            std::uint32_t(spectator_stage_playback_.History().Segments().size()),spectator_stage_playback_.Position(),
            spectator_stage_playback_.Target(),std::uint32_t(spectator_stage_playback_.Seeking()),
            std::uint32_t(spectator_stage_playback_.Failed()),std::uint32_t(spectator_publisher_started_)}};
    }
    bool SpectatorPlaybackControl(const char* run,const char* action,std::uint32_t frame){
        return spectator_stage_view_.Control(spectator_stage_playback_,run,action,frame);
    }
    bool SpectatorCheckpointRequired(std::uint32_t frame,std::uint32_t stage)const;
    bool SpectatorRecordedStage(std::uint32_t frame,std::uint32_t& stage)const{
        if(frame==Netplay::INVALID_FRAME)return false;
        const auto& record=spectator_recorded_frames_[frame%spectator_recorded_frames_.size()];
        if(record.frame!=frame)return false;stage=record.stage;return true;
    }
    bool RecordSpectatorFrame(std::uint32_t frame,std::uint32_t stage,
        const std::vector<std::uint8_t>& initializer,
        const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs);
    void PublishConfirmedSpectatorHistoryAfterReconcile(){
        if(!Correcting()&&!core_.HasRollbackRequest())PublishConfirmedSpectatorFrames();
    }
    Netplay::FrameDecision Prepare(std::uint32_t)const;
    bool MarkSimulated(std::uint32_t,const Netplay::FrameDecision&);
    bool ConfirmedInputs(std::uint32_t,std::array<Netplay::FrameInput,3>&)const;
    bool AllowsRollback()const{return configured_&&!ReadOnly()&&setup_.mode!=1;}
    Netplay::AdonisMode Mode()const{return Netplay::AdonisMode(setup_.mode);}
    unsigned PredictionLimit()const{return AllowsRollback()?setup_.prediction_limit:0;}
    bool WorldReady()const{return world_ready_;}
    bool SetWorldReady(bool);
    bool Correcting()const{return correction_end_!=Netplay::INVALID_FRAME;}
    std::uint32_t CorrectionEnd()const{return correction_end_;}
    std::uint32_t RollbackFrame()const{return core_.RollbackFrame();}
    std::uint32_t LastFrame()const{return core_.LastSimulatedFrame();}
    std::uint32_t PredictionFrontier()const{return Correcting()&&correction_end_>next_?correction_end_:next_;}
    std::uint32_t RemoteInputFrontier()const{const auto f=Confirmed();return f==Netplay::INVALID_FRAME?0:f+1;}
    std::uint32_t MissingRemoteInputs()const{const auto f=PredictionFrontier(),a=RemoteInputFrontier();return f>a?f-a:0;}
    // Call correction methods only after the corresponding title pre-tick
    // state has been restored. Rewinding preserves every captured/actual input.
    bool BeginCorrection(std::uint32_t first);
    bool RestartCorrection(std::uint32_t first);
    bool EndCorrection(bool lifecycleBoundary=false);
    bool InputsReconciledBeforeNext()const;
    // The driver grants only a reconciled prefix whose journal records and
    // typed lifetime leases have retired. This watermark never rewinds.
    bool SetCommitLimit(std::uint32_t through);
    // An explicitly sealed, fully actual prefix can retire while the old
    // display frontier is still being reconstructed. This never grants the
    // predicted suffix and does not complete correction or publish a frame.
    bool CommitReconciledPrefix(std::uint32_t through);
    bool CommittedPrefixInputs(std::uint32_t,std::array<Netplay::FrameInput,3>&)const;
    bool CanReplayConfirmed()const;
    std::uint32_t CommittedThrough()const{return commit_limit_;}
    bool Finish(bool restarting = false, bool local_results = false);
    bool EnterSpectatorResults(){const auto& p=spectator_stage_playback_;if(!spectator_||!p.History().Ended()||p.Paused()||p.Seeking()||p.Position()<p.History().Received())return false;retired_=local_results_=true;return true;}
    bool BeginNextRun(std::uint32_t seed);
    double PacedElapsed(double);
    const SessionSetup& Setup()const{return setup_;}
    bool Configured()const{return configured_;}
    bool Playback()const{return playback_;}
    bool Spectator()const{return spectator_;}
    bool ReadOnly()const{return playback_||spectator_;}
    bool NetworkEnabled()const{return network_;}
    bool Retired()const{return retired_;}
    std::uint32_t NextFrame()const{return StagePlaybackReady()?spectator_stage_playback_.Position():next_;}
    std::uint32_t Confirmed()const{return core_.ConfirmedThroughAllRemotes();}
    unsigned Generation()const{return generation_;}
    unsigned Backlog()const{return unsigned(spectator_frames_.size());}
    const char* Error()const;
    const std::uint32_t* Status();
    const double* PacingStatistics()const{return pacing_statistics_.data();}
    const std::uint32_t* CalibrationStatus(){return calibration_.Status();}
    Netplay::BrowserPeerTransport& Transport(){return transport_;}
    // Transport-free deterministic test lane; still uses the real gate/core.
    Netplay::SessionPacket Hello()const{return gate_.BuildPacket(Netplay::SessionPhase::Hello);}
    Netplay::SessionPacket Ready()const{return gate_.BuildPacket(Netplay::SessionPhase::Ready);}
    Netplay::SessionPacketResult ApplySession(const Netplay::SessionPacket& p){return gate_.Apply(p);}
    bool CanSendReady()const{return gate_.CanSendReady();}
    void MarkLocalReady(){gate_.MarkLocalReady();}
    Netplay::RemoteInputResult SubmitRemote(unsigned,std::uint32_t,const Netplay::FrameInput&);
private:
    bool Configure(const SessionSetup&,unsigned delay,bool read_only=false);
    void UpdatePacing(bool expects_input);
    bool Feed(std::uint32_t,const Netplay::FrameInput*,unsigned);
    bool DrainSpectator();
    void Publish();
    void PublishConfirmedSpectatorFrames();
    bool ReceiveSpectatorWire(const std::uint8_t*,std::size_t);
    void FreezePublicationTail(bool restarting);
    void DrainPublicationTail();
    bool Fail(const char*);
    SessionSetup setup_;
    Netplay::SessionGate gate_;
    Netplay::RollbackCore core_;
    Netplay::ConfirmedInputClock capture_clock_;
    std::uint64_t capture_now_us_=0;
    bool capture_ui_=false;
    Netplay::BrowserPeerTransport transport_;
    Netplay::AdonisConnection calibration_{transport_};
    Netplay::SessionChannel channel_{calibration_};
    std::uint64_t base_session_=0,now_ms_=0,initial_wait_=0,spectator_deadline_=0;
    bool configured_=false,network_=false,playback_=false,spectator_=false,retired_=false;
    bool timing_ready_=false,timing_sent_=false,publish_failed_=false,spectator_publisher_started_=false;
    bool world_ready_=false,local_results_=false;
    std::uint32_t generation_=0,publish_frame_=0,receive_frame_=0;
    std::uint32_t next_=0,next_capture_=0,correction_end_=Netplay::INVALID_FRAME,commit_limit_=Netplay::INVALID_FRAME;
    double phase_debt_ms_=0;
    double network_interval_scale_=1;
    // Read-only wall-clock accounting, never recorded in Replay or rollback.
    // version, words, input seconds, credited seconds, legacy-rule seconds,
    // current legacy shortage scale, current peer-lead scale, sample count.
    std::array<double,8> pacing_statistics_{{1,8,0,0,0,1,1,0}};
    std::uint64_t network_pace_ms_=0;
    std::string error_;
    std::deque<Netplay::SpectatorFramePacket> spectator_frames_;
    struct SpectatorRecordedFrame {std::uint32_t frame=Netplay::INVALID_FRAME,stage=0;};
    std::array<SpectatorRecordedFrame,Netplay::SpectatorStagePublisher::Capacity> spectator_recorded_frames_{};
    Netplay::SpectatorStagePublisher spectator_stage_publisher_{};
    Netplay::SpectatorStagePlayback spectator_stage_playback_{};
    Netplay::SpectatorStageView spectator_stage_view_{};
    Netplay::SpectatorPublicationTail publish_tail_;
    std::array<std::uint32_t,24> status_{};
};
}
