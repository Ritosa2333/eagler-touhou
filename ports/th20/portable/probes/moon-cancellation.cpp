#include "../../../source_reconstruction/bullet_system/shoot.hpp"
#include "../../../source_reconstruction/overlay_system/yellow_weapon.hpp"
extern "C" __attribute__((export_name("moon_stress"))) int moon_stress(int count,int pattern) {
 using namespace th20::source;
 auto* b=bullet::controller(0);auto* p=static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);if(!b||!p)return -1;
 auto m=std::make_shared<bullet::ShotMetadata>();m->field_3c=-1;
 for(int i=0;i<count;++i){bullet::ShotParameters s{};s.fields_00[0]=48;s.fields_00[1]=6;s.position={float(i%20)*16-152,float(i/20%20)*16+64,0};s.count=s.rows=1;s.speed=.6f;bullet::shoot_one(*b,s,m,0,0,0);}
 overlay::YellowWeapon<0> w;w.stone_id=pattern;th20::recovered::timer_set(w.end_timer,1);w.update_main();return p->shots.field_12464;
}

#include "../../../source_reconstruction/damage_regions/damage.hpp"
extern "C" __attribute__((export_name("moon_hit_stress"))) int moon_hit_stress() {
 using namespace th20::source;
 auto* p=static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
 for(int i=0;i<600;++i)overlay::weapon_services().fire_shots_at_position(115,0,0,{0,120,0});
 for(scheduler::Iterator it(damage::controller()->active.sentinel.next);it.current;it.advance()){auto& r=*reinterpret_cast<damage::Region*>(it.current->value);r.lifetime.current=100;r.lifetime.previous=101;}
 p->timers_644[0].current=p->timers_644[0].previous+1;
 return damage::calculate_damage(*damage::controller(),{0,120,0},nullptr,0,10000,nullptr,nullptr,0,0);
}

