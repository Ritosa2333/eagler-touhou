#include "AnmResource.hpp"
#include "Arithmetic.hpp"
namespace th09 {
namespace {
u32 word(const u8* p){return u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24);}
float value(const u8* p){float v;std::memcpy(&v,p,4);return v;}
}
bool AnmResource::load(i32 index,const u8* bytes,u32 size){
    loaded={};raw.clear();sources.clear();sprite_sources.clear();sprites.clear();script_pointers.clear();
    if(index<0||index>=256||size<64||size>64*1024*1024)return false;
    raw.assign(bytes,bytes+size);
    auto fail=[&](){loaded={};sources.clear();sprite_sources.clear();sprites.clear();script_pointers.clear();raw.clear();return false;};
    u32 base=0;
    for(;;){
        if(size-base<64)return fail();const auto* entry=raw.data()+base;
        const u32 ns=word(entry),nt=word(entry+4),next=word(entry+56);
        const u32 span=next?next:size-base;
        if(span<64||span>size-base||ns>100000||nt>100000||u64(ns)*4+u64(nt)*8+64>span||word(entry+40)!=3)return fail();
        AnmTextureSource source;source.width=word(entry+12);source.height=word(entry+16);source.format=word(entry+20);source.color_key=word(entry+24);source.priority=word(entry+44);
        if(!source.width||!source.height||source.width>16384||source.height>16384)return fail();
        const u32 name_offset=word(entry+28);if(name_offset>=span)return fail();u32 end=name_offset;while(end<span&&entry[end])++end;if(end==span)return fail();source.name.assign(reinterpret_cast<const char*>(entry+name_offset),end-name_offset);
        source.embedded=entry[52]!=0;source.empty=!source.embedded&&!source.name.empty()&&source.name[0]=='@';
        if(source.embedded){const u32 offset=word(entry+48);if(offset>span||span-offset<16||std::memcmp(entry+offset,"THTX",4))return fail();const auto* texture=entry+offset;
            source.pixel_format=u32(texture[6])|(u32(texture[7])<<8);source.pixel_width=u32(texture[8])|(u32(texture[9])<<8);source.pixel_height=u32(texture[10])|(u32(texture[11])<<8);source.pixel_size=word(texture+12);source.pixel_offset=base+offset+16;
            if(source.pixel_size>span-offset-16)return fail();
        }
        source.first_sprite=sprite_sources.size();source.sprite_count=ns;
        for(u32 i=0;i<ns;++i){const u32 offset=word(entry+64+i*4);if(offset>span||span-offset<20)return fail();const auto* sprite=entry+offset;
            const AnmSpriteRect rect{value(sprite+4),value(sprite+8),value(sprite+12),value(sprite+16)};
            sprite_sources.push_back({rect.x,rect.y,rect.width,rect.height});source.sprite_rects.push_back(rect);
        }
        for(u32 i=0;i<nt;++i){const u32 offset=word(entry+64+ns*4+i*8+4);if(offset>span||span-offset<8)return fail();
            u32 cursor=offset;bool terminated=false;
            while(cursor<=span-8){const auto* instruction=reinterpret_cast<const AnmRawInstr*>(entry+cursor);if(instruction->opcode==-1){terminated=true;break;}if(instruction->instructionSize<8||instruction->instructionSize>span-cursor)return fail();cursor+=instruction->instructionSize;}
            if(!terminated)return fail();script_pointers.push_back(reinterpret_cast<AnmRawInstr*>(raw.data()+base+offset));
        }
        sources.push_back(std::move(source));if(!next)break;base+=next;
    }
    sprites.resize(sprite_sources.size());loaded.anmIdx=index;loaded.rawData=raw.data();loaded.totalEntries=sources.size();loaded.spriteCount=sprites.size();loaded.scriptCount=script_pointers.size();loaded.sprites=sprites.data();loaded.scripts=script_pointers.data();
    for(u32 i=0;i<sources.size();++i)configure_texture(i,0,sources[i].width,sources[i].height);
    return true;
}
bool AnmResource::configure_texture(u32 index,u32 handle,u32 width,u32 height){
    if(index>=sources.size()||!width||!height)return false;const auto& source=sources[index];
    const float sx=float((double(width)/double(source.width))),sy=float((double(height)/double(source.height)));
    for(u32 i=source.first_sprite;i<source.first_sprite+source.sprite_count;++i){const auto& raw=sprite_sources[i];auto& sprite=sprites[i];sprite={};sprite.anmIdx=loaded.anmIdx;sprite.texture=handle;sprite.width=width;sprite.height=height;sprite.scaleFactor={sx,sy};
        sprite.startPixelInclusive={Scalar::mul(raw.x,sx),Scalar::mul(raw.y,sy)};
        sprite.endPixelInclusive={float(((number(raw.x)+number(raw.width))*number(sx))),float(((number(raw.y)+number(raw.height))*number(sy)))};
        sprite.uvStart={Scalar::div(sprite.startPixelInclusive.x,sprite.width),Scalar::div(sprite.startPixelInclusive.y,sprite.height)};
        sprite.uvEnd={Scalar::div(sprite.endPixelInclusive.x,sprite.width),Scalar::div(sprite.endPixelInclusive.y,sprite.height)};
        sprite.widthPx=float(((number(sprite.endPixelInclusive.x)-number(sprite.startPixelInclusive.x))/number(sx)));sprite.heightPx=float(((number(sprite.endPixelInclusive.y)-number(sprite.startPixelInclusive.y))/number(sy)));
    }
    return true;
}
void AnmResource::clone_from(const AnmResource& source,i32 index){
    loaded=source.loaded;raw=source.raw;sources=source.sources;sprite_sources=source.sprite_sources;sprites=source.sprites;
    script_pointers.resize(source.script_pointers.size());
    for(u32 i=0;i<script_pointers.size();++i)script_pointers[i]=reinterpret_cast<AnmRawInstr*>(raw.data()+(reinterpret_cast<const u8*>(source.script_pointers[i])-source.raw.data()));
    loaded.anmIdx=index;loaded.rawData=raw.data();loaded.sprites=sprites.data();loaded.scripts=script_pointers.data();loaded.textures=nullptr;loaded.numberEntriesToBeLoaded=0;
    for(auto& sprite:sprites){sprite.anmIdx=index;sprite.texture=0;}
}
} // namespace th09
