#include "ChargeReplayTrack.hpp"
#include "Lzss.hpp"
#include <memory>
#include <algorithm>
namespace th09 {
namespace {
u32 checksum(const u8* bytes,u32 size){u32 value=2166136261u;for(u32 i=0;i<size;++i)value=(value^bytes[i])*16777619u;return value;}
bool ordered(const ChargeReplayTrack::Entry& a,const ChargeReplayTrack::Entry& b){return a.frame<b.frame||(a.frame==b.frame&&a.hold);}
}
bool ChargeReplayTrack::record(u32 stage,u32 frame,bool hold,const std::array<ChargeSample,2>& samples){
    if(stage>=10||frame>=Limit||playing)return false;
    for(const auto& sample:samples)if(!sample.valid())return false;
    auto& entries=stages[stage];Entry entry{frame,u32(hold),samples};
    if(entries.size()>=Limit||(!entries.empty()&&!ordered(entries.back(),entry))){invalid=true;return false;}
    entries.push_back(entry);enabled=true;return true;
}
const ChargeReplayTrack::Entry* ChargeReplayTrack::peek(u32 stage)const{
    if(stage>=10||cursors[stage]>=stages[stage].size())return nullptr;return &stages[stage][cursors[stage]];
}
bool ChargeReplayTrack::load(const u8* bytes,u32 size){
    clear();
    // A touch track, when present, keeps its existing final-footer position.
    if(size>=24&&!std::memcmp(bytes+size-24,"THMOTION",8)){
        const auto length=charge_record::word(bytes+size-8);if(length>size-24)return false;size-=length+24;
    }
    if(size<24||std::memcmp(bytes+size-24,"T9CHARGE",8))return true;
    const auto* footer=bytes+size-24;const u32 length=charge_record::word(footer+16);
    if(charge_record::word(footer+8)!=9||charge_record::word(footer+12)!=1||length<5||length>size-24)return false;
    const auto* payload=footer-length;
    if(checksum(payload,length)!=charge_record::word(footer+20))return false;
    const u32 plain_size=charge_record::word(payload);
    if(plain_size<40||plain_size>40+Limit*RowBytes||(plain_size-40)%RowBytes)return false;
    std::vector<u8> plain(plain_size);u32 written=0;auto codec=std::make_unique<Lzss>();
    if(!codec->decode(payload+4,length-4,plain.data(),plain_size,written)||written!=plain_size)return false;
    u32 offset=40,total=0;
    for(u32 stage=0;stage<10;++stage){
        const auto count=charge_record::word(plain.data()+stage*4);
        if(count>Limit-total||count>(plain_size-offset)/RowBytes)return false;total+=count;
        auto& entries=stages[stage];entries.reserve(count);
        for(u32 i=0;i<count;++i){
            const auto* at=plain.data()+offset;Entry entry;entry.frame=charge_record::word(at);entry.hold=charge_record::word(at+4);
            if(entry.frame>=Limit||entry.hold>1||!charge_record::decode(at+8,entry.samples[0])||!charge_record::decode(at+48,entry.samples[1])||
               (!entries.empty()&&!ordered(entries.back(),entry)))return false;
            entries.push_back(entry);offset+=RowBytes;
        }
    }
    if(offset!=plain_size)return false;enabled=playing=true;return true;
}
std::vector<u8> ChargeReplayTrack::trailer()const{
    if(!enabled||invalid)return {};
    u32 total=0;for(const auto& stage:stages){if(stage.size()>Limit-total)return {};total+=u32(stage.size());}
    std::vector<u8> plain(40+total*RowBytes);u32 offset=40;
    for(u32 stage=0;stage<10;++stage){
        charge_record::word(plain.data()+stage*4,u32(stages[stage].size()));
        for(const auto& entry:stages[stage]){
            auto* at=plain.data()+offset;charge_record::word(at,entry.frame);charge_record::word(at+4,entry.hold);
            charge_record::encode(entry.samples[0],at+8);charge_record::encode(entry.samples[1],at+48);offset+=RowBytes;
        }
    }
    auto codec=std::make_unique<Lzss>();auto packed=codec->encode(plain.data(),u32(plain.size()));
    std::vector<u8> output(4+packed.size()+24);charge_record::word(output.data(),u32(plain.size()));
    std::copy(packed.begin(),packed.end(),output.begin()+4);const u32 length=u32(output.size())-24;
    auto* footer=output.data()+length;std::memcpy(footer,"T9CHARGE",8);charge_record::word(footer+8,9);charge_record::word(footer+12,1);
    charge_record::word(footer+16,length);charge_record::word(footer+20,checksum(output.data(),length));return output;
}
}
