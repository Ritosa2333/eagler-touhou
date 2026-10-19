#include "LaserState.hpp"
#include "Movement.hpp"
#include <array>
namespace th11 {
namespace {
bool visible(Vec3 p,float half){return double(p.x)+half>-192&&double(p.x)-half<192&&double(p.y)+half>0&&double(p.y)-half<448;}
template<class Contains>
i32 cut(LaserInfinite& l,Contains contains,bool circle,u32 rewards,bool skip,LaserWorld& w){
    auto& b=l.base;if(skip&&(l.parameters.flags&8))return 0;
    if(b.length>4112)return -2;if(!(b.length>16))return 0;
    const Vec3 original=b.position;const auto half=polar(b.angle,8);
    const Vec2 step{float(double(half.x)+half.x),float(double(half.y)+half.y)};
    Vec3 position{float(double(original.x)+half.x),float(double(original.y)+half.y),0};
    std::array<u8,256> mask{};i32 count=0,selected=0;float distance=8;
    do{
        const i32 hit=contains(position);
        if(hit){
            mask[count]=1;++selected;
            if(visible(position,32)){
                if((rewards&1)&&!w.cancel_reward(position))return -2;
                if((rewards&2)&&!w.cancel_shot(position,normalize_angle(float(double(b.angle)+3.1415927410125732421875))))return -2;
                if(!w.cancel_effect(position,l.parameters.color*2+2))return -2;
            }
        }
        ++count;position.x=float(double(position.x)+step.x);position.y=float(double(position.y)+step.y);distance=float(double(distance)+16);
    }while(double(distance)+8<b.length);
    if(!selected)return 0;
    i32 index=0;while(index<count&&!mask[index])++index;
    b.length=float(index*16);
    while(index<count){
        while(index<count&&mask[index])++index;
        const i32 start=index;while(index<count&&!mask[index])++index;
        if(index==start)break;
        const Vec3 pos{float(double(original.x)+float(double(start)*step.x)),float(double(original.y)+float(double(start)*step.y)),float(double(original.z)+0)};
        if(!circle||visible(pos,32)){
            LaserLineParameters p{};p.position=pos;p.angle=b.angle;p.growth_limit=p.initial_length=float((index-start)*16);
            p.end_distance=float(double(l.parameters.max_length)-double(start)*16);p.width=b.width;p.speed=8;p.sprite=l.parameters.sprite;p.color=l.parameters.color;
            if(!w.spawn_line(p))return -2;
        }
    }
    return selected;
}
}
i32 laser_infinite_cut_rectangle(LaserInfinite& l,Vec3 center,Vec3 size,bool reward,bool skip,LaserWorld& w)noexcept{
    const float x=float(double(size.x)*.5),y=float(double(size.y)*.5);
    const float left=float(double(center.x)-x),right=float(double(center.x)+x),top=float(double(center.y)-y),bottom=float(double(center.y)+y);
    return cut(l,[&](Vec3 p){return left<=p.x&&p.x<=right&&top<=p.y&&p.y<=bottom?2:0;},false,reward?1:0,skip,w);
}
i32 laser_infinite_cut_circle(LaserInfinite& l,Vec3 center,float radius,u32 rewards,bool skip,LaserWorld& w)noexcept{
    const float squared=float(double(radius)*radius);
    return cut(l,[&](Vec3 p){const double dx=double(center.x)-p.x,dy=double(center.y)-p.y;const float distance=float(dx*dx+dy*dy);return distance<squared?2:distance==squared?1:0;},true,rewards,skip,w);
}
i32 laser_infinite_cancel(LaserInfinite& l,bool reward,bool skip,LaserWorld& w)noexcept{
    auto& b=l.base;if(skip&&(l.parameters.flags&8))return 0;
    const auto half=polar(b.angle,8);Vec3 pos{float(double(half.x)+b.position.x),float(double(half.y)+b.position.y),float(double(b.position.z)+0)};
    const Vec2 step{float(double(half.x)+half.x),float(double(half.y)+half.y)};i32 count=0;float distance=8;
    if(b.length>16)do{
        if(++count>1000000)return -2;
        if(visible(pos,16)){
            if(!w.cancel_effect(pos,l.parameters.color*2+2))return -2;
            if(reward&&visible(pos,32)&&!w.cancel_reward(pos))return -2;
        }
        pos.x=float(double(pos.x)+step.x);pos.y=float(double(pos.y)+step.y);distance=float(double(distance)+16);
    }while(double(distance)+8<b.length);
    b.state=1;return count;
}
}
