#include "effect.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../program_entry/program_entry.hpp"
#include <bit>
namespace th20::source::screen::environment {
bool cancelled(){return gameplay::slowdown_frames!=0;}
const std::uint32_t* game_flags(){return gameplay::controller?&gameplay::controller->game_flags:nullptr;}
const float* timer_rate(){return state::timer_rate;}
std::uint32_t random_direction(){return state::bounded(state::random_streams[1],3);}
void apply_shake(int mode,int view,unsigned axis,std::uint32_t direction,float amplitude){
    auto& cameras=program_entry::graphics_state.viewports;
    if(direction==1||direction==2){const float value=direction==1?amplitude:std::bit_cast<float>(std::bit_cast<std::uint32_t>(amplitude)^0x80000000u);
        cameras[3].points[mode==1?0:view][axis]=value;const float scaled=recovered::mul32(value,program_entry::window_state.scale);
        cameras[1].points[view][axis]=scaled;cameras[0].points[view][axis]=scaled;cameras[5].points[view][axis]=scaled;
    }else{
        // 4249f0's zero-Y branch really targets553c, unlike its nonzero branch.
        cameras[3].points[mode==1?(axis==1?2:0):view][axis]=0.f;
        cameras[1].points[view][axis]=0.f;cameras[0].points[view][axis]=0.f;cameras[5].points[view][axis]=0.f;
    }
}
}
