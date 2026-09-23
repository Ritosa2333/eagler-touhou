#include "../th10_web/cpp/multiplayer/NetplayRuntime.hpp"
#include <cassert>
#include <vector>

using namespace th10::multiplayer;
using namespace Netplay;

namespace {
std::vector<std::uint8_t> session(const NetplayRuntime& runtime,bool ready=false){
    std::vector<std::uint8_t> bytes;
    assert(EncodeSessionPacket(ready?runtime.Ready():runtime.Hello(),&bytes));return bytes;
}
void handshake(NetplayRuntime& host,NetplayRuntime& peer){
    for(bool ready:{false,true}){
        const auto a=session(host,ready),b=session(peer,ready);
        assert(host.ApplyWire(b.data(),b.size())==NetplayRuntime::WireResult::Accepted);
        assert(peer.ApplyWire(a.data(),a.size())==NetplayRuntime::WireResult::Accepted);
        if(!ready){host.MarkLocalReady();peer.MarkLocalReady();}
    }
    assert(host.CanStart()&&peer.CanStart());
}
void frame_zero(NetplayRuntime& host,NetplayRuntime& peer,
                std::vector<std::uint8_t>& host_wire){
    assert(host.CaptureLocal(0,FrameInput(1)));
    assert(peer.CaptureLocal(0,FrameInput(2)));
    std::vector<std::uint8_t> peer_wire;
    assert(host.BuildInputWire(1,0,1,0,host_wire));
    assert(peer.BuildInputWire(0,0,1,0,peer_wire));
    assert(host.ApplyWire(peer_wire.data(),peer_wire.size())==NetplayRuntime::WireResult::Accepted);
    assert(peer.ApplyWire(host_wire.data(),host_wire.size())==NetplayRuntime::WireResult::Accepted);
    for(auto* runtime:{&host,&peer}){
        const auto decision=runtime->Prepare(0);
        assert(decision.canAdvance&&decision.predictedMask==0);
        assert(runtime->MarkSimulated(0,decision));
    }
}
}

int main(){
    SessionSetup first{},second{};
    std::uint32_t words[]{2,2,0,1,1234,0x22334455,0x66778899,0,0,1,1,0,0};
    assert(DecodeSessionSetup(first,words,13));words[2]=1;
    assert(DecodeSessionSetup(second,words,13));
    NetplayRuntime host,peer;
    assert(host.Reset(first,first.sessionId)&&peer.Reset(second,second.sessionId));
    assert(!host.RetireRun());handshake(host,peer);
    auto old_hello=session(host);std::vector<std::uint8_t> old_input;
    frame_zero(host,peer,old_input);
    const auto original=host.Config().sessionId;
    for(auto* runtime:{&host,&peer})assert(runtime->RetireRun());
    assert(peer.ApplyWire(old_input.data(),old_input.size())==NetplayRuntime::WireResult::IgnoredSession);
    assert(!host.BeginNextRun(first,65536)&&host.Retired());
    assert(host.BeginNextRun(first,5678)&&peer.BeginNextRun(second,5678));
    assert(host.Generation()==1&&peer.Generation()==1);
    assert(first.sessionId==second.sessionId&&first.sessionId!=original&&first.started);
    assert(!host.CanStart()&&!host.CaptureLocal(0,FrameInput(3)));
    const auto fresh=peer.Config().sessionId;
    assert(peer.ApplyWire(old_hello.data(),old_hello.size())==NetplayRuntime::WireResult::IgnoredSession);
    assert(peer.ApplyWire(old_input.data(),old_input.size())==NetplayRuntime::WireResult::IgnoredSession);
    assert(peer.NextFrame()==0&&peer.ConfirmedThroughAllRemotes()==INVALID_FRAME);
    assert(!peer.InputPresent(0,0));
    auto bad=host.Hello();++bad.gameplayAbi;std::vector<std::uint8_t> wrong;
    assert(EncodeSessionPacket(bad,&wrong));
    assert(peer.ApplyWire(wrong.data(),wrong.size())==NetplayRuntime::WireResult::ContractMismatch);
    handshake(host,peer);std::vector<std::uint8_t> generation_one;
    frame_zero(host,peer,generation_one);
    assert(host.RetireRun()&&peer.RetireRun());
    assert(host.BeginNextRun(first,42)&&peer.BeginNextRun(second,42));
    assert(host.Generation()==2&&host.Config().sessionId!=original&&host.Config().sessionId!=fresh);
    assert(peer.ApplyWire(generation_one.data(),generation_one.size())==NetplayRuntime::WireResult::IgnoredSession);
    assert(peer.ApplyWire(nullptr,0)==NetplayRuntime::WireResult::Malformed);
    handshake(host,peer);
    assert(host.CaptureLocal(0,FrameInput(1)));
    assert(host.MarkSimulated(0,host.Prepare(0)));
    assert(!host.RetireRun());
    assert(host.SubmitRemote(1,0,FrameInput(2))==RemoteInputResult::RollbackRequired);
    assert(!host.RetireRun()); // confirmation alone cannot discard correction
    assert(host.RewindSimulationTo(0));
    assert(host.MarkSimulated(0,host.Prepare(0))&&host.RetireRun());
}
