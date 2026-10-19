#pragma once
#include "AnmManager.hpp"
namespace th10 {
struct AnmChunk {
    i32 sprite_count,script_count;
    u32 reserved_008;
    i32 width,height,format;
    u32 color_key,name_offset,reserved_020,reserved_024,version,priority,texture_offset;
    u8 embedded_texture,reserved_035[3];
    u32 next_offset,reserved_03c;
    const AnmChunk* next() const noexcept {return reinterpret_cast<const AnmChunk*>(reinterpret_cast<const u8*>(this)+next_offset);}
    const char* texture_name() const noexcept {return reinterpret_cast<const char*>(this)+name_offset;}
};
static_assert(sizeof(AnmChunk)==0x40);
enum class AnmResourceError {InvalidSlot,MissingHeader,InvalidVersion,MissingTexture,EncodedTexture,EmbeddedTexture};
struct AnmTextureDimensions {u32 width,height;};
struct AnmTextureEnvironment {
    virtual i32 create_empty(AnmTexture& texture,i32 width,i32 height,i32 format)=0;
    virtual i32 create_encoded(AnmTexture& texture,i32 width,i32 height,i32 format,u32 color_key)=0;
    virtual i32 create_embedded(AnmTexture& texture,const u8* source,i32 width,i32 height,i32 format)=0;
    // thcrap-style PNG override for an embedded THTX atlas: the platform reads
    // /thcrap/th10/<texture_name>, decodes it to RGBA and alpha-blends it over
    // the surface at the chunk's sprite rectangles.
    virtual bool override_embedded(AnmTexture& texture,const char* texture_name,const AnmChunk* chunk){(void)texture;(void)texture_name;(void)chunk;return false;}
    virtual void set_priority(void* texture,u32 priority)=0;
    virtual void preload(void* texture)=0;
    virtual AnmTextureDimensions dimensions(void* texture)=0;
    virtual void texture_error(AnmResourceError error,const char* name)=0;
};
struct AnmResourceEnvironment {
    AnmRegistry* registry;
    const u32* loader_flags;
    virtual AnmFile* allocate_file()=0;
    virtual void* allocate_bytes(u32 size)=0;
    virtual void release_file(AnmFile* file)=0;
    virtual void release_bytes(void* memory)=0;
    virtual u8* read_file(const char* name,bool external_texture,u32* size)=0;
    virtual void release_texture(void* texture)=0;
    virtual void report(AnmResourceError error)=0;
    virtual i32 materialize(AnmFile& file,i32 texture,i32 sprite,i32 script,const AnmChunk* chunk)=0;
    virtual void wait_for_loading(AnmManager& manager,AnmFile& file)=0;
#ifdef TH_NATIVE_PLATFORM
    virtual AnmFile* prepared_file(i32,const char*){return nullptr;}
#endif
};
}
