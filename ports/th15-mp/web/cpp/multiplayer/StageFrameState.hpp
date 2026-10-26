#pragma once
#include "../game/SessionGameplay.hpp"
#include "BattleFrameState.hpp"
namespace th15::multiplayer {
class StageFrameState {
    struct Data;
    std::unique_ptr<Data> saved_;
    static auto stage_fields(StageGameplay&);
    static auto background_fields(StageScene&);
    static auto hud_fields(GameHud&);
    static auto popup_fields(PopupManager&);
    static auto session_fields(SessionGameplay&);
public:
    mutable std::string error;
    StageFrameState();~StageFrameState();
    StageFrameState(StageFrameState&&)noexcept;
    StageFrameState& operator=(StageFrameState&&)noexcept;
    bool capture(StageGameplay&,SessionGameplay*,const std::vector<AnmVm*>& roots={});
    bool restore(StageGameplay&,SessionGameplay*)const;
    void discard()noexcept;
};
}
