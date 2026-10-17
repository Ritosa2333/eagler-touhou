#include "LaserState.hpp"
#include "Movement.hpp"
#include <array>
namespace th11 {
namespace {
bool reward_visible(Vec3 p){return double(p.x)+32>-192&&double(p.x)-32<192&&double(p.y)+32>0&&double(p.y)-32<448;}
template<class Contains>
i32 cut(LaserLine& l,Contains contains,u32 rewards,bool skip,LaserWorld& w){
    auto& b=l.base;if(skip&&b.protection)return 0;
    // The native routines use a 256-byte segment mask. Reject longer input
    // before callbacks instead of reproducing their stack-buffer overrun.
    if(b.length>=4112)return -2;
    if(!(b.length>=16))return 0;
    const Vec3 original=b.position;const auto half=polar(b.angle,8);
    const Vec2 step{float(double(half.x)+half.x),float(double(half.y)+half.y)};
    Vec3 position{float(double(original.x)+half.x),float(double(original.y)+half.y),0};
    std::array<u8,256> mask{};i32 count=0,selected=0;float distance=8;
    do {
        if(contains(position)){
            mask[count]=1;++selected;
            if(reward_visible(position)){
                if((rewards&1)&&!w.cancel_reward(position))return -2;
                if((rewards&2)&&!w.cancel_shot(position,normalize_angle(float(double(b.angle)+3.1415927410125732421875))))return -2;
            }
            if(!w.cancel_effect(position,l.parameters.color*2+2))return -2;
        }
        ++count;position.x=float(double(position.x)+step.x);position.y=float(double(position.y)+step.y);distance=float(double(distance)+16);
    }while(double(distance)+8<=b.length);
    if(!selected)return 0;
    if(selected==count){b.marked=1;return selected;}
    i32 index=0;while(index<count&&mask[index])++index;
    if(index){
        b.position.x=float(double(b.position.x)+float(double(index)*step.x));
        b.position.y=float(double(b.position.y)+float(double(index)*step.y));
        b.position.z=float(double(b.position.z)+0);
        b.length=float(double(b.length)-double(index)*16);
        if(b.length<=24){b.marked=1;return selected;}
        l.parameters.growth_limit=b.length;b.offset=float(index*16);
    }
    i32 kept=0;for(;index<count&&!mask[index];++index)++kept;
    if(index==count)return selected;
    const float length=float(kept*16);
    l.parameters.growth_limit=float(double(l.parameters.growth_limit)-(double(b.length)-length));
    b.length=length;if(length<24)b.marked=1;
    while(index<count){
        while(index<count&&mask[index])++index;
        const i32 start=index;while(index<count&&!mask[index])++index;
        const float part_length=float((index-start)*16);
        if(part_length>24){
            auto p=l.parameters;p.growth_limit=part_length;p.initial_length=part_length;
            p.position={float(double(original.x)+float(double(start)*step.x)),float(double(original.y)+float(double(start)*step.y)),float(double(original.z)+0)};
            if(!w.spawn_line(p))return -2;
        }
    }
    return selected;
}
}
i32 laser_line_cut_rectangle(LaserLine& l,Vec3 center,Vec3 size,bool reward,bool skip,LaserWorld& w)noexcept{
    const float x=float(double(size.x)*.5),y=float(double(size.y)*.5);
    const float left=float(double(center.x)-x),right=float(double(center.x)+x),top=float(double(center.y)-y),bottom=float(double(center.y)+y);
    return cut(l,[&](Vec3 p){return left<=p.x&&p.x<=right&&top<=p.y&&p.y<=bottom;},reward?1:0,skip,w);
}
i32 laser_line_cut_circle(LaserLine& l,Vec3 center,float radius,u32 rewards,bool skip,LaserWorld& w)noexcept{
    const float squared=float(double(radius)*radius);
    return cut(l,[&](Vec3 p){const double dx=double(center.x)-p.x,dy=double(center.y)-p.y;return float(dx*dx+dy*dy)<=squared;},rewards,skip,w);
}
}
