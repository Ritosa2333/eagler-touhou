#include "../th08_web/cpp/multiplayer/NetworkConnection.hpp"
#include <cassert>
#include <cstdio>
using namespace th08::multiplayer;
using namespace Netplay;
double emscripten_get_now(){return 1000;}
static std::vector<std::vector<std::uint8_t>> sent_inputs;
// Transport-independent protocol test. Browser RTC/Relay acceptance is separate.
namespace Netplay {
BrowserPeerTransport::~BrowserPeerTransport()=default;
bool BrowserPeerTransport::Connect(const char*,std::uint8_t,std::uint8_t){return true;}
bool BrowserPeerTransport::ConnectSpectator(const char*,const char*,std::uint8_t){return true;}
void BrowserPeerTransport::Close(){}
bool BrowserPeerTransport::IsOpen()const{return true;}
bool BrowserPeerTransport::Failed()const{return false;}
bool BrowserPeerTransport::SendTo(std::uint8_t,const std::uint8_t* bytes,std::size_t size){sent_inputs.emplace_back(bytes,bytes+size);return true;}
bool BrowserPeerTransport::SendRepairTo(std::uint8_t,const std::uint8_t*,std::size_t){return true;}
bool BrowserPeerTransport::SendControl(const std::uint8_t*,std::size_t){return true;}
bool BrowserPeerTransport::SendSpectator(const std::uint8_t*,std::size_t){return true;}
bool BrowserPeerTransport::HasSpectators()const{return false;}
bool BrowserPeerTransport::Poll(std::vector<std::uint8_t>*){return false;}
std::size_t BrowserPeerTransport::BufferedAmount()const{return 0;}
const std::string& BrowserPeerTransport::LastError()const{return lastError_;}
const char* BrowserPeerTransport::Mode()const{return "fixture";}
}
static void boundary(unsigned count){
    NetplayRuntime peers[3];
    for(unsigned seat=0;seat<count;++seat){
        const std::uint32_t words[]{3,count,seat,1,1234,0x1234,0x5678,1,2,3,4,0,0,1,0,count==3?2u:0u,0};
        SessionSetup setup;assert(decode_session_setup(setup,words,17)&&peers[seat].Reset(setup));
    }
    for(auto phase:{SessionPhase::Hello,SessionPhase::Ready}){
        if(phase==SessionPhase::Ready)for(unsigned i=0;i<count;++i)assert(peers[i].MarkReady());
        for(unsigned i=0;i<count;++i)for(unsigned j=0;j<count;++j)if(i!=j)
            assert(peers[i].ApplySession(peers[j].SessionPacket(phase))==SessionPacketResult::Accepted);
    }
    auto& net=peers[0];NetworkConnection connection(net);
    assert(connection.Connect("ws://fixture/")&&net.SetWorldReady(true));
    for(unsigned frame=0;frame<5;++frame){
        assert(net.CaptureLocal(frame,FrameInput(64))&&connection.Captured(frame));
        const auto decision=net.Prepare(frame);assert(decision.canAdvance&&decision.predictedMask);
        assert(net.MarkSimulated(frame,decision));
    }
    for(unsigned peer=1;peer<count;++peer)for(unsigned frame=0;frame<5;++frame)
        assert(net.SubmitRemote(peer,frame,FrameInput(1))!=RemoteInputResult::InvalidPlayer);
    assert(net.RollbackFrame()==0&&net.BeginCorrection(0));
    assert(net.MarkSimulated(0,net.Prepare(0)));
    // The exact timeline reaches a native stage/resource boundary early.
    assert(net.EndCorrection(true)&&net.NextFrame()==1&&net.SetWorldReady(false));
    assert(connection.Channel().LatestCapture()==4);
    const auto sent=connection.Channel().PacketsSent();
    for(unsigned frame=1;frame<5;++frame){
        assert(net.HasLocal(frame));
        assert(!net.CaptureLocal(frame,FrameInput(128))); // no physical resample
        assert(connection.Captured(frame));
        assert(connection.Channel().LatestCapture()==4);
        assert(net.MarkSimulated(frame,net.Prepare(frame)));
    }
    assert(connection.Channel().PacketsSent()==sent); // old frames are not fresh sends
    assert(net.CaptureLocal(5,FrameInput(128))&&connection.Captured(5));
    assert(connection.Channel().LatestCapture()==5&&connection.Channel().PacketsSent()>sent);
    assert(!connection.Captured(99)); // absent captures remain fatal
    assert(!connection.Captured(2)); // an old capture must not hide an existing error
}
static void delayed_capture(){
    const std::uint32_t words[]{4,2,0,1,1234,0x1234,0x5678,1,2,3,4,0,0,1,0,0,0,4,2};
    SessionSetup setup;assert(decode_session_setup(setup,words,19));
    NetplayRuntime net;assert(net.Reset(setup));
    NetplayRuntime peer;auto other=setup;other.local_player=1;assert(peer.Reset(other));
    assert(net.ApplySession(peer.SessionPacket(SessionPhase::Hello))==SessionPacketResult::Accepted);
    assert(peer.ApplySession(net.SessionPacket(SessionPhase::Hello))==SessionPacketResult::Accepted);
    assert(net.MarkReady()&&peer.MarkReady());
    assert(net.ApplySession(peer.SessionPacket(SessionPhase::Ready))==SessionPacketResult::Accepted);
    NetworkConnection connection(net);assert(connection.Connect("ws://fixture/"));
    sent_inputs.clear();
    assert(net.CaptureLocal(0,FrameInput(64),0)&&connection.Captured(0));
    assert(connection.Channel().LatestCapture()==0); // physical sampling frame
    assert(net.NextCaptureFrame()==1&&sent_inputs.size()==1);
    InputPacket packet;
    assert(DecodeInputPacket(sent_inputs[0].data(),sent_inputs[0].size(),&packet));
    assert(packet.firstInputFrame==0&&packet.latestFrame==4&&packet.inputCount==5);
    assert(packet.inputs[4]==FrameInput(64)); // exactly one F -> F+D conversion
    assert(net.CaptureLocal(0,FrameInput(64),0)&&net.NextCaptureFrame()==1);
    assert(!net.CaptureLocal(0,FrameInput(128),0)); // no changed physical resample
}
int main(){boundary(2);boundary(3);delayed_capture();std::puts("TH08 corrected boundary capture reuse and delayed send: PASS");}
