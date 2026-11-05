#include "options.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_services/services.hpp"
#include "../platform_services/text_constants.hpp"
#include "../startup_scene/startup.hpp"
#include "../pause_system/menu_support.hpp"
#include "../runtime_state/state.hpp"
#include "../audio_runtime/audio.hpp"
#include "../screen_effect/effect.hpp"
#include "../input/input.hpp"
#include <cstring>
#include <new>
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c60b8=nullptr;}
namespace th20::source::options {
namespace pe=program_entry;
OptionInf* controller(){return static_cast<OptionInf*>(startup::unrecovered::owner_005c60b8);}
OptionInf::OptionInf():field_10(0),cursor(),age{},position{},selection_age{},key_config_age{},allow_escape(0),state(0){startup::unrecovered::owner_005c60b8=this;}
OptionInf::~OptionInf(){scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);startup::unrecovered::owner_005c60b8=nullptr;save_configuration_with_diagnostic();}
void save_configuration_with_diagnostic(){
    const auto path=(std::filesystem::path(pe::window_state.user_data_directory)/"th20.cfg").string();
    if(platform::write_loose_file(path.c_str(),&pe::graphics_state.configuration,0xb0)!=0)
        runtime::log_error(pe::log_buffer,platform::text::s00571dd0,path.c_str());
    // Original diagnostic has an omitted %s argument; use its actual path, as the platform loader does.
}
namespace {
struct Production final:Environment {
    Production():Environment(pe::graphics_state.configuration,pe::window_state.display_mode,state::timer_rate){}
    bool key_config_active() override{return startup::unrecovered::owner_005c4d28!=nullptr;}
    bool pressed(std::uint32_t bits) override{const auto* b=input::button_slot(2);return b&&(b->pressed&bits)!=0;}
    bool repeated(std::uint32_t bits) override{const auto* b=input::button_slot(2);return b&&((b->repeat8|b->pressed)&bits)!=0;}
    void play_effect(int id) override{pe::thread_registry.request_effect(id,0);}
    void screen_change_effect() override{screen::create_effect(9,10,0,0,0,109,0);}
    void apply_volume() override{options::apply_volume(configuration,pe::thread_registry);}
    void reset_device() override{pe::set_device_reset(pe::window_state,1);}
    void create_key_config(const sprite::Vec3& position) override{unrecovered::create_key_config(position);}
    void save_configuration() override{platform::save_configuration(configuration,pe::window_state);}
    void retire(OptionInf& o) override{runtime::retire_callback_owner(&o);}
};
int __cdecl update_callback(void* o){return update(*static_cast<OptionInf*>(o),environment());}
int __cdecl draw_callback(void* o){return draw(*static_cast<OptionInf*>(o),*text::renderer,pe::graphics_state.configuration,pe::window_state.display_mode,startup::unrecovered::owner_005c4d28!=nullptr);}
}
Environment& environment(){static Production instance;instance.timer_rate=state::timer_rate;return instance;}
int initialize(OptionInf& o,const sprite::Vec3& position){
    o.update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,10,update_callback,&o,false,true);
    o.draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,94,draw_callback,&o,true,true);
    o.enable_callbacks();o.cursor.select(0);o.cursor.wrapping=1;
    recovered::timer_set(o.age,0);recovered::timer_set(o.selection_age,0);recovered::timer_set(o.key_config_age,0);o.position=position;return 0;
}
OptionInf* create(const sprite::Vec3& position){
    auto* memory=::operator new(sizeof(OptionInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(OptionInf));auto* o=new(memory)OptionInf;
    if(initialize(*o,position)!=0){runtime::retire_callback_owner(o);return nullptr;}return o;
}
}
namespace th20::source::pause::unrecovered {runtime::CallbackOwner* create_options(const sprite::Vec3& position){return options::create(position);}}

