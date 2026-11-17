#include "ReplayState.hpp"
namespace th17 {
void ReplayRunState::assign(const ReplayRunState& source)noexcept{
    const i32 reward=source.roaring.reward_age.current,countdown=source.roaring.countdown.current;
    const Timer old_reward=roaring.reward_age,old_countdown=roaring.countdown;
    *this=source;roaring.reward_age=old_reward;roaring.countdown=old_countdown;
    roaring.reward_age.set(reward);roaring.countdown.set(countdown);
}
void ReplayStageState::restore_player(PlayerMotion& motion)const noexcept{
    motion.position_fixed=player;motion.position.x=float(player.x)*.0078125f;motion.position.y=float(player.y)*.0078125f;
    motion.focus=focus;for(u32 i=0;i<4;i++)motion.options[i].snap=1;motion.history.fill(player);
}
}
