#include "card.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include <cstring>
#include <new>
namespace th20::source::card {
namespace pe=program_entry;
namespace {
int __cdecl update_callback(void* value){return update(*static_cast<CardInf*>(value));}
int __cdecl draw_callback(void* value){return draw(*static_cast<CardInf*>(value));}
}
CardInf* controller(int index) noexcept{return static_cast<CardInf*>(game_session::context(index).objects_04[3]);}
CardInf::CardInf():background_handle(0),info_handles{},effect_handle(0),age{},name{},spell_index(0),flags(0),bonus(0),initial_bonus(0),duration(0),capture_index(0),frames(0),last_frames(0),start_time(0),elapsed(0),encoded_time(0),position{},field_b8(0),view_index(0),context(nullptr) {}
CardInf::~CardInf(){
    for(auto& handle:info_handles)sprite::request_animation_deletion(*pe::sprite_controller,handle);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
}
int initialize(CardInf& o,int index){
    o.view_index=index;o.context=&game_session::context(index);o.context->objects_04[3]=&o;
    o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,40,update_callback,&o,false,false);
    o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,12,draw_callback,&o,true,false);
    recovered::timer_set(o.age,0);return 0;
}
CardInf* create(int index){
    auto* memory=::operator new(sizeof(CardInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(CardInf));
    auto* value=new(memory)CardInf;if(initialize(*value,index)!=0){runtime::retire_callback_owner(value);return nullptr;}return value;
}
}
namespace th20::source::gameplay::unrecovered {runtime::CallbackOwner* create_00488b20(int index){return card::create(index);}}
