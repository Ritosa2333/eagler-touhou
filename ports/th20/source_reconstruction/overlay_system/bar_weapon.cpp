#include "bar_weapon.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
namespace th20::source::overlay {
namespace ps=gameplay::player_state;namespace n=recovered;
namespace {game_session::Player& stats(){return *game_session::context(0).current_player;}player_entity::Player& player(){return *static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);}}
template<int Character>void BarWeapon<Character>::reset(){active=0;n::timer_set(phase_age,0);field_10=1;n::timer_set(idle_age,0);handle=0;bar_y=0;}
template<int Character>void BarWeapon<Character>::shoot_main(int first,int second,int level){fire_player_shots(player().shots,first,second,n::signed_bits(static_cast<unsigned>(stone_id)*20u+(active?15u:0u)+static_cast<unsigned>(level)));}
template<int Character>void BarWeapon<Character>::initialize_passive(){stats().bytes_a4[1]=1;}
template<int Character>void BarWeapon<Character>::start_phase(){auto& host=weapon_services();n::timer_set(phase_age,phase_duration(stats()));active=1;host.delete_animation(handle);bar_y=448.f;const sprite::Vec3 position{0,bar_y,0};handle=host.spawn_player_animation(Character==0?"pl00":"pl01",Character==0?25:24,position);}
template<int Character>int BarWeapon<Character>::update_phase(){
 if(phase_age.current<=0)return 1;auto& host=weapon_services();host.show_animation(handle,true);auto& p=stats();const int maximum=std::clamp(ps::read<int>(p,0x50),100,500);ps::write(p,0x50,maximum);const int value=n::signed_bits(static_cast<unsigned>(maximum)*static_cast<unsigned>(phase_age.current))/phase_duration(p);ps::write(p,0x4c,std::clamp(value,0,500));
 float target=player().position_614.y+8.f;if(target<224.f)target=224.f;bar_y=bar_y-(bar_y-target)*.04f;
 sprite::Vec3 position{0,bar_y,0};host.animation_position(handle,position);const sprite::Vec3 size{384.f,224.f,0};position.y=position.y+112.f;host.clear_bullet_rectangle(position,size);host.clear_laser_rectangle(position,size);n::timer_add(phase_age,-1.f,state::timer_rate);return 0;
}
template<int Character>int BarWeapon<Character>::end_phase(){weapon_services().interrupt(handle,1);active=0;return 0;}
template<int Character>int BarWeapon<Character>::cancel_phase(){weapon_services().show_animation(handle,false);return 0;}
template class BarWeapon<0>;template class BarWeapon<1>;
}
