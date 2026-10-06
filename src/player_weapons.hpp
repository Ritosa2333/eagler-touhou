// TH12 1.00b player damage sources, separate from the game's entity container.
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>
namespace th12::pw {
struct Manager;
inline bool rectangle(float x,float y,float width,float height,float ex,float ey,float ew,float eh,bool laser=false);
enum class Phase:int {Inactive=0,Alive=1,Impact=2};
struct State {
  Phase phase=Phase::Alive;
  int visualAge=0,impactDuration=0,previousAge=-1,interrupt=0;
  bool hit=false,auxiliaryHit=false;
  float acceleration=0,angularVelocity=0;
};
struct DamageSource {
  uint32_t id=0;float x=0,y=0,radius=0,growth=0;
  float width=0,height=0,widthGrowth=0,heightGrowth=0,angle=0;
  int ticks=0,previousTicks=0,damage=0,accumulated=0,limit=999999,cadence=4;
  uint32_t flags=0;
};
struct BombArea {bool enabled=false;float x=0,y=0,width=0,height=0;};
// Marisa B: retail 4079d0 damage and 407a30 sweep rectangle. Timer starts at
// zero; Bomb priority 17 updates after Player priority 16, before Enemy 18.
inline float marisaBFront(int age){return age<90?float(448.0-(double(age)-30)*448.0/60.0):0.f;}
inline int marisaBDamage(int age,float y){return age>=30&&y>=marisaBFront(age)?16:0;}
inline BombArea marisaBArea(int age){if(age<30)return {};const float front=marisaBFront(age);return {true,0,float((double(front)+448)*.5),384,float(448-double(front))};}
struct MarisaBBomb {
  uint32_t id=0;bool active=false,visible=false;int age=0,visualAge=0,interrupt=0;
  float x=0,y=0;BombArea clear{};
  void begin(uint32_t identity,float playerX,float playerY){*this={};id=identity;active=visible=true;x=playerX;y=playerY;}
  // Returns whether the original Bomb update refreshed the player's iframes40.
  bool tick(){
    clear={};if(visible&&!active){if(++visualAge>=60)visible=false;return false;}
    if(!active)return false;
    if(age>=161){active=false;++visualAge;return true;}
    if(age==160){interrupt=1;visualAge=0;}
    else {clear=marisaBArea(age);++visualAge;}
    ++age;return true;
  }
  int damage(float enemyY)const{return active?marisaBDamage(age,enemyY):0;}
};
struct BombCircle {float x=0,y=0,radius=0;};
struct BombVisual {uint32_t id=0;int bank=0,script=0,age=0,interrupt=0;float x=0,y=0,angle=0;bool active=true;};
struct RetiredVisual {BombVisual visual{};int remaining=0;};
struct TrailPoint {float x=0,y=0;uint32_t color=0xffffffffu;};
struct BombTrail {uint32_t id=0;int kind=13;float width=1;std::vector<TrailPoint> points;};
inline float signedRandom(uint32_t raw){return float(double(raw)/2147483648.0-1);}
inline float unitRandom(uint32_t raw){return float(double(raw)/4294967296.0);}
inline float angle(float a){constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;while(a>pi)a-=tau;while(a<-pi)a+=tau;return a;}
struct Leaf {
  uint32_t id=0;int age=1;float heading=0;bool active=true;std::array<TrailPoint,16> points{};
  template<class R>void begin(uint32_t identity,float x,float y,R& rng){
    *this={};id=identity;points[0].x=x;points[0].y=y;heading=float(double(signedRandom(rng.next32()))*3.1415927410125732);
    for(int i=0;i<16;++i)points[i].color=i<8?(0xff40ff00u|uint32_t(255-i*32)):(uint32_t(255-(i-8)*16)<<24|0x0040ff00u);
  }
  template<class R>void tick(R& rng){
    if(!active)return;if(age>=16){active=false;return;}
    for(int i=0;i<age;++i){uint32_t alpha=points[i].color>>24;alpha=alpha>=16?alpha-16:0;points[i].color=(points[i].color&0xffffffu)|(alpha<<24);}
    const float length=float(double(unitRandom(rng.next32()))*8+8);
    const float vx=float(std::cos(double(heading))*length),vy=float(std::sin(double(heading))*length);
    points[age].x=float(double(points[age-1].x)+vx);points[age].y=float(double(points[age-1].y)+vy);
    const float turn=float(double(float(double(signedRandom(rng.next32()))*3.1415927410125732))/10);
    heading=angle(float(double(heading)+turn));++age;
  }
};
inline float sanaeBClearRadius(int age){return age<180?float(double(age)*48/180+16):age<200?float((double(age)-180)*416/20+64):480.f;}
inline int sanaeADamage(float y){return y>=0?14:0;}
inline int marisaADamage(int age,int previous,float x,float y,float ex,float ey,float ew,float eh){
  if(age==previous||age%4)return 0;
  const float cy=float(double(y)-224);int damage=0;
  if(rectangle(x,cy,32,512,ex,ey,ew,eh))damage+=30;
  if(rectangle(x,cy,128,448,ex,ey,ew,eh))damage+=20;
  if(rectangle(x,cy,256,384,ex,ey,ew,eh))damage+=20;
  return damage;
}
inline std::array<BombArea,3> marisaAAreas(float x,float y){return {{{true,x,float(double(y)-208),32,480},{true,x,float(double(y)-224),128,448},{true,x,float(double(y)-240),256,416}}};}
struct ReimuAEmitter {uint32_t id=0;int age=0;float first=0,second=0,firstStep=18,secondStep=-18;bool active=true;
  std::array<float,2> fire(){std::array<float,2> out{second,first};second=float(double(second)+secondStep);if(std::abs(second)>=24)secondStep=-secondStep;first=float(double(first)+firstStep);if(std::abs(second)>=24)firstStep=-firstStep;return out;}
};
struct ReimuABeam {
  uint32_t id=0,sourceId=0;int age=0,sourceSlot=-1;bool active=true;
  float x=0,y=464,heading=-1.5707963705062866f,anchorX=0;std::array<TrailPoint,12> history{};
  void begin(uint32_t identity,float originX,float anchor,Manager& manager);
  void tick(Manager& manager);
};
struct ReimuBOrb {
  uint32_t id=0,sourceId=0;int index=0,age=0,sourceSlot=-1,tailAge=0;bool active=true,polar=true;
  float x=0,y=0,centerX=0,centerY=0,heading=0,radius=0,speed=0,deltaX=0,deltaY=0;
  void begin(uint32_t identity,int ordinal,float playerX,float playerY,Manager& manager);
  void tick(float playerX,float playerY,bool hasTarget,float targetX,float targetY);
};
struct BombState {
  int loadout=-1,age=0,previousAge=-1,tailAge=0;bool active=false;
  float x=0,y=0,speedMultiplier=1;
  MarisaBBomb marisaB{};
  std::vector<BombVisual> visuals;
  std::vector<BombVisual> primaryVisuals;std::vector<RetiredVisual> retiredVisuals;
  std::vector<BombTrail> trails;std::vector<Leaf> leaves;
  ReimuAEmitter reimuAEmitter{};std::vector<ReimuABeam> reimuABeams;
  std::array<ReimuBOrb,8> reimuBOrbs{};
  std::vector<BombArea> rectangles;
  std::vector<BombCircle> circles;
  void reset(){*this={};}
};
struct Manager {
  std::array<DamageSource,128> sources{};uint32_t nextId=1;
  void reset(){sources={};nextId=1;}
  DamageSource* circle(float x,float y,float radius,float growth,int ticks,int damage){
    for(auto& s:sources)if(!(s.flags&1)){s={};s.id=nextId++;s.x=x;s.y=y;s.radius=radius;s.growth=growth;s.ticks=ticks;s.previousTicks=ticks-1;s.damage=damage;s.flags=3;return &s;}return nullptr;
  }
  // 436f90..437098 advances radius and timer before ordinary projectile update.
  void tick(){for(auto& s:sources)if(s.flags&1){s.radius=float(double(s.radius)+s.growth);s.width=float(double(s.width)+s.widthGrowth);s.height=float(double(s.height)+s.heightGrowth);s.previousTicks=s.ticks;--s.ticks;if(s.ticks<=0)s.flags&=~1u;}}
  int damage(float x,float y,float width,float height){
    int total=0;
    for(auto& s:sources){
      if(!(s.flags&1)||s.damage<=0||s.cadence<=0)continue;
      // Native 43a228..235 suppresses the divisible ticks. This differs from lasers.
      if(s.ticks!=s.previousTicks&&s.ticks%s.cadence==0)continue;
      bool hit=false;
      if(s.flags&2){const float dx=float(double(s.x)-x),dy=float(double(s.y)-y);const float d=float(double(dx)*dx+double(dy)*dy);hit=double(s.radius)*s.radius>=d;}
      else {const double dx=double(x)-s.x,dy=double(y)-s.y,c=std::cos(double(s.angle)),v=std::sin(double(s.angle));const float tx=float(dx*c+dy*v),ty=float(dy*c-dx*v);hit=std::abs(tx)<=width*.5f+s.width*.5f&&std::abs(ty)<=height*.5f+s.height*.5f;}
      if(hit){total+=s.damage;s.accumulated+=s.damage;if(s.accumulated>=s.limit)s.damage=0;}
    }return total;
  }
};
inline void ReimuABeam::begin(uint32_t identity,float originX,float anchor,Manager& manager){
  *this={};id=identity;x=originX;anchorX=anchor;
  for(auto& p:history){p.x=x;p.y=y;p.color=0xffffffffu;}
  auto* s=manager.circle(x,y,20,-3,999,10);if(s){s->flags&=~2u;s->width=128;s->height=24;sourceSlot=int(s-manager.sources.data());sourceId=s->id;}
}
inline void ReimuABeam::tick(Manager& manager){
  if(!active)return;
  for(int i=11;i>0;--i)history[i]=history[i-1];
  const float vx=float(std::cos(double(heading))*32),vy=float(std::sin(double(heading))*32);
  x=float(std::floor(double(float(double(float(double(x)+vx))*100)))/100);y=float(std::floor(double(float(double(float(double(y)+vy))*100)))/100);
  history[0].x=x;history[0].y=y;
  if(history[11].y<=0){active=false;return;}
  if(sourceSlot>=0){auto& s=manager.sources[sourceSlot];if(s.id!=sourceId||!(s.flags&1))sourceSlot=-1;
    else{s.x=history[2].x;s.y=history[2].y;const float dx=float(double(history[1].x)-history[0].x),dy=float(double(history[1].y)-history[0].y);s.angle=float(std::atan2(double(dy),double(dx)));if(s.y<-64){s.flags&=~1u;sourceSlot=-1;}}}
  ++age;
}
inline void ReimuBOrb::begin(uint32_t identity,int ordinal,float px,float py,Manager& manager){
  *this={};id=identity;index=ordinal;centerX=px;centerY=py;heading=float(double(ordinal)*.7853981852531433);
  auto* s=manager.circle(0,0,56,0,9999,10);if(s){s->limit=300;sourceSlot=int(s-manager.sources.data());sourceId=s->id;}
}
inline void ReimuBOrb::tick(float px,float py,bool hasTarget,float tx,float ty){
  if(!active){++tailAge;return;}
  const int launch=90+index*10;
  if(age<90){centerX=px;centerY=py;radius=float(double(radius)+1);heading=angle(float(double(heading)+.10471975803375244));}
  else if(age<launch){centerX=px;centerY=py;heading=angle(float(double(heading)+.10471975803375244));}
  else if(age==launch){polar=false;heading=angle(float(std::atan2(double(deltaY),double(deltaX))));speed=float(std::sqrt(double(float(double(deltaX)*deltaX+double(deltaY)*deltaY))));}
  else if(hasTarget){const float dx=float(double(tx)-x),dy=float(double(ty)-y),desired=float(std::atan2(double(dy),double(dx))),turn=angle(float(double(desired)-heading));
    if(std::abs(turn)>=.7853981852531433)speed=std::max(1.f,float(double(speed)-.699999988079071));
    else if(std::abs(turn)<.2617993950843811f)speed=std::min(8.f,float(double(speed)+.20000000298023224));
    const float step=float(double(turn)*.10000000149011612);heading=angle(float(double(heading)+step));
  }else if(x<=-160||x>=160||y<=32||y>=416)speed=float(double(speed)*.8999999761581421);
  const float oldX=x,oldY=y;
  if(polar){radius=float(double(radius)+.04908738657832146);x=float(double(centerX)+float(std::cos(double(heading))*radius));y=float(double(centerY)+float(std::sin(double(heading))*radius));}
  else {centerX=float(std::cos(double(heading))*speed);centerY=float(std::sin(double(heading))*speed);x=float(double(x)+centerX);y=float(double(y)+centerY);}
  x=float(std::floor(double(float(double(x)*100)))/100);y=float(std::floor(double(float(double(y)*100)))/100);
  deltaX=float(double(x)-oldX);deltaY=float(double(y)-oldY);++age;
}
inline bool rectangle(float x,float y,float width,float height,float ex,float ey,float ew,float eh,bool laser){
  const float left=float(double(x)-double(width)*.5),right=float(double(x)+double(width)*.5);
  const float top=laser?float(double(y)-height):float(double(y)-double(height)*.5),bottom=laser?y:float(double(y)+double(height)*.5);
  const float el=float(double(ex)-double(ew)*.5),er=float(double(ex)+double(ew)*.5),et=float(double(ey)-double(eh)*.5),eb=float(double(ey)+double(eh)*.5);
  return !(right<el||bottom<et||left>er||top>eb);
}
inline bool projectileCadence(int age,int previousAge,int type){return (type!=2&&type!=4)||(age!=previousAge&&age%4==0);}
}
