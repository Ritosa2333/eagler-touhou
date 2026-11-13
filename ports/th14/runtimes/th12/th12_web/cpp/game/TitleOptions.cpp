#include "TitleMenu.hpp"
#include <algorithm>
namespace th11 {
void TitleMenu::refresh_options(){
    volume_changed=true;
    for(i32 channel=0;channel<2;++channel){const i32 value=kPortTitle==14?i8(config.bytes[GameConfig::volume_offset+channel]):config.bytes[GameConfig::volume_offset+channel];
        // Retail45b870 binds scripts33..35/37..39 and41..43/45..47.
        // The earlier slots are labels; using them overwrites the menu text.
        for(i32 bright=0;bright<2;++bright){const i32 base=(kPortTitle==14?33:29)+channel*8+bright*4,sprite=(kPortTitle==14?50:42)+bright*(kPortTitle==14?11:10);
            for(i32 digit=0;digit<3;++digit)if(auto* vm=animations.find(child(1,base+digit))){
                const i32 n=digit==0?value/100:digit==1?(value/10)%10:value%10;
                if(!vm->bind_sprite(sprite+n)){error="options digit sprite missing";return;}
                if(digit<2){const bool visible=value>=(digit==0?100:10);vm->flags=visible?vm->flags|2:vm->flags&~2u;}
            }
        }
    }
}
void TitleMenu::options(u32 pressed,u32 repeat){
    auto present=[&](){
        if constexpr(kPortTitle==14){
            //45ad90 sends30/31 immediately to caption pairs and all eight
            // digit/percent children of the preceding/following volume rows.
            for(i32 i=0;i<cursor.selected;++i){interrupt(child(1,23+i),30,true);interrupt(child(1,28+i),30,true);}
            for(i32 i=cursor.selected+1;i<5;++i){interrupt(child(1,23+i),31,true);interrupt(child(1,28+i),31,true);}
            for(i32 channel=0;channel<2;++channel)if(channel!=cursor.selected)
                for(i32 digit=0;digit<8;++digit)interrupt(child(1,33+channel*8+digit),channel<cursor.selected?30:31,true);
        }
    };
    switch(substate){
    case 0:cursor.count=5;cursor.select(0);create(1);refresh_options();step(1);[[fallthrough]];
    case 1:if(timer.current>6){step(2);highlight(1,17);present();}break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected){sounds.push_back(12);highlight(1);present();}
        if(pressed&0x102){
            if(cursor.selected!=4){sounds.push_back(11);cursor.select(4);highlight(1);present();return;}
            family(1,6);sounds.push_back(11);step(4);return;
        }
        if(cursor.selected==1&&timer.current!=timer.previous&&timer.current%60==0)sounds.push_back(kPortTitle==14?2:4);
        if(cursor.selected<2){auto& value=config.bytes[GameConfig::volume_offset+cursor.selected];
            if((pressed|repeat)&64){value=(kPortTitle==14?i8(value)<5:value<5)?0:value-5;refresh_options();}
            if((pressed|repeat)&128){value=u8(value+5);if(kPortTitle==14?i8(value)>100:value>100)value=100;refresh_options();}
        }
        if(pressed&0x80001){
            if(cursor.selected==2||cursor.selected==4){family(1,6);sounds.push_back(cursor.selected==4?11:10);step(4);return;}
            if(cursor.selected==3){config.bytes[GameConfig::volume_offset]=100;config.bytes[GameConfig::volume_offset+1]=80;config.bytes[GameConfig::volume_offset+2]=0;refresh_options();sounds.push_back(10);}
        }break;
    case 4:
        if(timer.current>9){if(cursor.selected==2){change(TitleScreen::Keys);cursor.push();}
            else if(cursor.selected==4){change(TitleScreen::Main);cursor.pop();}}
        break;
    }
}
void TitleMenu::practice(u32 pressed,u32 repeat){
    switch(substate){
    case 0:cursor.count=6;cursor.select(selection.last_practice);if constexpr(kPortTitle==14)create(102);else{create(104);create(105+selection.character);}step(1);[[fallthrough]];
    case 1:if(timer.current>10)step(2);break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected)sounds.push_back(12);
        if(pressed&0x102){step(4);sounds.push_back(11);selection.last_practice=cursor.selected;return;}
        if(pressed&0x80001){
            const auto& record=scores.characters[selection.character*2+selection.partner];
            if(record[ScoreFile::practice_flags_offset+9+(cursor.selected+selection.difficulty*ScoreFile::practice_stage_stride)*8]){step(3);sounds.push_back(10);if constexpr(kPortTitle==14){sounds.push_back(50);music_fade=0;}}
            else{sounds.push_back(kPortTitle==14?16:37);if constexpr(kPortTitle==14)return;}
            selection.last_practice=cursor.selected;selection.practice_start=0;
            for(i32 n=0;n<9;++n)if(number_keys&(1u<<n)){selection.practice_start=n+1;break;}
        }break;
    case 3:
        if(timer.current==10)fade_requested=true;
        if(timer.current>=40){cursor.push();change(TitleScreen::Exit);selection.stage=cursor.selected+1;if constexpr(kPortTitle==14)selection.last_practice=cursor.selected;start_requested=true;}break;
    case 4:
        if(timer.current>=6){if constexpr(kPortTitle==14)close(102);else{close(104);close(105+selection.character);}change(TitleScreen::Partner);cursor.pop();}break;
    }
}
}
