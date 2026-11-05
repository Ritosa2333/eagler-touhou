#include "title.hpp"
#include "data.hpp"
#include "../options_system/options.hpp"
#include "../help_system/help.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::title {
void update_options(TitleInf& o){
    if(o.phase==0){options::create({data::f_00571054,data::f_0056fa30,0});set_phase(o,1);}
    else if(o.phase==1&&!options::controller()){set_state(o,1);activate(o,0);}
}
int update_help(TitleInf& o){
    if(o.phase==0){
        if(o.handle390==0)o.handle390=sprite::spawn_named_animation(*program_entry::sprite_controller,*text::renderer->animation_file,nullptr,19);
        spawn(o,45);help::create();set_phase(o,1);help::controller()->x=data::f_0056e050;
    }else if(o.phase==1&&help::controller()->finished){
        sprite::interrupt_animation_children(*program_entry::sprite_controller,o.handle390,1);o.handle390=0;retire_animation(o,45);set_state(o,1);runtime::retire_callback_owner(help::controller());
    }
    return 0;
}
}
