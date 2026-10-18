#include "render_mesh.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::sprite::mesh_environment {
namespace pe=program_entry;
bool enabled(){return pe::graphics_state.resource_019c!=nullptr;}
Controller& controller(){return *pe::sprite_controller;}
AnimationFile& surface_animation(){return *pe::graphics_state.surface_animation;}
game_session::Context& context(std::int32_t index){return game_session::context(index);}
std::int32_t view_offset(std::int32_t index,unsigned axis){return axis?pe::window_state.field_0040[index]:pe::window_state.field_0038[index];}
std::int32_t display_offset(unsigned axis){return axis?pe::graphics_state.viewports[2].offset_y:pe::graphics_state.viewports[2].offset_x;}
std::int32_t scaled_dimension(unsigned axis){return axis?pe::window_state.scaled_height:pe::window_state.scaled_width;}
}
