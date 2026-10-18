#include "rank_entry.hpp"
#include "../pause_system/menu_support.hpp"
#include <cstring>
namespace th20::source::title {
int update_rank_entry(TitleInf& o,RankEnvironment& e){
    auto& selection=e.selection;auto& m=selection.main;auto& s=m.session;auto& c=o.cursor;auto& name_cursor=o.cursor56e8;auto* name=entered_name(o);
    const char* characters=pause::name_characters();const int count=static_cast<int>(std::strlen(characters));
    auto character=[&]{return static_cast<int>(s.contexts[0].current_player->fields_00[2]);};auto profile=[&]{return static_cast<int>(s.contexts[0].current_player->fields_00[3]);};
    auto erase=[&](int index){m.interrupt(o,index,1,true);};
    auto append=[&](char value){if(static_cast<int>(o.word56e0)<8){name[o.word56e0++]=value;if(static_cast<int>(o.word56e0)>7)name_cursor.select(count-1);}else name[o.word56e0-1]=value;};
    switch(o.phase){
    case 0:{
        c.count=30;e.play_score_music();if(o.handle390==0)selection.spawn_text_overlay(o);m.spawn(o,41);set_phase(o,1);e.open_stones();e.select_stone(profile());
        m.spawn(o,72);selection.signal(o,72,profile()/2+37+character()*4,true);m.spawn(o,static_cast<int>(s.player_table.field_1e0)+64);
        if(!m.exists(o.handles[0])){m.spawn(o,0);m.interrupt(o,0,3,false);}
        const int rank=e.insert_score();e.select_stage_zero();
        if(rank<0){c.select(-1);o.word56e4=1;}else{
            recovered::timer_set(o.age,0);c.wrapping=1;c.select(rank);name_cursor.select(0);name_cursor.count=count;name_cursor.wrapping=1;strcpy_s(name,10,e.default_name());if(std::strcmp(name,"        ")!=0)name_cursor.move(-1);
            int length=8;while(length>0&&name[length-1]==' ')--length;o.word56e0=length;o.word56e4=0;
        }
        [[fallthrough]];
    }
    case 1:if(o.age.current>6)set_phase(o,2);break;
    case 2:
        if(o.word56e4==0){
            name_cursor.snapshot();if(m.repeated(0x10))name_cursor.move(-13);if(m.repeated(0x20))name_cursor.move(13);
            if(m.repeated(0x40))name_cursor.move(name_cursor.current%13==0?12:-1);
            if(m.repeated(0x80))name_cursor.move(name_cursor.current%13==12?-12:1);
            if(name_cursor.changed())m.sound(10);
        }
        if(m.pressed(0x80001)){
            if(o.word56e4==0){
                if(name_cursor.current<count-3)append(characters[name_cursor.current]);
                else if(name_cursor.current==count-3)append(' ');
                else if(name_cursor.current==count-2){if(o.word56e0==0)return 1;name[--o.word56e0]=' ';}
                else if(name_cursor.current==count-1){strcpy_s(reinterpret_cast<char*>(e.profile().bytes+0x18+static_cast<int>(s.player_table.field_1e0)*400+c.current*40+10),10,name);strcpy_s(e.default_name(),10,name);set_phase(o,3);}
                m.sound(7);
            }else{set_phase(o,3);m.sound(7);}
        }
        if(m.pressed(0x106)){
            if(o.word56e4==0){if(o.word56e0!=0){m.sound(9);name[--o.word56e0]=' ';}}
            else{set_phase(o,3);m.sound(7);}
        }
        break;
    case 3:
        if(o.age.current>=6){
            e.hide_stones();erase(41);erase(character()+60);erase(static_cast<int>(s.player_table.field_1e0)+64);erase(72);
            if(pause::continue_count(s)==0)set_state(o,16);
            else{e.retire_replay();m.interrupt_handle(o.handle390,1);o.handle390=0;pause::restore_cursor(c);set_state(o,1);e.play_title_music();}
        }
        break;
    }
    return 1;
}
}
