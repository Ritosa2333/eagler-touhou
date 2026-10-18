#include "notice.hpp"
#include "data_strings.hpp"
#include "../program_entry/program_entry.hpp"
#include "../startup_scene/startup.hpp"
#include "../runtime_core/worker.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include <cstring>
#include <new>
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c5b38=nullptr;}
namespace th20::source::notice {
namespace pe=program_entry;
// Message embeds pmr::string members; MSVC x86 STL layout evidence, binding
// on the Windows build only.
#ifndef TH_SDL3
static_assert(sizeof(Message)==0x38);
#endif
std::array<Message,25> messages=[] {
    std::array<Message,25> value;
    value[0]={"no notice","no notice"};
    for(int i=1;i<=24;++i)value[i]={i<9?data::s_00571ee0:i<17?data::s_00571f1c:data::s_00571f40,i<9?data::s_00571ef8:data::s_00571f28};
    return value;
}();
NoticeInf* controller(){return static_cast<NoticeInf*>(startup::unrecovered::owner_005c5b38);}
NoticeInf::NoticeInf():state(0),age{},cursor(),handles{},secondary_handle(0),finished(0),x(0),selected_index(0),file(nullptr),secondary_file(nullptr),image_bytes(nullptr),substate(0),filename{},image_size(0){startup::unrecovered::owner_005c5b38=this;}
NoticeInf::~NoticeInf(){scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);sprite::unload_animation_file(*pe::sprite_controller,14);sprite::unload_animation_file(*pe::sprite_controller,22);startup::unrecovered::owner_005c5b38=nullptr;}
void NoticeInf::enable_callbacks(){scheduler::enable(*update_node);scheduler::enable(*draw_node);}
namespace {
runtime::Worker& worker(){return *std::launder(reinterpret_cast<runtime::Worker*>(pe::graphics_state.worker_storage[0]));}
}
void load_file(){ //4df6e0 resolves the shared Notice owner at execution time.
    auto* file=sprite::load_animation_file(*pe::sprite_controller,14,data::s_00571f50,pe::log_buffer,pe::graphics_state.event_flags);
    controller()->file=file;
    if(!file){runtime::log_error(pe::log_buffer,data::s_00571f5c);return;}
    runtime::detach_worker(worker());scheduler::enable(*controller()->update_node);scheduler::enable(*controller()->draw_node);
}
namespace {
void launch(){ //4b99f0 /40b1d0 shared Graphics worker
    std::lock_guard<std::recursive_mutex> outer(runtime::shared_locks().slot(6));std::lock_guard<std::recursive_mutex> inner(runtime::shared_locks().slot(6));auto& w=worker();
    {std::lock_guard<std::recursive_mutex> detach(runtime::shared_locks().slot(6));if(w.thread.joinable())w.thread.detach();}
    w.close_requested.store(false,std::memory_order_seq_cst);w.thread=TH20_WORKER_THREAD("notice",load_file);
}
int __cdecl update_callback(void* o){return update(*static_cast<NoticeInf*>(o));}
int __cdecl draw_callback(void*){return 1;} //49dea0 ->478bf0, original literal return1
}
int initialize(NoticeInf& o){o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,16,update_callback,&o,false,false);o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,92,draw_callback,&o,true,false);launch();recovered::timer_set(o.age,0);o.state=0;return 0;}
NoticeInf* create(int selected){auto* memory=::operator new(sizeof(NoticeInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(NoticeInf));auto* o=new(memory)NoticeInf;if(initialize(*o)!=0){runtime::retire_callback_owner(o);return nullptr;}o->selected_index=selected;return o;}
}
