#include "ReplayFile.hpp"
#include "Binary.hpp"
#include <memory>
namespace th09 {
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
    bytes.clear();if(size<header_size||size>max_size||read32(input)!=fourcc('T','9','R','P')||read16(input+4)!=2||input[6])return false;
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
    const u32 end=header_size+packed.size();std::vector<u8> output(end+tail);
    std::memcpy(output.data(),bytes.data(),header_size);std::memcpy(output.data()+header_size,packed.data(),packed.size());
    std::memcpy(output.data()+end,bytes.data()+header_size+payload,tail);
    write32(output.data()+12,end);write32(output.data()+24,packed.size());
    u32 checksum=0x3f000318;for(u32 i=21;i<end;++i)checksum+=output[i];write32(output.data()+16,checksum);
    u8 key=output[21];for(u32 i=24;i<end;++i){output[i]=u8(output[i]+key);key=u8(key+7);}
    return output;
}
}
