#include "enemy_interpolation.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::gameplay {
namespace {
float subtract(float a,float b) noexcept{return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float divide(float a,float b) noexcept{return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float& component(sprite::Vec3& v,unsigned axis) noexcept{return axis==0?v.x:axis==1?v.y:v.z;}
}
sprite::Vec3 sample_enemy_motion_interpolation(EnemyMotionInterpolation& value,const float* rate) {
    const auto endpoint=[&]{return value.mode==7||value.mode==17?value.start:value.end;};
    if(value.duration>0) {
        recovered::timer_tick(value.timer,rate);
        if(value.timer.current>=value.duration){recovered::timer_set(value.timer,value.duration);value.duration=0;return endpoint();}
    } else if(value.duration==0)return endpoint();
    for(unsigned axis=0;axis<3;++axis) {
        const auto mode=value.flags&1u?value.axis_modes[axis]:value.mode;
        auto& start=component(value.start,axis);auto& end=component(value.end,axis);
        auto& tangent_start=component(value.tangent_start,axis);auto& tangent_end=component(value.tangent_end,axis);
        auto& current=component(value.current,axis);
        if(mode==7){start=recovered::add32(start,end);current=start;}
        else if(mode==17){start=recovered::add32(start,tangent_end);tangent_end=recovered::add32(tangent_end,end);current=start;}
        else if(mode==8) {
            const float t=divide(value.timer.current_f,recovered::int_float(value.duration));
            const float c0=recovered::mul32(recovered::mul32(subtract(t,1),subtract(t,1)),recovered::add32(recovered::mul32(2,t),1));
            const float c1=recovered::mul32(recovered::mul32(t,t),subtract(3,recovered::mul32(2,t)));
            const float c2=recovered::mul32(recovered::mul32(subtract(1,t),subtract(1,t)),t);
            const float c3=recovered::mul32(recovered::mul32(subtract(t,1),t),t);
            current=recovered::add32(recovered::add32(recovered::add32(recovered::mul32(start,c0),recovered::mul32(end,c1)),recovered::mul32(tangent_start,c2)),recovered::mul32(tangent_end,c3));
        } else {
            const auto ease=ecl::math::easing(mode,value.timer.current_f,recovered::int_float(value.duration)); //4aab10/4aab70
            current=recovered::add32(recovered::mul32(subtract(end,start),ease),start);
        }
    }
    return value.current;
}
float sample_enemy_scalar_interpolation(sprite::Interpolation<float>& value,const float* rate) {
    ecl::math::Interpolator scalar{};
    scalar.start=value.start;scalar.end=value.end;scalar.tangent_start=value.tangent_start;scalar.tangent_end=value.tangent_end;
    scalar.current=value.current;scalar.timer=value.timer;scalar.duration=value.duration;scalar.mode=value.mode;
    const auto result=scalar.sample(rate);
    value.start=scalar.start;value.tangent_end=scalar.tangent_end;value.current=scalar.current;value.timer=scalar.timer;value.duration=scalar.duration;
    return result;
}
sprite::Vec2 sample_enemy_vector_interpolation(sprite::Interpolation<sprite::Vec2>& value,const float* rate) {
    const auto initial_timer=value.timer;const auto initial_duration=value.duration;sprite::Vec2 result;
    for(unsigned axis=0;axis<2;++axis) {
        auto select=[&](sprite::Vec2& vector)->float&{return axis?vector.y:vector.x;};
        sprite::Interpolation<float> scalar{select(value.start),select(value.end),select(value.tangent_start),select(value.tangent_end),select(value.current),initial_timer,initial_duration,value.mode};
        select(result)=sample_enemy_scalar_interpolation(scalar,rate);
        select(value.start)=scalar.start;select(value.tangent_end)=scalar.tangent_end;select(value.current)=scalar.current;
        value.timer=scalar.timer;value.duration=scalar.duration;
    }
    return result;
}
}
