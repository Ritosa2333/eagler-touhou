#include "Interpolation.hpp"
namespace th10 {
// 0x44c350. Each multiplication keeps the original left-to-right order.
Extended easing(float elapsed, float duration, InterpolationMode mode) noexcept {
    auto t = number(elapsed) / number(duration);
    const auto one = number(1.0f), two = number(2.0f), half = number(0.5f);
    switch (mode) {
    case InterpolationMode::Accelerate2: return t*t;
    case InterpolationMode::Accelerate3: return t*t*t;
    case InterpolationMode::Accelerate4: return t*t*t*t;
    case InterpolationMode::Decelerate2: t=one-t; return one-t*t;
    case InterpolationMode::Decelerate3: t=one-t; return one-t*t*t;
    case InterpolationMode::Decelerate4: t=one-t; return one-t*t*t*t;
    case InterpolationMode::Smooth2:
        t=t+t; if(t<one)return t*t*half; t=two-t; return (two-t*t)*half;
    case InterpolationMode::Smooth3:
        t=t+t; if(t<one)return t*t*t*half; t=two-t; return (two-t*t*t)*half;
    case InterpolationMode::Smooth4:
        t=t+t; if(t<one)return t*t*t*t*half; t=two-t; return (two-t*t*t*t)*half;
    case InterpolationMode::FastSlow2:
        t=t+t; if(t<one){t=one-t;return half-t*t*half;} t=t-one; return t*t*half+half;
    case InterpolationMode::FastSlow3:
        t=t+t; if(t<one){t=one-t;return half-t*t*t*half;} t=t-one; return t*t*t*half+half;
    case InterpolationMode::FastSlow4:
        t=t+t; if(t<one){t=one-t;return half-t*t*t*t*half;} t=t-one; return t*t*t*t*half+half;
    case InterpolationMode::HoldStart: return number(0.0f);
    case InterpolationMode::HoldEnd: return one;
    default: return t;
    }
}
namespace {
template<class T,unsigned N> bool finish(Interpolator<T,N>& value, const float* default_rate) noexcept {
    if(value.duration<=0)return false;
    value.timer.tick();
    if(value.timer.current<value.duration)return false;
    if(!(value.flags&1)){value.timer.reset();value.timer.rate=default_rate;value.flags|=1;}
    value.timer.current=value.duration;
    value.timer.previous=wrapping_add(value.duration,-1);
    value.timer.fractional=Extended::from_int(value.duration).to_float();
    value.duration=0;
    return true;
}
template<unsigned N> void moving(Interpolator<float,N>& value,float* output,bool accelerated) noexcept {
    for(unsigned axis=0;axis<N;++axis){
        const float delta=accelerated?value.final_tangent[axis]:value.end[axis];
        value.start[axis]=Scalar::add(value.start[axis],delta);
        output[axis]=value.start[axis];
    }
    if(accelerated)for(unsigned axis=0;axis<N;++axis)
        value.final_tangent[axis]=Scalar::add(value.final_tangent[axis],value.end[axis]);
}
template<unsigned N> void hermite(const Interpolator<float,N>& value,float* output) noexcept {
    const auto rounded=[](Extended v){return number(v.to_float());};
    const auto t=number(value.timer.fractional)/Extended::from_int(value.duration);
    const auto one=number(1.0f),three=number(3.0f),minus_one=rounded(t-one);
    const auto first=rounded((one+(t+t))*minus_one*minus_one);
    const auto second=(three-(t+t))*t*t;
    const auto third=(one-t)*(one-t)*t;
    const auto fourth=minus_one*t*t;
    Extended a[N],b[N],c[N],d[N];
    for(unsigned axis=0;axis<N;++axis){
        a[axis]=first*number(value.start[axis]);
        b[axis]=second*number(value.end[axis]);
        c[axis]=rounded(third*number(value.initial_tangent[axis]));
        d[axis]=rounded(fourth*number(value.final_tangent[axis]));
        if(axis+1<N)b[axis]=rounded(b[axis]);
        else a[axis]=rounded(a[axis]);
    }
    // The compiler spilled the last coordinate's first sum in both versions;
    // the three-coordinate version also spilled each partial sum for x.
    if constexpr(N==2){
        output[0]=(a[0]+b[0]+c[0]+d[0]).to_float();
        output[1]=(rounded(a[1]+b[1])+c[1]+d[1]).to_float();
    }else{
        output[0]=(rounded(rounded(a[0]+b[0])+c[0])+d[0]).to_float();
        output[1]=(a[1]+b[1]+c[1]+d[1]).to_float();
        output[2]=(rounded(a[2]+b[2])+c[2]+d[2]).to_float();
    }
}
template<unsigned N> bool special(Interpolator<float,N>& value,float* output,const float* default_rate) noexcept {
    if(finish(value,default_rate)){
        const auto* result=value.mode==InterpolationMode::Velocity?value.start:value.end;
        std::memcpy(output,result,N*sizeof(float));return true;
    }
    if(value.mode==InterpolationMode::Velocity){moving(value,output,false);return true;}
    if(value.mode==InterpolationMode::Acceleration){moving(value,output,true);return true;}
    if(value.mode==InterpolationMode::Hermite){hermite(value,output);return true;}
    return false;
}
}
// 0x441ad0. The two coordinates have different spill points in the original.
Vec2 sample(Vec2Interpolator& value,const float* default_rate) noexcept {
    float out[2];
    if(!special(value,out,default_rate)){
        const auto t=easing(value.timer.fractional,Extended::from_int(value.duration).to_float(),value.mode);
        out[0]=((number(value.end[0])-number(value.start[0]))*t+number(value.start[0])).to_float();
        const auto difference=number(Scalar::sub(value.end[1],value.start[1]));
        out[1]=(number((difference*t).to_float())+number(value.start[1])).to_float();
    }
    return {out[0],out[1]};
}
// 0x404610.
Vec3 sample(Vec3Interpolator& value,const float* default_rate) noexcept {
    float out[3];
    if(!special(value,out,default_rate)){
        const auto t=easing(value.timer.fractional,Extended::from_int(value.duration).to_float(),value.mode);
        out[0]=(number(((number(value.end[0])-number(value.start[0]))*t).to_float())+number(value.start[0])).to_float();
        out[1]=((number(value.end[1])-number(value.start[1]))*t+number(value.start[1])).to_float();
        const auto difference=number(Scalar::sub(value.end[2],value.start[2]));
        out[2]=(number((difference*t).to_float())+number(value.start[2])).to_float();
    }
    return {out[0],out[1],out[2]};
}
namespace {
template<unsigned N> bool integer_motion(Interpolator<i32,N>& value,i32* output,const float* default_rate) noexcept {
    if(finish(value,default_rate)){
        std::memcpy(output,value.mode==InterpolationMode::Velocity?value.start:value.end,N*4);return true;
    }
    if(value.mode!=InterpolationMode::Velocity&&value.mode!=InterpolationMode::Acceleration)return false;
    for(unsigned axis=0;axis<N;++axis){
        value.start[axis]=wrapping_add(value.start[axis],value.mode==InterpolationMode::Velocity?value.end[axis]:value.final_tangent[axis]);
        output[axis]=value.start[axis];
    }
    if(value.mode==InterpolationMode::Acceleration)for(unsigned axis=0;axis<N;++axis)
        value.final_tangent[axis]=wrapping_add(value.final_tangent[axis],value.end[axis]);
    return true;
}
i32 difference(i32 end,i32 start){const u32 value=static_cast<u32>(end)-static_cast<u32>(start);i32 result;std::memcpy(&result,&value,4);return result;}
}
// 0x441600. Color components convert each weighted term separately.
Rgb sample(RgbInterpolator& value,const float* default_rate) noexcept {
    i32 out[3];
    if(!integer_motion(value,out,default_rate)){
        if(value.mode==InterpolationMode::Hermite){
            const auto ratio=number(value.timer.fractional)/Extended::from_int(value.duration);
            const auto t=number(ratio.to_float()),two_t=number((ratio+ratio).to_float());
            const auto one=number(1.0f),minus=number((t-one).to_float());
            const Extended weights[]={number(((two_t+one)*minus*minus).to_float()),
                number(((number(3.0f)-two_t)*t*t).to_float()),
                number(((one-t)*(one-t)*t).to_float()),number((minus*t*t).to_float())};
            const i32* terms[]={value.start,value.end,value.initial_tangent,value.final_tangent};
            for(unsigned axis=0;axis<3;++axis){
                out[axis]=0;unsigned term=0;
                for(const auto weight:weights)out[axis]=wrapping_add(out[axis],(Extended::from_int(terms[term++][axis])*weight).truncate_int());
            }
        }else{
            const auto t=easing(value.timer.fractional,Extended::from_int(value.duration).to_float(),value.mode);
            for(unsigned axis=0;axis<3;++axis)
                out[axis]=wrapping_add(value.start[axis],(Extended::from_int(difference(value.end[axis],value.start[axis]))*t).truncate_int());
        }
    }
    return {out[0],out[1],out[2]};
}
// 0x441950. Alpha converts the final sum, unlike the RGB interpolator.
i32 sample(AlphaInterpolator& value,const float* default_rate) noexcept {
    i32 result;
    if(integer_motion(value,&result,default_rate))return result;
    if(value.mode==InterpolationMode::Hermite){
        const auto t=number(value.timer.fractional)/Extended::from_int(value.duration),one=number(1.0f);
        const auto two_t=t+t,minus=t-one,inverse=one-t;
        const auto end=(number(3.0f)-two_t)*t*t*Extended::from_int(value.end[0]);
        const auto final_tangent=minus*t*t*Extended::from_int(value.final_tangent[0]);
        const auto initial_tangent=inverse*inverse*t*Extended::from_int(value.initial_tangent[0]);
        const auto start=(two_t+one)*minus*minus*Extended::from_int(value.start[0]);
        return (end+final_tangent+initial_tangent+start).truncate_int();
    }
    const auto t=easing(value.timer.fractional,Extended::from_int(value.duration).to_float(),value.mode);
    return (t*Extended::from_int(difference(value.end[0],value.start[0]))+Extended::from_int(value.start[0])).truncate_int();
}
}
