// TH12 1.00b development core. Hand-authored systems, not x86 translation.
// ECL and SHT data remain retail inputs; rendering never advances this state.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <list>
#include <memory>
#include <sstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>
#include "player.hpp"
#include "items.hpp"
#include "ufo.hpp"
#include "bullet.hpp"
#include "bullet_visuals.hpp"
#include "player_weapons.hpp"
#include "enemy_motion.hpp"
#include "message.hpp"
#include "anm_scene.hpp"
#include "spell.hpp"
#include "stage_award.hpp"
#include "stage_logic.hpp"

namespace th12 {
using u8=uint8_t;using u16=uint16_t;using u32=uint32_t;using i32=int32_t;
template<class T>T rd(const u8* p){T x;std::memcpy(&x,p,sizeof x);return x;}
float asfloat(u32 x){float f;std::memcpy(&f,&x,4);return f;}
u32 asbits(float f){u32 x;std::memcpy(&x,&f,4);return x;}
constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;
struct Random {
  u16 seed=0;u32 calls=0;
  u16 next16(){const u16 a=u16((seed^0x9630)-0x6553);seed=u16((a<<2)|(a>>14));calls++;return seed;}
  u32 next32(){u16 a=u16((seed^0x9630)-0x6553),b=u16((a<<2)|(a>>14)),c=u16((b^0x9630)-0x6553);seed=u16((c<<2)|(c>>14));calls+=2;return u32(a)<<16|c;}
  float unit(){return float(double(next32())/4294967296.0);}
  float signedUnit(){return float(double(next32())/2147483648.0-1);}
};
struct Program {
  std::vector<std::vector<u8>> files;std::unordered_map<std::string,const u8*> routines;
  bool add(const u8* input,u32 size){
    if(size<36||rd<u32>(input)!=0x54504353||rd<u16>(input+4)!=1)return false;
    u32 table=36+rd<u16>(input+6),count=rd<u16>(input+16);
    if(table>size||count>(size-table)/4)return false;
    files.emplace_back(input,input+size);const auto* b=files.back().data();u32 names=table+count*4;
    for(u32 i=0;i<count;i++){
      u32 off=rd<u32>(b+table+i*4),begin=names;
      while(names<size&&b[names])names++;
      if(names==size||off>size-16||rd<u32>(b+off)!=0x484c4345)return false;
      routines[std::string(reinterpret_cast<const char*>(b+begin),names-begin)]=b+off+16;names++;
      u32 end=size;for(u32 j=0;j<count;j++){u32 other=rd<u32>(b+table+j*4);if(other>off)end=std::min(end,other);}
      for(u32 p=off+16;p<end;){if(p>end-16)return false;u32 n=rd<u16>(b+p+6);if(n<16||n%4||n>end-p)return false;p+=n;}
    }return true;
  }
  const u8* find(const std::string& name){auto i=routines.find(name);return i==routines.end()?nullptr:i->second;}
} program;
i32 nativeInt(float value){return !std::isfinite(value)||double(value)<-2147483648.0||double(value)>=2147483648.0?i32(0x80000000u):i32(value);}
struct Value{u32 bits=0;bool integer=false;float floating()const{return integer?float(i32(bits)):asfloat(bits);}i32 integral()const{return integer?i32(bits):nativeInt(asfloat(bits));}};
struct CallFrame{const u8* pc;float time;std::array<u32,256> locals;};
struct Context {
  const u8* pc=nullptr;float time=0;int id=-1;std::array<u32,256> locals{};
  std::vector<Value> stack;std::vector<CallFrame> calls;
  Context(const u8* p=nullptr):pc(p){}
  Value popValue(){if(stack.empty())return {};Value value=stack.back();stack.pop_back();return value;}
  float pop(){return popValue().floating();}
  i32 popInt(){return popValue().integral();}
  void push(float f,bool integer=false){stack.push_back({integer?u32(nativeInt(f)):asbits(f),integer});}
  void pushInt(i32 value){stack.push_back({u32(value),true});}
};
struct Shooter{float x=0,y=0,originX=0,originY=0,radius=0,angle=0,spacing=0,speed=2,slow=0;int count=1,layers=1,mode=0,type=0,color=0,fireSound=22;bool fixedOrigin=false;eb::Program extensions{};Shooter(){extensions.flags=0x83;extensions.transformSound=40;}};
struct EnemyInterrupt{int health=-1,time=-1;std::string healthScript,timeoutScript;};
struct CircularMotion{bool enabled=false;float centerX=0,centerY=0,radius=0,radialSpeed=0,fromOmega=0,toOmega=0,fromRadius=0,toRadius=0,fromRadial=0,toRadial=0;int ticks=0,time=0,mode=0;};
struct Enemy {
  u32 id=0;float x=0,y=0,ax=0,ay=0,angle=0,speed=0,rx=0,ry=0,relativeAngle=0,relativeSpeed=0,hit=12;int life=30,score=1000,drop=1,age=0,bank=1,boundBank=1,animation=0,baseAnimation=0,animationAge=0;
  bool active=true,boss=false,hidden=false,invincible=false,mirror=false,timedOut=false;u32 flags=0;int immunityTicks=2,bodyImmunityTicks=0;
  std::array<u32,12> variables{};std::array<EnemyInterrupt,8> interrupts{};
  std::array<int,19> dropCounts{};float dropSpreadX=32,dropSpreadY=32;
  float fromX=0,fromY=0,toX=0,toY=0,fromA=0,toA=0,fromS=0,toS=0;
  int positionTicks=0,positionTime=0,positionMode=0,polarTicks=0,polarTime=0,polarMode=0;
  float relativeFromX=0,relativeFromY=0,relativeToX=0,relativeToY=0,relativeFromA=0,relativeToA=0,relativeFromS=0,relativeToS=0;
  int relativePositionTicks=0,relativePositionTime=0,relativePositionMode=0,relativePolarTicks=0,relativePolarTime=0,relativePolarMode=0;
  float clampX=0,clampY=0,clampWidth=0,clampHeight=0;int phaseAge=0;
  CircularMotion absoluteCircle{},relativeCircle{};
  int maxLife=30;float hitWidth=24,hitHeight=24,bodyWidth=24,bodyHeight=24,renderWidth=0,renderHeight=0;bool isUfo=false;
  std::array<int,16> animationSlots{};std::array<int,16> animationBanks{};std::array<bool,16> animationBound{};
  std::list<Context> contexts;std::array<Shooter,16> shooters{};
};
struct Projectile{float x,y,angle,speed,radius;int type,color,age=0,damage=0,animation=0;bool friendly=false,active=true,grazed=false;int option=0,update=0;float hitWidth=0,hitHeight=0;u32 targetId=0;int extraCallback=0;u32 entityId=0;eb::State enemy{};pw::State weapon{};int hitAnimation=0,hitCallback=0,spawnCallback=0;};
std::array<Loadout,6> loadouts;
PlayerMotion playerMotion;
ShotSchedule shotSchedule;
Random random;
Random displayRandom{0x1234,0};
anm_logic::Scene animationScene;
u32 nextDetachedAnimation=1;
uint64_t enemyAnimationKey(u32 id,int slot){return (uint64_t(id)<<8)|uint32_t(slot);}
void configureAnimations(){animationScene.runtime.gameRandom32=[](){return random.next32();};animationScene.runtime.displayRandom32=[](){return displayRandom.next32();};}
std::deque<std::unique_ptr<Enemy>> enemies;
std::deque<Projectile> bullets;
item_system::Manager itemManager;
ufo_system::Manager ufoManager;
item_system::ResourceState resources;
std::array<message_system::File,6> messageFiles;
message_system::VM messageVM;
spell_system::State spellState;
extern pw::BombState playerBomb;
extern int stageNumber,frame,character;
std::array<uint64_t,7> spellAnimationKeys{};
// Native MSG handles +40/+44/+48/+4c/+50/+54/+58/+5c. Clearing a
// handle preserves a VM that is still playing its exit animation.
std::array<uint64_t,8> messageAnimationKeys{};
struct MessageEvent {int op=0,frame=0;std::vector<u8> payload;};
std::vector<MessageEvent> messageEvents;
std::array<u32,1024> unsupported{};
std::unordered_map<u32,u32> animationUnsupported;
void bindEnemyAnimation(Enemy& e,int slot,int script){
  if(slot<0||slot>=16){++unsupported[259];return;}
  e.animationSlots[slot]=script;e.animationBanks[slot]=e.bank;e.animationBound[slot]=true;
  animationScene.bind(enemyAnimationKey(e.id,slot),e.id,e.bank,script,anm_logic::Membership::Primary,e.x,e.y);
}
void detachedAnimation(int bank,int script,const Enemy& e,bool head){animationScene.bind(0x100000000ull+nextDetachedAnimation++,0,bank,script,anm_logic::Membership::Primary,e.x,e.y,{},head);}
int spellAnimationBank(spell_system::VisualBank bank){switch(bank){case spell_system::VisualBank::Font:return 11;case spell_system::VisualBank::SpellText:return 8;case spell_system::VisualBank::Bullet:return 0;case spell_system::VisualBank::Stage:return 2;case spell_system::VisualBank::StageAlternate:return 3;case spell_system::VisualBank::Front:return 7;}return -1;}
uint64_t bindScreenAnimation(int bank,int script,int layer=23){const u32 serial=nextDetachedAnimation++;const uint64_t key=0x300000000ull+serial;animationScene.bind(key,0xe0000000u+serial,bank,script,anm_logic::Membership::Primary,0,0,{},false,nullptr,0,0,layer);return key;}
void startMessageAnimations(){
  messageAnimationKeys={};
  for(int slot=0;slot<4;slot++)messageAnimationKeys[slot+3]=bindScreenAnimation(8,slot);
}
void messagePortraitSprites(uint64_t key,const int* childScripts,const int* spriteBases,int expression){
  const auto* root=animationScene.find(key);if(!root)return;
  for(int i=0;i<3;i++)if(spriteBases[i]>=0)for(const auto* child:animationScene.chain(anm_logic::Membership::Primary)){
    if(child->vm.state().script!=childScripts[i])continue;
    u32 ancestor=child->parent;bool attached=false;
    for(int depth=0;ancestor&&depth<16;depth++){if(ancestor==root->id){attached=true;break;}auto at=animationScene.nodes().find(ancestor);if(at==animationScene.nodes().end())break;ancestor=at->second->parent;}
    if(attached){auto& pose=animationScene.nodes().at(child->id)->vm.control();const auto bank=animationScene.registry.bank(pose.bank);const int sprite=spriteBases[i]+expression;if(bank&&sprite>=0&&sprite<int(bank->sprites.size()))pose.sprite=sprite;else ++unsupported[983];break;}
  }
}
void messageCommand(const message_system::Command& command){
  const auto interrupt=[](int slot,int code){animationScene.interrupt(messageAnimationKeys[slot],code);};
  switch(command.op){
    case 1:{static constexpr int scripts[]={31,30,28};messageAnimationKeys[0]=bindScreenAnimation(4+character,scripts[character]);break;}
    case 2:{static constexpr int scripts[]={13,16,10,19,10,12,11};const bool alternate=(stageNumber==5||stageNumber==7)&&messageVM.entry>=2;messageAnimationKeys[1]=bindScreenAnimation(alternate?3:2,alternate?(stageNumber==5?10:13):scripts[stageNumber-1]);break;}
    case 3:messageAnimationKeys[2]=bindScreenAnimation(7,52);break;
    case 4:interrupt(0,1);messageAnimationKeys[0]=0;break;
    case 5:interrupt(1,1);interrupt(7,1);messageAnimationKeys[1]=0;break;
    case 6:for(int slot=2;slot<7;slot++)interrupt(slot,1);break;
    case 7:case 8:case 9:{const int code=command.op==9?8:2;interrupt(0,command.op==8?3:code);interrupt(1,command.op==7?3:code);interrupt(2,command.op==9?8:command.op-5);for(int slot=3;slot<7;slot++){animationScene.position(messageAnimationKeys[slot],8,0);if(auto* node=animationScene.find(messageAnimationKeys[slot]))node->vm.control().offsetY=0;}break;}
    case 13:{static constexpr int scripts[3][3]={{28,29,30},{27,28,29},{25,26,27}},sprites[3][3]={{43,51,59},{42,50,58},{43,51,59}};messagePortraitSprites(messageAnimationKeys[0],scripts[character],sprites[character],command.integer());break;}
    case 14:{static constexpr int scripts[7][3]={{10,11,12},{13,14,15},{7,8,9},{16,17,18},{7,8,9},{9,10,11},{8,9,10}},sprites[7][3]={{14,22,-1},{16,25,34},{12,21,30},{28,37,46},{15,24,33},{16,25,34},{17,26,35}};static constexpr int alternateScripts[2][3]={{7,8,9},{10,11,12}},alternateSprites[2][3]={{12,20,29},{14,23,32}};const bool alternate=(stageNumber==5||stageNumber==7)&&messageVM.entry>=2;const int index=stageNumber==7;messagePortraitSprites(messageAnimationKeys[1],alternate?alternateScripts[index]:scripts[stageNumber-1],alternate?alternateSprites[index]:sprites[stageNumber-1],command.integer());break;}
    case 18:for(int slot=3;slot<7;slot++)interrupt(slot,3);break;
    case 19:bindScreenAnimation(9,2);break;
    case 20:{static constexpr int scripts[]={17,21,14,23,14,16,16};messageAnimationKeys[7]=bindScreenAnimation(2,scripts[stageNumber-1]);break;}
    case 23:interrupt(0,7);break;
    case 24:interrupt(1,7);break;
    case 25:for(int slot=3;slot<7;slot++)if(auto* node=animationScene.find(messageAnimationKeys[slot]))node->vm.control().offsetY=float(command.integer());break;
    default:break;
  }
  for(const auto& action:messageVM.textActions)if(action.slot>=0&&action.slot<4)interrupt(action.slot+3,action.interrupt);
}
void startSpellAnimations(){
  spellAnimationKeys={};const auto bindings=spell_system::startBindings(stageNumber,frame);
  for(size_t i=0;i<bindings.size()&&i<spellAnimationKeys.size();i++)spellAnimationKeys[i]=bindScreenAnimation(spellAnimationBank(bindings[i].bank),bindings[i].script,bindings[i].layer);
  animationScene.position(spellAnimationKeys[3],spellState.ringX+224,spellState.ringY+16);
  if(auto* root=animationScene.find(spellAnimationKeys[3]))for(auto& [id,node]:animationScene.nodes())if(node->parent==root->id&&(node->vm.state().script==125||node->vm.state().script==126))node->vm.control().integers[2]=spellState.timeLimit;
}
void endSpellAnimations(const spell_system::Result& result){
  if(result.kind==spell_system::ResultKind::None)return;
  for(int i=0;i<3;i++)animationScene.interrupt(spellAnimationKeys[i],1);
  animationScene.remove(spellAnimationKeys[5]);animationScene.remove(spellAnimationKeys[3]);spellAnimationKeys[3]=spellAnimationKeys[5]=0;
  const bool captured=result.kind==spell_system::ResultKind::Captured;const auto digits=spell_system::resultDigits(result.bonusDisplayed);
  for(const auto& binding:spell_system::resultBindings(captured,result.bonusDisplayed)){const auto key=bindScreenAnimation(spellAnimationBank(binding.bank),binding.script,binding.layer);if(binding.bank==spell_system::VisualBank::Font&&binding.script>=5&&binding.script<=12)if(auto* node=animationScene.find(key)){const auto& digit=digits[binding.script-5];auto& state=node->vm.control();state.sprite=digit.sprite;state.visible=digit.visible;if(digit.visible)state.flags|=1;else state.flags&=~1u;}}
}
bool enemyOutside(Enemy& e){
  if(auto* node=animationScene.find(enemyAnimationKey(e.id,0))){const auto& pose=node->vm.state();if(auto bank=animationScene.registry.bank(pose.bank);bank&&pose.sprite>=0&&pose.sprite<int(bank->sprites.size())){
    const auto& sprite=bank->sprites[pose.sprite];
    // 413d3e crosses the axes: X extent uses sprite height*scaleY, Y uses
    // sprite width*scaleX. A missing slot retains the previous extents.
    e.renderWidth=std::abs(float(double(sprite.height)*pose.sy));e.renderHeight=std::abs(float(double(sprite.width)*pose.sx));
  }}
  const bool outsideX=float(double(e.x)+e.renderWidth*.5f)<-192||float(double(e.x)-e.renderWidth*.5f)>192;
  if(outsideX)return (e.flags&0x8000u)&&!(e.flags&4);
  const bool outsideY=float(double(e.y)+e.renderHeight*.5f)<0||float(double(e.y)-e.renderHeight*.5f)>448;
  if(outsideY)return (e.flags&0x8000u)&&!(e.flags&8);
  e.flags|=0x8000u;return false;
}
std::unordered_map<int,Value> globals;
int frame=0,character=0,shot=0,difficulty=1,stageNumber=1,px=0,py=51200,graze=0;
int backgroundFrame=0;bool backgroundVisible=true;
stage_logic::Program stageProgram;
stage_logic::State stageState;
uint64_t stagePrimitiveKey(int slot){return 0xc00000000ull+u32(slot);}
uint64_t stageEmbeddedKey(int slot){return 0xd00000000ull+u32(slot);}
struct StageWorld:stage_logic::World {
  void unsupported(int opcode,const char*)override{++th12::unsupported[982];}
  void tickObjects()override{for(const auto& q:stageProgram.primitives)animationScene.tickEmbedded(stagePrimitiveKey(q.slot));}
  void bindEmbedded(int slot,int script)override{if(slot<0||slot>=8){++th12::unsupported[982];return;}if(script<0){animationScene.remove(stageEmbeddedKey(slot));return;}animationScene.bind(stageEmbeddedKey(slot),0xc0000000u+slot,10,script,anm_logic::Membership::Embedded,0,0,{},false,nullptr,0,0);}
  void tickEmbedded(int slot)override{animationScene.tickEmbedded(stageEmbeddedKey(slot));}
  void publish(const stage_logic::Camera& camera)override{animationScene.runtime.cameraPosition=camera.position;animationScene.runtime.cameraEyeOffset=camera.eyeOffset;}
} stageWorld;
void initializeStageAnimations(){
  stageState.reset(stageProgram);stageWorld.publish(stageState.camera);
  for(const auto& q:stageProgram.primitives)if(auto* node=animationScene.bind(stagePrimitiveKey(q.slot),0xc1000000u+q.slot,10,q.script,anm_logic::Membership::Embedded,0,0,{},false,nullptr,0,0,q.layer))node->customGeometry=true;
}
int &power=resources.power,&lives=resources.lives,&bombs=resources.bombs,&score=resources.scoreStored;
int invuln=120,deathWindow=0,bombTimer=0,previousHeld=0,fireFrame=0,eventBits=0,dialogue=0,spell=0,ended=0;
int playerState=1,playerStateTicks=0;
int oracleFixtureFlags=0; // Explicit partial-world diagnostics only; normal runtime stays zero.
u32 nextEnemyId=1,nextProjectileId=1,nextItemId=1;
float aim(float x,float y){return float(std::atan2(double(py/128.f-y),double(px/128.f-x)));}
float normalize(float angle){while(angle>pi)angle-=tau;while(angle<-pi)angle+=tau;return angle;}
float mirrorAngle(float angle){return normalize(float(double(pi/2)-normalize(float(double(angle)-pi/2))));}
void combinePosition(Enemy& e){const float x=float(double(e.ax)+e.rx),y=float(double(e.ay)+e.ry);const float vx=float(double(x)-e.x),vy=float(double(y)-e.y);e.x=enemy_motion::quantize(float(double(e.x)+vx));e.y=enemy_motion::quantize(float(double(e.y)+vy));}
float movementCoordinate(double v){return enemy_motion::quantize(float(v));}
void circularPosition(Enemy& e,bool relative){auto& m=relative?e.relativeCircle:e.absoluteCircle;const float angle=relative?e.relativeAngle:e.angle;
  (relative?e.rx:e.ax)=movementCoordinate(double(m.centerX)+float(std::cos(double(angle))*m.radius));
  (relative?e.ry:e.ay)=movementCoordinate(double(m.centerY)+float(std::sin(double(angle))*m.radius));
}
Enemy* primaryBoss(){for(auto& p:enemies)if(p->active&&p->boss)return p.get();return nullptr;}
float easing(float value,int mode){
  const double t=std::clamp(value,0.f,1.f);
  if(mode>=1&&mode<=3)return float(std::pow(t,mode+1));
  if(mode>=4&&mode<=6)return float(1-std::pow(1-t,mode-2));
  if(mode>=9&&mode<=11){const int n=mode-7;return float(t<.5?std::pow(2*t,n)*.5:1-std::pow(2-2*t,n)*.5);}
  if(mode>=12&&mode<=14){const int n=mode-10;return float(t<.5?(1-std::pow(1-2*t,n))*.5:(std::pow(2*t-1,n)+1)*.5);}
  return mode==15?0.f:mode==16?1.f:float(t);
}
float global(Enemy& e,int id,bool floating=true){
  if(id>=-9985&&id<=-9982)return float(i32(e.variables[id+9985]));
  if(id>=-9981&&id<=-9978)return asfloat(e.variables[id+9985]);
  if(id>=-9935&&id<=-9932)return asfloat(e.variables[id+9943]);
  switch(id){case -10000:return float(random.next32());case -9999:return random.unit();case -9998:return random.signedUnit()*pi;case -9987:return random.signedUnit();
  case -9997:case -9977:return e.x;case -9996:case -9976:return e.y;
  case -9995:case -9975:return e.ax;case -9994:case -9974:return e.ay;
  case -9993:case -9973:return e.rx;case -9992:case -9972:return e.ry;
  case -9991:case -9965:return px/128.f;case -9990:case -9964:return py/128.f;case -9989:return aim(e.x,e.y);
  case -9988:return e.age;case -9986:return e.timedOut;case -9971:return e.angle;case -9969:return e.speed;
  case -9970:return e.relativeAngle;case -9968:return e.relativeSpeed;
  case -9963:{auto* b=primaryBoss();return b?b->x:0;}case -9962:{auto* b=primaryBoss();return b?b->y:224;}
  case -9960:return 0;case -9959:return difficulty;case -9954:return e.life;case -9957:return 1;
  case -9944:{const double dx=e.x-px/128.f,dy=e.y-py/128.f;return float(std::sqrt(dx*dx+dy*dy));}
  case -9953:case -9952:case -9951:case -9950:return difficulty==id+9953;
  case -9946:{int n=0;for(auto& p:enemies)n+=p->active&&!p->hidden;return n;}case -9945:return character*2+shot;
  default:{auto found=globals.find(id);return found==globals.end()?0:found->second.floating();}}
}
i32 globalInt(Enemy& e,int id){
  if(id==-10000)return i32(random.next32());
  if(id>=-9985&&id<=-9982)return i32(e.variables[id+9985]);
  if(auto found=globals.find(id);found!=globals.end())return found->second.integral();
  return nativeInt(global(e,id,false));
}
i32 argInt(Context& c,Enemy& e,int index,int referenceIndex=-1){
  const i32 raw=rd<i32>(c.pc+16+index*4);if(referenceIndex<0)referenceIndex=index;
  if(referenceIndex>=16||!(rd<u16>(c.pc+8)&(1<<referenceIndex)))return raw;
  if(raw==-1)return c.popInt();if(raw<0)return globalInt(e,raw);
  if(raw>=1024){unsupported[1000]++;return 0;}return i32(c.locals[raw/4]);
}
float arg(Context& c,Enemy& e,int index,bool isFloat=false,int referenceIndex=-1){
  const u32 raw=rd<u32>(c.pc+16+index*4);if(referenceIndex<0)referenceIndex=index;
  const bool ref=referenceIndex<16&&(rd<u16>(c.pc+8)&(1<<referenceIndex));
  if(!isFloat)return float(argInt(c,e,index,referenceIndex));
  const float f=asfloat(raw);if(!ref)return f;
  const int id=int(f);if(id==-1)return c.pop();if(id<0){const float v=global(e,id,isFloat);return isFloat?v:float(i32(v));}
  if(id>=1024){unsupported[1000]++;return 0;}return isFloat?asfloat(c.locals[id/4]):float(i32(c.locals[id/4]));
}
void store(Context& c,Enemy& e,int id,float value,bool isFloat){
  const u32 raw=isFloat?asbits(value):u32(i32(value));
  if(id>=0&&id<1024)c.locals[id/4]=raw;
  else if(id>=-9985&&id<=-9978)e.variables[id+9985]=raw;
  else if(id>=-9935&&id<=-9932)e.variables[id+9943]=raw;
  else globals[id]={raw,!isFloat};
}
void storeInt(Context& c,Enemy& e,int id,i32 value){
  if(id>=0&&id<1024)c.locals[id/4]=u32(value);
  else if(id>=-9985&&id<=-9978)e.variables[id+9985]=u32(value);
  else globals[id]={u32(value),true};
}
std::string stringArg(Context& c,int at=0){const auto* p=c.pc+16+at*4;const u32 n=rd<u32>(p);if(n>4096)return {};return std::string(reinterpret_cast<const char*>(p+4),strnlen(reinterpret_cast<const char*>(p+4),n));}
std::array<u32,256> callArguments(Context& c,Enemy& e,int skipped=0){
  std::array<u32,256> out{};const u32 length=rd<u32>(c.pc+16);const u8* descriptors=c.pc+16+length+4+skipped*4;
  const u32 count=c.pc[11];for(u32 i=skipped+1,slot=0;i<count&&slot<256;i++,slot++,descriptors+=8){
    const bool f=descriptors[0]=='f'||descriptors[0]=='g';u32 raw=rd<u32>(descriptors+4);Value value{raw,!f};
    if(i<16&&(rd<u16>(c.pc+8)&(1<<i))){int id=f?nativeInt(asfloat(raw)):i32(raw);if(id==-1)value=c.popValue();else if(id<0)value=f?Value{asbits(global(e,id,true)),false}:Value{u32(globalInt(e,id)),true};else if(id<1024)value={c.locals[id/4],!f};else value={};}
    out[slot]=descriptors[1]=='f'?asbits(value.floating()):u32(value.integral());
  }return out;
}
void runContext(Enemy&,Context&);
void dropItems(Enemy&);
void emit(Enemy&,Shooter&);
struct BulletWorld : eb::World {
  void emit(const eb::Emission& event)override{Enemy origin;origin.x=event.x;origin.y=event.y;Shooter s;s.angle=event.angle;s.spacing=event.spacing;s.speed=event.speed;s.slow=event.slow;s.type=event.type;s.color=event.color;s.count=event.count;s.layers=event.layers;s.mode=event.mode;s.extensions=event.program;th12::emit(origin,s);}
  void unsupported(uint32_t)override{++th12::unsupported[509];}
  void sound(int sound,float)override{if(sound>=0)eventBits|=1;}
  static uint64_t key(u32 id){return 0x500000000ull+id;}
  void appearance(eb::State& state,bool initial)override{
    const u32 id=state.id;const auto remap=[id](int sprite){for(const auto& b:bullets)if(!b.friendly&&b.entityId==id){int result=0;if(bullet_visuals::resolve(b.enemy.type,b.enemy.color,sprite,result))return result;++th12::unsupported[984];return -1;}return sprite;};
    // During synchronous birth the projectile is not yet in its manager. The
    // binding callback therefore captures the resolved birth appearance.
    const int type=state.type,color=state.color;const auto birthRemap=[type,color](int sprite){int result=0;if(bullet_visuals::resolve(type,color,sprite,result))return result;++th12::unsupported[984];return -1;};
    if(initial)animationScene.bind(key(id),0x10000000u+id,0,state.animation,anm_logic::Membership::Embedded,state.x,state.y,birthRemap,false,nullptr,224,16,0,true);
    else if(auto* node=animationScene.find(key(id))){node->spriteRemap=birthRemap;animationScene.rebind(key(id),0,state.animation);}
    (void)remap;
  }
  bool stateAnimation(eb::State& state,eb::AnimationEvent event,int argument)override{
    const auto root=key(state.id);switch(event){
      case eb::AnimationEvent::Interrupt:animationScene.interrupt(root,argument);return false;
      case eb::AnimationEvent::Retire:animationScene.remove(root);return false;
      case eb::AnimationEvent::CancelEffect:{const u32 serial=nextDetachedAnimation++;animationScene.bind(0x100000000ull+serial,0,0,argument,anm_logic::Membership::Primary,state.x,state.y);return false;}
      case eb::AnimationEvent::BirthTick:case eb::AnimationEvent::BulletTick:
        animationScene.position(root,state.x,state.y);if(auto* node=animationScene.find(root)){auto& pose=node->vm.control();if(state.rotateToMotion)pose.rotation=state.angle+pi/2;animationScene.tickEmbedded(root);return node->vm.state().ended;}
    }return false;
  }
  int animationInteger(const eb::State& state,int index,int fallback)override{if(auto* node=animationScene.find(key(state.id));node&&index>=0&&index<4)return node->vm.state().integers[index];return fallback;}
  bool ownsStateAnimation(const eb::State& state)const override{return const_cast<anm_logic::Scene&>(animationScene).find(key(state.id))!=nullptr;}
} bulletWorld;
Enemy* spawn(const std::string& sub,float x,float y,int life,int points,int drop,bool mirror=false,const Enemy* parent=nullptr){
  const auto* pc=program.find(sub);if(!pc||enemies.size()>1024){unsupported[999]++;return nullptr;}
  auto p=std::make_unique<Enemy>();p->id=nextEnemyId++;p->x=x;p->y=y;p->ax=enemy_motion::quantize(x);p->ay=enemy_motion::quantize(y);combinePosition(*p);p->life=p->maxLife=life;p->score=points;p->drop=drop;p->mirror=mirror;if(parent)p->variables=parent->variables;
  p->boss=sub.find("Boss")!=std::string::npos&&sub.find("At")==std::string::npos;
  p->hidden=sub=="main"||sub.find("Logo")!=std::string::npos||sub.find("Shadow")!=std::string::npos||sub.find("Maple")!=std::string::npos||sub.find("EtBreak")!=std::string::npos;
  p->contexts.emplace_back(pc);Enemy* result=p.get();enemyOutside(*result);runContext(*result,result->contexts.front());
  // 412990 executes a complete birth update, then sets 20000. The next
  // 413840 call only clears that guard. Birth motion has zero initial speed.
  ++result->age;++result->phaseAge;if(result->immunityTicks>0)--result->immunityTicks;result->invincible=bool(result->flags&17)||result->immunityTicks>0;result->flags|=0x20000u;
  // Registration follows the synchronous constructor. Births made by its ECL
  // are already in the manager's chain when this node is appended.
  enemies.push_back(std::move(p));return result;
}
ufo_system::EnemyHooks ufoHooks(){return {
  [](const ufo_system::EnemySpawn& request)->u32{auto* e=spawn(request.routine,request.position.x,request.position.y,request.health,0,0);if(e)e->isUfo=true;return e?e->id:0;},
  [](u32 id)->ufo_system::EnemyView{for(const auto& p:enemies)if(p->id==id)return {p->active,p->id,{p->x,p->y},p->life};return {};},
  [](u32 id,int index,int value){for(auto& p:enemies)if(p->id==id&&index>=0&&index<4)p->variables[index]=u32(value);}
};}
void applyUfoPlan(const ufo_system::RewardPlan& plan){for(const auto& spawn:plan.spawns)itemManager.spawn(spawn);if(plan.filledNow)eventBits|=64;}
void emit(Enemy& e,Shooter& s){
  const int count=std::clamp(s.count,0,120),layers=std::clamp(s.layers,0,20);
  const float originX=float(double(s.fixedOrigin?s.originX:e.x)+s.x),originY=float(double(s.fixedOrigin?s.originY:e.y)+s.y),aimed=aim(originX,originY);
  const auto between=[](float high,float low){const float delta=float(double(high)-low),unit=random.unit(),product=float(double(unit)*delta);return float(double(low)+product);};
  for(int layer=0;layer<layers;layer++)for(int j=0;j<count;j++){
    if(bullets.size()>16000)return;
    float a=0,speed=layers>1?float(double(s.speed)-(double(s.speed)-s.slow)*layer/layers):s.speed;
    switch(s.mode){
    case 0:case 1:{a=(count&1)?float(double((j+1)/2)*s.spacing):float(double(j/2)*s.spacing+double(s.spacing)*.5);if(j&1)a=-a;if(s.mode==0)a=float(double(a)+aimed);a=float(double(s.angle)+a);break;}
    case 2:case 3:{const float center=s.mode==2?aimed:0,ring=float(double(j)*tau/count+center);a=float(double(s.spacing)*layer+s.angle+ring);break;}
    case 4:case 5:{const float center=s.mode==4?aimed:0,half=float(double(pi)/count+center),ring=float(double(j)*tau/count+half);a=float(double(s.spacing)*layer+s.angle+ring);break;}
    case 6:a=between(s.angle,s.spacing);break;
    case 7:{speed=between(s.speed,s.slow);const float ring=float(double(j)*tau/count);a=float(double(s.spacing)*layer+s.angle+ring);break;}
    case 8:a=between(s.angle,s.spacing);speed=between(s.speed,s.slow);break;
    default:unsupported[507]++;break;
    }
    const float x=float(double(originX)+std::cos(double(a))*s.radius),y=float(double(originY)+std::sin(double(a))*s.radius);
    Projectile projectile{x,y,normalize(a),speed,s.type==11?4.f:3.f,s.type,s.color};projectile.entityId=nextProjectileId++;
    eb::initialize(projectile.enemy,projectile.entityId,s.extensions,s.type,s.color,x,y,normalize(a),speed,bulletWorld);projectile.x=projectile.enemy.x;projectile.y=projectile.enemy.y;bullets.push_back(projectile);
  }eventBits|=1;
}
void resetPhase(Enemy& e,const std::string& sub){e.contexts.clear();e.contexts.emplace_back(program.find(sub));e.phaseAge=e.age=0;runContext(e,e.contexts.front());}
void command(Enemy& e,Context& c,int op){
  auto I=[&](int i){return argInt(c,e,i);};auto F=[&](int i){return arg(c,e,i,true);};
  const int sid=op>=500&&op<536?std::clamp(I(0),0,15):0;auto& s=e.shooters[sid];
  switch(op){
  case 256:case 257:case 260:case 261:case 265:case 266:case 267:case 268:case 280:{
    if(op>=265&&op<=268&&primaryBoss())break;
    const std::string name=stringArg(c);const int at=(rd<u32>(c.pc+16)+4)/4;
    float x=arg(c,e,at,true,1),y=arg(c,e,at+1,true,2);
    const int life=argInt(c,e,at+2,3),points=argInt(c,e,at+3,4),drop=argInt(c,e,at+4,5);
    if(op==256||op==260||op==265||op==267||op==280){x=float(double(x)+e.x);y=float(double(y)+e.y);}
    spawn(name,x,y,life,points,drop,op==260||op==261||op==267||op==268,&e);break;}
  case 258:e.bank=I(0);break;
  case 259:case 262:{const int slot=I(0),script=I(1);if(slot==0){e.baseAnimation=e.animation=script;e.boundBank=e.bank;e.animationAge=0;}bindEnemyAnimation(e,slot,script);break;}
  case 269:{const int slot=I(0),script=e.baseAnimation+5;if(slot==0){e.animation=script;e.boundBank=e.bank;e.animationAge=0;}bindEnemyAnimation(e,slot,script);break;}
  case 274:{const int slot=I(0),script=e.baseAnimation+I(1)+5;if(slot==0){e.animation=script;e.boundBank=e.bank;e.animationAge=0;}bindEnemyAnimation(e,slot,script);break;}
  case 276:e.animation=e.baseAnimation;e.boundBank=e.bank;e.animationAge=0;bindEnemyAnimation(e,0,e.baseAnimation);break;
  case 263:case 264:detachedAnimation(I(0),I(1),e,op==263);break;
  case 275:break;
  case 300:case 302:{const float x=F(0),y=F(1);float& xx=op==300?e.ax:e.rx;float& yy=op==300?e.ay:e.ry;if(x>-999999)xx=x;if(y>-999999)yy=y;combinePosition(e);break;}
  case 301:case 303:{const float x=F(2),y=F(3);const int duration=I(0),mode=I(1);const bool rel=op==303;
    (rel?e.relativeCircle:e.absoluteCircle).enabled=false;
    int& ticks=rel?e.relativePositionTicks:e.positionTicks;ticks=std::max(0,duration);
    (rel?e.relativePositionTime:e.positionTime)=0;(rel?e.relativePositionMode:e.positionMode)=mode;
    (rel?e.relativeFromX:e.fromX)=rel?e.rx:e.ax;(rel?e.relativeFromY:e.fromY)=rel?e.ry:e.ay;
    (rel?e.relativeToX:e.toX)=x<=-999999?(rel?e.rx:e.ax):x;(rel?e.relativeToY:e.toY)=y<=-999999?(rel?e.ry:e.ay):y;break;}
  case 304:case 306:{float angle=F(0);const float speed=F(1);const bool rel=op==306;
    (rel?e.relativeCircle:e.absoluteCircle).enabled=false;
    if(angle>-999999)(rel?e.relativeAngle:e.angle)=normalize(e.mirror?mirrorAngle(angle):angle);
    if(speed>-999999)(rel?e.relativeSpeed:e.speed)=speed;break;}
  case 305:case 307:{float angle=F(2);const float speed=F(3);const int duration=I(0),mode=I(1);const bool rel=op==307;
    (rel?e.relativeCircle:e.absoluteCircle).enabled=false;
    int& ticks=rel?e.relativePolarTicks:e.polarTicks;ticks=std::max(0,duration);(rel?e.relativePolarTime:e.polarTime)=0;(rel?e.relativePolarMode:e.polarMode)=mode;
    float& fromA=rel?e.relativeFromA:e.fromA;float& toA=rel?e.relativeToA:e.toA;float& fromS=rel?e.relativeFromS:e.fromS;float& toS=rel?e.relativeToS:e.toS;
    fromA=rel?e.relativeAngle:e.angle;fromS=rel?e.relativeSpeed:e.speed;
    toA=angle<=-999999?fromA:e.mirror?mirrorAngle(angle):angle;toS=speed<=-999999?fromS:speed;
    if(std::fabs(fromA-toA)>=pi){if(toA<=fromA)toA+=tau;else fromA+=tau;}break;}
  case 308:case 310:{const bool rel=op==310;auto& m=rel?e.relativeCircle:e.absoluteCircle;
    if(!m.enabled){m.centerX=rel?e.rx:e.ax;m.centerY=rel?e.ry:e.ay;}m.enabled=true;
    const float angle=F(0),omega=F(1),radius=F(2),radial=F(3);
    if(angle>-999999)(rel?e.relativeAngle:e.angle)=normalize(angle);if(omega>-999999)(rel?e.relativeSpeed:e.speed)=omega;
    if(radius>-999999)m.radius=radius;if(radial>-999999)m.radialSpeed=radial;circularPosition(e,rel);combinePosition(e);break;}
  case 309:case 311:{const bool rel=op==311;auto& m=rel?e.relativeCircle:e.absoluteCircle;m.ticks=std::max(0,I(0));m.time=0;m.mode=I(1);
    m.fromOmega=rel?e.relativeSpeed:e.speed;m.fromRadius=m.radius;m.fromRadial=m.radialSpeed;
    const float omega=F(2),radius=F(3),radial=F(4);m.toOmega=omega<=-999999?m.fromOmega:omega;m.toRadius=radius<=-999999?m.fromRadius:radius;m.toRadial=radial<=-999999?m.fromRadial:radial;break;}
  case 312:case 313:{
    const float randomAngle=random.signedUnit()*pi;float angle;
    if(e.x<e.clampX-e.clampWidth*.25f)angle=randomAngle/3;
    else if(e.x>e.clampX+e.clampWidth*.25f)angle=normalize(randomAngle/3+pi);
    else if(e.x<px/128.f)angle=randomAngle*.5f;else angle=normalize(randomAngle*.5f+pi);
    if(e.y<e.clampY-e.clampHeight*.25f)angle=std::fabs(angle);else if(e.y>e.clampY+e.clampHeight*.25f)angle=-std::fabs(angle);
    const int mode=I(1);const float speed=F(2);const int duration=I(0);const bool rel=op==313;
    (rel?e.relativePolarTicks:e.polarTicks)=std::max(0,duration);(rel?e.relativePolarTime:e.polarTime)=0;(rel?e.relativePolarMode:e.polarMode)=mode;
    (rel?e.relativeFromA:e.fromA)=(rel?e.relativeToA:e.toA)=angle;(rel?e.relativeFromS:e.fromS)=speed;(rel?e.relativeToS:e.toS)=0;break;}
  case 314:case 315:{if(auto* boss=primaryBoss()){if(op==314){e.ax=boss->x;e.ay=boss->y;}else{e.rx=boss->x;e.ry=boss->y;}combinePosition(e);}break;}
  case 324:e.mirror=I(0)&1;break;
  case 400:e.hitWidth=F(0);e.hitHeight=F(1);e.hit=std::max(e.hitWidth,e.hitHeight)*.5f;break;
  case 404:e.clampX=F(0);e.clampY=F(1);e.clampWidth=F(2);e.clampHeight=F(3);e.flags|=0x10000u;break;
  case 402:e.flags|=u32(I(0));e.hidden=e.hidden||bool(e.flags&32);e.invincible=bool(e.flags&17)||e.immunityTicks>0;break;
  case 403:e.flags&=~u32(I(0));e.invincible=bool(e.flags&17)||e.immunityTicks>0;break;
  case 401:e.bodyWidth=F(0);e.bodyHeight=F(1);break;
  case 405:e.flags&=~0x10000u;break;
  case 406:e.dropCounts.fill(0);break;
  case 407:{const int type=I(0),count=I(1);if(type==0)e.drop=count;else if(type>=1&&type<=19)e.dropCounts[type-1]=count;else unsupported[op]++;break;}
  case 408:e.dropSpreadX=F(0);e.dropSpreadY=F(1);break;
  case 409:dropItems(e);break;
  case 410:e.drop=I(0);break;
  case 411:e.life=e.maxLife=I(0);break;
  case 412:e.boss=I(0)>=0;break;
  case 413:e.phaseAge=e.age=0;break;
  case 414:{const int timeout=I(2),health=I(1),slot=I(0);if(slot>=0&&slot<8){auto& interrupt=e.interrupts[slot];interrupt.health=health;if(health>=0){interrupt.time=timeout;interrupt.healthScript=interrupt.timeoutScript=stringArg(c,3);}}break;}
  case 415:e.immunityTicks=std::max(0,I(0));e.invincible=bool(e.flags&17)||e.immunityTicks>0;break;
  case 416:case 417:break;
  case 418:{if(messageVM.start(messageFiles[character*2+shot],I(0))){dialogue=I(0)+1;messageEvents.clear();startMessageAnimations();}else unsupported[418]++;break;}
  case 419:if(dialogue>0){c.time-=1;c.pc-=rd<u16>(c.pc+6);}break;
  case 420:if(primaryBoss()){c.time-=1;c.pc-=rd<u16>(c.pc+6);}break;
  case 421:{const int slot=I(0);if(slot>=0&&slot<8)e.interrupts[slot].timeoutScript=stringArg(c,1);break;}
  case 422:case 428:case 437:case 438:case 439:{
    spell_system::Start start;start.cardId=spell_system::resolveCardId(op,I(0),difficulty);start.timeLimit=I(1);(void)I(2);
    start.difficulty=difficulty;start.stage=stageNumber;start.bombActive=playerBomb.active;start.bossX=e.x;start.bossY=e.y;
    const int length=rd<i32>(c.pc+28);if(length>=0&&length<=int(rd<u16>(c.pc+6))-32)start.title=spell_system::decryptTitle(c.pc+32,length);
    if(!spellState.start(start))++unsupported[op];else startSpellAnimations();spell=spellState.active();break;}
  case 423:endSpellAnimations(spellState.end(score));backgroundVisible=true;stageState.flags|=1u;spell=spellState.active();break;
  case 424:case 425:case 426:case 427:break;
  case 435:{int id=rd<i32>(c.pc+16);storeInt(c,e,id,argInt(c,e,std::min(difficulty,3)+1));break;}
  case 436:{int id=int(rd<float>(c.pc+16));store(c,e,id,arg(c,e,std::min(difficulty,3)+1,true),true);break;}
  case 434:{const int low=I(1),high=I(2),rank=std::clamp(resources.rank,-1024,1024);storeInt(c,e,rd<i32>(c.pc+16),i32(u32(low)+u32(i32(i32(u32(high)-u32(low))*u32(rank+1024))/2048)));break;}
  case 448:c.time-=I(std::min(difficulty,3));break;
  case 442:spellState.setSurvival();break;
  case 443:spellState.markTimeoutRing();animationScene.remove(spellAnimationKeys[3]);spellAnimationKeys[3]=0;break;
  case 440:case 445:case 449:break;
  case 500:s=Shooter();break;
  case 501:emit(e,s);break;
  case 502:s.type=I(1);s.color=I(2);break;
  case 503:s.x=F(1);s.y=F(2);break;
  case 504:s.angle=F(1);s.spacing=F(2);break;
  case 505:s.speed=F(1);s.slow=F(2);break;
  case 506:s.count=I(1);s.layers=I(2);break;
  case 507:s.mode=I(1);break;
  case 511:e.shooters[sid]=e.shooters[std::clamp(I(1),0,15)];break;
  case 508:s.fireSound=I(1);s.extensions.transformSound=I(2);break;
  case 529:case 530:case 531:unsupported[op]++;break;
  case 509:if(!eb::set(s.extensions,I(1),I(2)!=0,u32(I(3)),I(4),I(5),F(6),F(7)))unsupported[op]++;break;
  case 510:for(auto& b:bullets)if(!b.friendly&&b.active)eb::cancel(b.enemy,bulletWorld);unsupported[600]++;break;
  case 514:case 517:{const int at=resources.rank<-512?1:resources.rank<512?3:5;if(op==514){s.speed=F(at);s.slow=F(at+1);}else{s.count=int16_t(I(at));s.layers=int16_t(I(at+1));}break;}
  case 515:case 518:{const int at=resources.rank<-600?1:resources.rank<-200?3:resources.rank<200?5:resources.rank<600?7:9;if(op==515){s.speed=F(at);s.slow=F(at+1);}else{s.count=int16_t(I(at));s.layers=int16_t(I(at+1));}break;}
  case 516:{const double t=double(resources.rank+1024)/2048;s.speed=float(double(F(1))+(double(F(3))-F(1))*t);s.slow=float(double(F(2))+(double(F(4))-F(2))*t);break;}
  case 519:{const auto interpolate=[&](i32 low,i32 high){return int16_t(low+i32(i32(u32(high-low)*u32(resources.rank+1024))/2048));};s.count=interpolate(I(1),I(3));s.layers=interpolate(I(2),I(4));break;}
  case 512:case 513:{const float radius=F(0);for(auto& b:bullets)if(!b.friendly&&b.active&&(b.enemy.phase==eb::Phase::Alive||b.enemy.phase==eb::Phase::Spawning)){const double dx=double(b.x)-e.x,dy=double(b.y)-e.y,reach=double(radius)+eb::appearances[b.enemy.type].hitbox*.5;if(dx*dx+dy*dy<=reach*reach){eb::cancel(b.enemy,bulletWorld);if(op==512&&b.x>=-192&&b.x<=192&&b.y>=0&&b.y<=448)itemManager.spawn(9,{b.x,b.y},-pi/2,.6000000238418579f);}}break;}
  case 520:{const float x=F(1),y=F(2);store(c,e,int(rd<float>(c.pc+16)),aim(x,y),true);break;}
  case 521:s.speed=F(1+std::min(difficulty,3));s.slow=F(5+std::min(difficulty,3));break;
  case 522:s.count=I(1+std::min(difficulty,3));s.layers=I(5+std::min(difficulty,3));break;
  case 523:{const float angle=F(1),radius=F(2);s.x=float(std::cos(double(angle))*radius);s.y=float(std::sin(double(angle))*radius);break;}
  case 524:s.radius=F(1);break;
  case 525:s.originX=F(1);s.originY=F(2);s.fixedOrigin=s.originX>=-990;break;
  case 526:break;
  case 527:case 528:break;
  case 600:case 601:unsupported[op]++;break; // laser configuration, not spell-card lifecycle
  case 602:case 603:case 604:case 605:case 606:case 607:case 608:case 609:case 610:case 611:unsupported[op]++;break;
  case 700:unsupported[op]++;break;
  default:if(op>=0&&op<1024)unsupported[op]++;break;
  }
}
void runContext(Enemy& e,Context& c){
  for(int budget=0;c.pc&&rd<i32>(c.pc)<=c.time&&budget<10000;budget++){
    const u8* instruction=c.pc;const int op=rd<u16>(c.pc+4),length=rd<u16>(c.pc+6);
    if(length<16){unsupported[998]++;c.pc=nullptr;break;}
    if(c.pc[10]&(1<<difficulty))switch(op){
    case 0:break;
    case 1:e.active=false;c.pc=nullptr;return;
    case 10:if(c.calls.empty()){c.pc=nullptr;return;}else{auto f=c.calls.back();c.calls.pop_back();c.pc=f.pc;c.time=f.time;c.locals=f.locals;continue;}
    case 11:{auto locals=callArguments(c,e);auto sub=stringArg(c);c.calls.push_back({c.pc+length,c.time,c.locals});c.locals=locals;c.pc=program.find(sub);c.time=0;continue;}
    case 12:case 13:case 14:{bool go=op==12;if(op!=12){const int v=int(c.pop());go=op==13?v==0:v!=0;}if(go){int delta=rd<i32>(c.pc+16);c.time=float(rd<i32>(c.pc+20));c.pc+=delta;continue;}break;}
    case 15:case 16:{Context next(program.find(stringArg(c)));next.locals=callArguments(c,e,op==16?1:0);next.id=op==16?int(arg(c,e,(rd<u32>(c.pc+16)+4)/4)):-1;e.contexts.insert(std::next(e.contexts.begin()),std::move(next));break;}
    case 17:{int id=int(arg(c,e,0));for(auto& t:e.contexts)if(t.id==id)t.pc=nullptr;break;}
    case 21:{auto i=e.contexts.begin();if(i!=e.contexts.end())++i;for(;i!=e.contexts.end();++i)i->pc=nullptr;break;}
    case 40:case 41:break;
    case 42:c.pushInt(argInt(c,e,0));break;
    case 44:c.push(arg(c,e,0,true));break;
    case 43:storeInt(c,e,rd<i32>(c.pc+16),c.popInt());break;
    case 45:{int id=int(rd<float>(c.pc+16));float v=c.pop();store(c,e,id,v,true);break;}
    case 50:case 51:case 52:case 53:case 54:case 55:case 56:case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:case 65:case 66:case 67:case 68:case 69:case 70:case 73:case 74:case 75:case 76:case 77:{
      const Value vb=c.popValue(),va=c.popValue();const float b=vb.floating(),a=va.floating();const i32 ib=vb.integral(),ia=va.integral();float v=0;bool integer=true;i32 iv=0;
      switch(op){case 50:iv=i32(u32(ia)+u32(ib));break;case 51:v=float(double(a)+b);integer=false;break;
      case 52:iv=i32(u32(ia)-u32(ib));break;case 53:v=float(double(a)-b);integer=false;break;
      case 54:iv=i32(u32(ia)*u32(ib));break;case 55:v=float(double(a)*b);integer=false;break;
      case 56:iv=ib?(ia==i32(0x80000000u)&&ib==-1?ia:ia/ib):0;break;case 57:v=b?float(double(a)/b):0;integer=false;break;case 58:iv=ib?(ia==i32(0x80000000u)&&ib==-1?0:ia%ib):0;break;
      case 59:iv=ia==ib;break;case 60:iv=a==b;break;case 61:iv=ia!=ib;break;case 62:iv=a!=b;break;case 63:iv=ia<ib;break;case 64:iv=a<b;break;case 65:iv=ia<=ib;break;case 66:iv=a<=b;break;case 67:iv=ia>ib;break;case 68:iv=a>b;break;case 69:iv=ia>=ib;break;case 70:iv=a>=b;break;
      case 73:iv=bool(ia)||bool(ib);break;case 74:iv=bool(ia)&&bool(ib);break;case 75:iv=ia^ib;break;case 76:iv=ia|ib;break;case 77:iv=ia&ib;break;}if(integer)c.pushInt(iv);else c.push(v);break;}
    case 71:c.pushInt(!c.popInt());break;case 72:c.pushInt(!c.pop());break;
    case 78:{int id=rd<i32>(c.pc+16);i32 v=argInt(c,e,0);storeInt(c,e,id,i32(u32(v)-1));c.pushInt(v);break;}
    case 79:c.push(float(std::sin(double(c.pop()))));break;
    case 80:c.push(float(std::cos(double(c.pop()))));break;
    case 81:{float angle=arg(c,e,2,true),length=arg(c,e,3,true);store(c,e,int(rd<float>(c.pc+16)),float(std::cos(double(angle))*length),true);store(c,e,int(rd<float>(c.pc+20)),float(std::sin(double(angle))*length),true);break;}
    case 82:{float v=arg(c,e,0,true);while(v>pi)v-=tau;while(v<-pi)v+=tau;store(c,e,int(rd<float>(c.pc+16)),v,true);break;}
    case 83:c.time-=arg(c,e,0);break;
    case 84:c.pushInt(i32(0u-u32(c.popInt())));break;
    // Retail op85 negates the converted float's raw word (not fchs).
    // Verified with the native VM: 3.5 becomes -1.25 in this title.
    case 85:{u32 bits=asbits(c.pop());c.stack.push_back({0u-bits,false});break;}
    case 86:{const double x=arg(c,e,1,true),y=arg(c,e,2,true);store(c,e,int(rd<float>(c.pc+16)),float(x*x+y*y),true);break;}
    case 87:{const float dx=float(double(arg(c,e,3,true))-arg(c,e,1,true)),dy=float(double(arg(c,e,4,true))-arg(c,e,2,true));store(c,e,int(rd<float>(c.pc+16)),float(std::atan2(double(dy),double(dx))),true);break;}
    case 89:{const double a=arg(c,e,1,true),b=arg(c,e,2,true);store(c,e,int(rd<float>(c.pc+16)),normalize(float(b-a)),true);break;}
    case 88:c.push(float(std::sqrt(double(c.pop()))));break;
    default:command(e,c,op);break;
    }
    if(c.pc==instruction)c.pc+=length;else if(c.pc==instruction-length)c.pc=instruction;
  }
  if(c.pc)c.time+=1;
}
void dropItems(Enemy& e){
  const auto ordinary=[](float x,float y,int type,int source){
    if(type>=1&&type<=18)itemManager.spawn(type,{x,y});else if(type!=0)unsupported[source]++;
  };
  ordinary(e.x,e.y,e.drop,410);e.drop=0;
  // Native 412140 consumes the initial angle even when all counts are zero.
  float angle=float(double(random.signedUnit())*pi);
  for(int type=1;type<=18;type++)for(int i=0;i<e.dropCounts[type-1];i++){
    const float offsetX=float(std::cos(double(angle))*e.dropSpreadX),offsetY=float(std::sin(double(angle))*e.dropSpreadY);
    const float radial=float(double(random.unit())*.5+.5);
    const float x=float(double(e.x)+float(double(offsetX)*radial)),y=float(double(e.y)+float(double(offsetY)*radial));
    ordinary(x,y,type,407);
    const float jitter=float(double(random.signedUnit())*pi);
    angle=normalize(float(double(angle)+pi/2+double(jitter)*.25));
  }
  e.dropCounts.fill(0);
}
void drop(Enemy& e){if(e.hidden)return;if(e.isUfo)applyUfoPlan(ufoManager.finish(e.id,resources));score+=e.score/10;eventBits|=2;dropItems(e);}
#include "player_gameplay.hpp"
uint64_t bombEffectKey(u32 id){return 0x600000000ull+id;}
void bindBombEffect(int profile,u32 id);
void configurePlayerAnimations(){
  friendlyBirthHook=[](Projectile& b){const auto key=0x800000000ull+b.entityId;auto* node=animationScene.bind(key,0x10000000u+b.entityId,4+character,b.animation,anm_logic::Membership::Primary,0,0,{},false,nullptr,0,0);if(node){if(node->vm.state().flags&0x20000000u){node->vm.control().rotation=b.angle;node->vm.control().flags|=4u;}animationScene.position(key,b.x+224,b.y+16);}if(b.type==2)animationScene.bind(0x900000000ull+b.entityId,0x10000000u+b.entityId,4+character,8,anm_logic::Membership::Primary,0,0,{},false,nullptr,0,0,23);};
  friendlyHitHook=[](Projectile& b){const auto key=0x800000000ull+b.entityId;float rotation=0;if(auto* old=animationScene.find(key))rotation=old->vm.state().rotation;animationScene.remove(key);auto* node=animationScene.bind(key,0x10000000u+b.entityId,4+character,b.animation,anm_logic::Membership::Primary,0,0,{},false,nullptr,0,0);if(node){node->vm.control().rotation=rotation;animationScene.position(key,b.x+224,b.y+16);}};
  friendlyInterruptHook=[](Projectile& b,int code,bool auxiliary){animationScene.interrupt((auxiliary?0x900000000ull:0x800000000ull)+b.entityId,code);};
  playerBombBindHook=[](const pw::BombVisual& v){const auto key=0x700000000ull+v.id;animationScene.bind(key,0x50000000u+v.id,4+v.bank,v.script,anm_logic::Membership::Primary,v.x,v.y,{},false,nullptr,224,16,23);};
  reimuBOrbBindHook=[](const pw::ReimuBOrb& orb){const auto key=0x700000000ull+orb.id;animationScene.bind(key,0x50000000u+orb.id,4,24,anm_logic::Membership::Primary,orb.x,orb.y,{},false,nullptr,224,16,23);};
  playerBombCustomBindHook=[](int profile,u32 id,float x,float y){bindBombEffect(profile,id);animationScene.position(bombEffectKey(id),x,y);};
}
constexpr uint64_t playerBodyKey=0xa00000000ull;
void initializePlayerAnimation(){animationScene.bind(playerBodyKey,0xffff0000u,4+character,0,anm_logic::Membership::Embedded,0,0,{},false,nullptr,0,0);animationScene.position(playerBodyKey,px/128.f+224,py/128.f+16);}
void tickPlayerAnimation(){
  if(auto* node=animationScene.find(playerBodyKey)){if(node->vm.state().script!=playerMotion.bodyAnimation)animationScene.rebind(playerBodyKey,4+character,playerMotion.bodyAnimation,false);
    animationScene.position(playerBodyKey,px/128.f+224,py/128.f+16);animationScene.tickEmbedded(playerBodyKey);
    auto& pose=node->vm.control();const bool visible=playerState!=2&&playerState!=4&&(!invuln||frame%6<3);pose.visible=visible;if(visible)pose.flags|=1u;else pose.flags&=~1u;
  }
}
void bindBombEffect(int profile,u32 id){
  if(!id||animationScene.find(bombEffectKey(id)))return;
  auto* node=animationScene.bind(bombEffectKey(id),0xa0000000u+id,0,profile==3?210:0,anm_logic::Membership::Primary,0,0,{},false,nullptr,224,16,23);
  if(!node)return;node->customGeometry=profile==3;
  animationScene.custom(bombEffectKey(id),[profile,id](){const bool retired=tickPlayerBombEffect(profile,id);if(profile==4)for(const auto& beam:playerBomb.reimuABeams)if(beam.active)bindBombEffect(3,beam.id);return retired;});
}
void syncBombAnimations(){
  for(const auto& visual:playerBomb.visuals)if(visual.active){const auto key=0x700000000ull+visual.id;auto* node=animationScene.find(key);
    if(!node){node=animationScene.bind(key,0x50000000u+visual.id,4+visual.bank,visual.script,anm_logic::Membership::Primary,visual.x,visual.y,{},false,nullptr,224,16,23);if(node&&visual.interrupt)animationScene.interrupt(key,visual.interrupt);}
    else {animationScene.position(key,visual.x,visual.y);if(visual.interrupt&&node->vm.diagnostics().pendingInterrupt!=visual.interrupt&&!node->vm.state().ended){if(visual.age==0)animationScene.interrupt(key,visual.interrupt);}}
  }
  if(playerBomb.loadout==0&&playerBomb.reimuAEmitter.active)bindBombEffect(4,playerBomb.reimuAEmitter.id);
  for(const auto& leaf:playerBomb.leaves)if(leaf.active)bindBombEffect(5,leaf.id);
}
void fire(){
  const auto& s=loadouts[character*2+shot];const int level=std::clamp(power/s.powerStep,0,s.maxLevel),index=level+((previousHeld&8)?s.maxLevel+1:0);
  if(index>=int(s.groups.size()))return;
  for(const auto& spec:s.groups[index])if(spec.interval>0&&fireFrame%spec.interval==spec.delay){
    if(std::count_if(bullets.begin(),bullets.end(),[](const Projectile& p){return p.friendly&&p.active;})>=256)break;
    if(spec.option>playerMotion.count)continue;
    if(spec.type==2&&std::any_of(bullets.begin(),bullets.end(),[&](const Projectile& b){return b.active&&b.friendly&&b.type==2&&b.option==spec.option;}))continue;
    float x=px/128.f,y=py/128.f;
    if(spec.option){const auto& o=playerMotion.options[spec.option-1];x=o.x/128.f;y=o.y/128.f;}
    // Native 439896 subtracts the initial velocity; its update occurs this tick.
    Projectile b{x+spec.x-float(std::cos(double(spec.angle))*spec.speed),y+spec.y-float(std::sin(double(spec.angle))*spec.speed),spec.angle,spec.speed,std::max(3.f,spec.w*.5f),spec.type,0,0,spec.damage,spec.animation+5,true};
    b.option=spec.option;b.update=spec.update;b.hitWidth=spec.w;b.hitHeight=spec.h;b.extraCallback=spec.extraCallback;
    if(spec.spawn==2)b.hitHeight=0;
    b.entityId=nextProjectileId++;initializeFriendlyProjectile(b,spec);bullets.push_back(b);
  }
}
void tick(int held,int pressed){
  if(ended)return;eventBits=0;const auto& s=loadouts[character*2+shot];const bool focus=held&8;
  animationScene.tickSecondary();
  if(stageState.program&&!stageProgram.commands.empty()){if(!stageState.tick(stageWorld))++unsupported[982];backgroundFrame=stageState.callbackCount;}else ++backgroundFrame;
  if(invuln>0)--invuln;
  const int speed=int((focus?s.focus:s.speed)*playerBomb.speedMultiplier*128),diagonal=int((focus?s.focusDiag:s.diag)*playerBomb.speedMultiplier*128);
  int dir=0;if((held&0x50)==0x50)dir=5;else if((held&0x60)==0x60)dir=7;else if((held&0x90)==0x90)dir=6;else if((held&0xa0)==0xa0)dir=8;else if(held&0x20)dir=2;else if(held&0x10)dir=1;else if(held&0x40)dir=3;else if(held&0x80)dir=4;
  const int beforeX=px;
  if(playerState==1){switch(dir){case 1:py-=speed;break;case 2:py+=speed;break;case 3:px-=speed;break;case 4:px+=speed;break;case 5:px-=diagonal;py-=diagonal;break;case 6:px+=diagonal;py-=diagonal;break;case 7:px-=diagonal;py+=diagonal;break;case 8:px+=diagonal;py+=diagonal;break;}}
  playerMotion.body(px-beforeX);
  px=std::clamp(px,-0x5c00,0x5c00);if(playerState==1)py=std::clamp(py,0x1000,0xd800);
  playerMotion.updateOptions(s,power,px,py,focus);
  tickPlayerAnimation();
  bulletWorld.playerX=px/128.f;bulletWorld.playerY=py/128.f;
  if(!(oracleFixtureFlags&4)&&(pressed&2)&&bombs>0&&(playerState==1||playerState==4)&&startPlayerBomb()){syncBombAnimations();--bombs;deathWindow=0;playerState=1;playerStateTicks=60;spellState.onBomb();eventBits|=4;}
  if(playerState==4){deathWindow=std::max(0,8-playerStateTicks);if(playerStateTicks>=8){spellState.onMiss(playerBomb.active);--lives;resources.addRank(-1024);playerState=2;playerStateTicks=0;deathWindow=0;invuln=180;}}
  if(playerState==2){
    if(playerStateTicks==3){const auto plan=item_system::deathPowerDrops(resources,{px/128.f,py/128.f});for(const auto& request:plan.spawns)itemManager.spawn(request);}
    if(playerStateTicks>=30){if(lives<0)ended=2;else{playerState=0;playerStateTicks=0;px=0;py=480*128;invuln=280;if(bombs<2){bombs=2;resources.bombFragments=0;}}}
  }else if(playerState==0){py=0xf000-(playerStateTicks*0x2800)/60;if(playerStateTicks>=30)for(auto& b:bullets)if(!b.friendly&&b.active)eb::cancel(b.enemy,bulletWorld);if(playerStateTicks>=60){playerState=1;playerStateTicks=0;py=400*128;}}
  ++playerStateTicks;
  previousHeld=held;fireFrame=shotSchedule.begin(held&1,(playerState==1||playerState==0)&&!dialogue);
  if(fireFrame>=0)fire();shotSchedule.finish(held&1);
  tickPlayerDamageSources();
  for(size_t i=0;i<bullets.size();i++){auto& b=bullets[i];if(!b.active||!b.friendly)continue;updateFriendlyProjectile(b);if(!b.active)continue;++b.age;if(b.type!=2){b.x=float(double(b.x)+float(std::cos(double(b.angle))*b.speed));b.y=float(double(b.y)+float(std::sin(double(b.angle))*b.speed));}b.x=enemy_motion::quantize(b.x);b.y=enemy_motion::quantize(b.y);if(b.x<-280||b.x>280||b.y<-100||b.y>520)b.active=false;}
  for(const auto& b:bullets)if(b.friendly){const auto key=0x800000000ull+b.entityId;if(!b.active){animationScene.remove(key);if(b.type==2)animationScene.remove(0x900000000ull+b.entityId);}else{animationScene.position(key,b.x+224,b.y+16);if(b.type==2)animationScene.position(0x900000000ull+b.entityId,b.x+224,b.y+16);if(auto* node=animationScene.find(key)){auto& pose=node->vm.control();if(pose.flags&0x20000000u)pose.rotation=b.angle;}}}
  tickPlayerBomb();bombTimer=playerBomb.active?playerBomb.age:0;
  syncBombAnimations();
  for(auto& b:bullets)if(b.active&&!b.friendly&&(b.enemy.phase==eb::Phase::Alive||b.enemy.phase==eb::Phase::Spawning)&&!b.enemy.protect.timer){
    bool clear=false;for(const auto& a:playerBomb.rectangles)if(std::abs(b.x-a.x)<=a.width*.5f&&std::abs(b.y-a.y)<=a.height*.5f)clear=true;
    for(const auto& a:playerBomb.circles){const float r=float(double(a.radius)+eb::appearances[b.enemy.type].hitbox*.5f);const double x=double(b.x)-a.x,y=double(b.y)-a.y;if(x*x+y*y<=double(r)*r)clear=true;}
    if(clear){eb::cancel(b.enemy,bulletWorld);if(!spell&&b.x>=-192&&b.x<=192&&b.y>=0&&b.y<=448)itemManager.spawn(9,{b.x,b.y},-pi/2,.6000000238418579f);}
  }
  for(size_t i=0;i<enemies.size();){
    auto& e=*enemies[i];
    // 413190 saves this node's successor before updating it. A birth appended
    // behind an existing successor is visited, but a birth at the old tail is
    // deferred until the next manager pass.
    i=i+1<enemies.size()?i+1:size_t(-1);if(!e.active)continue;
    if(e.flags&0x20000u){e.flags&=~0x20000u;continue;}
    e.invincible=bool(e.flags&17)||e.immunityTicks>0;
    if(e.polarTicks){float t=easing(float(++e.polarTime)/e.polarTicks,e.polarMode);e.angle=normalize(e.fromA+(e.toA-e.fromA)*t);e.speed=e.fromS+(e.toS-e.fromS)*t;if(e.polarTime>=e.polarTicks)e.polarTicks=0;}
    if(e.relativePolarTicks){float t=easing(float(++e.relativePolarTime)/e.relativePolarTicks,e.relativePolarMode);e.relativeAngle=normalize(e.relativeFromA+(e.relativeToA-e.relativeFromA)*t);e.relativeSpeed=e.relativeFromS+(e.relativeToS-e.relativeFromS)*t;if(e.relativePolarTime>=e.relativePolarTicks)e.relativePolarTicks=0;}
    for(int relative=0;relative<2;relative++){auto& m=relative?e.relativeCircle:e.absoluteCircle;if(m.ticks){float t=easing(float(++m.time)/m.ticks,m.mode);(relative?e.relativeSpeed:e.speed)=m.fromOmega+(m.toOmega-m.fromOmega)*t;m.radius=m.fromRadius+(m.toRadius-m.fromRadius)*t;m.radialSpeed=m.fromRadial+(m.toRadial-m.fromRadial)*t;if(m.time>=m.ticks)m.ticks=0;}}
    if(e.positionTicks){float t=easing(float(++e.positionTime)/e.positionTicks,e.positionMode);e.ax=e.fromX+(e.toX-e.fromX)*t;e.ay=e.fromY+(e.toY-e.fromY)*t;if(e.positionTime>=e.positionTicks)e.positionTicks=0;}
    else if(e.absoluteCircle.enabled){e.angle=normalize(float(double(e.angle)+e.speed));e.absoluteCircle.radius=float(double(e.absoluteCircle.radius)+e.absoluteCircle.radialSpeed);circularPosition(e,false);}
    else {e.ax+=float(std::cos(double(e.angle))*e.speed);e.ay+=float(std::sin(double(e.angle))*e.speed);}
    if(e.relativePositionTicks){float t=easing(float(++e.relativePositionTime)/e.relativePositionTicks,e.relativePositionMode);e.rx=e.relativeFromX+(e.relativeToX-e.relativeFromX)*t;e.ry=e.relativeFromY+(e.relativeToY-e.relativeFromY)*t;if(e.relativePositionTime>=e.relativePositionTicks)e.relativePositionTicks=0;}
    else if(e.relativeCircle.enabled){e.relativeAngle=normalize(float(double(e.relativeAngle)+e.relativeSpeed));e.relativeCircle.radius=float(double(e.relativeCircle.radius)+e.relativeCircle.radialSpeed);circularPosition(e,true);}
    else {e.rx+=float(std::cos(double(e.relativeAngle))*e.relativeSpeed);e.ry+=float(std::sin(double(e.relativeAngle))*e.relativeSpeed);}
    e.ax=enemy_motion::quantize(e.ax);e.ay=enemy_motion::quantize(e.ay);e.rx=enemy_motion::quantize(e.rx);e.ry=enemy_motion::quantize(e.ry);combinePosition(e);
    if(e.flags&0x10000u){e.x=std::clamp(e.x,e.clampX-e.clampWidth*.5f,e.clampX+e.clampWidth*.5f);e.y=std::clamp(e.y,e.clampY-e.clampHeight*.5f,e.clampY+e.clampHeight*.5f);e.ax=float(double(e.x)-e.rx);e.ay=float(double(e.y)-e.ry);}
    for(int slot=0;slot<16;slot++)if(e.animationBound[slot])animationScene.position(enemyAnimationKey(e.id,slot),e.x,e.y);
    if(enemyOutside(e)){e.active=false;continue;}
    // Snapshot existing contexts; asynchronous children start on a later tick.
    std::vector<Context*> existing;for(auto& c:e.contexts)existing.push_back(&c);for(auto* c:existing)if(c->pc)runContext(e,*c);
    if(!(oracleFixtureFlags&1)&&e.active&&!e.hidden&&!e.invincible){int damage=playerSourceDamage(e)+playerBombDamage(e);for(size_t j=0;j<bullets.size();j++){auto& b=bullets[j];if(b.active&&b.friendly)damage+=friendlyProjectileDamage(b,e);}damage=std::min(80,damage);if(spellState.pendingBomb())damage/=30;if(damage>0){e.life-=damage;eventBits|=8;}}
    ++e.age;++e.phaseAge;++e.animationAge;
    EnemyInterrupt* transition=nullptr;bool timeout=false;
    for(auto& interrupt:e.interrupts)if(interrupt.health>=0){if(e.life<=interrupt.health)transition=&interrupt;break;}
    if(!transition)for(auto& interrupt:e.interrupts)if(interrupt.health>=0&&interrupt.time>=0&&e.phaseAge>=interrupt.time){transition=&interrupt;timeout=true;break;}
    if(transition){const std::string next=timeout?transition->timeoutScript:transition->healthScript;e.life=transition->health;transition->health=-1;e.timedOut=timeout;if(timeout)spellState.onTimeout(playerBomb.active);resetPhase(e,next);}
    else if(e.life<=0){drop(e);e.active=false;}
    if(!(oracleFixtureFlags&2)&&e.active&&!(e.flags&(2|32|0x2000000))&&!e.bodyImmunityTicks&&!dialogue&&playerState!=2&&playerState!=3&&playerState!=4){
      const float dx=float(double(e.x)-px/128.f),dy=float(double(e.y)-py/128.f),radius=e.bodyWidth*.5f,hit=eb::playerHitboxes[character];
      if(float(double(dx)*dx+double(dy)*dy)<float(double(radius)*radius+double(hit)*hit)&&!invuln&&playerState==1){playerState=4;playerStateTicks=0;deathWindow=8;eventBits|=32;}
    }
    if(e.immunityTicks>0)--e.immunityTicks;if(e.bodyImmunityTicks>0)--e.bodyImmunityTicks;e.invincible=bool(e.flags&17)||e.immunityTicks>0;
  }
  const auto hooks=ufoHooks();ufoManager.tick({power,stageNumber,primaryBoss()!=nullptr,dialogue>0,1},hooks);
  bulletWorld.playerX=px/128.f;bulletWorld.playerY=py/128.f;
  for(size_t i=0;i<bullets.size();i++){auto& b=bullets[i];if(!b.active||b.friendly)continue;eb::tick(b.enemy,bulletWorld,true);b.x=b.enemy.x;b.y=b.enemy.y;b.angle=b.enemy.angle;b.speed=b.enemy.speed;b.age=b.enemy.age;b.active=b.enemy.phase!=eb::Phase::Inactive;
    if(!(oracleFixtureFlags&2)&&playerState!=2&&b.active){const auto collision=eb::collide(b.enemy,px/128.f,py/128.f,eb::playerHitboxes[character]);if(collision==eb::Collision::Graze&&!b.enemy.grazed){++graze;b.enemy.grazed=true;eventBits|=16;}if(collision==eb::Collision::Hit){eb::cancel(b.enemy,bulletWorld);if(!invuln&&playerState==1){playerState=4;playerStateTicks=0;deathWindow=8;eventBits|=32;}}}
    if(b.enemy.phase!=eb::Phase::Inactive)eb::finishAnimation(b.enemy,bulletWorld);b.active=b.enemy.phase!=eb::Phase::Inactive;
  }
  item_system::FrameView itemFrame;itemFrame.player=item_system::PlayerView::native(character,{px/128.f,py/128.f},focus,playerState);itemFrame.ufo=ufoManager.view(hooks);itemFrame.dialogue=dialogue>0;itemFrame.inventoryColors=&ufoManager.inventory.colors;itemFrame.inventoryCount=&ufoManager.inventory.count;
  itemManager.tick(itemFrame,[&](const item_system::Event& event){using K=item_system::EventKind;switch(event.kind){case K::CollectOrdinary:item_system::collectOrdinary(resources,event.entity,itemFrame.player);eventBits|=64;break;case K::CollectAggregate:item_system::collectAggregate(resources,event.entity);eventBits|=64;break;case K::CollectToken:applyUfoPlan(ufoManager.collectToken(event.value,resources));eventBits|=64;break;case K::Absorbed:applyUfoPlan(ufoManager.absorb(event.entity.type,stageNumber));break;case K::RankDelta:resources.addRank(event.value);break;default:break;}});
  auto* spellBoss=primaryBoss();const auto spellTick=spellState.tick(playerBomb.active,py/128.f,spellBoss?spellBoss->x:0,spellBoss?spellBoss->y:0);if(!spellTick.valid)++unsupported[986];spell=spellState.active();
  if(spellState.active()){animationScene.position(spellAnimationKeys[3],spellState.ringX+224,spellState.ringY+16);if(spellTick.bannerInterrupt)for(int i=0;i<3;i++)animationScene.interrupt(spellAnimationKeys[i],spellTick.bannerInterrupt);}
  if(spellTick.openedGameplay){backgroundVisible=false;stageState.flags&=~1u;}
  bullets.erase(std::remove_if(bullets.begin(),bullets.end(),[](auto& b){return !b.active;}),bullets.end());
  // Native HUD/MSG callback priority 25 follows ItemManager priority 22.
  messageVM.tick(u32(held),u32(pressed),[&](const message_system::Command& command){
    messageCommand(command);
    if(command.op!=10&&command.op!=11&&command.op!=12)messageEvents.push_back({command.op,frame+1,{command.payload,command.payload+command.size}});
    if(command.op==21){stage_award::add(score,stage_award::base(stageNumber));bindScreenAnimation(8,85);stage_award::add(score,stage_award::final(stageNumber,difficulty,power,lives,bombs,resources.pointValueStored));ended=1;}
  });
  dialogue=messageVM.active?messageVM.entry+1:0;
  for(const auto& e:enemies)if(!e->active)animationScene.removeOwner(e->id);
  animationScene.collect();animationScene.tickPrimary();animationScene.collect();
  if(animationScene.registry.bank(0))buildPlayerBombTrails();else tickPlayerBombEffects();
  enemies.erase(std::remove_if(enemies.begin(),enemies.end(),[](auto& e){return !e->active;}),enemies.end());
  ++frame;
}
struct Sprite{float x,y,rotation,scale;int bank,animation,kind,color,age;u32 entityId=0;int interrupt=0;float alpha=1;};
std::vector<Sprite> drawState;
std::array<float,41> hud;
#include "anm_draw.hpp"
#include "state_snapshot.hpp"
}

