#include "ChargeReplayTrack.hpp"
#include "Lzss.hpp"
#include <memory>
#include <algorithm>
namespace th09 {
namespace {
u32 checksum(const u8* bytes,u32 size){u32 value=2166136261u;for(u32 i=0;i<size;++i)value=(value^bytes[i])*16777619u;return value;}
// Delta each exact 32-bit word, then group equal byte lanes for LZSS.
// This preserves floats bit-for-bit while separating rapidly changing timers.
std::vector<u8> pack_columns(const std::vector<u8>& rows){
    const u32 count=u32(rows.size()-40)/88;std::vector<u8> columns(rows.size());
    std::copy_n(rows.data(),40,columns.data());
    for(u32 word=0;word<22;++word){u32 previous=0;
        for(u32 row=0;row<count;++row){const u32 value=charge_record::word(rows.data()+40+row*88+word*4),delta=value-previous;previous=value;
            for(u32 byte=0;byte<4;++byte)columns[40+(word*4+byte)*count+row]=u8(delta>>(byte*8));
        }
    }return columns;
}
void unpack_columns(std::vector<u8>& columns){
    const u32 count=u32(columns.size()-40)/88;std::vector<u8> rows(columns.size());std::copy_n(columns.data(),40,rows.data());
    for(u32 word=0;word<22;++word){u32 value=0;
        for(u32 row=0;row<count;++row){u32 delta=0;
            for(u32 byte=0;byte<4;++byte)delta|=u32(columns[40+(word*4+byte)*count+row])<<(byte*8);
            value+=delta;charge_record::word(rows.data()+40+row*88+word*4,value);
        }
    }columns.swap(rows);
}
std::vector<u8> pack_zeros(const std::vector<u8>& bytes){
    std::vector<u8> out;
    for(std::size_t i=0;i<bytes.size();){
        if(bytes[i]){out.push_back(bytes[i++]);continue;}
        u32 count=0;while(i<bytes.size()&&!bytes[i]){++count;++i;}
        out.push_back(0);do{const u8 part=u8(count&127);count>>=7;out.push_back(part|(count?128:0));}while(count);
    }return out;
}
bool unpack_zeros(const std::vector<u8>& bytes,std::vector<u8>& out){
    std::size_t at=0;
    for(std::size_t i=0;i<bytes.size();){
        if(bytes[i]){if(at==out.size())return false;out[at++]=bytes[i++];continue;}
        ++i;u32 count=0,shift=0;u8 part;
        do{if(i==bytes.size()||shift>=28)return false;part=bytes[i++];count|=u32(part&127)<<shift;shift+=7;}while(part&128);
        if(!count||count>out.size()-at)return false;at+=count;
    }return at==out.size();
}
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
    const u32 version=charge_record::word(footer+12);
    if(charge_record::word(footer+8)!=9||(version!=1&&version!=2)||length<5||length>size-24)return false;
    const auto* payload=footer-length;
    if(checksum(payload,length)!=charge_record::word(footer+20))return false;
    const u32 plain_size=charge_record::word(payload);
    if(plain_size<40||plain_size>40+Limit*RowBytes||(plain_size-40)%RowBytes)return false;
    const u32 header=version==2?8:4;if(length<=header)return false;
    const u32 packed_plain=version==2?charge_record::word(payload+4):plain_size;
    if(!packed_plain||packed_plain>2*plain_size)return false;
    std::vector<u8> decoded(packed_plain);u32 written=0;auto codec=std::make_unique<Lzss>();
    if(!codec->decode(payload+header,length-header,decoded.data(),packed_plain,written)||written!=packed_plain)return false;
    std::vector<u8> plain;
    if(version==2){plain.resize(plain_size);if(!unpack_zeros(decoded,plain))return false;unpack_columns(plain);}
    else plain.swap(decoded);
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
    auto codec=std::make_unique<Lzss>();auto columns=pack_zeros(pack_columns(plain));auto packed=codec->encode(columns.data(),u32(columns.size()));
    std::vector<u8> output(8+packed.size()+24);charge_record::word(output.data(),u32(plain.size()));charge_record::word(output.data()+4,u32(columns.size()));
    std::copy(packed.begin(),packed.end(),output.begin()+8);const u32 length=u32(output.size())-24;
    auto* footer=output.data()+length;std::memcpy(footer,"T9CHARGE",8);charge_record::word(footer+8,9);charge_record::word(footer+12,2);
    charge_record::word(footer+16,length);charge_record::word(footer+20,checksum(output.data(),length));return output;
}
}
