#include "TitleMain.hpp"
namespace th10 {
// 0x42d300: initial prompt, with the original 11- and 30-frame boundaries.
i32 TitleMenu::update_prompt(TitleMainEnvironment& env){
    switch(phase){
    case 0:if(!env.registry->find(animation_ids[88])){create_script(88,env);create_script(90,env);playing_animation=env.create(*version_animations,0);}set_phase(1,env.rate);menu.select(0);[[fallthrough]];
    case 1:if(elapsed.current>10)set_phase(2,env.rate);break;
    case 2:if(*env.pressed&0x160b){signal_script(90,6,env);set_phase(4,env.rate);env.sound(32);env.registry->interrupt(playing_animation,1);}break;
    case 4:if(elapsed.current>=30)set_screen(2,env.rate);break;
    }return 1;
}
// 0x42d420. Main menu transitions retain the selected difficulty, the practice
// flag, the selection stack and the Extra-unlock exclusion.
i32 TitleMenu::update_main(TitleMainEnvironment& env){
    switch(phase){
    case 0:{
        menu.item_count=8;bool extra=false;for(i32 i=0;i<6;++i)extra=extra||env.extra_unlocked[i];if(!extra)menu.disabled_items[menu.disabled_count++]=1;
        if(env.game->flags&0x10){menu.select(2);env.game->flags&=~0x10u;}if(!env.registry->find(animation_ids[88]))create_script(88,env);create_script(0,env);set_phase(1,env.rate);
        [[fallthrough]];}
    case 1:if(elapsed.current>10){set_phase(2,env.rate);env.interrupt_immediately(animation_ids[0],3);signal_script(0,static_cast<u16>(menu.selected+17),env);}break;
    case 2:
        menu.reserved=menu.selected;if((*env.pressed|*env.repeated)&0x10)menu.move(-1);if((*env.pressed|*env.repeated)&0x20)menu.move(1);
        if(menu.reserved!=menu.selected){env.sound(12);env.interrupt_immediately(animation_ids[0],3);signal_script(0,static_cast<u16>(menu.selected+7),env);}
        if(*env.pressed&10){if(menu.selected==7){env.sound(11);set_phase(4,env.rate);return 1;}env.sound(11);menu.select(7);env.interrupt_immediately(animation_ids[0],3);signal_script(0,static_cast<u16>(menu.selected+7),env);}
        if(*env.pressed&0x1001){signal_script(0,6,env);switch(menu.selected){
            case 0:case 1:case 2:case 3:case 4:case 5:env.sound(10);signal_script(90,7,env);signal_script(91,7,env);set_phase(4,env.rate);if(menu.selected==5)env.sound(10);return 1;
            case 6:env.sound(10);set_phase(4,env.rate);env.interrupt_immediately(animation_ids[90],3);env.interrupt_immediately(animation_ids[91],3);return 1;
            case 7:env.sound(11);set_phase(4,env.rate);return 1;
        }}break;
    case 4:
        if(elapsed.current<20)break;
        switch(menu.selected){
        case 0:case 2:
            if(menu.selected==0)env.game->flags&=~0x10u;else env.game->flags|=0x10;dismiss_script(88,env);set_screen(6,env.rate);menu.push();if(env.game->difficulty>3)env.game->difficulty=1;menu.select(env.game->difficulty);break;
        case 1:env.game->flags&=~0x10u;dismiss_script(88,env);set_screen(6,env.rate);menu.push();saved_difficulty=env.game->difficulty;env.game->difficulty=4;menu.select(0);break;
        case 3:case 4:case 5:{const i32 screens[]={12,11,14};const auto next=screens[menu.selected-3];dismiss_script(88,env);set_screen(next,env.rate);menu.push();break;}
        case 6:set_screen(4,env.rate);menu.push();break;
        case 7:set_screen(3,env.rate);break;
        }break;
    }return 1;
}
}
