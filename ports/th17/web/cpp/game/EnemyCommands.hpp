#pragma once
#include "EnemyMovementCommands.hpp"
#include "EnemyAttributeCommands.hpp"
#include "EnemySpawnCommands.hpp"
#include "BulletCommands.hpp"
#include "LaserCommands.hpp"
#include "EnemySceneCommands.hpp"
namespace th17 {
class EnemyCommands final:public EclCommands {
    EnemyMovementCommands movement;
public:
    EnemyVisualCommands& visuals;EnemyAttributeCommands attributes;EnemySpawnCommands spawning;BulletCommands bullets;LaserCommands lasers;EnemySceneCommands scene;
    EnemyCommands(EnemyState& e,EnemyWorldState& w):movement(e,w),visuals(movement.visuals),attributes(e,w),spawning(e,w),bullets(e,w),lasers(e),scene(e,w){}
    int execute(EclContext& c,u16 op)override{if(EnemySceneCommands::handles(op))return scene.execute(c,op);if(op>=702&&op<=714)return lasers.execute(c,op);if((op>=600&&op<=612)||op==614||(op>=617&&op<=628)||op==640||op==641||op==700||op==701)return bullets.execute(c,op);if(op==300||op==301||op==304||op==305||op==309||op==310||op==311||op==312||op==321)return spawning.execute(c,op);if((op>=400&&op<=447)||op==504||op==505||(op>=302&&op<=336)||op==550||op==552)return movement.execute(c,op);return attributes.execute(c,op);}
};
}
