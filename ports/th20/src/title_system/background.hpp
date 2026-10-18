#pragma once
#include "title.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::title {
std::uint32_t shade_component(int,float weight,float direction); //51e100
void deform_background(TitleInf&,state::Random&); //51ed90 vertex loop and wave progression
}
