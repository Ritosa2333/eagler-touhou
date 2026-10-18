#pragma once
#include "BulletFrame.hpp"
#include "LaserManager.hpp"
#include "UpdateChain.hpp"
namespace th10 {
struct ProjectileSystemsEnvironment {
    UpdateChain* chain;
    UpdateChainEnvironment* callbacks;
    AnmAllocationEnvironment* animations;
    EnemyBulletManager** active_bullets;
    LaserManager** active_lasers;
    CallbackToken bullet_update,bullet_draw,laser_update,laser_draw;
    virtual AnmFile* load_bullet_animations()=0;
    virtual void discard_file_animations(AnmFile* file)=0;
    virtual void missing_bullet_animations()=0;
    virtual void* allocate_manager(u32 bytes)=0;
    virtual void release_manager(void* manager)=0;
    virtual void destroy_laser(EnemyLaser& laser)=0;
};
}
