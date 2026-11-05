#pragma once
#include "AnmVm.hpp"
namespace th10 {
struct AnmResourceEnvironment;
struct AnmTextureEnvironment;
struct TexturePlatform;
struct AnmChunk;
struct AnmTexture {
    void* handle;u8* source;u32 source_size,bytes_per_pixel;
    i32 create_empty(i32 width,i32 height,i32 format,TexturePlatform& platform);
    i32 create_encoded(i32 width,i32 height,i32 format,u32 color_key,TexturePlatform& platform);
    i32 create_embedded(const u8* data,i32 width,i32 height,i32 format,TexturePlatform& platform);
    void repair_edges(TexturePlatform& platform);
};
static_assert(sizeof(AnmTexture)==16);
struct AnmSprite {
    u32 file_index;
    void* texture;
    float left,top,right,bottom;
    float texture_height,texture_width;
    float u0,v0,u1,v1;
    float height,width;
    float scale_x,scale_y;
    u32 reserved_40;
};
static_assert(sizeof(AnmSprite)==0x44);
struct AnmFile {
    i32 file_index;
    char name[260];
    u8* loaded;
    i32 texture_count,script_count,sprite_count;
    AnmSprite* sprites;
    AnmInstruction** scripts;
    AnmTexture* textures;
    u32 unavailable;
    u32 discard_request;
    u8* extra_data;
    i32 prepare_chunk(i32 index,const AnmChunk* chunk,AnmResourceEnvironment& environment);
    bool complete_next(AnmResourceEnvironment& environment);
    void release(AnmResourceEnvironment& environment);
    void set_sprite(i32 index,const AnmSprite& sprite) noexcept;
    i32 materialize(i32 texture,i32 first_sprite,i32 first_script,const AnmChunk* chunk,AnmTextureEnvironment& environment);
    i32 bind_sprite(AnmVm& vm,i32 index) noexcept;
    void start_script(AnmVm& vm,i32 index,AnmEnvironment& environment,u32& started_scripts);
    void initialize_script(AnmVm& vm,i32 index,AnmEnvironment& environment,u32& started_scripts);
    void bind_script(AnmVm& vm,i32 index,AnmEnvironment& environment,u32& started_scripts);
    void prepare_script(AnmVm& vm,i32 index,AnmEnvironment& environment,u32& started_scripts);
};
static_assert(offsetof(AnmFile,sprites)==0x118);
static_assert(offsetof(AnmFile,unavailable)==0x124);
static_assert(sizeof(AnmFile)==0x130);
void initialize_embedded_animation(AnmFile& file,AnmVm& vm,i32 script,AnmEnvironment& environment,u32& started);
struct AnmVertex {Vec3 position;float reciprocal_w;u32 color;Vec2 uv;};
static_assert(sizeof(AnmVertex)==28);
}
