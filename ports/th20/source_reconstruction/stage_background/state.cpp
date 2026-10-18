#include "background.hpp"
#include <cstring>
namespace th20::source::background {
void construct_camera(program_entry::ViewportState& camera) noexcept {
    // 40bda0 used on the view/projection matrices is an accessor with no stores.
    std::memset(&camera,0,offsetof(program_entry::ViewportState,view));
    std::memset(reinterpret_cast<std::uint8_t*>(&camera)+offsetof(program_entry::ViewportState,viewport),0,sizeof(camera)-offsetof(program_entry::ViewportState,viewport));
}
void construct_script_state(ScriptState& state) noexcept {
    state.timer={};state.instruction_offset=0;state.camera_motion=0;
    state.motion_timer={};state.secondary_motion_timer={};
    state.direction_interpolation={};state.position_interpolation={};state.up_interpolation={};
    state.fog_interpolation={};state.fov_interpolation={};construct_camera(state.camera);state.owner=nullptr;
    for(auto& animation:state.animations)sprite::construct_animation(animation);
    std::memset(state.fields_3294,0,sizeof(state.fields_3294));
    for(auto& timer:state.mesh_timers)timer={};
    for(auto& value:state.mesh_phase_x)value=0;for(auto& value:state.mesh_phase_y)value=0;
    state.mesh_mode=state.overlay_color=0;
}
void construct_background_members(Background& b) noexcept {
    construct_script_state(b.state);b.primitive_animations=nullptr;b.file=nullptr;b.objects=nullptr;b.instances=nullptr;b.instructions=nullptr;b.animation_file=nullptr;
    for(auto& count:b.rendered)count=0;b.state_flags&=~0x1fu;b.fade_timer={};b.stage_id=0;b.frame_count=0;b.original_data=nullptr;b.data_size=0;b.additional_draw=nullptr;
}
}
