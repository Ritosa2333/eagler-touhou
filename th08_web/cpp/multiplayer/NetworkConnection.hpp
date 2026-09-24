#pragma once
#include "NetplayRuntime.hpp"
#include "../game/Types.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/SessionChannel.hpp>

namespace th08::multiplayer {
// The shared channel owns handshake, ACK/retransmit and health. The only
// title-specific transport seam rejects inputs this adapter cannot consume.
class NetworkConnection final:private Netplay::PeerTransport {
public:
    explicit NetworkConnection(NetplayRuntime& n):net(n),channel(*this){}
    bool Connect(const char* relay);
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
    std::size_t Buffered()const{return transport.BufferedAmount();}
private:
    NetplayRuntime& net;
    Netplay::BrowserPeerTransport transport;
    Netplay::SessionChannel channel;
    bool enabled=false,invalid_input=false;
    static std::uint64_t Now();
    bool IsOpen()const override{return transport.IsOpen();}
    bool Failed()const override{return invalid_input||transport.Failed();}
    bool SendTo(u8 p,const u8* b,std::size_t n)override{return transport.SendTo(p,b,n);}
    bool SendRepairTo(u8 p,const u8* b,std::size_t n)override{return transport.SendRepairTo(p,b,n);}
    bool SendControl(const u8* b,std::size_t n)override{return transport.SendControl(b,n);}
    bool Poll(std::vector<u8>*)override;
    std::size_t BufferedAmount()const override{return transport.BufferedAmount();}
};
}