extern "C" {
int th12_load_ecl(const uint8_t* p,int n){return th12::program.add(p,n);}
int th12_load_msg(int index,const uint8_t* p,int n){if(index<0||index>=6)return 0;return th12::messageFiles[index].load(p,n);}
int th12_load_std(const uint8_t* p,int n){if(n<0)return 0;std::string error;return th12::stageProgram.load(p,size_t(n),error);}
int th12_load_anm(int bank,const uint8_t* p,int n){if(bank<0||bank>31||n<0)return 0;std::string error;return th12::animationScene.registry.loadBank(bank,p,size_t(n),error);}
void th12_oracle_fixture(int flags){th12::oracleFixtureFlags=flags&7;}
void th12_select_stage(int number){using namespace th12;ended=0;enemies.clear();bullets.clear();program=Program{};messageVM.reset();messageFiles={};messageEvents.clear();stageNumber=std::clamp(number,1,7);stageProgram={};stageState={};}
int th12_load_sht(int index,const uint8_t* p,int n){
  using namespace th12;if(index<0||index>=6||!loadouts[index].open(p,n))return 0;
  const int c=index/2;loadouts[index].hit=std::array<float,3>{2,3.5f,3}[c];loadouts[index].attractionSpeed=std::array<float,3>{5,7,6}[c];loadouts[index].attractionDiameter=std::array<float,3>{60,70,60}[c];return 1;
}
void th12_start(int c,int s,int d,int seed){using namespace th12;character=std::clamp(c,0,2);shot=std::clamp(s,0,1);difficulty=std::clamp(d,0,4);frame=backgroundFrame=0;backgroundVisible=true;px=0;py=51200;resources=item_system::ResourceState::initial(difficulty);graze=0;invuln=120;deathWindow=bombTimer=0;playerState=1;playerStateTicks=0;previousHeld=fireFrame=0;dialogue=spell=ended=0;oracleFixtureFlags=0;messageVM.reset();messageEvents.clear();spellState.reset();spellAnimationKeys={};messageAnimationKeys={};animationScene.reset();animationUnsupported.clear();configureAnimations();configurePlayerAnimations();animationScene.runtime.unsupported=[](int bank,int script,int opcode,const char*){++unsupported[987];++animationUnsupported[(uint32_t(bank)<<16)|uint16_t(opcode)];};displayRandom={0x1234,0};nextDetachedAnimation=1;th12::random={u16(seed),0};unsupported={};globals.clear();enemies.clear();bullets.clear();itemManager.reset();ufoManager.reset();playerDamageSources.reset();playerBomb.reset();playerMotion={};shotSchedule={};playerMotion.updateOptions(loadouts[character*2+shot],power,px,py,false,true);bulletWorld.playerX=px/128.f;bulletWorld.playerY=py/128.f;nextEnemyId=nextProjectileId=1;initializeStageAnimations();initializePlayerAnimation();spawn("main",0,0,10000,0,0);for(auto& e:enemies)e->flags&=~0x20000u;}
void th12_set_initial(int x,int y,int p,int life,int bomb,int points){using namespace th12;px=x;py=y;power=p;lives=life;bombs=bomb;score=points;bulletWorld.playerX=px/128.f;bulletWorld.playerY=py/128.f;playerMotion.updateOptions(loadouts[character*2+shot],power,px,py,previousHeld&8,true);}
void th12_set_replay_economy(int piv,int lifeFragments,int bombFragments,int red,int blue,int green,int rank){using namespace th12;resources.pointValueStored=piv;resources.lifeFragments=lifeFragments;resources.bombFragments=bombFragments;resources.rank=std::clamp(rank,-1024,1024);ufoManager.inventory.clearSlots();for(int color:{red,blue,green})if(color)ufoManager.inventory.append(color);}
const char* th12_state(){return th12::captureState().c_str();}
int th12_state_size(){return th12::snapshotBuffer.size();}
const char* th12_message(){return th12::captureMessage().c_str();}
int th12_message_size(){return th12::messageSnapshotBuffer.size();}
const char* th12_spell(){return th12::captureSpell().c_str();}
int th12_spell_size(){return th12::spellSnapshotBuffer.size();}
const char* th12_background(){return th12::captureBackground().c_str();}
int th12_background_size(){return th12::backgroundSnapshotBuffer.size();}
void th12_tick(int held,int pressed){th12::tick(held,pressed);}
int th12_draw(){using namespace th12;drawState.clear();for(auto& p:enemies){auto& e=*p;if(e.active&&!e.hidden)drawState.push_back({e.x+224,e.y+16,0,1,e.boundBank,e.animation,0,0,e.animationAge,e.id});}
  for(auto& b:bullets)if(b.active)drawState.push_back({b.x+224,b.y+16,b.angle+pi/2,b.friendly&&b.type==2?b.hitHeight/448.f:1,b.friendly?3:2,b.friendly?b.animation:b.enemy.animation,b.friendly?(b.type==2?6:2):1,b.type*16+b.color,b.friendly?b.weapon.visualAge:b.enemy.visualAge,0x10000000u+b.entityId,b.friendly?b.weapon.interrupt:b.enemy.interrupt});
  for(const auto& v:playerBomb.visuals)if(v.active)drawState.push_back({v.x+224,v.y+16,v.angle,1,v.bank,v.script,7,0,v.age,0x50000000u+v.id,v.interrupt});
  for(const auto& trail:playerBomb.trails)for(size_t i=1;i<trail.points.size();i++){const auto& a=trail.points[i-1];const auto& b=trail.points[i];const float dx=b.x-a.x,dy=b.y-a.y;drawState.push_back({float(double(a.x+b.x)*.5)+224,float(double(a.y+b.y)*.5)+16,float(std::atan2(double(dy),double(dx))),float(std::hypot(double(dx),double(dy))),0,int(b.color),trail.kind,int(a.color),0,0x60000000u+trail.id*16+u32(i),0,trail.kind==12?trail.width:1.f});}
  for(const auto& i:itemManager.entities)if(i.visible()){
    const int age=i.visualInterrupt?i.visualInterruptAge:i.visualAge;
    if(i.position.y>=-8)drawState.push_back({i.position.x+224,i.position.y+16,0,1,2,i.visualScript,3,i.type,age,0x20000000u+i.id,i.visualInterrupt,i.alpha});
    else {const float alpha=float(nativeInt(float(double(i.position.y+8)*255/32))&255)/255.f;drawState.push_back({i.position.x+224,24,0,1,2,i.type+185,3,i.type,i.visualAge,0x30000000u+i.id,0,alpha});}
    if(i.hintActive)drawState.push_back({i.position.x+224,i.position.y+16,0,1,2,i.hintScript,3,i.type,0,0x40000000u+i.id,i.hintInterrupt,i.alpha});
  }
  if(!deathWindow){for(int i=0;i<playerMotion.count;i++){const auto& o=playerMotion.options[i];drawState.push_back({o.x/128.f+224,o.y/128.f+16,0,1,3,PlayerMotion::optionScripts[character*2+shot],5,0,o.age,0xffff0001u+u32(i)});}}
  if(!deathWindow&&(!invuln||frame%6<3))drawState.push_back({px/128.f+224,py/128.f+16,0,1,3,playerMotion.bodyAnimation,4,0,playerMotion.bodyAge,0xffff0000u});return drawState.size();}
const void* th12_draw_ptr(){return th12::drawState.data();}
int th12_draw_stride(){return sizeof(th12::Sprite);}
int th12_draw_version(){return 2;}
int th12_anm_draw(){th12::captureAnimationDraw();return th12::animationDrawState.size();}
const void* th12_anm_draw_ptr(){return th12::animationDrawState.data();}
int th12_anm_draw_stride(){return sizeof(th12::AnimationPose);}
int th12_background_frame(){return th12::backgroundFrame;}
int th12_background_visible(){return th12::backgroundVisible;}
const float* th12_hud(){using namespace th12;int n=0;for(auto& p:enemies)n+=!p->hidden;u32 missing=0;for(auto v:unsupported)missing+=v;
  auto* boss=primaryBoss();hud={float(frame),px/128.f,py/128.f,float(double(score)*10),float(power),float(lives),float(bombs),float(graze),float(th12::random.seed),float(th12::random.calls),float(n),float(bullets.size()),float(spell),float(ended),float(eventBits),float(missing),float(bombTimer),float(character),float(shot),float(difficulty),float(resources.pointValueStored),float(resources.lifeFragments),float(resources.bombFragments),float(ufoManager.inventory.colors[0]),float(ufoManager.inventory.colors[1]),float(ufoManager.inventory.colors[2]),float(ufoManager.kind),float(ufoManager.age),ufoManager.fill,float(resources.rank),float(ufoManager.bucketA),float(ufoManager.bucketB),float(boss?boss->maxLife:0),float(boss?boss->phaseAge:0),float(boss?boss->life:0),ufoManager.rewardDisplayMultiplier,float(ufoManager.rewardDisplayFrames),float(ufoManager.rewardDisplayScore),float(dialogue),float(stageNumber),float(double(score)*10)};return hud.data();}
int th12_hud_size(){return th12::hud.size();}
const uint32_t* th12_unsupported(){return th12::unsupported.data();}
uint32_t th12_rng16(int seed){th12::Random r{uint16_t(seed),0};return r.next16();}
uint32_t th12_rng32(int seed){th12::Random r{uint16_t(seed),0};return r.next32();}
}
