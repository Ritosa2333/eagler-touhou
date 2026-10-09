#include "TitleMenu.hpp"
#include <cstring>
namespace th11 {
void TitleMenu::name_begin(){
    name_cursor.select(0);name_cursor.count=91;name_cursor.wrap=1;
    std::memcpy(entered_name.data(),scores.settings.data()+12,8);entered_name[8]=0;
    if(std::memcmp(entered_name.data(),"        ",8))name_cursor.move(-1);
    name_length=8;while(name_length>0&&entered_name[name_length-1]==' ')--name_length;
}
void TitleMenu::name_move(u32 input){
    auto& c=name_cursor;c.previous=c.selected;
    if(input&16)c.move(-13);if(input&32)c.move(13);
    if(input&64)c.move(c.selected%13==0?12:-1);
    if(input&128)c.move(c.selected%13==12?-12:1);
    if(c.previous!=c.selected)sounds.push_back(12);
}
i32 TitleMenu::name_confirm(){
    static constexpr char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
    const i32 index=name_cursor.selected;
    if(index<89){const char c=index==88?' ':letters[index];
        if(name_length<8){entered_name[name_length++]=c;if(name_length>=8)name_cursor.select(90);}
        else entered_name[name_length-1]=c;
    }else if(index==89){if(!name_length)return -1;entered_name[--name_length]=' ';}
    else if(index==90)return 1;
    return 0;
}
void TitleMenu::results(u32 pressed,u32 repeat){
#ifdef TH11_MULTIPLAYER
    if(multiplayer_result){multiplayer_results(pressed);return;}
#endif
    switch(substate){
    case 0:{
        cursor.count=30;music_request=17;
        if(!exists(92)){create(92);create(18,193,2);}create(102);step(1);
        create(selection.character+150);create(selection.character*3+selection.partner+152);create(selection.difficulty+158);
        const i32 rank=scores.insert_score(selection.character*3+selection.partner,selection.difficulty,result_score,8,result_continues,result_timestamp,result_slowdown);
        selection.stage=0;
        if(rank<0){cursor.select(-1);result_unranked=1;}
        else{timer.set(0,&animations.rate);cursor.wrap=1;cursor.select(rank);name_begin();result_unranked=0;}
        [[fallthrough]];
    }
    case 1:if(timer.current>6)step(2);break;
    case 2:
        if(!result_unranked)name_move(pressed|repeat);
        if(pressed&0x80001){
            if(result_unranked)step(3);
            else{const i32 action=name_confirm();if(action<0)return;if(action==1){scores.score_name(selection.character*3+selection.partner,selection.difficulty,cursor.selected,entered_name.data());step(3);}}
            sounds.push_back(10);
        }
        if(pressed&0x102){if(result_unranked){step(3);sounds.push_back(10);return;}if(name_length){sounds.push_back(11);entered_name[--name_length]=' ';}}break;
    case 3:
        if(timer.current>=6){close(102);close(selection.character+150);close(selection.character*3+selection.partner+152);close(selection.difficulty+158);change(TitleScreen::ReplaySave);}break;
    }
}
void TitleMenu::replay_save(u32 pressed,u32 repeat){
    switch(substate){
    case 0:
        cursor.count=25;cursor.wrap=1;cursor.select(0);selection.stage=8;
        replay_files.fill(nullptr);replay_scan_requested=true;create(103);step(1);
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected)sounds.push_back(12);
        if(pressed&0x102){step(4);sounds.push_back(11);}
        else if(pressed&0x80001){replay_file=cursor.selected;name_begin();sounds.push_back(10);step(3);}break;
    case 3:
        name_move(pressed|repeat);
        if(pressed&0x80001){const i32 action=name_confirm();if(action<0)break;
            if(action==1){sounds.push_back(10);replay_save_requested=true;std::memcpy(scores.settings.data()+12,entered_name.data(),9);step(2);}
            sounds.push_back(10);
        }
        if(pressed&0x102){if(name_length){sounds.push_back(11);entered_name[--name_length]=' ';}else step(2);}break;
    case 4:
        if(timer.current>=6){close(103);close(92);family(193,1);change(TitleScreen::Main);cursor.pop();music_request=0;replay_files.fill(nullptr);pending_replay.reset();}break;
    }
}
}
