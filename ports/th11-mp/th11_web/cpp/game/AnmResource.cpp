#include "AnmResource.hpp"
#include <cmath>
#include <utility>
namespace th11 {
namespace {
template<class T> T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
bool span(u32 offset,u32 length,u32 size){return offset<=size&&length<=size-offset;}
u32 pixel_size(u16 f){return f==1?4:f==2||f==3||f==5?2:f==4?3:0;}
}
bool AnmResource::open(const u8* data,u32 size){
    textures.clear();sprites.clear();scripts.clear();
    if(!data)return false;
    AnmResource next;u32 base=0;
    do {
        if(!span(base,64,size))return false;
        const u8* h=data+base;if(read<u32>(h)!=7)return false;
        const u32 stride=read<u32>(h+36),chunk_size=stride?stride:size-base;
        if(chunk_size<64||!span(base,chunk_size,size))return false;
        const u32 ns=read<u16>(h+4),nc=read<u16>(h+6),table_end=64+ns*4+nc*8;
        if(table_end>chunk_size)return false;
        const u32 name=read<u32>(h+16);if(name<table_end||name>=chunk_size)return false;
        u32 end=name;while(end<chunk_size&&h[end])++end;if(end==chunk_size)return false;
        AnmTexture texture;texture.name.assign(reinterpret_cast<const char*>(h+name),end-name);
        texture.width=read<u16>(h+10);texture.height=read<u16>(h+12);texture.format=read<u16>(h+14);
        texture.source_x=read<i16>(h+20);texture.source_y=read<i16>(h+22);texture.priority=read<u32>(h+24);
        if(!texture.width||!texture.height)return false;
        const u32 tex=read<u32>(h+28);
        if(h[32]){
            if(!span(tex,16,chunk_size)||std::memcmp(h+tex,"THTX",4))return false;
            texture.pixel_format=read<u16>(h+tex+6);texture.pixel_width=read<u16>(h+tex+8);texture.pixel_height=read<u16>(h+tex+10);
            const u32 bytes=read<u32>(h+tex+12),bpp=pixel_size(texture.pixel_format);
            if(!bpp||!texture.pixel_width||!texture.pixel_height||!span(tex+16,bytes,chunk_size))return false;
            if(u64(texture.pixel_width)*texture.pixel_height*bpp!=bytes)return false;
            texture.pixels.assign(h+tex+16,h+tex+16+bytes);
        }else if(texture.name=="@R")texture.kind=AnmTexture::Kind::RenderTarget;
        else if(!texture.name.empty()&&texture.name[0]=='@')texture.kind=AnmTexture::Kind::Blank;
        else texture.kind=AnmTexture::Kind::External;
        const u32 texture_index=next.textures.size();
        for(u32 i=0;i<ns;++i){
            const u32 off=read<u32>(h+64+i*4);if(off<table_end||!span(off,20,chunk_size))return false;
            AnmSprite sprite;sprite.source_id=read<u32>(h+off);sprite.texture=texture_index;
            sprite.x=read<float>(h+off+4);sprite.y=read<float>(h+off+8);sprite.width=read<float>(h+off+12);sprite.height=read<float>(h+off+16);
            if(!std::isfinite(sprite.x)||!std::isfinite(sprite.y)||!std::isfinite(sprite.width)||!std::isfinite(sprite.height))return false;
            sprite.u0=float(double(sprite.x)/texture.width);sprite.v0=float(double(sprite.y)/texture.height);
            const float right=float(double(sprite.x)+sprite.width),bottom=float(double(sprite.y)+sprite.height);
            sprite.u1=float(double(right)/texture.width);sprite.v1=float(double(bottom)/texture.height);
            next.sprites.push_back(sprite);
        }
        for(u32 i=0;i<nc;++i){
            const u32 entry=64+ns*4+i*8,off=read<u32>(h+entry+4);
            if(off<table_end||!span(off,8,chunk_size))return false;
            u32 cursor=off;bool terminated=false;
            while(span(cursor,8,chunk_size)){
                const i16 opcode=read<i16>(h+cursor);const u16 length=read<u16>(h+cursor+2);
                if(opcode==-1){cursor+=8;terminated=true;break;}
                if(length<8||length%4||!span(cursor,length,chunk_size))return false;
                cursor+=length;
            }
            if(!terminated)return false;
            AnmScript script;script.source_id=read<i32>(h+entry);script.file_offset=base+off;
            script.bytes.assign(h+off,h+cursor);next.scripts.push_back(std::move(script));
        }
        next.textures.push_back(std::move(texture));
        if(!stride)break;base+=stride;
    }while(true);
    *this=std::move(next);return true;
}
bool AnmTexture::rgba(std::vector<u8>& out) const {
    out.clear();if(kind!=Kind::Embedded)return false;
    const u32 n=u32(pixel_width)*pixel_height,bpp=pixel_size(pixel_format);
    if(!bpp||u64(n)*bpp!=pixels.size())return false;
    out.resize(size_t(n)*4);
    for(u32 i=0;i<n;++i){
        const auto* p=pixels.data()+i*bpp;auto* q=out.data()+i*4;
        if(pixel_format==1||pixel_format==4){q[0]=p[2];q[1]=p[1];q[2]=p[0];q[3]=pixel_format==1?p[3]:255;}
        else if(pixel_format==5){const u16 v=read<u16>(p);q[0]=u8(((v>>8)&15)*17);q[1]=u8(((v>>4)&15)*17);q[2]=u8((v&15)*17);q[3]=u8((v>>12)*17);}
        else {
            const u16 v=read<u16>(p);
            if(pixel_format==2){q[0]=u8((((v>>10)&31)*255+15)/31);q[1]=u8((((v>>5)&31)*255+15)/31);q[2]=u8(((v&31)*255+15)/31);q[3]=(v&0x8000)?255:0;}
            else {q[0]=u8((((v>>11)&31)*255+15)/31);q[1]=u8((((v>>5)&63)*255+31)/63);q[2]=u8(((v&31)*255+15)/31);q[3]=255;}
        }
    }
    return true;
}
}
