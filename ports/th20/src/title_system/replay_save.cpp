#include "replay_save.hpp"
#include "../pause_system/menu_support.hpp"
#include <algorithm>
#include <cstring>
#include <cstdio>
namespace th20::source::title {
void update_replay_save(TitleInf& o,ReplaySaveEnvironment& e){
    auto& m=e.selection.main;auto& c=o.cursor;auto& n=o.cursor56e8;auto* name=entered_name(o);const char* characters=pause::name_characters();const int count=static_cast<int>(std::strlen(characters));
    auto append=[&](char value){if(static_cast<int>(o.word56e0)<8){name[o.word56e0++]=value;if(static_cast<int>(o.word56e0)>7)n.select(count-1);}else name[o.word56e0-1]=value;};
    switch(o.phase){
    case 0:
        c.count=25;c.wrapping=1;c.select(0);e.select_stage_eight();for(int index=1;index<26;++index){char filename[64];sprintf_s(filename,"th20_%.2d.rpy",index);o.metadata[index-1]=e.read_metadata(filename);}
        if(!m.exists(o.handles[0])){m.spawn(o,0);m.interrupt(o,0,3,false);}m.spawn(o,42);set_phase(o,1);
        [[fallthrough]];
    case 1:if(o.age.current>6)set_phase(o,2);break;
    case 2:
        c.snapshot();if(m.repeated(0x10))c.move(-1);if(m.repeated(0x20))c.move(1);if(c.changed())m.sound(10);
        if(m.pressed(0x106)){set_phase(o,4);m.sound(9);}
        else if(m.pressed(0x80001)){
            o.words5734[1]=static_cast<unsigned>(c.current);n.select(0);n.count=count;n.wrapping=1;e.prepare_save();strcpy_s(name,10,e.default_name());o.word56e0=0;
            if(std::strcmp(name,"        ")!=0)n.move(-1);int length=8;while(length>0&&name[length-1]==' ')--length;o.word56e0=length;m.sound(7);set_phase(o,3);
        }
        break;
    case 3:
        n.snapshot();if(m.repeated(0x10))n.move(-13);if(m.repeated(0x20))n.move(13);if(m.repeated(0x40))n.move(n.current%13==0?12:-1);if(m.repeated(0x80))n.move(n.current%13==12?-12:1);if(n.changed())m.sound(10);
        if(m.pressed(0x80001)){
            if(n.current<count-3)append(characters[n.current]);else if(n.current==count-3)append(' ');
            else if(n.current==count-2){if(o.word56e0==0)break;name[--o.word56e0]=' ';}
            else if(n.current==count-1){
                m.sound(17);char filename[64];sprintf_s(filename,"th20_%.2d.rpy",c.current+1);e.retire(o.metadata[c.current]);e.save(filename,name);o.metadata[c.current]=e.read_metadata(filename);strcpy_s(e.default_name(),10,name);set_phase(o,2);
            }
            m.sound(7);
        }
        if(m.pressed(0x106)){if(o.word56e0==0)set_phase(o,2);else{m.sound(9);name[--o.word56e0]=' ';}}
        break;
    case 4:
        if(o.age.current>=6){
            m.interrupt(o,42,1,true);m.interrupt_handle(o.handle390,1);o.handle390=0;set_state(o,1);pause::restore_cursor(c);e.retire_replay();e.play_title_music();for(int i=0;i<25;++i)e.retire(o.metadata[i]);std::fill(std::begin(o.metadata),std::end(o.metadata),nullptr);
        }
        break;
    }
}
}
