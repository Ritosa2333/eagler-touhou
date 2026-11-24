#pragma once

namespace th13::sdl {

bool drive_one_frame(double delta_seconds);

int mp_run_frame(double delta_ms);

void reset_mp_accumulator();

}
