#pragma once
#include "AnmExecutor.hpp"
#include "BulletEmission.hpp"
#include <array>
namespace th09 {
struct Laser {
    AnmVm body,glow;
    Vec3 position;float direction=0,start_offset=0,end_offset=0,length=0,width=0,active_width=0,speed=0;
    i32 appear=0,hit_start=0,duration=0,disappear=0,hit_end=0;
    u32 active=0;Timer time{0,0,0};u16 flags=0;i16 color=0;u8 phase=0,reserved=0;
};
struct LaserCollisionActions {
    virtual ~LaserCollisionActions()=default;
    virtual void add_laser_collision(const Vec2& center,const Vec2& size,const Vec3& pivot,float angle)=0;
};
class LaserManager {
    AnmLoaded& file;AnmExecutor& executor;
public:
    static constexpr u32 capacity=48;
    std::array<Laser,capacity+1> pool;
    LaserManager(AnmLoaded& f,AnmExecutor& e):file(f),executor(e){for(auto& l:pool){std::memset(&l.body,0,sizeof(l.body));std::memset(&l.glow,0,sizeof(l.glow));}}
    Laser* create(const BulletEmission&,const Vec3& player,i32 cancel_frames);
    bool update(const FrameTiming&,u32 field_flags,u32 game_flags,LaserCollisionActions&);
};
}
