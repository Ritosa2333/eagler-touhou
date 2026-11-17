#include "ResourceCrypt.hpp"
#include <algorithm>
namespace th11 {
// Cipher tables as shipped by the original archives. `thdat95.c` in thtk carries
// the same eight rows per generation; keeping them here lets the Runtime unpack
// its own data without depending on a build-time tool.
const CryptParams archive_crypt_th95[8]={
    {0x1b,0x37,0x40,0x2800},{0x51,0xe9,0x40,0x3000},{0xc1,0x51,0x80,0x3200},{0x03,0x19,0x400,0x7800},
    {0xab,0xcd,0x200,0x2800},{0x12,0x34,0x80,0x3200},{0x35,0x97,0x80,0x2800},{0x99,0x37,0x400,0x2000}};
const CryptParams archive_crypt_th12[8]={
    {0x1b,0x73,0x40,0x3800},{0x51,0x9e,0x40,0x4000},{0xc1,0x15,0x400,0x2c00},{0x03,0x91,0x80,0x6400},
    {0xab,0xdc,0x80,0x6e00},{0x12,0x43,0x200,0x3c00},{0x35,0x79,0x400,0x3c00},{0x99,0x7d,0x80,0x2800}};
const CryptParams archive_crypt_th13[8]={
    {0x1b,0x73,0x100,0x3800},{0x12,0x43,0x200,0x3e00},{0x35,0x79,0x400,0x3c00},{0x03,0x91,0x80,0x6400},
    {0xab,0xdc,0x80,0x6e00},{0x51,0x9e,0x100,0x4000},{0xc1,0x15,0x400,0x2c00},{0x99,0x7d,0x80,0x4400}};
const CryptParams archive_crypt_th14[8]={
    {0x1b,0x73,0x100,0x3800},{0x12,0x43,0x200,0x3e00},{0x35,0x79,0x400,0x3c00},{0x03,0x91,0x80,0x6400},
    {0xab,0xdc,0x80,0x7000},{0x51,0x9e,0x100,0x4000},{0xc1,0x15,0x400,0x2c00},{0x99,0x7d,0x80,0x4400}};
const CryptParams* archive_crypt_table(u32 version) noexcept {
    switch(version){
        case 95: case 10: case 103: case 11: return archive_crypt_th95;
        case 12: case 125: case 128: return archive_crypt_th12;
        case 13: return archive_crypt_th13;
        // TH143 1.00a: all 319 entries independently compared with thtk 143.
        case 14: case 143: return archive_crypt_th14;
        // Measurement (section 292): the pbg archives of TH16, TH17, TH18 and TH19 decode to the very
        // bytes an independent thtk extraction produces with this table, sampled per entry, and with the
        // TH13 rows as well; the two tables agree on those entries, and the suite checks every entry.
        case 16: case 17: case 18: case 19: return archive_crypt_th14;
        // An unknown generation used to fall through to the TH14 rows, so a caller that forgot the
        // argument decrypted every entry with the wrong cipher table and the failure surfaced much
        // later as a refused read. A table that cannot be chosen is not a table; say so here.
        default: return nullptr;
    }
}

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
