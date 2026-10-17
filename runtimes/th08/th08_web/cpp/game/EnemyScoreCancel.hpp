#pragma once
#include "Types.hpp"
namespace th08 {
struct EnemyScoreActions {
    virtual ~EnemyScoreActions()=default;
    virtual void item(const Vec3&,i32 type,i32 mode)=0;
    virtual void score_popup(const Vec3&,i32 score,u32 color)=0;
};
}
