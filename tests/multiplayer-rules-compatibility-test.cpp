#include "../th10_web/cpp/multiplayer/NetplayRuntime.hpp"
#include "multiplayer-contract-fixture.hpp"
#include <cassert>
#include <deque>
#include <map>
#include <cstring>

// In-memory transport boundary only. The production title runtime, common
// codecs/session/core, spectator admission and input ownership execute intact.
// This is a deterministic contract test, not a WebRTC/browser transport test.
namespace {
using Bytes=std::vector<std::uint8_t>;
std::map<const Netplay::BrowserPeerTransport*,std::deque<Bytes>> queues;
const Netplay::BrowserPeerTransport* receiver=nullptr;
}
namespace Netplay {
BrowserPeerTransport::~BrowserPeerTransport(){Close();}
bool BrowserPeerTransport::Connect(const char*,std::uint8_t,std::uint8_t){return false;}
bool BrowserPeerTransport::ConnectSpectator(const char*,const char*,std::uint8_t){
    queues[this]={};receiver=this;return true;
}
void BrowserPeerTransport::Close(){queues.erase(this);if(receiver==this)receiver=nullptr;}
bool BrowserPeerTransport::IsOpen()const{return queues.count(this)!=0;}
bool BrowserPeerTransport::Failed()const{return false;}
bool BrowserPeerTransport::Send(const std::uint8_t*,std::size_t){return false;}
bool BrowserPeerTransport::SendTo(std::uint8_t,const std::uint8_t*,std::size_t){return false;}
bool BrowserPeerTransport::SendRepairTo(std::uint8_t,const std::uint8_t*,std::size_t){return false;}
bool BrowserPeerTransport::SendControl(const std::uint8_t*,std::size_t){return false;}
bool BrowserPeerTransport::SendSpectator(const std::uint8_t*,std::size_t){return false;}
bool BrowserPeerTransport::HasSpectators()const{return false;}
bool BrowserPeerTransport::Poll(Bytes* out){
    auto& pending=queues[this];if(pending.empty())return false;
    *out=std::move(pending.front());pending.pop_front();return true;
}
std::size_t BrowserPeerTransport::BufferedAmount()const{return 0;}
std::size_t BrowserPeerTransport::BufferedInputAmount()const{return 0;}
std::size_t BrowserPeerTransport::BufferedControlAmount()const{return 0;}
const std::string& BrowserPeerTransport::LastError()const{return lastError_;}
const char* BrowserPeerTransport::Mode()const{return "test-memory";}
}
int main(){
    using namespace th10::multiplayer;using namespace Netplay;
    for(const auto seats:{2u,3u}){
        SessionSetup setup{};setup.configured=true;setup.playerCount=seats;
        setup.seed=1234;setup.difficulty=2;setup.sessionId=42;
        setup.loadouts[0]={0,0};setup.loadouts[1]={1,2};
        if(seats==3)setup.loadouts[2]={0,1};
        for(const auto oldVersion:{0x10000004u,0x10000005u,0x10000006u}){
            NetplayRuntime host,peer;assert(host.Reset(setup,42));
            auto remote=setup;remote.localPlayer=1;assert(peer.Reset(remote,42));
            auto hello=peer.Hello();hello.gameplayAbi=historical_contract(setup,oldVersion);
            assert(host.ApplySession(hello)==SessionPacketResult::ContractMismatch);
            assert(!host.CanSendReady()&&!host.CanStart());
            Bytes wire;assert(EncodeSessionPacket(hello,&wire));
            assert(host.ApplyWire(wire.data(),wire.size())==NetplayRuntime::WireResult::ContractMismatch);
            assert(!host.InputPresent(1,0));

            NetplayRuntime spectator;assert(spectator.Reset(setup,42));
            assert(spectator.ConnectSpectator("test-memory://stream","reader"));
            SpectatorFramePacket packet{};packet.sessionId=42;packet.playerCount=std::uint8_t(seats);
            packet.gameplayAbi=historical_contract(setup,oldVersion);packet.frame=0;
            assert(EncodeSpectatorFramePacket(packet,&wire));queues[receiver].push_back(wire);
            assert(!spectator.PumpNetwork(true));
            assert(std::strcmp(spectator.NetworkError(),"Invalid spectator stream")==0);
            assert(spectator.SpectatorBacklog()==0&&!spectator.FeedSpectator(0));
            assert(spectator.NextFrame()==0&&!spectator.InputPresent(0,0));
        }
        NetplayRuntime spectator;assert(spectator.Reset(setup,42));
        assert(spectator.ConnectSpectator("test-memory://stream","reader"));
        assert(spectator.Spectator()&&!spectator.CanSendReady());
        assert(!spectator.CaptureLocal(0,FrameInput(1)));
        assert(spectator.SubmitRemote(1,0,FrameInput(1))==RemoteInputResult::InvalidPlayer);
        SpectatorFramePacket packet{};packet.sessionId=42;packet.playerCount=std::uint8_t(seats);
        packet.gameplayAbi=GameplayContract(setup);packet.frame=0;
        for(unsigned seat=0;seat<seats;++seat)packet.inputs[seat]=FrameInput(std::uint16_t(1u<<seat));
        Bytes wire;assert(EncodeSpectatorFramePacket(packet,&wire));queues[receiver].push_back(wire);
        assert(spectator.PumpNetwork(true)&&spectator.SpectatorBacklog()==1);
        assert(spectator.FeedSpectator(0));const auto decision=spectator.Prepare(0);
        assert(decision.canAdvance&&decision.predictedMask==0);
        for(unsigned seat=0;seat<seats;++seat)assert(decision.inputs[seat]==packet.inputs[seat]);
        assert(spectator.MarkSimulated(0,decision)&&spectator.NextFrame()==1);
        assert(!spectator.FeedSpectator(0)&&spectator.SpectatorBacklog()==0);
    }
}
