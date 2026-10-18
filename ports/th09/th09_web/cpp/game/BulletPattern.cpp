#include "BulletEmission.hpp"
namespace th09 {
BulletPatternResult bullet_pattern(const BulletEmission& e,i32 index,i32 layer,float aim,Rng& rng) noexcept {
    BulletPatternResult result;
    result.speed=e.layers<2?e.speed:e.speed-((e.speed-e.ending_speed)*float(layer))/float(e.layers);
    switch(e.pattern){
    case 0:case 1:{
        float a=(e.count&1)?float(wrapping_add(index,1)/2):float(index/2)+.5f;
        a*=e.spread;if(index&1)a*=-1.f;if(e.pattern==0)a+=aim;result.angle=a+e.angle;break;
    }
    case 2:case 3:{
        float a=(float(index)*6.283185482025147f)/float(e.count);
        a+=e.pattern==2?aim:0.f;result.angle=(float(layer)*e.spread+a)+e.angle;break;
    }
    case 4:case 5:{
        float a=3.1415927410125732f/float(e.count);a+=e.pattern==4?aim:0.f;
        result.angle=((float(index)*6.283185482025147f)/float(e.count)+a)+e.angle;break;
    }
    case 6:result.angle=float(rng.range(e.angle-e.spread))+e.spread;break;
    case 7:result.speed=float(rng.range(e.speed-e.ending_speed))+e.ending_speed;
        result.angle=(float(layer)*e.spread+(float(index)*6.283185482025147f)/float(e.count))+e.angle;break;
    case 8:result.angle=float(rng.range(e.angle-e.spread))+e.spread;result.speed=float(rng.range(e.speed-e.ending_speed))+e.ending_speed;break;
    }
    return result;
}
}
