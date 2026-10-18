#include "fog.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::background {
namespace n=th20::recovered;namespace m=th20::source::ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float divide(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
}
void pack_fog_color(FogState& f) noexcept {
    f.color=(std::uint32_t(n::truncate32(f.blue))&255u)|((std::uint32_t(n::truncate32(f.green))&255u)<<8)|((std::uint32_t(n::truncate32(f.red))&255u)<<16)|((std::uint32_t(n::truncate32(f.alpha))&255u)<<24);
}
FogState make_fog(float near_distance,float far_distance,float blue,float green,float red,float alpha) noexcept {
    FogState f{near_distance,far_distance,blue,green,red,alpha,0};pack_fog_color(f);return f;
}
FogState scale_fog(const FogState& f,float factor) noexcept {return make_fog(n::mul32(f.near_distance,factor),n::mul32(f.far_distance,factor),n::mul32(f.blue,factor),n::mul32(f.green,factor),n::mul32(f.red,factor),n::mul32(f.alpha,factor));}
FogState subtract_fog(const FogState& a,const FogState& b) noexcept {return make_fog(sub(a.near_distance,b.near_distance),sub(a.far_distance,b.far_distance),sub(a.blue,b.blue),sub(a.green,b.green),sub(a.red,b.red),sub(a.alpha,b.alpha));}
FogState add_fog(const FogState& a,const FogState& b) noexcept {return make_fog(n::add32(a.near_distance,b.near_distance),n::add32(a.far_distance,b.far_distance),n::add32(a.blue,b.blue),n::add32(a.green,b.green),n::add32(a.red,b.red),n::add32(a.alpha,b.alpha));}
FogState sample_fog(sprite::Interpolation<FogState>& p,const float* rate){
    if(p.duration>0){n::timer_tick(p.timer,rate);if(p.timer.current>=p.duration){n::timer_set(p.timer,p.duration);p.duration=0;return p.mode==7||p.mode==17?p.start:p.end;}}
    else if(p.duration==0)return p.mode==7||p.mode==17?p.start:p.end;
    if(p.mode==7){p.start=add_fog(p.start,p.end);p.current=p.start;}
    else if(p.mode==17){p.start=add_fog(p.start,p.tangent_end);p.tangent_end=add_fog(p.tangent_end,p.end);p.current=p.start;}
    else if(p.mode==8){
        const float t=divide(p.timer.current_f,n::int_float(p.duration));
        const float b0=n::mul32(n::mul32(sub(t,1.f),sub(t,1.f)),n::add32(n::mul32(2.f,t),1.f));
        const float b1=n::mul32(n::mul32(t,t),sub(3.f,n::mul32(2.f,t)));
        const float b2=n::mul32(n::mul32(sub(1.f,t),sub(1.f,t)),t);
        const float b3=n::mul32(n::mul32(sub(t,1.f),t),t);
        p.current=add_fog(add_fog(add_fog(scale_fog(p.start,b0),scale_fog(p.end,b1)),scale_fog(p.tangent_start,b2)),scale_fog(p.tangent_end,b3));
    }else p.current=add_fog(scale_fog(subtract_fog(p.end,p.start),m::easing(p.mode,p.timer.current_f,n::int_float(p.duration))),p.start);
    return p.current;
}
}
