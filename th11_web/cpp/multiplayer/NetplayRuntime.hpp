#pragma once
#include "SessionSetup.hpp"
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/AdonisConnection.hpp>
#include <eagler/netplay/AdonisSpectatorTiming.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <deque>
#include <string>
namespace th11::multiplayer {
// Title adapter over shared network authorities. The common history object is
// used with prediction disabled. No title snapshot, undo or resimulation owner
// exists in the TH11 multiplayer source set.
class NetplayRuntime {
public:
    bool Reset(const SessionSetup&);
    void Clear();
    bool Connect(const char*);
    bool ConnectSpectator(const char*,const char*);
    bool BeginPlayback(const SessionSetup&);
    bool Pump(bool world_ready);
    bool Capture(std::uint32_t,const Netplay::FrameInput&);
    bool NeedsCapture()const;
    bool InitialInputsReady()const;
    bool CanStart()const;
    bool FeedPlayback(std::uint32_t,const Netplay::FrameInput*,unsigned);
    bool FeedSpectator(std::uint32_t);
    Netplay::FrameDecision Prepare(std::uint32_t)const;
    bool MarkSimulated(std::uint32_t,const Netplay::FrameDecision&);
    bool ConfirmedInputs(std::uint32_t,std::array<Netplay::FrameInput,3>&)const;
    bool Finish();
    bool BeginNextRun(std::uint32_t seed);
    double PacedElapsed(double);
    const SessionSetup& Setup()const{return setup_;}
    bool Configured()const{return configured_;}
    bool Playback()const{return playback_;}
    bool Spectator()const{return spectator_;}
    bool ReadOnly()const{return playback_||spectator_;}
    bool NetworkEnabled()const{return network_;}
    bool Retired()const{return retired_;}
    std::uint32_t NextFrame()const;
    std::uint32_t Confirmed()const{return core_.ConfirmedThroughAllRemotes();}
    unsigned Generation()const{return generation_;}
    unsigned Backlog()const{return unsigned(spectator_frames_.size());}
    const char* Error()const;
    const std::uint32_t* Status();
    const std::uint32_t* CalibrationStatus(){return calibration_.Status();}
    Netplay::BrowserPeerTransport& Transport(){return transport_;}
    // Transport-free deterministic test lane; still uses the real gate/core.
    Netplay::SessionPacket Hello()const{return gate_.BuildPacket(Netplay::SessionPhase::Hello);}
    Netplay::SessionPacket Ready()const{return gate_.BuildPacket(Netplay::SessionPhase::Ready);}
    Netplay::SessionPacketResult ApplySession(const Netplay::SessionPacket& p){return gate_.Apply(p);}
    bool CanSendReady()const{return gate_.CanSendReady();}
    void MarkLocalReady(){gate_.MarkLocalReady();}
    Netplay::RemoteInputResult SubmitRemote(unsigned s,std::uint32_t f,const Netplay::FrameInput& i){return core_.SubmitRemoteInput(std::uint8_t(s),f,i);}
private:
    bool Configure(const SessionSetup&,unsigned delay);
    bool Feed(std::uint32_t,const Netplay::FrameInput*,unsigned);
    bool DrainSpectator();
    void Publish();
    void FreezePublicationTail();
    void DrainPublicationTail();
    bool Fail(const char*);
    SessionSetup setup_;
    Netplay::SessionGate gate_;
    Netplay::RollbackCore core_;
    Netplay::BrowserPeerTransport transport_;
    Netplay::AdonisConnection calibration_{transport_};
    Netplay::SessionChannel channel_{calibration_};
    std::uint64_t base_session_=0,now_ms_=0,initial_wait_=0,spectator_deadline_=0,publish_deadline_=0;
    bool configured_=false,network_=false,playback_=false,spectator_=false,retired_=false;
    bool timing_ready_=false,timing_sent_=false,publish_failed_=false;
    std::uint32_t generation_=0,publish_frame_=0,receive_frame_=0;
    double phase_debt_ms_=0;
    std::string error_;
    std::deque<Netplay::SpectatorFramePacket> spectator_frames_;
    std::deque<std::vector<std::uint8_t>> publish_tail_;
    std::array<std::uint32_t,24> status_{};
};
}
