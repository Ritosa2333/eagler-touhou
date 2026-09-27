#include "NetworkConnection.hpp"
#include <emscripten/emscripten.h>
#include <algorithm>
#include <cstring>
namespace th08::multiplayer {
std::uint64_t NetworkConnection::Now(){return std::uint64_t(emscripten_get_now());}
bool NetworkConnection::Connect(const char* relay){
    if(enabled||net.ReadOnly()||!relay||!*relay||!net.Configured()||net.LastFrame()!=Netplay::INVALID_FRAME)return false;
    if(!transport.Connect(relay,u8(net.Setup().local_player),u8(net.Setup().player_count)))return false;
    if(!channel.BeginSession(net.Config(),Now())){transport.Close();return false;}
    enabled=true;invalid_input=false;return true;
}
bool NetworkConnection::ConnectSpectator(const char* relay,const char* id){
    if(enabled||net.ReadOnly()||!relay||!*relay||!id||!net.Configured()||net.LastFrame()!=Netplay::INVALID_FRAME)return false;
    const auto length=std::strlen(id);if(length<8||length>64)return false;
    for(std::size_t i=0;i<length;++i){const auto c=id[i];
        if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='-'))return false;
    }
    if(!transport.ConnectSpectator(relay,id,u8(net.Setup().player_count)))return false;
    if(!net.BeginSpectator()){transport.Close();return false;}
    enabled=true;invalid_input=false;spectator_finished=false;spectator_error="";
    spectator_frames.Clear();return true;
}
bool NetworkConnection::Pump(bool expects_input){
    if(!enabled)return true;
    if(net.Spectator()){
        std::vector<u8> bytes;
        while(transport.Poll(&bytes)){
            Netplay::SpectatorFramePacket packet;
            if(!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||
               !spectator_frames.Append(packet,net.Config())){
                spectator_error="Invalid, out-of-order or excessive spectator history";return false;
            }
        }
        return !transport.Failed()&&!spectator_frames.Failed();
    }
    if(!channel.Pump(net.gate_,net.core_,Now(),expects_input)||invalid_input)return false;
    PublishConfirmedSpectatorFrames();return true;
}
bool NetworkConnection::ConsumeSpectator(){
    const auto* packet=spectator_frames.Front();if(!packet)return false;
    if(!net.FeedSpectator(*packet)){spectator_error="Spectator native input rejected";return false;}
    spectator_frames.Pop();return true;
}
void NetworkConnection::PublishConfirmedSpectatorFrames(){
    if(!enabled||net.Spectator()||net.Generation()!=0||net.Setup().local_player!=0||
       spectator_publish_failed||!transport.HasSpectators())return;
    const auto last=net.LastFrame(),confirmed=net.ConfirmedThrough();
    if(last==Netplay::INVALID_FRAME||confirmed==Netplay::INVALID_FRAME)return;
    const auto through=std::min(last,confirmed);
    // Losing publication history must not stop or rewrite the players' run.
    if(through>=spectator_publish&&through-spectator_publish>=Netplay::INPUT_HISTORY_SIZE){spectator_publish_failed=true;return;}
    while(spectator_publish<=through){
        Netplay::SpectatorFramePacket packet;
        if(!net.ConfirmedInputs(spectator_publish,packet.inputs))return;
        packet.sessionId=net.Config().sessionId;packet.gameplayAbi=net.Config().gameplayAbi;
        packet.playerCount=net.Config().playerCount;packet.frame=spectator_publish;
        std::vector<u8> bytes;
        if(!Netplay::EncodeSpectatorFramePacket(packet,&bytes)||!transport.SendSpectator(bytes.data(),bytes.size()))return;
        ++spectator_publish;
    }
}
void NetworkConnection::FinishSpectator(){
    if(!net.Spectator())return;
    transport.Close();enabled=false;spectator_finished=true;
    spectator_frames.Clear();
}
bool NetworkConnection::Captured(u32 frame){
    if(net.Spectator())return false;
    if(!enabled)return true;
    // A correction can reach a stage boundary before the predicted timeline
    // did. The following loading ticks reuse already captured input. Pump owns
    // retransmission of the highest capture; never announce the older replayed
    // frame as a new capture or move the channel's capture frontier backwards.
    if(channel.LatestCapture()!=Netplay::INVALID_FRAME&&frame<channel.LatestCapture())
        return net.core_.HasLocalCapture(frame)&&channel.Error()==Netplay::SessionChannel::Failure::None;
    return channel.LocalCaptured(net.core_,frame,Now());
}
bool NetworkConnection::CanRetire()const{return net.CanRetire()&&(!enabled||channel.CanRetire(net.core_,net.LastFrame()));}
bool NetworkConnection::Retire(){return CanRetire()&&(!enabled||channel.Retire(net.core_,net.LastFrame(),Now()))&&net.Retire();}
bool NetworkConnection::BeginGeneration(){return !enabled||channel.BeginSession(net.Config(),Now());}
void NetworkConnection::Close(){transport.Close();channel.Clear();enabled=false;invalid_input=false;spectator_frames.Clear();}
const char* NetworkConnection::Error()const{
    if(*spectator_error)return spectator_error;
    if(invalid_input)return "Unsupported or out-of-window TH08 input";
    if(transport.Failed())return transport.LastError().c_str();
    return channel.ErrorText();
}
bool NetworkConnection::Poll(std::vector<u8>* bytes){
    if(!transport.Poll(bytes))return false;
    Netplay::PacketType type;
    if(Netplay::PeekPacketType(bytes->data(),bytes->size(),&type)&&type==Netplay::PacketType::Input){
        Netplay::InputPacket packet;
        if(Netplay::DecodeInputPacket(bytes->data(),bytes->size(),&packet)&&packet.sessionId==net.Config().sessionId){
            if(packet.inputCount&&(!net.receive_frame(packet.firstInputFrame)||!net.receive_frame(packet.latestFrame)))invalid_input=true;
            for(u32 i=0;i<packet.inputCount;++i)if(!NetplayRuntime::ValidInput(packet.inputs[i]))invalid_input=true;
            if(invalid_input)return false;
        }
    }
    return true;
}
}
