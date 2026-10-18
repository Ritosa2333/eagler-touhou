#include "yellow_weapon.hpp"
#include "frame.hpp"
#include "../gameplay/player_state.hpp"
#include "../player_entity/power.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
#include <iterator>
namespace th20::source::overlay {
namespace n=recovered;namespace ps=gameplay::player_state;namespace math=ecl::math;
namespace {
YellowState reimu_state{},marisa_state{};
constexpr float pi=3.1415927410125732421875f;
game_session::Player& stats(){return *game_session::context(0).current_player;}player_entity::Player& player(){return *static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);}
template<int Character>void __fastcall option_callback(player_entity::Option* p,void*){update_yellow_option(*p,Character);}
}
YellowState& yellow_state(int character) noexcept{return character==0?reimu_state:marisa_state;}
template<int C>void YellowWeapon<C>::reset(){active=0;n::timer_set(phase_age,0);field_10=1;n::timer_set(idle_age,0);auto& s=yellow_state(C);std::fill(std::begin(s.animation_handles),std::end(s.animation_handles),0u);std::fill(std::begin(s.damage_handles),std::end(s.damage_handles),0u);s.phase_animation=0;n::timer_set(passive_timer,0);n::timer_set(end_timer,0);if constexpr(C==1)cooldown=0;}
template<int C>void YellowWeapon<C>::shoot_main(int a,int b,int level){fire_player_shots(player().shots,a,b,n::signed_bits(static_cast<unsigned>(stone_id)*20u+static_cast<unsigned>(level)));}
template<int C>void YellowWeapon<C>::shoot_focused(int a,int b,int level){StandardWeapon::shoot_focused(a,b,level);update_options(level);}
template<int C>void YellowWeapon<C>::shoot_unfocused(int a,int b,int level){StandardWeapon::shoot_unfocused(a,b,level);update_options(level);}
template<int C>void YellowWeapon<C>::activate_main(){auto& s=yellow_state(C);for(auto& h:s.animation_handles){weapon_services().delete_animation(h);h=0;}for(auto& h:s.damage_handles)weapon_services().retire_damage(h);}
template<int C>void YellowWeapon<C>::activate_focused(){auto& s=yellow_state(C);if(ps::read<int>(stats(),0xc)!=5||controller()->phase!=1)for(auto& h:s.animation_handles){weapon_services().delete_animation(h);h=0;}for(auto& h:s.damage_handles)weapon_services().retire_damage(h);}
template<int C>void YellowWeapon<C>::initialize_focused_option(player_entity::Option* p,int){ps::write(*p,0x11c,reinterpret_cast<unsigned>(&option_callback<C>));}
template<int C>void YellowWeapon<C>::initialize_unfocused_option(player_entity::Option* p,int){ps::write(*p,0x120,reinterpret_cast<unsigned>(&option_callback<C>));}
template<int C>void YellowWeapon<C>::initialize_passive(){n::timer_set(passive_timer,0);}
template<int C>void YellowWeapon<C>::start_phase(){n::timer_set(phase_age,phase_duration(stats()));active=1;}
template<int C>int YellowWeapon<C>::end_phase(){n::timer_set(end_timer,60);yellow_state(C).phase_animation=weapon_services().spawn_player_animation(C==0?"pl00":"pl01",C==0?31:27,player().position_614);active=0;return 0;}
template<int C>int YellowWeapon<C>::cancelled_bullet(const sprite::Vec3& position,bool passive_callback){weapon_services().fire_shots_at_position(n::signed_bits(static_cast<unsigned>(stone_id)*20u+15u),0,0,position);if(passive_callback)passive=1;return 1;}
template<int C>void YellowWeapon<C>::update_main(){if(end_timer.current>0){const float radius=(60.f-end_timer.current_f)*10.f;
 weapon_services().cancel_filtered(player().position_614,radius,[this](const sprite::Vec3& p){return cancelled_bullet(p,false);});
 frame_environment().clear_lasers(player().position_614,radius);n::timer_add(end_timer,-1.f,state::timer_rate);}}
