#include "netplay/NetplaySideEffects.hpp"
#include <eagler/audio/ReplayMusicScope.hpp>

#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    char path[256] = {};
    Netplay::SideEffects::ResetBgmHistory();

    Netplay::SideEffects::SetSpeculative(false);
    assert(!Netplay::SideEffects::ObserveBgmPlay("bgm/stage.mid"));

    Netplay::SideEffects::SetSpeculative(true);
    assert(Netplay::SideEffects::ObserveBgmPlay("bgm/stage.mid"));
    assert(!Netplay::SideEffects::ConsumeCorrectedBgmPlay(path, sizeof(path)));

    assert(Netplay::SideEffects::ObserveBgmPlay("bgm/boss.mid"));
    assert(Netplay::SideEffects::ObserveBgmPlay("bgm/stage.mid"));
    assert(!Netplay::SideEffects::ConsumeCorrectedBgmPlay(path, sizeof(path)));

    assert(Netplay::SideEffects::ObserveBgmPlay("bgm/boss.mid"));
    Netplay::SideEffects::SetSpeculative(false);
    assert(Netplay::SideEffects::ConsumeCorrectedBgmPlay(path, sizeof(path)));
    assert(std::strcmp(path, "bgm/boss.mid") == 0);
    assert(!Netplay::SideEffects::ConsumeCorrectedBgmPlay(path, sizeof(path)));

    // Restoring/seeking a read-only Replay emits its persistent music state,
    // while the same historical scope continues to suppress transient SFX.
    Netplay::SideEffects::SetSpeculative(true);
    {
        eagler::audio::ReplayMusicScope replay;
        assert(Netplay::SideEffects::IsSpeculative());
        assert(!Netplay::SideEffects::SuppressMusic());
        assert(!Netplay::SideEffects::ObserveBgmPlay("bgm/stage.mid"));
        {
            eagler::audio::ReplayMusicScope nested;
            assert(!Netplay::SideEffects::ObserveBgmPlay("bgm/boss.mid"));
        }
        assert(!Netplay::SideEffects::SuppressMusic());
        assert(!Netplay::SideEffects::ConsumeCorrectedBgmPlay(path, sizeof(path)));
    }
    assert(Netplay::SideEffects::SuppressMusic());
    assert(Netplay::SideEffects::ObserveBgmPlay("bgm/stage.mid"));
    Netplay::SideEffects::SetSpeculative(false);
    assert(Netplay::SideEffects::ConsumeCorrectedBgmPlay(path, sizeof(path)));
    assert(std::strcmp(path, "bgm/stage.mid") == 0);

    using namespace Netplay::SideEffects;
    assert(!RecordCommittedMiss(0));
    assert(MissFrameVisible(CurrentMissFrameTag()));
    MissFrameTag tag;
    {
        SimulationFrameScope frame(71, 0, false);
        MissScope miss;
        tag = CurrentMissFrameTag();
        assert(tag.deferred && RecordCommittedMiss(0));
        assert(!RecordCommittedMiss(3));
    }
    assert(!MissFrameVisible(tag));
    assert(ConsumeConfirmedMisses(71, 0xffffffffu) == 0);
    assert(ConsumeConfirmedMisses(71, 0) == 1);
    assert(MissFrameVisible(tag));
    assert(ConsumeConfirmedMisses(71, 0) == 0);

    // The original prediction commits two deaths. Corrections remove both,
    // then reintroduce only P2. Neither sound nor particles are presented early.
    for (unsigned attempt = 0; attempt < 6; ++attempt)
    {
        SimulationFrameScope frame(71, 5, attempt != 0);
        MissScope miss;
        tag = CurrentMissFrameTag();
        if (attempt % 2 == 0) { assert(RecordCommittedMiss(0)); assert(RecordCommittedMiss(1)); }
        assert(!MissFrameVisible(tag));
    }
    assert(ConsumeConfirmedMisses(71, 5) == 0);
    {
        SimulationFrameScope frame(71, 6, false);
        MissScope miss;
        tag = CurrentMissFrameTag();
        assert(RecordCommittedMiss(1));
    }
    assert(ConsumeConfirmedMisses(71, 5) == 0 && !MissFrameVisible(tag));
    assert(ConsumeConfirmedMisses(71, 6) == 2 && MissFrameVisible(tag));
    for (unsigned attempt = 0; attempt < 4; ++attempt)
    {
        { SimulationFrameScope frame(71, 6, true); assert(RecordCommittedMiss(1)); }
        assert(ConsumeConfirmedMisses(71, 6) == 0);
    }
    for (unsigned frame = 10; frame < 22; ++frame)
    {
        { SimulationFrameScope scope(71, frame, false); assert(RecordCommittedMiss(0)); }
        { SimulationFrameScope scope(71, frame, true); }
    }
    assert(ConsumeConfirmedMisses(71, 21) == 0);
    {
        SimulationFrameScope frame(72, 0, false);
        assert(!MissFrameVisible(tag));
        assert(!CurrentMissFrameTag().deferred);
        { MissScope a; MissScope b; assert(CurrentMissFrameTag().deferred); }
        assert(!CurrentMissFrameTag().deferred);
        assert(RecordCommittedMiss(0) && RecordCommittedMiss(2));
    }
    assert(ConsumeConfirmedMisses(71, 100) == 0);
    assert(ConsumeConfirmedMisses(72, 0) == 5);
    assert(ConsumeConfirmedMisses(72, 0) == 0);
    std::cout << "TH06 rollback BGM and confirmed Miss presentation: PASS\n";
    return 0;
}
