#include "effect.hpp"
#include "../program_entry/program_entry.hpp"
#include <cstring>
#include <new>
namespace th20::source::effects {
namespace pe=program_entry;namespace e=environment;
namespace {int __cdecl update_callback(void* p){return static_cast<Controller*>(p)->update();}int __cdecl draw_callback(void* p){return static_cast<Controller*>(p)->draw();}}
Controller::Controller(){for(auto& file:files)file=nullptr;ready=0;for(auto& handle:handles)handle=0;for(auto& r:requests)construct_request(r);view_index=0;context=nullptr;/*40c6b0 release diagnostic is an actual empty five-byte function.*/}
Controller::~Controller(){runtime::join_worker(worker);clear();scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);e::unload(8);e::unload(7);e::unload(21);}
int Controller::load_assets(){files[1]=e::load(7,"bullet.anm");if(!files[1]){e::load_error();return -1;}files[0]=e::load(8,"effect.anm");if(!files[0]){e::load_error();return -1;}files[2]=e::load(21,"screenswitch.anm");if(!files[2]){e::load_error();return -1;}ready=1;return 0;}
int Controller::initialize(std::int32_t index){select_context(index);e::context(index).objects_04[7]=this;(void)load_assets();
    update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,41,&update_callback,this,false,false);draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,42,&draw_callback,this,true,false);runtime::CallbackOwner::enable_callbacks();for(auto& r:requests)r.type=-1;return 0;
}
Controller* create_controller(std::int32_t index){auto* memory=::operator new(sizeof(Controller),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Controller));auto* result=new(memory)Controller;if(result->initialize(index)!=0){runtime::retire_callback_owner(result);return nullptr;}return result;}
void destroy_controller(std::int32_t index){auto& slot=e::context(index).objects_04[7];if(slot){runtime::retire_callback_owner(static_cast<Controller*>(slot));slot=nullptr;}}
}
