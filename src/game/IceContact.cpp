#include "IceContact.hpp"
#include <cmath>
namespace th128::ice {
bool canFreezeBullet(std::uint32_t flags,int policy,int age,bool overridePolicy) noexcept {
    return (flags&1u)&&(overridePolicy||(policy!=1&&(policy!=2||age>=60)));
}
std::optional<bool> contacts(Position ice,float radius,int lifetime,int delay,Position point,float pointRadius) noexcept {
    for(const float value:{ice.x,ice.y,point.x,point.y,radius,pointRadius})if(!std::isfinite(value)||std::abs(value)>65536)return std::nullopt;
    if(radius<0||pointRadius<0)return std::nullopt;
    if(lifetime<=5||delay>0)return false;
    const double horizontal=double(point.x)-ice.x,vertical=double(point.y)-ice.y;
    const double combined=double(radius)+pointRadius;
    return horizontal*horizontal+vertical*vertical<=combined*combined;
}
}
