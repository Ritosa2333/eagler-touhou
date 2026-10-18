#include "key_config.hpp"
#include "../options_system/options.hpp"
#include "../program_entry/program_entry.hpp"
#include "../startup_scene/startup.hpp"
#include "../runtime_state/state.hpp"
#include "../audio_runtime/audio.hpp"
#include <cstring>
#include <new>
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c4d28=nullptr;}
namespace th20::source::key_config {
namespace pe=program_entry;
const platform::KeyBindings& default_bindings(){static const platform::KeyBindings value=[](){platform::KeyBindings b;platform::initialize_bindings(b);return b;}();return value;}
KeyConfigInf* controller(){return static_cast<KeyConfigInf*>(startup::unrecovered::owner_005c4d28);}
KeyConfigInf::KeyConfigInf():field_10(0),cursor(),bindings{},age{},transition_age{},position{},selection_age{},state(0),phase(0),selected_devices{},selected_slot(0),refresh_devices(0){startup::unrecovered::owner_005c4d28=this;}
KeyConfigInf::~KeyConfigInf(){scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);startup::unrecovered::owner_005c4d28=nullptr;options::save_configuration_with_diagnostic();}
namespace {
struct Production final:Environment {
    Production():Environment(*input::controller,pe::graphics_state.configuration,default_bindings(),state::timer_rate){}
    bool pressed(int slot,unsigned mask) override{if(slot<0)return false;const auto* b=input::button_slot(slot);return b&&(b->pressed&mask)!=0;}
    bool repeated(int slot,unsigned mask) override{if(slot<0)return false;const auto* b=input::button_slot(slot);return b&&((b->pressed|b->repeat8)&mask)!=0;}
    void play_effect(int id) override{pe::thread_registry.request_effect(id,0);}
    void rebuild_devices() override{input.rebuild_devices();}
    void retire(KeyConfigInf& value) override{runtime::retire_callback_owner(&value);}
};
int __cdecl update_callback(void* value){return update(*static_cast<KeyConfigInf*>(value),environment());}
int __cdecl draw_callback(void* value){return draw(*static_cast<KeyConfigInf*>(value),*text::renderer,*input::controller);}
}
Environment& environment(){static Production value;value.timer_rate=state::timer_rate;return value;}
int initialize(KeyConfigInf& o,const sprite::Vec3& position){
    o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,10,update_callback,&o,false,true);o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,94,draw_callback,&o,true,true);
    o.enable_callbacks();o.cursor.select(0);o.cursor.wrapping=1;recovered::timer_set(o.age,0);o.position=position;o.refresh_devices=1;o.selected_slot=0;set_state(o,0);return 0;
}
KeyConfigInf* create(const sprite::Vec3& position){auto* memory=::operator new(sizeof(KeyConfigInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(KeyConfigInf));auto* o=new(memory)KeyConfigInf;if(initialize(*o,position)!=0){runtime::retire_callback_owner(o);return nullptr;}return o;}
}
namespace th20::source::options::unrecovered {runtime::CallbackOwner* create_key_config(const sprite::Vec3& position){return key_config::create(position);}}
