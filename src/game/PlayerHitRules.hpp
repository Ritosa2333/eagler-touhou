#pragma once

#include <array>
#include <cstdint>
#include <optional>

namespace th128::player::hit {

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

struct PlayerGate {
    std::int32_t stateF78;
    std::uint32_t flags438ec;
    std::int32_t counter438dc;
};

struct HitControl {
    bool present;
    std::int32_t counter5a1c;
};

enum class GateOutcome : std::uint8_t { Suppressed, HitWithoutHitCall, HitCallRequired };

struct GateDecision {
    GateOutcome outcome;
    std::optional<std::int32_t> helperReturn;
    std::uint32_t callSite;
    std::uint32_t callTarget;
};

enum class BankSource : std::uint32_t { None, BulletManagerAnm8a7f9c, PlayerField10 };
enum class CallKind : std::uint32_t {
    AllocateHitEffect, Sound, LoadPlayerAnimation, SoundOperation458590, CommitDeath, BeginDeathbomb,
    NormalInitDetach
};

struct CallRequest {
    CallKind kind;
    std::uint32_t callSite;
    std::uint32_t callTarget;
    std::int32_t argument;
    std::uint32_t playerOffset;
    BankSource bankSource;
};

using CallWords = std::array<std::uint32_t, 6>;

struct EntryPrefix {
    std::int32_t stateF78;
    Position deferredEffectPosition;
    CallRequest requiredCall;
};

struct SoundControl {
    bool present;
    std::uint32_t flags60;
};

struct TimerPrefix {
    Timer updated;
    std::optional<CallRequest> requiredCall;
    std::uint32_t continuation;
};

struct WindowControl {
    bool present;
    std::int32_t mode3c;
    std::int32_t perfectCharge4b4d70;
    std::uint32_t keys4db210;
};

enum class WindowOutcome : std::uint8_t {
    NoCall, DeathCommitRequired, HeldFreezeSoundRequired, DeathbombSystemRequired
};

struct WindowPrefix {
    WindowOutcome outcome;
    Timer stateTimer;
    Timer heldTimer;
    std::optional<CallRequest> requiredCall;
    std::uint32_t continuation;
};

struct TailState {
    std::uint32_t flags7c;
    std::int32_t counter28;
    std::int32_t value80;
};

struct TailControl {
    bool present;
    std::int32_t mode3c;
};

struct NormalInitPrefix {
    std::int32_t stateF78;
    std::uint32_t flags438ec;
    Timer stateTimer;
    Timer focusTimer;
    Timer rechargeTimer;
    std::int32_t fieldF70;
    std::int32_t fieldF74;
    CallRequest requiredCall;
};

GateDecision gateDecision(PlayerGate player, HitControl control) noexcept;
std::optional<NormalInitPrefix> normalInitPrefix(Timer stateTimer, Timer focusTimer,
                                                Timer rechargeTimer, std::uint32_t flags438ec) noexcept;
std::optional<Timer> constructorCounterPrefix(Timer initializedCounter) noexcept;
std::optional<Timer> counterStep(Timer counter, std::optional<float> resolvedClock) noexcept;
std::optional<EntryPrefix> entryPrefix(std::int32_t stateF78, Position playerPosition) noexcept;
std::optional<TimerPrefix> timerResetPrefix(Timer stateTimer, SoundControl control) noexcept;
std::optional<TimerPrefix> counterResetPrefix(Timer hitCounter) noexcept;
std::optional<WindowPrefix> windowPrefix(std::int32_t stateF78, Timer stateTimer,
                                        Timer heldTimer, WindowControl control) noexcept;
std::optional<TailState> tailPrefix(TailState state, TailControl control) noexcept;
CallWords serializeCallRequest(const CallRequest& request) noexcept;

}
