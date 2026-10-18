#include "help.hpp"
#include "../program_entry/program_entry.hpp"
#include "../startup_scene/startup.hpp"
#include "../archive/resource_manager.hpp"
#include "../runtime_core/worker.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../audio_runtime/audio.hpp"
#include "../input/input.hpp"
#include "../pause_system/menu_support.hpp"
#include <cstring>
#include <cstdio>
#include <new>
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c4d24=nullptr;}
namespace th20::source::help {
namespace pe=program_entry;
HelpInf* controller(){return static_cast<HelpInf*>(startup::unrecovered::owner_005c4d24);}
HelpInf::HelpInf():state(0),age{},cursor(),handles{},finished(0),x(0),file(nullptr),image_bytes(nullptr),substate(0),filename{},image_size(0){startup::unrecovered::owner_005c4d24=this;}
HelpInf::~HelpInf(){scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);sprite::unload_animation_file(*pe::sprite_controller,14);startup::unrecovered::owner_005c4d24=nullptr;}
namespace {
runtime::Worker& worker(){return *std::launder(reinterpret_cast<runtime::Worker*>(pe::graphics_state.worker_storage[0]));}
void launch(void(*entry)()){ //4b99f0/40b1d0, real one shared Graphics worker
    std::lock_guard<std::recursive_mutex> outer(runtime::shared_locks().slot(6));std::lock_guard<std::recursive_mutex> inner(runtime::shared_locks().slot(6));auto& w=worker();
    {std::lock_guard<std::recursive_mutex> detach(runtime::shared_locks().slot(6));if(w.thread.joinable())w.thread.detach();}
    w.close_requested.store(false,std::memory_order_seq_cst);w.thread=TH20_WORKER_THREAD("help",entry);
}
void load_file(){auto& o=*controller();o.file=sprite::load_animation_file(*pe::sprite_controller,14,"help.anm",pe::log_buffer,pe::graphics_state.event_flags);if(o.file){runtime::detach_worker(worker());scheduler::enable(*o.update_node);scheduler::enable(*o.draw_node);}}
void load_image(){auto& o=*controller();auto data=resources::read(o.filename,false);if(data){o.image_size=static_cast<std::uint32_t>(data->size());o.image_bytes=static_cast<std::uint8_t*>(runtime::allocate_bytes(data->size()));std::memcpy(o.image_bytes,data->data(),data->size());}else{o.image_bytes=nullptr;o.image_size=0;}o.substate=3;runtime::detach_worker(worker());}
bool pressed(unsigned mask){const auto* b=input::button_slot(2);return b&&(b->pressed&mask)!=0;}
bool repeated(unsigned mask){const auto* b=input::button_slot(2);return b&&((b->repeat8|b->pressed)&mask)!=0;}
void effect(int id){pe::thread_registry.request_effect(id,0);}
void interrupt(HelpInf& o,int index,int event){sprite::interrupt_animation_children(*pe::sprite_controller,o.handles[index],event);}
void execute(HelpInf& o,int index,int event){sprite::execute_animation_interrupt(*pe::sprite_controller,o.handles[index],event);}
void begin_image(HelpInf& o){o.substate=2;recovered::timer_set(o.age,0);sprintf_s(o.filename,"help_%.2d.png",o.cursor.current+1);launch(load_image);for(int i=0;i<9;++i)interrupt(o,i,1);}
int __cdecl update_callback(void* o){return update(*static_cast<HelpInf*>(o));}
int __cdecl draw_callback(void*){return 1;} //original478bf0 is literally mov eax,1;ret
}
void spawn(HelpInf& o,int index,const sprite::Vec3& position){sprite::spawn_named_animation(*pe::sprite_controller,*o.file,o.handles[index],"help",index,&position,0.f,-1,4);}
int initialize(HelpInf& o){o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,16,update_callback,&o,false,false);o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,92,draw_callback,&o,true,false);launch(load_file);recovered::timer_set(o.age,0);o.state=0;return 0;}
HelpInf* create(){auto* memory=::operator new(sizeof(HelpInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(HelpInf));auto* o=new(memory)HelpInf;if(initialize(*o)!=0){runtime::retire_callback_owner(o);return nullptr;}return o;}
int update(HelpInf& o){
    const sprite::Vec3 position{o.x,0.f,0.f};
    if(o.state==0)o.state=1;
    else if(o.state==2){if(o.age.current>=30)o.finished=1;}
    else if(o.state==1)switch(o.substate){
    case 0:
        o.cursor.count=9;o.cursor.select(0);o.cursor.wrapping=1;for(int i=0;i<9;++i){spawn(o,i,position);execute(o,i,o.cursor.current==i?2:3);}clear_texture(o.file->textures[1]);o.substate=1;
        [[fallthrough]];
    case 1:
        if(o.age.current>=20){o.cursor.snapshot();if(repeated(0x10))o.cursor.move(-1);if(repeated(0x20))o.cursor.move(1);if(o.cursor.changed()){effect(10);for(int i=0;i<9;++i)execute(o,i,o.cursor.current==i?2:3);}
            if(pressed(0x80001)){effect(7);begin_image(o);}else if(pressed(0x106)){effect(9);for(int i=0;i<9;++i)interrupt(o,i,1);o.state=2;o.substate=0;recovered::timer_set(o.age,0);}}
        break;
    case 3:
        replace_texture_image(o.file->textures[1],o.image_bytes,o.image_size,1,false);if(o.image_bytes){runtime::release_bytes(o.image_bytes);o.image_bytes=nullptr;}o.image_bytes=nullptr;
#ifdef TH_SDL3
        web::shared_device().preload(o.file->textures[1].texture);
#else
        o.file->textures[1].texture->PreLoad();
#endif
        spawn(o,13,position);o.substate=4;recovered::timer_set(o.age,0);
        [[fallthrough]];
    case 4:
        if(o.age.current>=20){
            if(pressed(0x20)&&o.cursor.current<=7){o.substate=5;recovered::timer_set(o.age,0);effect(7);o.cursor.move(1);execute(o,13,7);}
            else if(pressed(0x10)&&o.cursor.current>=1){o.substate=5;recovered::timer_set(o.age,0);effect(7);o.cursor.move(-1);execute(o,13,8);}
            else if(pressed(0x80001)||pressed(0x106)){effect(9);o.substate=1;recovered::timer_set(o.age,0);interrupt(o,13,1);for(int i=0;i<9;++i){spawn(o,i,position);execute(o,i,o.cursor.current==i?2:3);}break;}
            else break;
            if(o.age.current>=20)begin_image(o);
        }break;
    case 5:if(o.age.current>=20)begin_image(o);break;
    }
    recovered::timer_tick(o.age,state::timer_rate);return 1;
}
}
namespace th20::source::pause::unrecovered {runtime::CallbackOwner* create_help(){return help::create();}}
