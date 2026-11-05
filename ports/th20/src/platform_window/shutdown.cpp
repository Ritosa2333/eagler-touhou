#include "platform_window.hpp"
#include "data_constants.hpp"
#include "../runtime_core/runtime_core.hpp"

namespace th20::source::platform_window {
int shutdown_graphics() {
    auto& g=program_entry::graphics_state;
    while(unrecovered::poll_background_jobs(program_entry::thread_registry)!=0) {}
    join_graphics_worker(g,1);sync_close_graphics_worker(g);
    if(g.dynamic_buffer) {runtime::release_bytes(g.dynamic_buffer);g.dynamic_buffer=nullptr;}
    unrecovered::shutdown_scene_objects();
    unrecovered::retire_scheduler_object(unrecovered::scheduler_object_005c4a00);
    unrecovered::retire_scheduler_object(unrecovered::scheduler_object_005b8898);
    // 0x004dab40: exact COM release of Controller+0x6000e1c. Root renderer
    // owns the controller; the original assumes it is valid at this point.
    auto& sprite=*program_entry::sprite_controller;
    if(sprite.corner_buffer) {
        sprite.corner_buffer->Release();sprite.corner_buffer=nullptr;
    }
    unrecovered::stop_audio(program_entry::thread_registry,4,0,data::audio_stop_tag);
    unrecovered::close_archive_manager();
    for(auto*& vm:g.surface_sprites) {unrecovered::destroy_animation_vm(vm);vm=nullptr;}
    return 0; // original explicit return0, not a missing-operation substitute
}
}
namespace th20::source::program_entry::unrecovered {
void fn_004dd490(GraphicsStatePrefix&) {platform_window::shutdown_graphics();}
}
