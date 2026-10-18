#pragma once
#include "sprite.hpp"
namespace th20::source::sprite {
void apply_animation_render_state(Controller&,Animation&,Device&); //445bc0 /445ba0
void select_texture_combine(Controller&,Device&,std::uint8_t); //446690
std::int32_t append_textured_quad(Controller&,const Vertex28 (&)[4]) noexcept; //43e9b0
}
