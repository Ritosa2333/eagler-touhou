#include "EnemyRandomMovement.hpp"
#include <cmath>

namespace th128::enemy {
namespace {
float normalized(float angle){
    unsigned adjustments=0;
    while(angle>Pi&&adjustments++<34)angle=float(double(angle)-Tau);
    while(angle< -Pi&&adjustments++<34)angle=float(double(angle)+Tau);
    return angle;
}
}
std::optional<float> randomMovementDirection(const Position& position,const MovementArea& area,
                                            float playerX,Random& random) noexcept {
    if(!prepareMovementArea(area.centerX,area.centerY,area.width,area.height)||!std::isfinite(position.x)
        ||!std::isfinite(position.y)||!std::isfinite(playerX)||std::abs(position.x)>65536
        ||std::abs(position.y)>65536||std::abs(playerX)>65536)return std::nullopt;
    auto candidate=random;
    float angle=0;
    const double quarterWidth=double(area.width)*.25,quarterHeight=double(area.height)*.25;
    if(double(position.x)<double(area.centerX)-quarterWidth)angle=float(double(candidate.signedAngle())/3.0);
    else if(double(position.x)>double(area.centerX)+quarterWidth)
        angle=normalized(float(double(candidate.signedAngle())/3.0+double(Pi)));
    else{
        angle=float(double(candidate.signedAngle())*.25);
        const auto choice=candidate.next16()%3u;
        if((playerX>position.x&&choice==0)||(playerX<=position.x&&choice!=0))angle=float(double(angle)+double(Pi));
    }
    if(double(position.y)<double(area.centerY)-quarterHeight)angle=std::abs(angle);
    else if(double(position.y)>double(area.centerY)+quarterHeight)angle=-std::abs(angle);
    constexpr float halfPi=1.5707963705062866f;
    if(std::abs(float(double(angle)+double(halfPi)))<=0.05000000074505806)
        angle=double(angle)<=-double(halfPi)?-1.6207963228225708f:-1.5207964181900024f;
    else if(std::abs(float(double(angle)-double(halfPi)))<=0.05)
        angle=angle>halfPi?1.6207963228225708f:1.5207964181900024f;
    random=candidate;return angle;
}
bool beginRandomMovement(Polar& polar,PolarInterpolation& interpolation,float angle,std::int32_t duration,
                         std::int32_t mode,float speed) noexcept {
    if(!std::isfinite(angle)||std::abs(angle)>8||!std::isfinite(speed)||std::abs(speed)>65536
        ||duration<=0||duration>65536||(mode!=4&&mode!=9))return false;
    interpolation={angle,speed,angle,0,duration,0,0,mode,-1};
    polar.modeFlags&=~3u;return true;
}
}
