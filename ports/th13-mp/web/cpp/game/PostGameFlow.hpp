#pragma once

#include <string>

#include "SessionState.hpp"
#include "Types.hpp"

namespace th13 {

enum class OverlayState : u8 {
    Monitor = 0,
    PauseEntry = 1,
    PracticeEntry = 2,
    PauseMenu = 3,
    QuitConfirm = 4,
    ReplaySaveConfirm = 5,
    ReplaySaveMenu = 6,
    ReplaySlots = 8,
    ReplayConfirm = 9,
    NameEntry = 10,
    Blank = 11,
    MessageWait = 12,
    GameOverEntry = 13,
    GameOverShow = 14,
    ContinueMenu = 15,
    GoReplaySave = 16,
    GoNameEntry = 17,
    GoPostName = 18,
    GoNoRankWait = 19,
    GoMessageWait = 20,
    ContinueExec = 21,
    BEntry = 22,
    BShow = 23,
    BGameOver = 24,
    BMenu = 25,
    BReplayConfirm = 26,
    BNameEntry = 27,
    BNoRank = 28,
    BExitExec = 29,
    BSpellVariant = 30,
    BClear = 31,
};

struct PostGamePlatform {
    virtual ~PostGamePlatform() = default;
    virtual void request_destination(i32 raw) = 0;
    virtual bool practice_end() { return true; }
    virtual bool request_practice_end() { return practice_end(); }
    virtual bool request_stage6_fade() { return false; }
    virtual bool request_extra_results() { return false; }
    virtual void bgm(i32 id) = 0;
};

class PostGameFlow {
public:
    PostGameFlow(SessionState& progress, HudPlayerState& player, HudScoreState& score, PostGamePlatform& host)
        : progress_(progress), player_(player), score_(score), host_(host) {}

    struct RollbackState {
        OverlayState state_;
        u32 age_;
        bool clear_sequence_;
        u32 clear_frame_;
    };
    RollbackState capture_rollback() const { return {state_, age_, clear_sequence_, clear_frame_}; }
    void restore_rollback(const RollbackState& state) {
        state_ = state.state_;
        age_ = state.age_;
        clear_sequence_ = state.clear_sequence_;
        clear_frame_ = state.clear_frame_;
    }

    OverlayState state() const { return state_; }
    u32 age() const { return age_; }
    bool frozen() const { return (progress_.scene_flags & 0x10u) != 0; }

    void on_stage_clear();
    void on_game_over();
    bool on_practice_end();
    void on_extra_end();

    std::string error;

    void update();

    bool clear_sequence() const { return clear_sequence_; }

private:
    void enter(OverlayState next);
    void apply_stage_clear_bonus();

    SessionState& progress_;
    HudPlayerState& player_;
    HudScoreState& score_;
    PostGamePlatform& host_;
    OverlayState state_ = OverlayState::Monitor;
    u32 age_ = 0;
    bool clear_sequence_ = false;
    u32 clear_frame_ = 0;
};

}
