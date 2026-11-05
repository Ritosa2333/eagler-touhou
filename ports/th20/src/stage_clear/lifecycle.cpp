#include "stage_clear.hpp"
#include "../program_entry/program_entry.hpp"
#include "../startup_scene/startup.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../hud_system/dialogue.hpp"
#include <cstring>
#include <new>
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c6114=nullptr;}
namespace th20::source::stage_clear {
namespace pe=program_entry;
namespace {
int __cdecl update_callback(void* value){return update(*static_cast<StageClearInf*>(value));}
int __cdecl draw_callback(void* value){return draw(*static_cast<StageClearInf*>(value));}
}
StageClearInf* controller(){return static_cast<StageClearInf*>(startup::unrecovered::owner_005c6114);}
StageClearInf::StageClearInf():file(nullptr),panel_handle(0),secondary_handle(0),field_1c(0),kind(0),state(0),bonus(0),age{},secondary_age{},field_4c(0),fields_50{},fields_90{} {
    //422d90 constructs both timers as four zero words, distinct from timer_set.
    startup::unrecovered::owner_005c6114=this;
}
StageClearInf::~StageClearInf(){
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
    sprite::request_animation_deletion(*pe::sprite_controller,secondary_handle);
    startup::unrecovered::owner_005c6114=nullptr;
}
int initialize(StageClearInf& o,int kind){
    o.file=sprite::load_animation_file(*pe::sprite_controller,5,"fronttr.anm",pe::log_buffer,pe::graphics_state.event_flags);if(!o.file)return -1;
    o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,45,update_callback,&o,false,true);
    o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,86,draw_callback,&o,true,true);
    o.state=1;recovered::timer_set(o.age,0);o.kind=kind;return 0;
}
StageClearInf* create(int kind){
    runtime::retire_callback_owner(controller());
    auto* memory=::operator new(sizeof(StageClearInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(StageClearInf));
    auto* value=new(memory)StageClearInf;if(initialize(*value,kind)!=0){runtime::retire_callback_owner(value);return nullptr;}return value;
}
}
namespace th20::source::hud::unrecovered {runtime::CallbackOwner* create_scene_005115c0(int kind){return stage_clear::create(kind);}}
