#pragma once
#include "EclVariables.hpp"
#include <array>
namespace th09 {
enum class GameMode:i32 {story,extra,versus};
struct ScoreCounter {
    float lives=0;u32 displayed=0,points=0,increment=0;i32 extends=0;
    void advance() noexcept;
    bool eligible_for_extend(GameMode)const noexcept;
    bool extend(GameMode)noexcept;
    // The original accumulator stores score in units of ten points.
    void add(i32 amount)noexcept{points+=u32(amount/10);}
};
struct MatchProgress {
    i32 round_frames=0,reward_stage=2,reward_stage_frames=0,rank_interval=1800,maximum_rank=16;
    i32 reward_accumulator=0,total_frames=0,last_reward=-1,stage=0,round=0;
    u32 active_frames=0;
};
struct MatchRuleActions {
    virtual ~MatchRuleActions()=default;
    virtual void reward_enemy(i32 side,i32 reward)=0;
    virtual void play_sound(i32 id,i32 pan)=0;
    virtual void reward_notification(i32 side)=0;
};
class MatchRules {
    EclWorldState& world;MatchRuleActions& actions;
public:
    MatchProgress progress;GameMode mode=GameMode::story;
    std::array<ScoreCounter,2> scores;
    i32 attack_levels[2][2]{{1,1},{1,1}};
    MatchRules(EclWorldState& w,MatchRuleActions& a):world(w),actions(a){}
    i32 world_difficulty()const noexcept{return world.difficulty;}
    void initialize(i32 difficulty,GameMode,i32 stage);
    void restart_round(i32 round);
    // Called after the session's pause/transition gate. Dialogue suppresses
    // difficulty growth but the displayed score still catches up.
    void update(bool dialogue,u32 game_flags,u32 first_field_flags);
};
}
