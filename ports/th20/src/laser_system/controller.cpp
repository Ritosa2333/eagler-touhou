#include "laser.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/gameplay.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/dispatch.hpp"
#include <new>
#include <cstring>
namespace th20::source::laser {
namespace pe=program_entry;
namespace {
int __cdecl update_callback(void* value){
    auto& game=*gameplay::controller;if(game.update_suppressed()||(game.game_flags&0x800u))return 1;
    auto& c=*static_cast<Controller*>(value);if(!game.animation_frozen())return c.update();
    const auto previous=state::clock_scale;state::set_clock_scale(0);const auto result=c.update();state::set_clock_scale(previous);return result;
}
int __cdecl draw_callback(void* value){if(gameplay::controller->game_flags&4u)return 1;return static_cast<Controller*>(value)->draw();}
}
Controller::Controller():count(0),next_handle(0x10000),cancel_position{},cancel_size{},file(nullptr),field_4c(0),view_index(0),context(nullptr){scheduler::initialize_list(active);}
Controller::~Controller(){scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);clear();}
Controller* controller(std::int32_t index) noexcept {return static_cast<Controller*>(game_session::context(index).objects_04[4]);}
void Controller::select_context(std::int32_t index) noexcept {view_index=index;context=&game_session::context(index);}
int Controller::initialize(std::int32_t index){
    file=sprite::load_animation_file(*pe::sprite_controller,7,"bullet.anm",pe::log_buffer,pe::graphics_event_flags);
    if(!file){runtime::log_printf(pe::log_buffer,"\x93\x47\x92\x65\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");return -1;}
    update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,37,&update_callback,this,false,false);
    draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,39,&draw_callback,this,true,false);
    game_session::context(index).objects_04[4]=this;select_context(index);scheduler::initialize_list(active);return 0;
}
Controller* create_controller(std::int32_t index){auto* memory=::operator new(sizeof(Controller),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Controller));auto* result=new(memory)Controller;if(result->initialize(index)){runtime::retire_callback_owner(result);return nullptr;}return result;}
void destroy_laser(Laser* value){if(!value)return;value->~Laser();std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(value);}
void Controller::attach(Laser& value){scheduler::initialize_link(value.link,reinterpret_cast<scheduler::Node*>(&value));scheduler::insert_after(active.sentinel,value.link);value.link.owner=&active;if(active.tail==&active.sentinel)active.tail=&value.link;++count;}
void Controller::detach(Laser& value){scheduler::unlink(value.link);--count;}
void Controller::clear(){scheduler::Iterator iterator(active.sentinel.next);for(;iterator.current;iterator.advance()){auto* value=reinterpret_cast<Laser*>(iterator.current->value);scheduler::unlink(value->link);destroy_laser(value);}count=0;}
int Controller::update(){
    pe::sprite_controller->field_6c4=static_cast<std::uint32_t>(view_index);
    scheduler::Iterator iterator(active.sentinel.next);
    for(;iterator.current;iterator.advance()){
        auto& value=*reinterpret_cast<Laser*>(iterator.current->value);bool remove=false;
        if((value.flags>>1)&3u){value.flags=(value.flags&~6u)|(((((value.flags>>1)&3u)+1u)&3u)<<1);if(((value.flags>>1)&3u)>=2)remove=true;}
        if(!remove){
            if(value.state==1)remove=true;
            else if(value.flags&8u)value.cancel_all(1);
            else if(value.update())remove=true;
            else{recovered::timer_tick(value.age,state::timer_rate);value.flags|=1u;}
        }
        if(remove){value.finish();detach(value);destroy_laser(&value);}
    }
    return 1;
}
int Controller::draw(){auto& sprites=*pe::sprite_controller;sprites.field_6c4=static_cast<std::uint32_t>(view_index);sprite::configure_animation_layer(sprites,12,view_index);
    scheduler::Iterator iterator(active.sentinel.next);for(;iterator.current;iterator.advance()){auto& value=*reinterpret_cast<Laser*>(iterator.current->value);if(value.state!=1)value.draw();}return 1;
}
int Controller::cancel_rectangle(const sprite::Vec3& p,const sprite::Vec3& size,float angle,std::int32_t mode,std::int32_t kind){cancel_position=p;cancel_size=size;std::uint32_t count=0;
    scheduler::Iterator iterator(active.sentinel.next);for(;iterator.current;iterator.advance()){auto& value=*reinterpret_cast<Laser*>(iterator.current->value);if(value.state!=1&&(value.flags&1u))count+=static_cast<std::uint32_t>(value.cancel_rectangle(p,size,angle,mode,kind));}return recovered::signed_bits(count);
}
int Controller::cancel_circle(const sprite::Vec3& p,float radius,std::int32_t mode,std::int32_t kind){cancel_position=p;std::uint32_t count=0;
    scheduler::Iterator iterator(active.sentinel.next);for(;iterator.current;iterator.advance()){auto& value=*reinterpret_cast<Laser*>(iterator.current->value);if(value.state!=1)count+=static_cast<std::uint32_t>(value.cancel_circle(p,radius,mode,kind));}return recovered::signed_bits(count);
}
}
