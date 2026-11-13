#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
#if PORT_TITLE == 14
#include <memory>
#endif
namespace th11 {
#if PORT_TITLE == 14
struct AnmTemplatesTh14;
#endif
struct AnmInstruction {
    i16 opcode; u16 length; i16 time; u16 references;
    template<class T> T argument(u32 index) const noexcept {
        T value; std::memcpy(&value,reinterpret_cast<const u8*>(this)+8+index*4,sizeof value); return value;
    }
};
static_assert(sizeof(AnmInstruction)==8);
struct AnmSprite {
    u32 source_id=0,texture=0;
    float x=0,y=0,width=0,height=0;
    float u0=0,v0=0,u1=0,v1=0;
};
struct AnmTexture {
    enum class Kind {Embedded,External,Blank,RenderTarget};
    std::string name;
    Kind kind=Kind::Embedded;
    u16 width=0,height=0,format=0,pixel_width=0,pixel_height=0,pixel_format=0;
    i16 source_x=0,source_y=0;
    u32 priority=0;
#if PORT_TITLE == 14
    bool half_resolution=false;
#endif
    // Where this texture's scripts begin in `scripts`. A chunk declares `nc` scripts, and the loader appends them
    // per chunk in table order, so a texture owns a run of slots. The title compositor binds the render-target
    // textures' first scripts, which is 81 and 82 on th12 and 82 and 83 on th13: the values differ because
    // th13's second texture declares one script more (7 against 6), so the slot has to come from the file.
    u32 first_script=0,script_count=0;
    std::vector<u8> pixels;
    bool rgba(std::vector<u8>& output) const;
};
struct AnmScript {
    i32 source_id=0;
    u32 file_offset=0;
    std::vector<u8> bytes;
    // A hash of the bytes as the loader left them. A script's bytes are read by an interpreter
    // that reaches them through a raw pointer, so a report that compares this with the bytes at that
    // pointer says which side moved: the loader wrote them, or the session did.
    u32 load_hash=0;
};
// FNV-1a over a byte range; the two sides of a comparison have to use one function.
inline u32 anm_bytes_hash(const u8* bytes,u32 count)noexcept{
    u32 hash=2166136261u;
    for(u32 i=0;i<count;++i){hash^=bytes[i];hash*=16777619u;}
    return hash;
}
// ANM v7 only. Identifiers in the file are metadata; the original loader
// appends sprites and scripts in table order across chained chunks.
class AnmResource {
public:
#if PORT_TITLE == 14
    std::shared_ptr<AnmTemplatesTh14> templates;
#endif
    std::vector<AnmTexture> textures;
    std::vector<AnmSprite> sprites;
    std::vector<AnmScript> scripts;
    // Diagnostic for the first structural failure; 0 means the last open() succeeded.
    int last_error=0;
    // Set together with it when the title's own ANM numbering has an opcode the measured table has no row for,
    // so the caller reporting the failure can name it.
    std::string detail;
    bool open(const u8* bytes,u32 size);
};
}
