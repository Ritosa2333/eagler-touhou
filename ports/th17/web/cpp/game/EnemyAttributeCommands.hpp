#pragma once
#include "EnemyState.hpp"
#include "EclContext.hpp"
namespace th17 {
struct EnemyAttributeHost {
    virtual ~EnemyAttributeHost()=default;
    virtual bool pause_animation(u32 handle,bool paused)=0;
    virtual bool sound(i32 id,const Vec3& position)=0;
};
class EnemyAttributeCommands final:public EclCommands {
    EnemyState& enemy;EnemyWorldState& world;
public:
    EnemyAttributeHost* host=nullptr;
    EnemyAttributeCommands(EnemyState& enemy,EnemyWorldState& world):enemy(enemy),world(world){}
    int execute(EclContext&,u16 opcode)override;
};
}
