#include "netplay/NetplaySideEffects.hpp"

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

    std::cout << "TH07 rollback BGM reconciliation: PASS\n";
    return 0;
}
