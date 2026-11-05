#pragma once
#include "sprite.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <span>
namespace th20::source::sprite {
#pragma pack(push,1)
struct AnmHeader {
    std::uint32_t version;
    std::uint16_t sprite_count,script_count;
    std::uint16_t field_08,width,height,format;
    std::uint32_t name_offset;
    std::int16_t x,y;
    std::uint32_t field_18,texture_offset;
    std::uint8_t has_data,padding_21[3];
    std::uint32_t next_offset;
    std::uint16_t original_width,original_height;
    std::uint32_t fields_2c[5];
};
#pragma pack(pop)
static_assert(sizeof(AnmHeader)==64 && offsetof(AnmHeader,next_offset)==0x24);
struct AnmCounts {std::uint32_t textures=0,sprites=0,scripts=0;};
AnmCounts count_animation_entries(std::span<const std::uint8_t>);
AnimationFile* create_animation_file();                    // 4477b0 +448a30
int preload_animation_data(AnimationFile&,const char*,runtime::Log&); // 44a700
int preload_external_texture(AnimationFile&,std::uint32_t,AnmHeader*,runtime::Log&); // 44ebf0
void release_texture_data(TextureRecord&);                 // 44c510
void clear_animation_file(Controller&,AnimationFile&);     // 44ab00
void destroy_animation_file(Controller&,AnimationFile*);   // 4473a0 +449310
void unload_animation_file(Controller&,std::int32_t);       // 44c430
AnimationFile* preload_animation_file(Controller&,std::int32_t,const char*,runtime::Log&); //44ed40
AnimationFile* load_animation_file(Controller&,std::int32_t,const char*,runtime::Log&,std::uint32_t& graphics_flags); //44eb30, waits for main-thread postload
bool animation_files_ready(Controller&,std::uint32_t graphics_flags); //44d880 (AL boolean return)
namespace file_environment {
void detach_file_animations(Controller&,AnimationFile&,bool); //44fd30
void destroy_animation_contents(Animation&);                 //449370
}
}
