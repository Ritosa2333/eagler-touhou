#pragma once
#include "BulletState.hpp"
namespace th08 {
struct BulletCancelActions {
    virtual ~BulletCancelActions()=default;
    virtual i32 barrier(BulletState& bullet)=0;
    virtual void drop(const Vec3& position,i32 type,i32 mode)=0;
};
// Original 00430830, including its repeated barrier check.
bool cancel_projectiles(BulletManagerState&,i32 mode,const i32& cancel_item,BulletCancelActions* actions);
void cancel_projectiles_near(BulletManagerState&,const Vec3&,float radius,BulletCancelActions&);
}
