#include "TitleMenu.hpp"
namespace th11 {
void TitleMenu::replays(u32 pressed,u32 repeat){
    switch(substate){
    case 0:
        cursor.count=25;cursor.select(last_replay%25);
        page.count=i32(replay_files.size()/25);page.select(last_replay/25);page.wrap=1;last_replay=0;
        if(!exists(92)){create(92);create(18,193,2);}create(99);step(1);
        clear_replays();flags&=~12u;replay_scan_requested=true;
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        move(pressed|repeat,16,32);page.previous=page.selected;
        if((pressed|repeat)&64)page.move(-1);
        if((pressed|repeat)&128)page.move(1);
        if(page.previous!=page.selected)sounds.push_back(12);
        if(cursor.previous!=cursor.selected)sounds.push_back(12);
        if(pressed&0x102){step(5);sounds.push_back(11);flags|=4;return;}
        if((pressed&0x80001)&&replay_files[page.selected*25+cursor.selected]){
            step(4);replay_file=page.selected*25+cursor.selected;cursor.push();sounds.push_back(10);
            cursor.count=7;cursor.select(0);
            for(i32 i=0;i<7;++i)if(!replay_files[replay_file]->has_stage(i+1))cursor.disabled[cursor.disabled_count++]=i;
            cursor.move(-1);cursor.move(1);
        }break;
    case 3:
        if(timer.current==2)fade_requested=true;
        if(timer.current>=32&&(flags&8)){
            const auto& r=*replay_files[replay_file];
            change(TitleScreen::Exit);selection.stage=replay_stage+1;
            selection.character=r.character();selection.partner=r.subtype();selection.difficulty=r.difficulty();
            last_replay=replay_file;replay_start_requested=true;
        }break;
    case 4:
        if(timer.current<=14)break;
        move(pressed|repeat,16,32);if(cursor.previous!=cursor.selected)sounds.push_back(12);
        if(pressed&0x102){cursor.pop();cursor.count=25;cursor.disabled_count=0;step(2);sounds.push_back(11);return;}
        if(pressed&0x80001){replay_stage=cursor.selected;step(3);flags|=4;}break;
    case 5:
        if(timer.current>=6&&(flags&8)){clear_replays();close(99);close(92);family(193,1);
#ifdef TH11_MULTIPLAYER
            if(multiplayer_replay_menu){change(TitleScreen::Exit);break;}
#endif
            change(TitleScreen::Main);cursor.pop();}break;
    }
}
}
