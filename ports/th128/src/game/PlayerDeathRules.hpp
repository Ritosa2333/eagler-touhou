#pragma once

#include <array>
#include <cstdint>
#include <optional>

namespace th128::player::death {

struct Position {
    float horizontal;
    float vertical;
    float depth;
};

struct Timer {
    std::int32_t previous;
    std::int32_t current;
    float fractional;
    std::uint32_t clockSource;
    std::uint32_t flags;
};

enum class Binding : std::uint32_t {
    None, Control4b8950, PlayerAnimationField10, PlayerPositionEcc, GlobalState4b4cc0
};

enum class CallKind : std::uint32_t {
    MotivationDelta = 0, PerfectChargeDelta = 1, Sound = 2, ChargeNotify424b80 = 3,
    ChargeUi425820 = 4, ControlRefresh41fc80 = 5, PlayerAnimation = 6,
    Scene438030 = 8, Scene437050 = 9, RespawnField43df90 = 10, PostRespawn427190 = 11
};

struct CallRequest {
    CallKind kind;
    std::uint32_t callSite;
    std::uint32_t callTarget;
    Binding binding;
    std::array<std::uint32_t, 6> operands;
};

using CallWords = std::array<std::uint32_t, 10>;

struct MotivationSnapshot {
    std::int32_t value4b4d64;
    std::int32_t flag4b4d68;
};

struct ChargeAwardPrefix {
    std::int32_t updatedCharge;
    std::array<CallRequest, 3> requiredCalls;
    std::uint32_t requestCount;
    bool completedIndependentHelper;
};

struct AwardControl {
    std::int32_t selector4b4cfc;
    std::int32_t value4b4d00;
};

struct AfterAwardsPrefix {
    std::int32_t rawPower;
    std::optional<CallRequest> requiredCall;
    std::uint32_t continuation;
};

struct StateTwoPrefix {
    std::int32_t stateF78;
    Timer stateTimer;
    Timer protectionTimer;
    CallRequest requiredCall;
    std::uint32_t continuation;
};

struct CleanupRecord {
    std::uint32_t active;
    std::uint32_t primaryHandle;
    std::uint32_t secondaryHandle;
};

enum class CleanupEntry : std::uint8_t { RecordStart, AfterPrimary };

struct CleanupPrefix {
    std::uint32_t activeAfter;
    std::uint32_t requiredHandle;
    std::uint32_t phase;
    std::uint32_t lookupInstruction;
    std::uint32_t continuation;
};

struct TailSnapshot {
    std::uint32_t flags7c;
    std::int32_t counter28;
    std::int32_t value80;
    std::int32_t scalar4b4d30;
};

struct TailControl {
    bool present;
    std::int32_t mode3c;
};

struct TailPrefix {
    TailSnapshot updated;
    std::int32_t field438f8;
};

struct SceneSnapshot {
    bool present;
    std::int32_t mode10;
};

enum class RespawnOutcome : std::uint8_t { Waiting, SceneDefaultRequired, SceneOneRequired, RespawnFieldRequired };

struct RespawnGatePrefix {
    RespawnOutcome outcome;
    std::int32_t stateF78;
    std::optional<float> clockWrite;
    std::optional<CallRequest> requiredCall;
    std::uint32_t continuation;
};

struct AfterFieldPrefix {
    std::optional<CallRequest> requiredCall;
    std::uint32_t continuation;
};

struct RespawnPositionPrefix {
    Position position;
    Position savedPosition;
    std::int32_t rawHorizontal;
    std::int32_t rawVertical;
    Timer protectionTimer;
    Timer stateTimer;
};

std::array<CallRequest, 2> commitCalls() noexcept;
std::optional<MotivationSnapshot> motivationAfterDeath(MotivationSnapshot state) noexcept;
std::optional<ChargeAwardPrefix> chargeAwardPrefix(std::int32_t perfectCharge) noexcept;
std::array<CallRequest, 2> chargeCompletionCalls() noexcept;
AfterAwardsPrefix afterAwardsPrefix(AwardControl control) noexcept;
std::optional<StateTwoPrefix> stateTwoSetupPrefix(Timer stateTimer, Timer protectionTimer) noexcept;
std::optional<CleanupPrefix> cleanupRecordPrefix(CleanupRecord record, CleanupEntry entry) noexcept;
std::optional<TailPrefix> tailPrefix(TailSnapshot state, TailControl control) noexcept;
std::optional<RespawnGatePrefix> respawnGatePrefix(std::int32_t stateF78, std::int32_t elapsedF84,
    std::int32_t resource4b4d64, SceneSnapshot scene, Position position) noexcept;
AfterFieldPrefix respawnAfterFieldPrefix(std::int32_t difficulty4b4d04) noexcept;
std::optional<RespawnPositionPrefix> respawnPositionPrefix(std::int32_t stateF78, Position position,
    Timer protectionTimer, Timer stateTimer) noexcept;
CallWords serializeCallRequest(const CallRequest& request) noexcept;

}
