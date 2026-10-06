// TH12 1.00b STD logical state. Original 403ec0/403020/4049a0.
// Geometry rendering never advances this state or its logical ANM hooks.
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
namespace th12::stage_logic {
using Vec3=std::array<float,3>;
inline float floatWord(uint32_t word){float value;std::memcpy(&value,&word,4);return value;}
inline float wrapAngle(float value){constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;while(value>pi)value=float(double(value)-tau);while(value<-pi)value=float(double(value)+tau);return value;}
inline float easing(float t,int mode){
  const double n=t;
  if(mode>=1&&mode<=3)return float(std::pow(n,mode+1));
  if(mode>=4&&mode<=6)return float(1-std::pow(1-n,mode-2));
  if(mode>=9&&mode<=11){const int p=mode-7;return float(n<.5?std::pow(2*n,p)*.5:1-std::pow(2-2*n,p)*.5);}
  if(mode>=12&&mode<=14){const int p=mode-10;return float(n<.5?(1-std::pow(1-2*n,p))*.5:(1+std::pow(2*n-1,p))*.5);}
  return mode==15?0:mode==16?1:t;
}
struct Camera {
  Vec3 position{0,0,1000},direction{},up{0,1,0},eyeOffset{},offset{},velocity{};
  float fov=.5235987901687622f;
  float fogStart=0,fogEnd=0;uint32_t fogArgb=0;
  std::array<float,4> fogBytes{}; // B,G,R,A native float channels before quantization.
};
struct Command {int time=0,opcode=0;uint32_t offset=0;std::vector<uint32_t> words;int I(int i)const{return int32_t(words[i]);}float F(int i)const{return floatWord(words[i]);}Vec3 V(int i)const{return {F(i),F(i+1),F(i+2)};}};
struct Primitive {int object=0,slot=0,script=0,layer=0;Vec3 position{};std::array<float,2> size{};};
struct Program {
  std::vector<Command> commands;std::unordered_map<uint32_t,size_t> offsets;std::vector<Primitive> primitives;
  bool load(const uint8_t* bytes,size_t length,std::string& error){
    auto fail=[&](const char* why){error=why;return false;};
    if(!bytes||length<144||length>32u*1024u*1024u)return fail("STD length");
    auto U16=[&](size_t p){uint16_t n;std::memcpy(&n,bytes+p,2);return n;};auto U32=[&](size_t p){uint32_t n;std::memcpy(&n,bytes+p,4);return n;};
    Program candidate;const size_t count=U16(0),start=U32(8);if(count>4096||144+count*4>length||start>length-8)return fail("STD tables");
    int slot=0;for(size_t i=0;i<count;++i){const size_t object=U32(144+i*4);if(object>length-30)return fail("STD object");size_t p=object+28;int budget=0;
      while(p+4<=length&&U16(p)!=65535){const size_t size=U16(p+2);if(size<28||size>length-p||++budget>4096)return fail("STD primitive");
        auto F=[&](size_t at){return floatWord(U32(at));};Primitive q;q.object=i;q.slot=slot++;q.script=U16(p+4);q.layer=bytes[object+2];q.position={F(p+8),F(p+12),F(p+16)};q.size={F(p+20),F(p+24)};
        for(float n:q.position)if(!std::isfinite(n))return fail("STD primitive position");for(float n:q.size)if(!std::isfinite(n))return fail("STD primitive size");candidate.primitives.push_back(q);p+=size;
      }
      if(p+4>length||U16(p)!=65535)return fail("STD primitive end");
    }
    size_t p=start;bool ended=false;for(int budget=0;p+8<=length&&budget<65536;++budget){const uint16_t size=U16(p+6);if(size==65535||int32_t(U32(p))<0){ended=true;break;}
      if(size<8||size>32767||(size-8)%4||size>length-p)return fail("STD instruction");Command c;c.time=int32_t(U32(p));c.opcode=U16(p+4);c.offset=p-start;
      for(size_t at=p+8;at<p+size;at+=4)c.words.push_back(U32(at));
      const int minimum=c.opcode==0?0:c.opcode==1||c.opcode==14?2:c.opcode==2||c.opcode==4||c.opcode==6||c.opcode==8?3:c.opcode==3||c.opcode==5||c.opcode==9||c.opcode==18?5:c.opcode==10||c.opcode==11?11:1;
      if(c.opcode<=18&&int(c.words.size())<minimum)return fail("STD operands");candidate.offsets.emplace(c.offset,candidate.commands.size());candidate.commands.push_back(std::move(c));p+=size;
    }
    if(!ended||candidate.commands.empty())return fail("STD script end");
    for(const auto& c:candidate.commands)if(c.opcode==1&&!candidate.offsets.contains(c.words[0]))return fail("STD jump");
    *this=std::move(candidate);error.clear();return true;
  }
};
struct World {
  virtual ~World()=default;virtual void unsupported(int,const char*){}
  virtual void tickObjects(){} // native404620 before STD commands.
  virtual void bindEmbedded(int /*slot*/,int /*script*/){} // op14; negative script retires.
  virtual void tickEmbedded(int /*slot*/){} // eight Stage+1cc VMs after STD commands.
  virtual void backgroundEffect(int /*kind*/){unsupported(17,"background effect mesh not connected");}
  virtual void publish(const Camera&){}
};
struct Interpolation {
  std::array<float,6> from{},to{},a{},b{};int duration=0,mode=0,age=0;float time=0;
  void begin(const float* current,const float* target,int size,int ticks,int curve,const float* tangentA=nullptr,const float* tangentB=nullptr){duration=ticks;mode=curve;age=0;time=0;for(int i=0;i<size;++i){from[i]=current[i];to[i]=target[i];a[i]=tangentA?tangentA[i]:0;b[i]=tangentB?tangentB[i]:0;}}
  std::array<float,6> tick(int size,float delta){
    if(duration>0){time=float(double(time)+delta);age=int(time);if(age>=duration){age=duration;time=duration;duration=0;return mode==7?from:to;}}
    if(mode==7){for(int i=0;i<size;++i)from[i]=float(double(from[i])+to[i]);return from;}
    if(mode==17){for(int i=0;i<size;++i){from[i]=float(double(from[i])+b[i]);b[i]=float(double(b[i])+to[i]);}return from;}
    const float fraction=duration?float(double(time)/duration):0;std::array<float,6> out{};
    if(mode==8){
      const double t=fraction;
      const float h0=float((2*t+1)*(t-1)*(t-1)),h1=float((3-2*t)*t*t),h2=float(t*(t-1)*(t-1)),h3=float(t*t*(t-1));
      for(int i=0;i<size;++i){const float x=float(double(from[i])*h0),y=float(double(to[i])*h1),z=float(double(a[i])*h2),w=float(double(b[i])*h3);out[i]=float(double(float(double(float(double(x)+y))+z))+w);}return out;
    }
    const float n=easing(fraction,mode);for(int i=0;i<size;++i){const float diff=float(double(to[i])-from[i]),product=float(double(diff)*n);out[i]=float(double(from[i])+product);}return out;
  }
};
struct State {
  const Program* program=nullptr;size_t pc=0;int clock=0,previousClock=-1,callbackCount=0,transitionAge=0,shakeMode=0,shakeAge=0,renderHint=0,backgroundMode=0;float clockTime=0,shakeTime=0;
  uint32_t flags=1;Camera camera{};Interpolation positionTrack{},directionTrack{},upTrack{},fogTrack{};
  void reset(const Program& p,const Camera& initial={}){*this={};program=&p;camera=initial;}
  bool visibleLayer(int layer)const{return !(flags&8)&&((flags&1)||layer<8||layer>11);}
  bool seekLabel(int label){if(!program)return false;for(size_t i=0;i+1<program->commands.size();++i)if(program->commands[i].opcode==16&&program->commands[i].I(0)==label){pc=i+1;clock=program->commands[pc].time;previousClock=clock-1;clockTime=clock;return true;}return false;}
  void normalizeEye(){const double n=std::sqrt(double(camera.direction[0])*camera.direction[0]+double(camera.direction[1])*camera.direction[1]+double(camera.direction[2])*camera.direction[2]);for(int i=0;i<3;++i)camera.eyeOffset[i]=n?float(double(camera.direction[i])/n):0;}
  void updateShake(float delta){
    if(shakeMode==0||shakeMode==3||shakeMode==4||shakeMode>7)return;
    auto normalized=[](double n){return wrapAngle(float(n));};constexpr double pi=3.1415927410125732;
    float value;
    if(shakeMode==1){const float a=normalized(shakeTime*pi*2*.001953125),s=float(std::sin(double(a)));value=float(double(s)-1);camera.offset[0]=float(double(value)*-50);camera.up[0]=float(double(value)*-.10000000149011612);}
    else if(shakeMode==2){const float a=normalized(shakeTime*pi*2*.00048828125),s=float(std::cos(double(a)));camera.offset[0]=float(double(s)*5);const float b=normalized(float(double(a)*2));camera.offset[2]=float(std::cos(double(b))*5);camera.up[0]=float(-double(s)*.05000000074505806);}
    else {const double rate=shakeMode==5?.00048828125:shakeMode==6?.0009765625:.001953125;const float a=float(pi-shakeTime*pi*2*rate),s=float(std::cos(double(a)));camera.offset[0]=float(double(s)*(shakeMode==5?70:shakeMode==6?-50:-15));camera.up[0]=float(-double(s)*(shakeMode==7?.009999999776482582:.10000000149011612));if(shakeMode==5)camera.offset[2]=float(std::cos(double(normalized(float(double(s)*2))))*200);}
    shakeTime=float(double(shakeTime)+delta);shakeAge=int(shakeTime);const int period=shakeMode==2||shakeMode==5?2048:shakeMode==6?1024:512;if(shakeAge>=period){shakeAge=0;shakeTime=0;}
  }
  bool tick(World& world,float delta=1){
    if(!program||!std::isfinite(delta)||delta<0)return false;if((flags&8)||((flags&4)&&transitionAge>=60))return true;
    camera.velocity={};normalizeEye();
    if(!((flags&4)&&transitionAge>=30)){
      world.tickObjects();bool stopped=false;int budget=0;
      while(pc<program->commands.size()&&program->commands[pc].time<=clock){if(++budget>4096){world.unsupported(-1,"STD instruction budget");return false;}const auto& c=program->commands[pc];++pc;
        switch(c.opcode){
          case 0:--pc;stopped=true;break;
          case 1:pc=program->offsets.at(c.words[0]);clock=c.I(1);previousClock=clock-1;clockTime=clock;continue;
          case 2:{const auto old=camera.position;camera.position=c.V(0);for(int i=0;i<3;++i)camera.velocity[i]=float(double(camera.position[i])-old[i]);break;}
          case 3:case 5:case 18:{auto& field=c.opcode==3?camera.position:c.opcode==5?camera.direction:camera.up;auto& track=c.opcode==3?positionTrack:c.opcode==5?directionTrack:upTrack;const auto end=c.V(2);track.begin(field.data(),end.data(),3,c.I(0),c.I(1));break;}
          case 4:camera.direction=c.V(0);break;case 6:camera.up=c.V(0);break;case 7:camera.fov=c.F(0);break;
          case 8:{camera.fogArgb=c.words[0];for(int i=0;i<4;++i)camera.fogBytes[i]=(c.words[0]>>(8*i))&255;camera.fogStart=c.F(1);camera.fogEnd=c.F(2);break;}
          case 9:{std::array<float,6> current{camera.fogStart,camera.fogEnd,camera.fogBytes[0],camera.fogBytes[1],camera.fogBytes[2],camera.fogBytes[3]},end{c.F(3),c.F(4),float(c.words[2]&255),float((c.words[2]>>8)&255),float((c.words[2]>>16)&255),float((c.words[2]>>24)&255)};fogTrack.begin(current.data(),end.data(),6,c.I(0),c.I(1));break;}
          case 10:case 11:{auto& field=c.opcode==10?camera.position:camera.direction;auto& track=c.opcode==10?positionTrack:directionTrack;const auto end=c.V(5),a=c.V(2),b=c.V(8);track.begin(field.data(),end.data(),3,c.I(0),8,a.data(),b.data());break;}
          case 12:shakeMode=c.I(0)&255;shakeAge=0;shakeTime=0;if(!shakeMode)camera.offset={};break;
          case 13:renderHint=c.I(0);break;case 14:world.bindEmbedded(c.I(0),c.I(1));break;case 15:case 16:break;
          case 17:backgroundMode=c.I(0);world.backgroundEffect(backgroundMode);break;
          default:world.unsupported(c.opcode,"STD command not restored");break;
        }
        if(stopped)break;
      }
      if(!stopped){previousClock=clock;clockTime=float(double(clockTime)+delta);clock=int(clockTime);}
      auto vectorTrack=[&](Interpolation& t,Vec3& field){if(t.duration){const auto v=t.tick(3,delta);std::copy_n(v.begin(),3,field.begin());}};
      vectorTrack(directionTrack,camera.direction);vectorTrack(positionTrack,camera.position);
      if(fogTrack.duration){const auto v=fogTrack.tick(6,delta);camera.fogStart=v[0];camera.fogEnd=v[1];camera.fogArgb=0;for(int i=0;i<4;++i){camera.fogBytes[i]=v[i+2];camera.fogArgb|=uint32_t(int(v[i+2])&255)<<(8*i);}}
      vectorTrack(upTrack,camera.up);updateShake(delta);for(int i=0;i<8;++i)world.tickEmbedded(i);
    }
    world.publish(camera);++callbackCount;return true;
  }
};
} // namespace th12::stage_logic