template<int C>void YellowWeapon<C>::update_passive(){passive=0;if(passive_timer.current!=passive_timer.previous)for(int index=0;index<player_entity::power_level(stats());++index){const int period=stats().bytes_2c[3]?22:16;if(passive_timer.current%period==index*4){const auto p=player().options[index].previous_position;weapon_services().cancel_filtered(p,3.f,[this](const sprite::Vec3& position){return cancelled_bullet(position,true);});}}n::timer_tick(passive_timer,state::timer_rate);}
template<int C>void YellowWeapon<C>::update_options(int level){auto& s=yellow_state(C);auto& host=weapon_services();for(int index=0;index<level;++index){const auto p=player().options[index].previous_position;if(!s.animation_handles[index]){effects::Parameters params;effects::construct_parameters(params);params.vector_00=player().position_614;s.animation_handles[index]=host.spawn_effect(8,params);host.animation(s.animation_handles[index]).base.vector_50={32.f,1.f};}host.animation_position(s.animation_handles[index],p);if(cooldown)cooldown=n::signed_bits(static_cast<unsigned>(cooldown)-1u);if(!cooldown)cooldown=n::signed_bits(static_cast<unsigned>(cooldown)+static_cast<unsigned>(host.cancel_counted(p,2.f)));}for(int index=level;index<4;++index){host.delete_animation(s.animation_handles[index]);host.retire_damage(s.damage_handles[index]);}}
template<int C>int YellowWeapon<C>::update_phase(){if(phase_age.current<=0)return 1;auto& p=stats();const int maximum=std::clamp(ps::read<int>(p,0x50),100,500);ps::write(p,0x50,maximum);ps::write(p,0x4c,std::clamp(n::signed_bits(static_cast<unsigned>(maximum)*static_cast<unsigned>(phase_age.current))/phase_duration(p),0,500));auto& s=yellow_state(C);auto& host=weapon_services();for(int index=0;index<player_entity::power_level(stats());++index){const auto pos=player().options[index].previous_position;if(!s.animation_handles[index]){effects::Parameters params;effects::construct_parameters(params);params.vector_00=player().position_614;s.animation_handles[index]=host.spawn_effect(8,params);host.animation(s.animation_handles[index]).base.vector_50={32.f,1.f};}host.animation_position(s.animation_handles[index],pos);if(cooldown)cooldown=n::signed_bits(static_cast<unsigned>(cooldown)-1u);if(!cooldown){const int removed=host.cancel_filtered(pos,8.f,[this](const sprite::Vec3& q){return cancelled_bullet(q,false);});cooldown=n::signed_bits((static_cast<unsigned>(cooldown)+static_cast<unsigned>(removed))*2u);frame_environment().clear_lasers(pos,8.f);}}n::timer_add(phase_age,-1.f,state::timer_rate);return 0;}
void update_yellow_option(player_entity::Option& option,int character){
 auto& rng=state::random_streams[0];auto& age=option.timer_e4;
 float angle=ps::read<float>(option,0xd4);if(age.current==0)angle=math::wrap_angle(state::signed_unit(rng)*pi);
 const bool focused=player().focused_204c!=0;const auto offset=ps::read<player_entity::Fixed2>(option,focused?0x88:0x80);
 float x=math::sine(angle)*(character==0?40.f:25.f),y=math::sine(math::wrap_angle(angle*2.f))*8.f;
 if(character==1){std::swap(x,y);math::rotate(x,y,math::arctangent(n::int_float(offset.y)/128.f,n::int_float(offset.x)/128.f));}
 auto& context=*ps::read<game_session::Context*>(option,0x128);auto& owner=*static_cast<player_entity::Player*>(context.objects_04[0]);
 option.vector_70.x=n::signed_bits(static_cast<unsigned>(n::truncate32(x*128.f))+static_cast<unsigned>(owner.fixed_position.x)+static_cast<unsigned>(offset.x));option.vector_70.y=n::signed_bits(static_cast<unsigned>(n::truncate32(y*128.f))+static_cast<unsigned>(owner.fixed_position.y)+static_cast<unsigned>(offset.y));
 const float base=(pi*2.f)/(character==0&&focused?3000.f:1000.f);const float random=state::unit(rng);ps::write(option,0xd4,math::wrap_angle(angle+(base+random*(pi*2.f)/1000.f)));n::timer_tick(age,state::timer_rate);
}
template class YellowWeapon<0>;template class YellowWeapon<1>;
}
