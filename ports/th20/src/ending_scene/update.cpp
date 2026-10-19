#include "ending.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/frame.hpp"
namespace th20::source::ending {
bool update_script(Script& o){const int result=run_script(o);if(result==0)recovered::timer_tick(o.elapsed,state::timer_rate);return result!=0;}
int update(EndingInf& o){
    if(update_script(*o.script)){if(!(o.ending_flags&4u)){o.ending_flags|=8u;gameplay::request_scene(16);return 1;}o.ending_flags|=8u;}
    ++o.frames;if(!(o.script->flags&4u)&&!(o.ending_flags&2u)&&(o.script->flags&2u)){
        auto* buttons=input::button_slot(2);if(!(buttons&&(buttons->current&0x200u))&&held_frames(buttons,0)<20)return 1;
        if(recovered::signed_bits(o.frames)%12!=0)return 6;
    }
    return 1;
}
}
