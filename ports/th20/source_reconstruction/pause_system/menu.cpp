#include "menu_support.hpp"
#include "capture.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../progress_state/manager.hpp"
#include "../gameplay/player_state.hpp"
#include "../hud_system/hud.hpp"
#include "../replay_system/replay.hpp"
#include "../startup_scene/startup.hpp"
#include "../input/input.hpp"
#include <cstdio>
#include <cstring>
namespace th20::source::pause {
namespace pe=program_entry;namespace ps=gameplay::player_state;namespace pr=progress;
namespace {
bool repeated(std::uint32_t mask){auto* buttons=input::button_slot(0);return buttons&&((buttons->repeat8|buttons->pressed)&mask)!=0;}
void interrupt(std::uint32_t handle,int event){sprite::interrupt_animation_children(*pe::sprite_controller,handle,event);}
void select_panel(PauseInf& o,int bias){interrupt(o.panel_handle,static_cast<std::int16_t>(o.cursor.current+bias));}
void interrupt_child(PauseInf& o,int script,int event){auto* a=sprite::find_animation_child(*pe::sprite_controller,o.panel_handle,script,0);std::uint32_t handle=a?a->handle:0;interrupt(handle,event);}
void move_vertical(menu::Cursor& c){c.snapshot();if(repeated(0x10))c.move(-1);if(repeated(0x20))c.move(1);}
void resume_immediately(PauseInf& o){interrupt(o.background_handle,1);interrupt(o.panel_handle,1);o.cursor.select(0);set_substate(o,18);}
void save_default_name(PauseInf& o){strcpy_s(reinterpret_cast<char*>(pr::manager->current.metadata.bytes+12),10,o.player_name);}
void result_menu(PauseInf& o,Services& e){
    set_substate(o,6);o.cursor.count=6;o.cursor.wrapping=1;
    if((o.menu_flags&4)||e.session().mode!=0){o.panel_handle=e.spawn_panel(*o.file,0x94);exclude(o.cursor,0);exclude(o.cursor,3);o.cursor.select(0);if(!(o.menu_flags&4))o.cursor.select(5);}
    else {o.panel_handle=e.spawn_panel(*o.file,0x93);if(continue_count(e.session())>0)exclude(o.cursor,2);if(credits(e.session())<1){exclude(o.cursor,0);o.cursor.select(1);}else o.cursor.select(0);}
    sprite::execute_animation_interrupt(*pe::sprite_controller,o.panel_handle,3);select_panel(o,7);
}
void confirm_retry(PauseInf& o,Services& e){
    e.effect(7);interrupt_child(o,0x7c,6);o.cursor.select(5);
    if(e.game().restart()==0&&o.state==1)set_substate(o,7);else {e.delete_animation(o.background_handle);set_substate(o,18);}
}
}
void update_menu(PauseInf& o,Services& e){
    switch(o.substate){
    case 0:
        if(o.age.current>=10){set_substate(o,6);o.cursor.count=6;if(e.game().restart()!=0){exclude(o.cursor,2);exclude(o.cursor,3);}
            if(continue_count(e.session())>0){exclude(o.cursor,2);select_panel(o,7);for(int child:{0x79,0x7f,0x8a})interrupt_child(o,child,5);}
            o.cursor.wrapping=1;o.cursor.select(0);select_panel(o,7);o.cancel_disabled=0;}
        break;
    case 1:
        if(o.age.current>=10){set_substate(o,6);o.cursor.count=6;for(int value:{3,2,0,4})exclude(o.cursor,value);o.cursor.wrapping=1;o.cursor.select(1);select_panel(o,7);o.cancel_disabled=1;}
        break;
    case 2:case 3:case 4:case 5:
        if(o.age.current>=10){ //4b5ae0: the original high-score comparison ignores score high word
            auto& h=*hud::controller;h.score=ps::score(e.session().player_table.players[0]);const auto best=pr::read<std::uint64_t>(&e.session(),0x60);
            if((best>>32)==0&&static_cast<std::uint32_t>(best)<static_cast<std::uint32_t>(h.score))pr::write(&e.session(),0x60,h.score);
            update_ranking(o);if(o.field_e4==0){set_substate(o,15);show_animation(o.panel_handle,false);}else result_menu(o,e);}
        break;
    case 6:
        if(o.age.current<=1&&o.state==2&&credits(e.session())<1)exclude(o.cursor,0);
        move_vertical(o.cursor);if(o.cursor.changed()){select_panel(o,7);e.effect(10);}
        if(e.pressed(0x80001)){
            e.effect(7);switch(o.cursor.current){
            case 0:interrupt(o.background_handle,1);interrupt(o.panel_handle,1);set_substate(o,18);break;
            case 1:for(int child:{0x78,0x7e,0x84,0x87,0x89})interrupt_child(o,child,6);set_substate(o,e.game().restart()==0&&o.state==1?7:18);break;
            case 2:for(int child:{0x79,0x7f,0x8a})interrupt_child(o,child,6);set_substate(o,o.state==1?9:10);break;
            case 3:for(int child:{0x7a,0x80})interrupt_child(o,child,6);set_substate(o,14);break;
            case 4:for(int child:{0x7b,0x81})interrupt_child(o,child,6);set_substate(o,16);break;
            case 5:confirm_retry(o,e);return;
            }recovered::timer_set(o.age,0);
        }
        if(o.cancel_disabled==0){if(e.pressed(0x200000)&&!o.cursor.is_excluded(5)){confirm_retry(o,e);break;}if(e.pressed(0x100)){resume_immediately(o);break;}}
        if(e.pressed(0x10000)){e.effect(7);interrupt_child(o,0x78,6);o.cursor.select(1);set_substate(o,18);}break;
    case 7:case 9:
        if(o.age.current<20)break;
        if(o.age.current==20){save_cursor(o.cursor);o.cursor.count=2;o.cursor.wrapping=1;o.cursor.select(1);interrupt(o.panel_handle,14);}
        if(o.age.current<30)break;
        if(o.age.current==30)select_panel(o,15);
        move_vertical(o.cursor);if(o.cursor.changed()){select_panel(o,15);e.effect(10);}
        if(e.pressed(0x80001)){if(o.cursor.current==0){interrupt_child(o,0x8e,6);set_substate(o,o.substate==9?10:8);e.effect(7);}else if(o.cursor.current==1){interrupt_child(o,0x8f,6);set_substate(o,8);e.effect(9);}}
        if(e.pressed(0x106)){e.effect(9);if(o.cursor.current==0){o.cursor.select(1);select_panel(o,15);}else if(o.cursor.current==1){interrupt_child(o,0x8f,6);set_substate(o,8);}}
        if(e.pressed(0x100)&&o.state==1)resume_immediately(o);break;
    case 8:
        if(o.age.current>=20){if(o.cursor.current==0){interrupt(o.panel_handle,1);restore_cursor(o.cursor);set_substate(o,18);}else if(o.cursor.current==1){restore_cursor(o.cursor);if(continue_count(e.session())>0)exclude(o.cursor,2);select_panel(o,7);set_substate(o,6);}}break;
    case 10:
        if(o.age.current>=20){o.menu_flags=(o.menu_flags&~3u)|1;set_substate(o,11);show_animation(o.panel_handle,false);save_cursor(o.cursor);o.cursor.count=25;o.cursor.wrapping=1;o.cursor.select(0);
            for(int index=1;index<26;++index){char filename[64];sprintf_s(filename,"th20_%.2d.rpy",index);o.metadata[index-1]=replay::read_metadata(filename);}}
        break;
    case 11:
        if(o.age.current<10)break;
        move_vertical(o.cursor);if(o.cursor.changed())e.effect(10);
        if(e.pressed(0x80001)){o.menu_flags=(o.menu_flags&~3u)|2;set_substate(o,12);o.name_cursor.select(0);o.name_cursor.count=static_cast<int>(std::strlen(name_characters()));o.name_cursor.wrapping=1;
            replay::prepare_save(*replay::controller(),o.completed!=0&&e.session().mode==0?1:0);initialize_name(o);e.effect(7);}
        else if(e.pressed(0x106)){o.menu_flags&=~3u;restore_cursor(o.cursor);o.cursor.count=6;o.cursor.wrapping=1;select_panel(o,7);
            for(auto*& value:o.metadata){runtime::retire_callback_owner(value);value=nullptr;}
            if(o.state==1){set_substate(o,18);o.cursor.select(1);}else {set_substate(o,6);show_animation(o.panel_handle,true);if(e.session().mode!=0){exclude(o.cursor,0);exclude(o.cursor,3);}}e.effect(9);}
        break;
    case 12:case 15:{
        if(o.age.current<10)break;auto& c=o.name_cursor;c.snapshot();if(repeated(0x10))c.move(-13);if(repeated(0x20))c.move(13);
        if(repeated(0x40))c.move(c.current%13==0?12:-1);if(repeated(0x80))c.move(c.current%13==12?-12:1);if(c.changed())e.effect(10);
        if(!e.pressed(0x80001)){if(e.pressed(0x106)){e.effect(9);if(o.name_length==0){if(o.substate==12){o.menu_flags=(o.menu_flags&~3u)|1;set_substate(o,11);}}else o.player_name[--o.name_length]=' ';}break;}
        const int length=static_cast<int>(std::strlen(name_characters()));
        if(c.current<length-3){const char character=name_characters()[c.current];if(o.name_length<8){o.player_name[o.name_length++]=character;if(o.name_length>7)c.select(length-1);}else o.player_name[o.name_length-1]=character;}
        else if(c.current==length-3){if(o.name_length<8){o.player_name[o.name_length++]=' ';if(o.name_length>7)c.select(length-1);}else o.player_name[o.name_length-1]=' ';}
        else if(c.current==length-2){if(o.name_length!=0){o.player_name[--o.name_length]=' ';e.effect(9);}break;}
        else if(c.current==length-1){
            if(o.substate!=12){e.effect(7);auto* record=pr::current_profile()->bytes+0x18+ps::difficulty(e.session().player_table)*400+o.cursor.current*40;strcpy_s(reinterpret_cast<char*>(record+10),10,o.player_name);save_default_name(o);result_menu(o,e);break;}
            o.menu_flags=(o.menu_flags&~3u)|1;e.effect(17);char filename[64];sprintf_s(filename,"th20_%.2d.rpy",o.cursor.current+1);
            runtime::retire_callback_owner(o.metadata[o.cursor.current]);replay::save(*replay::controller(),filename,o.player_name,0,1);o.metadata[o.cursor.current]=replay::read_metadata(filename);set_substate(o,11);save_default_name(o);
        }
        e.effect(7);break;}
    case 14:
        if(o.age.current==20){show_animation(o.panel_handle,false);unrecovered::create_help();pr::write(startup::unrecovered::owner_005c4d24,0xac,32.f);}
        if(auto* options=startup::unrecovered::owner_005c4d24;options&&pr::read<int>(options,0xa8)!=0){runtime::retire_callback_owner(options);set_substate(o,6);show_animation(o.panel_handle,true);}break;
    case 16:
        if(o.age.current==20){show_animation(o.panel_handle,false);unrecovered::create_options({52.f,76.f,0.f});pr::write(startup::unrecovered::owner_005c60b8,0x9c,1);}
        if(o.age.current>20&&startup::unrecovered::owner_005c60b8==nullptr){set_substate(o,6);show_animation(o.panel_handle,true);}break;
    case 18:if(o.age.current>=12)finish_choice(o,e);break;
    }
}
}
