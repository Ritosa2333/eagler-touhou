#include "Archive.hpp"
#include "ResourceCrypt.hpp"
namespace th15 {
namespace {
u32 word(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
constexpr CryptParams ciphers[]={{27,115,256,14336},{18,67,512,15872},{53,121,1024,15360},{3,145,128,25600},{171,220,128,28672},{81,158,256,16384},{193,21,1024,11264},{153,125,128,17408}};
}
bool Archive::open(const u8* bytes,u32 size){
    entries.clear();source=nullptr;source_size=0;if(size<16)return false;
    u8 header[16];resource_crypt(bytes,header,16,{27,55,16,16},false);
    if(std::memcmp(header,"THA1",4))return false;
    const u32 unpacked=word(header+4)-123456789u,packed=word(header+8)-987654321u,count=word(header+12)-135792468u;
    if(!count||count>100000||packed>size-16||!packed||unpacked>64*1024*1024||unpacked<count*16)return false;
    const u32 offset=size-packed;std::vector<u8> encoded(packed),table(unpacked);
    resource_crypt(bytes+offset,encoded.data(),packed,{62,155,128,packed},false);
    u32 written=0;if(!codec.decode(encoded.data(),packed,table.data(),unpacked,written)||written!=unpacked)return false;
    u32 cursor=0;std::vector<ArchiveEntry> parsed;parsed.reserve(count);
    for(u32 i=0;i<count;++i){
        const u32 begin=cursor;while(cursor<unpacked&&table[cursor])++cursor;
        if(cursor==unpacked||cursor-begin>1024)return false;
        ArchiveEntry e;e.name.assign(reinterpret_cast<const char*>(table.data()+begin),cursor-begin);
        cursor=begin+((cursor-begin+4)&~3u);if(cursor>unpacked||unpacked-cursor<12)return false;
        e.offset=word(table.data()+cursor);e.size=word(table.data()+cursor+4);e.reserved=word(table.data()+cursor+8);cursor+=12;
        if(e.offset<16||e.offset>offset||e.size>64*1024*1024)return false;
        if(!parsed.empty()){if(e.offset<parsed.back().offset)return false;parsed.back().compressed=e.offset-parsed.back().offset;}
        parsed.push_back(std::move(e));
    }
    parsed.back().compressed=offset-parsed.back().offset;entries.swap(parsed);source=bytes;source_size=size;return true;
}
bool Archive::read(u32 index,std::vector<u8>& output){
    output.clear();if(!source||index>=entries.size())return false;
    const auto& e=entries[index];u8 sum=0;for(unsigned char c:e.name)sum=u8(sum+c);
    std::vector<u8> decoded(e.compressed);resource_crypt(source+e.offset,decoded.data(),e.compressed,ciphers[sum&7],false);
    if(e.compressed==e.size){output.swap(decoded);return true;}
    output.resize(e.size);u32 written=0;
    if(!codec.decode(decoded.data(),decoded.size(),output.data(),output.size(),written)||written!=e.size){output.clear();return false;}
    return true;
}
}
