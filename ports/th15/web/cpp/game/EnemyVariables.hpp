#pragma once
#include "EclContext.hpp"
#include "EnemyState.hpp"
namespace th15 {
class EnemyVariables final:public EclVariables {
    EnemyState& enemy;EnemyWorldState& world;Rng& random;
    bool numeric(i32 index,float& value)noexcept;
public:
    EnemyVariables(EnemyState& enemy,EnemyWorldState& world,Rng& random):enemy(enemy),world(world),random(random){}
    bool integer(i32 id,i32& value)override;
    bool floating(i32 id,float& value)override;
    i32* integer_destination(i32 id)override;
    float* float_destination(i32 id)override;
};
}
