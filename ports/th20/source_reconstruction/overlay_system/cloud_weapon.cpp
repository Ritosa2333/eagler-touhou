#include "cloud_weapon.hpp"
#include "frame.hpp"
#include "../gameplay/player_state.hpp"
#include "../special_state/special.hpp"
#include <algorithm>
namespace th20::source::overlay {
namespace n=recovered;namespace ps=gameplay::player_state;
namespace {game_session::Player& stats(){return *game_session::context(0).current_player;}player_entity::Player& player(){return *static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);}
template<int Character>int __cdecl cloud_callback(sprite::Animation* animation){return update_cloud_animation<Character>(*animation);}
}
template<int Character>int update_cloud_animation(sprite::Animation& animation){auto& weapon=*reinterpret_cast<CloudWeapon<Character>*>(animation.field_5c8);weapon.tail.radius=special_state::sample(weapon.tail.interpolation,state::timer_rate);weapon.tail.position.y=weapon.tail.position.y-280.f/n::int_float(phase_duration(stats()));animation.vector_5bc=weapon.tail.position;animation.base.vector_50={weapon.tail.radius,1.f};animation.base.flags[1]|=4;return 0;}
template<int Character>void CloudWeapon<Character>::reset(){active=0;n::timer_set(phase_age,0);field_10=1;n::timer_set(idle_age,0);tail.position={0,0,0};tail.radius=0;tail.interpolation.duration=0;handle=0;n::timer_set(timer_3c,0);n::timer_set(timer_4c,0);}
template<int Character>void CloudWeapon<Character>::shoot_main(int first,int second,int level){if(!active)fire_player_shots(player().shots,first,second,n::signed_bits(static_cast<unsigned>(stone_id)*20u+static_cast<unsigned>(level)));}
template<int Character>void CloudWeapon<Character>::initialize_passive(){ps::write(stats(),0xa8,stats().bytes_2c[3]?10:15);}
template<int Character>void CloudWeapon<Character>::start_phase(){auto& host=weapon_services();n::timer_set(phase_age,phase_duration(stats()));active=1;host.delete_animation(handle);effects::Parameters p;effects::construct_parameters(p);p.vector_00=player().position_614;handle=host.spawn_effect(9,p);tail.radius=16.f;host.animation(handle).base.vector_50={tail.radius,1.f};tail.position=player().position_614;special_state::start(tail.interpolation,20,4,16.f,96.f);auto& animation=host.animation(handle);animation.field_5c8=reinterpret_cast<unsigned>(this);animation.field_5dc=reinterpret_cast<unsigned>(&cloud_callback<Character>);}
template<int Character>int CloudWeapon<Character>::update_phase(){auto& host=weapon_services();if(phase_age.current<=0){host.delete_animation(handle);return 1;}host.pause_animation(handle,false);host.show_animation(handle,true);auto& p=stats();const int maximum=std::clamp(ps::read<int>(p,0x50),100,500);ps::write(p,0x50,maximum);const int value=n::signed_bits(static_cast<unsigned>(maximum)*static_cast<unsigned>(phase_age.current))/phase_duration(p);ps::write(p,0x4c,std::clamp(value,0,500));frame_environment().clear_bullets(tail.position,tail.radius);frame_environment().clear_lasers(tail.position,tail.radius);auto damage_handle=host.create_damage_circle(tail.position,tail.radius,0,2,20);host.damage(damage_handle).flags|=0x40;n::timer_add(phase_age,-1.f,state::timer_rate);return 0;}
template<int Character>int CloudWeapon<Character>::end_phase(){weapon_services().delete_animation(handle);active=0;return 0;}
template<int Character>int CloudWeapon<Character>::cancel_phase(){weapon_services().pause_animation(handle,true);weapon_services().show_animation(handle,false);return 0;}
template<int Character>bool CloudWeapon<Character>::passive_active(){return controller()->phase==1;}
template class CloudWeapon<0>;template class CloudWeapon<1>;
template int update_cloud_animation<0>(sprite::Animation&);template int update_cloud_animation<1>(sprite::Animation&);
}
