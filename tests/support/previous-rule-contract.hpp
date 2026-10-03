#pragma once
#include "../../th08_web/cpp/multiplayer/SessionSetup.hpp"
#include <eagler/netplay/AdonisTiming.hpp>
// Frozen contracts before the cooperation changes (6/7) and from the prior
// 2026-10-02 rules branch (8/9). Both must reject admission and Replay playback.
inline std::uint32_t previous_rule_contract(const th08::multiplayer::SessionSetup& setup,bool rules20261002=false){
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){for(int i=0;i<4;++i){hash^=(value>>(8*i))&255u;hash*=16777619u;}};
    word(rules20261002?(setup.version==4?0x08000009u:0x08000008u):(setup.version==4?0x08000007u:0x08000006u));
    word(setup.player_count);word(setup.difficulty);word(setup.seed);
    for(auto value:setup.build)word(value);
    for(auto value:setup.characters)word(value);
    if(setup.input_delay||setup.prediction_limit!=8){word(0x54494d31u);word(setup.input_delay);word(setup.prediction_limit);}
    return hash;
}
// Frozen Adonis contract from 5fd2ce4, before adding the cooperation rules.
inline std::uint32_t previous_adonis_contract(const th08::multiplayer::SessionSetup& setup){
    std::uint32_t hash=2166136261u;
    const auto word=[&](std::uint32_t value){for(int i=0;i<4;++i){hash^=(value>>(8*i))&255u;hash*=16777619u;}};
    word(setup.version>=6?0x08000009u:setup.version==5?0x08000008u:setup.version==4?0x08000007u:0x08000006u);
    word(setup.player_count);word(setup.difficulty);word(setup.seed);
    for(auto value:setup.build)word(value);
    for(auto value:setup.characters)word(value);
    if(setup.input_delay||setup.prediction_limit!=8){word(0x54494d31u);word(setup.input_delay);word(setup.prediction_limit);}
    if(setup.version>=6){word(setup.input_delay_auto);word(setup.prediction_reserve);word(setup.measured_prediction);}
    return Netplay::AdonisGameplayAbi(hash,Netplay::AdonisMode(setup.adonis_mode),setup.input_delay);
}
