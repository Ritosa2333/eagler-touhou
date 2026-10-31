#include "Anm.hpp"
#include <cmath>
#include <cstring>

namespace th12host {
namespace {
u16 half(const std::vector<u8>& b,size_t at){return at+2<=b.size()?u16(b[at]|b[at+1]<<8):0;}
u32 word(const std::vector<u8>& b,size_t at){u32 v=0;if(at+4<=b.size())std::memcpy(&v,&b[at],4);return v;}
float real(const std::vector<u8>& b,size_t at){float v=0;if(at+4<=b.size())std::memcpy(&v,&b[at],4);return v;}

bool decode_texture(const std::vector<u8>& b,size_t at,AnmEntry& e,std::string& error){
    if(at+16>b.size()){error="THTX header out of range";return false;}
    const u32 format=half(b,at+6),w=half(b,at+8),h=half(b,at+10),size=word(b,at+12);
    if(at+16+u64(size)>b.size()){error="THTX data out of range";return false;}
    const u8* raw=&b[at+16];const u32 pixels=w*h;
    const u32 tw=std::max(w,e.width),th=std::max(h,e.height);e.texture_width=tw;e.texture_height=th;
    e.rgba.assign(size_t(tw)*th*4,0);
    if(e.format==3)for(size_t i=3;i<e.rgba.size();i+=4)e.rgba[i]=255;
    const auto row=[&](u32 y){return &e.rgba[size_t(y)*tw*4];};
    if(format==1){if(size<pixels*4){error="THTX too short";return false;}
        for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){const u8* p=raw+(y*w+x)*4;u8* q=row(y)+x*4;q[0]=p[2];q[1]=p[1];q[2]=p[0];q[3]=p[3];}}
    else if(format==3||format==5){if(size<pixels*2){error="THTX too short";return false;}
        for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){const u16 v=u16(raw[(y*w+x)*2]|raw[(y*w+x)*2+1]<<8);u8* q=row(y)+x*4;
            if(format==5){q[0]=u8(((v>>8)&15)*17);q[1]=u8(((v>>4)&15)*17);q[2]=u8((v&15)*17);q[3]=u8(((v>>12)&15)*17);}
            else{q[0]=u8(((v>>11)&31)*255/31);q[1]=u8(((v>>5)&63)*255/63);q[2]=u8((v&31)*255/31);q[3]=255;}}}
    else if(format==7){if(size<pixels){error="THTX too short";return false;}
        // D3DFMT_A8: sampled RGB is zero, not legacy OpenGL ALPHA's white.
        for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){u8* q=row(y)+x*4;q[0]=q[1]=q[2]=0;q[3]=raw[y*w+x];}}
    else{error="unsupported texture format "+std::to_string(format);return false;}
    return true;
}
}

bool parse_anm(const std::string& name,const std::vector<u8>& b,AnmFile& out,std::string& error,bool keep_pixels){
    out=AnmFile{};out.name=name;size_t base=0;
    for(u32 guard=0;guard<4096;++guard){
        if(base+64>b.size()||word(b,base)!=7){error=name+": expected ANM v7";return false;}
        const u32 ns=half(b,base+4),nc=half(b,base+6),name_at=word(b,base+16),td=word(b,base+28),next=word(b,base+36);
        AnmEntry e;e.width=half(b,base+10);e.height=half(b,base+12);e.format=half(b,base+14);
        e.offset_x=half(b,base+20);e.offset_y=half(b,base+22);
        for(size_t p=base+name_at;p<b.size()&&b[p];++p)e.name+=char(b[p]);
        const i32 entry=i32(out.entries.size());
        if(td&&half(b,base+32)){e.has_image=true;
            if(keep_pixels&&!decode_texture(b,base+td,e,error)){error=name+": "+error;return false;}
            if(!keep_pixels){e.texture_width=std::max<u32>(half(b,base+td+8),e.width);e.texture_height=std::max<u32>(half(b,base+td+10),e.height);}}
        if(base+64+u64(ns)*4+u64(nc)*8>b.size()){error=name+": tables out of range";return false;}
        for(u32 i=0;i<ns;++i){const size_t p=base+word(b,base+64+i*4);
            out.sprites.push_back({entry,real(b,p+4),real(b,p+8),real(b,p+12),real(b,p+16)});}
        const size_t table=base+64+size_t(ns)*4;
        const size_t limit=base+(td?td:next?next:b.size()-base);
        for(u32 i=0;i<nc;++i){
            AnmScript s;s.source_id=i32(word(b,table+i*8));s.entry=entry;const u32 offset=word(b,table+i*8+4);
            size_t p=base+offset;
            for(u32 steps=0;p+8<=limit&&p+8<=b.size();++steps){
                const u16 op=half(b,p),length=half(b,p+2);if(op==65535||length<8)break;
                if(p+length>limit||p+length>b.size()){error=name+": ANM instruction crosses entry";return false;}
                if(steps>100000){error=name+": ANM script limit";return false;}
                AnmCommand c;c.offset=u32(p-base-offset);c.op=op;c.time=i16(half(b,p+4));c.mask=half(b,p+6);
                for(size_t q=p+8;q+4<=p+length;q+=4){c.ints.push_back(i32(word(b,q)));const float f=real(b,q);c.floats.push_back(std::isfinite(f)?f:0.f);}
                s.offsets.emplace(c.offset,u32(s.commands.size()));s.commands.push_back(std::move(c));p+=length;
            }
            out.scripts.push_back(std::move(s));
        }
        out.entries.push_back(std::move(e));
        if(!next){
            // Pieces of one source image must sample their own boundary at
            // enlarged raster scales, rather than wrapping to the opposite edge.
            for(auto& entry:out.entries)for(const auto& other:out.entries)if(entry.name==other.name){
                if(entry.offset_x!=other.offset_x)entry.address_flags&=~1u;
                if(entry.offset_y!=other.offset_y)entry.address_flags&=~2u;
            }
            return true;
        }
        base+=next;if(base>=b.size()){error=name+": ANM entry offset out of bounds";return false;}
    }
    error=name+": too many ANM entries";return false;
}
}
