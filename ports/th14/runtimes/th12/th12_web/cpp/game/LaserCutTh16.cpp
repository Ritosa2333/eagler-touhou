#include "LaserState.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 16
namespace {
float scalar(const u8* p){float v;std::memcpy(&v,p,4);return v;}
void scalar(u8* p,float v){std::memcpy(p,&v,4);}
i32 cut_samples(Th16LaserLine& l,Vec3 center,float radius,Vec2 size,float angle,bool rectangle,u32 rewards,bool skip,std::vector<std::array<u8,0x358>>& fragments,LaserWorld& w){
 fragments.clear();auto& b=l.base;if(skip&&b.protection)return 0;if(b.length<16.f)return 0;if(b.length>4096.f)return -2;
 const Vec3 original=b.position;const auto half=polar(b.angle,8.f);const Vec2 step{half.x+half.x,half.y+half.y};Vec3 p{original.x+half.x,original.y+half.y,0};
 std::array<u8,256> mask{};const float squared=radius*radius;float distance=8.f;i32 count=0,selected=0;Vec2 local{},local_step{};
 if(rectangle){const float x=original.x-center.x,y=original.y-center.y,sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
  const auto v=polar(normalize_angle(b.angle-angle),8.f);local={x*cosine-y*sine+v.x,y*cosine+x*sine+v.y};local_step={v.x+v.x,v.y+v.y};size.x*=.5f;size.y*=.5f;}
 do{
  if(count>=256)return -2;const float dx=center.x-p.x,dy=center.y-p.y;
  const bool inside=rectangle?!(local.x< -size.x||local.x>size.x||local.y< -size.y||local.y>size.y):!(dx*dx+dy*dy>squared);
  if(inside){mask[count]=1;++selected;if(!w.th16_line_reward(p,rewards))return -2;
   const i32 family=(b.sprite<=17||b.sprite==34||b.sprite==38)?209:b.sprite<=(rectangle?30:31)?257:b.sprite<=33?281:-1;
   if(family>=0&&!(rectangle?w.th16_line_cut_effect(p,b.color*2+family):w.cancel_effect(p,b.color*2+family)))return -2;}
  ++count;p.x+=step.x;p.y+=step.y;local.x+=local_step.x;local.y+=local_step.y;distance+=16.f;
 }while(!(b.length<distance+8.f));
 if(!selected)return 0;const auto mark=[&](){l.native_flags=(l.native_flags&~4u)|2u;};
 if(selected>=count){mark();return selected;}
 i32 at=0;while(at<count&&mask[at])++at;
 if(at){const float moved=float(at)*16.f;b.position.x+=float(at)*step.x;b.position.y+=float(at)*step.y;b.length-=moved;
  if(!(b.length>24.f)){mark();return selected;}scalar(l.request+0x10,b.length);b.offset=moved;}
 i32 prefix=0;while(at<count&&!mask[at]){++at;++prefix;}if(at>=count)return selected;
 const float length=float(prefix)*16.f,removed=b.length-length;b.length=length;scalar(l.request+0x10,scalar(l.request+0x10)-removed);if(length<24.f)mark();
 while(at<count){
  while(at<count&&mask[at])++at;if(at>=count)break;const i32 begin=at;while(at<count&&!mask[at])++at;
  const float length=float(at-begin)*16.f;if(!(length>24.f))continue;
  std::array<u8,0x358> request;std::memcpy(request.data(),l.request,0x358);scalar(request.data()+0x10,length);scalar(request.data()+0x14,length);
  const Vec3 pos{original.x+float(begin)*step.x,original.y+float(begin)*step.y,original.z};std::memcpy(request.data(),&pos,12);fragments.push_back(request);
 }
 return selected;
}
}
// Own434730 circle and433860 rotated rectangle; every fragment retains358 bytes.
i32 laser_th16_line_cut_circle(Th16LaserLine& l,Vec3 c,float r,u32 rewards,bool skip,std::vector<std::array<u8,0x358>>& f,LaserWorld& w){return cut_samples(l,c,r,{},0,false,rewards,skip,f,w);}
i32 laser_th16_line_cut_rectangle(Th16LaserLine& l,Vec3 c,Vec2 size,float angle,u32 rewards,bool skip,std::vector<std::array<u8,0x358>>& f,LaserWorld& w){return cut_samples(l,c,0,size,angle,true,rewards,skip,f,w);}
#endif
}
