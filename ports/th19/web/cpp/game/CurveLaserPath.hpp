#pragma once
#include "BulletChanges.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
namespace th19 {
// Native LaserCurveInf: 525d50 builds timed path segments; 5274f0 samples
// their trajectories. ECL609 flag8 uses absolute start time (integer1),
// duration (integer0), speed increment and angular increment, not bullet turns.
struct CurvePoint {float x=0,y=0,angle=0,speed=0;};
struct CurveLaserPath {
 struct Segment {float start=0,end=999999;CurvePoint origin;float acceleration=0,turn=0;};
 std::vector<Segment> segments;std::vector<CurvePoint> nodes;
 int time=0,offscreen_hold=0;bool body_additive=true;
 bool dynamic=false;int homing_duration=0,bounds_duration=0;
 float homing_speed=0,homing_offset=0,homing_response=0,head_vx=0,head_vy=0;
 // Verified525d50/5256e0/527ef0 sequence used by pl17 BossCard1_at.
 // Keep historical nodes: they must not be recomputed toward today's player position.
 static CurvePoint sample(const Segment&s,float t){
  t=std::max(0.f,t-s.start);auto p=s.origin;
  if(s.acceleration==0&&s.turn==0){p.x+=std::cos(p.angle)*p.speed*t;p.y+=std::sin(p.angle)*p.speed*t;return p;}
  const int whole=int(t);
  for(int i=0;i<whole;++i){p.x+=std::cos(p.angle)*p.speed;p.y+=std::sin(p.angle)*p.speed;
   p.angle=std::remainder(p.angle+s.turn,6.283185307179586f);p.speed+=s.acceleration;}
  p.x+=std::cos(p.angle)*p.speed*(t-whole);p.y+=std::sin(p.angle)*p.speed*(t-whole);return p;
 }
 CurvePoint at(float t)const{
  if(t<0)return segments.front().origin;
  for(auto i=segments.rbegin();i!=segments.rend();++i)if(t>=i->start)return sample(*i,t);
  return segments.front().origin;
 }
 bool initialize(float x,float y,float angle,float speed,int count,const std::array<BulletChange,24>&changes){
  if(count<2||count>4096)return false;
  dynamic=false;homing_duration=bounds_duration=0;offscreen_hold=0;body_additive=true;
  for(const auto&c:changes){if(!c.flags)break;if(c.flags==0x40000002u)dynamic=true;}
  if(dynamic){
   bool found=false,bounds=false;
   for(const auto&c:changes){if(!c.flags)break;
    if(c.flags==0x40000002u&&!found&&!bounds&&c.integers[0]>0&&supportedBulletChange(c)){
     found=true;homing_duration=c.integers[0];homing_speed=c.floats[0];homing_offset=c.floats[1];homing_response=c.floats[2];
    }else if(c.flags==0x100&&found&&!bounds&&c.concurrent==0&&c.integers[1]==0&&c.integers[0]>0){
     bounds=true;bounds_duration=c.integers[0];
    }else return false; // Do not silently flatten unverified mixed curve actions.
   }
   time=0;segments={{0,999999,{x,y,angle,speed},0,0}};
   nodes.assign(count,segments.front().origin);head_vx=std::cos(angle)*speed;head_vy=std::sin(angle)*speed;return true;
  }
  segments={{0,999999,{x,y,angle,speed},0,0}};
  for(const auto&c:changes){if(!c.flags)break;
   if(c.flags==0x80){offscreen_hold=c.integers[0];continue;}
   if(c.flags==0x100000){body_additive=c.integers[0]!=0;continue;}
   if(c.flags!=8||c.integers[1]<0||c.integers[0]<0)return false;
   const float start=float(c.integers[1]),end=start+c.integers[0];
   auto origin=at(start);segments.back().end=start;
   segments.push_back({start,end,origin,c.floats[0],c.floats[1]});
   auto next=sample(segments.back(),end);segments.push_back({end,999999,next,0,0});
  }
  time=0;nodes.assign(count,segments.front().origin);return true;
 }
 // 5279d0: sampled rectangles start after 16 units; type2 uses .55
 // collision scale, other curves1.1. 549570 tests circle vs rectangle.
 bool hits(float px,float py,float radius,float width,int type,float global_angle)const{
  float distance=0;for(size_t i=0;i+1<nodes.size();++i){const auto&p=nodes[i];distance+=p.speed;
   if(distance<16||p.speed<=0)continue;
   const float c=std::cos(p.angle),s=std::sin(p.angle);
   float x=p.x+c*p.speed*.5f+std::cos(global_angle)*p.speed*.5f;
   float y=p.y+s*p.speed*.5f+std::sin(global_angle)*p.speed*.5f;
   float dx=px-x,dy=py-y,a=std::abs(dx*c+dy*s),b=std::abs(-dx*s+dy*c);
   float hx=p.speed*1.1f*.5f,hy=width*.25f*(type==2?.55f:1.1f);
   if((a<=hx+radius&&b<=hy)||(a<=hx&&b<=hy+radius))return true;
   if(a>hx&&b>hy&&(a-hx)*(a-hx)+(b-hy)*(b-hy)<radius*radius)return true;
  }return false;
 }
 bool tick(float width,float player_x=0,float player_y=400){
  if(dynamic){
   CurvePoint head=nodes.front();
   if(time<homing_duration){
    const float dx=player_x-head.x,dy=player_y-head.y;
    const float aimed=(dx==0&&dy==0?1.57079632679f:std::atan2(dy,dx))+homing_offset;
    head_vx+=(std::cos(aimed)*homing_speed-head_vx)*homing_response;
    head_vy+=(std::sin(aimed)*homing_speed-head_vy)*homing_response;
    head.angle=std::atan2(head_vy,head_vx);head.speed=std::hypot(head_vx,head_vy);
   }
   // 527ef0 copies nodes backward before integrating only node0.
   for(size_t i=nodes.size()-1;i>0;--i)nodes[i]=nodes[i-1];
   head.x+=head_vx;head.y+=head_vy;nodes[0]=head;
   bool visible=false;for(const auto&p:nodes)visible|=p.x+width>-148&&p.x-width<148&&p.y+width>0&&p.y-width<448;
   // Following nonconcurrent0x100 starts when homing finishes; countdown precedes culling.
   const bool preserved=bounds_duration>0 && time>=homing_duration && time-homing_duration+1<bounds_duration;
   ++time;return visible||time<=30||preserved;
  }
  bool visible=false;for(size_t i=0;i<nodes.size();++i){nodes[i]=at(float(time)-i);const auto&p=nodes[i];
   visible|=p.x+width>-148&&p.x-width<148&&p.y+width>0&&p.y-width<448;}
  ++time;if(offscreen_hold>0)--offscreen_hold;return visible||time<=30||offscreen_hold>0;
 }
};
}
