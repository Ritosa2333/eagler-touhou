// Exercises the actual RoomTh14 lifecycle with an open, asset-free transport.
#include "RoomTh14.hpp"
#include <cassert>
#include <iostream>
namespace Netplay {
BrowserPeerTransport::~BrowserPeerTransport() = default;
bool BrowserPeerTransport::Connect(const char*,std::uint8_t,std::uint8_t,bool){return true;}
bool BrowserPeerTransport::ConnectSpectator(const char*,const char*,std::uint8_t,bool){return false;}
void BrowserPeerTransport::Close(){}
void BrowserPeerTransport::ReleaseRelayPresence(){}
bool BrowserPeerTransport::IsOpen()const{return true;}
bool BrowserPeerTransport::Failed()const{return false;}
bool BrowserPeerTransport::Recovering()const{return false;}
bool BrowserPeerTransport::Disconnected()const{return false;}
bool BrowserPeerTransport::CalibrationSuspended()const{return false;}
bool BrowserPeerTransport::SendTo(std::uint8_t,const std::uint8_t*,std::size_t){return true;}
bool BrowserPeerTransport::SendRepairTo(std::uint8_t,const std::uint8_t*,std::size_t){return true;}
bool BrowserPeerTransport::SendControl(const std::uint8_t*,std::size_t){return true;}
bool BrowserPeerTransport::Poll(std::vector<std::uint8_t>*){return false;}
std::size_t BrowserPeerTransport::BufferedAmount()const{return 0;}
void BrowserPeerTransport::BeginSpectatorSession(std::uint64_t,std::uint32_t){}
}
int main(){
 auto* room=new th11::RoomTh14;
 room->setup.id=7;room->setup.seats=2;room->setup.local=1;
 room->setup.seed=417;room->setup.difficulty=1;room->setup.prediction=12;room->setup.build[0]=1;
 assert(room->begin("ws://fixture.invalid/",nullptr,10));
 // Resource loading can exceed the handshake window before the first pump.
 assert(room->poll(60000));
 assert(room->channel.Active());assert(room->channel.PacketsSent()>0);
 assert(room->poll(60001));assert(room->poll(104999));
 // Starting the timer later retains the same 45-second timeout thereafter.
 assert(!room->poll(105000));
 assert(room->channel.Error()==Netplay::SessionChannel::Failure::HandshakeTimeout);
 delete room;
 std::cout<<"TH14 delayed room handshake and original timeout passed\n";
}
