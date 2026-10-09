#pragma once
#ifndef TH11_MULTIPLAYER
#error TH11 Multiplayer must remain outside the ordinary source set
#endif
#include <array>
#include <cstddef>
#include <cstdint>

namespace th11::multiplayer {
struct SessionSetup {
    static constexpr std::uint32_t Version=5, Words=22, GameplayVersion=1;
    std::uint64_t session_id=0;
    std::uint32_t player_count=2,local_player=0,difficulty=1,seed=1;
    std::uint32_t requested_delay=0,input_delay=0,prediction_reserve=2;
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
