#include "environment.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
#include <cstring>
#include <new>
namespace th20::source::overlay {
namespace ps=gameplay::player_state;namespace n=recovered;
namespace {int __cdecl update_callback(void* self){return update(*static_cast<WeaponStoneInf*>(self));}int __cdecl draw_callback(void*){return 1;} //49dea0→478bf0 exact no-op draw
}
int initialize(WeaponStoneInf& owner,int view,Environment& host){
 owner.select_context(view);owner.context->overlay_owner=&owner;host.select_view(view);owner.file=host.load_animation(23,"aura.anm");if(!owner.file){host.load_error();return -1;}
 auto& scheduler=*program_entry::function_controller;auto& se=program_entry::scheduler_environment;
 owner.update_node=scheduler::register_callback(scheduler,se,27,update_callback,&owner,false,true);owner.draw_node=scheduler::register_callback(scheduler,se,55,draw_callback,&owner,true,true);
 refresh_selection(owner,1,host);refresh_selection(owner,0,host);n::timer_set(owner.age,0);n::timer_set(owner.cancellation_age,0);return 0;
}
int initialize(WeaponStoneInf& owner,int view){return initialize(owner,view,environment());}
WeaponStoneInf* create_controller(int view){void* memory=::operator new(sizeof(WeaponStoneInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(WeaponStoneInf));WeaponStoneInf* owner;try{owner=new(memory)WeaponStoneInf;}catch(...){::operator delete(memory);throw;}if(initialize(*owner,view)!=0){runtime::retire_callback_owner(owner);return nullptr;}return owner;}
void release(int view){auto& slot=game_session::context(view).overlay_owner;if(slot){runtime::retire_callback_owner(slot);slot=nullptr;}}
int clear(WeaponStoneInf& owner,Environment& host){host.preserve_animation(23,false);owner.phase=0;host.delete_animation(owner.animation_handle);owner.animation_handle=0;host.destroy_mesh(owner.mesh);owner.mesh=nullptr;owner.disable_callbacks();return 0;}
int clear(WeaponStoneInf& owner){return clear(owner,environment());}
void reset(WeaponStoneInf& owner,int stage){
 auto& p=*game_session::context(0).current_player;ps::write(p,0x9c,0);ps::write(p,0xa0,0);p.bytes_a4[0]=p.bytes_a4[1]=0;ps::write(p,0xa8,0);p.byte_b0=0;
 owner.main_weapon->reset();owner.main_weapon->initialize_main();owner.focused_weapon->reset();owner.unfocused_weapon->reset();owner.passive_weapon->reset();owner.passive_weapon->initialize_passive();
 n::timer_set(owner.age,0);n::timer_set(owner.mesh_age,0);owner.mesh=nullptr;n::timer_set(owner.cancellation_age,0);owner.phase=0;owner.counter.current=0;owner.main_shooting=0;
 constexpr int duration[]{30,30,40,65,100,60,70,20},maximum[]{130,130,150,150,180,130,130,150},meter[]{0,1100,1200,1300,1400,1400,1400,1400};
 ps::write(p,0x54,duration[ps::read<int>(p,0xc)]);ps::write(p,0x50,maximum[ps::read<int>(p,0xc)]);ps::write(p,0x60,meter[stage]);
 ps::write(p,0x5c,std::clamp(std::clamp(ps::read<int>(p,0x5c),0,10000)/2,0,10000));
 const int current=std::clamp(ps::read<int>(p,0x5c),0,10000);ps::write(p,0x5c,current);const int limit=std::clamp(ps::read<int>(p,0x60),0,5000);ps::write(p,0x60,limit);
 if(limit-200<=current)ps::write(p,0x5c,std::clamp(limit-200,0,10000));
}
void activate_weapons(WeaponStoneInf& owner,bool selected){auto& p=*game_session::context(0).current_player;auto& entity=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);if(selected){owner.main_weapon->activate_main();if(entity.focused_204c)owner.focused_weapon->activate_focused();else owner.unfocused_weapon->activate_unfocused();}else if(ps::read<int>(p,0x10)!=ps::read<int>(p,0x14)){if(entity.focused_204c)owner.unfocused_weapon->activate_unfocused();else owner.focused_weapon->activate_focused();}}
void shoot(WeaponStoneInf& owner,int frame,int secondary,int level){owner.main_weapon->shoot_main(frame,secondary,level);auto& entity=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);if(entity.focused_204c)owner.focused_weapon->shoot_focused(frame,secondary,level);else owner.unfocused_weapon->shoot_unfocused(frame,secondary,level);entity.shots.field_1255c&=~8u;}
int shot_script_index(WeaponStoneInf& owner){auto& entity=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);return entity.focused_204c?owner.focused_weapon->shot_script_index():owner.unfocused_weapon->shot_script_index();}
}
