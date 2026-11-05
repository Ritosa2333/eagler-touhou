#include "LaserContact.hpp"
#include "LineIntersection.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
bool LaserContact::curved(CurveLaserMotion& motion,bool graze_only,float rate,const PlayerCollision& player,PlayerDamageHost& damage,LaserContactHost& host){
    float distance=0;bool spark=false;Vec3 spark_position{};
    for(u32 i=0;i+1<motion.points.size();i++){
        const auto point=motion.points[i];const float half=float(point.speed*.5f);
        const Vec3 midpoint{float(point.position.x+float(std::cos(double(point.angle))*double(half))),float(point.position.y+float(std::sin(double(point.angle))*double(half))),point.position.z};
        distance=float(distance+point.speed);if(distance<16)continue;
        const auto result=player.laser({midpoint.x,midpoint.y},point.angle,point.speed,float(motion.width*.5f),graze_only,&damage);
        if(result==PlayerContact::hit){if(!host.cancel_hit({player.position.x,player.position.y,0}))return false;}
        else if(result==PlayerContact::graze&&!spark){
            if(graze_age.current%3==0){spark=true;spark_position=midpoint;}
            host.graze_flash();host.graze_resonance(.3f);
        }
    }
    if(spark)host.graze_spark(spark_position);graze_age.tick(&rate);return true;
}
bool LaserContact::moving(const Vec3& position,float angle,float length,float width,u32 flags,bool graze_only,float rate,const PlayerCollision& player,PlayerDamageHost& damage,LaserContactHost& host){
    if(length<=16||width<=3)return true;Vec3 origin=position;float extent=length;if(!(flags&2)){const float offset=float(length/10.f);origin={float(position.x+float(std::cos(double(angle))*double(offset))),float(position.y+float(std::sin(double(angle))*double(offset))),float(position.z+0.f)};extent=float(float(length*4.f)/5.f);}
    const float thickness=width<32?float(width*.5f):float(width-float(float(width+16.f)*.5f));return apply(origin,angle,extent,thickness,graze_only,rate,player,damage,host);
}
bool LaserContact::stationary(const Vec3& position,float angle,float length,float width,i32 state,bool graze_only,float rate,const PlayerCollision& player,PlayerDamageHost& damage,LaserContactHost& host){
    if((state!=4&&state!=2)||length<=16)return true;const float thickness=width<32?float(width*.5f):float(width-float(float(width+16.f)/3.f));return apply(position,angle,float(length*.9f),thickness,graze_only,rate,player,damage,host);
}
bool LaserContact::apply(const Vec3& origin,float angle,float length,float width,bool graze_only,float rate,const PlayerCollision& player,PlayerDamageHost& damage,LaserContactHost& host){
    const auto contact=player.laser({origin.x,origin.y},angle,length,width,graze_only,&damage);if(contact==PlayerContact::hit)return host.cancel_hit({player.position.x,player.position.y,0});if(contact!=PlayerContact::graze)return true;
    if(graze_age.current%3==0){Vec2 point{origin.x,origin.y};line_intersection(point,angle,player.position,normalize_angle(float(angle+1.5707963705062866f)),point);host.graze_spark({point.x,point.y,origin.z});}
    host.graze_flash();host.graze_resonance(.3f);graze_age.tick(&rate);return true;
}
}
