#pragma once
#include "AttackTransfer.hpp"
#include "Bullet.hpp"
#include "PlayerMotion.hpp"
#include <array>
namespace th09 {
enum class AttackAreaKind:i32 {damage=0,cancel=1,reflect_damage=2,reflect=3,special_damage=4};
struct AttackArea {
    Vec2 position;float radius=0,radius_growth=0;Vec2 half_extent,extent_growth;float angle=0;
    i32 lifetime=0,damage=0,applied_damage=0,damage_limit=0,interval=1;AttackAreaKind kind=AttackAreaKind::damage;
    u8 active=0;u8 reserved[3]{};i32 delay=0;
};
struct AreaCollisionActions {
    virtual ~AreaCollisionActions()=default;
    virtual TransferParameters* create_transfer(i32 effect,const Vec3& from,const Vec3& target)=0;
    virtual void charge(float)=0;
    virtual void play_sound(i32,i32)=0;
    virtual void combo(const Vec3&,i32 normal,i32 spirit,i32 character,i32 score)=0;
    virtual void add_score(i32)=0;
};
struct AreaCollisionContext {
    Rng& random;AreaCollisionActions& actions;PlayfieldGeometry geometry[2];
    i32 side=0,difficulty=0,combo_count=0,transfer_kind=0;
};
class AttackAreas {
public:
    static constexpr u32 capacity=512;
    std::array<AttackArea,capacity+1> pool;
    std::array<AttackArea*,capacity+2> active{};
    std::array<AttackArea*,capacity> free{};
    i32 count=0,free_count=capacity;
    AttackAreas();
    AttackAreas(const AttackAreas&)=delete;
    AttackAreas& operator=(const AttackAreas&)=delete;
    void reset();
    AttackArea& create(const Vec3&,i32 lifetime,i32 delay);
    AttackArea& circle(const Vec3&,float radius,float growth,i32 damage,i32 lifetime,i32 delay,AttackAreaKind);
    AttackArea& box(const Vec3&,float width,float height,i32 damage,i32 lifetime,i32 delay,AttackAreaKind);
    void update();
    i32 cancel(const Vec3&,Bullet*,AreaCollisionContext&);
    i32 probe(const Vec3&,const Vec3& extent,const Bounds& player_graze,Bullet*,AreaCollisionContext&);
    i32 damage(const Vec3&,const Vec3& extent,i32& special_damage);
};
}
