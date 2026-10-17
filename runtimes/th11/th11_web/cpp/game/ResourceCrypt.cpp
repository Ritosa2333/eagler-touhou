#include "ResourceCrypt.hpp"
#include <algorithm>
namespace th11 {
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
}
