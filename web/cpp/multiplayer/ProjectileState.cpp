#include "FrameImage.hpp"
#include "../game/BulletManager.hpp"
#include "../game/EnemyManager.hpp"
#include <algorithm>

namespace th17 {
void BulletManager::save_rollback(multiplayer::FrameImage& image){
    image.save(free_head,active_head,occupied,context,minimum_distance_squared,world_paused,
        cancellation_rewards,graze_history,error,visible_count,draw_groups);
    // Slot storage is addressed by BulletScene's permanent collision hosts.
    image.restore_with([this,saved=slots]{
        std::copy(saved.begin(),saved.end(),slots.begin());
        return true;
    });
}

void EnemyManager::save_rollback(multiplayer::FrameImage& image){
    image.save(active,world,timer,next_identifier,rate,background_delta,error);
    for(const auto& enemy:active){
        image.save(enemy->state,enemy->error);
        image.restore_with([enemy,scripts=enemy->scripts.snapshot(),error=enemy->scripts.error]{
            if(!enemy->scripts.restore(scripts))return false;
            enemy->scripts.error=error;
            return true;
        });
    }
}
}
