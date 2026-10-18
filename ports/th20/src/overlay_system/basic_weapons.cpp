#include "basic_weapons.hpp"
#include "../gameplay/player_state.hpp"
#include "../player_entity/power.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::overlay {
namespace ps=gameplay::player_state;namespace math=ecl::math;namespace n=recovered;
namespace {
game_session::Player& stats(){return *game_session::context(0).current_player;}
void __fastcall orbit_reimu(player_entity::Option* option,void*){orbit_option(*option,0);}
void __fastcall orbit_marisa(player_entity::Option* option,void*){orbit_option(*option,1);}
void initialize_orbit(player_entity::Option& option,int index,int character,bool focused){
 ps::write(option,focused?0x11c:0x120,reinterpret_cast<std::uintptr_t>(character==0?&orbit_reimu:&orbit_marisa));
 if(player_entity::power_level(stats())!=0){const float total=3.1415927410125732421875f*2.f;const float step=total/n::int_float(player_entity::power_level(stats()));ps::write(option,0xd4,math::wrap_angle(n::int_float(index)*step));}
}
}
template<int Character>void FocusBoostWeapon<Character>::initialize_passive(){ps::write(stats(),0x9c,stats().bytes_2c[3]?7:12);}
template<int Character>void FlagBoostWeapon<Character>::initialize_passive(){stats().bytes_a4[0]=1;}
template<int Character>void OrbitWeapon<Character>::initialize_passive(){ps::write(stats(),0xa0,stats().bytes_2c[3]?7:12);}
template<int Character>void OrbitWeapon<Character>::initialize_focused_option(player_entity::Option* option,int index){initialize_orbit(*option,index,Character,true);}
template<int Character>void OrbitWeapon<Character>::initialize_unfocused_option(player_entity::Option* option,int index){initialize_orbit(*option,index,Character,false);}
void orbit_option(player_entity::Option& option,int character){
 auto& context=*ps::read<game_session::Context*>(option,0x128);auto& player=*static_cast<player_entity::Player*>(context.objects_04[0]);
 const float radius=character==0?(player.focused_204c?32.f:60.f):(player.focused_204c?28.f:40.f);
 float x=0,y=0;float angle=ps::read<float>(option,0xd4);math::polar(x,y,angle,radius);
 option.vector_70.x=n::signed_bits(static_cast<unsigned>(n::truncate32(x*128.f))+static_cast<unsigned>(player.fixed_position.x));
 option.vector_70.y=n::signed_bits(static_cast<unsigned>(n::truncate32(y*128.f))+static_cast<unsigned>(player.fixed_position.y));
 const float step=3.1415927410125732421875f/(player.focused_204c?40.f:120.f);
 // Marisa uses452f80→4396c0; Reimu uses452fc0. The angle wrapper
 // is intentionally invoked again by the recovered452f20 setter.
 angle=character==0?angle+step:math::angle_difference(angle,step);
 ps::write(option,0xd4,math::wrap_angle(math::wrap_angle(angle)));
}
template class FocusBoostWeapon<0>;template class FocusBoostWeapon<1>;
template class FlagBoostWeapon<0>;template class FlagBoostWeapon<1>;
template class OrbitWeapon<0>;template class OrbitWeapon<1>;
}
