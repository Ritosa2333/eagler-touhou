#include "CurveLaserPath.hpp"
#include "StageMath.hpp"

namespace th13 {
namespace {
Vec3 polar(float angle,float speed){Vec3 v;polar_xy(angle,speed,v.x,v.y);return v;}
Vec3 add(Vec3 a,Vec3 b){return{float(a.x+b.x),float(a.y+b.y),float(a.z+b.z)};}
Vec3 mul(Vec3 a,float b){return{float(a.x*b),float(a.y*b),float(a.z*b)};}
Vec3 sub(Vec3 a,Vec3 b){return{float(a.x-b.x),float(a.y-b.y),float(a.z-b.z)};}
float magnitude(Vec3 a){return float(std::sqrt(double(a.x)*a.x+double(a.y)*a.y+double(a.z)*a.z));}
float direction_angle(Vec3 a){return float(std::atan2(double(a.y),double(a.x)));}
}
CurveLaserSample CurveLaserSegment::head(float time)const {
    CurveLaserSample out;const float t=time-start_time;
    out.angle=angle;out.speed=speed;
    if(mode==0)out.position=add(origin,mul(mul(direction,t),speed));
    else if(mode==1) {
        if(angle_delta>=-990.f) {
            const Vec3 velocity=add(polar(angle,speed),polar(angle_delta,speed_delta));
            out.position=add(origin,mul(velocity,t));out.speed=magnitude(velocity);out.angle=direction_angle(velocity);
        } else {
            const float total=float(double(speed)+speed+double(t)*speed_delta);
            const float steps=float(double(t)+1);
            out.position=add(origin,mul(mul(mul(direction,total),steps),.5f));
            out.speed=float(double(t)*speed_delta+speed);
        }
    } else if(mode==2) {
        Vec3 position=origin;float a=angle,s=speed;
        for(i32 n=0;n<i32(t);++n) {
            const Vec3 velocity=polar(a,s);
            a=wrap_angle(float(a+angle_delta));s+=speed_delta;position=add(position,velocity);
        }
        const Vec3 velocity=polar(a,s);const float fraction=t-float(std::floor(double(t)));
        out.position=add(position,mul(velocity,fraction));out.angle=a;out.speed=s;
    }
    return out;
}
CurveLaserSample CurveLaserSegment::tail(const CurveLaserSample& previous,float time)const {
    CurveLaserSample out;out.angle=angle;out.speed=speed;
    if(mode==0)out.position=sub(previous.position,mul(direction,speed));
    else if(mode==1) {
        if(angle_delta<-990.f) {
            const float step=previous.speed-speed_delta;
            out.position=sub(previous.position,mul(direction,step));
            out.speed=speed-speed_delta;out.angle=previous.angle;
        } else {
            const Vec3 velocity=add(polar(previous.angle,-previous.speed),polar(angle_delta,-speed_delta));
            out.position=add(previous.position,velocity);out.speed=magnitude(velocity);out.angle=direction_angle(velocity);
        }
    } else if(mode==2) {
        const float whole=float(std::floor(double(time))),fraction=time-whole;
        out.position=sub(previous.position,mul(polar(previous.angle,previous.speed),fraction));
        out.speed=previous.speed-speed_delta;out.angle=wrap_angle(float(previous.angle-angle_delta));
        out.position=sub(out.position,mul(polar(out.angle,out.speed),float((1.0-double(time))+whole)));
    }
    return out;
}
void CurveLaserPath::reset(Vec3 origin,float angle,float speed) {
    segments.clear();CurveLaserSegment s;s.origin=origin;s.angle=wrap_angle(angle);s.speed=speed;
    s.direction=polar(angle,1);segments.push_back(s);
}
void CurveLaserPath::append(i32 mode,float at,i32 duration,float speed_delta,float angle_delta) {
    auto add_segment=[&](i32 kind,float when){
        auto& previous=segments.back();previous.end_time=when;
        const auto point=previous.head(when);
        CurveLaserSegment next;next.start_time=when;next.mode=kind;next.origin=point.position;
        next.origin.z=0;next.angle=point.angle;next.speed=point.speed;next.direction=polar(next.angle,1);
        if(kind){next.speed_delta=speed_delta;next.angle_delta=angle_delta;}
        segments.push_back(next);
    };
    add_segment(mode,at);
    if(duration>=0)add_segment(0,float(at+float(duration)));
}
const CurveLaserSegment* CurveLaserPath::find(float time)const {
    for(const auto& segment:segments)if(segment.start_time<=time&&time<segment.end_time)return &segment;
    return nullptr;
}
void CurveLaserPath::sample(float time,std::vector<CurveLaserSample>& points)const {
    if(segments.empty())return;
    bool head=true;
    for(size_t i=0;i<points.size();++i) {
        const float at=time-float(i);
        if(at<0){points[i].position=segments[0].origin;points[i].delta={};points[i].angle=segments[0].angle;points[i].speed=segments[0].speed;}
        else if(const auto* segment=find(at)) {
            const Vec3 delta=points[i].delta;
            points[i]=head?segment->head(at):segment->tail(points[i-1],at);
            points[i].delta=delta;head=false;
        }
    }
}
}
