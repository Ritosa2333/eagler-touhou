#pragma once
#include "NetplayRuntime.hpp"
#include "SpectatorStream.hpp"
#include "../game/Types.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/AdonisConnection.hpp>
#include <eagler/netplay/AdonisSpectatorTiming.hpp>

namespace th08::multiplayer {
// The shared channel owns handshake, ACK/retransmit and health. The only
// title-specific transport seam rejects inputs this adapter cannot consume.
class NetworkConnection final:private Netplay::PeerTransport {
public:
    explicit NetworkConnection(NetplayRuntime& n):net(n),channel(*this){}
    bool Connect(const char* relay);
    bool ConnectSpectator(const char* relay,const char* id);
    bool ConsumeSpectator();
    void PublishConfirmedSpectatorFrames();
    void FinishSpectator();
    bool SpectatorFinished()const{return spectator_finished;}
    std::size_t SpectatorBacklog()const{return spectator_frames.Size();}
    u32 SpectatorReceived()const{return spectator_frames.Received();}
    u32 SpectatorPublished()const{return spectator_publish;}
    bool SpectatorPublishFailed()const{return spectator_publish_failed;}
    bool Pump(bool expects_input);
    bool Captured(u32 frame);
    bool CanRetire()const;
    bool Retire();
    bool BeginGeneration();
    void Close();
    bool Enabled()const{return enabled;}
    const char* Error()const;
    const char* Mode()const{return transport.Mode();}
    const Netplay::SessionChannel& Channel()const{return channel;}
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
    SpectatorStream spectator_frames;
    u32 spectator_publish=0;
    bool spectator_finished=false,spectator_publish_failed=false;
    bool spectator_timing_sent=false;
    std::uint64_t spectator_deadline=0;
    const char* spectator_error="";
    bool enabled=false,invalid_input=false;
    double phase_debt_ms=0;
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
