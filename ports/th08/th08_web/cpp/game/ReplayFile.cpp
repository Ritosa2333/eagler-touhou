#include "ReplayFile.hpp"
#include "ProgramVersion.hpp"
namespace th08 {
bool ReplayFile::assign_decoded(const u8* data,u32 size){
    bytes.clear();failure="";if(!data||size<sizeof(ReplayMetadata)||size>16*1024*1024){failure="replay size";return false;}
    ReplayMetadata metadata;std::memcpy(&metadata,data,sizeof(metadata));const auto& h=metadata.header;
    if(h.magic!=0x50523854||h.version!=6||h.payload_size<0xcc||h.payload_size>8*1024*1024||h.payload_size>size-sizeof(h)){failure="replay payload size";return false;}
    const u32 end=sizeof(h)+h.payload_size;
    for(u32 offset:h.stage_offsets)if(offset&&(offset<sizeof(metadata)||offset>end||end-offset<sizeof(ReplayStage))){failure="stage offset";return false;}
    for(u32 offset:h.timing_offsets)if(offset&&(offset<sizeof(metadata)||offset>=end)){failure="timing offset";return false;}
    bytes.assign(data,data+size);return true;
}
bool ReplayFile::decode(const u8* data,u32 size){
    bytes.clear();failure="";
    auto fail=[&](const char* reason){bytes.clear();failure=reason;return false;};
    if(size<sizeof(ReplayHeader)||size>16*1024*1024)return fail("replay size");
    ReplayHeader header;std::memcpy(&header,data,sizeof(header));
    if(header.magic!=0x50523854||header.version!=6)return fail("replay format");
    if(header.file_size<sizeof(header)||header.file_size>size)return fail("replay length");
    std::vector<u8> plain(data,data+size);u8 key=header.key;
    for(u32 p=0x18;p<header.file_size;++p){plain[p]=u8(plain[p]-key);key=u8(key+7);}
    u32 sum=0x3f000318;for(u32 p=0x15;p<header.file_size;++p)sum+=plain[p];
    if(sum!=header.checksum)return fail("replay checksum");
    std::memcpy(&header,plain.data(),sizeof(header));
    if(header.compressed_size>header.file_size-sizeof(header)||header.payload_size<0xcc||header.payload_size>8*1024*1024)return fail("replay payload size");
    const u32 end=sizeof(header)+header.payload_size;
    bytes.resize(end+size-header.file_size);std::memcpy(bytes.data(),plain.data(),sizeof(header));u32 written=0;
    if(!codec.decode(plain.data()+sizeof(header),header.compressed_size,bytes.data()+sizeof(header),header.payload_size,written)||written!=header.payload_size)return fail("replay compressed data");
    std::memcpy(bytes.data()+end,data+header.file_size,size-header.file_size);
    if(bytes[0xd9])return fail("slow mode recording");
    u32 exe_size,checksum;std::memcpy(&exe_size,bytes.data()+0x124,4);std::memcpy(&checksum,bytes.data()+0x128,4);
    if(!compatible_program(reinterpret_cast<const char*>(bytes.data()+0x12c),exe_size,checksum,true))return fail("recording program version");
    for(u32 offset:header.stage_offsets)if(offset&&(offset<0x134||offset>end||end-offset<sizeof(ReplayStage)))return fail("stage offset");
    for(u32 offset:header.timing_offsets)if(offset&&(offset<0x134||offset>=end))return fail("timing offset");
    return true;
}
std::vector<u8> ReplayFile::encode(){
    if(bytes.size()<0x134)return {};
    ReplayHeader h;std::memcpy(&h,bytes.data(),sizeof(h));
    auto compressed=codec.encode(bytes.data()+sizeof(h),h.payload_size);
    const u32 end=sizeof(h)+h.payload_size;std::vector<u8> out(sizeof(h)+compressed.size()+bytes.size()-end);
    h.compressed_size=compressed.size();h.file_size=sizeof(h)+compressed.size();h.checksum=0;
    std::memcpy(out.data(),&h,sizeof(h));std::memcpy(out.data()+sizeof(h),compressed.data(),compressed.size());
    std::memcpy(out.data()+h.file_size,bytes.data()+end,bytes.size()-end);
    h.checksum=0x3f000318;for(u32 p=0x15;p<h.file_size;++p)h.checksum+=out[p];std::memcpy(out.data(),&h,sizeof(h));
    u8 key=h.key;for(u32 p=0x18;p<h.file_size;++p){out[p]=u8(out[p]+key);key=u8(key+7);}return out;
}
}
