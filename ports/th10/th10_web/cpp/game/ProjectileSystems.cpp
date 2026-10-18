#include "ProjectileSystems.hpp"
namespace th10 {
// 0x405cc0 / 0x41c100. The original constructors initialize embedded objects,
// then clear the complete manager. No embedded allocation survives that clear.
void EnemyBulletManager::initialize(ProjectileSystemsEnvironment& env) noexcept {__builtin_memset(this,0,sizeof(*this));*env.active_bullets=this;}
void LaserManager::initialize(ProjectileSystemsEnvironment& env) noexcept {__builtin_memset(this,0,sizeof(*this));*env.active_lasers=this;}
// 0x405e20 / 0x41c120. Registration ordering is part of the game frame order.
i32 EnemyBulletManager::start(ProjectileSystemsEnvironment& env){
    animation_file=env.load_bullet_animations();if(!animation_file){env.missing_bullet_animations();return -1;}
    cursor=pool;pool[2000].state=5;
    update_handle=env.chain->add(env.bullet_update,this,20,false,false,*env.callbacks);
    draw_handle=env.chain->add(env.bullet_draw,this,29,true,false,*env.callbacks);return 0;
}
i32 LaserManager::start(ProjectileSystemsEnvironment& env){
    animation_file=env.load_bullet_animations();if(!animation_file){env.missing_bullet_animations();return -1;}
    update_handle=env.chain->add(env.laser_update,this,19,false,false,*env.callbacks);
    draw_handle=env.chain->add(env.laser_draw,this,27,true,false,*env.callbacks);tail=&sentinel;return 0;
}
// 0x405ed0. Clearing embedded bullets differs from releasing their geometry at
// destruction: the reset deliberately zeroes the entire 2001-object pool.
void EnemyBulletManager::clear(ProjectileSystemsEnvironment& env){env.discard_file_animations(animation_file);__builtin_memset(pool,0,sizeof(pool));cursor=pool;pool[2000].state=5;}
// 0x405f70. Animation instances are discarded before reverse-order destruction.
void EnemyBulletManager::shutdown(ProjectileSystemsEnvironment& env){
    env.chain->remove_locked(update_handle,*env.callbacks);env.chain->remove_locked(draw_handle,*env.callbacks);
    env.discard_file_animations(animation_file);*env.active_bullets=nullptr;for(i32 i=2000;i>=0;--i)pool[i].release(*env.animations);
}
// 0x41c1c0. Preserve links read again after the destructor callback; count and
// tail are not maintained during this terminal teardown.
void LaserManager::shutdown(ProjectileSystemsEnvironment& env){
    env.chain->remove_locked(update_handle,*env.callbacks);env.chain->remove_locked(draw_handle,*env.callbacks);
    auto* laser=sentinel.next;while(laser){auto* next=laser->next;env.destroy_laser(*laser);laser->previous->next=laser->next;if(laser->next)laser->next->previous=laser->previous;env.release_manager(laser);laser=next;}
    *env.active_lasers=nullptr;
}
// 0x406060 / 0x41c290. A failed resource load tears down both registrations.
EnemyBulletManager* EnemyBulletManager::create(ProjectileSystemsEnvironment& env){
    auto* manager=static_cast<EnemyBulletManager*>(env.allocate_manager(sizeof(EnemyBulletManager)));manager->initialize(env);
    if(manager->start(env)){manager->shutdown(env);env.release_manager(manager);return nullptr;}return manager;
}
LaserManager* LaserManager::create(ProjectileSystemsEnvironment& env){
    auto* manager=static_cast<LaserManager*>(env.allocate_manager(sizeof(LaserManager)));manager->initialize(env);
    if(manager->start(env)){manager->shutdown(env);env.release_manager(manager);return nullptr;}return manager;
}
}
