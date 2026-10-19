#pragma once
#include "Types.hpp"
namespace th09 {
struct AttackQueueActions {
    virtual ~AttackQueueActions()=default;
    virtual void queue_attack(i32 kind,i32 source_side,const Vec3& position,const Vec3* extra)=0;
};
}
