#include "GlyphAtlas.hpp"
namespace th15 {
namespace {template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}}
bool GlyphAtlas::load(u32 slot,const u8* data,u32 size){
    if(!data)return false;
    if(slot==13){if(size!=131072)return false;encoding.assign(data,data+size);return true;}
    if(slot==14){if(size<8192||size>1048576||size%4096)return false;for(u32 i=0;i<size;++i)if(data[i]>15)return false;blend.assign(data,data+size);return true;}
    if(slot>=13||size<32||std::memcmp(data,"T15G",4)||read<u32>(data+4)!=3||read<u32>(data+8)!=slot||read<u32>(data+24)!=size)return false;
    const u32 bits=read<u32>(data+28);if(bits<1||bits>8)return false;
    const u32 count=read<u32>(data+16),begin=read<u32>(data+20);if(count>65536||begin!=32+count*20||begin>size)return false;
    u32 previous=0;
    for(u32 n=0;n<count;++n){const auto* p=data+32+n*20;const u32 code=read<u32>(p),at=read<u32>(p+4),w=read<u16>(p+14),h=read<u16>(p+16);
        if(code>65535||(n&&code<=previous)||at<begin||at>size||(u64(w)*h*3*bits+7)/8>size-at||w>128||h>128)return false;previous=code;
    }
    decoded[slot].clear();fonts[slot].assign(data,data+size);return true;
}
bool GlyphAtlas::ready()const{return !encoding.empty()&&!blend.empty();}
bool GlyphAtlas::draw(TextureImage& image,i32 x,i32 y,u32 font,u32 color,const std::string& text){
    if(!ready()||font>=13||fonts[font].empty()||image.format!=touhou::graphics::PixelFormat::Argb4444||image.pitch<image.width*2||image.pixels.size()<u64(image.pitch)*image.height)return false;
    const auto& f=fonts[font];const u32 count=read<u32>(f.data()+16),bits=read<u32>(f.data()+28),mask=(1u<<bits)-1;const u8* p=reinterpret_cast<const u8*>(text.data()),*end=p+text.size();
    while(p<end&&*p){u32 bytes=*p++;if((bytes>=0x81&&bytes<=0x9f)||(bytes>=0xe0&&bytes<=0xfc)){if(p<end&&*p)bytes=(bytes<<8)|*p++;}
        u32 code=read<u16>(encoding.data()+bytes*2),lo=0,hi=count;
        for(u32 attempt=0;attempt<2;++attempt){lo=0;hi=count;while(lo<hi){u32 mid=(lo+hi)/2;if(read<u32>(f.data()+32+mid*20)<code)lo=mid+1;else hi=mid;}if(lo<count&&read<u32>(f.data()+32+lo*20)==code)break;code=0x30fb;}
        if(lo>=count||read<u32>(f.data()+32+lo*20)!=code)return false;
        const auto* g=f.data()+32+lo*20;const u32 at=read<u32>(g+4),w=read<u16>(g+14),h=read<u16>(g+16);const i32 left=x+read<i16>(g+10),top=y+read<i16>(g+12);
        auto& cache=decoded[font];if(cache.size()>=256&&cache.find(lo)==cache.end())cache.clear();auto& pixels=cache[lo];
        if(pixels.empty()&&w&&h){pixels.resize(w*h*3);for(u32 n=0;n<w*h*3;n++){const u32 bit=n*bits,byte=at+(bit>>3),shift=bit&7;const u32 value=f[byte]|(byte+1<f.size()?u32(f[byte+1])<<8:0);pixels[n]=(value>>shift)&mask;if((u32(pixels[n])+1)*4096>blend.size())return false;}}
        for(u32 gy=0;gy<h;++gy){const i32 dy=top+i32(gy);if(dy<0||dy>=i32(image.height))continue;for(u32 gx=0;gx<w;++gx){const i32 dx=left+i32(gx);const u8* coverage=pixels.data()+(gy*w+gx)*3;if((!coverage[0]&&!coverage[1]&&!coverage[2])||dx<0||dx>=i32(image.width))continue;for(u32 channel=0;channel<3;++channel)if((coverage[channel]+1)*4096>blend.size())return false;
            auto* to=image.pixels.data()+dy*image.pitch+dx*2;const u32 before=read<u16>(to);
            const u16 result=u16((blend[coverage[2]*4096+((before>>8)&15)*256+(color&255)]<<8)|(blend[coverage[1]*4096+((before>>4)&15)*256+((color>>8)&255)]<<4)|blend[coverage[0]*4096+(before&15)*256+((color>>16)&255)]);std::memcpy(to,&result,2);
        }}
        x+=read<i16>(g+8);
    }return true;
}
}
