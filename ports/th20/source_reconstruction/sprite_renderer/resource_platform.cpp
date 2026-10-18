#include "vertex_buffer.hpp"
#include "file_tasks.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../platform_window/frame_statistics.hpp"
namespace th20::source::platform_window::unrecovered {
void initialize_sprite_assets(sprite::Controller& controller){sprite::initialize_corner_buffer(controller,*program_entry::graphics_state.device);}
int update_sprite_tasks(sprite::Controller& controller){
    auto& g=program_entry::graphics_state;auto& w=program_entry::window_state;
    sprite::TextureContext context{*g.device,w.scaled_width,w.scaled_height,g.presentation.BackBufferFormat,w.scale};
    return sprite::update_animation_file_tasks(controller,context,program_entry::log_buffer,*static_cast<FrameStatistics*>(scheduler_object_005c4a00));
}
}
