#include "TextRaster.hpp"
#include "PortTitle.hpp"
#include <algorithm>
namespace th11 {
TextRaster::TextRaster(){scratch.width=1024;scratch.height=PORT_TITLE==14?128:65;scratch.pitch=2048;scratch.format=touhou::graphics::PixelFormat::Argb4444;scratch.packed_format=5;scratch.pixels.resize(scratch.pitch*scratch.height);}
void TextRaster::bleed(TextureImage& image,u32 rows){
    auto* pixels=reinterpret_cast<u16*>(image.pixels.data());const u32 stride=image.pitch/2;
    for(u32 y=0;y<std::min(rows,image.height);++y)for(u32 x=0;x<image.width;++x){auto& v=pixels[y*stride+x];if(v&0xf000)continue;u32 red=0,green=0,blue=0,count=0;
        auto collect=[&](u16 n){if(n&0xf000){red+=(n>>8)&15;green+=(n>>4)&15;blue+=n&15;++count;}};
        if(x)collect(pixels[y*stride+x-1]);if(x+1<image.width)collect(pixels[y*stride+x+1]);if(y)collect(pixels[(y-1)*stride+x]);if(y+1<(PORT_TITLE==14?image.height:64))collect(pixels[(y+1)*stride+x]);
        if(count>1){red/=count;green/=count;blue/=count;}v=u16((red>>1)<<8|(green>>1)<<4|(blue>>1));
    }
}
bool TextRaster::rasterize(const std::string& text,const TextStyle& input){
#if PORT_TITLE==14
    const i32 height=std::max(17,input.height),font=input.font>=1&&input.font<=7?input.font:0;
    const u32 rows=height*2+12;if(rows>scratch.height||text.size()>1024)return false;
    auto* pixels=reinterpret_cast<u16*>(scratch.pixels.data());std::fill(pixels,pixels+scratch.pixels.size()/2,0);
    auto invert=[&](){for(u32 n=0;n<rows*1024;++n){pixels[n]^=0xf000;if((pixels[n]&0xf000)&&(pixels[n]&0xf000)!=0xf000)pixels[n]|=0xf000;}};
    invert();
    auto draw=[&](const std::string& value,i32 x,bool fixed){
        if(!input.plain){
            if(fixed){for(const auto d:{Vec2{2,4},Vec2{-2,4},Vec2{2,0},Vec2{-2,0}})if(!glyphs.draw(scratch,x+i32(d.x),i32(d.y),font,input.outline,value))return false;}
            else {for(const auto d:{Vec2{1,1},Vec2{-1,1},Vec2{1,3},Vec2{-1,3},Vec2{2,2},Vec2{-2,2},Vec2{0,0},Vec2{0,4}})if(!glyphs.draw(scratch,x+i32(d.x),i32(d.y),font,input.outline,value))return false;}
        }
        return glyphs.draw(scratch,x,2,font,input.color,value);
    };
    const i32 offset=input.offset_in_pixels?input.offset:wrapping_add(input.offset,input.offset);
    if(input.spacing){i32 x=offset;for(u32 i=0;i<text.size();i+=2){if(!draw(text.substr(i,2),x,true))return false;x=signed_bits(u32(x)+input.spacing);}}
    else if(!draw(text,wrapping_add(offset,2),false))return false;
    invert();
    // Retail466eb0 reads an unchanged copy, skips the left column, and includes
    // the right column's cross-row neighbours. Cardinal weights2, diagonal1 /14.
    std::vector<u16> source(pixels,pixels+rows*1024);source.push_back(0);
    for(u32 i=1024;i<(rows-1)*1024;++i){if((source[i]&0xf000)||i%1024==0)continue;
        u32 alpha=0;for(i32 d:{-1024,1024,-1,1})alpha+=2*(source[i+d]>>12);
        for(i32 d:{-1025,-1023,1023,1025})alpha+=source[i+d]>>12;
        pixels[i]=u16((pixels[i]&0xfff)|((alpha/14)<<12));
    }
    bleed(scratch,rows);return true;
#else
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
#endif
}
bool TextRaster::write(TextureImage& image,const ImageRect& rect,const std::string& text,const TextStyle& style){
    if(style.height>0&&style.height<=8)return true; // Native 454bb0 early return.
    if(!rasterize(text,style))return false;
    if constexpr(PORT_TITLE==14)return ImageResample::copy(image,rect,scratch,{0,0,std::min(1024,rect.right-rect.left),rect.bottom-rect.top});
    return ImageResample::triangle(image,rect,scratch,{0,0,std::min(1024,(rect.right-rect.left)*2+22),std::max(17,style.height)*2+2});
}
}
