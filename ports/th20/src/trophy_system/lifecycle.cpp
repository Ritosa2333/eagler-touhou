#include "trophy.hpp"
#include "data_strings.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include <new>
#include <cstring>
namespace th20::source::trophy {
TrophyInf* controller=nullptr;
namespace {
int __cdecl update_callback(void* owner){return update(*static_cast<TrophyInf*>(owner));}
//49dea0 calls478bf0, whose actual body returns1 without rendering.
int __cdecl draw_callback(void*){return 1;}
}
TrophyInf::TrophyInf():animation_file(nullptr),previous_state(0),state(0),state_frame(0),age{},handles{}{controller=this;}
TrophyInf::~TrophyInf(){
    scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,update_node);
    scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,draw_node);
    controller=nullptr;
}
void change_state(TrophyInf& o,int next){o.previous_state=o.state;o.state=next;o.state_frame=0;recovered::timer_set(o.age,0);}
int initialize(TrophyInf& o,int index){
    o.animation_file=sprite::load_animation_file(*program_entry::sprite_controller,20,"trophy.anm",program_entry::log_buffer,program_entry::graphics_state.event_flags);
    if(!o.animation_file){runtime::log_error(program_entry::log_buffer,data::s_00571f5c);return -1;}
    o.update_node=scheduler::register_callback(*program_entry::function_controller,program_entry::scheduler_environment,17,update_callback,&o,false,true);
    o.draw_node=scheduler::register_callback(*program_entry::function_controller,program_entry::scheduler_environment,93,draw_callback,&o,true,true);
    o.pending.push(index);change_state(o,0);return 0;
}
TrophyInf* announce(unsigned index){
    if(achieved(index))return nullptr;mark_achieved(index);
    if(controller){controller->pending.push(static_cast<int>(index));return controller;}
    auto* memory=::operator new(sizeof(TrophyInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(TrophyInf));
    auto* object=new(memory)TrophyInf;if(initialize(*object,static_cast<int>(index))==0)return object;runtime::retire_callback_owner(object);return nullptr;
}
}
namespace th20::source::stage_completion::unrecovered {void announce_achievement(int index){trophy::announce(static_cast<unsigned>(index));}}
