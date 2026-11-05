#include "ResourceCrypt.hpp"
#include <algorithm>
namespace th09 {
bool resource_crypt(const u8* in,u8* out,u32 size,CryptParams p,bool encrypt) noexcept {
    if(!p.block)return false;
    u32 untouched=(size%p.block<p.block/4?size%p.block:0)+(size&1);
    // The original accepts signed sizes; an odd tiny input can make this -1.
    // Its loop then copies the entire input unchanged.
    i64 remaining=i64(size)-untouched,limit=p.limit;
    u32 cursor=0;
    while(remaining>0&&limit>0){
        const u32 block=std::min<u32>(p.block,remaining);
        u32 linear=0;
        for(i32 parity=1;parity<=2;++parity)for(i32 pos=i32(block)-parity;pos>=0;pos-=2){
            if(encrypt)out[cursor+linear]=in[cursor+pos]^p.key;
            else out[cursor+pos]=in[cursor+linear]^p.key;
            p.key=u8(p.key+p.step);++linear;
        }
        cursor+=block;remaining-=block;limit-=block;
    }
    if(cursor<size)std::memcpy(out+cursor,in+cursor,size-cursor);
    return true;
}
bool resource_unwrap(std::vector<u8>& bytes) {
    if(bytes.size()<4||bytes[0]!=0x65||bytes[1]!=0x64||bytes[2]!=0x7a)return true;
    static constexpr u8 tags[]={0x4d,0x54,0x41,0x4a,0x45,0x57,0x2d,0x2a};
    static constexpr CryptParams table[]={
        {0x1b,0x37,0x40,0x2800},{0x51,0xe9,0x40,0x3000},
        {0xc1,0x51,0x400,0x400},{0x03,0x19,0x400,0x400},
        {0xab,0xcd,0x200,0x1000},{0x12,0x34,0x400,0x400},
        {0x35,0x97,0x80,0x2800},{0x99,0x37,0x400,0x1000}};
    for(u32 i=0;i<8;++i)if(bytes[3]==tags[i]){
        std::vector<u8> decoded(bytes.size()-4);
        resource_crypt(bytes.data()+4,decoded.data(),decoded.size(),table[i],false);
        bytes.swap(decoded);return true;
    }
    // Match the original behavior for an unrecognized signature discriminator.
    return true;
}
}
