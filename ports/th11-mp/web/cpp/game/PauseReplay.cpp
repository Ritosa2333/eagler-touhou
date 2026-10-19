#include "PauseMenu.hpp"
#include <cstring>
namespace th11 {
void PauseMenu::name_begin(){
    name_cursor.select(0);name_cursor.count=91;name_cursor.wrap=1;
    std::memcpy(entered_name.data(),scores.settings.data()+12,8);entered_name[8]=0;
    if(std::memcmp(entered_name.data(),"        ",8))name_cursor.move(-1);
    name_length=8;while(name_length>0&&entered_name[name_length-1]==' ')--name_length;
}
void PauseMenu::name_move(u32 input){
    auto& c=name_cursor;c.previous=c.selected;
    if(input&16)c.move(-13);if(input&32)c.move(13);
    if(input&64)c.move(c.selected%13==0?12:-1);
    if(input&128)c.move(c.selected%13==12?-12:1);
    if(c.previous!=c.selected)sounds.push_back(12);
}
i32 PauseMenu::name_confirm(){
    static constexpr char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
    const i32 index=name_cursor.selected;
    if(index<89){const char c=index==88?' ':letters[index];
        if(name_length<8){entered_name[name_length++]=c;if(name_length>=8)name_cursor.select(90);}else entered_name[name_length-1]=c;
    }else if(index==89){if(!name_length)return -1;entered_name[--name_length]=' ';}
    else if(index==90)return 1;
    return 0;
}
void PauseMenu::save_menu(u32 pressed,u32 repeat){
    switch(state){
    case 8:
        if(timer.current>=20){transition(9);
            if(auto* vm=animations.find(menu_animation)){vm->flags&=~2u;if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->flags&=~2u;}
            cursor.push();cursor.count=25;cursor.wrap=1;cursor.select(0);scan_requested=true;
        }break;
    case 9:
        if(timer.current<10)break;
        cursor.previous=cursor.selected;if((pressed|repeat)&16)cursor.move(-1);if((pressed|repeat)&32)cursor.move(1);
        if(cursor.previous!=cursor.selected)sounds.push_back(12);
        if(pressed&0x80001){transition(10);recording_metadata_requested=true;name_begin();sounds.push_back(10);return;}
        if(pressed&0x102){cursor.pop();cursor.count=4;cursor.wrap=1;family(menu_animation,cursor.selected+7);
            replay_files.fill(nullptr);transition(4);cursor.select(1);sounds.push_back(11);
        }break;
    case 10:
        if(timer.current<10)break;
        name_move(pressed|repeat);
        if(pressed&0x80001){const i32 choice=name_cursor.selected,edit=name_confirm();if(edit<0)return;
            if(edit==1){sounds.push_back(44);save_requested=true;transition(9);std::memcpy(scores.settings.data()+12,entered_name.data(),9);return;}
            sounds.push_back(choice==89?11:10);if(choice==89)return;
        }
        if(pressed&0x102){sounds.push_back(11);if(name_length)entered_name[--name_length]=' ';else transition(9);}break;
    }
}
}
