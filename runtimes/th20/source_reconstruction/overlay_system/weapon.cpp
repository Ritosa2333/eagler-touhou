#include "overlay.hpp"
#include "../gameplay/player_state.hpp"
#include "../player_entity/power.hpp"
#include "../player_entity/shots.hpp"
#include <algorithm>
namespace th20::source::overlay {
namespace ps=gameplay::player_state;namespace n=recovered;
namespace {
game_session::Player& stats(){return *game_session::context(0).current_player;}
player_entity::Player* player(){return static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);}
int clamp(game_session::Player& p,unsigned offset,int lower,int upper){const int value=std::clamp(ps::read<int>(p,offset),lower,upper);ps::write(p,offset,value);return value;}
int multiply(int a,int b){return n::signed_bits(static_cast<unsigned>(a)*static_cast<unsigned>(b));}
}
Weapon::Weapon() noexcept:stone_id(0),field_08(0),role(0),field_10(1),phase_age{},idle_age{},active(0),passive(0){}
void Weapon::reset(){n::timer_set(phase_age,0);n::timer_set(idle_age,0);active=passive=0;}
// These defaults are actual empty original method bodies, not unknown fallbacks:
// 40e5e0,52fbc0,414b60 contain only prologue/epilogue/ret. Their concrete
// overrides are recovered separately; no missing override is replaced here.
void Weapon::initialize_main(){}void Weapon::shoot_main(int,int,int){}void Weapon::shoot_focused(int,int,int){}void Weapon::shoot_unfocused(int,int,int){}
const sprite::Vec2* Weapon::focused_offset(int,int){return nullptr;} //477ce0 exact EAX=0
const sprite::Vec2* Weapon::unfocused_offset(int,int){return nullptr;} //477ce0
void Weapon::activate_main(){}void Weapon::activate_focused(){}void Weapon::activate_unfocused(){}
void Weapon::initialize_focused_option(player_entity::Option*,int){}void Weapon::initialize_unfocused_option(player_entity::Option*,int){}
void Weapon::update_focused_option(player_entity::Option*,int){}void Weapon::update_unfocused_option(player_entity::Option*,int){}
void Weapon::initialize_passive(){}void Weapon::update_main(){}void Weapon::update_focused(){}void Weapon::update_unfocused(){}
void Weapon::update_passive(){passive=0;}
int Weapon::script_variant(){return 0;} //412540 actual EAX=0
void Weapon::start_phase(){} //40e5e0
int Weapon::update_phase(){return 0;} //412540
int Weapon::end_phase(){
 active=0;
#ifdef TH_SDL3
 // Red rage shots can remain in flight with their rage damage long after
 // the meter expires. Remove only the main red rage pattern at the boundary.
 if(role==0&&stone_id>=0&&stone_id<2){
  if(auto* p=player()){
   auto& shots=p->shots;
   for(scheduler::Iterator it(shots.active.sentinel.next);it.current;it.advance()){
    auto& shot=*reinterpret_cast<player_entity::Shot*>(it.current->value);
    const auto pattern=shot.fields_b8[8]>>8;
    if(pattern>=unsigned(stone_id*20+15)&&pattern<=unsigned(stone_id*20+19))player_entity::retire_shot(shot);
   }
  }
 }
#endif
 return 0;
} //532f20
int Weapon::cancel_phase(){return 0;} //412540
const sprite::Vec3* Weapon::phase_position(){return nullptr;} //412540
int Weapon::shot_script_index(){if(role<0||role>2)return 0;return n::signed_bits(static_cast<unsigned>(stone_id)*20u+static_cast<unsigned>(role==0?0:role==1?5:10)+static_cast<unsigned>(player_entity::power_level(stats())));}
bool Weapon::phase_active(){return active!=0;}
bool Weapon::unfocused_shooting(){auto* p=player();return !controller()->main_shooting&&p&&p->shots.timer_12400.current>=0&&!p->focused_204c;}
bool Weapon::focused_shooting(){auto* p=player();return !controller()->main_shooting&&p&&p->shots.timer_12400.current>=0&&p->focused_204c;}
bool Weapon::passive_active(){return passive!=0;}
int phase_duration(game_session::Player& p){const int base=multiply(clamp(p,0x54,1,100),1200)/100;return n::signed_bits(static_cast<unsigned>(multiply(clamp(p,0xa8,0,100),base)/100)+static_cast<unsigned>(base));}
void StandardWeapon::shoot_main(int first,int second,int level){auto& owner=*controller();owner.main_shooting=active?1:0;fire_player_shots(player()->shots,first,second,n::signed_bits(static_cast<unsigned>(stone_id)*20u+(active?15u:0u)+static_cast<unsigned>(level)));}
void StandardWeapon::shoot_focused(int first,int second,int level){if(!controller()->main_shooting)fire_player_shots(player()->shots,first,second,n::signed_bits(static_cast<unsigned>(stone_id)*20u+10u+static_cast<unsigned>(level)));}
void StandardWeapon::shoot_unfocused(int first,int second,int level){if(!controller()->main_shooting)fire_player_shots(player()->shots,first,second,n::signed_bits(static_cast<unsigned>(stone_id)*20u+5u+static_cast<unsigned>(level)));}
const sprite::Vec2* StandardWeapon::focused_offset(int level,int index){const unsigned offsets[]{0x124,0x124,0x12c,0x13c,0x154};const unsigned offset=level>=1&&level<=4?offsets[level]+static_cast<unsigned>(index)*8u:0x124;return reinterpret_cast<const sprite::Vec2*>(static_cast<const std::uint8_t*>(player()->shot_data)+static_cast<unsigned>(stone_id)*0xa0u+offset);}
const sprite::Vec2* StandardWeapon::unfocused_offset(int level,int index){const unsigned offsets[]{0xd4,0xd4,0xdc,0xec,0x104};const unsigned offset=level>=1&&level<=4?offsets[level]+static_cast<unsigned>(index)*8u:0xd4;return reinterpret_cast<const sprite::Vec2*>(static_cast<const std::uint8_t*>(player()->shot_data)+static_cast<unsigned>(stone_id)*0xa0u+offset);}
void StandardWeapon::start_phase(){n::timer_set(phase_age,phase_duration(stats()));active=1;n::timer_set(idle_age,0);}
int StandardWeapon::update_phase(){
    if(phase_age.current<=0)return 1;
    auto& p=stats();const int maximum=clamp(p,0x50,100,500);const int duration=phase_duration(p);ps::write(p,0x4c,std::clamp(multiply(maximum,phase_age.current)/duration,0,500));
    if((player()->shots.field_1255c&2)==0)n::timer_tick(idle_age,state::timer_rate);else n::timer_set(idle_age,0);
    if(idle_age.current<30||idle_age.current%4==0)n::timer_add(phase_age,-1.f,state::timer_rate);return 0;
}
}
