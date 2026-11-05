#pragma once
#include "texture_load.hpp"
namespace th20::source::platform_window {class FrameStatistics;}
namespace th20::source::sprite {
double elapsed_frame_interval(std::int64_t begin,std::int64_t end) noexcept; //44cf50, truncates nanoseconds to microseconds before converting to seconds
void begin_frame_interval(platform_window::FrameStatistics&,unsigned index); //451520
void end_frame_interval(platform_window::FrameStatistics&,unsigned index); //4515e0
std::int32_t update_animation_file_tasks(Controller&,TextureContext&,runtime::Log&,platform_window::FrameStatistics&); //44dfb0
}
