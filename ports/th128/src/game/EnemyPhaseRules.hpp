#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace th128::enemy {
struct PhaseRecord {
    std::int32_t threshold=-1,limit=-1;
    std::string healthTarget,timeTarget;
};
struct PhaseTimer {
    std::int32_t previous=-1,current=0;
    float fraction=0;
    std::uint32_t clockSource=0,flags=0;
};
struct PhaseSpell {
    std::uint32_t flags=0;
    std::int32_t age=0;
    std::uint32_t value80=0;
    std::int32_t playerMode=0;
    std::int32_t id=-1,limit=0,baseBonus=0;
};
struct PhaseContext {
    std::array<PhaseRecord,8> records{};
    PhaseTimer timer;
    std::int32_t life=0,remaining=0,threshold=0;
    std::uint32_t flags=0;
    std::array<std::uint32_t,2> countdown{};
    std::optional<PhaseSpell> spell;
    std::int32_t managerWord18=0;
};
struct PhaseDecision {
    PhaseContext next;
    std::string target;
    bool timedOut=false;
};
bool registerPhase(std::array<PhaseRecord,8>& records,int slot,std::int32_t threshold,
                   std::int32_t limit,std::string target);
std::optional<PhaseDecision> queryPhase(const PhaseContext& context);
}
