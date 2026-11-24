#pragma once
#include "animation/AnmLogic.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace th128::player {

struct StageClearSlot {
    std::uint32_t handle = 0;
    anm::VM* vm = nullptr;
};

struct PlayerStageClearSlotsState {
    bool configured = false;
    bool emptyContext = false;
    bool retired = false;
    bool failed = false;
    std::uint32_t configuredHandles = 0;
    std::uint32_t retiredHandles = 0;
};

class PlayerStageClearSlots {
public:
    using Retire = std::function<bool(std::uint32_t,anm::VM&,int,std::string&)>;
    using ChildVisit = std::function<bool(std::uint32_t,anm::VM&,std::string&)>;
    using Children = std::function<bool(std::uint32_t,anm::VM&,const ChildVisit&,std::string&)>;
    bool configure(const std::array<std::array<StageClearSlot,2>,8>& groups,bool explicitEmpty,
                   Retire retire,Children children,std::string& error);
    bool clear(std::string& error);
    PlayerStageClearSlotsState state() const;
private:
    std::array<std::array<StageClearSlot,2>,8> groups_{};
    Retire retire_;
    Children children_;
    bool configured_=false,emptyContext_=false,retired_=false,executing_=false;
    std::uint32_t configuredHandles_=0,retiredHandles_=0;
    std::string failure_;
    bool fail(std::string message,std::string& output);
};

}
