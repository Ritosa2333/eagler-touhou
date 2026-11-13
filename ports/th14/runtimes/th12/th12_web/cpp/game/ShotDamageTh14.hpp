#pragma once
#include "DamageArea.hpp"
#include "GameEconomy.hpp"
#include "NativeStackTh14.hpp"
namespace th11 {
#if PORT_TITLE == 14
// Normalized fields of retail +de18, stride a4. Pool allocation, movement and
// shot ownership are separate from the 451430 collector.
struct Th14DamageArea {
    u32 flags=0;float radius=0,angle=0;Vec2 size;Vec3 position;Timer timer;
    float growth=0,angular_velocity=0;
    i32 damage=0,total=0,maximum=0,period=1;u32 stamp=0,callback=0,argument=0;
    Movement movement{};bool moving=false;
};
struct Th14DamageWorld {
    virtual ~Th14DamageWorld()=default;
    virtual bool hit(u32 index,Th14DamageArea&,const Vec3&,const Vec2*,float,float,i32&)=0;
};
bool th14_damage_circle_rectangle(Vec3 rectangle,Vec2 size,float angle,Vec3 circle,float radius,NativeStackTh14* stack=nullptr)noexcept;
bool th14_damage_rectangles(Vec3,Vec2,float,Vec3,Vec2,float)noexcept;
bool th14_damage_circle(std::array<Th14DamageArea,256>&,u32& cursor,Vec3,float radius,float growth,i32 duration,i32 damage,const float* rate,u32& index)noexcept;
bool th14_damage_rectangle(std::array<Th14DamageArea,256>&,u32& cursor,Vec3,Vec2,float angle,i32 duration,i32 damage,const float* rate,u32& index)noexcept;
void th14_damage_tick(Th14DamageArea&)noexcept;
bool th14_damage_collect(std::array<Th14DamageArea,256>&,Th14DamageWorld&,Vec3,const Vec2*,float radius,float angle,
    bool frame_advanced,bool probe,u32 argument,u32 stamp,i32 cap,GameEconomy&,i32& damage,bool& bomb,Vec3* last_hit,NativeStackTh14* stack=nullptr)noexcept;
#endif
}
