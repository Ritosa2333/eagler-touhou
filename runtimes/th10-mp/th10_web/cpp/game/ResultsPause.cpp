#include "Results.hpp"
namespace th10 {
// 0x422c80. Key tests stay independent: multiple pressed bits can transition the
// menu more than once during one update, exactly as in the original.
void Results::update_pause(ResultsEnvironment& env){
    switch(state){
    case 1:
        if(elapsed.current>=10){state=2;menu.item_count=*env.replay_mode?2:3;menu.wrap=1;menu.select(0);env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));}return;
    case 2:
        menu.reserved=menu.selected;if(env.repeat(0x10))menu.move(-1);if(env.repeat(0x20))menu.move(1);
        if(menu.reserved!=menu.selected){env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));env.sound(12);}
        if(*env.pressed&0x1001){env.sound(10);switch(menu.selected){case 0:env.interrupt(background_animation,1);env.interrupt(menu_animation,1);env.interrupt(auxiliary_animation,1);state=3;break;case 1:env.interrupt(env.child(menu_animation,116),6);state=*env.replay_mode?3:4;break;case 2:env.interrupt(env.child(menu_animation,117),6);state=4;break;}reset_timer(env.rate);}
        if(*env.pressed&0x4000){env.sound(10);env.interrupt(env.child(menu_animation,117),6);state=3;reset_timer(env.rate);menu.select(2);env.interrupt(background_animation,1);}
        if(*env.pressed&0x200){env.sound(10);env.interrupt(env.child(menu_animation,116),6);state=3;reset_timer(env.rate);menu.select(1);}break;
    case 3:
        if(elapsed.current>=12){state=0;switch(menu.selected){case 0:resume(env);break;case 1:env.interrupt(background_animation,1);env.interrupt(menu_animation,1);env.select_screen(4);break;case 2:env.interrupt(menu_animation,1);*env.pending_screen=10;break;}}return;
    case 4:
        if(elapsed.current<20)return;
        if(elapsed.current==20){menu.push();menu.item_count=2;menu.wrap=1;menu.select(1);env.interrupt(menu_animation,14);}
        if(elapsed.current<30)return;
        if(elapsed.current==30)env.interrupt(menu_animation,static_cast<u16>(menu.selected+15));
        menu.reserved=menu.selected;if(env.repeat(0x10))menu.move(-1);if(env.repeat(0x20))menu.move(1);
        if(menu.reserved!=menu.selected){env.interrupt(menu_animation,static_cast<u16>(menu.selected+15));env.sound(12);}
        if(*env.pressed&0x1001){env.sound(10);if(menu.selected==0||menu.selected==1){env.interrupt(env.child(menu_animation,menu.selected+119),6);state=5;}reset_timer(env.rate);}break;
    case 5:
        if(elapsed.current<20)return;
        if(menu.selected==0){env.interrupt(background_animation,1);env.interrupt(menu_animation,1);state=3;menu.pop();}
        else if(menu.selected==1){menu.pop();env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));state=2;}
        reset_timer(env.rate);return;
    default:return;
    }
    if(*env.pressed&8){menu.select(0);env.interrupt(background_animation,1);env.interrupt(menu_animation,1);env.interrupt(auxiliary_animation,1);state=3;reset_timer(env.rate);}
}
}
