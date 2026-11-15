#include "LaserState.hpp"
#include "BulletTh16.hpp"
#include "AnmRenderer.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 16
namespace {
float scalar(const u8* p){float v;std::memcpy(&v,p,4);return v;}
i32 integer(const u8* p){i32 v;std::memcpy(&v,p,4);return v;}
void scalar(u8* p,float v){std::memcpy(p,&v,4);}
bool visible(Vec3 p,float width){return p.x+width>-192.f&&p.x-width<192.f&&p.y+width>0.f&&p.y-width<448.f;}
i32 sprite_callback(AnmVm& vm,i32 frame){
 const auto& l=*reinterpret_cast<Th16LaserLine*>(reinterpret_cast<u8*>(&vm)-offsetof(Th16LaserLine,body));
 if(u32(l.base.sprite)>=44)return -1;const auto& a=th16_bullet_appearances[l.base.sprite];if(a.sprites[0]<0)return frame;
 const i32 index=l.base.color*4+frame;return u32(index)<64?a.sprites[index]:-1;
}
bool transform_sound(const Th16LaserLine& l,LaserWorld& w){const i32 id=integer(l.request+0x354);return id<0||w.sound(id,0,false);}
}
// Own431b30; the request is distinct from TH14's354-byte descriptor.
void laser_th16_line_initialize_state(Th16LaserLine& l,const u8* request,LaserWorld& w)noexcept{
 std::memcpy(l.request,request,0x358);auto& b=l.base;b.state=2;b.type=0;
 b.sprite=i16(integer(request+0x24));b.color=i16(integer(request+0x28));
 b.position={scalar(request),scalar(request+4),scalar(request+8)};b.angle=scalar(request+0xc);
 const float offset=scalar(request+0x2c);if(offset!=0.f){const auto v=polar(b.angle,offset);b.position.x+=v.x;b.position.y+=v.y;}
 b.length=scalar(request+0x14);b.width=scalar(request+0x1c);b.speed=scalar(request+0x20);b.offset=b.length>scalar(request+0x10)?.01f:0.f;
 const auto v=polar(b.angle,b.speed);b.velocity.x=v.x;b.velocity.y=v.y;b.transform_index=integer(request+0x30);
 b.offscreen_timer.set(30,&w.rate);l.visibility_clock.set(3,&w.rate);b.graze_timer.set(0,&w.rate);l.motion_clock.set(0,&w.rate);
}
bool laser_th16_line_initialize(Th16LaserLine& l,const u8* request,AnmResource& resource,AnmEnvironment& a,LaserWorld& w,u16 file){
 laser_th16_line_initialize_state(l,request,w);auto& b=l.base;if(u32(b.sprite)>=44||u32(b.color)>=16)return false;
 auto& body=l.body;if(!body.bind_script(resource,th16_bullet_appearances[b.sprite].script,file,&a.rate))return false;
 body.sprite_callback=sprite_callback;body.reserved_430=u32(uintptr_t(&l));body.interrupt(2);if(body.update(a)<0)return false;
 body.flags=(body.flags&~(0x70u|0x1c0000u))|0x10u|0x200000u;body.th13_native_flags&=~0x100u;body.set_drawing_mode(1);body.flags2=(body.flags2&~0x80000u)|0x40000u;
 auto& origin=l.origin;if(!origin.bind_script(resource,b.color+56,file,&a.rate)||origin.update(a)<0)return false;
 origin.interrupt(2);if(origin.update(a)<0)return false;origin.flags=(origin.flags&~0x70u)|0x10u;origin.th13_native_flags&=~0x100u;origin.set_drawing_mode(1);origin.flags2=(origin.flags2&~0x80000u)|0x40000u;
 auto& tip=l.tip;const bool small=b.sprite<=17||b.sprite==38;
 if(!tip.bind_script(resource,b.color+(small?91:83),file,&a.rate)||tip.update(a)<0)return false;
 if(small){tip.flags=(tip.flags&~0x70u)|0x10u;tip.th13_native_flags&=~0x100u;}tip.flags2=(tip.flags2&~0x80000u)|0x40000u;
 const i32 id=integer(request+0x350);return id<0||w.sound(id,0,true);
}
i32 laser_th16_line_transforms(Th16LaserLine& l,LaserWorld& w)noexcept{
 auto& b=l.base;
 for(u32 budget=0;b.transform_index<18;++budget){
  if(budget>=4096||b.transform_index<0)return -2;const u8* t=l.request+0x38+b.transform_index*0x2c;
  const u32 op=u32(integer(t+0x20));if(!op||(!integer(t+0x24)&&b.active_transforms))break;
  switch(op){
  case 1:b.active_transforms|=1;b.motion[0].timer.set(0,&w.rate);b.motion[0].vector.z=0;break;
  case 4:{auto& s=b.motion[1];b.active_transforms|=4;s.a=scalar(t);const float angle=scalar(t+4);s.b=angle<=-999990.f?b.angle:angle>=999990.f?bullet_aim(b.position,w.player):angle;
   s.timer.set(0,&w.rate);s.duration=integer(t+0x10);const auto v=polar(s.b,s.a);s.vector.x=v.x;s.vector.y=v.y;if(b.transform_index&&!transform_sound(l,w))return -2;break;}
  case 8:{auto& s=b.motion[2];b.active_transforms|=8;s.a=scalar(t);s.b=scalar(t+4);s.timer.set(0,&w.rate);s.duration=integer(t+0x10);if(b.transform_index&&!transform_sound(l,w))return -2;break;}
  case 16:{auto& s=b.motion[3];b.active_transforms|=16;s.a=scalar(t+4)<=-999990.f?b.speed:scalar(t+4);s.b=scalar(t);s.timer.set(0,&w.rate);s.duration=integer(t+0x10);s.limit=integer(t+0x14);s.count=0;l.turn_mode=integer(t+0x18);break;}
  case 0x80:b.protection=integer(t+0x10);break;
  case 0x100:l.visibility_clock.set(integer(t+0x10),&w.rate);break;
  case 0x200:{const i32 sprite=integer(t+0x10);if(u32(sprite)>=44||!w.th16_line_appearance(l,wrapping_add(th16_bullet_appearances[sprite].script,integer(t+0x14))))return -2;break;}
  case 0x400:b.state=3;break;
  case 0x800:if(!w.sound(integer(t+0x10),b.position.x,true))return -2;break;
  case 0x1000:b.active_transforms|=op;b.motion[6].timer.set(integer(t+0x10),&w.rate);break;
  case 0x8000:b.id=integer(t+0x10);break;
  case 0x10000:b.transform_index=integer(t+0x10);continue;
  case 0x100000:l.body.flags=(l.body.flags&~0x70u)|(integer(t+0x10)?0x10u:0u);l.body.th13_native_flags&=~0x100u;break;
  case 0x80000000:b.active_transforms|=op;l.wait_clock.set(integer(t+0x10),&w.rate);break;
  default:return -2;
  }
  ++b.transform_index;
 }
 return 0;
}
i32 laser_th16_line_step(Th16LaserLine& l,LaserWorld& w)noexcept{
 auto& b=l.base;
 for(u32 budget=0;;++budget){
  if(budget>=4096||laser_th16_line_transforms(l,w)<0)return -2;
  if(!b.active_transforms)break;if(b.active_transforms&~0x8000101du)return -2;i32 complete=0;
  // Ownkind0 vtable492424 leaves1,8,1000 and nonzero turn modes active.
  if(b.active_transforms&4){auto& s=b.motion[1];if(s.timer.current>=s.duration){b.active_transforms&=~4u;++complete;}
   else{b.speed+=s.a*w.rate;b.velocity.x+=s.vector.x*w.rate;b.velocity.y+=s.vector.y*w.rate;b.velocity.z+=s.vector.z*w.rate;
    if(std::abs(b.velocity.x)>9.999999747378752e-05f||std::abs(b.velocity.y)>9.999999747378752e-05f)b.angle=float(std::atan2(double(b.velocity.y),double(b.velocity.x)));s.timer.tick();}}
  if((b.active_transforms&16)&&!l.turn_mode){auto& s=b.motion[3];float speed=b.speed;
   if(s.timer.current>=s.duration){if(!transform_sound(l,w))return -2;b.speed=s.a;b.angle+=s.b;s.count=wrapping_add(s.count,1);s.timer.set(0,&w.rate);speed=b.speed;
    if(s.count>=s.limit){b.active_transforms&=~16u;++complete;}}
   else speed-=float(s.timer.fractional*speed)/float(s.duration);
   const auto v=polar(b.angle,speed);b.velocity.x=v.x;b.velocity.y=v.y;if(b.active_transforms&16)s.timer.tick();}
  if(b.active_transforms&0x80000000u){if(l.wait_clock.current<=0){b.active_transforms&=~0x80000000u;++complete;}else l.wait_clock.advance(-1);}
  if(b.protection)b.protection=wrapping_add(b.protection,-1);if(!complete)break;
 }
 const float delta=b.speed*w.rate,limit=scalar(l.request+0x10);
 if(limit>b.length){b.length+=delta;if(b.length>limit)b.length=limit;}
 else{b.offset+=delta;b.position.x+=b.velocity.x*w.rate;b.position.y+=b.velocity.y*w.rate;b.position.z+=b.velocity.z*w.rate;
  const float end=scalar(l.request+0x18);if(end>0.f&&b.offset+b.length>end){b.length=end-b.offset;scalar(l.request+0x10,b.length);if(b.length<=0.f)return 1;}}
 if(b.offscreen_timer.current<=0&&l.visibility_clock.current<=0){const auto v=polar(b.angle,b.length);if(!visible(b.position,b.width)&&!visible({b.position.x+v.x,b.position.y+v.y,b.position.z},b.width))return 1;}
 else{if(b.offscreen_timer.current>0)b.offscreen_timer.advance(-1);if(l.visibility_clock.current>0)l.visibility_clock.advance(-1);}
 return 0;
}
namespace {
// Own404220 infinite-line intersection. Here the second line is perpendicular.
Vec2 graze_contact(Vec3 start,float angle,Vec3 player){
 const auto a=polar(angle,10),c=polar(normalize_angle(angle+1.5707963705062866f),10);
 const bool av=std::abs(a.x)<.01f,cv=std::abs(c.x)<.01f;
 const float as=av?0.f:a.y/a.x,ai=av?start.x:start.y-as*start.x,cs=cv?0.f:c.y/c.x,ci=cv?player.x:player.y-cs*player.x;
 if(!av&&!cv){const float delta=ci-ai,den=as-cs;return{delta/den,(delta*as)/den+ai};}
 if(!av)return{player.x,as*player.x+ai};if(!cv)return{start.x,cs*start.x+ci};return{start.x,start.y};
}
}
i32 laser_th16_line_collision(Th16LaserLine& l,LaserWorld& w,bool probe){
 auto& b=l.base;if(!(b.length>16.f)||!(b.width>3.f))return 0;
 Vec3 start=b.position;float length=b.length;if(!(u32(integer(l.request+0x34))&2)){const auto v=polar(b.angle,b.length/10.f);start.x+=v.x;start.y+=v.y;length=(b.length*4.f)/5.f;}
 const float width=b.width<32.f?b.width*.5f:b.width-(b.width+16.f)*.5f;
 const i32 result=probe?w.warning_collision(start,b.angle,width,length):w.collision(start,b.angle,width,length);
 if(result<0){l.diagnostic=2;return -2;}
 if(result==1){if(w.th16_line_cut(l,w.player,{32,32},0,0,true)<0){l.diagnostic=3;return -2;}return 0;}
 if(result==2){if(b.graze_timer.current%3==0){const auto p=graze_contact(start,b.angle,w.player);start.x=p.x;start.y=p.y;if(!w.th16_line_graze(start)){l.diagnostic=4;return -2;}}b.graze_timer.tick();}
 return 0;
}
i32 laser_th16_line_update(Th16LaserLine& l,AnmEnvironment& a,LaserWorld& w){
 l.diagnostic=0;const i32 result=laser_th16_line_step(l,w);if(result){l.diagnostic=1;return result;}
 if(laser_th16_line_collision(l,w)<0)return -2;auto& b=l.base;if(!l.body.sprite){l.diagnostic=5;return -2;}
 l.body.scale.x=b.width/l.body.sprite->width;l.body.scale.y=b.length/l.body.sprite->height;l.body.flags|=8;
 if(l.body.update(a)<0){l.diagnostic=6;return -2;}if(b.offset==0.f&&l.origin.update(a)<0){l.diagnostic=7;return -2;}
 if(l.tip.update(a)<0){l.diagnostic=8;return -2;}l.motion_clock.tick();return 0;
}
bool laser_th16_line_draw(Th16LaserLine& l,AnmRenderer& r){
 const auto& b=l.base;l.body.script_position=b.position;l.body.rotation.z=normalize_angle(b.angle+1.5707963705062866f);l.body.flags|=4;if(r.draw(l.body)==-2)return false;
 const auto v=polar(b.angle,b.length);l.tip.script_position={b.position.x+v.x,b.position.y+v.y,0};if(r.draw(l.tip)==-2)return false;
 if(b.offset==0.f){l.origin.script_position=b.position;if(r.draw(l.origin)==-2)return false;}return true;
}
i32 laser_th16_line_cancel(Th16LaserLine& l,bool reward,bool skip,LaserWorld& w)noexcept{
 auto& b=l.base;if(skip&&b.protection)return 0;const auto v=polar(b.angle,8.f);Vec3 p{b.position.x+v.x,b.position.y+v.y,b.position.z};const Vec2 step{v.x+v.x,v.y+v.y};float distance=8.f;i32 count=0;
 if(b.length>16.f)do{if(++count>1000000)return -2;
  const i32 family=(b.sprite<=17||b.sprite==34||b.sprite==38)?209:b.sprite<=31?257:b.sprite<=33?281:-1;
  if(family>=0&&!w.cancel_effect(p,b.color*2+family))return -2;if(!w.th16_line_reward(p,reward?1u:0u))return -2;
  p.x+=step.x;p.y+=step.y;distance+=16.f;
 }while(b.length>distance+8.f);b.state=1;return count;
}
#endif
}
