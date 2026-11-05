#include "notice.hpp"
#include "../help_system/help.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../text_renderer/centered.hpp"
#include "../stone_menu/stone.hpp"
#include "../input/input.hpp"
#include "../audio_runtime/audio.hpp"
#include <cstdio>
#include <stdexcept>
namespace th20::source::notice {
namespace pe=program_entry;
void spawn(NoticeInf& o,int index){sprite::spawn_named_animation(*pe::sprite_controller,*o.file,o.handles[index],"notice",index,nullptr,0.f,-1,4);}
namespace {
bool pressed(){const auto* b=input::button_slot(2);return b&&(b->pressed&0x80001u)!=0;}
void effect(int id){pe::thread_registry.request_effect(id,0);}
void interrupt(NoticeInf& o,int index){sprite::interrupt_animation_children(*pe::sprite_controller,o.handles[index],1);}
void show(NoticeInf& o){
    o.substate=4;for(int index:{0,4,5,6})spawn(o,index);
    const int index=o.cursor.current;if(index<0||index>=static_cast<int>(messages.size()))throw std::out_of_range("Notice index outside original25 message table");
    auto& sprites=*pe::sprite_controller;auto& title=*sprite::resolve_animation_handle(sprites,o.handles[5]);
    // Five original completion wrappers all resolve46ebb0->46a4b0->40e5e0,
    // a verified empty function. The actual two text uploads remain queued.
    if(index>=1&&index<=24){const int stone=(index-1)%8;text::write_midpoint_animation_text(sprites,title,0xffd080,0,7,0,nullptr,{},messages[index][0].c_str(),stone_menu::controller->names[stone*4]);}
    else text::write_midpoint_animation_text(sprites,title,0xffd080,0,7,0,nullptr,{},messages[index][0].c_str());
    text::write_midpoint_animation_text(sprites,*sprite::resolve_animation_handle(sprites,o.handles[6]),0xffd080,0,8,0,nullptr,{},messages[index][1].c_str());
    recovered::timer_set(o.age,0);sprintf_s(o.filename,"notice_%.2d.png",index);
}
}
int update(NoticeInf& o){
    if(o.state==0)o.state=1;
    else if(o.state==2){if(o.age.current>=30)o.finished=1;}
    else if(o.state==3){if(o.age.current>=240)o.finished=1;}
    else if(o.state==1)switch(o.substate){
    case 0:o.cursor.count=99;o.cursor.select(o.selected_index);o.cursor.wrapping=1;help::clear_texture(o.file->textures[1]);o.substate=1;effect(83);show(o);break;
    case 3:
        help::replace_texture_image(o.file->textures[1],o.image_bytes,o.image_size,1,false);if(o.image_bytes){runtime::release_bytes(o.image_bytes);o.image_bytes=nullptr;}o.image_bytes=nullptr;
#ifdef TH_SDL3
        web::shared_device().preload(o.file->textures[1].texture);
#else
        o.file->textures[1].texture->PreLoad();
#endif
        spawn(o,0);spawn(o,4);o.substate=4;recovered::timer_set(o.age,0);
        [[fallthrough]];
    case 4:
        if(o.age.current>=20&&pressed()){o.state=2;o.substate=0;recovered::timer_set(o.age,0);effect(7);o.cursor.move(1);interrupt(o,4);interrupt(o,0);}break;
    case 5:if(o.age.current>=20)show(o);break;
    case 6:
        o.secondary_handle=sprite::spawn_named_animation(*pe::sprite_controller,*o.secondary_file,nullptr,1);o.substate=7;recovered::timer_set(o.age,0);
        [[fallthrough]];
    case 7:
        if(o.age.current>=240&&pressed()&&o.cursor.current>0){o.state=3;o.substate=0;recovered::timer_set(o.age,0);effect(7);o.cursor.move(1);sprite::interrupt_animation_children(*pe::sprite_controller,o.secondary_handle,1);}break;
    }
    recovered::timer_tick(o.age,state::timer_rate);return 1;
}
}
