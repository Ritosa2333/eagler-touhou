#include "text.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::platform_window::unrecovered {
void draw_frame_rate_text(float fps) { //4abf30, 4ac160 is an actual constant-false function
    const auto scene=program_entry::graphics_state.field_0b0c;if(scene==15||scene==4)return;
    auto* renderer=text::renderer;if(!renderer)return;
    renderer->color=fps>=30.f?(fps>=40.f?0xffffffffu:0xffa0a0ffu):0xff5050ffu;
    renderer->fields_1a1d4[3]=1;
    renderer->write_float({588.f,470.f,0.f},static_cast<float>(static_cast<double>(fps)+0.05),"fps",1);
    renderer->color=0xffffffffu;renderer->fields_1a1d4[3]=0;
}
}
