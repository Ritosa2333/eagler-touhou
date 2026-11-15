#include "BulletExtension16.hpp"
#include <cmath>

namespace th128::bullet {
namespace {
bool bounded(float value){return std::isfinite(value)&&std::abs(value)<=4096;}
std::optional<float> normalize(float angle){
    if(!std::isfinite(angle)||std::abs(angle)>2048)return std::nullopt;
    constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
    float value=angle;int loops=0;
    while(double(value)>pi&&loops++<=32)value=float(double(value)-tau);
    while(double(value)<-pi&&loops++<=32)value=float(double(value)+tau);
    return loops>33?std::nullopt:std::optional(value);
}
}
std::optional<Extension16> initializeExtension16(const ExtensionRaw& record,float speed,float angle) noexcept {
    const bool mode1=record.parameterC==1;
    if(record.type!=16||record.callback||(record.parameterC!=0&&record.parameterC!=1&&record.parameterC!=2)
        ||!(record.parameterD==0||(mode1&&record.parameterD==3))
        ||record.parameterA<0||record.parameterA>65535||record.parameterB<1||record.parameterB>65535
        ||!bounded(speed)||!bounded(angle)||!bounded(record.parameterR)
        ||record.parameterR<=-990||record.parameterR>=990||!std::isfinite(record.parameterS))return std::nullopt;
    Extension16 result;result.active=true;result.targetSpeed=record.parameterS> -999?record.parameterS:speed;
    if(!bounded(result.targetSpeed))return std::nullopt;
    result.angleOffset=record.parameterR;result.period=record.parameterA;result.limit=record.parameterB;
    result.mode=record.parameterC;result.angleMode=record.parameterD;
    return result;
}
std::optional<Extension16Step> stepExtension16(const Extension16& state,float speed,float angle,Position velocity,
    Position position,Position player,int sound,const Position* cachedPosition) noexcept {
    if(!state.active||(state.mode!=0&&state.mode!=1&&state.mode!=2)
        ||!(state.angleMode==0||(state.mode==1&&state.angleMode==3))
        ||state.time<0||state.time>65535||state.fraction!=float(state.time)
        ||state.period<0||state.period>65535||state.limit<1||state.count<0||state.count>=65535
        ||!bounded(state.targetSpeed)||!bounded(state.angleOffset)||!bounded(speed)||!bounded(angle))return std::nullopt;
    Extension16Step result{state,speed,angle,velocity};float currentSpeed=0;
    if(state.time>=state.period){
        if(sound>=0){result.soundRequired=true;return result;}
        if(state.mode==1){
            if(state.angleMode==3){
                if(!cachedPosition)return std::nullopt;
                const auto aim=targetAnglePrefix(*cachedPosition,player);if(!aim)return std::nullopt;
                result.angle=float(double(*aim)+double(state.angleOffset));
            }else result.angle=state.angleOffset;
            if(!std::isfinite(result.angle))return std::nullopt;
        }else if(state.mode==2){
            const auto aim=targetAnglePrefix(position,player);if(!aim)return std::nullopt;
            const auto nextAngle=normalize(float(double(*aim)+state.angleOffset));if(!nextAngle)return std::nullopt;
            result.angle=*nextAngle;
        }else{
            const float nextAngle=float(double(angle)+double(state.angleOffset));
            if(!std::isfinite(nextAngle))return std::nullopt;
            result.angle=nextAngle;
        }
        result.speed=currentSpeed=state.targetSpeed;++result.next.count;
        result.next.previous=-1;result.next.time=0;result.next.fraction=0;
        if(result.next.count>=state.limit){result.next.active=false;result.finished=true;}
    }else currentSpeed=float(double(speed)-double(speed)*state.fraction/state.period);
    result.velocity.x=float(std::cos(double(result.angle))*currentSpeed);
    result.velocity.y=float(std::sin(double(result.angle))*currentSpeed);
    if(!result.finished){result.next.previous=result.next.time;++result.next.time;result.next.fraction+=1;}
    return result;
}
}
