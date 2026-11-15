#pragma once
#include "Types.hpp"
#include "Interpolation.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
namespace th19 {
// TH10+ layout from thtk/thstd/thstd.h. Instance z is a coordinate, NOT speed.
struct StdScene {
 struct Quad {u16 script,order;float x,y,z,w,h;u32 animation=0;};
 struct AnimationEvent {int slot,script,layer;};
 std::vector<AnimationEvent> animationEvents;
 std::vector<int> interrupts;
 struct Ins {u32 time,offset;u16 op;std::vector<u32> args;};
 std::vector<Quad> quads;std::vector<Ins> code;
 Vec3 eye{},direction{0,0,1},up{0,1,0};float fov=.448799f,fogNear=600,fogFar=1200;u32 fog=0xff70b880,clearColor=0;
 struct Motion {Vec3 from,to;u32 begin=0,duration=0,mode=0;bool active=false;Vec3 tangentFrom{},tangentTo{};};
 Motion eyeMotion,dirMotion,upMotion;u32 cursor=0,clock=0;int time=-1;
 struct FogMotion {u32 from=0,to=0,begin=0,duration=0,mode=0;float nearFrom=0,nearTo=0,farFrom=0,farTo=0;bool active=false;} fogMotion;
 int cameraEffect=0,effectFrame=0,distortion=0;u32 distortionStart=0;
 Vec3 eyeOffset{},directionOffset{};
 static float f(u32 n){float v;std::memcpy(&v,&n,4);return v;}
 static u32 u(const u8*p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
 static u16 h(const u8*p){return u16(p[0])|u16(p[1])<<8;}
 bool load(const std::vector<u8>&b){
  *this=StdScene{};if(b.size()<144)return false;
  u32 n=h(b.data()),fo=u(b.data()+4),so=u(b.data()+8);
  if(144+n*4>b.size()||fo>so||so>b.size())return false;
  std::vector<std::vector<Quad>> objects(n);u32 animation=0;
  for(u32 i=0;i<n;++i){u32 o=u(b.data()+144+i*4);if(o+28>b.size())return false;u16 order=b[o+2];o+=28;
   while(o+4<=fo&&h(b.data()+o)!=65535){if(o+28>fo||h(b.data()+o+2)!=28)return false;
    objects[i].push_back({h(b.data()+o+4),order,f(u(b.data()+o+8)),f(u(b.data()+o+12)),f(u(b.data()+o+16)),f(u(b.data()+o+20)),f(u(b.data()+o+24)),animation++});o+=28;}}
  for(u32 o=fo;o+16<=so;o+=16){u16 id=h(b.data()+o);if(id==65535)break;if(id>=n)return false;
   for(auto q:objects[id]){q.x+=f(u(b.data()+o+4));q.y+=f(u(b.data()+o+8));q.z+=f(u(b.data()+o+12));quads.push_back(q);}}
  for(u32 o=so;o+8<=b.size();){u32 t=u(b.data()+o);if(t==0xffffffff)break;u16 op=h(b.data()+o+4),len=h(b.data()+o+6);if(len<8||o+len>b.size()||(len-8)%4)return false;
   Ins ins{t,o-so,op,{}};for(u32 p=o+8;p<o+len;p+=4)ins.args.push_back(u(b.data()+p));code.push_back(ins);o+=len;}
  return !quads.empty();
 }
 static void move(Vec3&v,Motion&m,u32 t){if(!m.active)return;float a=std::min(1.f,float(t-m.begin)/std::max(1u,m.duration));
  if(m.mode==8){float s=1-a,p=s*s*(2*a+1),q=a*a*(3-2*a),r=s*s*a,t=(a-1)*a*a;
   v={p*m.from.x+q*m.to.x+r*m.tangentFrom.x+t*m.tangentTo.x,p*m.from.y+q*m.to.y+r*m.tangentFrom.y+t*m.tangentTo.y,p*m.from.z+q*m.to.z+r*m.tangentFrom.z+t*m.tangentTo.z};if(a>=1)m.active=false;return;
  }
  a=nativeEase(m.mode,a);
  v={m.from.x+(m.to.x-m.from.x)*a,m.from.y+(m.to.y-m.from.y)*a,m.from.z+(m.to.z-m.from.z)*a};if(a>=1)m.active=false;}
 void tick(){++time;++clock;animationEvents.clear();interrupts.clear();move(eye,eyeMotion,clock);move(direction,dirMotion,clock);move(up,upMotion,clock);
  if(fogMotion.active){auto&m=fogMotion;float a=nativeEase(m.mode,float(clock-m.begin)/std::max(1u,m.duration));fog=0;
   for(int i=0;i<4;++i){float x=(m.from>>(i*8))&255,y=(m.to>>(i*8))&255;fog|=u32(std::clamp(x+(y-x)*a,0.f,255.f))<<(i*8);}
   fogNear=m.nearFrom+(m.nearTo-m.nearFrom)*a;fogFar=m.farFrom+(m.farTo-m.farFrom)*a;if(clock-m.begin>=m.duration)m.active=false;
  }
  for(int budget=0;cursor<code.size()&&code[cursor].time<=u32(time)&&budget<100;++budget){const auto&i=code[cursor++];auto&a=i.args;
   auto vec=[&](int off){return Vec3{f(a[off]),f(a[off+1]),f(a[off+2])};};
   if(i.op==2&&a.size()>=3){eye=vec(0);eyeMotion.active=false;}
   else if(i.op==4&&a.size()>=3){direction=vec(0);dirMotion.active=false;}
   else if((i.op==3||i.op==5||i.op==18)&&a.size()>=5){auto&m=i.op==3?eyeMotion:(i.op==5?dirMotion:upMotion);m={i.op==3?eye:(i.op==5?direction:up),vec(2),clock,a[0],a[1],true};}
   else if((i.op==10||i.op==11)&&a.size()>=11){auto&m=i.op==10?eyeMotion:dirMotion;m={i.op==10?eye:direction,vec(5),clock,a[0],8,true,vec(2),vec(8)};}
   else if(i.op==6&&a.size()>=3)up=vec(0);
   else if(i.op==7&&a.size())fov=f(a[0]);
   else if(i.op==8&&a.size()>=3){fog=a[0];fogNear=f(a[1]);fogFar=f(a[2]);fogMotion.active=false;}
   else if(i.op==9&&a.size()>=5)fogMotion={fog,a[2],clock,a[0],a[1],fogNear,f(a[3]),fogFar,f(a[4]),true};
   else if(i.op==12&&!a.empty()){cameraEffect=i32(a[0]);effectFrame=cameraEffect==2?512:0;}
   else if(i.op==13&&!a.empty())clearColor=a[0];
   else if(i.op==14&&a.size()>=3)animationEvents.push_back({i32(a[0]),i32(a[1]),i32(a[2])});
   else if(i.op==17&&!a.empty()){distortion=i32(a[0]);distortionStart=clock;}
   else if(i.op==19&&!a.empty())interrupts.push_back(i32(a[0])+7);
   else if(i.op==0){--cursor;break;}
   else if(i.op==1&&a.size()>=2){time=i32(a[1]);for(u32 j=0;j<code.size();++j)if(code[j].offset==a[0]){cursor=j;break;}}
  }
  eyeOffset=directionOffset={};
  float angle=effectFrame*6.28318530718f/1024.f;
  if(cameraEffect==1){float a=std::min(1.f,effectFrame/512.f);eyeOffset={-10*std::sin(angle)*a,0,-10*std::sin(angle*2)*a};directionOffset={-eyeOffset.x/2,0,-eyeOffset.z/2};up.x=-std::sin(angle)*.01f*a;}
  else if(cameraEffect==3){float x=std::sin(angle)*80*std::min(1.f,effectFrame/1024.f);eyeOffset.x=x;directionOffset.x=-x/10;}
  else if(cameraEffect==2||cameraEffect==4){float a=effectFrame*6.28318530718f/(512*8.f);up.x=-std::sin(a);up.z=(cameraEffect==2?1:-1)*std::cos(a);}
  if(cameraEffect)++effectFrame;
 }
};
}
