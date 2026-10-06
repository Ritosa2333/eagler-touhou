#include "CheckpointFile.hpp"
#include "Lzss.hpp"
#include <cstring>
namespace th15 {
namespace {
u32 word(const u8* p)noexcept{return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
void put(u8* p,u32 v)noexcept{for(u32 i=0;i<4;i++)p[i]=u8(v>>(i*8));}
}
CheckpointHeader CheckpointHeader::create(i64 stamp,i32 c,i32 d,i32 s,i32 chapter,const std::array<i32,10>& retries,u32 flags)noexcept{
    CheckpointHeader h;put(h.bytes.data(),0x62353174);h.bytes[4]=6;
    put(h.bytes.data()+8,u32(stamp));put(h.bytes.data()+12,u32(u64(stamp)>>32));
    put(h.bytes.data()+0x10,u32(c));put(h.bytes.data()+0x14,u32(d));put(h.bytes.data()+0x18,u32(s));put(h.bytes.data()+0x1c,u32(chapter));
    for(u32 i=0;i<retries.size();i++)put(h.bytes.data()+0x20+i*4,u32(retries[i]));put(h.bytes.data()+0x48,((flags>>2)&15)+1);return h;
}
bool CheckpointHeader::valid()const noexcept{return word(bytes.data())==0x62353174&&bytes[4]==6&&bytes[5]==0;}
bool CheckpointHeader::open(const u8* p,u32 n)noexcept{bytes.fill(0);if(!p||n<bytes.size())return false;std::memcpy(bytes.data(),p,bytes.size());return valid();}
i32 CheckpointHeader::character()const noexcept{return signed_bits(word(bytes.data()+0x10));}
i32 CheckpointHeader::difficulty()const noexcept{return signed_bits(word(bytes.data()+0x14));}
i32 CheckpointHeader::stage()const noexcept{return signed_bits(word(bytes.data()+0x18));}
i32 CheckpointHeader::chapter()const noexcept{return signed_bits(word(bytes.data()+0x1c));}
bool CheckpointHeader::compatible(i32 c,i32 d,u32 flags)const noexcept{
    if(!valid()||character()!=c||difficulty()!=d)return false;
    const u32 mode=word(bytes.data()+0x48),current=(flags>>2)&15;
    if(!mode)return true;
    if((current==2||current==5)&&(mode==3||mode==6))return true;
    if((current==1||current==4)&&(mode==2||mode==5))return true;
    return (current==0||current==3)&&(mode==1||mode==4);
}
bool CheckpointFile::open(const u8* p,u32 n){
    error.clear();for(auto& v:sections)v.clear();
    if(!header.open(p,n)){error="Invalid Pointdevice checkpoint header";return false;}
    const u32 packed=word(p+0x58),plain=word(p+0x5c);
    if(packed>n-0x60||!plain||plain>max_payload){error="Pointdevice checkpoint lengths outside range";return false;}
    std::vector<u8> data(plain);u32 written=0;Lzss codec;
    if(!codec.decode(p+0x60,packed,data.data(),plain,written)||written!=plain){error="Invalid Pointdevice checkpoint compression";return false;}
    u32 at=0;
    for(u32 id=0;id<sections.size();id++){
        if(plain-at<12||word(data.data()+at)!=0x70616863||word(data.data()+at+4)!=id){error="Invalid Pointdevice checkpoint section order";return false;}
        const u32 length=word(data.data()+at+8);at+=12;
        if(length>plain-at){error="Truncated Pointdevice checkpoint section";return false;}
        sections[id].assign(data.begin()+at,data.begin()+at+length);at+=length;
    }
    if(at!=plain||sections[0].size()!=0x1cc||sections[7].size()!=0x514){error="Invalid Pointdevice checkpoint module size";return false;}
    return true;
}
bool CheckpointFile::payload(std::vector<u8>& output){
    error.clear();output.clear();u64 total=sections.size()*12;
    for(const auto& v:sections)total+=v.size();
    if(!header.valid()||total>max_payload||sections[0].size()!=0x1cc||sections[7].size()!=0x514){error="Invalid Pointdevice checkpoint state";return false;}
    std::vector<u8> data(static_cast<size_t>(total),u8(0));u32 at=0;
    for(u32 id=0;id<sections.size();id++){
        put(data.data()+at,0x70616863);put(data.data()+at+4,id);put(data.data()+at+8,u32(sections[id].size()));at+=12;
        if(!sections[id].empty())std::memcpy(data.data()+at,sections[id].data(),sections[id].size());at+=u32(sections[id].size());
    }
    output=std::move(data);return true;
}
bool CheckpointFile::encode(std::vector<u8>& output){
    std::vector<u8> data;if(!payload(data))return false;
    Lzss codec;const auto packed=codec.encode(data.data(),data.size());
    if(packed.empty()){error="Pointdevice checkpoint compression failed";return false;}
    put(header.bytes.data()+0x58,u32(packed.size()));put(header.bytes.data()+0x5c,u32(data.size()));
    output.assign(header.bytes.begin(),header.bytes.end());output.insert(output.end(),packed.begin(),packed.end());return true;
}
bool CheckpointFile::update_header(std::vector<u8>& bytes,const CheckpointHeader& h)noexcept{
    CheckpointHeader old;if(!h.valid()||!old.open(bytes.data(),u32(bytes.size())))return false;
    std::memcpy(bytes.data(),h.bytes.data(),0x58);return true;
}
}
