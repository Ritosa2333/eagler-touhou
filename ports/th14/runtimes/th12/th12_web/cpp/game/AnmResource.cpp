#include "AnmResource.hpp"
#include "AnmOpcodes.hpp"
#include "PortTitle.hpp"
#include <cmath>
#include <utility>
namespace th11 {
namespace {
// The first word of an ANM chunk is its format marker, and the marker is the title's: th12's ascii.anm starts
// 7, th13's starts 8 (measured, 179084 and 311192 bytes, the same header shape otherwise). The port began with
// TH12 and accepted 7 only, so loading th13's own ascii.anm failed at the first check and the failure surfaced as
// "ascii ANM" before the title screen. th14..th18 name their own marker when their archives are opened.
constexpr u32 anm_format = kPortTitle==12?7u:8u;
template<class T> T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
bool span(u32 offset,u32 length,u32 size){return offset<=size&&length<=size-offset;}
// THTX pixel formats. The original enum is the one thtk documents for ANM v2+:
// 1 = BGRA8888, 3 = RGB565, 5 = ARGB4444, 7 = GRAY8 (luminance). 2 and 4 are the
// older 16/24-bit variants still seen in th06-th09 archives.
u32 pixel_size(u16 f){
    switch(f){
        case 1: return 4;
        case 2: case 3: case 5: return 2;
        case 4: return 3;
        case 7: return 1;
        default: return 0;
    }
}
}
bool AnmResource::open(const u8* data,u32 size){
    textures.clear();sprites.clear();scripts.clear();last_error=0;detail.clear();
#define ANM_FAIL(code) do{last_error=(code);return false;}while(0)
    if(!data)ANM_FAIL(1);
    AnmResource next;u32 base=0;u32 chunks=0;
    do {
        if(!span(base,64,size))ANM_FAIL(2);
        const u8* h=data+base;if(read<u32>(h)!=anm_format)ANM_FAIL(3);
        const u32 stride=read<u32>(h+36),chunk_size=stride?stride:size-base;
        if(chunk_size<64||!span(base,chunk_size,size))ANM_FAIL(4);
        const u32 ns=read<u16>(h+4),nc=read<u16>(h+6),table_end=64+ns*4+nc*8;
        if(table_end>chunk_size)ANM_FAIL(5);
        const u32 name=read<u32>(h+16);if(name<table_end||name>=chunk_size)ANM_FAIL(6);
        u32 end=name;while(end<chunk_size&&h[end])++end;if(end==chunk_size)ANM_FAIL(7);
        AnmTexture texture;texture.name.assign(reinterpret_cast<const char*>(h+name),end-name);
        texture.width=read<u16>(h+10);texture.height=read<u16>(h+12);texture.format=read<u16>(h+14);
        texture.source_x=read<i16>(h+20);texture.source_y=read<i16>(h+22);texture.priority=read<u32>(h+24);
#if PORT_TITLE == 14
        texture.half_resolution=h[34]!=0;
#endif
        if(!texture.width||!texture.height)ANM_FAIL(8);
        const u32 tex=read<u32>(h+28);
        if(h[32]){
            if(!span(tex,16,chunk_size)||std::memcmp(h+tex,"THTX",4))ANM_FAIL(9);
            texture.pixel_format=read<u16>(h+tex+6);texture.pixel_width=read<u16>(h+tex+8);texture.pixel_height=read<u16>(h+tex+10);
            const u32 bytes=read<u32>(h+tex+12),bpp=pixel_size(texture.pixel_format);
            if(!bpp||!texture.pixel_width||!texture.pixel_height||!span(tex+16,bytes,chunk_size))ANM_FAIL(10);
            if(u64(texture.pixel_width)*texture.pixel_height*bpp!=bytes)ANM_FAIL(11);
            texture.pixels.assign(h+tex+16,h+tex+16+bytes);
        }else if(texture.name=="@R")texture.kind=AnmTexture::Kind::RenderTarget;
        else if(!texture.name.empty()&&texture.name[0]=='@')texture.kind=AnmTexture::Kind::Blank;
        else texture.kind=AnmTexture::Kind::External;
        const u32 texture_index=next.textures.size();
        // The run of script slots this texture owns starts where the list stands now; the chunk's `nc` scripts are
        // appended below, so the start has to be taken before the loop and not after it.
        texture.first_script=u32(next.scripts.size());texture.script_count=nc;
        for(u32 i=0;i<ns;++i){
            const u32 off=read<u32>(h+64+i*4);if(off<table_end||!span(off,20,chunk_size))ANM_FAIL(12);
            AnmSprite sprite;sprite.source_id=read<u32>(h+off);sprite.texture=texture_index;
            sprite.x=read<float>(h+off+4);sprite.y=read<float>(h+off+8);sprite.width=read<float>(h+off+12);sprite.height=read<float>(h+off+16);
            if(!std::isfinite(sprite.x)||!std::isfinite(sprite.y)||!std::isfinite(sprite.width)||!std::isfinite(sprite.height))ANM_FAIL(13);
            sprite.u0=float(double(sprite.x)/texture.width);sprite.v0=float(double(sprite.y)/texture.height);
            const float right=float(double(sprite.x)+sprite.width),bottom=float(double(sprite.y)+sprite.height);
            sprite.u1=float(double(right)/texture.width);sprite.v1=float(double(bottom)/texture.height);
            next.sprites.push_back(sprite);
        }
        for(u32 i=0;i<nc;++i){
            const u32 entry=64+ns*4+i*8,off=read<u32>(h+entry+4);
            if(off<table_end||!span(off,8,chunk_size))ANM_FAIL(14);
            u32 cursor=off;bool terminated=false;
            while(span(cursor,8,chunk_size)){
                const i16 opcode=read<i16>(h+cursor);const u16 length=read<u16>(h+cursor+2);
                if(opcode==-1){cursor+=8;terminated=true;break;}
                if(length<8||length%4||!span(cursor,length,chunk_size))ANM_FAIL(15);
                cursor+=length;
            }
            if(!terminated)ANM_FAIL(16);
            AnmScript script;script.source_id=read<i32>(h+entry);script.file_offset=base+off;
            script.bytes.assign(h+off,h+cursor);
            // The numbering in the bytes is the title's and this interpreter is written in th12's, so each
            // command is translated on the way in. A command the measured table has no row for is named here:
            // the interpreter's default case would skip it without a word.
            for(u32 at=0;at+8<=u32(script.bytes.size());){
                auto* command=reinterpret_cast<AnmInstruction*>(script.bytes.data()+at);
                if(command->opcode==-1)break;
                const u16 title_opcode=u16(command->opcode);
                const u16 canonical=anm_canonical_opcode(title_opcode);
                if(canonical==anm_opcode_unmapped){
                    detail=" (opcode "+std::to_string(title_opcode)+" of the title's numbering has no measured row)";
                    ANM_FAIL(18);
                }
                command->opcode=i16(canonical);
                if(!command->length)break;
                at+=command->length;
            }
            script.load_hash=anm_bytes_hash(reinterpret_cast<const u8*>(script.bytes.data()),
                                          u32(script.bytes.size()));
            next.scripts.push_back(std::move(script));
        }
        next.textures.push_back(std::move(texture));
        if(!stride)break;base+=stride;if(++chunks>4096)ANM_FAIL(17);
    }while(true);
    *this=std::move(next);return true;
#undef ANM_FAIL
}
bool AnmTexture::rgba(std::vector<u8>& out) const {
    out.clear();if(kind!=Kind::Embedded)return false;
    const u32 n=u32(pixel_width)*pixel_height,bpp=pixel_size(pixel_format);
    if(!bpp||u64(n)*bpp!=pixels.size())return false;
    out.resize(size_t(n)*4);
    for(u32 i=0;i<n;++i){
        const auto* p=pixels.data()+i*bpp;auto* q=out.data()+i*4;
        if(pixel_format==1||pixel_format==4){q[0]=p[2];q[1]=p[1];q[2]=p[0];q[3]=pixel_format==1?p[3]:255;}
        else if(pixel_format==7){q[0]=q[1]=q[2]=p[0];q[3]=255;}
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
