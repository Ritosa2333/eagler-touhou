#include "CurveCancellation.hpp"
#include <algorithm>
#include <cmath>
namespace th15 {
bool CurveCancellation::all(const CurveLaserMotion& motion,i32 color,i32 protection,bool honor,i32& state){
    if(honor&&protection)return true;for(u32 i=0;i<motion.points.size();i+=3)if(!host.curve_effect(wrapping_add(201,wrapping_mul(color,2)),motion.points[i].position,false)){error="Curve cancellation animation unavailable";return false;}state=1;return true;
}
i32 CurveCancellation::circle(CurveLaserMotion& motion,u32& flags,i32 color,i32 protection,bool honor,const Vec3& center,float radius,i32 reward){
    if(honor&&protection)return 0;std::vector<u8> mask(motion.points.size());i32 hits=0;const float square=float(radius*radius);for(u32 i=0;i<mask.size();i++){const auto& p=motion.points[i].position;const float x=float(center.x-p.x),y=float(center.y-p.y);if(float(float(x*x)+float(y*y))<=square){mask[i]=1;hits++;if(i%3==0){cancellation_reward(p,reward,random,rewards,host);if(!host.curve_effect(wrapping_add(201,wrapping_mul(color,2)),p,false)){error="Curve circle cancellation animation unavailable";return -1;}}}}
    if(!hits)return 0;if(u32(hits)>=mask.size()){flags=(flags&~4u)|2;return hits;}u32 prefix=0;while(prefix<mask.size()&&mask[prefix])prefix++;if(prefix){motion.points.erase(motion.points.begin(),motion.points.begin()+prefix);return hits;}
    u32 first=0;while(first<mask.size()&&!mask[first])first++;u32 after=first;while(after<mask.size()&&mask[after])after++;if(after>=mask.size())motion.points.resize(first);else motion.points.erase(motion.points.begin(),motion.points.begin()+after);return hits;
}
bool CurveCancellation::clip_rectangle(CurveLaserMotion& motion,u32& flags,std::vector<u8>& mask,float rate){
    u32 prefix=0;while(prefix<mask.size()&&mask[prefix])prefix++;if(prefix==mask.size()){flags=(flags&~4u)|2;return true;}
    if(prefix){mask.erase(mask.begin(),mask.begin()+prefix);auto& timer=motion.path_age;timer.rate_index=0;timer.previous=timer.current;const float delta=float(-i32(prefix)),step=rate>.99f&&rate<1.01f?delta:float(delta*rate);timer.fractional=float(step+timer.fractional);timer.current=truncate_int(timer.fractional);motion.points.resize(mask.size());if(mask.size()<4){flags=(flags&~4u)|2;return true;}}
    u32 first=0;while(first<mask.size()&&!mask[first])first++;u32 at=first;while(at<mask.size()){while(at<mask.size()&&mask[at])at++;const u32 start=at;while(at<mask.size()&&!mask[at])at++;const u32 count=at-start;if(count>3&&!host.split_curve(count,float(motion.path_age.fractional-float(start)),motion.path)){error="Curve split creation failed";return false;}}
    if(first<4)flags=(flags&~4u)|2;else motion.points.resize(first);return true;
}
bool CurveCancellation::rectangle(CurveLaserMotion& motion,u32& flags,i32 color,i32 protection,bool honor,const Vec3& center,const Vec2& size,float angle,i32 reward,float rate){
    if(honor&&protection)return true;std::vector<u8> mask(motion.points.size());bool hit=false;const float sine=float(std::sin(double(-angle))),cosine=float(std::cos(double(-angle))),width=float(size.x*.5f),height=float(size.y*.5f);
    for(u32 i=0;i<mask.size();i++){const auto& p=motion.points[i].position;float x=float(p.x-center.x),y=float(p.y-center.y);if(angle!=0){const float prior=x;x=float(float(cosine*x)-float(sine*y));y=float(float(cosine*y)+float(sine*prior));}if(std::fabs(x)<=width&&std::fabs(y)<=height){mask[i]=1;hit=true;if(i%3==0){cancellation_reward(p,reward,random,rewards,host);if(!host.curve_effect(wrapping_add(201,wrapping_mul(color,2)),p,true)){error="Curve rectangle cancellation animation unavailable";return false;}}}}
    return !hit||clip_rectangle(motion,flags,mask,rate);
}
}
