#include "PlayerHitRules.hpp"

#include <cmath>
#include <limits>

namespace th128::player::hit {
namespace {

static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559);

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 65536.0f;
}

bool boundedTimerValue(float value) noexcept {
    // Timers can outlive the coordinate domain; conversion still requires int32 range.
    return std::isfinite(value)
        && static_cast<double>(value) >= std::numeric_limits<std::int32_t>::min()
        && static_cast<double>(value) <= std::numeric_limits<std::int32_t>::max();
}

bool bounded(Timer timer) noexcept {
    return boundedTimerValue(timer.fractional);
}

Timer setTimer(Timer timer, std::int32_t value) noexcept {
    if (!(timer.flags & 1U)) {
        timer.clockSource = 0x4b6f58;
        timer.flags |= 1U;
    }
    timer.previous = value - 1;
    timer.current = value;
    timer.fractional = static_cast<float>(value);
    return timer;
}

}

GateDecision gateDecision(PlayerGate player, HitControl control) noexcept {
    if ((control.present && control.counter5a1c != 0) || player.stateF78 == 2
        || player.stateF78 == 4 || player.stateF78 == 3 || (player.flags438ec & 2U)) {
        return {GateOutcome::Suppressed, 0, 0, 0};
    }
    if (player.counter438dc > 0) return {GateOutcome::HitWithoutHitCall, 1, 0, 0};
    return {GateOutcome::HitCallRequired, std::nullopt, 0x43c648, 0x43d0c0};
}

std::optional<NormalInitPrefix> normalInitPrefix(Timer stateTimer, Timer focusTimer,
                                                Timer rechargeTimer, std::uint32_t flags438ec) noexcept {
    if (!bounded(stateTimer) || !bounded(focusTimer) || !bounded(rechargeTimer)) return std::nullopt;
    return NormalInitPrefix{1, flags438ec & 0xfffffff8U,
        setTimer(stateTimer, 0), setTimer(focusTimer, 0), setTimer(rechargeTimer, -1), 0, 0,
        {CallKind::NormalInitDetach, 0x43ac1e, 0x4679b0, 0, 0x87a8, BankSource::None}};
}

std::optional<Timer> constructorCounterPrefix(Timer initializedCounter) noexcept {
    if (!bounded(initializedCounter) || !(initializedCounter.flags & 1U)) return std::nullopt;
    return setTimer(initializedCounter, 120);
}

std::optional<Timer> counterStep(Timer counter, std::optional<float> resolvedClock) noexcept {
    if (!bounded(counter)) return std::nullopt;
    if (counter.current <= 0) return counter;
    if (counter.clockSource != 0x4b6f58 || !resolvedClock || !std::isfinite(*resolvedClock)
        || *resolvedClock < 0 || *resolvedClock > 4) {
        return std::nullopt;
    }
    const auto clock = *resolvedClock;
    const auto step = clock > 0.9900000095367431640625 && clock < 1.0099999904632568359375 ? 1.0f : clock;
    const auto fractional = static_cast<float>(static_cast<double>(counter.fractional) - step);
    if (!boundedTimerValue(fractional)) return std::nullopt;
    counter.previous = counter.current;
    counter.fractional = fractional;
    counter.current = static_cast<std::int32_t>(fractional);
    return counter;
}

std::optional<EntryPrefix> entryPrefix(std::int32_t, Position playerPosition) noexcept {
    if (!bounded(playerPosition.horizontal) || !bounded(playerPosition.vertical) || !bounded(playerPosition.depth)) {
        return std::nullopt;
    }
    const Position deferred{
        static_cast<float>((static_cast<double>(playerPosition.horizontal) + 128.0) + 192.0),
        static_cast<float>(static_cast<double>(playerPosition.vertical) + 16.0), playerPosition.depth};
    return EntryPrefix{4, deferred,
        {CallKind::AllocateHitEffect, 0x43d11b, 0x4670b0, 83, 0, BankSource::BulletManagerAnm8a7f9c}};
}

std::optional<TimerPrefix> timerResetPrefix(Timer stateTimer, SoundControl control) noexcept {
    if (!bounded(stateTimer)) return std::nullopt;
    TimerPrefix result{setTimer(stateTimer, 0), std::nullopt, 0x43d2d1};
    if (control.present && !(control.flags60 & 0x200U)) {
        result.requiredCall = {CallKind::Sound, 0x43d2cc, 0x458410, 2, 0, BankSource::None};
    }
    return result;
}

std::optional<TimerPrefix> counterResetPrefix(Timer hitCounter) noexcept {
    if (!bounded(hitCounter)) return std::nullopt;
    return TimerPrefix{setTimer(hitCounter, 6),
        CallRequest{CallKind::LoadPlayerAnimation, 0x43d32c, 0x4593b0, 0, 0x14, BankSource::PlayerField10},
        0x43d331};
}

std::optional<WindowPrefix> windowPrefix(std::int32_t stateF78, Timer stateTimer,
                                        Timer heldTimer, WindowControl control) noexcept {
    if (stateF78 != 4 || !bounded(stateTimer) || !bounded(heldTimer)) return std::nullopt;
    WindowPrefix result{WindowOutcome::NoCall, stateTimer, heldTimer, std::nullopt, 0x43b9ee};
    if (stateTimer.current >= 8) {
        result.outcome = WindowOutcome::DeathCommitRequired;
        result.requiredCall = {CallKind::CommitDeath, 0x43b84e, 0x43cdd0, 0, 0, BankSource::None};
        result.continuation = 0x43b853;
    } else if (heldTimer.current > 0) {
        result.heldTimer = setTimer(heldTimer, 0);
        result.outcome = WindowOutcome::HeldFreezeSoundRequired;
        result.requiredCall = {CallKind::SoundOperation458590, 0x43b8b2, 0x458590, 31, 0, BankSource::None};
        result.continuation = 0x43b8b7;
    } else if (control.present && control.mode3c == 0 && control.perfectCharge4b4d70 >= 10000
               && (control.keys4db210 & 2U)) {
        result.stateTimer = setTimer(stateTimer, 60);
        result.outcome = WindowOutcome::DeathbombSystemRequired;
        result.requiredCall = {CallKind::BeginDeathbomb, 0x43b902, 0x407810, 0, 0, BankSource::None};
        result.continuation = 0x43b907;
    }
    return result;
}

std::optional<TailState> tailPrefix(TailState state, TailControl control) noexcept {
    if (state.flags7c & 1U) {
        if (state.counter28 >= 60) {
            state.flags7c &= 0xffffffb9U;
            state.value80 = 0;
        } else {
            if (!control.present) return std::nullopt;
            if (control.mode3c != 0) state.flags7c |= 0x40U;
        }
    }
    return state;
}

CallWords serializeCallRequest(const CallRequest& request) noexcept {
    return {static_cast<std::uint32_t>(request.kind), request.callSite, request.callTarget,
            static_cast<std::uint32_t>(request.argument), request.playerOffset,
            static_cast<std::uint32_t>(request.bankSource)};
}

}
