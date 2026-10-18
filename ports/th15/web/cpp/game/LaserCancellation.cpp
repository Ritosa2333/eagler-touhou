#include "LaserCancellation.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
i32 LaserCancellation::rectangle(MovingLaserMotion& motion,u32& flags,i32 type,i32 color,i32 protection,bool honor,const Vec3& center,const Vec2& size,float angle,i32 reward,bool stationary){
    if(honor&&protection)return 0;if(motion.length<16)return 0;if(!std::isfinite(motion.length)||motion.length>4096){error="Laser cancellation exceeds original safe sample capacity";return -1;}
    const Vec3 origin=motion.position;const float dx=float(origin.x-center.x),dy=float(origin.y-center.y),s=float(std::sin(double(-angle))),c=float(std::cos(double(-angle))),bearing=normalize_angle(float(motion.angle-angle));
    const float local_dx=float(std::cos(double(bearing))*8.),local_dy=float(std::sin(double(bearing))*8.);float x=float(local_dx+float(float(c*dx)-float(dy*s))),y=float(local_dy+float(float(s*dx)+float(dy*c)));const Vec2 local_step{float(local_dx+local_dx),float(local_dy+local_dy)},half{float(size.x*.5f),float(size.y*.5f)};
    const float world_dx=float(std::cos(double(motion.angle))*8.),world_dy=float(std::sin(double(motion.angle))*8.);const Vec3 step{float(world_dx+world_dx),float(world_dy+world_dy),0};Vec3 point{float(origin.x+world_dx),float(origin.y+world_dy),0};float distance=8;std::vector<u8> mask;u32 count=0;
    for(;;){const bool hit=-half.x<=x&&x<=half.x&&-half.y<=y&&y<=half.y;mask.push_back(hit?1:0);if(hit){count++;cancellation_reward(point,reward,random,rewards,host);const i32 script=!stationary&&type==31?wrapping_add(wrapping_mul(color,2),273):effect_script(type,color);if(script>=0&&!host.effect(script,point,true)){error="Laser cancellation animation failed";return -1;}}
        x=float(x+local_step.x);y=float(y+local_step.y);point={float(point.x+step.x),float(point.y+step.y),float(point.z+step.z)};distance=float(distance+16.f);if(motion.length<float(distance+8.f))break;}
    return (stationary?clip_stationary(motion,origin,step,mask,count,true):clip(motion,flags,origin,step,mask,count))?i32(count):-1;
}
bool LaserCancellation::clip(MovingLaserMotion& motion,u32& flags,const Vec3& origin,const Vec3& step,const std::vector<u8>& mask,u32 canceled){
    const auto defer=[&](){flags=(flags&~4u)|2;};if(!canceled)return true;if(canceled>=mask.size()){defer();return true;}u32 index=0;while(index<mask.size()&&mask[index])index++;
    if(index){const float amount=float(index);motion.position={float(motion.position.x+float(amount*step.x)),float(float(amount*step.y)+motion.position.y),float(float(amount*step.z)+motion.position.z)};motion.length=float(motion.length-float(amount*16.f));if(motion.length<=24){defer();return true;}motion.maximum_length=motion.length;motion.traveled=float(amount*16.f);}
    u32 kept=0;while(index<mask.size()&&!mask[index]){index++;kept++;}if(index==mask.size())return true;
    const float old=motion.length;motion.length=float(float(kept)*16.f);motion.maximum_length=float(motion.maximum_length-float(old-motion.length));if(motion.length<24)defer();
    while(index<mask.size()){if(mask[index]){index++;continue;}const u32 start=index;while(index<mask.size()&&!mask[index])index++;const float length=float(float(index-start)*16.f);if(length>24){const float offset=float(start);const Vec3 position{float(origin.x+float(step.x*offset)),float(origin.y+float(step.y*offset)),float(origin.z+float(step.z*offset))};if(!host.segment(position,length)){error="Laser split segment creation failed";return false;}}}return true;
}
i32 LaserCancellation::circle(MovingLaserMotion& motion,u32& flags,i32 type,i32 color,i32 protection,bool honor,const Vec3& center,float radius,i32 reward,bool stationary){
    if(honor&&protection)return 0;if(stationary?motion.length<=16:motion.length<16)return 0;if(!std::isfinite(motion.length)||motion.length>4096){error="Laser cancellation exceeds original safe sample capacity";return -1;}
    const Vec3 origin=motion.position;const float dx=float(std::cos(double(motion.angle))*8.),dy=float(std::sin(double(motion.angle))*8.);const Vec3 step{float(dx+dx),float(dy+dy),0};Vec3 point{float(origin.x+dx),float(origin.y+dy),0};float distance=8;std::vector<u8> mask;u32 count=0;
    for(;;){const float x=float(center.x-point.x),y=float(center.y-point.y);const bool hit=float(float(x*x)+float(y*y))<=float(radius*radius);mask.push_back(hit?1:0);if(hit){count++;cancellation_reward(point,reward,random,rewards,host);const i32 script=effect_script(type,color);if((!stationary||(float(point.x+32.f)>-192&&float(point.x-32.f)<192&&float(point.y+32.f)>0&&float(point.y-32.f)<448))&&script>=0&&!host.effect(script,point,false)){error="Laser cancellation animation failed";return -1;}}
        point={float(point.x+step.x),float(point.y+step.y),float(point.z+step.z)};distance=float(distance+16.f);if(stationary?motion.length<=float(distance+8.f):motion.length<float(distance+8.f))break;}
    return (stationary?clip_stationary(motion,origin,step,mask,count,false):clip(motion,flags,origin,step,mask,count))?i32(count):-1;
}
bool LaserCancellation::clip_stationary(MovingLaserMotion& motion,const Vec3& origin,const Vec3& step,const std::vector<u8>& mask,u32 canceled,bool rectangle){
    if(!canceled)return true;u32 index=0;while(index<mask.size()&&mask[index])index++;
    if(index)motion.length=0;else{while(index<mask.size()&&!mask[index])index++;if(index==mask.size())return true;motion.length=float(float(index)*16.f);}
    while(index<mask.size()){if(mask[index]){index++;continue;}const u32 start=index;while(index<mask.size()&&!mask[index])index++;const float offset=float(start),length=float(float(index-start)*16.f);const Vec3 position{float(origin.x+float(step.x*offset)),float(origin.y+float(step.y*offset)),float(origin.z+float(step.z*offset))};
        if(!rectangle&&(float(position.x+32.f)<=-192||float(position.x-32.f)>=192||float(position.y+32.f)<=0||float(position.y-32.f)>=448))continue;
        if(!host.stationary_segment(position,length,float(offset*16.f),rectangle)){error="Stationary laser split child creation failed";return false;}
    }return true;
}
i32 LaserCancellation::effect_script(i32 type,i32 color)noexcept{if(type<18||type==34||type==38)return wrapping_add(wrapping_mul(color,2),201);if(type<32||type==27)return wrapping_add(wrapping_mul(color,2),249);if(type<34)return wrapping_add(wrapping_mul(color,2),273);return -1;}
i32 LaserCancellation::all(const Vec3& origin,float angle,float length,i32 type,i32 color,i32 protection,bool honor,i32 reward,i32& state,bool stationary){
    if(honor&&protection)return 0;const float dx=float(std::cos(double(angle))*8.),dy=float(std::sin(double(angle))*8.);Vec3 point{float(dx+origin.x),float(origin.y+dy),float(origin.z+0.f)};const Vec3 step{float(dx+dx),float(dy+dy),0};float distance=8;i32 count=0;
    if(length>16)for(;;){count=wrapping_add(count,1);if(!stationary||(float(point.x+16.f)>-192&&float(point.x-16.f)<192&&float(point.y+16.f)>0&&float(point.y-16.f)<448)){const i32 script=effect_script(type,color);if(script>=0&&!host.effect(script,point,false)){error="Laser cancellation animation failed";return -1;}cancellation_reward(point,reward,random,rewards,host);}distance=float(distance+16.f);point={float(point.x+step.x),float(point.y+step.y),float(point.z+step.z)};if(length<=float(distance+8.f))break;}
    state=1;return count;
}
i32 LaserCancellation::query(const Vec3& origin,float angle,float length,float width,const Vec3& center,float radius)noexcept{
    const float dx=float(center.x-origin.x),dy=float(center.y-origin.y),s=float(std::sin(double(-angle))),c=float(std::cos(double(-angle)));
    const float x=float(float(c*dx)-float(dy*s)),y=float(float(s*dx)+float(c*dy));return float(x-radius)>length||float(y-radius)>float(width*.5f)||float(x+radius)<0||float(y+radius)<float(width*-.5f)?0:2;
}
}
