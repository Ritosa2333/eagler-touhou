#pragma once
#include "../th10_web/cpp/multiplayer/SessionSetup.hpp"

// Historical wire-contract fixture, independent of the current production
// version. It reconstructs historical identities without enabling legacy gameplay.
inline std::uint32_t historical_contract(
    const th10::multiplayer::SessionSetup& setup,std::uint32_t version){
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){
        for(unsigned i=0;i<4;++i){hash^=(value>>(8*i))&255u;hash*=16777619u;}
    };
    word(version);word(setup.playerCount);word(setup.difficulty);word(setup.seed);
    for(const auto& loadout:setup.loadouts){word(loadout.character);word(loadout.shot);}
    return hash;
}
