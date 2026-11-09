#include "ReplayFile.hpp"
#include "Binary.hpp"
#include "ReplayCompression.hpp"
#include <algorithm>
#include <memory>
namespace th09 {
namespace {
constexpr u32 compact_magic=fourcc('T','9','X','1'),compact_header=12,predicted_inputs=1u<<31;

bool transform_inputs(const u8* source,u8* target,u32 size,bool decode){
    std::array<u32,40> offsets{},ordered{};
    for(u32 i=0;i<offsets.size();++i){
        const u32 offset=read32(source+32+i*4);
        if(offset&&(offset<ReplayFile::metadata_end||offset>=size))return false;
        offsets[i]=offset;
    }
    ordered=offsets;std::sort(ordered.begin(),ordered.end());
    for(u32 i=1;i<ordered.size();++i)if(ordered[i]&&ordered[i]==ordered[i-1])return false;
    for(u32 stage=0;stage<10;++stage){
        if(!offsets[stage]&&!offsets[10+stage]&&!offsets[20+stage])continue;
        u32 starts[3]{},frames=0;
        for(u32 group=0;group<3;++group){
            const auto start=offsets[group*10+stage];
            if(!start)return false;
            const auto next=std::upper_bound(ordered.begin(),ordered.end(),start);
            const auto end=next==ordered.end()?size:*next;
            if(end-start<32||(end-start-32)%2)return false;
            const u32 count=(end-start-32)/2;
            if(group&&count!=frames)return false;
            frames=count;starts[group]=start+32;
        }
        u16 previous[3]{};
        for(u32 frame=0;frame<frames;++frame){
            u16 values[3]{};
            for(u32 group=0;group<3;++group){
                const auto* in=source+starts[group];
                if(decode){
                    const auto delta=u16(in[frame])|(u16(in[frames+frame])<<8);
                    previous[group]^=delta;values[group]=previous[group];
                }else values[group]=read16(in+2*frame);
            }
            values[2]^=values[0]|values[1];
            for(u32 group=0;group<3;++group){
                auto* out=target+starts[group];
                if(decode)write16(out+2*frame,values[group]);
                else{
                    const u16 delta=values[group]^previous[group];previous[group]=values[group];
                    out[frame]=u8(delta);out[frames+frame]=u8(delta>>8);
                }
            }
        }
    }
    return true;
}
}
u32 ReplayFile::stream_offset(u32 group,u32 round) const noexcept {
    return group<4&&round<10&&bytes.size()>=header_size?read32(bytes.data()+32+4*(group*10+round)):0;
}
bool ReplayFile::valid() const {
    if(bytes.size()<metadata_end||bytes.size()>max_size||read32(bytes.data())!=fourcc('T','9','R','P')||read16(bytes.data()+4)!=2||bytes[6])return false;
    const u32 payload=read32(bytes.data()+28);
    if(payload<metadata_end-header_size||payload>bytes.size()-header_size)return false;
    for(u32 group=0;group<4;++group)for(u32 round=0;round<10;++round){
        const u32 offset=stream_offset(group,round);
        if(offset&&(offset<metadata_end||offset>=header_size+payload||(group<3&&header_size+payload-offset<32)))return false;
    }
    return true;
}
bool ReplayFile::assign(const u8* data,u32 size){bytes.assign(data,data+size);if(valid())return true;bytes.clear();return false;}
bool ReplayFile::decode(const u8* input,u32 size){
    bytes.clear();
    if(size>=compact_header&&size<=max_size&&read32(input)==compact_magic){
        const u32 flags=read32(input+4),plain=flags&~predicted_inputs,packed=read32(input+8);
        if(!packed||packed>=size-compact_header||plain<metadata_end)return false;
        const u32 tail=size-compact_header-packed;
        if(plain>max_size-tail)return false;
        bytes.resize(plain+tail);
        if(replay_compression::inflate(input+compact_header,packed,bytes.data(),plain)&&
           read32(bytes.data()+28)==plain-header_size){
            std::memcpy(bytes.data()+plain,input+compact_header+packed,tail);
            if(valid()){
                if(!(flags&predicted_inputs))return true;
                const std::vector<u8> source(bytes.begin(),bytes.begin()+plain);
                if(transform_inputs(source.data(),bytes.data(),plain,true))return true;
            }
        }
        bytes.clear();return false;
    }
    if(size<header_size||size>max_size||read32(input)!=fourcc('T','9','R','P')||read16(input+4)!=2||input[6])return false;
    const u32 file_end=read32(input+12);if(file_end<header_size||file_end>size)return false;
    std::vector<u8> plain(input,input+size);u8 key=plain[21];
    for(u32 i=24;i<file_end;++i){plain[i]=u8(plain[i]-key);key=u8(key+7);}
    u32 checksum=0x3f000318;for(u32 i=21;i<file_end;++i)checksum+=plain[i];
    const u32 packed=read32(plain.data()+24),payload=read32(plain.data()+28),tail=size-file_end;
    if(checksum!=read32(plain.data()+16)||packed!=file_end-header_size||payload<metadata_end-header_size||payload>max_size-header_size-tail)return false;
    bytes.resize(header_size+payload+tail);std::memcpy(bytes.data(),plain.data(),header_size);u32 written=0;
    auto codec=std::make_unique<Lzss>();
    if(!codec->decode(plain.data()+header_size,packed,bytes.data()+header_size,payload,written)||written!=payload){bytes.clear();return false;}
    std::memcpy(bytes.data()+header_size+payload,plain.data()+file_end,tail);
    if(valid())return true;bytes.clear();return false;
}
std::vector<u8> ReplayFile::encode(){
    if(!valid())return {};
    const u32 payload=read32(bytes.data()+28),tail=bytes.size()-header_size-payload;
    auto codec=std::make_unique<Lzss>();auto packed=codec->encode(bytes.data()+header_size,payload);
    const u32 end=header_size+packed.size();
    if(tail){
        const u32 plain=header_size+payload;
        auto compact=replay_compression::deflate(bytes.data(),plain);
        u32 flags=plain;
        std::vector<u8> transformed(bytes.begin(),bytes.begin()+plain);
        if(transform_inputs(bytes.data(),transformed.data(),plain,false)){
            auto candidate=replay_compression::deflate(transformed.data(),plain);
            if(!candidate.empty()&&(compact.empty()||candidate.size()<compact.size())){
                compact=std::move(candidate);flags|=predicted_inputs;
            }
        }
        if(!compact.empty()&&compact.size()+compact_header<end){
            std::vector<u8> output(compact_header+compact.size()+tail);
            write32(output.data(),compact_magic);write32(output.data()+4,flags);write32(output.data()+8,u32(compact.size()));
            std::memcpy(output.data()+compact_header,compact.data(),compact.size());
            std::memcpy(output.data()+compact_header+compact.size(),bytes.data()+plain,tail);
            return output;
        }
    }
    std::vector<u8> output(end+tail);
    std::memcpy(output.data(),bytes.data(),header_size);std::memcpy(output.data()+header_size,packed.data(),packed.size());
    std::memcpy(output.data()+end,bytes.data()+header_size+payload,tail);
    write32(output.data()+12,end);write32(output.data()+24,packed.size());
    u32 checksum=0x3f000318;for(u32 i=21;i<end;++i)checksum+=output[i];write32(output.data()+16,checksum);
    u8 key=output[21];for(u32 i=24;i<end;++i){output[i]=u8(output[i]+key);key=u8(key+7);}
    return output;
}
}
