#include "pause.hpp"
#include "../program_entry/program_entry.hpp"
#include "../startup_scene/startup.hpp"
#include <cstring>
#include <new>
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c60bc=nullptr;}
namespace th20::source::pause {
namespace pe=program_entry;
PauseInf* controller(){return static_cast<PauseInf*>(startup::unrecovered::owner_005c60bc);}
PauseInf::PauseInf():age{},secondary_age{},cursor(),name_cursor(),panel_handle(0),background_handle(0),state(0),previous_state(0),substate(0),name_length(0),completed(0),field_e4(0),cancel_disabled(0),saved_input(0),metadata{},retained_154{},player_name{},saved_clock_scale(0),saved_music_position(0),saved_music_name{},file(nullptr) {
    // 4e1cb0 preserves the high 29 bits; original factory separately zero-fills.
    menu_flags&=~7u;startup::unrecovered::owner_005c60bc=this;
}
PauseInf::~PauseInf(){
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
    for(auto* value:metadata)runtime::retire_callback_owner(value);
    startup::unrecovered::owner_005c60bc=nullptr;
}
namespace {int __cdecl update_callback(void* value){return update(*static_cast<PauseInf*>(value),services());}int __cdecl draw_callback(void* value){draw(*static_cast<PauseInf*>(value));return 1;}}
int initialize(PauseInf& o){
    o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,15,update_callback,&o,false,false);
    o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,95,draw_callback,&o,true,false);
    recovered::timer_set(o.age,0);recovered::timer_set(o.secondary_age,0);return 0;
}
PauseInf* create(){
    auto* storage=::operator new(sizeof(PauseInf),std::nothrow);if(!storage)return nullptr;
    std::memset(storage,0,sizeof(PauseInf));auto* value=new(storage)PauseInf;
    if(initialize(*value)!=0){runtime::retire_callback_owner(value);return nullptr;}return value;
}
}
