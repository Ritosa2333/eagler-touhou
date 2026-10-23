#pragma once
#include "../game/ChargeSample.hpp"
#include <eagler/netplay/NetplayProtocol.hpp>
#include <algorithm>
#include <cstring>
namespace th09::multiplayer {
// Title-owned immutable input records. The shared input protocol and other
// games retain their existing wire format and confirmation policy.
class ChargeHistory {
    struct Record {u32 frame=Netplay::INVALID_FRAME;bool actual=false,applied=false;ChargeSample value,used;};
    static constexpr u32 Capacity=512;
    std::array<std::array<Record,Capacity>,2> records;
    std::uint64_t session=0;u32 side=0,frontier[2]{},acknowledged=0,rewind=Netplay::INVALID_FRAME;bool enabled=false;
    static u32 word(const u8* bytes){return u32(bytes[0])|(u32(bytes[1])<<8)|(u32(bytes[2])<<16)|(u32(bytes[3])<<24);}
    static void word(u8* bytes,u32 value){for(u32 n=0;n<4;++n)bytes[n]=u8(value>>(8*n));}
    bool identity(const u8* bytes)const{return (std::uint64_t(word(bytes+8))<<32|word(bytes+4))==session;}
    void identity(u8* bytes)const{word(bytes+4,u32(session));word(bytes+8,u32(session>>32));}
    Record& slot(u32 player,u32 frame){auto& record=records[player][frame%Capacity];if(record.frame!=frame){record=Record{};record.frame=frame;}return record;}
    void advance(u32 player){while(Get(player,frontier[player]))++frontier[player];}
public:
    static constexpr u32 SampleBytes=40,PacketBytes=20+SampleBytes,AckBytes=20;
    static constexpr u32 BatchHeaderBytes=24,BatchRecordBytes=4+SampleBytes,MaxBatchSamples=8;
    static constexpr u32 MaxBatchBytes=BatchHeaderBytes+MaxBatchSamples*BatchRecordBytes;
    void begin(std::uint64_t id,u32 player,bool on){*this=ChargeHistory{};session=id;side=player;enabled=on;}
    bool Enabled()const{return enabled;}
    u32 RollbackFrame()const{return rewind;}
    void Restored(u32 frame){
        rewind=Netplay::INVALID_FRAME;
        // Future predictions no longer describe the restored world. A late
        // record for that tail is input for resimulation, not another rewind.
        for(auto& player:records)for(auto& record:player)if(record.frame>=frame)record.applied=false;
    }
    u32 ConfirmedCount()const{return std::min(frontier[0],frontier[1]);}
    bool CanAdvance(u32 frame,u32 history)const{return !enabled||frame<ConfirmedCount()+history;}
    bool CanRetire(u32 next)const{return !enabled||(ConfirmedCount()>=next&&acknowledged>=next);}
    u32 LocalCount()const{return frontier[side];}
    u32 RemoteCount()const{return frontier[1-side];}
    u32 AcknowledgedCount()const{return acknowledged;}
    const ChargeSample* Get(u32 player,u32 frame)const{
        if(!enabled||player>1)return nullptr;const auto& record=records[player][frame%Capacity];
        return record.frame==frame&&record.actual?&record.value:nullptr;
    }
    static bool IsSample(const u8* bytes,std::size_t size){return size>=4&&!std::memcmp(bytes,"T9CP",4);}
    static bool IsAck(const u8* bytes,std::size_t size){return size>=4&&!std::memcmp(bytes,"T9CA",4);}
    static bool IsBatch(const u8* bytes,std::size_t size){return size>=4&&!std::memcmp(bytes,"T9CB",4);}
    static void EncodeSample(const ChargeSample& sample,u8* bytes){
        charge_record::encode(sample,bytes);
    }
    static bool DecodeSample(const u8* bytes,ChargeSample& sample){
        return charge_record::decode(bytes,sample);
    }
    bool Publish(u32 frame,const ChargeSample& sample,std::array<u8,PacketBytes>& packet){
        if(!enabled||!sample.valid()||Get(side,frame))return false;
        const auto& previous=records[side][frame%Capacity];
        if(previous.actual&&previous.frame!=frame&&previous.frame>=acknowledged)return false;
        auto& record=slot(side,frame);record.actual=true;record.value=sample;advance(side);
        std::memcpy(packet.data(),"T9CP",4);identity(packet.data());word(packet.data()+12,frame);word(packet.data()+16,side);
        EncodeSample(sample,packet.data()+20);return true;
    }
    bool Repeat(u32 frame,std::array<u8,PacketBytes>& packet)const{
        const auto* sample=Get(side,frame);if(!sample)return false;
        std::memcpy(packet.data(),"T9CP",4);identity(packet.data());word(packet.data()+12,frame);word(packet.data()+16,side);
        EncodeSample(*sample,packet.data()+20);return true;
    }
    bool Applied(u32 frame,const std::array<ChargeSample,2>& samples){
        if(!enabled)return true;
        for(u32 player=0;player<2;++player){auto& record=slot(player,frame);
            if(!samples[player].valid()||(record.actual&&record.value!=samples[player]))return false;
            record.applied=true;record.used=samples[player];}
        return true;
    }
    bool Receive(const u8* bytes,std::size_t size,u32 next,std::array<u8,AckBytes>& ack,bool& accepted){
        accepted=false;if(!enabled||size!=PacketBytes)return false;if(!identity(bytes))return true;
        const auto frame=word(bytes+12),player=word(bytes+16);
        if(player!=1-side||frame==Netplay::INVALID_FRAME||(frame>=next&&frame-next>=Capacity))return false;
        ChargeSample sample;if(!DecodeSample(bytes+20,sample))return false;
        if(frame<next&&next-frame>=Capacity){
            if(frame>=frontier[player])return false;
            accepted=true;std::memcpy(ack.data(),"T9CA",4);identity(ack.data());word(ack.data()+12,frontier[player]);word(ack.data()+16,side);return true;
        }
        auto& record=slot(player,frame);
        if(record.actual&&record.value!=sample)return false;
        if(record.applied&&record.used!=sample)rewind=std::min(rewind,frame);
        record.actual=true;record.value=sample;advance(player);accepted=true;
        std::memcpy(ack.data(),"T9CA",4);identity(ack.data());word(ack.data()+12,frontier[player]);word(ack.data()+16,side);return true;
    }
    bool ReceiveAck(const u8* bytes,std::size_t size){
        if(!enabled||size!=AckBytes)return false;if(!identity(bytes))return true;
        const u32 count=word(bytes+12);if(word(bytes+16)!=1-side||count>frontier[side])return false;
        acknowledged=std::max(acknowledged,count);return true;
    }
    bool Batch(std::array<u8,MaxBatchBytes>& packet,std::size_t& size)const{
        if(!enabled)return false;
        std::memcpy(packet.data(),"T9CB",4);identity(packet.data());word(packet.data()+12,side);
        word(packet.data()+16,RemoteCount());u32 count=0;
        const auto append=[&](u32 frame){
            const auto* sample=Get(side,frame);if(!sample)return false;
            auto* record=packet.data()+BatchHeaderBytes+count*BatchRecordBytes;
            word(record,frame);EncodeSample(*sample,record+4);++count;return true;
        };
        const auto end=LocalCount();
        // Always retain the oldest hole. The last slot also carries the fresh
        // result when ACK lag exceeds one batch, without delaying new input.
        const auto prefix=acknowledged+std::min(end-acknowledged,MaxBatchSamples-1);
        for(u32 frame=acknowledged;frame<prefix;++frame)if(!append(frame))return false;
        if(prefix<end&&!append(end-1))return false;
        word(packet.data()+20,count);size=BatchHeaderBytes+count*BatchRecordBytes;return true;
    }
    bool ReceiveBatch(const u8* bytes,std::size_t size,u32 next,bool& accepted,bool& hasData){
        accepted=hasData=false;
        if(!enabled||size<BatchHeaderBytes)return false;
        const auto count=word(bytes+20);
        if(count>MaxBatchSamples||size!=BatchHeaderBytes+count*BatchRecordBytes)return false;
        if(!identity(bytes))return true;
        const auto player=word(bytes+12),ack=word(bytes+16);
        if(player!=1-side||ack>LocalCount())return false;
        std::array<u8,PacketBytes> sample{};std::array<u8,AckBytes> ignored{};
        std::memcpy(sample.data(),"T9CP",4);identity(sample.data());word(sample.data()+16,player);
        for(u32 i=0;i<count;++i){
            const auto* record=bytes+BatchHeaderBytes+i*BatchRecordBytes;
            word(sample.data()+12,word(record));std::memcpy(sample.data()+20,record+4,SampleBytes);
            bool received=false;if(!Receive(sample.data(),sample.size(),next,ignored,received)||!received)return false;
        }
        acknowledged=std::max(acknowledged,ack);accepted=true;hasData=count!=0;return true;
    }
};
}
