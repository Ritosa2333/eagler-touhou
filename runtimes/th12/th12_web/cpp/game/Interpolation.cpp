#include "Interpolation.hpp"
#include "PortTitle.hpp"
#include <cmath>
#include <type_traits>
namespace th11 {
// 0x459730, reached after an explicit float store of elapsed/duration.
float easing(float elapsed,i32 duration,InterpolationMode mode) noexcept {
    if constexpr(kPortTitle==14||kPortTitle==16){
        // Retail 4041a0 rounds every SSE operation before the next product.
        if(!duration)return 1;
        float t=elapsed/float(duration);const i32 m=i32(mode);
        const auto power=[](float x,u32 n){float result=x;for(u32 j=1;j<n;++j)result*=x;return result;};
        if(m>=1&&m<=3)return power(t,u32(m+1));
        if(m>=4&&m<=6)return 1.f-power(1.f-t,u32(m-2));
        if(m>=9&&m<=11){t*=2.f;return t<1.f?power(t,u32(m-7))*.5f:(2.f-power(2.f-t,u32(m-7)))*.5f;}
        if(m>=12&&m<=14){t*=2.f;return t<1.f?.5f-power(1.f-t,u32(m-10))*.5f:power(t-1.f,u32(m-10))*.5f+.5f;}
        if(m==15)return 0;if(m==16)return 1;
        constexpr float pi=3.1415927410125732422f,half_pi=1.5707963705062866211f;
        if(m==18||m==19){float a=(t*pi)*.5f;if(m==19)a+=half_pi;const float s=float(std::sin(double(a)));return m==18?s:1.f-s;}
        if(m==20){t*=2.f;const bool first=t<1.f;const float s=float(std::sin(double((t*pi)*.5f)));return first?s*.5f:(1.f-s)*.5f+.5f;}
        if(m==21){t*=2.f;if(t<1.f){const float s=float(std::sin(double((t*pi)*.5f+half_pi)));return (1.f-s)*.5f;}t-=1.f;return .5f+float(std::sin(double((t*pi)*.5f)))*.5f;}
        if(m>=22&&m<=31){
            // Raw retail constants 4c194c..4c19e4, including rounded ratios.
            static constexpr float p[][4]={
                {0x1p-2f,0x1.2p-1f,0x1.c71c72p-4f,0x1.c71c72p-1f},
                {0x1.333334p-2f,0x1.f5c28ep-2f,0x1.7829cep-3f,0x1.a1f58cp-1f},
                {0x1.666666p-2f,0x1.b0a3d4p-2f,0x1.28e64p-2f,0x1.6b8cep-1f},
                {0x1.851eb8p-2f,0x1.89a028p-2f,0x1.80aa7cp-2f,0x1.3faac2p-1f},
                {0x1.99999ap-2f,0x1.70a3d8p-2f,0x1.c71c72p-2f,0x1.1c71c8p-1f}};
            const bool reverse=m>=27;const auto* c=p[m-(reverse?27:22)];if(reverse)t=1.f-t;
            t-=c[0];t*=t;t/=c[1];t-=c[2];t/=c[3];return reverse?1.f-t:t;
        }
        return t;
    }
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
    if(v.duration>0){v.timer.tick();if(v.timer.current>=v.duration){v.timer.set(v.duration,rate);v.duration=0;std::memcpy(out,v.mode==InterpolationMode::Velocity||(kPortTitle==14&&v.mode==InterpolationMode::Acceleration)?v.start:v.end,N*4);return true;}}
    if(v.mode!=InterpolationMode::Velocity&&v.mode!=InterpolationMode::Acceleration)return false;
    for(u32 i=0;i<N;++i)out[i]=v.start[i]=add(v.start[i],v.mode==InterpolationMode::Velocity?v.end[i]:v.final_tangent[i]);
    if(v.mode==InterpolationMode::Acceleration)for(u32 i=0;i<N;++i)v.final_tangent[i]=add(v.final_tangent[i],v.end[i]);
    return true;
}
template<class T,u32 N> void weights(const Interpolator<T,N>& v,float* w){
    if constexpr(kPortTitle==14&&((std::is_same_v<T,float>&&(N==2||N==3))||(std::is_same_v<T,i32>&&(N==1||N==3)))){
        // 409dcc..409e52: preserve each SSE product and its association.
        const float t=v.timer.fractional/float(v.duration),m=t-1.f,r=1.f-t,twice=t*2.f;
        w[0]=(m*m)*(twice+1.f);w[1]=(3.f-twice)*(t*t);w[2]=(r*r)*t;w[3]=(m*t)*t;return;
    }
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
        if constexpr(kPortTitle==14){
            // 473df0 truncates each RGB product separately, then adds integers.
            if(v.mode==InterpolationMode::Hermite){float w[4];weights(v,w);for(u32 n=0;n<3;++n){out[n]=0;const i32 values[]={v.start[n],v.end[n],v.initial_tangent[n],v.final_tangent[n]};for(u32 j=0;j<4;++j)out[n]=wrapping_add(out[n],truncate_int(w[j]*float(values[j])));}}
            else{const float t=easing(v.timer.fractional,v.duration,v.mode);for(u32 n=0;n<3;++n)out[n]=wrapping_add(v.start[n],truncate_int(t*float(wrapping_sub(v.end[n],v.start[n]))));}
            return {out[0],out[1],out[2]};
        }
        if(v.mode==InterpolationMode::Hermite){float w[4];weights(v,w);for(u32 i=0;i<3;++i){out[i]=wrapping_add(wrapping_add(wrapping_add(truncate_int(double(w[0])*v.start[i]),truncate_int(double(w[1])*v.end[i])),truncate_int(double(w[2])*v.initial_tangent[i])),truncate_int(double(w[3])*v.final_tangent[i]));}}
        else {const float t=easing(v.timer.fractional,v.duration,v.mode);for(u32 i=0;i<3;++i)out[i]=wrapping_add(v.start[i],truncate_int(double(t)*wrapping_sub(v.end[i],v.start[i])));}
    }
    return {out[0],out[1],out[2]};
}
i32 sample(AlphaInterpolator& v,const float* rate) noexcept {
    i32 out;if(step(v,&out,rate))return out;
    if constexpr(kPortTitle==14){
        // 474190 converts once, after the rounded SSE sum (end, start, tangents).
        if(v.mode==InterpolationMode::Hermite){float w[4];weights(v,w);float result=w[1]*float(v.end[0]);result+=w[0]*float(v.start[0]);result+=w[2]*float(v.initial_tangent[0]);result+=w[3]*float(v.final_tangent[0]);return truncate_int(result);}
        const float weighted=easing(v.timer.fractional,v.duration,v.mode)*float(wrapping_sub(v.end[0],v.start[0]));return truncate_int(weighted+float(v.start[0]));
    }
    if(v.mode==InterpolationMode::Hermite){float w[4];weights(v,w);return truncate_int(double(w[1])*v.end[0]+double(w[0])*v.start[0]+double(w[2])*v.initial_tangent[0]+double(w[3])*v.final_tangent[0]);}
    return truncate_int(double(easing(v.timer.fractional,v.duration,v.mode))*wrapping_sub(v.end[0],v.start[0])+v.start[0]);
}
}
