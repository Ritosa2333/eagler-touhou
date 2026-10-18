#include "PauseMenu.hpp"
#include <cstring>
namespace th11 {
void PauseMenu::visible(bool value){
    if(auto* vm=animations.find(menu_animation)){
        if(value)vm->flags|=2;else vm->flags&=~2u;
        if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next){if(value)n->value->flags|=2;else n->value->flags&=~2u;}
    }
}
bool PauseMenu::begin_end(bool is_practice,bool cleared){
    practice=is_practice;completed=cleared;transition(practice||completed?21:13);elapsed.set(0,&animations.rate);
    sounds.clear();action=PauseAction::None;
    auto* vm=animations.create(text,75,0,29,true);if(!vm){error="game-over backdrop creation failed";return false;}
    background_animation=vm->id;capture_requested=true;scores.settings[0x37]=1;return true;
}
void PauseMenu::show_end_choices(i32 script,i32 next,i32 count){
    auto* vm=animations.create(front,script,5,29,true);if(!vm){error="game-over menu creation failed";return;}
    menu_animation=vm->id;state=next;cursor.count=count;cursor.wrap=1;cursor.select(0);
    family(menu_animation,3);family(menu_animation,7);
}
void PauseMenu::end_menu(u32 pressed,u32 repeat){
    const bool practice_states=state>=20;const i32 list=practice_states?23:16,name=practice_states?24:17;
    switch(state){
    case 12:case 20:
        if(timer.current>=10&&(pressed&0x80001)){sounds.push_back(10);family(menu_animation,2);transition(state+1);}break;
    case 13:case 21:
        if(timer.current<10)break;
        if(!practice){
            const i32 rank=scores.insert_score(selection,difficulty,result_score,stage==7&&completed?8:stage,continues,timestamp,slowdown);
            unranked=rank<0;timer.set(0,&animations.rate);
            if(!unranked){cursor.count=25;cursor.wrap=1;cursor.select(rank);name_begin();state=practice_states?25:18;visible(false);break;}
            show_end_choices(98,14,practice_states?3:4);
        }else{
            if(selection>=0&&selection<7&&difficulty>=0&&difficulty<5&&stage>=1&&stage<=7){
                auto* p=scores.characters[selection].data()+0x59c+(stage+difficulty*6)*8;i32 best;std::memcpy(&best,p,4);
                if(best<result_score)std::memcpy(p,&result_score,4);
            }
            show_end_choices(practice_states?(completed?103:104):98,practice_states?22:14,practice_states?3:4);
        }break;
    case 14:case 22:{
        const bool compact=state==22;const i32 save=compact?1:2,back=compact?0:1;
        move(pressed|repeat,7);
        if(pressed&0x80001){
            if(practice_save_disabled&&cursor.selected==save){sounds.push_back(37);break;}
            choose(cursor.selected+94);sounds.push_back(10);transition(compact?26:19);
            if(cursor.selected==save){transition(compact?23:16);visible(false);cursor.push();cursor.count=25;cursor.wrap=1;cursor.select(0);scan_requested=true;}
            else if(cursor.selected==back||cursor.selected==(compact?2:3))sounds.push_back(10);
        }
        if(pressed&0x102){sounds.push_back(11);if(cursor.selected!=back){cursor.select(back);family(menu_animation,cursor.selected+7);}}
        break;
    }
    case 16:case 23:
        if(timer.current<10)break;
        cursor.previous=cursor.selected;if((pressed|repeat)&16)cursor.move(-1);if((pressed|repeat)&32)cursor.move(1);
        if(cursor.previous!=cursor.selected)sounds.push_back(12);
        if(pressed&0x80001){transition(name);recording_metadata_requested=true;name_begin();sounds.push_back(10);return;}
        if(pressed&0x102){transition(practice_states?22:14);visible(true);cursor.pop();cursor.count=practice_states?3:4;cursor.wrap=1;family(menu_animation,cursor.selected+7);replay_files.fill(nullptr);sounds.push_back(11);}break;
    case 17:case 24:
        if(timer.current<10)break;
        name_move(pressed|repeat);
        if(pressed&0x80001){const i32 choice=name_cursor.selected,edit=name_confirm();if(edit<0)return;
            if(edit==1){sounds.push_back(44);save_requested=true;transition(list);std::memcpy(scores.settings.data()+12,entered_name.data(),9);return;}
            sounds.push_back(choice==89?11:10);if(choice==89)return;
        }
        if(pressed&0x102){sounds.push_back(11);if(name_length)entered_name[--name_length]=' ';else transition(list);}break;
    case 18:case 25:
        if(timer.current<10)break;
        if(!unranked)name_move(pressed|repeat);
        if(pressed&0x80001){
            if(!unranked){const i32 edit=name_confirm();if(edit<0)return;
                if(edit!=1){sounds.push_back(10);return;}
                scores.score_name(selection,difficulty,cursor.selected,entered_name.data());
            }
            show_end_choices(practice_states?103:98,practice_states?22:14,practice_states?3:4);visible(true);return;
        }
        if(pressed&0x102){if(cursor.selected<0){show_end_choices(practice_states?103:98,practice_states?22:14,practice_states?3:4);visible(true);return;}
            if(name_length){sounds.push_back(11);entered_name[--name_length]=' ';}
        }break;
    case 19:case 26:
        if(timer.current>=12){family(background_animation,1);family(menu_animation,1);transition(27);
            if(practice_states)action=cursor.selected==2?PauseAction::Restart:PauseAction::Title;
            else action=cursor.selected==0?(completed?PauseAction::Restart:PauseAction::Continue):cursor.selected==3?PauseAction::Restart:PauseAction::Title;
        }break;
    case 15:break;
    }
}
}
