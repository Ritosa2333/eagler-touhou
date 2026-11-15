#pragma once
#include <cmath>
namespace th19 {
struct EnemyFacing {float angle,scale_x;};
// Enemy animation update 0x5089d0, ANM424 modes 0..5. Mode2 folds
// the angle into [-pi/2,pi/2] and mirrors X so animals stay upright.
inline EnemyFacing nativeEnemyFacing(int mode,float angle,float scale,float heading){
 constexpr float pi=3.14159274101257324219f;
 auto normalize=[&](float a){for(int i=0;i<34&&a>pi;++i)a-=2*pi;for(int i=0;i<34&&a<-pi;++i)a+=2*pi;return a;};
 switch(mode){
 case 1:angle=heading;break;
 case 2:{angle=normalize(heading);bool left=angle<-pi/2||angle>pi/2;
   scale=left?-std::abs(scale):std::abs(scale);if(left)angle=normalize(angle+pi);break;}
 case 3:angle=normalize(heading+pi);break;
 case 4:angle=normalize(heading+pi/2);break;
 case 5:angle=normalize(heading-pi/2);break;
 }
 return {angle,scale};
}
}
