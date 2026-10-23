#pragma once
#include <cstdint>

namespace th08 {

// Read-only snapshot of a completed PlayerSimulation consumer boundary.
// Signed game values are transported as their u32 two's-complement bits.
class TouchActionStateV1 {
public:
    using Word = std::uint32_t;
    static constexpr Word version = 1;
    static constexpr Word word_count = 20;
    enum Valid : Word {
        TickAndStage = 1u << 0,
        ConsumedInput = 1u << 1,
        Focus = 1u << 2,
        ShootingTimer = 1u << 3,
        ActiveShots = 1u << 4,
        Bomb = 1u << 5,
        BombInventory = 1u << 6,
        Life = 1u << 7,
    };
    enum Phase : Word { None=0, ConsumerRun=1, Paused=2 };
    enum Offset : Word {
        Version=0, ValidMask=1, PhaseWord=2, PlayerUpdateSequence=3,
        ControlFrame=4, Stage=5, ConsumedCurrent=6, ConsumedPrevious=7,
        Focused=8, ShootingTimerCurrent=9, ShootingTimerPrevious=10,
        ActivePlayerShots=11, BombActive=12, BombTriggered=13, BombType=14,
        BombTimerCurrent=15, BombsRemaining=16, BombsUsed=17,
        BombsUsedStage=18, LifeState=19,
    };

    TouchActionStateV1() noexcept { reset(); }

    void reset() noexcept { for(auto& word:words_)word=0; words_[Version]=version; }

    void publish_consumer(Word sequence,Word frame,Word stage,
                          Word current,Word previous,Word focused,
                          Word shootingCurrent,Word shootingPrevious,
                          Word activeShots,Word bombActive,Word bombTriggered,
                          Word bombType,Word bombTimer,Word bombsRemaining,
                          Word bombsUsed,Word bombsUsedStage,Word lifeState) noexcept {
        reset();
        words_[ValidMask]=TickAndStage|ConsumedInput|Focus|ShootingTimer|
            ActiveShots|Bomb|BombInventory|Life;
        words_[PhaseWord]=ConsumerRun;
        words_[PlayerUpdateSequence]=sequence;
        words_[ControlFrame]=frame;
        words_[Stage]=stage;
        words_[ConsumedCurrent]=current;
        words_[ConsumedPrevious]=previous;
        words_[Focused]=focused;
        words_[ShootingTimerCurrent]=shootingCurrent;
        words_[ShootingTimerPrevious]=shootingPrevious;
        words_[ActivePlayerShots]=activeShots;
        words_[BombActive]=bombActive;
        words_[BombTriggered]=bombTriggered;
        words_[BombType]=bombType;
        words_[BombTimerCurrent]=bombTimer;
        words_[BombsRemaining]=bombsRemaining;
        words_[BombsUsed]=bombsUsed;
        words_[BombsUsedStage]=bombsUsedStage;
        words_[LifeState]=lifeState;
    }

    void publish_paused(Word frame,Word stage,Word lifeState) noexcept {
        reset();
        words_[ValidMask]=TickAndStage|Life;
        words_[PhaseWord]=Paused;
        words_[ControlFrame]=frame;
        words_[Stage]=stage;
        words_[LifeState]=lifeState;
    }

    const Word* data() const noexcept { return words_; }

private:
    Word words_[word_count]{};
};

static_assert(sizeof(TouchActionStateV1)==TouchActionStateV1::word_count*sizeof(std::uint32_t));

} // namespace th08
