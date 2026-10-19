#pragma once
#include "Enemy.hpp"
#include "AnmManager.hpp"
namespace th10 {
struct EnemyAnimationEnvironment {
    AnmManager* manager;
    AnmFile* const* files;
    AnmEnvironment* animations;
    AnmAllocationEnvironment* allocation;
    u32 create(i32 file,i32 script,u32 tag,AnimationPlacement placement) {
        return manager->create(*files[file],script,tag,placement,*animations,*allocation);
    }
};
}
