#include "TextRaster.hpp"
#include <algorithm>
namespace th15 {
namespace {template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}template<class T>void put(u8* p,T v){std::memcpy(p,&v,sizeof v);}
bool valid(const TextureImage& im,u32 rows,u32 bpp){return im.width&&rows<=im.height&&im.pitch==im.width*bpp&&im.pixels.size()>=u64(im.pitch)*im.height;}
constexpr u32 styles[2][10]={{2,4,6,7,3,5,6,7,1,0},{10,4,12,7,11,5,12,7,9,8}};
}
TextRaster::TextRaster(){scratch.width=1024;scratch.height=129;scratch.pitch=2048;scratch.packed_format=5;scratch.format=touhou::graphics::PixelFormat::Argb4444;scratch.pixels.resize(scratch.pitch*scratch.height);}
u32 TextRaster::font_index(i32 style,bool fallback)noexcept{return styles[fallback?1:0][style>=0&&style<=8?style:9];}
bool TextRaster::invert_alpha(TextureImage& image,u32 rows){
    using F=touhou::graphics::PixelFormat;const u32 bpp=image.format==F::Bgra8?4:2;if(!valid(image,rows,bpp))return false;
    for(u32 p=0;p<rows*image.pitch;p+=bpp){auto* at=image.pixels.data()+p;
        if(image.format==F::Argb4444){u16 v=read<u16>(at)^0xf000;if((v&0xf000)!=0&&(v&0xf000)!=0xf000)v|=0xf000;put(at,v);}
        else if(image.format==F::Argb1555){u16 v=read<u16>(at)^0x8000;if(!(v&0x8000))v=0;put(at,v);}
        else if(image.format==F::Bgra8)at[3]^=255;else return false;
    }return true;
}
bool TextRaster::expand_alpha(TextureImage& image,u32 rows){
    if(image.format!=touhou::graphics::PixelFormat::Argb4444)return true;if(!valid(image,rows,2))return false;if(rows<=2)return true;
    const auto input=image.pixels;const u32 width=image.width;auto alpha=[&](u32 n){return n<rows*width?read<u16>(input.data()+n*2)>>12:0;};
    u32 left=width,right=0;for(u32 n=0;n<rows*width;n++)if(alpha(n)){left=std::min(left,n%width);right=std::max(right,n%width);}
    if(left==width)return true;
    // Only the glyph columns and their one-pixel halo can gain alpha. The
    // final column is also evaluated: the native linear kernel wraps there.
    const u32 first=std::max(1u,left?left-1:0u),last=std::min(width,right+2);
    auto expand=[&](u32 n){if(alpha(n))return;const u32 sum=2*(alpha(n-width)+alpha(n+width)+alpha(n-1)+alpha(n+1))+alpha(n+width-1)+alpha(n+width+1)+alpha(n-width-1)+alpha(n-width+1);auto* out=image.pixels.data()+n*2;put(out,u16((read<u16>(out)&0xfff)|((sum/14)<<12)));};
    for(u32 y=1;y+1<rows;y++){for(u32 x=first;x<last;x++)expand(y*width+x);if(last<width&&width>1)expand(y*width+width-1);} return true;
}
bool TextRaster::bleed(TextureImage& image,u32 rows){
    using F=touhou::graphics::PixelFormat;const u32 bpp=image.format==F::Bgra8?4:2;if(image.format!=F::Bgra8&&image.format!=F::Argb4444)return true;if(!valid(image,rows,bpp))return false;
    u32 left=image.width,right=0;const u32 scanRows=rows<image.height?rows+1:rows;
    for(u32 y=0;y<scanRows;y++)for(u32 x=0;x<image.width;x++){const auto* p=image.pixels.data()+y*image.pitch+x*bpp;if(bpp==2?(read<u16>(p)&0xf000):p[3]){left=std::min(left,x);right=std::max(right,x);}}
    if(left==image.width){std::fill_n(image.pixels.data(),rows*image.pitch,u8(0));return true;}
    left=left?left-1:0;right=std::min(image.width,right+2);
    for(u32 y=0;y<rows;y++){auto* p=image.pixels.data()+y*image.pitch;std::fill_n(p,left*bpp,u8(0));std::fill_n(p+right*bpp,(image.width-right)*bpp,u8(0));}
    for(u32 y=0;y<rows;++y)for(u32 x=left;x<right;++x){auto* out=image.pixels.data()+y*image.pitch+x*bpp;if(bpp==2?(read<u16>(out)&0xf000):out[3])continue;u32 sum[3]={0,0,0},count=0;
        auto collect=[&](const u8* in){if(bpp==2){const u16 v=read<u16>(in);if(!(v&0xf000))return;sum[0]+=v&15;sum[1]+=(v>>4)&15;sum[2]+=(v>>8)&15;}else{if(!in[3])return;for(u32 c=0;c<3;++c)sum[c]+=in[c];}++count;};
        if(x)collect(out-bpp);if(x+1<image.width)collect(out+bpp);if(y)collect(out-image.pitch);if(y+1<image.height)collect(out+image.pitch);if(count>1)for(auto& v:sum)v/=count;
        if(bpp==2)put(out,u16((sum[0]>>1)|((sum[1]>>1)<<4)|((sum[2]>>1)<<8)));else for(u32 c=0;c<3;++c)out[c]=u8(sum[c]);
    }return true;
}
bool TextRaster::rasterize(const std::string& text,const TextStyle& input){
    const i32 height=std::max(17,input.height);if(height>58||text.size()>1024)return false;const u32 rows=height*2+12,font=font_index(input.font,input.fallback);
    std::fill(scratch.pixels.begin(),scratch.pixels.end(),0);if(!invert_alpha(scratch,rows))return false;
    auto draw=[&](const std::string& value,i32 x,bool spaced){
        if(input.shadow){if(spaced){for(const auto delta:{Vec2{2,4},Vec2{-2,4},Vec2{2,0},Vec2{-2,0}})if(!glyphs.draw(scratch,x+i32(delta.x),i32(delta.y),font,input.outline,value))return false;}
        else for(const auto delta:{Vec2{1,1},Vec2{-1,1},Vec2{1,3},Vec2{-1,3},Vec2{2,2},Vec2{-2,2},Vec2{0,0},Vec2{0,4}})if(!glyphs.draw(scratch,x+i32(delta.x),i32(delta.y),font,input.outline,value))return false;}
        return glyphs.draw(scratch,x,2,font,input.color,value);};
    if(input.spacing){i32 x=input.offset;for(u32 i=0;i<text.size();i+=2){if(!draw(text.substr(i,2),x,true))return false;x+=input.spacing;}}else if(!draw(text,input.offset+2,false))return false;
    return invert_alpha(scratch,rows)&&expand_alpha(scratch,rows)&&bleed(scratch,rows);
}
bool TextRaster::write(TextureImage& image,const TextRect& rect,const std::string& text,const TextStyle& style){
    if(style.height>0&&style.height<=8)return true;if(!rasterize(text,style))return false;
    if(image.format!=scratch.format||rect.left<0||rect.top<0||rect.right<=rect.left||rect.bottom<=rect.top||rect.right>i32(image.width)||rect.bottom>i32(image.height)||image.pitch<image.width*2||image.pixels.size()<u64(image.pitch)*image.height)return false;
    const u32 width=rect.right-rect.left,height=rect.bottom-rect.top,sourceWidth=std::min(1024u,width),offset=style.fallback&&style.font!=2&&style.font!=6?6:0;if(height+offset>scratch.height)return false;
    // Point sampling uses the original source rectangle, not a smooth downscale.
    for(u32 y=0;y<height;++y)for(u32 x=0;x<width;++x){const u32 sx=u32((u64(x)*2+1)*sourceWidth/(u64(width)*2));std::memcpy(image.pixels.data()+(rect.top+y)*image.pitch+(rect.left+x)*2,scratch.pixels.data()+(y+offset)*scratch.pitch+sx*2,2);}return true;
}
}
