#include "LaserState.hpp"
#include "BulletTh14.hpp"
#include "AnmRenderer.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 14
namespace {
float scalar(const u8* p){float v;std::memcpy(&v,p,4);return v;}
i32 integer(const u8* p){i32 v;std::memcpy(&v,p,4);return v;}
void scalar(u8* p,float v){std::memcpy(p,&v,4);}
bool visible(Vec3 p,float width){return p.x+width>-192.f&&p.x-width<192.f&&p.y+width>0.f&&p.y-width<448.f;}
i32 sprite_callback(AnmVm& vm,i32 frame){
 const auto& l=*reinterpret_cast<Th14LaserLine*>(reinterpret_cast<u8*>(&vm)-offsetof(Th14LaserLine,body));
 if(u32(l.base.sprite)>=43)return -1;const auto& a=th14_bullet_appearances[l.base.sprite];if(a.sprites[0]<0)return frame;
 const i32 index=l.base.color*4+frame;return u32(index)<64?a.sprites[index]:-1;
}
}
void laser_th14_line_initialize_state(Th14LaserLine& l,const u8* request,LaserWorld& w)noexcept{
 std::memcpy(l.request,request,0x354);auto& b=l.base;b.state=2;b.type=0;b.sprite=i16(integer(request+0x24));b.color=i16(integer(request+0x28));
 b.position={scalar(request),scalar(request+4),scalar(request+8)};b.angle=scalar(request+0xc);
 const float offset=scalar(request+0x2c);if(offset!=0.f){const auto v=polar(b.angle,offset);b.position.x+=v.x;b.position.y+=v.y;}
 b.length=scalar(request+0x14);b.width=scalar(request+0x1c);b.speed=scalar(request+0x20);b.offset=b.length>scalar(request+0x10)?.01f:0.f;
 const auto v=polar(b.angle,b.speed);b.velocity.x=v.x;b.velocity.y=v.y;
 b.offscreen_timer.set(30,&w.rate);b.graze_timer.set(0,&w.rate);l.motion_clock.set(0,&w.rate);
}
bool laser_th14_line_initialize(Th14LaserLine& l,const u8* request,AnmResource& resource,AnmEnvironment& a,LaserWorld& w,u16 file){
 laser_th14_line_initialize_state(l,request,w);auto& b=l.base;if(u32(b.sprite)>=43||u32(b.color)>=16)return false;
 auto& body=l.body;if(!body.bind_script(resource,th14_bullet_appearances[b.sprite].script,file,&a.rate))return false;
 body.sprite_callback=sprite_callback;body.reserved_430=u32(uintptr_t(&l));body.interrupt(2);if(body.update(a)<0)return false;
 body.flags=(body.flags&~(0x70u|0x3c0000u))|0x10u|0x200000u;body.th13_native_flags&=~0x100u;body.set_drawing_mode(1);body.flags2=(body.flags2&~0x40000u)|0x20000u;
 auto& origin=l.origin;if(!origin.copy_script(resource,b.color+56,file,&a.rate)||origin.update(a)<0)return false;
 origin.interrupt(2);if(origin.update(a)<0)return false;origin.flags=(origin.flags&~(0x70u|0x3c0000u))|0x10u;origin.th13_native_flags&=~0x100u;origin.set_drawing_mode(1);origin.flags2=(origin.flags2&~0x40000u)|0x20000u;
 auto& tip=l.tip;if(!tip.copy_script(resource,b.color+82,file,&a.rate)||tip.update(a)<0)return false;tip.flags2=(tip.flags2&~0x40000u)|0x20000u;
 const i32 sound=integer(request+0x34c);return sound<0||w.sound(sound,0,true);
}
i32 laser_th14_line_transforms(Th14LaserLine& l,LaserWorld& w)noexcept{
 auto& b=l.base;
 for(u32 budget=0;b.transform_index<18;++budget){
  if(budget>=4096||b.transform_index<0)return -2;const u8* t=l.request+0x34+b.transform_index*0x2c;const u32 op=u32(integer(t+0x20));if(!op||(!integer(t+0x24)&&b.active_transforms))break;
  switch(op){
  case 0x40:{const i32 count=integer(t+0x10);if(count>0){auto& s=b.motion[4];b.active_transforms|=op;s.a=scalar(t)>=0.f?scalar(t):b.speed;s.limit=count-1;s.count=0;s.duration=integer(t+0x14);std::memcpy(l.request+0x34+b.transform_index*0x2c+0x10,&s.limit,4);}break;}
  case 0x80:b.protection=integer(t+0x10);break;
  case 0x400:b.state=3;break;
  case 0x800:if(!w.sound(integer(t+0x10),b.position.x,true))return -2;break;
  case 0x8000:b.id=integer(t+0x10);break;
  case 0x10000:b.transform_index=integer(t+0x10);continue;
  case 0x100000:l.body.flags=(l.body.flags&~0x70u)|(integer(t+0x10)?0x10u:0u);l.body.th13_native_flags&=~0x100u;break;
  default:return -2;
  }
  ++b.transform_index;
 }
 return 0;
}
namespace {
// Retail 4055e0: preserve finite-segment rejection and float slope stores.
bool edge_intersection(Vec2 a,Vec2 b,Vec2 c,Vec2 d,Vec2& p){
 const auto side=[](Vec2 a,Vec2 b,Vec2 p){return (p.y-a.y)*(a.x-b.x)+(a.x-p.x)*(a.y-b.y);};
 const float u=side(a,b,c),v=side(a,b,d);if(u*v>0.f)return false;
 if(u==0.f&&v==0.f){auto aa=a,bb=b,cc=c,dd=d;if(aa.x>bb.x)std::swap(aa,bb);if(cc.x>dd.x)std::swap(cc,dd);if(bb.x<cc.x||bb.y<cc.y||dd.x<aa.x||dd.y<aa.y)return false;}
 else if(side(c,d,a)*side(c,d,b)>0.f)return false;
 const float dx=b.x-a.x,dy=b.y-a.y,lx=d.x-c.x,ly=d.y-c.y;const bool av=std::abs(dx)<.01f,cv=std::abs(lx)<.01f;
 const float as=av?0.f:dy/dx,ai=av?a.x:a.y-(dy*a.x)/dx,cs=cv?0.f:ly/lx,ci=cv?c.x:c.y-(ly*c.x)/lx;
 if(!av&&!cv){const float delta=ci-ai,den=as-cs;p={delta/den,(delta*as)/den+ai};}
 else if(!av)p={c.x,as*c.x+ai};else if(!cv)p={a.x,cs*a.x+ci};else{if(std::abs(a.x-c.x)>=.001f)return false;p=a;}
 return true;
}
}
i32 laser_th14_line_bounce(Th14LaserLine& l,LaserWorld& w)noexcept{
 auto& b=l.base;const auto v=polar(b.angle,b.length);const Vec2 tip{b.position.x+v.x,b.position.y+v.y},base{b.position.x,b.position.y};
 if(tip.x> -192.f&&tip.x<192.f&&tip.y>0.f&&tip.y<448.f)return 0;
 const auto& slot=b.motion[4];bool complete=false;
 const auto bounce=[&](Vec2 a,Vec2 c,bool vertical){
  complete=true;if(slot.duration&0x10)return true;Vec2 p{scalar(l.request),scalar(l.request+4)};edge_intersection(a,c,tip,base,p);
  scalar(l.request,p.x);scalar(l.request+4,p.y);scalar(l.request+8,0.f);scalar(l.request+0xc,vertical?normalize_angle(-b.angle-3.1415927410125732f):-b.angle);scalar(l.request+0x20,slot.a);scalar(l.request+0x2c,0.f);return w.th14_line_spawn(l.request);
 };
 if((slot.duration&1)&&tip.y<0.f&&!bounce({-256,0},{256,0},false))return -2;
 if((slot.duration&2)&&tip.y>448.f&&!bounce({-256,448},{256,448},false))return -2;
 if((slot.duration&4)&&tip.x< -192.f&&!bounce({-192,-192},{-192,640},true))return -2;
 if((slot.duration&8)&&tip.x>192.f&&!bounce({192,-192},{192,640},true))return -2;
 if(!complete)return 0;b.active_transforms&=~0x40u;const i32 sound=integer(l.request+0x350);if(sound>=0)w.sound(sound,0,false);return 1;
}
i32 laser_th14_line_step(Th14LaserLine& l,LaserWorld& w)noexcept{
 auto& b=l.base;
 for(u32 budget=0;;++budget){
  if(budget>=4096||laser_th14_line_transforms(l,w)<0)return -2;
  if(b.active_transforms&~0x40u)return -2;
  if(!b.active_transforms)break;
  const i32 complete=laser_th14_line_bounce(l,w);if(complete<0)return -2;
  if(b.protection)b.protection=wrapping_add(b.protection,-1);if(!complete)break;
 }
 const float delta=b.speed*w.rate,limit=scalar(l.request+0x10);
 if(limit>b.length){b.length+=delta;if(b.length>limit)b.length=limit;}
 else{
  b.offset+=delta;b.position.x+=b.velocity.x*w.rate;b.position.y+=b.velocity.y*w.rate;b.position.z+=b.velocity.z*w.rate;
  const float end=scalar(l.request+0x18);if(end>0.f&&b.offset+b.length>end){b.length=end-b.offset;scalar(l.request+0x10,b.length);if(b.length<=0.f)return 1;}
 }
 if(b.offscreen_timer.current<=0){const auto v=polar(b.angle,b.length);if(!visible(b.position,b.width)&&!visible({b.position.x+v.x,b.position.y+v.y,b.position.z},b.width))return 1;}
 else b.offscreen_timer.advance(-1);
 return 0;
}
i32 laser_th14_line_collision(Th14LaserLine& l,LaserWorld& w,bool probe){
 auto& b=l.base;if(!(b.length>16.f)||!(b.width>3.f))return 0;
 const auto v=polar(b.angle,b.length/10.f);const Vec3 start{b.position.x+v.x,b.position.y+v.y,b.position.z};
 const float width=b.width<32.f?b.width*.5f:b.width-(b.width+16.f)*.5f;
 const i32 result=probe?w.warning_collision(start,b.angle,width,(b.length*4.f)/5.f):w.collision(start,b.angle,width,(b.length*4.f)/5.f);
 if(result<0){l.diagnostic=2;return -2;}
 if(result==1){if(w.th14_line_cut(l,w.player,{32,32},0,0,true)<0){l.diagnostic=3;return -2;}return 0;}
 if(result==2){if(b.graze_timer.current%3==0&&!w.th14_curve_graze(start)){l.diagnostic=4;return -2;}b.graze_timer.tick();}
 return 0;
}
i32 laser_th14_line_update(Th14LaserLine& l,AnmEnvironment& a,LaserWorld& w){
 l.diagnostic=0;const i32 result=laser_th14_line_step(l,w);if(result){l.diagnostic=1;return result;}
 if(laser_th14_line_collision(l,w)<0)return -2;auto& b=l.base;
 if(!l.body.sprite){l.diagnostic=5;return -2;}
 l.body.scale.x=b.width/l.body.sprite->width;l.body.scale.y=b.length/l.body.sprite->height;l.body.flags|=8;
 if(l.body.update(a)<0){l.diagnostic=6;return -2;}
 if(b.offset==0.f&&l.origin.update(a)<0){l.diagnostic=7;return -2;}
 if(l.tip.update(a)<0){l.diagnostic=8;return -2;}l.motion_clock.tick();return 0;
}
bool laser_th14_line_draw(Th14LaserLine& l,AnmRenderer& r){
 const auto& b=l.base;l.body.script_position=b.position;l.body.rotation.z=normalize_angle(b.angle+1.5707963705062866f);l.body.flags|=4;if(r.draw(l.body)==-2)return false;
 const auto v=polar(b.angle,b.length);l.tip.script_position={b.position.x+v.x,b.position.y+v.y,0};if(r.draw(l.tip)==-2)return false;
 if(b.offset==0.f){l.origin.script_position=b.position;if(r.draw(l.origin)==-2)return false;}return true;
}
i32 laser_th14_line_cancel(Th14LaserLine& l,bool reward,bool skip,LaserWorld& w)noexcept{
 auto& b=l.base;if(skip&&b.protection)return 0;const auto v=polar(b.angle,8.f);Vec3 p{b.position.x+v.x,b.position.y+v.y,b.position.z};const Vec2 step{v.x+v.x,v.y+v.y};float distance=8.f;i32 count=0;
 if(b.length>16.f)do{
  if(++count>1000000)return -2;const i32 family=(b.sprite<=17||b.sprite==33||b.sprite==37)?181:b.sprite<=30?229:b.sprite<=32?253:-1;
  if(family>=0&&!w.cancel_effect(p,b.color*2+family))return -2;
  if(reward&&!w.th14_curve_reward(p,1))return -2;p.x+=step.x;p.y+=step.y;distance+=16.f;
 }while(b.length>distance+8.f);
 b.state=1;return count;
}
static i32 cut_samples(Th14LaserLine& l,Vec3 center,float radius,Vec2 size,float angle,bool rectangle,u32 rewards,bool skip,std::vector<std::array<u8,0x354>>& fragments,LaserWorld& w){
 fragments.clear();auto& b=l.base;if(skip&&b.protection)return 0;if(b.length<16.f)return 0;if(b.length>4096.f)return -2;
 const Vec3 original=b.position;const auto half=polar(b.angle,8.f);const Vec2 step{half.x+half.x,half.y+half.y};Vec3 p{original.x+half.x,original.y+half.y,0};
 std::array<u8,256> mask{};const float squared=radius*radius;float distance=8.f;i32 count=0,selected=0;
 Vec2 local{},local_step{};
 if(rectangle){const float x=original.x-center.x,y=original.y-center.y,sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
  const auto v=polar(normalize_angle(b.angle-angle),8.f);local={x*cosine-y*sine+v.x,y*cosine+x*sine+v.y};local_step={v.x+v.x,v.y+v.y};size.x*=.5f;size.y*=.5f;
 }
 do{
  if(count>=256)return -2;const float dx=center.x-p.x,dy=center.y-p.y;
  const bool inside=rectangle?!(local.x< -size.x||local.x>size.x||local.y< -size.y||local.y>size.y):!(dx*dx+dy*dy>squared);
  if(inside){mask[count]=1;++selected;if(rewards&&!w.th14_curve_reward(p,rewards))return -2;
   const i32 family=(b.sprite<=17||b.sprite==33||b.sprite==37)?181:b.sprite<=30?229:b.sprite<=32?253:-1;
   if(family>=0&&!w.cancel_effect(p,b.color*2+family))return -2;
  }
  ++count;p.x+=step.x;p.y+=step.y;local.x+=local_step.x;local.y+=local_step.y;distance+=16.f;
 }while(!(b.length<distance+8.f));
 if(!selected)return 0;
 const auto mark=[&](){l.native_flags=(l.native_flags&~4u)|2u;};
 if(selected>=count){mark();return selected;}
 i32 at=0;while(at<count&&mask[at])++at;
 if(at){const float moved=float(at)*16.f;b.position.x+=float(at)*step.x;b.position.y+=float(at)*step.y;b.length-=moved;
  if(!(b.length>24.f)){mark();return selected;}scalar(l.request+0x10,b.length);b.offset=moved;
 }
 i32 prefix=0;while(at<count&&!mask[at]){++at;++prefix;}
 if(at>=count)return selected;
 const float length=float(prefix)*16.f,removed=b.length-length;b.length=length;scalar(l.request+0x10,scalar(l.request+0x10)-removed);if(length<24.f)mark();
 while(at<count){
  while(at<count&&mask[at])++at;if(at>=count)break;const i32 begin=at;while(at<count&&!mask[at])++at;
  const float length=float(at-begin)*16.f;if(!(length>24.f))continue;
  std::array<u8,0x354> request;std::memcpy(request.data(),l.request,0x354);scalar(request.data()+0x10,length);scalar(request.data()+0x14,length);
  const Vec3 pos{original.x+float(begin)*step.x,original.y+float(begin)*step.y,original.z};std::memcpy(request.data(),&pos,12);fragments.push_back(request);
 }
 return selected;
}
i32 laser_th14_line_cut_circle(Th14LaserLine& l,Vec3 c,float r,u32 rewards,bool skip,std::vector<std::array<u8,0x354>>& f,LaserWorld& w){return cut_samples(l,c,r,{},0,false,rewards,skip,f,w);}
i32 laser_th14_line_cut_rectangle(Th14LaserLine& l,Vec3 c,Vec2 size,float angle,u32 rewards,bool skip,std::vector<std::array<u8,0x354>>& f,LaserWorld& w){return cut_samples(l,c,0,size,angle,true,rewards,skip,f,w);}
#endif
}
