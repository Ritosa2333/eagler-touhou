#include "shoot.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
namespace th20::source::bullet {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
constexpr float pi=3.1415927410125732f;
}
ShotTrajectory shot_trajectory(const ShotParameters& p,std::uint16_t pattern,std::uint32_t column,std::uint32_t row,float player_angle){
    const auto c=n::signed_bits(column),r=n::signed_bits(row);
    float angle=0,speed=p.rows<2?p.speed:sub(p.speed,div(n::mul32(sub(p.speed,p.speed_step),n::int_float(r)),sub(n::int_float(p.rows),1)));
    const float initial_speed=speed;
    const auto circle=[&](){return div(n::mul32(n::int_float(c),n::mul32(pi,2)),n::int_float(p.count));};
    const auto row_angle=[&](){return n::add32(n::mul32(n::int_float(r),p.angle_step),p.angle);};
    switch(pattern){
    case 0:case 1:{
        const float offset=(p.count&1)?n::mul32(n::int_float(n::signed_bits(column+1u)/2),p.angle_step):n::add32(n::mul32(n::int_float(c/2),p.angle_step),n::mul32(p.angle_step,0.5f));
        angle=n::add32(angle,offset);if(column&1u)angle=n::mul32(angle,-1);if(pattern==0)angle=n::add32(angle,player_angle);angle=n::add32(angle,p.angle);break;
    }
    case 2:angle=n::add32(angle,player_angle);[[fallthrough]];
    case 3:angle=n::add32(circle(),angle);angle=n::add32(row_angle(),angle);break;
    case 4:angle=n::add32(angle,player_angle);[[fallthrough]];
    case 5:angle=n::add32(div(pi,n::int_float(p.count)),angle);angle=n::add32(circle(),angle);angle=n::add32(row_angle(),angle);break;
    case 6:angle=n::add32(n::mul32(state::signed_unit(state::random_streams[0]),p.angle_step),p.angle);break;
    case 7:speed=n::add32(n::mul32(state::unit(state::random_streams[0]),p.speed_step),p.speed);angle=n::add32(circle(),angle);angle=n::add32(row_angle(),angle);break;
    case 8:angle=n::add32(n::mul32(state::signed_unit(state::random_streams[0]),p.angle_step),p.angle);speed=n::add32(n::mul32(state::unit(state::random_streams[0]),p.speed_step),p.speed);break;
    case 9:case 10:{
        angle=circle();
        if(!(p.rows&1)){
            angle=n::add32(n::add32(n::mul32(n::int_float(r/2),p.angle_step),n::mul32(p.angle_step,0.5f)),angle);
            if(p.rows>1)speed=n::add32(div(n::mul32(n::int_float(row&0xfffeu),sub(p.speed_step,p.speed)),n::int_float(p.rows-1)),p.speed);
        }else{
            angle=n::add32(n::mul32(n::int_float(n::signed_bits(row+1u)/2),p.angle_step),angle);
            if(p.rows>1)speed=n::add32(div(n::mul32(n::int_float((row+1u)&0xfffeu),sub(p.speed_step,p.speed)),n::int_float(p.rows-1)),p.speed);
        }
        if(row&1u)angle=n::mul32(angle,-1);if(pattern==9)angle=n::add32(angle,player_angle);angle=n::add32(angle,p.angle);break;
    }
    case 11:{angle=circle();const float x=n::mul32(m::cosine(angle),p.speed),y=n::mul32(m::sine(angle),p.speed_step);speed=m::square_root(n::add32(n::mul32(x,x),n::mul32(y,y)));angle=n::add32(m::arctangent(y,x),p.angle);break;}
    case 12:{const float sample=n::add32(circle(),div(pi,n::int_float(p.count)));angle=n::add32(n::add32(sample,p.angle),angle);speed=n::mul32(sub(1,n::mul32(std::fabs(m::sine(sample)),p.speed_step)),speed);break;}
    }
    return {angle,speed,initial_speed};
}
}
