#include "TitleMenu.hpp"
namespace th11 {
void TitleMenu::restore_keys(){
    for(u32 i=0;i<(kPortTitle==14?4:5);++i)std::memcpy(&edited_buttons[i],config.bytes.data()+4+(i==4?8:i)*2,2);
    refresh_keys();
}
void TitleMenu::refresh_keys(){
    for(i32 i=0;i<(kPortTitle==14?4:5);++i)for(i32 variant=0;variant<2;++variant)for(i32 digit=0;digit<2;++digit){
        if(auto* vm=animations.find(child(2,(kPortTitle==14?61:59)+i*2+variant*(kPortTitle==14?8:10)+digit)))
            if(!vm->bind_sprite((kPortTitle==14?50:42)+(digit?edited_buttons[i]%10:edited_buttons[i]/10)))error="controller digit sprite missing";
    }
}
void TitleMenu::keys(u32 pressed,u32 repeat){
    constexpr i32 rows=kPortTitle==14?4:5,reset=rows,quit=rows+1;
    auto present=[&](){
        if constexpr(kPortTitle==14){
            // Complete45d4c0 delivers30/31 immediately to surrounding rows.
            // In retail title.anm these labels resume the saved animation continuation.
            auto row=[&](i32 i,i32 label,bool digits){
                interrupt(child(2,49+i),label,true);interrupt(child(2,55+i),label,true);
                if(digits)for(const auto script:{61+i*2,62+i*2,69+i*2,70+i*2})interrupt(child(2,script),label,true);
            };
            for(i32 i=0;i<cursor.selected;++i)row(i,30,i<4);
            for(i32 i=cursor.selected+1;i<6;++i)row(i,31,false);
            for(i32 i=cursor.selected+1;i<4;++i)for(const auto script:{61+i*2,62+i*2,69+i*2,70+i*2})interrupt(child(2,script),31,true);
        }
    };
    switch(substate){
    case 0:cursor.count=rows+2;cursor.select(0);create(2);step(1);restore_keys();[[fallthrough]];
    case 1:if(timer.current>6){step(2);highlight(2,17);present();}break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(2);present();}
        for(i16 button=0;button<31;++button)if(controller_buttons&(1u<<button)){
            const auto index=cursor.selected;
            if(index>=0&&index<rows&&edited_buttons[index]!=button){
                for(i32 i=0;i<i32(edited_buttons.size());++i)if(i!=index&&edited_buttons[i]==button)edited_buttons[i]=edited_buttons[index];
                edited_buttons[index]=button;refresh_keys();sounds.push_back(10);
            }break;
        }
        if((pressed&0x102)&&cursor.selected==quit){restore_keys();sounds.push_back(11);family(2,6);step(4);return;}
        if(pressed&0x80001){
            if(cursor.selected==reset){restore_keys();sounds.push_back(10);return;}
            if(cursor.selected==quit){for(u32 i=0;i<rows;++i)std::memcpy(config.bytes.data()+4+(i==4?8:i)*2,&edited_buttons[i],2);sounds.push_back(11);family(2,6);step(4);}
        }break;
    case 4:if(timer.current>9){change(TitleScreen::Options);cursor.pop();}break;
    }
}
}
