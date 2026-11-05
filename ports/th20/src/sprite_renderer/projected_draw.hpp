#pragma once
#include "draw.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::sprite {
namespace draw_environment {
program_entry::ViewportState& current_camera();
std::int32_t scaled_dimension(unsigned axis);
}
// Original helpers 440340 and 441430. The latter intentionally only updates
// Animation.matrix_57c and Controller.matrix_60007d8 in this executable.
std::int32_t prepare_projected_billboard(Animation&);
std::int32_t prepare_projected_matrix(Controller&,Animation&);
}
