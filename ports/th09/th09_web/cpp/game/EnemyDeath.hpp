#pragma once
#include "EclVm.hpp"
#include "AttackTransfer.hpp"
namespace th09 {
struct EnemyDeathActions {
    virtual ~EnemyDeathActions()=default;
    virtual void reset_chain()=0;
    virtual i32 chain_count()const=0;
    virtual void chain_kill(const Vec3&,i32 normal_attack,i32 spirit_attack,i32 character_attack,i32 score)=0;
    virtual void play_positioned_sound(i32,float x)=0;
    virtual void effect(i32 type,const Vec3&)=0;
    virtual void explosion(const Vec3&,float size,float speed,i32 kind,i32 sprite,i32 color)=0;
    virtual void drop_item(i32,const Vec3&)=0;
    virtual void charge(float amount)=0;
    virtual i32 opposing_spirits()const=0;
    virtual TransferParameters* create_transfer(i32 effect,const Vec3& position,const Vec3& control)=0;
};
class EnemyDeath {
public:
    static bool apply(EclVm&,i32 kill_source,const PlayfieldGeometry&,float opposing_width,EnemyDeathActions&);
};
}
