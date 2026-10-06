// TH12 1.00b hostile bullets. Retail 40aa60 dispatch, 4099e0 lifecycle,
// 40b6f0/40bef0 motion, 437810/437980 collision. No translated x86 executes here.
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
namespace th12::eb {
constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;
struct Record {float a=0,b=0;int32_t c=0,d=0;uint32_t kind=0;bool concurrent=false;};
struct Program {std::array<Record,18> records{};int first=0;uint32_t flags=0;int transformSound=-1;};
inline bool set(Program& p,int slot,bool concurrent,uint32_t kind,int c,int d,float a,float b){
  if(slot<0||slot>=18||!std::isfinite(a)||!std::isfinite(b))return false;
  p.records[slot]={a,b,c,d,kind,concurrent};return true;
}
enum class Phase:uint32_t {Inactive=0,Alive=1,Spawning=2,Cancelling=3};
enum class Collision:uint32_t {None=0,Hit=1,Graze=2};
enum class AnimationEvent:uint32_t {Interrupt=0,BirthTick=1,BulletTick=2,Retire=3,CancelEffect=4};
struct Appearance {int script;float hitbox;int group,effect,width,height;};
// 31 retail records at 4af280, stride d0: script +0, hitbox +c4,
// draw group +c8, cancel effect class +cc. Sprite sizes are retail ANM body sizes.
inline constexpr std::array<Appearance,31> appearances={{
 {35,4,5,0,8,8},{36,4,5,0,8,8},{37,4,5,0,8,8},
 {38,6,3,0,16,16},{39,6,3,0,16,16},{40,6,3,0,16,16},{41,6,3,0,16,16},
 {42,4,4,0,16,14},{43,4,4,0,16,14},{44,4,4,0,16,14},{45,4,3,0,14,16},
 {46,4,4,0,14,16},{47,4,4,0,14,14},{48,0,4,0,14,14},{49,4,4,0,14,16},
 {50,6,3,0,14,16},{51,6,3,0,16,16},{69,10,1,1,32,32},{70,10,1,1,32,32},
 {71,8,2,1,30,30},{72,8,2,1,30,30},{73,8,2,1,30,30},{74,8,2,1,30,30},
 {98,6,2,3,30,30},{99,6,2,4,30,30},{161,10,3,1,32,32},
 {75,14,0,2,64,64},{163,14,0,2,64,64},{215,4,5,0,16,16},
 {216,6,2,5,30,30},{233,6,2,5,16,14}
}};
inline constexpr float playerHitboxes[3]={2.f,3.5f,3.f}; // 4b31a8; native replaces SHT hitbox.
struct Motion {float a=0,b=0,vx=0,vy=0;int timer=0,duration=0,limit=0,count=0;};
struct State {
  uint32_t id=0;float x=0,y=0,vx=0,vy=0,angle=0,speed=0;
  int type=0,color=0,age=0,collisionAge=0,visualAge=0,animation=35,interrupt=2;
  Phase phase=Phase::Inactive;bool grazed=false,rotateToMotion=false;
  int spawnDuration=0,cancelAge=0,collisionDelay=0,offscreenGrace=10,cancelEffect=4;
  uint32_t active=0;int cursor=0;Program program{};
  Motion fast{},acceleration{},polar{},turn{},bounce{},wait{},protect{};
};
struct Emission {
  float x=0,y=0,angle=0,spacing=0,speed=0,slow=0;
  int type=0,color=0,count=1,layers=1,mode=0;Program program{};
};
struct World {
  float playerX=0,playerY=0;
  virtual ~World()=default;
  virtual void emit(const Emission&)=0;
  virtual void unsupported(uint32_t)=0;
  virtual void sound(int,float){}
  // Native 454ee0 binds synchronously, before extension initialization. The
  // caller owns the ANM registry/RNG; this module never executes its bytecode.
  virtual void appearance(State&,bool /*initial*/){}
  // A tick returns true when original 455630 would retire this embedded VM.
  virtual bool stateAnimation(State&,AnimationEvent,int /*argument*/=0){return false;}
  virtual bool ownsStateAnimation(const State&)const{return false;}
  virtual int animationInteger(const State&,int /*index*/,int fallback){return fallback;}
};
inline float normalize(float a){while(a>pi)a-=tau;while(a<-pi)a+=tau;return a;}
inline float aim(const State& s,const World& w){const float x=float(double(w.playerX)-s.x),y=float(double(w.playerY)-s.y);return x==0&&y==0?pi/2:float(std::atan2(double(y),double(x)));}
inline void velocity(State& s,float speed){s.vx=float(std::cos(double(s.angle))*speed);s.vy=float(std::sin(double(s.angle))*speed);}
inline float resolvedAngle(const State& s,const World& w,float value){return value<=-990?s.angle:value>=990?aim(s,w):value;}
inline void appearance(State& s,int type,int color){
  s.type=std::clamp(type,0,30);s.color=std::clamp(color,0,15);s.animation=appearances[s.type].script;
  static constexpr int effects[8]={4,8,12,16,20,24,28,34};
  switch(appearances[s.type].effect){case 0:s.cancelEffect=s.color*2+4;break;case 1:s.cancelEffect=effects[s.color%8];break;case 2:s.cancelEffect=-1;break;case 3:s.cancelEffect=16;break;case 4:s.cancelEffect=6;break;case 5:s.cancelEffect=12;break;}
}
inline void cancel(State& s,World* w=nullptr){
  if(s.phase!=Phase::Alive&&s.phase!=Phase::Spawning)return;
  s.phase=Phase::Cancelling;s.interrupt=1;s.visualAge=0;s.cancelAge=0;
  // Retail 40c8b0 retires offscreen cancels without an effect.
  if(s.x+8<=-192||s.x-8>=192||s.y+8<=0||s.y-8>=448)s.phase=Phase::Inactive;
  if(w){if(s.phase==Phase::Inactive)w->stateAnimation(s,AnimationEvent::Retire);else{w->stateAnimation(s,AnimationEvent::Interrupt,1);if(s.cancelEffect>=0)w->stateAnimation(s,AnimationEvent::CancelEffect,s.cancelEffect);}}
}
inline void cancel(State& s,World& w){cancel(s,&w);}
inline void finishAnimation(State& s,World& w,AnimationEvent event=AnimationEvent::BulletTick){
  if(w.stateAnimation(s,event)){s.phase=Phase::Inactive;w.stateAnimation(s,AnimationEvent::Retire);}
}
inline void start(State& s,World& w){
  for(int budget=0;s.cursor>=0&&s.cursor<18&&budget<4096;++budget){
    const auto t=s.program.records[s.cursor];const auto kind=t.kind;
    if(!kind||(!t.concurrent&&s.active)||(kind&s.active))return;
    if(kind==0x80000000){++s.cursor;continue;}
    switch(kind){
    case 1:s.active|=kind;s.fast={};break;
    case 2:break; // Spawn-only command, consumed by initialize at the selected start.
    case 4:{s.active|=kind;auto& m=s.acceleration;m={};m.a=t.a;m.b=resolvedAngle(s,w,t.b);m.duration=t.c;m.vx=float(std::cos(double(m.b))*m.a);m.vy=float(std::sin(double(m.b))*m.a);break;}
    case 8:{s.active|=kind;s.polar={};s.polar.a=t.a;s.polar.b=t.b;s.polar.duration=t.c;break;}
    case 0x10:case 0x20:case 0x40:{s.active|=kind;s.turn={};s.turn.b=resolvedAngle(s,w,t.a);s.turn.a=t.b<=-999?s.speed:t.b;s.turn.duration=t.c;s.turn.limit=t.d;break;}
    case 0x100:s.active|=kind;s.bounce={};s.bounce.a=t.a;s.bounce.limit=t.c;s.bounce.count=t.d;break;
    case 0x200:s.collisionDelay=t.c;break;
    case 0x400:s.active|=kind;s.protect={};s.protect.timer=t.c;s.protect.count=t.d;if(t.d)w.unsupported(kind);break;
    case 0x800:case 0x1000000:appearance(s,t.c,t.d);s.interrupt=2;s.visualAge=0;w.appearance(s,false);w.stateAnimation(s,AnimationEvent::Interrupt,2);break;
    case 0x1000:s.active|=kind;s.wait={};s.wait.timer=t.c;break;
    case 0x2000:cancel(s,w);break;
    case 0x4000:w.sound(t.c,s.x);break;
    case 0x80000:{
      if(s.cursor>=17){w.unsupported(kind);s.cursor=18;return;}
      const auto next=s.program.records[s.cursor+1];const uint32_t packed=uint32_t(t.c);
      Emission e;e.x=s.x;e.y=s.y;e.speed=t.a;e.slow=t.b;e.type=(packed>>16)&255;e.color=int8_t((packed>>8)&255);e.mode=(packed>>24)&127;
      e.count=int16_t(t.d);e.layers=int16_t(next.c);e.angle=next.a;e.spacing=next.b;e.program=s.program;e.program.flags=uint32_t(next.d);e.program.first=packed&255;e.program.transformSound=-1;
      s.cursor+=2;w.emit(e);if(packed&0x80000000){cancel(s,w);++s.cursor;}continue;
    }
    case 0x100000:break; // Paired payload consumed by 80000, harmless if reached alone.
    case 0x400000:s.cursor=t.c;continue;
    case 0x4000000:
      if(t.a>=990)s.angle=normalize(float(double(aim(s,w))+float(double(t.a)-999)));else if(t.a>=-990)s.angle=t.a;
      if(t.b>=-990)s.speed=t.b;velocity(s,s.speed);break;
    case 0x10000000:s.rotateToMotion=t.c!=0;break;
    default:w.unsupported(kind);break;
    }
    if(s.cursor&&s.program.transformSound>=0&&(kind==4||kind==8))w.sound(s.program.transformSound,s.x);
    ++s.cursor;
  }
}
inline void initialize(State& s,uint32_t id,const Program& p,int type,int color,float x,float y,float angle,float speed,World& w){
  s={};s.id=id;s.x=x;s.y=y;s.angle=angle;s.speed=speed;s.program=p;s.cursor=std::clamp(p.first,0,18);s.phase=Phase::Alive;
  appearance(s,type,color);velocity(s,speed);
  w.appearance(s,true);
  if(s.cursor<18&&p.records[s.cursor].kind==2){
    const int variant=p.records[s.cursor].c;s.interrupt=variant+7;s.phase=Phase::Spawning;
    s.spawnDuration=variant==0?8:variant==1?15:25;
    if(s.type==30){s.spawnDuration=0;s.phase=Phase::Alive;}
    s.x=float(double(s.x)-float(double(s.vx)*4));s.y=float(double(s.y)-float(double(s.vy)*4));++s.cursor;
  }
  w.stateAnimation(s,AnimationEvent::Interrupt,s.interrupt);
  start(s,w);
  finishAnimation(s,w,AnimationEvent::BirthTick);
}
inline bool outside(const State& s){const auto& a=appearances[s.type];return !(s.x+a.width*.5f>-192&&s.x-a.width*.5f<192&&s.y+a.height*.5f>-64&&s.y-a.height*.5f<448);}
inline void tick(State& s,World& w,bool deferAnimation=false){
  if(s.phase==Phase::Inactive)return;
  s.collisionAge=s.age; // 40a020 advances lifetime only after 4099e0's collision.
  if(s.phase==Phase::Cancelling){s.x=float(double(s.x)+float(double(s.vx)*.5));s.y=float(double(s.y)+float(double(s.vy)*.5));++s.cancelAge;++s.visualAge;++s.age;if(!deferAnimation)finishAnimation(s,w);if(!w.ownsStateAnimation(s)&&(s.cancelAge>=8||s.type==30)){s.phase=Phase::Inactive;w.stateAnimation(s,AnimationEvent::Retire);}return;}
  // Native state 2 moves half speed, then observes ANM int0 to become state 1.
  if(s.phase==Phase::Spawning){
    s.x=float(double(s.x)+float(double(s.vx)*.5));s.y=float(double(s.y)+float(double(s.vy)*.5));
    if(!w.animationInteger(s,0,s.age>=s.spawnDuration?1:0)){++s.age;++s.visualAge;if(s.collisionDelay>0)--s.collisionDelay;if(s.offscreenGrace>0)--s.offscreenGrace;if(!deferAnimation)finishAnimation(s,w);return;}
    s.phase=Phase::Alive;
  }
  // 409dda jumps back to 409c9a on any completed transform. Remaining motion
  // callbacks may run again this tick, and newly started commands run at once.
  for(int pass=0;pass<4096;++pass){
  start(s,w);int completed=0;const bool hadActive=s.active!=0;
  if(s.active&1){if(s.fast.timer<=16){velocity(s,float(double(s.speed)+(5-double(s.fast.timer)*5*.0625)));++s.fast.timer;}else{s.active&=~1u;++completed;}}
  if(s.active&4){auto& m=s.acceleration;if(m.timer<m.duration){s.speed=float(double(s.speed)+m.a);s.vx=float(double(s.vx)+m.vx);s.vy=float(double(s.vy)+m.vy);if(std::abs(s.vx)>.0001f||std::abs(s.vy)>.0001f)s.angle=float(std::atan2(double(s.vy),double(s.vx)));++m.timer;}else{s.active&=~4u;++completed;}}
  if(s.active&8){auto& m=s.polar;if(m.timer<m.duration){s.angle=normalize(float(double(s.angle)+m.b));s.speed=float(double(s.speed)+m.a);velocity(s,s.speed);++m.timer;}else{s.active&=~8u;++completed;}}
  for(uint32_t kind:{0x10u,0x40u,0x20u})if(s.active&kind){auto& m=s.turn;float speed;
    if(m.timer<m.duration)speed=float(double(s.speed)-double(m.timer)*s.speed/m.duration);
    else{++m.count;if(m.count>=m.limit)s.active&=~kind;s.angle=kind==0x10?float(double(m.b)+s.angle):kind==0x40?m.b:normalize(float(double(aim(s,w))+m.b));speed=s.speed=m.a;m.timer=0;w.sound(s.program.transformSound,s.x);}
    velocity(s,speed);++m.timer;if(!(s.active&kind))++completed;
  }
  if(s.active&0x100){auto& m=s.bounce;if(!(s.x>-192&&s.x<192&&s.y>0&&s.y<448)){
    bool reflected=false;const bool detectOnly=m.count&16;
    if((m.count&1)&&s.y<0){if(!detectOnly){s.y=-s.y;s.angle=-s.angle;}reflected=true;}
    if((m.count&2)&&s.y>=448){if(!detectOnly){s.y=float((448-double(s.y))+448);s.angle=-s.angle;}reflected=true;}
    if((m.count&8)&&s.x>=192){if(!detectOnly){s.x=float(384-double(s.x));s.angle=normalize(float(-double(s.angle)-pi));}reflected=true;}
    if((m.count&4)&&s.x<-192){if(!detectOnly){s.x=float(-384-double(s.x));s.angle=normalize(float(-double(s.angle)-pi));}reflected=true;}
    if(m.a>-990)s.speed=m.a;velocity(s,s.speed);if(reflected){++m.duration;w.sound(s.program.transformSound,s.x);}if(m.duration>=m.limit){s.active&=~0x100u;++completed;}
  }}
  if(s.active&0x400){--s.protect.timer;if(s.protect.timer<=0){s.active&=~0x400u;++completed;}}
  if(s.active&0x1000){if(s.wait.timer<=0){s.active&=~0x1000u;++completed;}else --s.wait.timer;}
  if(hadActive&&s.collisionDelay>0)--s.collisionDelay;
  if(!completed)break;
  if(pass==4095)w.unsupported(0xffffffffu);
  }
  s.x=float(double(s.x)+s.vx);s.y=float(double(s.y)+s.vy);
  if(!(s.active&0x400)&&s.offscreenGrace<1&&outside(s)){s.phase=Phase::Inactive;w.stateAnimation(s,AnimationEvent::Retire);return;}
  // Common end-of-update countdown is additional to active motion passes.
  if(s.collisionDelay>0)--s.collisionDelay;
  if(s.offscreenGrace>0)--s.offscreenGrace;++s.age;++s.visualAge;
  if(!deferAnimation)finishAnimation(s,w);
}
inline Collision collide(const State& s,float x,float y,float playerHit){
  if(s.phase==Phase::Inactive||s.phase==Phase::Cancelling||s.collisionDelay>0||(s.phase==Phase::Spawning&&s.collisionAge<8))return Collision::None;
  const auto& a=appearances[s.type];
  if(a.effect==2){
    const float dx=float(double(x)-s.x),dy=float(double(y)-s.y),distance=float(double(dx)*dx+double(dy)*dy);const double r2=double(a.hitbox)*a.hitbox;
    if(double(playerHit)*playerHit+r2>distance)return Collision::Hit;
    const float graze=std::max(40.f,float(double(a.hitbox)/2.5));const double reach=double(playerHit)+graze;
    return reach*reach+r2>distance?Collision::Graze:Collision::None;
  }
  const float playerLeft=float(double(x)-double(playerHit)*.5),playerRight=float(double(x)+double(playerHit)*.5),playerTop=float(double(y)-double(playerHit)*.5),playerBottom=float(double(y)+double(playerHit)*.5);
  auto overlaps=[&](double half){const float left=float(double(s.x)-half),right=float(double(s.x)+half),top=float(double(s.y)-half),bottom=float(double(s.y)+half);return !(right<playerLeft||bottom<playerTop||left>playerRight||top>playerBottom);};
  if(overlaps(double(a.hitbox)*.5))return Collision::Hit;
  return overlaps(24)?Collision::Graze:Collision::None;
}
}
