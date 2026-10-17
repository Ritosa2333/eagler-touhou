#pragma once
#include "EclVm.hpp"
namespace th08 {
struct EnemyDropSequence {u16 enemies=0,item=0;};
struct EnemyDropActions {
    virtual ~EnemyDropActions()=default;
    virtual void effect(i32 kind,const Vec3& position,i32 count,u32 color)=0;
    virtual void item(const Vec3& position,i32 type,i32 mode)=0;
    virtual i32 power()=0;
};
// Original 0042bea0. The shared sequence is consumed only by default drops;
// scattered point/power items consume the game's shared random stream.
void drop_enemy_items(EclVm&,bool bomb,EnemyDropSequence&,Rng&,EnemyDropActions&);
}
