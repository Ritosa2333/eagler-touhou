#include "BitmapData.hpp"
#include <algorithm>
namespace th075 {
static bool supported(u8 depth){return depth==8||depth==16||depth==24||depth==32;}
bool BitmapData::load(ByteReader& input,const std::array<u16,256>& colors){
    width=input.dword();height=input.dword();sourceStride=input.dword();depth=input.byte();packedSize=input.dword();palette=colors;storage.clear();
    if(!input.valid||!supported(depth)||width==0||height==0||width>8192||height>8192||sourceStride<width||sourceStride>8192)return false;
    const std::uint64_t size=packedSize?packedSize:std::uint64_t(sourceStride)*height*(depth>=24?4:depth/8);
    if(size>256ull*1024*1024)return false;
    const auto* p=input.take(std::size_t(size));if(!p)return false;storage.assign(p,p+size);return true;
}
static void write_pixel(u8* out,u32 value,u32 width){for(u32 i=0;i<width;i++)out[i]=u8(value>>(8*i));}
bool BitmapData::decode(std::vector<u8>& out,u32 outputStride)const{
    if(!supported(depth)||!width||!height)return false;
    if(!outputStride)outputStride=width;if(outputStride<width)return false;
    const u32 bytes=pixel_bytes();const auto size=std::uint64_t(outputStride)*height*bytes;
    if(size>256ull*1024*1024)return false;out.assign(std::size_t(size),0);
    if(!packedSize){
        const std::uint64_t required=std::uint64_t(sourceStride)*height*(depth>=24?4:depth/8);if(required>storage.size())return false;
        for(u32 y=0;y<height;++y){
            if(depth==8){for(u32 x=0;x<width;++x)write_pixel(out.data()+(std::size_t(y)*outputStride+x)*2,palette[storage[std::size_t(y)*sourceStride+x]],2);}
            else if(depth==16){
                // The original copies pairs of 16-bit pixels; an odd final pixel
                // remains untouched. Keep the rule for original-compatible loads.
                const u32 pairs=width/2;const std::size_t from=std::size_t(y)*(sourceStride/2)*4,to=std::size_t(y)*(outputStride/2)*4;
                std::memcpy(out.data()+to,storage.data()+from,pairs*4);
            }else std::memcpy(out.data()+std::size_t(y)*outputStride*4,storage.data()+std::size_t(y)*sourceStride*4,width*4);
        }return true;
    }
    ByteReader runs(storage.data(),storage.size());u32 count=0,value=0,used=0;bool have=false;
    for(u32 y=0;y<height;++y){u32 x=0;while(x<width){
        if(!have){
            if(depth==8){count=runs.byte();value=palette[runs.byte()];}
            else if(depth==16){count=runs.word();value=runs.word();}
            else {count=runs.dword();value=runs.dword();}
            if(!runs.valid)return false;have=true;
        }
        const u32 available=count>used?count-used:1,amount=std::min(available,width-x);
        auto* dest=out.data()+(std::size_t(y)*outputStride+x)*bytes;
        if(bytes==4){auto* pixels=reinterpret_cast<u32*>(dest);std::fill_n(pixels,amount,value);}
        else {auto* pixels=reinterpret_cast<u16*>(dest);std::fill_n(pixels,amount,u16(value));}
        used+=amount;x+=amount;
        if(used>=count){used=depth==8&&used>count?1:0;have=false;}
    }}return true;
}
bool BitmapData::encode_runs(){
    if(packedSize)return true;if(!supported(depth)||!width||!height)return false;
    const u32 bytes=depth>=24?4:depth/8;const std::uint64_t required=std::uint64_t(sourceStride)*height*bytes;if(required>storage.size())return false;
    const auto pixel=[&](u32 x,u32 y){const auto* p=storage.data()+(std::size_t(y)*sourceStride+x)*bytes;u32 value=0;for(u32 i=0;i<bytes;i++)value|=u32(p[i])<<(8*i);return value;};
    std::vector<u8> runs;u32 value=pixel(0,0),count=0;
    const auto append=[&](){const auto start=runs.size();runs.resize(start+bytes*2);write_pixel(runs.data()+start,count,bytes);write_pixel(runs.data()+start+bytes,value,bytes);};
    for(u32 y=0;y<height;++y)for(u32 x=0;x<width;++x){const auto next=pixel(x,y);if(next==value&&(depth!=8||count<255)){count=depth==16?u16(count+1):count+1;}else{append();value=next;count=1;}}
    append();storage=std::move(runs);packedSize=storage.size();return true;
}
bool TexturePack::load(const void* source,std::size_t size,u8 requestedPalette){
    images.clear();error.clear();ByteReader file(source,size);paletteCount=file.byte();selectedPalette=0;std::array<u16,256> colors{};
    if(!file.valid||paletteCount>127){error="Invalid palette count";return false;}
    if(paletteCount){selectedPalette=requestedPalette>=1&&requestedPalette<=paletteCount?requestedPalette:1;for(u32 i=1;i<=paletteCount;i++)for(u32 c=0;c<256;c++){const auto color=file.word();if(i==selectedPalette)colors[c]=color;}}
    if(!file.valid){error="Truncated texture palette";return false;}
    std::vector<BitmapData> parsed;
    while(file.offset<size){BitmapData image;if(!image.load(file,colors)){error="Invalid bitmap record at "+std::to_string(file.offset);return false;}parsed.push_back(std::move(image));if(parsed.size()>65536){error="Too many bitmap records";return false;}}
    images=std::move(parsed);return true;
}
}
