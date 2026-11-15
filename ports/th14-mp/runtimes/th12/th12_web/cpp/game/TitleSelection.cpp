#include "TitleMenu.hpp"
#include "PortTitle.hpp"
namespace th11 {
void TitleMenu::character(u32 pressed,u32 repeat){
    if constexpr(kPortTitle==14){character_th14(pressed,repeat);return;}
    if constexpr(kPortTitle==13){
        // 451450: four characters and no partner screen. TH13's two group
        // scripts share the normal/Extra selection and directional interrupts.
        const i32 group=selection.difficulty==4?130:129;
        switch(substate){
        case 0:
            cursor.count=4;
            if(selection.difficulty==4)for(i32 c=0;c<4;++c)if(!available(c,0)&&!available(c,1)){
                if(cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=c;
            }
            create(102,97);
            if(!exists(group)){close(group);create(group);highlight(group);}
            step(1);[[fallthrough]];
        case 1:if(timer.current>6)step(2);break;
        case 2:
            if((pressed|repeat)&64){sounds.push_back(12);family(group,cursor.selected+22,true);cursor.move(-1);family(group,cursor.selected+12);}
            if((pressed|repeat)&128){sounds.push_back(12);family(group,cursor.selected+17,true);cursor.move(1);family(group,cursor.selected+7);}
            if(pressed&0x102){step(4);sounds.push_back(11);return;}
            if(pressed&0x80001){
                interrupt(child(group,125+cursor.selected),6);
                interrupt(child(group,85),6);interrupt(child(group,86),6);
                sounds.push_back(10);sounds.push_back(50);step(3);
            }break;
        case 3:
            if(timer.current==10){
                if(!(selection.flags&16))fade_requested=true;
                else {selection.character=selection.last_character=cursor.selected;selection.partner=selection.last_partner=0;cursor.push();close(97);change(TitleScreen::Practice);}
            }
            if(timer.current>=40){
                selection.character=selection.last_character=cursor.selected;selection.partner=selection.last_partner=0;cursor.push();
                change(TitleScreen::Exit);selection.stage=selection.difficulty<4?1:7;start_requested=true;
            }break;
        case 4:
            if(timer.current>=6){close(group);close(97);selection.character=selection.last_character=cursor.selected;change(TitleScreen::Difficulty);cursor.pop();}break;
        }
        return;
    }
    switch(substate){
    case 0:
        cursor.count=2;
        if(selection.difficulty==4)for(i32 c=0;c<2;++c)if(!available(c,0)&&!available(c,1)&&!available(c,2)){
            if(cursor.selected==c)cursor.select(1-c);if(cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=c;
        }
        create(97);close(123);create(123);highlight(123,17);step(1);[[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        move(pressed|repeat,64,128);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(123);}
        if(pressed&0x102){step(4);sounds.push_back(11);return;}
        if(pressed&0x80001){
            interrupt(child(123,cursor.selected+119),6);interrupt(child(123,cursor.selected+121),6);
            interrupt(child(123,120-cursor.selected),1);interrupt(child(123,122-cursor.selected),1);
            sounds.push_back(10);step(3);
        }break;
    case 3:
        if(timer.current>=14){close(97);change(TitleScreen::Partner);const i32 old=selection.character;
            selection.character=selection.last_character=cursor.selected;cursor.push();cursor.count=3;
            if(old==selection.character){cursor.select(selection.last_partner);selection.partner=selection.last_partner;}
            else {cursor.select(0);selection.last_partner=0;}
        }break;
    case 4:
        if(timer.current>=6){close(123);close(97);change(TitleScreen::Difficulty);selection.character=cursor.selected;cursor.pop();selection.last_character=selection.character;}break;
    }
}
void TitleMenu::partner(u32 pressed,u32 repeat){
    if constexpr(kPortTitle==14){partner_th14(pressed,repeat);return;}
    const i32 group=142+selection.character;
    switch(substate){
    case 0:
        cursor.count=3;
        if(selection.difficulty==4)for(i32 p=0;p<3;++p)if(!available(selection.character,p)){
            if(cursor.selected==p){const i32 other=p==0?1:0;cursor.select(available(selection.character,other)?other:(p==2?1:2));}
            if(cursor.disabled_count<16)cursor.disabled[cursor.disabled_count++]=p;
        }
        create(98);close(group);create(group);highlight(group,17);step(1);
        for(i32 p=0;p<3;++p){u32 clears=0;std::memcpy(&clears,scores.characters[selection.character*2+p].data()+0x590+selection.difficulty*4,4);
            if(!clears)hide(child(group,136+selection.character*2+p));}
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(group);}
        if(pressed&0x102){step(4);sounds.push_back(11);return;}
        if(pressed&0x80001){step(3);sounds.push_back(10);interrupt(child(group,124+selection.character*6+cursor.selected),6);}break;
    case 3:
        if(timer.current==10){
            if(!(selection.flags&16))fade_requested=true;
            else {selection.partner=selection.last_partner=cursor.selected;cursor.push();close(98);change(TitleScreen::Practice);}
        }
        if(timer.current>=40){selection.partner=selection.last_partner=cursor.selected;cursor.push();
            if(selection.flags&16){close(98);change(TitleScreen::Practice);}
            else {change(TitleScreen::Exit);selection.stage=selection.difficulty<4?1:7;start_requested=true;}
        }break;
    case 4:
        if(timer.current>=6){selection.partner=selection.last_partner=cursor.selected;close(group);close(98);change(TitleScreen::Character);cursor.pop();}break;
    }
}
}
