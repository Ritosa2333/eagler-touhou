#pragma once
#include "ChargeHistory.hpp"
#include <eagler/netplay/PeerTransport.hpp>
#include <eagler/netplay/InputRepairBudget.hpp>

namespace th09::multiplayer {
// Charge results are produced after simulation, independently of keyboard
// capture. Transport clocks and ACKs never rewind with the game world.
class ChargeChannel {
    static constexpr std::uint64_t RepeatMs=32,RepairMs=Netplay::InputRepairBudget::StalledMs;
    static constexpr std::size_t BufferedLimit=256*1024;
    u32 peer=0,acknowledged=0,received=0;
    std::uint64_t sendAt=0,repairAt=0,ackRepairAt=0;
    bool outstanding=false,ackPending=false,peerNeedsAck=false;
    void observe(const ChargeHistory& history,std::uint64_t now){
        const bool pending=history.AcknowledgedCount()<history.LocalCount();
        if(history.AcknowledgedCount()!=acknowledged||(pending&&!outstanding))repairAt=now+RepairMs;
        if(history.RemoteCount()!=received)ackRepairAt=now+RepairMs;
        acknowledged=history.AcknowledgedCount();received=history.RemoteCount();outstanding=pending;
    }
    bool send(Netplay::PeerTransport& transport,const ChargeHistory& history,std::uint64_t now){
        sendAt=now+RepeatMs;
        if(!transport.IsOpen()||transport.BufferedAmount()>BufferedLimit)return !transport.Failed();
        std::array<u8,ChargeHistory::MaxBatchBytes> packet{};std::size_t size=0;
        if(!history.Batch(packet,size))return false;
        if(transport.SendTo(u8(peer),packet.data(),size))ackPending=false;
        return !transport.Failed();
    }
public:
    void Begin(u32 side,std::uint64_t now){
        *this=ChargeChannel{};peer=1-side;sendAt=now;repairAt=ackRepairAt=now+RepairMs;
    }
    bool Published(Netplay::PeerTransport& transport,const ChargeHistory& history,std::uint64_t now){
        observe(history,now);return send(transport,history,now);
    }
    bool Pump(Netplay::PeerTransport& transport,const ChargeHistory& history,std::uint64_t now){
        if(!history.Enabled())return true;
        observe(history,now);
        if(now>=sendAt&&(outstanding||ackPending)&&!send(transport,history,now))return false;
        const bool repair=outstanding&&now>=repairAt,ackRepair=peerNeedsAck&&now>=ackRepairAt;
        if(repair||ackRepair){
            // Reuse the shared repair lane's queue bound. A rejected repair is
            // retried later; it cannot stop fresh unordered copies or ACKs.
            if(transport.IsOpen()){
                std::array<u8,ChargeHistory::MaxBatchBytes> packet{};std::size_t size=0;
                if(!history.Batch(packet,size))return false;
                transport.SendRepairTo(u8(peer),packet.data(),size);
            }
            if(repair)repairAt=now+RepairMs;
            if(ackRepair)ackRepairAt=now+RepairMs;
        }
        return !transport.Failed();
    }
    bool Receive(ChargeHistory& history,const u8* bytes,std::size_t size,u32 next,bool& handled){
        handled=true;
        if(ChargeHistory::IsBatch(bytes,size)){
            bool accepted=false,data=false;
            if(!history.ReceiveBatch(bytes,size,next,accepted,data))return false;
            if(accepted){peerNeedsAck=data;ackPending=ackPending||data;}
        }else if(ChargeHistory::IsSample(bytes,size)){
            std::array<u8,ChargeHistory::AckBytes> ack{};bool accepted=false;
            if(!history.Receive(bytes,size,next,ack,accepted))return false;
            if(accepted){peerNeedsAck=true;ackPending=true;}
        }else if(ChargeHistory::IsAck(bytes,size)){
            if(!history.ReceiveAck(bytes,size))return false;
        }else handled=false;
        return true;
    }
};
}
