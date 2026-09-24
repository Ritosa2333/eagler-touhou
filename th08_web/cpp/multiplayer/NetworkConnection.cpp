#include "NetworkConnection.hpp"
#include <emscripten/emscripten.h>
namespace th08::multiplayer {
std::uint64_t NetworkConnection::Now(){return std::uint64_t(emscripten_get_now());}
bool NetworkConnection::Connect(const char* relay){
    if(enabled||!relay||!*relay||!net.Configured()||net.LastFrame()!=Netplay::INVALID_FRAME)return false;
    if(!transport.Connect(relay,u8(net.Setup().local_player),u8(net.Setup().player_count)))return false;
    if(!channel.BeginSession(net.Config(),Now())){transport.Close();return false;}
    enabled=true;invalid_input=false;return true;
}
bool NetworkConnection::Pump(bool expects_input){
    if(!enabled)return true;
    return channel.Pump(net.gate_,net.core_,Now(),expects_input)&&!invalid_input;
}
bool NetworkConnection::Captured(u32 frame){return !enabled||channel.LocalCaptured(net.core_,frame,Now());}
bool NetworkConnection::CanRetire()const{return net.CanRetire()&&(!enabled||channel.CanRetire(net.core_,net.LastFrame()));}
bool NetworkConnection::Retire(){return CanRetire()&&(!enabled||channel.Retire(net.core_,net.LastFrame(),Now()))&&net.Retire();}
bool NetworkConnection::BeginGeneration(){return !enabled||channel.BeginSession(net.Config(),Now());}
void NetworkConnection::Close(){transport.Close();channel.Clear();enabled=false;invalid_input=false;}
const char* NetworkConnection::Error()const{
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
