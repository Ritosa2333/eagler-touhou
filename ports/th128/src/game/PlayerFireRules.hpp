#pragma once
#include "PlayerHitRules.hpp"
#include "PlayerRules.hpp"

namespace th128::player::fire {
enum class Continuation { Emit, Cycle, Charge, Presentation, RequiredCall };
struct Plan {
    Continuation continuation=Continuation::Presentation;
    hit::Timer next{};
    int powerRaw=0,savedPower=0;
    float scale=1;
    std::uint32_t boundary=0,target=0;
    int argument=0;
};
std::optional<Plan> head(int playerState,Input input,hit::Timer shot) noexcept;
std::optional<Plan> charge(hit::Timer held,unsigned holding,int managerBusy,int systemMode,
                          int powerRaw,int savedPower,float scale) noexcept;
}
