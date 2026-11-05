#include "PositionInterpolation.hpp"
namespace th15 {
std::array<float,3> PositionInterpolation::step(float rate)noexcept{
    if(duration>0){timer.tick(&rate);if(timer.current>=duration){timer.set(duration);duration=0;}}
    if(duration==0)return mode==7||mode==17?start:end;
    if(!(flags&1)){Vec3Interpolation vector;vector.start=start;vector.end=end;vector.control1=control1;vector.control2=control2;vector.current=current;vector.timer=timer;vector.duration=duration;vector.mode=mode;current=vector.evaluate();start=vector.start;control2=vector.control2;}
    else for(u32 i=0;i<3;i++){ScalarInterpolation axis;axis.start={start[i]};axis.end={end[i]};axis.control1={control1[i]};axis.control2={control2[i]};axis.current={current[i]};axis.timer=timer;axis.duration=duration;axis.mode=axis_modes[i];current[i]=axis.evaluate()[0];start[i]=axis.start[0];control2[i]=axis.control2[0];}
    return current;
}
}
