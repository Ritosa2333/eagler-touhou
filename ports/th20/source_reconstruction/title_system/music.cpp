#include "music.hpp"
#include "music_data.hpp"
#include "pages.hpp"
#include "data.hpp"
#include "../pause_system/menu_support.hpp"
#include <stdexcept>
namespace th20::source::title {
namespace {
char* track(TitleInf& o,int index){if(index<0||index>=32)throw std::out_of_range("Music track index outside32 records");return reinterpret_cast<char*>(o.data490)+index*0x40;}
int number(unsigned value){return recovered::signed_bits(value);}
}
int update_music(TitleInf& o,MusicEnvironment& e){
    auto& c=o.cursor;auto& count=o.words47c[0];auto& rows=o.words47c[1];auto& playing=o.words47c[2];auto& previous=o.words47c[3];auto& warning=o.words47c[4];
    switch(o.phase){
    case 0:
        if(!o.handle390)e.spawn_background(o);e.spawn_heading(o);o.flag478.store(false,std::memory_order_seq_cst);e.begin_read(o);rows=playing=warning=0;set_phase(o,1);
        [[fallthrough]];
    case 1:recovered::timer_set(o.age,-1);if(!o.flag478.load(std::memory_order_seq_cst))return 1;o.phase=2;break;
    case 2:
        c.snapshot();
        if(!warning){
            if(e.repeated(0x10))c.move(-1);if(e.repeated(0x20))c.move(1);
            if(c.changed()){e.sound(10);if(c.current<number(o.word56d0))o.word56d0=unsigned(c.current);else if(c.current>=recovered::signed_bits(o.word56d0+10u))o.word56d0=unsigned(c.current)-9u;}
        }else if(e.repeated(0x10)||e.repeated(0x20)||e.repeated(0x106)){playing=previous;rows=0;e.play(track(o,number(playing)));warning=0;}
        if(number(rows)<8)++rows;
        if(e.pressed(0x80001)){
            for(auto handle:o.handles454)e.interrupt(handle,3);previous=playing;playing=unsigned(c.current);rows=0;
            if(!e.unlocked(number(playing))&&!warning){e.stop();warning=1;}
            else{e.play(track(o,c.current));warning=0;}
        }else if(e.pressed(0x106)){
            if(count>32)throw std::out_of_range("Music animation count outside32 slots");for(unsigned i=0;i<count;++i)e.interrupt(o.handles3d4[i],1);for(auto handle:o.handles454)e.interrupt(handle,1);e.sound(9);set_phase(o,3);
        }
        break;
    case 3:
        if(o.age.current>=10){e.retire_heading(o);e.interrupt(o.handle390,1);o.handle390=0;set_state(o,1);e.play(data::s_00575098);pause::restore_cursor(c);}break;
    }
    return 0;
}
}
