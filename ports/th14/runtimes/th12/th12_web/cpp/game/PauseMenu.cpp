#include "PauseMenu.hpp"
#if PORT_TITLE == 16
#include "BulletTh16.hpp"
#endif
namespace th11 {
#if PORT_TITLE == 16
AnmVm* PauseMenu::th16_overlay(AnmResource& file,i32 script,u16 file_id){
    // Own418fe0: retain the template layer, initialize origin/rotation,
    // tick once, set pending4, then register on the overlay list.
    const Vec3 origin{};const float rotation=0;
    auto* vm=animations.create(file,script,file_id,u32(-1),true,false,&origin,nullptr,nullptr,false,&rotation,0x400);
    if(vm)vm->pending_interrupt=4;return vm;
}
bool PauseMenu::update_th16(u32 pressed,u32 repeat){
    sounds.clear();action=PauseAction::None;
    const auto substate=[&](i32 next){th16_substate=next;timer.set(0,&animations.rate);};
    const auto child_interrupt=[&](i32 script,i32 label){if(auto* root=animations.find(menu_animation))for(auto* n=&root->child;n;n=n->next)if(n->value->script_index==script){family(n->value->id,label);break;}};
    if(th16_substate==0||th16_substate==1){
        if(timer.current>=10){const bool forced=th16_substate==1;substate(6);cursor.count=5;
            if(forced){if(!cursor.disable_th16(3)||!cursor.disable_th16(2)||!cursor.disable_th16(0))return false;}
            else{if(replay){if(!cursor.disable_th16(2)||!cursor.disable_th16(3))return false;}
                if(continues>0){if(!cursor.disable_th16(2))return false;family(menu_animation,i32(i16(cursor.selected+7)));for(i32 script:{130,139,151})child_interrupt(script,5);}}
            cursor.wrap=1;cursor.select_th16(forced?1:0);family(menu_animation,i32(i16(cursor.selected+7)));forced_exit=forced;
        }
    }else if(th16_substate>=2&&th16_substate<=5){
        if(timer.current>=10&&!th16_score_begin())return false;
    }else if(th16_substate==6){
        cursor.previous=cursor.selected;if((pressed|repeat)&16)cursor.move(-1);if((pressed|repeat)&32)cursor.move(1);
        if(cursor.previous!=cursor.selected){family(menu_animation,i32(i16(cursor.selected+7)));sounds.push_back(10);}
        if(pressed&0x80001){
            sounds.push_back(7);
            switch(cursor.selected){
            case 0:family(background_animation,1);family(menu_animation,1);substate(16);break;
            case 1:case 4:
                for(i32 script:cursor.selected==1?std::initializer_list<i32>{129,138,146,148,150}:std::initializer_list<i32>{132,141,147,149,152})child_interrupt(script,6);
                substate(state==1&&!replay?7:16);break;
            case 2:for(i32 script:{130,139,151})child_interrupt(script,6);substate(state==1?9:10);break;
            case 3:for(i32 script:{131,140})child_interrupt(script,6);substate(14);break;
            }
            timer.set(0,&animations.rate);
        }
        if(!forced_exit){
            if(pressed&0x200000){sounds.push_back(7);child_interrupt(132,6);family(background_animation,1);cursor.select_th16(4);substate(16);}
            if(pressed&256){family(background_animation,1);family(menu_animation,1);cursor.select_th16(0);substate(16);}
        }
        if(pressed&0x10000){sounds.push_back(7);child_interrupt(129,6);cursor.select_th16(1);substate(16);}
    }else if(th16_substate==7||th16_substate==9){
        if(timer.current>=20){
            if(timer.current==20){cursor.push();cursor.count=2;cursor.wrap=1;cursor.select_th16(1);family(menu_animation,14);}
            if(timer.current>=30){
                if(timer.current==30)family(menu_animation,i32(i16(cursor.selected+15)));
                cursor.previous=cursor.selected;if((pressed|repeat)&16)cursor.move(-1);if((pressed|repeat)&32)cursor.move(1);
                if(cursor.previous!=cursor.selected){family(menu_animation,i32(i16(cursor.selected+15)));sounds.push_back(10);}
                if(pressed&0x80001){
                    if(cursor.selected==0){child_interrupt(154,6);substate(th16_substate==9?10:8);sounds.push_back(7);}
                    else if(cursor.selected==1){child_interrupt(155,6);substate(8);sounds.push_back(9);}
                }
                if(pressed&0x102){sounds.push_back(9);if(cursor.selected==0){cursor.select_th16(1);family(menu_animation,i32(i16(cursor.selected+15)));}
                    else if(cursor.selected==1){child_interrupt(155,6);substate(8);}}
                if(pressed&256){family(background_animation,1);family(menu_animation,1);cursor.select_th16(0);substate(16);}
            }
        }
    }else if(th16_substate==8){
        if(timer.current>=20){
            if(cursor.selected==0){family(menu_animation,1);cursor.pop();substate(16);}
            else if(cursor.selected==1){cursor.pop();if(continues>0&&!cursor.disable_th16(2))return false;family(menu_animation,i32(i16(cursor.selected+7)));substate(6);}
        }
    }else if(th16_substate==15){
        if(timer.current>=10){
            auto& c=name_cursor;c.previous=c.selected;const u32 keys=pressed|repeat;
            if(keys&16)c.move(-13);if(keys&32)c.move(13);if(keys&64)c.move(c.selected%13==0?12:-1);if(keys&128)c.move(c.selected%13==12?-12:1);
            if(c.previous!=c.selected)sounds.push_back(10);
            bool consumed=false;
            if(pressed&0x80001){
                const i32 choice=c.selected;
                if(choice<89){static constexpr char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*";
                    const char value=choice==88?' ':letters[choice];
                    if(name_length<8){entered_name[name_length++]=value;if(name_length==8)c.select_th16(90);}else entered_name[name_length-1]=value;
                    sounds.push_back(7);consumed=true;
                }else if(choice==89){if(name_length){entered_name[--name_length]=' ';sounds.push_back(9);}consumed=true;}
                else if(choice==90){sounds.push_back(7);if(!scores.th16_score_name(selection,difficulty,cursor.selected,entered_name.data())){error="TH16 high-score name write failed";return false;}if(!th16_end_choices())return false;consumed=true;}
            }
            if(!consumed&&(pressed&0x102)){sounds.push_back(9);if(name_length)entered_name[--name_length]=' ';}
        }
    }else if(th16_substate==16){
        if(timer.current>=12){if(state!=1&&state!=2){error="TH16 end exit consumer unavailable";return false;}
            if(state==2){if(cursor.selected==0){error="TH16 in-place continue consumer unavailable";return false;}family(background_animation,1);family(menu_animation,1);}
            if(cursor.selected==0)action=PauseAction::Resume;
            else if(cursor.selected==1){family(background_animation,1);family(menu_animation,1);action=PauseAction::Title;}
            else if(cursor.selected==4){for(u32 id:{background_animation,menu_animation})if(auto* vm=animations.find(id))animations.destroy(*vm);background_animation=menu_animation=0;action=PauseAction::Restart;}
            state=0;th16_substate=0;cursor.disabled_count=0;timer.set(0,&animations.rate);elapsed.set(0,&animations.rate);
        }
    }else{error="TH16 pause input consumer unavailable: kind="+std::to_string(state)+" substate="+std::to_string(th16_substate);return false;}
    bullet_th16_clock(timer,animations.rate);bullet_th16_clock(elapsed,animations.rate);return true;
}
#endif
#if PORT_TITLE == 14
bool PauseMenu::create_backdrop(bool immediate_capture){
#else
bool PauseMenu::create_backdrop(){
#endif
#if PORT_TITLE == 16
    auto* vm=th16_overlay(text,52,0); // Own43ef20 prefix through43ef60.
#else
    // TH14 448bf0 creates text.anm script 52 through 44c1a0. The
    // script establishes layer 35; the factory clears bits 0x6000 only
    // after its first update, exactly as the other TH14 overlay factory.
    auto* vm=animations.create(text,kPortTitle==14?52:75,0,kPortTitle==14?0:29,true);
#endif
    if(!vm){error="pause capture animation creation failed";return false;}
    if constexpr(kPortTitle==14)vm->flags2&=~0x6000u;
    background_animation=vm->id;
#if PORT_TITLE == 14
    capture_requested=immediate_capture;
#else
    capture_requested=true;
#endif
    return true;
}
void PauseMenu::family(u32 id,i32 label){
    auto* vm=animations.find(id);if(!vm)return;vm->interrupt(i16(label));
    if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->interrupt(i16(label));
}
void PauseMenu::choose(i32 script){
    auto* vm=animations.find(menu_animation);if(!vm){menu_animation=0;return;}
    for(auto* n=&vm->child;n;n=n->next)if(n->value->script_index==script){family(n->value->id,6);return;}
}
void PauseMenu::move(u32 keys,i32 base){
    cursor.previous=cursor.selected;if(keys&16)cursor.move(-1);if(keys&32)cursor.move(1);
    if(cursor.previous!=cursor.selected){family(menu_animation,base+cursor.selected);sounds.push_back(kPortTitle==14?10:12);}
}
void PauseMenu::escape(u32 pressed){if(pressed&256){cursor.select(0);family(background_animation,1);family(menu_animation,1);transition(4);}}
#if PORT_TITLE == 14
bool PauseMenu::begin_replay_end(){
    // Retail448ef0 enters the forced replay menu before enabling pause rate1.
    replay=true;transition(2);elapsed.set(0,&animations.rate);forced_exit=0;
    sounds.clear();action=PauseAction::None;cursor.disabled_count=0;
#if defined(TH14_CPP_STAGE_DECODE)
    if(auto* control=animations.th14_control.get())control->flags|=0x10u; //448f8d, before capture.
#endif
    if(!create_backdrop())return false;
    auto* menu=animations.create(front,131,5,0,true);
    if(!menu){error="replay end menu creation failed";return false;}
    menu->flags2&=~0x6000u;menu_animation=menu->id;family(menu_animation,3);
    return true;
}
#endif
bool PauseMenu::begin(bool playback){
#if PORT_TITLE == 16
    replay=playback;transition(1);elapsed.set(0,&animations.rate);th16_substate=0;sounds.clear();action=PauseAction::None;
    // Own43f0.. entry creates the choice family before capture.
    auto* menu=th16_overlay(front,replay?158:156,5);if(!menu){error="TH16 pause menu creation failed";return false;}
    menu_animation=menu->id;family(menu_animation,3);sounds.push_back(14);return create_backdrop();
#else
    replay=playback;transition(1);elapsed.set(0,&animations.rate);sounds.clear();action=PauseAction::None;
#if PORT_TITLE == 14
    cursor.disabled_count=0; //448e1a, on every reuse of the persistent owner.
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    if(auto* control=animations.th14_control.get())control->flags|=0x10u; //448e24, before menu129/130.
#endif
    if constexpr(kPortTitle!=14)if(!create_backdrop())return false;
    auto* menu=animations.create(front,kPortTitle==14?(replay?130:129):(replay?90:89),5,kPortTitle==14?0:29,true);if(!menu){error="pause menu creation failed";return false;}
    if constexpr(kPortTitle==14)menu->flags2&=~0x6000u;
    menu_animation=menu->id;family(menu_animation,3);sounds.push_back(kPortTitle==14?14:32);
    if constexpr(kPortTitle==14)return create_backdrop();
    return true;
#endif
}
bool PauseMenu::update(u32 pressed,u32 repeat){
#if PORT_TITLE == 16
    return update_th16(pressed,repeat);
#endif
    sounds.clear();action=PauseAction::None;scan_requested=false;recording_metadata_requested=false;
    switch(state){
    case 0:break;
    case 1:case 2:
        if(timer.current>=10){const bool forced=state==2;state=3;cursor.count=kPortTitle==14?5:4;
            if constexpr(kPortTitle==14){
                // Retail4490e9/44917a: help is disabled in replay/forced exit;
                // continuing additionally disables saving, including the
                // duplicate disable entry present in the retail cursor.
                if(forced){for(i32 choice:{3,2,0})cursor.disabled[cursor.disabled_count++]=choice;}
                else{if(replay){for(i32 choice:{2,3})cursor.disabled[cursor.disabled_count++]=choice;}
                    if(continues>0)cursor.disabled[cursor.disabled_count++]=2;}
            }else{if(forced||replay)cursor.disabled[cursor.disabled_count++]=2;
                if(forced)cursor.disabled[cursor.disabled_count++]=0;}
            cursor.wrap=1;cursor.select(forced?1:0);family(menu_animation,cursor.selected+7);forced_exit=forced;
        }break;
    case 3:
        move(pressed|repeat,7);
        if(pressed&0x80001){sounds.push_back(kPortTitle==14?7:10);
            switch(cursor.selected){case 0:family(background_animation,1);family(menu_animation,1);transition(4);break;
            case 1:choose(kPortTitle==14?118:78);transition(replay?4:5);break;
            case 2:choose(kPortTitle==14?119:79);transition(7);break;
            case 3:
#if PORT_TITLE == 14
                choose(151);transition(12);manual_requested=true;manual_origin_x=32.f;break;
#else
                choose(80);transition(replay?4:5);break;
#endif
            case 4:if constexpr(kPortTitle==14){choose(120);transition(replay?4:5);}break;}
        }
        if(!forced_exit&&(pressed&0x200000)){sounds.push_back(kPortTitle==14?7:10);choose(kPortTitle==14?120:80);transition(4);cursor.select(kPortTitle==14?4:3);family(background_animation,1);}
        if(pressed&0x10000){sounds.push_back(kPortTitle==14?7:10);choose(kPortTitle==14?118:78);transition(4);cursor.select(1);}
        if(!forced_exit)escape(pressed);break;
    case 4:
        if(timer.current>11){state=0;
            if(cursor.selected==0)action=PauseAction::Resume;
            else if(cursor.selected==1||cursor.selected==(kPortTitle==14?4:3)){family(background_animation,1);family(menu_animation,1);action=cursor.selected==1?PauseAction::Title:PauseAction::Restart;}
        }break;
    case 5:case 7:
        if(timer.current<20)break;
        if(timer.current==20){cursor.push();cursor.count=2;cursor.wrap=1;cursor.select(1);family(menu_animation,14);}
        if(timer.current<30)break;
        if(timer.current==30)family(menu_animation,cursor.selected+15);
        move(pressed|repeat,15);
        if(pressed&0x80001){sounds.push_back(kPortTitle==14?(cursor.selected?9:7):10);choose(cursor.selected?(kPortTitle==14?128:88):(kPortTitle==14?127:87));transition(!cursor.selected&&state==7?8:6);}
        if constexpr(kPortTitle==14)if(pressed&0x102){sounds.push_back(9);
            if(cursor.selected==0){cursor.select(1);family(menu_animation,16);}
            else if(cursor.selected==1){choose(128);transition(6);}}
        escape(pressed);break;
    case 6:
        if(timer.current>=20){if(cursor.selected==0){family(menu_animation,1);state=4;cursor.pop();}
            else if(cursor.selected==1){cursor.pop();family(menu_animation,cursor.selected+7);state=3;}timer.set(0,&animations.rate);
        }break;
    case 8:case 9:case 10:save_menu(pressed,repeat);break;
    case 11:case 27:break;
    case 12:
#if PORT_TITLE == 14
        if(manual_resource){manual_update(pressed,repeat);if(manual_finished){transition(3);visible(true);}}
        break;
#else
        end_menu(pressed,repeat);break;
#endif
    case 13:case 14:case 15:case 16:case 17:case 18:case 19:
    case 20:case 21:case 22:case 23:case 24:case 25:case 26:end_menu(pressed,repeat);break;
    default:error="pause state not restored: "+std::to_string(state);return false;
    }
    timer.tick();elapsed.tick();return error.empty();
}
}
