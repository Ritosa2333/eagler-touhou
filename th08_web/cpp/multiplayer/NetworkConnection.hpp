#pragma once
#include "NetplayRuntime.hpp"
#include "InputGapPacing.hpp"
#include "../game/Types.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/AdonisConnection.hpp>
#include <eagler/netplay/AdonisSpectatorTiming.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>
#include <vector>

namespace th08::multiplayer {
// The shared channel owns handshake, ACK/retransmit and health. The only
// title-specific transport seam rejects inputs this adapter cannot consume.
class NetworkConnection final:private Netplay::PeerTransport {
public:
    explicit NetworkConnection(NetplayRuntime& n):net(n),channel(*this){}
    bool Connect(const char* relay);
    bool ConnectSpectator(const char* relay,const char* id);
    bool RecordSpectatorStage(u32 frame,u32 stage,u32 schema,const std::vector<u8>& initializer,
        const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs,u8 boss_slots);
    void DisableSpectatorPublishing(){spectator_publish_failed=true;transport.StopSpectators();}
    Netplay::SpectatorStagePlayback& StagePlayback(){return spectator_playback;}
    const Netplay::SpectatorStagePlayback& StagePlayback()const{return spectator_playback;}
    const char* SpectatorState();
    bool SpectatorControl(const char* run,const char* action,u32 frame){return spectator_view.Control(spectator_playback,run,action,frame);}
    void PublishConfirmedSpectatorFrames();
    void FinishSpectator();
    bool SpectatorFinished()const{return spectator_finished;}
    std::size_t SpectatorBacklog()const{return spectator_playback.History().Received()-spectator_playback.Position();}
    u32 SpectatorReceived()const{return spectator_playback.History().Received();}
    u32 SpectatorPublished()const{return spectator_publisher.Published();}
    bool SpectatorPublishFailed()const{return spectator_publish_failed;}
    // A callback may pump once before choosing its logical work. Step consumes
    // that pump before capture; it must not pump again and send the older tail.
    // Flush even when no tick is due, before every callback return or yield.
    bool Pump(bool expects_input,bool capture_follows=false);
    bool PumpForCapture(bool expects_input);
    bool FlushDeferredInputs();
    bool Captured(u32 frame);
    bool CanRetire()const;
    bool Retire();
    bool BeginGeneration();
    void Close();
    bool Enabled()const{return enabled;}
    const char* Error()const;
    const char* Mode()const{return transport.Mode();}
    const Netplay::SessionChannel& Channel()const{return channel;}
    double IntervalScale()const;
    const u32* CalibrationStatus(){return calibration.Status();}
    bool Calibrating()const{return calibration.Waiting();}
    // Wall-clock pacing only; consume only when no already-due tick is pending.
    double PacedElapsedSeconds(double);
    std::size_t Buffered()const{return transport.BufferedAmount();}
    std::size_t BufferedInput()const{return transport.BufferedInputAmount();}
    std::size_t BufferedControl()const{return transport.BufferedControlAmount();}
private:
    NetplayRuntime& net;
    Netplay::BrowserPeerTransport transport;
    Netplay::AdonisConnection calibration{transport};
    Netplay::SessionChannel channel;
    Netplay::SessionConfig retired_config{};
    Netplay::SpectatorStagePlayback spectator_playback;
    Netplay::SpectatorStagePublisher spectator_publisher;
    Netplay::SpectatorStageView spectator_view;
    bool spectator_finished=false,spectator_publish_failed=false,spectator_publisher_started=false;
    bool spectator_timing_sent=false;
    std::uint64_t spectator_deadline=0;
    const char* spectator_error="";
    bool enabled=false,invalid_input=false;
    bool capture_pump_pending=false;
    double phase_debt_ms=0;
    InputGapPacing input_gap_pacing;
    static std::uint64_t Now();
    bool IsOpen()const override{return transport.IsOpen();}
    bool Recovering()const override{return transport.Recovering();}
    bool Disconnected()const override{return transport.Disconnected();}
    bool CalibrationSuspended()const override{return transport.CalibrationSuspended();}
    bool Failed()const override{return invalid_input||transport.Failed();}
    bool SendTo(u8 p,const u8* b,std::size_t n)override{return transport.SendTo(p,b,n);}
    bool SendRepairTo(u8 p,const u8* b,std::size_t n)override{return transport.SendRepairTo(p,b,n);}
    bool SendControl(const u8* b,std::size_t n)override{return transport.SendControl(b,n);}
    bool Poll(std::vector<u8>*)override;
    std::size_t BufferedAmount()const override{return transport.BufferedAmount();}
};
}
