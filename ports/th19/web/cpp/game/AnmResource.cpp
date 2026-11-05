#include "AnmResource.hpp"
namespace th19 {
namespace {
u16 rd16(const u8* b){return u16(b[0])|(u16(b[1])<<8);}
u32 rd32(const u8* b){return u32(b[0])|(u32(b[1])<<8)|(u32(b[2])<<16)|(u32(b[3])<<24);}
}
bool parse_anm(const u8* data,u32 size,std::vector<AnmEntry>& entries) {
    entries.clear();
    u32 offset=0;
    while(offset+64<=size){
        const u8* h=data+offset;
        const u32 version=rd32(h);
        if(version!=8)return false;
        AnmEntry e;
        e.base=offset;
        const u16 sprites=rd16(h+4),scripts=rd16(h+6);
        e.w=rd16(h+10);e.h=rd16(h+12);e.format=rd16(h+14);
        e.name_offset=rd32(h+16);e.thtx_offset=rd32(h+0x1c);
        e.has_data=rd16(h+0x20);e.lowres_scale=h[0x22];e.jpeg_quality=h[0x23];
        e.next_offset=rd32(h+0x24);e.w_max=rd16(h+0x28);e.h_max=rd16(h+0x2a);
        // name (Shift-JIS)
        if(e.name_offset<size-offset){const u8* n=data+offset+e.name_offset;const u8* end=data+size;while(n<end&&*n)e.name+=char(*n++);}
        // sprite offsets (sprites*u32 at 0x40), each sprite19_t = id u32 + x,y,w,h float + 5 floats (40 bytes)
        const u8* sp= h+0x40;
        if(uintptr_t(sp)+sprites*4>uintptr_t(data+size))return false;
        for(u16 i=0;i<sprites;++i){
            const u32 so=rd32(sp+i*4);
            if(offset+so+40>size)return false;
            const u8* s=data+offset+so;
            AnmSprite spr;spr.id=rd32(s);
            float x,y,w,h;std::memcpy(&x,s+4,4);std::memcpy(&y,s+8,4);std::memcpy(&w,s+12,4);std::memcpy(&h,s+16,4);
            spr.x=x;spr.y=y;spr.w=w;spr.h=h;
            std::memcpy(&spr.offset_x,s+20,4);std::memcpy(&spr.offset_y,s+24,4);
            std::memcpy(&spr.scale_x,s+28,4);std::memcpy(&spr.scale_y,s+32,4);
            e.sprites.push_back(spr);
        }
        // script table follows sprite-offset array: (i32 id, u32 offset)
        const u8* sc=sp+sprites*4;
        if(uintptr_t(sc)+scripts*8>uintptr_t(data+size))return false;
        for(u16 i=0;i<scripts;++i){
            i32 id;std::memcpy(&id,sc+i*8,4);
            AnmScript s;s.id=id;s.offset=rd32(sc+i*8+4);
            e.scripts.push_back(s);
        }
        // THTX texture at thtx_offset
        if(e.thtx_offset && offset+e.thtx_offset+16<=size){
            const u8* t=data+offset+e.thtx_offset;
            if(t[0]=='T'&&t[1]=='H'&&t[2]=='T'&&t[3]=='X'){
                e.thtx_format=rd16(t+6);e.thtx_w=rd16(t+8);e.thtx_h=rd16(t+10);e.thtx_size=rd32(t+12);
                e.data_offset=offset+e.thtx_offset+16;
            }
        }
        entries.push_back(std::move(e));
        if(!e.next_offset)break;
        offset+=e.next_offset;
        if(offset>=size)break;
    }
    return !entries.empty();
}
}
