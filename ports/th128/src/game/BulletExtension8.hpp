#pragma once
#include "BulletRules.hpp"
#include <cmath>
namespace th128::bullet {
struct Extension8 {bool active=false;int previous=-1,time=0,period=0;float fraction=0,acceleration=0,rotation=0;};
struct Extension8Step {Extension8 next;float speed=0,angle=0;Position velocity;bool finished=false;};
inline std::optional<Extension8> initializeExtension8(const ExtensionRaw& record) noexcept {
    if(record.type!=8||(record.control!=0&&record.control!=1)||record.parameterA< -65535||record.parameterA>65535||!std::isfinite(record.parameterR)||!std::isfinite(record.parameterS))return std::nullopt;
    return Extension8{true,-1,0,record.parameterA,0,record.parameterR,record.parameterS};
}
inline std::optional<Extension8Step> stepExtension8(const Extension8& state,float speed,float angle,Position velocity) noexcept {
    if(!state.active||state.time<0||state.time>1000000||!std::isfinite(speed)||!std::isfinite(angle)||!std::isfinite(state.acceleration)||!std::isfinite(state.rotation))return std::nullopt;
    Extension8Step result{state,speed,angle,velocity,false};
    if(state.time>=state.period){result.next.active=false;result.finished=true;return result;}
    result.angle=float(double(angle)+state.rotation);unsigned changes=0;
    while(result.angle>3.1415927410125732f&&changes<34){result.angle=float(double(result.angle)-6.2831854820251465);++changes;}
    while(result.angle< -3.1415927410125732f&&changes<34){result.angle=float(double(result.angle)+6.2831854820251465);++changes;}
    result.speed=float(double(speed)+state.acceleration);result.velocity.x=float(std::cos(double(result.angle))*result.speed);result.velocity.y=float(std::sin(double(result.angle))*result.speed);
    result.next.previous=state.time;++result.next.time;result.next.fraction=float(double(state.fraction)+1.);return result;
}
}
