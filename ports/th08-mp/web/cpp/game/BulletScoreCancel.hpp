#pragma once
#include "BulletCancel.hpp"
namespace th08 {
struct BulletScoreCancelActions:BulletCancelActions {
    virtual void score_popup(const Vec3&,i32 score,u32 color)=0;
};
// Original 00430aa0. This also visits bullets already in cancellation state;
// lasers produce items without contributing to the returned score.
bool cancel_projectiles_for_score(BulletManagerState&,i32 maximum,bool laser_items,const i32& cancel_item,BulletScoreCancelActions&,i32& score);
}
