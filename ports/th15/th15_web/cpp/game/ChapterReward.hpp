#pragma once
#include "EnemyState.hpp"
#include "ItemCollection.hpp"
#include "AnmManager.hpp"
namespace th15 {
struct ChapterRewardState {u32 animation=0;i32 grazes=0;float percent=0,display_percent=0;i32 bonus=0,display_bonus=0,base_bonus=0,deaths=0;u32 flags=0;Timer age{0,0,0,0,0};i32 duration=0;};
class ChapterReward {
    AnmManager& animations;ItemScoreState& score;PlayerLifeSession& player;EnemyWorldState& enemies;ItemCollection& collection;i32 front;
    bool hide(i32 script);
public:
    ChapterRewardState state;std::string error;
    ChapterReward(AnmManager& a,ItemScoreState& s,PlayerLifeSession& p,EnemyWorldState& e,ItemCollection& c,i32 bank):animations(a),score(s),player(p),enemies(e),collection(c),front(bank){}
    ~ChapterReward(){animations.retire(state.animation);}
    bool complete(bool boss,i32 chapter_deaths);
};
}
