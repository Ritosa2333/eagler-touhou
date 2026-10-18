#include "bullet.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../gameplay/gameplay.hpp"
#include <cstring>
#include <new>
namespace th20::source::bullet {
namespace pe=program_entry;
namespace {
int __cdecl update_callback(void* p){auto& c=*static_cast<Controller*>(p);if(gameplay::controller&&gameplay::controller->update_suppressed())return 1;++c.age;return c.update();}
int __cdecl draw_callback(void* p){if(gameplay::controller&&(gameplay::controller->game_flags&4u))return 1;return static_cast<Controller*>(p)->draw();}
}
Controller::Controller(){scheduler::initialize_list(free);scheduler::initialize_list(active);}
Controller::~Controller(){
    for(auto& value:pool)value.metadata.reset();
    game_session::context(view_index).primary_owner=nullptr;
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
    sprite::mark_file_animations(*pe::sprite_controller,pe::sprite_controller->files[7],false);
}
int Controller::initialize(std::int32_t index){
    file=sprite::load_animation_file(*pe::sprite_controller,7,"bullet.anm",pe::log_buffer,pe::graphics_event_flags);
    if(!file){runtime::log_printf(pe::log_buffer,"\x93\x47\x92\x65\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");return -1;}
    game_session::context(index).primary_owner=this;select_context(index);
    pe::sprite_controller->field_6c4=static_cast<std::uint32_t>(view_index);next_bullet=pool;pool[2000].state=5;
    update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,38,&update_callback,this,false,false);
    draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,41,&draw_callback,this,true,false);
    scheduler::initialize_list(free);scheduler::initialize_list(active);
    for(std::uint32_t i=0;i<2000;++i){pool[i].index=i;scheduler::insert_after(free.sentinel,pool[i].link);pool[i].link.owner=&free;if(free.tail==&free.sentinel)free.tail=&pool[i].link;}
    return 0;
}
Controller* create_controller(std::int32_t index){auto* memory=::operator new(sizeof(Controller),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Controller));auto* result=new(memory)Controller;if(result->initialize(index)){runtime::retire_callback_owner(result);return nullptr;}return result;}
void destroy_controller(std::int32_t index){auto& value=game_session::context(index).primary_owner;runtime::retire_callback_owner(value);value=nullptr;}
}
