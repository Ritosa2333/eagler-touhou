#include "LaserBehavior.hpp"
#include "GameMath.hpp"
#include <cstdlib>
namespace th10 {
namespace {
struct Samples {
    u8 fixed[256]{};
    u8* overflow=nullptr;
    u32 capacity=0;
    i32 count=0,hits=0;
    Samples()=default;
    Samples(const Samples&)=delete;
    Samples(Samples&& other) noexcept :overflow(other.overflow),capacity(other.capacity),count(other.count),hits(other.hits){std::memcpy(fixed,other.fixed,sizeof(fixed));other.overflow=nullptr;}
    ~Samples(){std::free(overflow);}
    void add(bool hit){
        if(count<256)fixed[count]=hit;
        else{const auto index=static_cast<u32>(count-256);if(index==capacity){capacity=capacity?capacity*2:256;auto* resized=static_cast<u8*>(std::realloc(overflow,capacity));if(!resized)__builtin_trap();overflow=resized;}overflow[index]=hit;}
        ++count;if(hit)++hits;
    }
    bool operator[](i32 index) const {return index<256?fixed[index]:overflow[index-256];}
};
struct Sweep {
    Vec3 original,step,point;
    float distance=6;
    explicit Sweep(const EnemyLaser& laser,bool preserve_z):original(laser.position){
        const auto half=polar(laser.angle,6);
        point={Scalar::add(half.x,original.x),Scalar::add(half.y,original.y),preserve_z?original.z:0.f};
        step={Scalar::add(half.x,half.x),Scalar::add(half.y,half.y),0};
    }
    void next(){point.x=Scalar::add(step.x,point.x);point.y=Scalar::add(step.y,point.y);point.z=Scalar::add(point.z,step.z);distance=Scalar::add(distance,12.f);}
    Vec3 position(i32 index,const Vec3& origin) const {
        const auto count=Extended::from_int(index),x=number(step.x)*count;
        const float y=(number(step.y)*count).to_float(),z=(count*number(step.z)).to_float();
        return {(x+number(origin.x)).to_float(),Scalar::add(y,origin.y),Scalar::add(z,origin.z)};
    }
};
struct Rectangle {
    Vec2 minimum,maximum;
    Rectangle(const Vec3& point,const Vec3& size){
        const float x=Scalar::mul(size.x,.5f),y=Scalar::mul(size.y,.5f);
        minimum={Scalar::sub(point.x,x),Scalar::sub(point.y,y)};
        maximum={Scalar::add(x,point.x),Scalar::add(y,point.y)};
    }
    bool contains(const Vec3& point) const {return !(point.x<minimum.x||maximum.x<point.x||point.y<minimum.y||maximum.y<point.y);}
};
struct Circle {
    const Vec3& center;float squared_radius;
    Circle(const Vec3& center,float radius):center(center),squared_radius(Scalar::mul(radius,radius)){}
    bool contains(const Vec3& point) const {
        const auto y=number(center.y)-number(point.y),x=number(center.x)-number(point.x);
        return !(number(squared_radius)<x*x+y*y);
    }
};
template<class Laser,class Region>
Samples cancel_samples(Laser& laser,Sweep& sweep,const Region& region,i32 convert,bool inclusive,LaserBehaviorEnvironment& env){
    Samples samples;
    if(!(laser.base.length>12.f||(inclusive&&laser.base.length==12.f)))return samples;
    do{
        const bool hit=region.contains(sweep.point);samples.add(hit);
        if(hit){
            if(convert&&!laser_outside_playfield(sweep.point,32,32))env.spawn_faith(sweep.point);
            env.cancel_effect(wrapping_add(static_cast<i32>(laser.parameters.color)*2,17),sweep.point);
        }
        sweep.next();const auto end=number(sweep.distance)+number(6.f);
        if(!(end<number(laser.base.length)||(inclusive&&end==number(laser.base.length))))break;
    }while(true);
    return samples;
}
void split(StraightLaser& laser,const Sweep& sweep,const Samples& samples,LaserBehaviorEnvironment& env){
    if(!samples.hits)return;
    auto& base=laser.base;
    if(samples.hits>=samples.count){base.delete_wait=1;return;}
    i32 index=0;while(index<samples.count&&samples[index])++index;
    if(index){
        base.position=sweep.position(index,base.position);
        const auto removed=Extended::from_int(index)*number(12.f),length=number(base.length)-removed;base.length=length.to_float();
        if(!(number(18.f)<length)){base.delete_wait=1;return;}
        laser.parameters.target_length=length.to_float();base.distance_travelled=removed.to_float();
    }
    i32 retained=0;while(index<samples.count&&!samples[index]){++index;++retained;}
    if(index>=samples.count)return;
    const auto length=Extended::from_int(retained)*number(12.f);
    laser.parameters.target_length=(number(laser.parameters.target_length)-(number(base.length)-length)).to_float();base.length=length.to_float();
    if(length<number(18.f))base.delete_wait=1;
    while(index<samples.count){
        while(index<samples.count&&samples[index])++index;if(index>=samples.count)return;
        const i32 start=index;while(index<samples.count&&!samples[index])++index;
        auto child=laser.parameters;const auto length=Extended::from_int(index-start)*number(12.f);child.target_length=child.initial_length=length.to_float();
        if(number(18.f)<length){child.position=sweep.position(start,sweep.original);env.spawn_straight(child);}
    }
}
void split(TimedLaser& laser,const Sweep& sweep,const Samples& samples,LaserBehaviorEnvironment& env){
    if(!samples.hits)return;
    i32 index=0;while(index<samples.count&&samples[index])++index;
    if(index)laser.base.length=0;
    else{while(index<samples.count&&!samples[index])++index;if(index>=samples.count)return;laser.base.length=(Extended::from_int(index)*number(12.f)).to_float();}
    while(index<samples.count){
        while(index<samples.count&&samples[index])++index;if(index>=samples.count)return;
        const i32 start=index;while(index<samples.count&&!samples[index])++index;
        StraightLaserParameters child{};child.position=sweep.position(start,sweep.original);child.angle=laser.base.angle;
        child.target_length=child.initial_length=(Extended::from_int(index-start)*number(12.f)).to_float();
        child.maximum_distance=(number(laser.parameters.target_length)-Extended::from_int(start)*number(12.f)).to_float();
        child.width=laser.base.width;child.speed=8;child.sprite_type=laser.parameters.sprite_type;child.color=laser.parameters.color;env.spawn_straight(child);
    }
}
template<class Laser>
i32 cancel_whole(Laser& laser,i32 convert,bool clip_effects,LaserBehaviorEnvironment& env){
    Sweep sweep(laser.base,true);i32 result=0;
    if(laser.base.length>12.f){
        do{
            result=wrapping_add(result,1);
            if(!clip_effects||!laser_outside_playfield(sweep.point,16,16)){
                env.cancel_effect(wrapping_add(static_cast<i32>(laser.parameters.color)*2,17),sweep.point);
                if(convert&&!laser_outside_playfield(sweep.point,32,32))env.spawn_faith(sweep.point);
            }
            sweep.next();
        }while(number(sweep.distance)+number(6.f)<number(laser.base.length));
    }
    laser.base.state=1;return result;
}
}
i32 StraightLaser::cancel_all(i32 convert,LaserBehaviorEnvironment& env){return cancel_whole(*this,convert,false,env);}
i32 TimedLaser::cancel_all(i32 convert,LaserBehaviorEnvironment& env){return cancel_whole(*this,convert,true,env);}
i32 StraightLaser::cancel_rectangle(const Vec3& center,const Vec3& size,i32 convert,LaserBehaviorEnvironment& env){Sweep sweep(base,false);const auto samples=cancel_samples(*this,sweep,Rectangle(center,size),convert,true,env);split(*this,sweep,samples,env);return samples.hits;}
i32 StraightLaser::cancel_circle(const Vec3& center,float radius,i32 convert,LaserBehaviorEnvironment& env){Sweep sweep(base,false);const auto samples=cancel_samples(*this,sweep,Circle(center,radius),convert,true,env);split(*this,sweep,samples,env);return samples.hits;}
i32 TimedLaser::cancel_rectangle(const Vec3& center,const Vec3& size,i32 convert,LaserBehaviorEnvironment& env){Sweep sweep(base,false);const auto samples=cancel_samples(*this,sweep,Rectangle(center,size),convert,false,env);split(*this,sweep,samples,env);return samples.hits;}
i32 TimedLaser::cancel_circle(const Vec3& center,float radius,i32 convert,LaserBehaviorEnvironment& env){Sweep sweep(base,false);const auto samples=cancel_samples(*this,sweep,Circle(center,radius),convert,false,env);split(*this,sweep,samples,env);return samples.hits;}
// 0x41e4d0 / 0x41f670. This is an oriented rectangle query, including margins.
i32 EnemyLaser::intersects(const Vec3& point,float margin) const noexcept {
    const float x=Scalar::sub(point.x,position.x),y=Scalar::sub(point.y,position.y);
    const auto sin=sine(-number(angle)),cos=cosine(-number(angle));
    const float rotated_x=(number(x)*cos-sin*number(y)).to_float();const auto rotated_y=cos*number(y)+sin*number(x);
    const auto left=number(rotated_x)-number(margin);
    const float top=(rotated_y-number(margin)).to_float(),right=Scalar::add(rotated_x,margin),bottom=(rotated_y+number(margin)).to_float();
    return number(length)<left||number(width)*number(.5f)<number(top)||right<0.f||number(bottom)<number(width)*number(-.5f)?0:2;
}
}
