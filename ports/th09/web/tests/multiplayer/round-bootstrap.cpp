#include "../../cpp/game/MatchRules.hpp"
#include <cassert>
#include <cstdio>
using namespace th09;
struct Actions final : MatchRuleActions {
    void reward_enemy(i32,i32) override {}
    void play_sound(i32,i32) override {}
    void reward_notification(i32) override {}
};
int main() {
    EclWorldState shortWorld{},longWorld{}; Actions actions;
    MatchRules shortRound(shortWorld,actions),longRound(longWorld,actions);
    shortRound.initialize(2,GameMode::versus,9);
    longRound.initialize(2,GameMode::versus,9);
    for(int n=0;n<1000;++n) shortRound.update(false,0,0);
    for(int n=0;n<7000;++n) longRound.update(false,0,0);
    const auto shortSeed=shortWorld.random.seed,longSeed=longWorld.random.seed;
    const auto shortReward=shortRound.progress.last_reward,longReward=longRound.progress.last_reward;
    shortRound.scores[0].points=100;longRound.scores[0].points=900;
    shortRound.restart_round(1);longRound.restart_round(1);
    assert(shortRound.progress.round==1 && longRound.progress.round==1);
    assert(shortRound.progress.round_frames==0 && longRound.progress.round_frames==0);
    assert(shortWorld.rank!=longWorld.rank);
    assert(shortRound.attack_levels[0][0]!=longRound.attack_levels[0][0]);
    assert(shortRound.scores[0].points==100 && longRound.scores[0].points==900);
    assert(shortWorld.random.seed==shortSeed && longWorld.random.seed==longSeed);
    assert(shortRound.progress.last_reward==shortReward && longRound.progress.last_reward==longReward);
    std::printf("Round reset counterexample: same round=1 / frame=0, ranks=%d/%d, attack levels=%d/%d, scores=100/900. RNG and last reward retained. New round alone is not a replay initializer. PASS\n",
        shortWorld.rank,longWorld.rank,shortRound.attack_levels[0][0],longRound.attack_levels[0][0]);
}
