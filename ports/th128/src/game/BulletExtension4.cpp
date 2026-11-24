#include "BulletExtension4.hpp"
#include <cmath>
namespace th128::bullet {
namespace {bool bounded(float value){return std::isfinite(value)&&std::abs(value)<=4096;}}
std::optional<Extension4> initializeExtension4(const ExtensionRaw& record,float angle,Position position,Position player) noexcept {
    if(record.type!=4||record.callback||record.parameterA<0||record.parameterA>65535||!bounded(record.parameterR)||!std::isfinite(record.parameterS)||!bounded(angle))return std::nullopt;
    float direction=record.parameterS;
    if(direction<=-990)direction=angle;
    else if(direction>=990){const auto aim=targetAnglePrefix(position,player);if(!aim)return std::nullopt;direction=*aim;}
    if(!bounded(direction))return std::nullopt;
    Extension4 result;result.active=true;result.period=record.parameterA;result.magnitude=record.parameterR;result.direction=direction;
    result.vector={float(std::cos(double(direction))*record.parameterR),float(std::sin(double(direction))*record.parameterR),0};return result;
}
std::optional<Extension4Step> stepExtension4(const Extension4& state,float speed,float angle,Position velocity) noexcept {
    if(!state.active||state.time<0||state.time>65535||state.fraction!=float(state.time)||state.period<0||state.period>65535
        ||!bounded(speed)||!bounded(angle)||!bounded(state.magnitude)||!bounded(velocity.x)||!bounded(velocity.y)||!bounded(velocity.z)
        ||!bounded(state.vector.x)||!bounded(state.vector.y)||!bounded(state.vector.z))return std::nullopt;
    Extension4Step result{state,speed,angle,velocity};
    if(state.time>=state.period){result.next.active=false;result.finished=true;return result;}
    result.speed=float(double(speed)+state.magnitude);
    result.velocity={float(double(velocity.x)+state.vector.x),float(double(velocity.y)+state.vector.y),float(double(velocity.z)+state.vector.z)};
    if(std::abs(result.velocity.x)>0.00009999999747378752||std::abs(result.velocity.y)>0.00009999999747378752){
        result.angle=float(std::atan2(double(result.velocity.y),double(result.velocity.x)));
        const float squared=float(double(result.velocity.x)*result.velocity.x+double(result.velocity.y)*result.velocity.y);
        result.speed=float(std::sqrt(double(squared)));
    }
    result.next.previous=state.time;++result.next.time;result.next.fraction+=1;return result;
}
}
