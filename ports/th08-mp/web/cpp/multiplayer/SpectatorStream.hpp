#pragma once
#include "InputSample.hpp"
#include <eagler/netplay/NetplaySession.hpp>
#include <deque>

namespace th08::multiplayer {
// Admission is owned by the shared relay. This bounded title receiver checks
// ABI/input semantics and contiguous frame zero history before it reaches the
// native simulation. It never guesses missing input or admits mid-run joins.
class SpectatorStream {
public:
    static constexpr std::size_t Capacity=4096;
    bool Append(const Netplay::SpectatorFramePacket& packet,const Netplay::SessionConfig& config){
        if(failed||config.playerCount<2||config.playerCount>3||packet.frame==Netplay::INVALID_FRAME||packet.frame!=next||
           packet.sessionId!=config.sessionId||packet.gameplayAbi!=config.gameplayAbi||
           packet.playerCount!=config.playerCount||frames.size()>=Capacity)return Fail();
        for(std::uint8_t seat=0;seat<packet.playerCount;++seat)
            if(!ValidInputSample(packet.inputs[seat]))return Fail();
        frames.push_back(packet);++next;return true;
    }
    const Netplay::SpectatorFramePacket* Front()const{return frames.empty()?nullptr:&frames.front();}
    void Pop(){if(!frames.empty())frames.pop_front();}
    void Clear(){frames.clear();next=0;failed=false;}
    std::size_t Size()const{return frames.size();}
    bool Failed()const{return failed;}
    std::uint32_t Received()const{return next;}
private:
    std::deque<Netplay::SpectatorFramePacket> frames;
    std::uint32_t next=0;bool failed=false;
    bool Fail(){failed=true;return false;}
};
}
