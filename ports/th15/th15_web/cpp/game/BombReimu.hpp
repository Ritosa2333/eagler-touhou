#pragma once
#include "BombController.hpp"
#include "DamageSources.hpp"
#include "EnemyState.hpp"
namespace th15 {
struct ReimuOrb {u32 animation=0;MotionState motion;i32 active=0;Timer age{0,0,0,0,0};Vec3 displacement{};u32 target=0;i32 index=0,damage_source=0;bool archived=false;};
class BombReimu final:public BombController {
    DamageSources& damage;EnemyWorldState* enemies;bool start()override;bool frame(bool&)override;bool stop_active()override;
    bool initialize(ReimuOrb&,i32);bool update_orb(ReimuOrb&);bool explode(ReimuOrb&);bool explode_all();bool cancel();bool interrupt(u32,i32);bool animation(u32&,i32,const Vec3&);
public:
    std::array<ReimuOrb,8> orbs;u32 aura=0;bool orbs_available=false;
    BombReimu(BombContext& c,DamageSources& d,EnemyWorldState* world):BombController(c),damage(d),enemies(world){}
};
}
