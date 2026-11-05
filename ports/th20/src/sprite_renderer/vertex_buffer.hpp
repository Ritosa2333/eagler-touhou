#pragma once
#include "sprite.hpp"
namespace th20::source::sprite {
extern WorldVertex24 world_quad[4]; // initialized data 0x5aef80
void initialize_corner_buffer(Controller&,Device&); //44d020
}
