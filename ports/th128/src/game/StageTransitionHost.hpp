#pragma once
#include "StageControl.hpp"
#include "StageSettlementHost.hpp"
#include "RouteHandoffRules.hpp"
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <vector>

namespace th128::stage {

inline constexpr std::uint32_t kStageTransitionObjectTarget = 0x4d2e68u;
inline constexpr std::int32_t kStageTransitionObjectValue = 12;
inline constexpr bool stageTransitionCompletesMessageEntry(int entry) noexcept { return entry == 1; }
inline constexpr bool stageTransitionMessageCompletionApplies(bool configured,bool begun,int entry) noexcept {
    return configured && begun && stageTransitionCompletesMessageEntry(entry);
}

struct StageControlSession {
    bool present = false;
    std::int32_t field74 = 0;
};

struct StageTransitionContext {
    int currentId = 0;
    std::int32_t progress = 0;
    std::int32_t gate74 = 0;
    std::int32_t message78 = 0;
    StageControlSession session;
    StageSettlementContext settlement;
};

struct StageTransitionCallbacks {
    StageSettlementCallbacks settlement;
    std::function<bool(const StageSettlementState&,std::string&)> settlementCommitted;
};

struct StageTransitionState {
    bool loaded = false;
    bool begun = false;
    bool settlementDone = false;
    bool controlApplied = false;
    bool routePlanned = false;
    bool requested = false;
    bool committed = false;
    bool messageDone = false;
    bool failed = false;
    int currentId = 0;
    std::int32_t progress = 0;
    int nextId = 0;
    std::int32_t nextProgress = 0;
    std::uint32_t controlFlags = 0;
    bool controlGate = false;
    std::int32_t controlCurrent = 0;
    std::int32_t controlPending = 0;
    bool controlActionRecreate = false;
    bool controlDestroyExisting = false;
    std::int32_t controlSessionArgument = 0;
    std::string error;
};

class StageTransitionHost {
public:
    bool load(std::span<const std::uint8_t> front, std::string& error);
    bool attachControl(Control& control, std::string& error);
    bool begin(const StageTransitionContext& context, const StageTransitionCallbacks& callbacks,
               std::string& error);
    bool acknowledgeControl(std::string& error);
    bool messageDoneForEntry(int entry, std::string& error);
    bool messageDone(std::string& error);
    bool interrupt(int code, std::string& error);
    bool tick(std::string& error);
    StageTransitionState state() const;
    StageSettlementState settlementState() const;
    std::vector<StageSettlementSnapshot> snapshots() const;

private:
    StageSettlementHost settlement_;
    StageTransitionContext context_;
    Control* control_ = nullptr;
    bool loaded_ = false, begun_ = false, settlementDone_ = false;
    bool controlApplied_ = false;
    bool controlActionRecreate_ = false, controlDestroyExisting_ = false;
    std::int32_t controlSessionArgument_ = 0;
    bool routePlanned_ = false, requested_ = false, committed_ = false;
    bool messageDone_ = false;
    int nextId_ = 0;
    std::int32_t nextProgress_ = 0;
    std::string failure_;
    bool fail(std::string reason, std::string& error);
};

}
