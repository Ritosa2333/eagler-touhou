#include "Interpolation.hpp"
#include <type_traits>
namespace th11 {
// 0x459730, reached after an explicit float store of elapsed/duration.
float easing(float elapsed,i32 duration,InterpolationMode mode) noexcept {
    double t=float(double(elapsed)/duration);
    switch(mode){
    case InterpolationMode::Accelerate2:return float(t*t);
    case InterpolationMode::Accelerate3:return float(t*t*t);
    case InterpolationMode::Accelerate4:return float(t*t*t*t);
    case InterpolationMode::Decelerate2:t=1-t;return float(1-t*t);
    case InterpolationMode::Decelerate3:t=1-t;return float(1-t*t*t);
    case InterpolationMode::Decelerate4:t=1-t;return float(1-t*t*t*t);
    case InterpolationMode::Smooth2:t=float(t*2);if(t<1)return float(t*t*0.5);t=2-t;return float((2-t*t)*0.5);
    case InterpolationMode::Smooth3:t=float(t*2);if(t<1)return float(t*t*t*0.5);t=2-t;return float((2-t*t*t)*0.5);
    case InterpolationMode::Smooth4:t=float(t*2);if(t<1)return float(t*t*t*t*0.5);t=2-t;return float((2-t*t*t*t)*0.5);
    case InterpolationMode::FastSlow2:t=float(t*2);if(t<1){t=1-t;return float(0.5-t*t*0.5);}t-=1;return float(t*t*0.5+0.5);
    case InterpolationMode::FastSlow3:t=float(t*2);if(t<1){t=1-t;return float(0.5-t*t*t*0.5);}t-=1;return float(t*t*t*0.5+0.5);
    case InterpolationMode::FastSlow4:t=float(t*2);if(t<1){t=1-t;return float(0.5-t*t*t*t*0.5);}t-=1;return float(t*t*t*t*0.5+0.5);
    case InterpolationMode::HoldStart:return 0;
    case InterpolationMode::HoldEnd:return 1;
    default:return float(t);
    }
}
namespace {
template<class T> T add(T a,T b){if constexpr(std::is_same_v<T,i32>)return wrapping_add(a,b);else return float(double(a)+b);}
template<class T,u32 N> bool step(Interpolator<T,N>& v,T* out,const float* rate){
    if(v.duration>0){v.timer.tick();if(v.timer.current>=v.duration){v.timer.set(v.duration,rate);v.duration=0;std::memcpy(out,v.mode==InterpolationMode::Velocity?v.start:v.end,N*4);return true;}}
    if(v.mode!=InterpolationMode::Velocity&&v.mode!=InterpolationMode::Acceleration)return false;
    for(u32 i=0;i<N;++i)out[i]=v.start[i]=add(v.start[i],v.mode==InterpolationMode::Velocity?v.end[i]:v.final_tangent[i]);
    if(v.mode==InterpolationMode::Acceleration)for(u32 i=0;i<N;++i)v.final_tangent[i]=add(v.final_tangent[i],v.end[i]);
    return true;
}
template<class T,u32 N> void weights(const Interpolator<T,N>& v,float* w){
    const double t=float(double(v.timer.fractional)/v.duration),m=t-1,r=1-t;
    w[0]=float((t+t+1)*m*m);w[1]=float((3-(t+t))*t*t);w[2]=float(r*r*t);w[3]=float(m*t*t);
}
template<u32 N> void vector_sample(Interpolator<float,N>& v,float* out,const float* rate){
    if(step(v,out,rate))return;
    if(v.mode==InterpolationMode::Hermite){
        float w[4];weights(v,w);
        for(u32 i=0;i<N;++i){const float a=float(double(w[0])*v.start[i]),b=float(double(w[1])*v.end[i]),c=float(double(w[2])*v.initial_tangent[i]),d=float(double(w[3])*v.final_tangent[i]);out[i]=float(double(float(double(float(double(a)+b))+c))+d);}
    }else{
        const float t=easing(v.timer.fractional,v.duration,v.mode);
        for(u32 i=0;i<N;++i){const float difference=float(double(v.end[i])-v.start[i]);const float weighted=float(double(t)*difference);out[i]=float(double(weighted)+v.start[i]);}
    }
}
}
Vec2 sample(Vec2Interpolator& v,const float* rate) noexcept {float out[2];vector_sample(v,out,rate);return {out[0],out[1]};}
Vec3 sample(Vec3Interpolator& v,const float* rate) noexcept {float out[3];vector_sample(v,out,rate);return {out[0],out[1],out[2]};}
float sample(FloatInterpolator& v,const float* rate) noexcept {
    float out;if(step(v,&out,rate))return out;
    if(v.mode==InterpolationMode::Hermite){float w[4];weights(v,w);return float(double(w[1])*v.end[0]+double(w[0])*v.start[0]+double(w[2])*v.initial_tangent[0]+double(w[3])*v.final_tangent[0]);}
    return float((double(v.end[0])-v.start[0])*easing(v.timer.fractional,v.duration,v.mode)+v.start[0]);
}
Rgb sample(RgbInterpolator& v,const float* rate) noexcept {
    i32 out[3];if(!step(v,out,rate)){
        if(v.mode==InterpolationMode::Hermite){float w[4];weights(v,w);for(u32 i=0;i<3;++i){out[i]=wrapping_add(wrapping_add(wrapping_add(truncate_int(double(w[0])*v.start[i]),truncate_int(double(w[1])*v.end[i])),truncate_int(double(w[2])*v.initial_tangent[i])),truncate_int(double(w[3])*v.final_tangent[i]));}}
        else {const float t=easing(v.timer.fractional,v.duration,v.mode);for(u32 i=0;i<3;++i)out[i]=wrapping_add(v.start[i],truncate_int(double(t)*wrapping_sub(v.end[i],v.start[i])));}
    }
    return {out[0],out[1],out[2]};
}
i32 sample(AlphaInterpolator& v,const float* rate) noexcept {
    i32 out;if(step(v,&out,rate))return out;
    if(v.mode==InterpolationMode::Hermite){float w[4];weights(v,w);return truncate_int(double(w[1])*v.end[0]+double(w[0])*v.start[0]+double(w[2])*v.initial_tangent[0]+double(w[3])*v.final_tangent[0]);}
    return truncate_int(double(easing(v.timer.fractional,v.duration,v.mode))*wrapping_sub(v.end[0],v.start[0])+v.start[0]);
}
}
