#include "shield_weapon.hpp"
#include "frame.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
#include <iterator>
namespace th20::source::overlay {
namespace n=recovered;namespace ps=gameplay::player_state;
namespace {
ShieldState reimu_state{},marisa_state{}; // BSS, original first mutation is5350b0/538370
game_session::Player& stats(){return *game_session::context(0).current_player;}player_entity::Player& player(){return *static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);}
void __fastcall empty_option(player_entity::Option*,void*){} //40e5e0 exact original empty method
int __cdecl shield_callback(sprite::Animation* animation){return update_shield_animation(*animation);}
}
ShieldState& shield_state(int character) noexcept{return character==0?reimu_state:marisa_state;}
int update_shield_animation(sprite::Animation& animation){const auto& weapon=*reinterpret_cast<const Weapon*>(animation.field_5c8);const float numerator=n::int_float(weapon.phase_age.current)*96.f;const int duration=phase_duration(stats());animation.base.vector_50={numerator/n::int_float(duration)+26.f,1.f};animation.base.flags[1]|=4;animation.vector_5bc=player().position_614;return 0;}
template<int Character>void ShieldWeapon<Character>::reset(){active=0;n::timer_set(phase_age,0);field_10=1;n::timer_set(idle_age,0);n::timer_set(timer_38,0);auto& s=shield_state(Character);std::fill(std::begin(s.animation_handles),std::end(s.animation_handles),0u);std::fill(std::begin(s.damage_handles),std::end(s.damage_handles),0u);s.phase_animation=0;}
template<int Character>void ShieldWeapon<Character>::shoot_main(int first,int second,int level){fire_player_shots(player().shots,first,second,n::signed_bits(static_cast<unsigned>(stone_id)*20u+static_cast<unsigned>(level)));}
template<int Character>void ShieldWeapon<Character>::shoot_focused(int first,int second,int level){StandardWeapon::shoot_focused(first,second,level);update_options(level);}
template<int Character>void ShieldWeapon<Character>::shoot_unfocused(int first,int second,int level){StandardWeapon::shoot_unfocused(first,second,level);update_options(level);}
template<int Character>void ShieldWeapon<Character>::activate_main(){auto& s=shield_state(Character);auto& host=weapon_services();for(auto& handle:s.animation_handles){host.delete_animation(handle);handle=0;}for(auto& handle:s.damage_handles)host.retire_damage(handle);}
template<int Character>void ShieldWeapon<Character>::initialize_focused_option(player_entity::Option* option,int){if constexpr(Character==0)ps::write(*option,0x11c,reinterpret_cast<unsigned>(&empty_option));}
template<int Character>void ShieldWeapon<Character>::initialize_unfocused_option(player_entity::Option* option,int){if constexpr(Character==0)ps::write(*option,0x120,reinterpret_cast<unsigned>(&empty_option));}
template<int Character>void ShieldWeapon<Character>::initialize_passive(){stats().byte_b0=1;ps::write(stats(),0xb4,0);}
template<int Character>void ShieldWeapon<Character>::start_phase(){n::timer_set(phase_age,phase_duration(stats()));active=1;effects::Parameters p;effects::construct_parameters(p);p.vector_00=player().position_614;auto& handle=shield_state(Character).phase_animation;auto& host=weapon_services();host.delete_animation(handle);handle=host.spawn_effect(7,p);auto& animation=host.animation(handle);animation.field_5c8=reinterpret_cast<unsigned>(this);animation.field_5dc=reinterpret_cast<unsigned>(&shield_callback);}
template<int Character>int ShieldWeapon<Character>::update_phase(){const bool ended=phase_age.current<=0;if(!ended){auto& host=weapon_services();auto& handle=shield_state(Character).phase_animation;host.pause_animation(handle,false);host.show_animation(handle,true);auto& p=stats();const int maximum=std::clamp(ps::read<int>(p,0x50),100,500);ps::write(p,0x50,maximum);const int value=n::signed_bits(static_cast<unsigned>(maximum)*static_cast<unsigned>(phase_age.current))/phase_duration(p);ps::write(p,0x4c,std::clamp(value,0,500));n::timer_add(phase_age,-1.f,state::timer_rate);}return ended;}
template<int Character>int ShieldWeapon<Character>::end_phase(){if(controller()->phase==2){frame_environment().clear_bullets(player().position_614,32.f);frame_environment().clear_lasers(player().position_614,32.f);n::timer_add(phase_age,-200.f,state::timer_rate);return 1;}weapon_services().delete_animation(shield_state(Character).phase_animation);active=0;return 0;}
template<int Character>int ShieldWeapon<Character>::cancel_phase(){auto& handle=shield_state(Character).phase_animation;weapon_services().pause_animation(handle,true);weapon_services().show_animation(handle,false);return 0;}
template<int Character>void ShieldWeapon<Character>::update_options(int level){auto& s=shield_state(Character);auto& host=weapon_services();for(int index=0;index<level;++index){const auto position=player().options[index].previous_position;
 if(s.animation_handles[index]==0){effects::Parameters p;effects::construct_parameters(p);p.vector_00=player().position_614;s.animation_handles[index]=host.spawn_effect(7,p);host.animation(s.animation_handles[index]).base.vector_50={32.f,1.f};host.retire_damage(s.damage_handles[index]);s.damage_handles[index]=host.create_damage_circle(position,32.f,0,9999999,2);host.damage(s.damage_handles[index]).period=3;host.damage(s.damage_handles[index]).damage_limit=9999999;host.damage(s.damage_handles[index]).flags|=0x40;}
 host.animation_position(s.animation_handles[index],position);host.damage_position(s.damage_handles[index],position);
 }for(int index=level;index<4;++index){host.delete_animation(s.animation_handles[index]);host.retire_damage(s.damage_handles[index]);}}
template class ShieldWeapon<0>;template class ShieldWeapon<1>;
}
