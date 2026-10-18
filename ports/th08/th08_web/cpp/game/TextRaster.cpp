#include "TextRaster.hpp"
namespace th08 {
bool TextRasterBuffer::create(i32 w,i32 h,u32 fmt){
    const auto description=TexturePixels::describe(fmt);
    if(w<=0||h<=0||w>16384||h>16384||fmt<21||fmt>26||!description.bytes)return false;
    const u32 row=(u32(w)*description.bytes+3)&~3u;if(u64(row)*(u32(h)+1)>256*1024*1024)return false;
    width=w;height=h;format=fmt;pitch=row;pixels.assign(u64(pitch)*(height+1),0);return true;
}
bool TextRasterBuffer::invert_alpha(i32 x,i32 y,i32 sprite_width,i32 font_height,bool soft){
    (void)x;const i64 area=i64(sprite_width)*font_height*2,offset=i64(y)*sprite_width*2;
    if(area<0||offset<0||u64(offset+area)>pixels.size())return false;
    auto* region=pixels.data()+offset;
    if(format==21){for(i64 i=3;i<area;i+=4)region[i]^=255;return true;}
    if(format==26){for(i64 i=1;i<area;i+=2)region[i]^=240;return true;}
    if(format!=25)return false;
    for(i32 i=0;i<area;i+=2){u16 pixel;std::memcpy(&pixel,region+i,2);pixel^=0x8000;
        if(!(pixel&0x8000)){region[i]=region[i+1]=0;continue;}
        i32 r=(pixel>>10)&31,g=(pixel>>5)&31,b=pixel&31;
        const i32 multiplier=!soft&&r>=b?2:1,divisor=soft?4:r>=b?3:2;
        const auto shade=[&](i32 n){return n-i32(u32(n)*u32(i)*u32(multiplier))/i32(area)/divisor;};
        if(r>=b)r=shade(r);else b=shade(b);g=shade(g);
        pixel=0x8000|((r>=32?31:r)&31)<<10|((g>=32?31:g)&31)<<5|((b>=32?31:b)&31);std::memcpy(region+i,&pixel,2);
    }
    return true;
}
bool TextRaster::render(PixelSurface& output,i32 x,i32 y,i32 sprite_width,i32 sprite_height,i32 font_height,i32 font_width,u32 color,u32 outline,const u8* text,u32 size,bool bold){
    (void)sprite_height;if(font_height<=0||font_height>32||font_width<=0||sprite_width<=0||sprite_width>1024||x<-16384||x>16384)return false;
    const auto* font=fonts.find(font_height*2-(bold?2:0),bold?600:400);if(!font)return false;
    if(!scratch.create(1024,64,25))return false;
    auto buffer=scratch.surface();const i32 shade_width=bold?sprite_width*2:1024,shade_height=font_height*2+6;
    if(!scratch.invert_alpha(0,0,shade_width,shade_height,false))return false;
    const i32 center=x*2+2,shift=outline==0xffffffff?1:2;
    if(!fonts.draw(*font,buffer,center+shift,2,0,text,size)||!fonts.draw(*font,buffer,center-shift,2,0,text,size)||
       !fonts.draw(*font,buffer,center,2-shift,0,text,size)||!fonts.draw(*font,buffer,center,2+shift,0,text,size)||
       !fonts.draw(*font,buffer,center,2,color,text,size))return false;
    if(!scratch.invert_alpha(0,0,shade_width,shade_height,outline==0xffffffff))return false;
    const i32 source_width=sprite_width*2>1024?1024:sprite_width*2;
    return TextureResample::triangle(output,{0,y,sprite_width,y+font_width},buffer,{0,0,source_width,font_height*2})==0;
}
}
