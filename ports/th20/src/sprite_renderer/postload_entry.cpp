#include "texture_load.hpp"
#include "binding.hpp"
#include "file_text.hpp"
#include <cstring>
namespace th20::source::sprite {
namespace {
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float mul(float a,float b){return th20::recovered::mul32(a,b);}
float add(float a,float b){return th20::recovered::add32(a,b);}
float as_float(std::uint32_t n){return static_cast<float>(static_cast<double>(n));}
struct FileSprite {std::uint32_t id;float x,y,width,height;std::uint32_t extra[5];};
static_assert(sizeof(FileSprite)==40);
}
int postload_animation_entry(AnimationFile& file,std::uint32_t texture_index,std::int32_t sprite_base,
                            std::int32_t script_base,AnmHeader* header,TextureContext& context,runtime::Log& log) {
    if(!header)return -1; // The original diagnostic at this branch is verified no-op 40c6b0.
    if(header->version!=8){runtime::log_error(log,file_text::s0056e53c,file.filename.c_str());return -1;}
    auto& texture=file.textures[texture_index];texture.header=reinterpret_cast<TextureHeader*>(header);
    auto* data=reinterpret_cast<std::uint8_t*>(header);
    auto* name=reinterpret_cast<const char*>(data+header->name_offset);
    if(!header->has_data) {
        if(name[0]=='@') {
            if(name[1]=='R') {
                header->width=static_cast<std::uint16_t>(context.render_width);header->height=static_cast<std::uint16_t>(context.render_height);
                create_render_target(texture,context.device,context.render_width,context.render_height,context.backbuffer_format);
            }else file.fields_5c[2]+=create_dynamic_texture(texture,context.device,header->width,header->height,header->format);
        }else {
            const auto loaded=create_external_texture(texture,header->format,header->width,header->height,header->x,header->y,
                                                      header->original_width,header->original_height,context);
            if(loaded<0)return -1;file.fields_5c[2]+=static_cast<std::uint32_t>(loaded);
        }
    }else {
        const auto loaded=create_embedded_texture(texture,data+header->texture_offset,header->format,header->width,header->height,context);
        if(loaded<0)return -1;file.fields_5c[2]+=static_cast<std::uint32_t>(loaded);
    }
#ifdef TH_SDL3
    const std::uint32_t surface_width=context.device.texture_width(texture.texture),surface_height=context.device.texture_height(texture.texture);
#else
    D3DSURFACE_DESC description;texture.texture->GetLevelDesc(0,&description);
    const auto surface_width=description.Width,surface_height=description.Height;
#endif
    auto* table=reinterpret_cast<std::uint32_t*>(header+1);SpriteData incoming{};
    for(std::uint32_t i=0;i<header->sprite_count;++i) {
        auto& raw=*reinterpret_cast<const FileSprite*>(data+*table++);
        incoming.field_00=file.id;incoming.field_04=texture_index;incoming.texture_id=(file.id<<8)|texture_index;
        incoming.scale_50=div(as_float(surface_width),th20::recovered::int_float(header->width));
        incoming.scale_54=div(as_float(surface_height),th20::recovered::int_float(header->height));
        incoming.left=mul(raw.x,incoming.scale_50);incoming.top=mul(raw.y,incoming.scale_54);
        incoming.right=mul(add(raw.x,raw.width),incoming.scale_50);incoming.bottom=mul(add(raw.y,raw.height),incoming.scale_54);
        incoming.texture_extent_20=as_float(surface_width);incoming.texture_extent_1c=as_float(surface_height);
        std::memcpy(incoming.fields_24,raw.extra,sizeof(raw.extra));
        store_sprite_descriptor(file,sprite_base++,incoming);
    }
    for(std::uint32_t i=0;i<header->script_count;++i) {
        file.scripts[script_base++]=reinterpret_cast<AnmInstruction*>(data+table[1]);table+=2;
    }
    return 1;
}
}
