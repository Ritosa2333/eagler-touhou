#pragma once
#include "Timer.hpp"
#include <list>
#include <memory>
#include <string>
namespace th15 {
enum class LaserKind:i32 { moving=0, stationary=1, curved=2, segmented=3 };
class LaserObject {
public:
    u32 flags=0,id=0;i32 state=0;LaserKind kind=LaserKind::moving;Timer age;
    Timer outside_delay{0,0,0,0,0};i32 outside_count=0;std::string error;
    LaserObject(){age.set(0);}virtual ~LaserObject()=default;
    virtual bool update(float rate,bool& finished)=0;
    virtual bool update_visual(float rate,bool paused)=0;
    virtual bool retire()=0;
    virtual i32 cancel_rectangle(const Vec3&,const Vec2&,float angle,i32 reward,bool)=0;
    virtual i32 query_rectangle(const Vec3&,const Vec2&,float angle,i32,i32,i32)=0;
    virtual i32 cancel_circle(const Vec3&,float radius,i32 reward,bool)=0;
    virtual bool cancel_all(i32 reward,bool)=0;
    virtual i32 query_circle(const Vec3&,float radius)=0;
};
class LaserManager {
    std::list<std::unique_ptr<LaserObject>> objects;
public:
    static constexpr u32 capacity=512;u32 generation=0;Vec3 query_position{},query_size{};i32 query_count=0;std::string error;
    u32 insert(std::unique_ptr<LaserObject>);
    LaserObject* find(u32)const noexcept;
    u32 count()const noexcept{return u32(objects.size());}
    bool update(float rate,u32 game_flags=0);
    i32 cancel_rectangle(const Vec3&,const Vec2&,float angle,i32 reward);
    i32 query_rectangle(const Vec3&,const Vec2&,float angle,i32,i32,i32);
    i32 cancel_circle(const Vec3&,float radius,i32 reward,bool);
    bool cancel_all(i32 reward,bool);bool clear_with_rewards();bool clear();
    i32 query_circle(const Vec3&,float radius);
    template<class F> void each(F&& fn){for(auto& p:objects)fn(*p);}
};
}
