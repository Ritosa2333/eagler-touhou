#pragma once
#ifndef TH12_MULTIPLAYER
#error TH12 Multiplayer must remain outside the ordinary source set
#endif
#include <array>
#include <cstddef>
#include <cstdint>

namespace th12::multiplayer {
struct SessionSetup {
    static constexpr std::uint32_t Version=6, LegacyVersion=5, Words=22, GameplayVersion=7, MaxRollbackFrames=12;
    // Legacy is still the default for existing pure-delay callers and Replays.
    std::uint32_t version=LegacyVersion,mode=1;
    std::uint64_t session_id=0;
    std::uint32_t player_count=2,local_player=0,difficulty=1,seed=1;
    std::uint32_t requested_delay=0,input_delay=0,prediction_reserve=2;
    // Fixed title history budget. Negotiated Hybrid allowance is phase advice,
    // never a smaller rollback window and never an additional local delay.
    std::uint32_t prediction_limit=MaxRollbackFrames,measured_prediction=0;
    bool automatic=true,challenge=false;
    std::array<std::uint32_t,3> selections{};
    std::array<std::uint32_t,4> build{};
    std::uint32_t initial_stage()const{return difficulty==4?7:1;}
    bool valid()const;
};
bool DecodeSessionSetup(SessionSetup&,const std::uint32_t*,std::size_t);
std::array<std::uint32_t,SessionSetup::Words> EncodeSessionSetup(const SessionSetup&);
std::uint32_t GameplayAbi(const SessionSetup&);
}
