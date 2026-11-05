#include "BulletPattern.hpp"
namespace th08 {
BulletPatternResult bullet_pattern(const BulletEmission& e,i32 index,i32 layer,float aim,Rng& rng)noexcept{
    BulletPatternResult out;
    out.speed=e.layers<2?e.speed:(number(e.speed)-(number(e.speed)-number(e.ending_speed))*Extended::from_int(layer)/Extended::from_int(e.layers)).to_float();
    const auto ring=[&](){const float offset=(Extended::from_int(index)*number(6.2831854820251465f)/Extended::from_int(e.count)+number(out.angle)).to_float();return (Extended::from_int(layer)*number(e.spread)+number(e.angle)+number(offset)).to_float();};
    switch(e.pattern){
    case 0:case 1:
        out.angle=(e.count&1)?(Extended::from_int(wrapping_add(index,1)/2)*number(e.spread)).to_float():(number(e.spread)*number(.5f)+Extended::from_int(index/2)*number(e.spread)).to_float();
        out.angle=Scalar::add(out.angle,0);if(index&1)out.angle=Scalar::mul(out.angle,-1);
        if(e.pattern==0)out.angle=Scalar::add(out.angle,aim);out.angle=Scalar::add(out.angle,e.angle);break;
    case 2:out.angle=Scalar::add(aim,0);[[fallthrough]];
    case 3:out.angle=ring();break;
    case 4:out.angle=Scalar::add(aim,0);[[fallthrough]];
    case 5:out.angle=(number(3.1415927410125732f)/Extended::from_int(e.count)+number(out.angle)).to_float();out.angle=(Extended::from_int(index)*number(6.2831854820251465f)/Extended::from_int(e.count)+number(out.angle)).to_float();out.angle=Scalar::add(out.angle,e.angle);break;
    case 6:out.angle=(rng.range(Scalar::sub(e.angle,e.spread))+number(e.spread)).to_float();break;
    case 7:out.speed=(rng.range(Scalar::sub(e.speed,e.ending_speed))+number(e.ending_speed)).to_float();out.angle=ring();break;
    case 8:out.angle=(rng.range(Scalar::sub(e.angle,e.spread))+number(e.spread)).to_float();out.speed=(rng.range(Scalar::sub(e.speed,e.ending_speed))+number(e.ending_speed)).to_float();break;
    default:break;
    }
    return out;
}
}
