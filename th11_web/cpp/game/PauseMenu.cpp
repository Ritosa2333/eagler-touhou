#include "PauseMenu.hpp"
namespace th11 {
void PauseMenu::family(u32 id,i32 label){
    auto* vm=animations.find(id);if(!vm)return;vm->pending_interrupt=i16(label);
    if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->pending_interrupt=i16(label);
}
void PauseMenu::choose(i32 script){
    auto* vm=animations.find(menu_animation);if(!vm){menu_animation=0;return;}
    for(auto* n=&vm->child;n;n=n->next)if(n->value->script_index==script){family(n->value->id,6);return;}
}
void PauseMenu::move(u32 keys,i32 base){
    cursor.previous=cursor.selected;if(keys&16)cursor.move(-1);if(keys&32)cursor.move(1);
    if(cursor.previous!=cursor.selected){family(menu_animation,base+cursor.selected);sounds.push_back(12);}
}
void PauseMenu::escape(u32 pressed){if(pressed&256){cursor.select(0);family(background_animation,1);family(menu_animation,1);transition(4);}}
bool PauseMenu::begin(bool playback){
    replay=playback;transition(1);elapsed.set(0,&animations.rate);sounds.clear();action=PauseAction::None;
    auto* backdrop=animations.create(text,75,0,29,true);if(!backdrop){error="pause backdrop creation failed";return false;}
    background_animation=backdrop->id;capture_requested=true;
    auto* menu=animations.create(front,replay?90:89,5,29,true);if(!menu){error="pause menu creation failed";return false;}
    menu_animation=menu->id;family(menu_animation,3);sounds.push_back(32);return true;
}
bool PauseMenu::update(u32 pressed,u32 repeat){
#ifdef TH11_MULTIPLAYER
    if(multiplayer_pause)return update_multiplayer_pause(pressed);
#endif
    sounds.clear();action=PauseAction::None;scan_requested=false;recording_metadata_requested=false;
    switch(state){
    case 0:break;
    case 1:case 2:
        if(timer.current>=10){const bool forced=state==2;state=3;cursor.count=4;
            if(forced||replay)cursor.disabled[cursor.disabled_count++]=2;
            if(forced)cursor.disabled[cursor.disabled_count++]=0;
            cursor.wrap=1;cursor.select(forced?1:0);family(menu_animation,cursor.selected+7);forced_exit=forced;
        }break;
    case 3:
        move(pressed|repeat,7);
        if(pressed&0x80001){if(practice_save_disabled&&cursor.selected==2){sounds.push_back(37);break;}sounds.push_back(10);
            switch(cursor.selected){case 0:family(background_animation,1);family(menu_animation,1);transition(4);break;
            case 1:choose(78);transition(replay?4:5);break;
            case 2:choose(79);transition(7);break;
            case 3:choose(80);transition(replay?4:5);break;}
        }
        if(!forced_exit&&(pressed&0x200000)){sounds.push_back(10);choose(80);transition(4);cursor.select(3);family(background_animation,1);}
        if(pressed&0x10000){sounds.push_back(10);choose(78);transition(4);cursor.select(1);}
        if(!forced_exit)escape(pressed);break;
    case 4:
        if(timer.current>11){state=0;
            if(cursor.selected==0)action=PauseAction::Resume;
            else if(cursor.selected==1||cursor.selected==3){family(background_animation,1);family(menu_animation,1);action=cursor.selected==1?PauseAction::Title:PauseAction::Restart;}
        }break;
    case 5:case 7:
        if(timer.current<20)break;
        if(timer.current==20){cursor.push();cursor.count=2;cursor.wrap=1;cursor.select(1);family(menu_animation,14);}
        if(timer.current<30)break;
        if(timer.current==30)family(menu_animation,cursor.selected+15);
        move(pressed|repeat,15);
        if(pressed&0x80001){sounds.push_back(10);choose(cursor.selected?88:87);transition(!cursor.selected&&state==7?8:6);}
        escape(pressed);break;
    case 6:
        if(timer.current>=20){if(cursor.selected==0){family(menu_animation,1);state=4;cursor.pop();}
            else if(cursor.selected==1){cursor.pop();family(menu_animation,cursor.selected+7);state=3;}timer.set(0,&animations.rate);
        }break;
    case 8:case 9:case 10:save_menu(pressed,repeat);break;
    case 11:case 27:break;
    case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:
    case 20:case 21:case 22:case 23:case 24:case 25:case 26:end_menu(pressed,repeat);break;
    default:error="pause state not restored: "+std::to_string(state);return false;
    }
    timer.tick();elapsed.tick();return error.empty();
}
}
