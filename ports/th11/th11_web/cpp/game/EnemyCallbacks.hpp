#pragma once
#include "EnemyState.hpp"
#include "EnemyCommands.hpp"
#include "AnmVm.hpp"
namespace th11 {
// Script table indices name recovered behavior, never executable addresses.
enum class EnemyTick:u32 {
    None,RotateWarningLasers,KillMarkedEnemies,SignalMarkedEnemies,ReleaseMarkedEnemies,
    AttractPlayerAndBullets,SpawnEnemiesFromBullets,BoundaryEmitter,BulletEmitter,
    HideIndicator,TransformCircle,ResetIndicator,TransformAngles
};
enum class EnemyDamage:u32 {None,Arc};
enum class EnemyCollision:u32 {None,Arc};
struct EnemyCallbackWorld {
    virtual ~EnemyCallbackWorld()=default;
    virtual EnemyLink* callback_enemies()=0;
    virtual bool callback_spawn(const EnemyState&,Vec3)=0;
    virtual bool callback_move_player(Vec3)=0;
    virtual bool callback_indicator(i16 interrupt,bool update)=0;
    virtual AnmVm* callback_animation(u32& id)=0;
    virtual bool callback_damage(Vec3,Vec2,i32&)=0;
    virtual bool callback_collision(Vec3,float,float,float)=0;
};
struct EnemyCallbackControl {
    virtual ~EnemyCallbackControl()=default;
    virtual bool invoke_tick(EnemyState&,EnemyTick,i32&)=0;
};
class EnemyCallbacks final:public EnemyCallbackControl {
public:
    EnemyCallbacks(EnemyEnvironment& e,EnemyCommandEnvironment& c,EnemyCallbackWorld& w):environment(e),commands(c),world(w){}
    bool invoke_tick(EnemyState&,EnemyTick,i32&)override;
    bool damage_arc(EnemyState&,i32&);
    bool collision_arc(EnemyState&);
private:
    EnemyEnvironment& environment;EnemyCommandEnvironment& commands;EnemyCallbackWorld& world;
    bool marked_enemies(EnemyTick);
    bool attract(EnemyState&);
    bool spawn_from_bullets(EnemyState&);
    bool boundary_emitter(EnemyState&);
    bool bullet_emitter(EnemyState&);
    bool transform(EnemyState&,bool angular);
    bool fire(EnemyState&,u32 slot);
};
}
