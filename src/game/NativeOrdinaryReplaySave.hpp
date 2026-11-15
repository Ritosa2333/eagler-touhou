#pragma once
#include "NativeReplaySlotPreview.hpp"
#include <algorithm>
#include <bit>
#include <span>
#include <vector>
namespace th128::postrun {
inline std::array<std::uint8_t,32> ordinaryReplaySHA256(std::span<const std::uint8_t> bytes){
    constexpr std::array<std::uint32_t,64> k{0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::array<std::uint32_t,8> h{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    std::vector<std::uint8_t> data(bytes.begin(),bytes.end());data.push_back(0x80);while(data.size()%64!=56)data.push_back(0);
    const std::uint64_t bits=std::uint64_t(bytes.size())*8;for(int shift=56;shift>=0;shift-=8)data.push_back(std::uint8_t(bits>>shift));
    for(std::size_t block=0;block<data.size();block+=64){std::array<std::uint32_t,64>w{};
        for(unsigned i=0;i<16;++i)for(unsigned j=0;j<4;++j)w[i]=(w[i]<<8)|data[block+i*4+j];
        for(unsigned i=16;i<64;++i){const auto a=std::rotr(w[i-15],7)^std::rotr(w[i-15],18)^(w[i-15]>>3),b=std::rotr(w[i-2],17)^std::rotr(w[i-2],19)^(w[i-2]>>10);w[i]=w[i-16]+a+w[i-7]+b;}
        auto a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],z=h[7];
        for(unsigned i=0;i<64;++i){const auto s=std::rotr(e,6)^std::rotr(e,11)^std::rotr(e,25),ch=(e&f)^(~e&g),t1=z+s+ch+k[i]+w[i];
            const auto t2=(std::rotr(a,2)^std::rotr(a,13)^std::rotr(a,22))+((a&b)^(a&c)^(b&c));z=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;}
        h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=z;
    }
    std::array<std::uint8_t,32> result{};for(unsigned i=0;i<8;++i)for(unsigned j=0;j<4;++j)result[i*4+j]=std::uint8_t(h[i]>>(24-8*j));return result;
}
class NativeOrdinaryReplaySave {
    unsigned generation_=0,serial_=0,tick_=0,slot_=0;
    std::array<std::array<std::uint8_t,32>,25> digests_{};
    std::vector<std::uint8_t> previews_,packet_;
    static unsigned word(std::span<const std::uint8_t> bytes,unsigned offset){return record::NativeReplaySlotPreview::word(bytes,offset);}
public:
    static unsigned checksum(std::span<const std::uint8_t> bytes){unsigned value=2166136261u;for(auto b:bytes){value^=b;value*=16777619u;}return value;}
    bool bound()const{return generation_!=0;}
    bool pending()const{return !packet_.empty();}
    unsigned slot()const{return slot_;}
    unsigned serial()const{return serial_;}
    std::span<const std::uint8_t> packet()const{return packet_;}
    std::span<const std::uint8_t> previews()const{return previews_;}
    void bind(unsigned generation,unsigned tick,std::span<const std::uint8_t> bytes){
        if(bound()||!generation||bytes.size()!=4236||word(bytes,0)!=0x3150524eu||word(bytes,4)!=1||word(bytes,8)!=generation
            ||word(bytes,12)!=tick||word(bytes,16)!=1||word(bytes,20)!=3412)throw std::runtime_error("ordinary replay observation generation/tick envelope rejected");
        const auto preview=bytes.subspan(24,3412);const auto rows=record::NativeReplaySlotPreview::decode(preview);
        for(unsigned slot=0;slot<25;++slot){const auto digest=bytes.subspan(3436+slot*32,32);const bool nonzero=std::any_of(digest.begin(),digest.end(),[](auto b){return b!=0;});
            if(rows[slot].occupied!=nonzero||rows[slot].occupied&&!rows[slot].known)throw std::runtime_error("ordinary occupied replay observation lacks its complete metadata/SHA256");
            std::copy(digest.begin(),digest.end(),digests_[slot].begin());}
        previews_.assign(preview.begin(),preview.end());generation_=generation;
    }
    void prepare(unsigned generation,unsigned tick,unsigned slot,unsigned route,unsigned rank,unsigned score,unsigned frames,unsigned finishSerial,std::span<const std::uint8_t> raw){
        if(!bound()||pending()||generation!=generation_||!tick||slot>=25||!((route<6&&rank<4)||(route==6&&rank==4))||!frames||!finishSerial||serial_==UINT32_MAX
            ||raw.size()<(route==6?299u:580u)||raw.size()>32u*1024u*1024u||word(raw,0)!=0x72383231||word(raw,4)!=2||word(raw,56)!=score||word(raw,124)!=(route==6?1u:3u)
            ||word(raw,128)!=route||word(raw,136)!=rank||word(raw,140)!=route+17||word(raw,144)!=0)
            throw std::runtime_error("ordinary full variable replay save producer identity rejected");
        ++serial_;tick_=tick;slot_=slot;const auto put=[this](unsigned v){for(unsigned i=0;i<4;++i)packet_.push_back(std::uint8_t(v>>(8*i)));};
        put(0x3153524eu);put(1);put(128+unsigned(raw.size()));put(generation);put(serial_);put(tick);put(slot);put(word(previews_,12+136*slot));
        put(unsigned(raw.size()));put(checksum(raw));put(route);put(rank);put(score);put(route+17);put(frames);put(finishSerial);
        packet_.insert(packet_.end(),digests_[slot].begin(),digests_[slot].end());const auto sha=ordinaryReplaySHA256(raw);
        packet_.insert(packet_.end(),sha.begin(),sha.end());packet_.insert(packet_.end(),raw.begin(),raw.end());
    }
    void acknowledge(unsigned generation,unsigned tick,std::span<const std::uint8_t> bytes){
        if(!pending()||generation!=generation_||tick!=tick_||bytes.size()!=packet_.size()+32+3412
            ||!std::equal(packet_.begin(),packet_.end(),bytes.begin()))throw std::runtime_error("ordinary full replay save ACK is stale, duplicate or changed");
        const auto digest=bytes.subspan(packet_.size(),32),preview=bytes.last(3412);const auto rows=record::NativeReplaySlotPreview::decode(preview);
        if(std::none_of(digest.begin(),digest.end(),[](auto b){return b!=0;})||!rows[slot_].occupied||!rows[slot_].known
            ||!std::equal(rows[slot_].metadata.begin(),rows[slot_].metadata.end(),packet_.begin()+128+36))
            throw std::runtime_error("ordinary saved slot full metadata/SHA256 readback rejected");
        for(unsigned slot=0;slot<25;++slot)if(slot!=slot_&&!std::equal(previews_.begin()+12+slot*136,previews_.begin()+12+(slot+1)*136,preview.begin()+12+slot*136))
            throw std::runtime_error("ordinary other replay slot changed during save receipt");
        std::copy(digest.begin(),digest.end(),digests_[slot_].begin());previews_.assign(preview.begin(),preview.end());packet_.clear();
    }
};
}
