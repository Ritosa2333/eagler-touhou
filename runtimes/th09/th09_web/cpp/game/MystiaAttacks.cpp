#include "AttackTravel.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th09 {
namespace {
constexpr float pi=3.1415927410125732f;
enum class BirdPath {normal,crossing,left,right,directed};
enum class BirdFire {normal,paired,spiral,alternating_spiral,alternating_straight};
Vec3 polar(float a,float r){return {float(std::cos(double(a))*double(r)),float(std::sin(double(a))*double(r)),0};}
bool initialize_bird(AttackActor& a,CharacterAttackServices& s,BirdPath path){
    a.animations.resize(1);std::memset(a.animations.data(),0,sizeof(AnmVm));a.state=std::make_unique<MystiaAttackState>();auto& t=static_cast<MystiaAttackState&>(*a.state);
    if(!s.start_animation(a,0,a.source_side,AttackAnimationResource::shared_effects,37))return true;
    const float angle=float(s.random.range(-pi));t.velocity=polar(path==BirdPath::directed?a.position.z:angle,2);
    if(path==BirdPath::directed){t.flight_angle=a.position.z;a.position.z=0;}
    a.layer=2;if(path!=BirdPath::normal)a.time.reset(90);
    float radius=float(s.random.range(192)),heading=float(s.random.signed_unit())*pi;t.start_tangent=polar(heading,radius);
    radius=float(s.random.range(192));heading=float(s.random.signed_unit())*pi;t.end_tangent=polar(heading,radius);
    t.origin=s.geometry[a.source_side].to_screen(a.position);t.origin.z=0;
    switch(path){
    case BirdPath::normal:case BirdPath::crossing:{
        const bool right=s.random.bounded32(2)==0;t.target.x=right?144.f:-144.f;
        if(path==BirdPath::normal){
            const float delta=float(s.random.signed_unit())*.5235987901687622f;t.flight_angle=right?float(add_angle(pi,delta)):delta;
            t.angular_velocity=float(s.random.signed_unit())*.013089969754219055f;t.target.y=float(s.random.range(320));
        }else {t.flight_angle=right?pi:0;t.target.y=224;}break;
    }
    case BirdPath::left:t.target={-144,192,0};t.flight_angle=0;break;
    case BirdPath::right:t.target={144,192,0};t.flight_angle=pi;break;
    case BirdPath::directed:t.target={a.position.x,a.position.y,0};break;
    }
    t.destination=s.geometry[a.destination_side].to_screen(t.target);t.destination.z=0;
    if(path!=BirdPath::normal)t.volley_angle=float(s.random.signed_unit())*pi;return false;
}
bool initialize_mystia(AttackActor& a,AttackServices& s){return initialize_bird(a,static_cast<CharacterAttackServices&>(s),BirdPath::normal);}
bool initialize_crossing(AttackActor& a,AttackServices& s){return initialize_bird(a,static_cast<CharacterAttackServices&>(s),BirdPath::crossing);}
bool initialize_left(AttackActor& a,AttackServices& s){return initialize_bird(a,static_cast<CharacterAttackServices&>(s),BirdPath::left);}
bool initialize_right(AttackActor& a,AttackServices& s){return initialize_bird(a,static_cast<CharacterAttackServices&>(s),BirdPath::right);}
bool initialize_directed(AttackActor& a,AttackServices& s){return initialize_bird(a,static_cast<CharacterAttackServices&>(s),BirdPath::directed);}
bool update_bird(AttackActor& a,CharacterAttackServices& s,BirdFire fire){
    auto& t=static_cast<MystiaAttackState&>(*a.state);
    if(t.phase==0){
        interpolate_attack_travel(a,t);
        if(a.time.current>90){s.start_animation(a,0,a.source_side,AttackAnimationResource::character_shots,9);++t.phase;a.position=t.target;a.layer=a.destination_side;a.time.reset();}
        return false;
    }
    if(t.phase==2)return a.time.current>20;if(t.phase!=1)return false;
    if(a.time.current>20){
        const i32 interval=fire==BirdFire::normal?20-s.rank/8:fire==BirdFire::paired||fire==BirdFire::spiral?10-s.difficulty:10;
        if(interval>0&&a.time.current!=a.time.previous&&a.time.current%interval==0){
            BulletEmission b;b.position=a.position;b.sprite=6;b.color=fire==BirdFire::normal?6:2;b.pattern=5;b.layers=1;b.flags=0x20014;
            const bool alternate=fire==BirdFire::alternating_spiral||fire==BirdFire::alternating_straight;
            if(fire!=BirdFire::normal)t.volley_angle=float(add_angle(t.volley_angle,.10471975803375244f));
            b.angle=fire==BirdFire::normal||fire==BirdFire::alternating_straight?t.flight_angle:t.volley_angle;
            float acceleration;
            if(fire==BirdFire::normal){b.count=2;acceleration=float(s.rank)*.0003787878667935729f+.015833333134651184f;}
            else if(alternate){b.count=1;b.sprite=t.alternate_sprite?6:0;acceleration=(float(s.attack_levels[a.source_side][1])*.05f+2.f)*.008333333767950535f;}
            else {b.count=i16(s.attack_levels[a.source_side][0]/5+2);acceleration=(float(s.attack_levels[a.source_side][0])*.1f+2.f)*.008333333767950535f;}
            b.extras[0]={0,0,80,0,0x20000,0};b.extras[1]={acceleration,-999,120,0,0x10,0};s.emit_bullets(a.destination_side,false,b);
            if(fire==BirdFire::paired){b.sprite=0;b.pattern=3;b.angle=float(add_angle(pi,-t.volley_angle));s.emit_bullets(a.destination_side,false,b);}
            if(alternate)t.alternate_sprite=1-t.alternate_sprite;
        }
        if(s.probe_cancellation(a.destination_side,a.position,{24,24,0})==2){t.phase=2;a.time.reset();a.animations[0].pendingInterrupt=1;a.angle=t.flight_angle;return false;}
        t.velocity=polar(t.flight_angle,1.5f);if(fire==BirdFire::normal)t.flight_angle=float(add_angle(t.flight_angle,t.angular_velocity));
        a.position={t.velocity.x+a.position.x,t.velocity.y+a.position.y,t.velocity.z+a.position.z};
        if(a.position.y>=480||a.position.y<=-32||a.position.x>=176||a.position.x<=-176)return true;
    }
    a.angle=t.flight_angle;return false;
}
bool update_mystia(AttackActor& a,AttackServices& s){return update_bird(a,static_cast<CharacterAttackServices&>(s),BirdFire::normal);}
bool update_crossing(AttackActor& a,AttackServices& s){return update_bird(a,static_cast<CharacterAttackServices&>(s),BirdFire::paired);}
bool update_side(AttackActor& a,AttackServices& s){return update_bird(a,static_cast<CharacterAttackServices&>(s),BirdFire::spiral);}
bool update_directed_spiral(AttackActor& a,AttackServices& s){return update_bird(a,static_cast<CharacterAttackServices&>(s),BirdFire::alternating_spiral);}
bool update_directed_straight(AttackActor& a,AttackServices& s){return update_bird(a,static_cast<CharacterAttackServices&>(s),BirdFire::alternating_straight);}
}
void add_mystia_attack_behaviors(std::array<AttackBehavior,27>& result){
    result[6]={initialize_mystia,update_mystia,nullptr,nullptr};result[7]={initialize_crossing,update_crossing,nullptr,nullptr};
    result[8]={initialize_right,update_side,nullptr,nullptr};result[9]={initialize_left,update_side,nullptr,nullptr};
    result[11]={initialize_directed,update_directed_spiral,nullptr,nullptr};result[12]={initialize_directed,update_directed_straight,nullptr,nullptr};
}
}
