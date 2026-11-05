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

    std::cout << "TH07 rollback BGM reconciliation: PASS\n";
    return 0;
}
