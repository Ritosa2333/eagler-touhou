#pragma once
#include "animation_file.hpp"
#include "../program_entry/device.hpp"
namespace th20::source::sprite {
struct TextureContext {
    Device& device;
    std::int32_t render_width,render_height;
    D3DFORMAT backbuffer_format;
    float screen_scale;
};
int create_embedded_texture(TextureRecord&,const std::uint8_t* thtx,UINT format,
                            std::int32_t width,std::int32_t height,TextureContext&); //44d9d0
int create_external_texture(TextureRecord&,UINT format,std::int32_t width,std::int32_t height,
                            std::int32_t x,std::int32_t y,std::int32_t original_width,
                            std::int32_t original_height,TextureContext&); //44e8f0
int postload_animation_entry(AnimationFile&,std::uint32_t texture_index,std::int32_t sprite_base,
                            std::int32_t script_base,AnmHeader*,TextureContext&,runtime::Log&); //44e350
AnimationFile* postload_animation_file(AnimationFile&,TextureContext&,runtime::Log&); //44e150
}
