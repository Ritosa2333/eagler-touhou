#pragma once
#include "Types.hpp"
#include <array>

namespace th08 {

enum class CombatItem : u8 {PowerSmall,Point,PowerBig,Bomb,PowerFull,Extend,PointStar,Time,PointSmall,Unknown,TimeBig};
struct CombatPlayer {Vec3 position{128,400,0};float hitbox=2.5f,grazebox=28;u8 focused=0,alive=1,youkai=0;};
struct CombatEnemy {u8 active=0,kind=0,flags=0,reserved=0;Vec3 position,velocity;float life=0,hitbox=16;};
struct CombatBullet {u8 active=0,kind=0,color=0,state=0;Vec3 position,velocity;float speed=0,angle=0,hitbox=3;u32 flags=0;u16 graze=0,despawn=0;};
struct CombatLaser {u8 active=0,kind=0,state=0,reserved=0;Vec3 position;float angle=0,start=0,end=0,length=0,width=4;u32 timer=0,duration=0;};
struct CombatItemState {u8 active=0,type=0,state=0,reserved=0;Vec3 position,velocity;u32 timer=0;};
struct CombatStats {u32 spawned_enemies=0,spawned_bullets=0,spawned_lasers=0,spawned_items=0,grazes=0,hits=0,cancelled_bullets=0;};

class CombatRuntime {
    static constexpr float Left=-32,Right=288,Top=-64,Bottom=480;
    u32 frame=0;
    std::array<CombatEnemy,481> enemies{};
    std::array<CombatBullet,0x600> bullets{};
    std::array<CombatLaser,0x100> lasers{};
    std::array<CombatItemState,2096> items{};
    CombatPlayer player{};CombatStats stats{};
public:
    void reset() noexcept;
    CombatPlayer& player_state() noexcept{return player;}
    const CombatPlayer& player_state()const noexcept{return player;}
    CombatStats& statistics() noexcept{return stats;}
    const CombatStats& statistics()const noexcept{return stats;}
    const auto& enemy_pool()const noexcept{return enemies;}
    const auto& bullet_pool()const noexcept{return bullets;}
    const auto& laser_pool()const noexcept{return lasers;}
    CombatEnemy* spawn_enemy(u8 kind,const Vec3& position,float life=100) noexcept;
    CombatBullet* spawn_bullet(u8 kind,const Vec3& position,float angle,float speed,u8 color=0) noexcept;
    CombatLaser* spawn_laser(u8 kind,const Vec3& position,float angle,float length,float width) noexcept;
    CombatItemState* spawn_item(CombatItem type,const Vec3& position) noexcept;
    void cancel_bullets(float radius=-1) noexcept;
    void cancel_lasers() noexcept;
    void clear_enemies() noexcept;
    void update(float rate=1) noexcept;
};
}
