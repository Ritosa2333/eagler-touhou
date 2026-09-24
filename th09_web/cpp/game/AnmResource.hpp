#pragma once
#include "AnmExecutor.hpp"
#include <string>
#include <vector>
namespace th09 {
struct AnmSpriteRect {float x=0,y=0,width=0,height=0;};
struct AnmTextureSource {
    u32 width=0,height=0,format=0,color_key=0,priority=0;
    u32 pixel_offset=0,pixel_size=0,pixel_format=0,pixel_width=0,pixel_height=0;
    u32 first_sprite=0,sprite_count=0;
    bool embedded=false,empty=false;
    std::string name;
    std::vector<AnmSpriteRect> sprite_rects;
};
class AnmResource {
public:
    bool load(i32 index,const u8* bytes,u32 size);
    void clone_from(const AnmResource&,i32 index);
    bool configure_texture(u32 index,u32 handle,u32 actual_width,u32 actual_height);
    AnmLoaded& view() noexcept {return loaded;}
    const std::vector<AnmTextureSource>& textures() const noexcept {return sources;}
    const std::vector<u8>& data() const noexcept {return raw;}
    u32 script_count() const noexcept {return script_pointers.size();}
private:
    struct SpriteSource {float x,y,width,height;};
    AnmLoaded loaded;
    std::vector<u8> raw;
    std::vector<AnmTextureSource> sources;
    std::vector<SpriteSource> sprite_sources;
    std::vector<AnmLoadedSprite> sprites;
    std::vector<AnmRawInstr*> script_pointers;
};
}
