#include "TextRaster.hpp"
#include <algorithm>
namespace th11 {
TextRaster::TextRaster(){scratch.width=1024;scratch.height=65;scratch.pitch=2048;scratch.format=touhou::graphics::PixelFormat::Argb4444;scratch.packed_format=5;scratch.pixels.resize(scratch.pitch*scratch.height);}
void TextRaster::bleed(TextureImage& image,u32 rows){
    auto* pixels=reinterpret_cast<u16*>(image.pixels.data());const u32 stride=image.pitch/2;
    for(u32 y=0;y<std::min(rows,image.height);++y)for(u32 x=0;x<image.width;++x){auto& v=pixels[y*stride+x];if(v&0xf000)continue;u32 red=0,green=0,blue=0,count=0;
        auto collect=[&](u16 n){if(n&0xf000){red+=(n>>8)&15;green+=(n>>4)&15;blue+=n&15;++count;}};
        if(x)collect(pixels[y*stride+x-1]);if(x+1<image.width)collect(pixels[y*stride+x+1]);if(y)collect(pixels[(y-1)*stride+x]);if(y+1<64)collect(pixels[(y+1)*stride+x]);
        if(count>1){red/=count;green/=count;blue/=count;}v=u16((red>>1)<<8|(green>>1)<<4|(blue>>1));
    }
}
bool TextRaster::rasterize(const std::string& text,const TextStyle& input){
    const i32 height=std::max(17,input.height),font=input.font>=0&&input.font<=2?input.font:3;
    const u32 rows=height*2+6;if(rows>scratch.height||text.size()>1024)return false;
    const u16 background=input.plain?u16(((input.color>>4)&15)|((input.color>>8)&0xf0)|((input.color>>12)&0xf00)):0;
    auto* pixels=reinterpret_cast<u16*>(scratch.pixels.data());std::fill(pixels,pixels+scratch.pixels.size()/2,background);
    for(u32 n=0;n<rows*1024;++n)pixels[n]^=0xf000;
    auto draw=[&](const std::string& value,i32 x){
        if(input.plain)return glyphs.draw(scratch,x,0,font,input.color,value);
        for(const auto delta:{Vec2{2,4},Vec2{-2,4},Vec2{2,0},Vec2{-2,0}})if(!glyphs.draw(scratch,x+i32(delta.x),i32(delta.y),font,input.outline,value))return false;
        return glyphs.draw(scratch,x,2,font,input.color,value);
    };
    if(input.spacing&&!input.plain){i32 x=input.offset*2;for(u32 i=0;i<text.size();i+=2){if(!draw(text.substr(i,2),x))return false;x+=input.spacing*2;}}
    else if(!draw(text,input.offset*2))return false;
    for(u32 n=0;n<rows*1024;++n)pixels[n]^=0xf000;
    bleed(scratch,rows);return true;
}
bool TextRaster::write(TextureImage& image,const ImageRect& rect,const std::string& text,const TextStyle& style){
    if(style.height>0&&style.height<=8)return true; // Native 454bb0 early return.
    if(!rasterize(text,style))return false;
    return ImageResample::triangle(image,rect,scratch,{0,0,std::min(1024,(rect.right-rect.left)*2+22),std::max(17,style.height)*2+2});
}
}
