#pragma once
#include "LaserWholeLineEffectsOwner.hpp"
#include <array>

namespace th128::laser::cancel {
struct CircleRemainderPlan {
    float x=0,y=0,z=0,length=0;
    std::uint32_t firstSample=0,samples=0;
};
struct CircleCancellationPlan {
    std::vector<ColoredEffectRequest> effects;
    std::uint32_t samples=0;
    bool full=false,requiresRemainder=false,remainderBelowThreshold=false;
    std::uint32_t removedPrefix=0,remainingPrefix=0;
    float stepX=0,stepY=0;
    std::optional<CircleRemainderPlan> remainder;
};
inline std::optional<CircleCancellationPlan> planCircleCancellation(const OwnedWholeLine& line,
    float centerX,float centerY,float centerZ,float radius,bool honorGuard,std::string& error){
    error.clear();
    if(line.vtable!=0x4a2c1c||line.state!=2||line.type!=20||line.color!=6||line.length<0||line.length>160
        ||!std::isfinite(line.length)||!std::isfinite(line.angle)||std::abs(line.angle)>3.1415927410125732f
        ||!std::isfinite(line.x)||!std::isfinite(line.y)||!std::isfinite(line.z)||line.z!=0||centerZ!=0
        ||!std::isfinite(centerX)||!std::isfinite(centerY)||!std::isfinite(radius)||radius<0||radius>1024
        ||std::abs(line.x)>65536||std::abs(line.y)>65536||std::abs(centerX)>65536||std::abs(centerY)>65536){
        error="laser circle cancellation outside actual type0/profile20/color6 radius domain";return std::nullopt;
    }
    CircleCancellationPlan plan;
    if(honorGuard&&line.guard5b4)return plan;
    if(line.length<16)return plan;
    const float directionX=float(std::cos(double(line.angle))*8.),directionY=float(std::sin(double(line.angle))*8.);
    const float stepX=float(double(directionX)*2.),stepY=float(double(directionY)*2.);
    plan.stepX=stepX;plan.stepY=stepY;
    const float radiusSquared=float(double(radius)*radius);
    float horizontal=float(double(line.x)+directionX),vertical=float(double(line.y)+directionY),distance=8;
    std::array<bool,10> covered{};
    do{
        if(plan.samples>=10){error="native circle sample budget";return std::nullopt;}
        const double deltaX=double(centerX)-horizontal,deltaY=double(centerY)-vertical;
        const float distanceSquared=float(deltaX*deltaX+deltaY*deltaY);
        if(radiusSquared>=distanceSquared){covered[plan.samples]=true;plan.effects.push_back({horizontal,vertical,line.z,16,23});}
        ++plan.samples;
        horizontal=float(double(horizontal)+stepX);vertical=float(double(vertical)+stepY);
        distance=float(double(distance)+16.);
    }while(double(line.length)>=double(distance)+8.);
    plan.full=plan.effects.size()==plan.samples;
    if(!plan.effects.empty()&&!plan.full){
        while(plan.removedPrefix<plan.samples&&covered[plan.removedPrefix])++plan.removedPrefix;
        if(plan.removedPrefix){
            for(std::uint32_t index=plan.removedPrefix;index<plan.samples;++index)if(covered[index])plan.requiresRemainder=true;
        }else{
            while(plan.remainingPrefix<plan.samples&&!covered[plan.remainingPrefix])++plan.remainingPrefix;
            for(std::uint32_t index=plan.remainingPrefix;index<plan.samples;++index)if(!covered[index])plan.requiresRemainder=true;
        }
    }
    if(plan.requiresRemainder&&plan.removedPrefix==0&&plan.remainingPrefix>0
        &&line.length==float(plan.samples*16u)){
        auto firstRemaining=plan.remainingPrefix;
        while(firstRemaining<plan.samples&&covered[firstRemaining])++firstRemaining;
        bool contiguous=firstRemaining<plan.samples;
        for(auto index=firstRemaining;index<plan.samples;++index)if(covered[index])contiguous=false;
        if(contiguous){
            const auto count=plan.samples-firstRemaining;
            plan.remainderBelowThreshold=count*16u<=24u;
            if(count*16u>24u)plan.remainder=CircleRemainderPlan{
                float(double(line.x)+float(double(stepX)*firstRemaining)),
                float(double(line.y)+float(double(stepY)*firstRemaining)),line.z,float(count*16u),firstRemaining,count};
        }
    }
    return plan;
}
}
