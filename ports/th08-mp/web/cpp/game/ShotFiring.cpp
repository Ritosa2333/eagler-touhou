#include "ShotFiring.hpp"
namespace th08 {
void trigger_shooting(Timer& timer)noexcept{if(timer.current<0)timer.set(0);}
void update_shooting(Timer& timer,const ShotFiringInputs& input,const FrameTiming& timing,ShotFiringActions& actions){
    if(input.stage_frame<20||timer.current<0||(input.bomb&&input.bomb_type==4))return;
    if(timer.changed()&&(!input.bomb||(input.character!=1&&input.character!=6&&input.character!=7)))actions.fire(timer.current);
    timer.tick(timing);if(timer.current>=20)timer.set(-1);
    if((input.buttons&1)&&timer.current<0&&!input.gui_blocked)timer.set(0);
    if(input.player_state==1||input.player_state==2)timer.set(-1);
}
}
