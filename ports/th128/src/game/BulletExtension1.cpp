#include "BulletExtension1.hpp"
#include <cmath>
namespace th128::bullet {
std::optional<Extension1> initializeExtension1(const ExtensionRaw& record) noexcept {
    if(record.type!=1||record.callback||record.control<0||record.control>1)return std::nullopt;
    return Extension1{true,-1,0,0};
}
std::optional<Extension1Step> stepExtension1(const Extension1& state,float speed,float angle,Position velocity) noexcept {
    if(!state.active||state.time<0||state.time>65535||state.fraction!=float(state.time)
        ||!std::isfinite(speed)||std::abs(speed)>4096||!std::isfinite(angle)||std::abs(angle)>8192
        ||!std::isfinite(velocity.x)||!std::isfinite(velocity.y)||!std::isfinite(velocity.z))return std::nullopt;
    Extension1Step result{state,velocity};
    if(state.time>16){result.next.active=false;result.finished=true;return result;}
    const float boost=float(5.-double(state.fraction)*5.*.0625);
    const float magnitude=float(double(boost)+speed);
    result.velocity.x=float(std::cos(double(angle))*magnitude);result.velocity.y=float(std::sin(double(angle))*magnitude);
    result.next.previous=state.time;++result.next.time;result.next.fraction=float(result.next.time);return result;
}
}
