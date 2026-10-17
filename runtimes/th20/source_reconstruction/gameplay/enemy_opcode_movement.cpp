#include "enemy_opcode_movement.hpp"
#include "enemy_movement.hpp"
#include "enemy_variables.hpp"
#include "../runtime_state/motion.hpp"
#include "../ecl_vm/math.hpp"
#include <bit>
#include <cmath>
namespace th20::source::gameplay {
namespace {
constexpr float missing=-999999.f,pi=3.1415927f,half_pi=1.5707964f,tau=6.2831855f;
using Motion=th20::source::state::Motion;
EnemyMovementRecord& record(EnemyState& s,unsigned i){if(s.movements.size()<=i)s.movements.resize(i+1);return s.movements.at(i);}
struct MotionEditor {
    EnemyMovementRecord& record;Motion value;
    explicit MotionEditor(EnemyMovementRecord& r):record(r),value(std::bit_cast<Motion>(r.motion)){}
    void save(){record.motion=std::bit_cast<EnemyMotion>(value);}
    ~MotionEditor(){save();}
};
void mode(Motion& m,unsigned value){m.field_44=(m.field_44&~15u)|value;} //47a500,47a4c0,49c470
float mirror(float angle){return ecl::math::wrap_angle(-ecl::math::wrap_angle(angle-half_pi)+half_pi);}
void angle(Motion& m,float value){m.angle_1c=ecl::math::wrap_angle(value);} //47a560/452f20
void apply_position(Motion& m,float rate){if((m.field_44&15u)==4)m.velocity=m.position;th20::source::state::update_motion_position(m,rate);} //453e00
template<class T>void prepare(sprite::Interpolation<T>& value,int duration,int curve,T start,T end,bool current){value.duration=duration;value.mode=curve;value.start=start;value.end=end;if(current)value.current=start;recovered::timer_set(value.timer,0);}
template<class T>void prepare_zero_tangents(sprite::Interpolation<T>& value,int duration,int curve,T start,T end){value.tangent_start={};value.tangent_end={};prepare(value,duration,curve,start,end,false);}
void position_curve(EnemyMotionInterpolation& value,int duration,int curve,sprite::Vec3 start,sprite::Vec3 end,sprite::Vec3 tangent_start={},sprite::Vec3 tangent_end={}){
    value.duration=duration;value.tangent_start=tangent_start;value.tangent_end=tangent_end;value.flags&=~1u;value.mode=curve;value.start=start;value.end=end;recovered::timer_set(value.timer,0); //438e70..4396a0; current is deliberately retained
}
sprite::Vec3 add(sprite::Vec3 a,sprite::Vec3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
}
EnemyOpcodeResult execute_enemy_movement_opcode(EnemyOpcodeReader& r,EnemyMovementOpcodeServices& env){
    auto& s=r.state;const auto op=r.opcode();if(op<400||op>448)return std::nullopt;
    const bool mirrored=(s.fields_2c8[1]&8u)!=0;
    const auto selected_index=[&](){return s.fields_1c[(0x44-0x1c)/4];};
    const auto combine=[&](MotionEditor& edit){edit.save();combine_enemy_movements(s,env.clock_scale());edit.value=std::bit_cast<Motion>(edit.record.motion);};
    switch(op){
    case 400:case 402:{MotionEditor edit(record(s,op==400?selected_index():1));auto& m=edit.value;const float x=r.real(0),y=r.real(1);if(missing<x)m.position.x=x;if(missing<y)m.position.y=y;mode(m,0);combine(edit);break;}
    case 401:case 403:case 434:case 435:case 436:case 437:case 438:case 439:{
        const bool axis=op==434||op==435||op==438||op==439;
        const bool relative=op>=436;
        MotionEditor edit(record(s,(op==401||op==434||op==436||op==438)?selected_index():1));auto& m=edit.value;auto& curve=edit.record.position;
        float x=r.real(axis?3:2),y=r.real(axis?4:3);
        if(r.integer(0)<1){curve.duration=0;break;}
        if(relative){x=mirrored?m.position.x-x:m.position.x+x;y=m.position.y+y;}
        const int duration=r.integer(0);const int mode_x=r.integer(1);const int mode_y=axis?r.integer(2):0;
        if(axis){curve.duration=duration;curve.tangent_start={};curve.tangent_end={};curve.flags|=1;curve.axis_modes[0]=mode_x;curve.axis_modes[1]=mode_y;curve.start=m.position;curve.end={x<=missing?m.position.x:x,y<=missing?m.position.y:y,0};recovered::timer_set(curve.timer,0);}
        else position_curve(curve,duration,mode_x,m.position,{x<=missing?m.position.x:x,y<=missing?m.position.y:y,0});
        mode(m,0);break;
    }
    case 404:case 406:case 428:case 430:{MotionEditor edit(record(s,(op==404||op==428)?selected_index():1));auto& m=edit.value;float a=r.real(0);const float speed=r.real(1);if(missing<a){if(mirrored&&(op==404||op==406))a=mirror(a);angle(m,a);}if(missing<speed)m.field_18=speed;mode(m,0);break;}
    case 405:case 407:case 429:case 431:{
        MotionEditor edit(record(s,(op==405||op==429)?selected_index():1));auto& m=edit.value;auto& angular=edit.record.scalar_ac;auto& speed=edit.record.scalar_d8;
        float a=r.real(2),v=r.real(3);if(r.integer(0)<1){angular.duration=speed.duration=0;break;}
        const int curve=r.integer(1);const bool reflect=mirrored&&(op==405||op==407);
        if(curve==7){a=a<=missing?0:reflect?-a:a;v=v<=missing?0:v;}
        else {a=a<=missing?m.angle_1c:reflect?mirror(a):a;v=v<=missing?m.field_18:v;}
        float start=m.angle_1c;if(pi<=std::fabs(start-a)){if(a<=start)a=a+tau;else start=start+tau;}
        const int duration=r.integer(0);prepare(angular,duration,curve,start,a,true);prepare(speed,duration,curve,m.field_18,v,true);mode(m,0);break;
    }
    case 408:case 410:case 420:case 422:{
        MotionEditor edit(record(s,(op==408||op==420)?selected_index():1));auto& m=edit.value;
        const float a=r.real(0),v=r.real(1),x=r.real(2),y=r.real(3);float a2=0,v2=0;if(op>=420){a2=r.real(4);v2=r.real(5);}
        if((m.field_44&15u)!=2)m.vector_38=m.position;
        if(missing<a)angle(m,a);if(missing<v)m.field_18=v;if(missing<x)m.field_20=x;if(missing<y)m.field_24=y;
        if(op>=420){if(missing<a2)m.angle_28=ecl::math::wrap_angle(ecl::math::wrap_angle(a2));if(missing<v2)m.field_2c=v2;}
        mode(m,op>=420?3:2);apply_position(m,env.clock_scale());combine(edit);break;
    }
    case 409:case 411:case 421:case 423:{
        MotionEditor edit(record(s,(op==409||op==421)?selected_index():1));auto& m=edit.value;
        const float v=r.real(2),x=r.real(3),y=r.real(4);float a2=0,v2=0;if(op>=421){a2=r.real(5);v2=r.real(6);}
        const float end_v=v<=missing?m.field_18:v;const sprite::Vec2 end_xy{x<=missing?m.field_20:x,y<=missing?m.field_24:y};
        const sprite::Vec2 end_second{a2<=missing?m.angle_28:a2,v2<=missing?m.field_2c:v2};
        const int duration=r.integer(0),curve=r.integer(1);if(duration<1){edit.record.scalar_d8.duration=edit.record.vector_104.duration=0;if(op>=421)edit.record.vector_144.duration=0;break;}
        prepare_zero_tangents(edit.record.scalar_d8,duration,curve,m.field_18,end_v);prepare_zero_tangents(edit.record.vector_104,duration,curve,sprite::Vec2{m.field_20,m.field_24},end_xy);
        if(op>=421){prepare_zero_tangents(edit.record.vector_144,duration,curve,sprite::Vec2{m.angle_28,m.field_2c},end_second);m.vector_38=m.position;}
        mode(m,op>=421?3:2);apply_position(m,env.clock_scale());combine(edit);break;
    }
    case 412:case 413:{
        MotionEditor edit(record(s,op==412?selected_index():1));auto& m=edit.value;
        const auto position=r.get<sprite::Vec3>(0x110);const float cx=r.get<float>(0x178),cy=r.get<float>(0x17c),width=r.get<float>(0x180),height=r.get<float>(0x184);float a;
        if(cx-width/4.f<=position.x){if(position.x<=width/4.f+cx){const auto player=env.player_position(r.context());a=env.random_angle()/4.f;const auto choice=(env.random_integer()&0xffffu)%3;if(player.x<=position.x){if(choice!=0)a=a+pi;}else if(choice==0)a=a+pi;}
            else a=ecl::math::wrap_angle(env.random_angle()/3.f+pi);}
        else a=env.random_angle()/3.f;
        if(cy-height/4.f<=position.y){if(height/4.f+cy<position.y)a=-std::fabs(a);}else a=std::fabs(a);
        if(std::fabs(a+half_pi)<0.05f)a=(-half_pi<=a)?-1.5207964f:-1.6207963f;
        else if(static_cast<double>(std::fabs(a-half_pi))<0.05)a=(half_pi<=a)?1.6207963f:1.5207964f;
        const int curve=r.integer(1);const float v=r.real(2);const int duration=r.integer(0);
        prepare_zero_tangents(edit.record.scalar_ac,duration,curve,a,a);
        // Original writes the mode to +ac a second time, leaving +d8.mode unchanged.
        auto& speed=edit.record.scalar_d8;speed.duration=r.integer(0);edit.record.scalar_ac.mode=curve;speed.tangent_start=speed.tangent_end=0;speed.start=v;speed.end=0;recovered::timer_set(speed.timer,0);mode(m,0);break;
    }
    case 414:case 415:case 432:case 433:{MotionEditor edit(record(s,(op==414||op==432)?0:1));auto* target=op<432?env.selected(r.controller(),0):env.find(r.controller(),unsigned(r.integer(0)));if(!target)throw std::logic_error("Enemy movement target is required by this original opcode");edit.value.position=enemy_position(target);break;}
    case 416:case 417:{MotionEditor edit(record(s,op==416?selected_index():1));auto& m=edit.value;m.position.x=m.position.x+r.real(0);m.position.y=m.position.y+r.real(1);m.position.z=m.position.z+r.real(2);combine(edit);break;}
    case 418:case 419:{MotionEditor edit(record(s,op==418?0:1));auto& m=edit.value;const float x=r.real(0),y=r.real(1);if(missing<x)m.vector_38.x=x;if(missing<y)m.vector_38.y=y;apply_position(m,env.clock_scale());combine(edit);break;}
    case 424:{const auto value=unsigned(r.integer(0));s.fields_2c8[1]=(s.fields_2c8[1]&~8u)|((value&1u)<<3);break;}
    case 425:case 426:{MotionEditor edit(record(s,op==425?selected_index():1));auto& m=edit.value;const float x=r.real(3),y=r.real(4);const sprite::Vec3 start_tangent{r.real(1),r.real(2),0},end_tangent{r.real(5),r.real(6),0};position_curve(edit.record.position,r.integer(0),8,m.position,{x<=missing?m.position.x:x,y<=missing?m.position.y:y,0},start_tangent,end_tangent);mode(m,0);break;}
    case 427:{for(unsigned i=0;i+1<s.movements.size();++i){const auto v=std::bit_cast<Motion>(s.movements[i+1].motion).position;r.put(0x110,add(r.get<sprite::Vec3>(0x110),v));}s.movements.resize(1);MotionEditor edit(s.movements[0]);edit.value.position={};edit.value.vector_38={};mode(edit.value,0);break;}
    case 440:case 442:{MotionEditor edit(record(s,op==440?selected_index():1));float a=r.real(0);if(mirrored)a=mirror(a);angle(edit.value,a);mode(edit.value,0);break;}
    case 441:case 443:{MotionEditor edit(record(s,op==441?selected_index():1));auto& m=edit.value;auto& angular=edit.record.scalar_ac;float a=r.real(2);if(r.integer(0)<1){angular.duration=0;break;}const int curve=r.integer(1);if(curve==7)a=a<=missing?0:mirrored?-a:a;else a=a<=missing?m.angle_1c:mirrored?mirror(a):a;a=ecl::math::angle_difference(a,m.angle_1c)+m.angle_1c;prepare(angular,r.integer(0),curve,m.angle_1c,a,true);mode(m,0);break;}
    case 444:case 446:{MotionEditor edit(record(s,op==444?selected_index():1));const float v=r.real(0);if(missing<v)edit.value.field_18=v;mode(edit.value,0);break;}
    case 445:case 447:{MotionEditor edit(record(s,op==445?selected_index():1));auto& m=edit.value;auto& speed=edit.record.scalar_d8;float v=r.real(2);const float start=m.field_18;if(r.integer(0)<1){speed.duration=0;break;}const int curve=r.integer(1);v=v<=missing?(curve==7?0:m.field_18):v;prepare(speed,r.integer(0),curve,start,v,true);mode(m,0);break;}
    case 448:s.fields_1c[(0x44-0x1c)/4]=unsigned(r.integer(0));break;
    }
    return 0;
}
}
