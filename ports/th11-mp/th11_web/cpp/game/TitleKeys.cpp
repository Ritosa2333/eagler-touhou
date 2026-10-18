#include "TitleMenu.hpp"
namespace th11 {
void TitleMenu::restore_keys(){
    for(u32 i=0;i<5;++i)std::memcpy(&edited_buttons[i],config.bytes.data()+4+(i==4?8:i)*2,2);
    refresh_keys();
}
void TitleMenu::refresh_keys(){
    for(i32 i=0;i<5;++i)for(i32 variant=0;variant<2;++variant)for(i32 digit=0;digit<2;++digit){
        if(auto* vm=animations.find(child(2,59+i*2+variant*10+digit)))
            if(!vm->bind_sprite(42+(digit?edited_buttons[i]%10:edited_buttons[i]/10)))error="controller digit sprite missing";
    }
}
void TitleMenu::keys(u32 pressed,u32 repeat){
    switch(substate){
    case 0:cursor.count=7;cursor.select(0);create(2);step(1);restore_keys();[[fallthrough]];
    case 1:if(timer.current>6){step(2);highlight(2,17);}break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(2);}
        for(i16 button=0;button<31;++button)if(controller_buttons&(1u<<button)){
            const auto index=cursor.selected;
            if(index>=0&&index<5&&edited_buttons[index]!=button){
                for(i32 i=0;i<5;++i)if(i!=index&&edited_buttons[i]==button)edited_buttons[i]=edited_buttons[index];
                edited_buttons[index]=button;refresh_keys();sounds.push_back(10);
            }break;
        }
        if((pressed&0x102)&&cursor.selected==6){restore_keys();sounds.push_back(11);family(2,6);step(4);return;}
        if(pressed&0x80001){
            if(cursor.selected==5){restore_keys();sounds.push_back(10);return;}
            if(cursor.selected==6){for(u32 i=0;i<5;++i)std::memcpy(config.bytes.data()+4+(i==4?8:i)*2,&edited_buttons[i],2);sounds.push_back(11);family(2,6);step(4);}
        }break;
    case 4:if(timer.current>9){change(TitleScreen::Options);cursor.pop();}break;
    }
}
}
