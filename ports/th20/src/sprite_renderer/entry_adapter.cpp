#include "../program_entry/unrecovered_dependencies.hpp"
#include "binding.hpp"
namespace th20::source::program_entry::unrecovered {
void reset_sprite_queue(SpriteController* sprites) {
    // The old entry declaration's name is misleading: 0x4455c0 flushes any
    // pending textured triangle batch before subsequent state changes.
    sprite::flush_textured_quads(*sprites,*graphics_state.device);
}
void prepare_sprite_draw(SpriteController* sprites) {sprite::prepare_buffers(*sprites);}
void fn_0041dbe0(SpriteController* sprites) {sprite::release_device_textures(*sprites);}
void fn_0041d9f0(SpriteController* sprites) {
    sprite::recreate_device_textures(*sprites,*graphics_state.device,graphics_state.presentation.BackBufferFormat);
}
}
namespace th20::source::platform_window::unrecovered {
void bind_animation_script(sprite::AnimationFile& file,void* vm,int script,int parent) {
    sprite::bind_animation_script(file,*static_cast<sprite::Animation*>(vm),script,reinterpret_cast<sprite::Animation*>(parent));
}
}
