#include "RasterImage.hpp"
namespace th10 {
// 0x436ca0 always writes the atlas as A4R4G4B4, including its padding.
void RasterImage::fill_text_background(u32 color) noexcept {
    const u16 pixel=((color>>12)&0xf00)|((color>>8)&0xf0)|((color>>4)&0xf);
    for(i32 offset=0;offset<byte_size;offset+=2)__builtin_memcpy(pixels+offset,&pixel,2);
}
// 0x437160. Alpha inversion is used while loading animation textures, including
// the transparent-color convention that clears RGB when A1 becomes zero.
bool RasterImage::invert_alpha(i32 rows) noexcept {
    const i32 bytes=static_cast<i32>(static_cast<u32>(pitch)*static_cast<u32>(rows));
    switch(format){
    case 21:for(i32 index=3;index<bytes;index+=4)pixels[index]=~pixels[index];return true;
    case 25:for(i32 index=0;index<bytes;index+=2){u16 pixel;__builtin_memcpy(&pixel,pixels+index,2);pixel^=0x8000;if(!(pixel&0x8000))pixel=0;__builtin_memcpy(pixels+index,&pixel,2);}return true;
    case 26:for(i32 index=1;index<bytes;index+=2)pixels[index]^=0xf0;return true;
    default:return false;
    }
}
// 0x436da0. Only opaque direct neighbours contribute. The A4 format applies
// the original additional division by two. Traversal is tightly packed even
// when the vertical neighbour stride contains padding; preserve that quirk.
bool RasterImage::fill_transparent_edges(u32 rows) noexcept {
    if(format!=21&&format!=26)return true;
    const i32 size=format==21?4:2,stride=(pitch/size)*size;auto* pixel=pixels;
    for(u32 y=0;y<rows;++y)for(u32 x=0;x<static_cast<u32>(width);++x,pixel+=size){
        if(format==21?pixel[3]!=0:(pixel[1]&0xf0)!=0)continue;
        u32 red=0,green=0,blue=0,count=0;
        auto sample=[&](const u8* source){
            if(format==21){if(!source[3])return;red+=source[2];green+=source[1];blue+=source[0];}
            else {if(!(source[1]&0xf0))return;red+=source[1]&15;green+=source[0]>>4;blue+=source[0]&15;}
            ++count;
        };
        if(x)sample(pixel-size);if(x<static_cast<u32>(width)-1)sample(pixel+size);
        if(y)sample(pixel-stride);if(y<static_cast<u32>(height)-1)sample(pixel+stride);
        if(count>1){red/=count;green/=count;blue/=count;}
        if(format==21){pixel[2]=red;pixel[1]=green;pixel[0]=blue;}
        else {pixel[1]=(pixel[1]&0xf0)|(red>>1);pixel[0]=((green>>1)<<4)|(blue>>1);}
    }
    return true;
}
}
