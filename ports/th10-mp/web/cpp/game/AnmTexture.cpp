#include "AnmResources.hpp"
namespace th10 {
namespace {
Extended unsigned_dimension(u32 value){auto result=Extended::from_int(static_cast<i32>(value));if(value&0x80000000)result=result+number(4294967296.f);return result;}
template<class T>T read(const u8* bytes){T value;__builtin_memcpy(&value,bytes,sizeof(value));return value;}
}
// 0x447470. Texture creation is a platform operation. Sprite and script tables
// are reconstructed here, including separate ratios for resized texture axes.
i32 AnmFile::materialize(i32 texture_index,i32 first_sprite,i32 first_script,const AnmChunk* chunk,AnmTextureEnvironment& env){
    if(!chunk){env.texture_error(AnmResourceError::MissingHeader,nullptr);return -1;}
    if(chunk->version!=4){env.texture_error(AnmResourceError::InvalidVersion,nullptr);return -1;}
    auto& texture=textures[texture_index];const auto* bytes=reinterpret_cast<const u8*>(chunk);
    if(chunk->embedded_texture){if(env.create_embedded(texture,bytes+chunk->texture_offset,chunk->width,chunk->height,chunk->format)){env.texture_error(AnmResourceError::EmbeddedTexture,nullptr);return -1;}}
    else if(*chunk->texture_name()=='@')env.create_empty(texture,chunk->width,chunk->height,chunk->format);
    else if(env.create_encoded(texture,chunk->width,chunk->height,chunk->format,chunk->color_key)){env.texture_error(AnmResourceError::EncodedTexture,chunk->texture_name());return -1;}
#ifdef TH_ENABLE_THCRAP
    if(chunk->embedded_texture)env.override_embedded(texture,chunk->texture_name(),chunk);
#endif
    env.set_priority(texture.handle,chunk->priority);env.preload(texture.handle);const auto size=env.dimensions(texture.handle);
    auto* offsets=bytes+sizeof(AnmChunk);
    for(i32 index=0;index<chunk->sprite_count;++index,offsets+=4){
        const auto* record=bytes+read<u32>(offsets);AnmSprite sprite{};sprite.file_index=file_index;sprite.texture=texture.handle;
        const auto width=unsigned_dimension(size.width),height=unsigned_dimension(size.height);
        sprite.scale_x=(width/Extended::from_int(chunk->width)).to_float();sprite.scale_y=(height/Extended::from_int(chunk->height)).to_float();
        sprite.left=(number(sprite.scale_x)*number(read<float>(record+4))).to_float();sprite.top=(number(sprite.scale_y)*number(read<float>(record+8))).to_float();
        sprite.right=((number(read<float>(record+12))+number(read<float>(record+4)))*number(sprite.scale_x)).to_float();
        sprite.bottom=((number(read<float>(record+16))+number(read<float>(record+8)))*number(sprite.scale_y)).to_float();
        sprite.texture_width=width.to_float();sprite.texture_height=height.to_float();set_sprite(first_sprite,sprite);first_sprite=wrapping_add(first_sprite,1);
    }
    // The first word of each script record is an id; the second is its offset.
    for(i32 index=0;index<chunk->script_count;++index,offsets+=8){scripts[first_script]=reinterpret_cast<AnmInstruction*>(const_cast<u8*>(bytes+read<u32>(offsets+4)));first_script=wrapping_add(first_script,1);}
    return 1;
}
}
