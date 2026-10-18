#include "ring_weapon.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
namespace th20::source::overlay {
namespace n=recovered;namespace ps=gameplay::player_state;
namespace {game_session::Player& stats(){return *game_session::context(0).current_player;}player_entity::Player& player(){return *static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);}
void __fastcall option_callback(player_entity::Option* option,void*){anchored_option(*option);}
int __cdecl ring_callback(sprite::Animation* animation){return update_ring_animation(*animation);}
}
void anchored_option(player_entity::Option& option){auto& context=*ps::read<game_session::Context*>(option,0x128);auto& entity=*static_cast<player_entity::Player*>(context.objects_04[0]);const auto base=ps::read<player_entity::Fixed2>(entity,0x630);option.vector_70={n::signed_bits(static_cast<unsigned>(base.x)+static_cast<unsigned>(option.offsets_80[1].x)),n::signed_bits(static_cast<unsigned>(base.y)+static_cast<unsigned>(option.offsets_80[1].y))};}
int update_ring_animation(sprite::Animation& animation){const auto& weapon=*reinterpret_cast<const Weapon*>(animation.field_5c8);animation.vector_5bc=player().position_614;const float numerator=weapon.phase_age.current_f*160.f;const int duration=phase_duration(stats());animation.base.vector_50={numerator/n::int_float(duration),1.f};animation.base.flags[1]|=4;return 0;}
template<int Character>void RingWeapon<Character>::reset(){active=0;n::timer_set(phase_age,0);field_10=1;n::timer_set(idle_age,0);handle=0;n::timer_set(timer_3c,0);n::timer_set(timer_4c,0);}
template<int Character>void RingWeapon<Character>::shoot_main(int first,int second,int level){fire_player_shots(player().shots,first,second,n::signed_bits(static_cast<unsigned>(stone_id)*20u+static_cast<unsigned>(level)));}
template<int Character>void RingWeapon<Character>::initialize_focused_option(player_entity::Option* option,int){ps::write(*option,0x11c,reinterpret_cast<std::uint32_t>(&option_callback));}
template<int Character>void RingWeapon<Character>::initialize_passive(){ps::write(stats(),0xac,stats().bytes_2c[3]?20:30);}
template<int Character>void RingWeapon<Character>::start_phase(){auto& host=weapon_services();n::timer_set(phase_age,phase_duration(stats()));active=1;host.delete_animation(handle);effects::Parameters p;effects::construct_parameters(p);p.vector_00=player().position_614;handle=host.spawn_effect(8,p);auto& animation=host.animation(handle);animation.base.vector_50={160.f,1.f};animation.field_5c8=reinterpret_cast<unsigned>(this);animation.field_5dc=reinterpret_cast<unsigned>(&ring_callback);}
template<int Character>int RingWeapon<Character>::update_phase(){auto& host=weapon_services();const bool ended=phase_age.current<=0;if(!ended){host.pause_animation(handle,false);host.show_animation(handle,true);auto& p=stats();const int maximum=std::clamp(ps::read<int>(p,0x50),100,500);ps::write(p,0x50,maximum);const int value=n::signed_bits(static_cast<unsigned>(maximum)*static_cast<unsigned>(phase_age.current))/phase_duration(p);ps::write(p,0x4c,std::clamp(value,0,500));n::timer_add(phase_age,-1.f,state::timer_rate);}else{host.spawn_item(6,{0,64,0},-3.1415927410125732421875f/2.f,2.2000000476837158203125f);host.delete_animation(handle);}return ended;}
template<int Character>int RingWeapon<Character>::end_phase(){weapon_services().delete_animation(handle);active=0;return 0;}
template<int Character>int RingWeapon<Character>::cancel_phase(){weapon_services().pause_animation(handle,true);weapon_services().show_animation(handle,false);return 0;}
template class RingWeapon<0>;template class RingWeapon<1>;
}
