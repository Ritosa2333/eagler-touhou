#include "Lzss.hpp"
namespace th19 {
bool Lzss::decode(const u8* input,u32 size,u8* output,u32 capacity,u32& written) noexcept {
    u32 cursor=0,head=1;u8 mask=0x80,byte=0;written=0;
    auto bit=[&](){if(mask==0x80)byte=cursor<size?input[cursor++]:0;u32 result=bool(byte&mask);mask>>=1;if(!mask)mask=0x80;return result;};
    auto bits=[&](u32 n){u32 value=0;while(n--)value=(value<<1)|bit();return value;};
    auto put=[&](u8 value){output[written++]=value;dictionary[head]=value;head=(head+1)&8191;};
    for(;;){
        if(bit()){if(written==capacity)return false;put(u8(bits(8)));}
        else {
            const u32 offset=bits(13);if(!offset)return true;
            const u32 length=bits(4)+3;if(length>capacity-written)return false;
            for(u32 i=0;i<length;++i)put(dictionary[(offset+i)&8191]);
        }
    }
}
}
