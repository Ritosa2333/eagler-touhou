#include "AnmResource.hpp"
#include <cmath>
#include <cstdint>
#include <utility>
namespace th13 {
namespace {
template<class T> T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
bool span(u32 offset,u32 length,u32 size){return offset<=size&&length<=size-offset;}
u32 pixel_size(u16 f){return f==1?4:f==2||f==3||f==5||f==6?2:f==4?3:f==7?1:0;}
struct ContentIdentity {
    u64 value=0xcbf29ce484222325ull;
    void byte(u8 v)noexcept{value=(value^v)*0x100000001b3ull;}
    void number(u64 v)noexcept{for(unsigned shift=0;shift<64;shift+=8)byte(u8(v>>shift));}
    void real(float v)noexcept{u32 bits;std::memcpy(&bits,&v,sizeof bits);number(bits);}
    void bytes(const u8* data,size_t size)noexcept{number(size);for(size_t i=0;i<size;++i)byte(data[i]);}
};
template<class T,class M>i32 table_index(const std::vector<T>& table,const M* member,const M* first)noexcept{
    if(!member)return -1;
    if(table.empty())return -2;
    const auto address=reinterpret_cast<std::uintptr_t>(member);
    const auto base=reinterpret_cast<std::uintptr_t>(first);
    if(address<base)return -2;
    const auto offset=address-base,index=offset/sizeof(T);
    return offset%sizeof(T)==0&&index<table.size()?i32(index):-2;
}
}
namespace {
// 单文件 entry 链解析（open 与 append 共用）：内容追加到 target，
// sprite 的纹理索引加 texture_base。返回 false 即整体无效。
bool parse_entries(const u8* data,u32 size,AnmResource& target){
    if(!data)return false;
    const u32 sprite_base=u32(target.sprites.size()),script_base=u32(target.scripts.size());
    u32 base=0;
    do {
        if(!span(base,64,size))return false;
        const u8* h=data+base;if(read<u32>(h)!=8)return false;
        const u32 stride=read<u32>(h+36),chunk_size=stride?stride:size-base;
        if(chunk_size<64||!span(base,chunk_size,size))return false;
        const u32 ns=read<u16>(h+4),nc=read<u16>(h+6),table_end=64+ns*4+nc*8;
        if(table_end>chunk_size)return false;
        const u32 name=read<u32>(h+16);if(name<table_end||name>=chunk_size)return false;
        u32 end=name;while(end<chunk_size&&h[end])++end;if(end==chunk_size)return false;
        AnmTexture texture;texture.name.assign(reinterpret_cast<const char*>(h+name),end-name);
        texture.width=read<u16>(h+10);texture.height=read<u16>(h+12);texture.format=read<u16>(h+14);
        texture.low_resolution_scale=read<u16>(h+34)!=0;
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
        const u32 texture_index=target.textures.size();
        for(u32 i=0;i<ns;++i){
            const u32 off=read<u32>(h+64+i*4);if(off<table_end||!span(off,20,chunk_size))return false;
            AnmSprite sprite;sprite.source_id=read<u32>(h+off);sprite.texture=texture_index;
            sprite.x=read<float>(h+off+4);sprite.y=read<float>(h+off+8);sprite.width=read<float>(h+off+12);sprite.height=read<float>(h+off+16);
            if(!std::isfinite(sprite.x)||!std::isfinite(sprite.y)||!std::isfinite(sprite.width)||!std::isfinite(sprite.height))return false;
            sprite.u0=float(double(sprite.x)/texture.width);sprite.v0=float(double(sprite.y)/texture.height);
            const float right=float(double(sprite.x)+sprite.width),bottom=float(double(sprite.y)+sprite.height);
            sprite.u1=float(double(right)/texture.width);sprite.v1=float(double(bottom)/texture.height);
            target.sprites.push_back(sprite);
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
            script.sprite_base=sprite_base;script.script_base=script_base;
            script.bytes.assign(h+off,h+cursor);target.scripts.push_back(std::move(script));
        }
        target.textures.push_back(std::move(texture));
        if(!stride)break;base+=stride;
    }while(true);
    return true;
}
}

bool AnmResource::open(const u8* data,u32 size){
    textures.clear();sprites.clear();scripts.clear();content_identity_=0;
    AnmResource next;
    if(!parse_entries(data,size,next))return false;
    next.refresh_identity();*this=std::move(next);return true;
}

bool AnmResource::append(const u8* data,u32 size){
    if(textures.empty()&&sprites.empty()&&scripts.empty())return open(data,size);
    if(!parse_entries(data,size,*this))return false;
    refresh_identity();
    return true;
}

u64 anm_script_identity(const std::vector<u8>& bytes)noexcept{
    ContentIdentity id;id.number(0x414e4d5343524950ull);id.bytes(bytes.data(),bytes.size());return id.value;
}
u64 AnmResource::content_identity()const noexcept{
    if(!content_identity_)refresh_identity();
    return content_identity_;
}
void AnmResource::refresh_identity()const noexcept{
    ContentIdentity id;id.number(0x414e4d5245530001ull);
    id.number(textures.size());
    for(const auto& t:textures){
        id.bytes(reinterpret_cast<const u8*>(t.name.data()),t.name.size());
        id.number(u32(t.kind));id.number(t.width);id.number(t.height);id.number(t.format);
        id.number(t.pixel_width);id.number(t.pixel_height);id.number(t.pixel_format);
        id.number(u16(t.source_x));id.number(u16(t.source_y));id.number(t.priority);
        id.number(t.low_resolution_scale);id.bytes(t.pixels.data(),t.pixels.size());
    }
    id.number(sprites.size());
    for(const auto& s:sprites){
        id.number(s.source_id);id.number(s.texture);
        id.real(s.x);id.real(s.y);id.real(s.width);id.real(s.height);
        id.real(s.u0);id.real(s.v0);id.real(s.u1);id.real(s.v1);
    }
    id.number(scripts.size());
    for(const auto& s:scripts){
        id.number(u32(s.source_id));id.number(s.file_offset);
        id.number(s.sprite_base);id.number(s.script_base);id.bytes(s.bytes.data(),s.bytes.size());
    }
    content_identity_=id.value;
}
i32 AnmResource::script_index(const std::vector<u8>* script)const noexcept{
    const auto index=table_index(scripts,script,scripts.empty()?nullptr:&scripts[0].bytes);
    return index>=0 && &scripts[u32(index)].bytes!=script ? -2 : index;
}
i32 AnmResource::sprite_index(const AnmSprite* sprite)const noexcept{
    const auto index=table_index(sprites,sprite,sprites.data());
    return index>=0 && &sprites[u32(index)]!=sprite ? -2 : index;
}
bool AnmTexture::rgba(std::vector<u8>& out) const {
    out.clear();if(kind!=Kind::Embedded)return false;
    const u32 n=u32(pixel_width)*pixel_height,bpp=pixel_size(pixel_format);
    if(!bpp||u64(n)*bpp!=pixels.size())return false;
    out.resize(size_t(n)*4);
    for(u32 i=0;i<n;++i){
        const auto* p=pixels.data()+i*bpp;auto* q=out.data()+i*4;
        if(pixel_format==1||pixel_format==4){q[0]=p[2];q[1]=p[1];q[2]=p[0];q[3]=pixel_format==1?p[3]:255;}
        // TH15 format table at 0x4cb568 maps file format 7 to alpha-only A8.
        else if(pixel_format==6){const u16 value=read<u16>(p);q[0]=u8((((value>>5)&7)*255+3)/7);q[1]=u8((((value>>2)&7)*255+3)/7);q[2]=u8((value&3)*85);q[3]=u8(value>>8);}
        else if(pixel_format==7){q[0]=q[1]=q[2]=255;q[3]=p[0];}
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
