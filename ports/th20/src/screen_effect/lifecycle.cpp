#include "effect.hpp"
#include "../program_entry/program_entry.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::screen {
namespace pe=program_entry;
namespace {
template<int(*Function)(Effect&)>int __cdecl callback(void* self){return Function(*static_cast<Effect*>(self));}
int __cdecl retire(void* self){runtime::retire_callback_owner(static_cast<Effect*>(self));return 0;}
}
Effect::Effect():mode(0),field_14(0),alpha(0),duration(0),argument_20(0),argument_24(0),argument_28(0),phase(0),timer{},view_index(0){}
Effect::~Effect(){scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);}
void Effect::initialize(int type,int duration_value,std::uint32_t a,std::uint32_t b,std::uint32_t c,int priority){
    constexpr scheduler::Callback updates[]={callback<update_fade_out>,callback<update_linear_shake>,callback<update_fade_in>,callback<update_fade_out>,callback<update_flashes>,callback<update_fade_in>,callback<update_hold>,callback<update_hold>,callback<update_envelope_shake>,callback<update_solid>};
    constexpr scheduler::Callback draws[]={callback<draw_display>,nullptr,callback<draw_current_view>,callback<draw_current_view>,callback<draw_offset_playfield>,callback<draw_display>,callback<draw_current_view>,callback<draw_playfield>,nullptr,callback<draw_current_view>};
    if(type<0||type>9)throw std::out_of_range("ScreenInf mode outside recovered valid0..9");
    if(type==3)alpha=255;
    update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,24,updates[type],this,false,true);
    if(draws[type])draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,priority,draws[type],this,true,true);
    scheduler::set_shutdown_callback(*update_node,&retire);recovered::timer_set(timer,0);mode=type;duration=duration_value;argument_20=a;argument_24=b;argument_28=c;
}
Effect* create_effect(int type,int duration,std::uint32_t a,std::uint32_t b,std::uint32_t c,int priority,int view_index){
    auto* memory=::operator new(sizeof(Effect),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Effect));auto* effect=new(memory)Effect;effect->view_index=view_index;effect->initialize(type,duration,a,b,c,priority);return effect;
}
}
namespace th20::source::background::unrecovered {
void create_transition_effect(int mode,int duration,int a,int b,int c,int priority){screen::create_effect(mode,duration,static_cast<std::uint32_t>(a),static_cast<std::uint32_t>(b),static_cast<std::uint32_t>(c),priority);}
}
