#include "PlayerDeathRules.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

namespace th128::player::death {
namespace {

static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559);

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 65536.0f;
}

bool bounded(Position position) noexcept {
    return bounded(position.horizontal) && bounded(position.vertical) && bounded(position.depth);
}

bool bounded(Timer timer) noexcept {
    return bounded(timer.fractional);
}

Timer setTimer(Timer timer, std::int32_t value) noexcept {
    if (!(timer.flags & 1U)) {
        timer.flags |= 1U;
        timer.clockSource = 0x4b6f58;
    }
    timer.previous = value - 1;
    timer.current = value;
    timer.fractional = static_cast<float>(value);
    return timer;
}

CallRequest call(CallKind kind, std::uint32_t site, std::uint32_t target, Binding binding,
                 std::array<std::uint32_t, 6> operands = {}) noexcept {
    return {kind, site, target, binding, operands};
}

}

std::array<CallRequest, 2> commitCalls() noexcept {
    return {call(CallKind::MotivationDelta, 0x43cddf, 0x427260, Binding::None,
                 {static_cast<std::uint32_t>(-10000)}),
            call(CallKind::PerfectChargeDelta, 0x43cde9, 0x427300, Binding::None, {2500})};
}

std::optional<MotivationSnapshot> motivationAfterDeath(MotivationSnapshot state) noexcept {
    if (state.flag4b4d68 != 0) return state;
    if (state.value4b4d64 < std::numeric_limits<std::int32_t>::min() + 10000) return std::nullopt;
    state.value4b4d64 -= 10000;
    if (state.value4b4d64 > 100000) state.value4b4d64 = 100000;
    if (state.value4b4d64 < 0) state.flag4b4d68 = 1;
    return state;
}

std::array<CallRequest, 2> chargeCompletionCalls() noexcept {
    return {call(CallKind::ChargeNotify424b80, 0x427369, 0x424b80, Binding::Control4b8950, {2, 2, 0}),
            call(CallKind::ChargeUi425820, 0x427375, 0x425820, Binding::Control4b8950)};
}

std::optional<ChargeAwardPrefix> chargeAwardPrefix(std::int32_t perfectCharge) noexcept {
    if (perfectCharge > std::numeric_limits<std::int32_t>::max() - 2500) return std::nullopt;
    const auto updated = std::min(perfectCharge + 2500, 30000);
    if (updated / 10000 > perfectCharge / 10000) {
        const auto suffix = chargeCompletionCalls();
        return ChargeAwardPrefix{updated,
            {call(CallKind::Sound, 0x427355, 0x458410, Binding::None, {72}), suffix[0], suffix[1]}, 3, false};
    }
    return ChargeAwardPrefix{std::max(updated, 0), {}, 0, true};
}

AfterAwardsPrefix afterAwardsPrefix(AwardControl control) noexcept {
    AfterAwardsPrefix result{10000, std::nullopt, 0x43ce17};
    if (control.selector4b4cfc >= 0) {
        result.requiredCall = call(CallKind::ControlRefresh41fc80, 0x43ce12, 0x41fc80,
            Binding::Control4b8950, {static_cast<std::uint32_t>(control.selector4b4cfc),
                                   static_cast<std::uint32_t>(control.value4b4d00)});
    }
    return result;
}

std::optional<StateTwoPrefix> stateTwoSetupPrefix(Timer stateTimer, Timer protectionTimer) noexcept {
    if (!bounded(stateTimer) || !bounded(protectionTimer)) return std::nullopt;
    return StateTwoPrefix{2, setTimer(stateTimer, 0), setTimer(protectionTimer, 180),
        call(CallKind::PlayerAnimation, 0x43cec6, 0x4593b0, Binding::PlayerAnimationField10, {0, 0x14}),
        0x43cecb};
}

std::optional<CleanupPrefix> cleanupRecordPrefix(CleanupRecord record, CleanupEntry entry) noexcept {
    if (entry != CleanupEntry::RecordStart && entry != CleanupEntry::AfterPrimary) return std::nullopt;
    const auto active = entry == CleanupEntry::RecordStart ? 0U : record.active;
    if (entry == CleanupEntry::RecordStart && record.primaryHandle != 0) {
        return CleanupPrefix{active, record.primaryHandle, 0, 0x43cef4, 0x43cf8c};
    }
    if (record.secondaryHandle != 0) return CleanupPrefix{active, record.secondaryHandle, 1, 0x43cf97, 0x43d02c};
    return CleanupPrefix{active, 0, 0, 0, 0x43d02c};
}

std::optional<TailPrefix> tailPrefix(TailSnapshot state, TailControl control) noexcept {
    if (state.scalar4b4d30 < std::numeric_limits<std::int32_t>::min() + 1024) return std::nullopt;
    if (state.flags7c & 1U) {
        if (state.counter28 >= 60) {
            state.flags7c &= 0xffffffb9U;
            state.value80 = 0;
        } else {
            if (!control.present) return std::nullopt;
            if (control.mode3c != 0) state.flags7c |= 0x40U;
        }
    }
    state.scalar4b4d30 = std::clamp(state.scalar4b4d30 - 1024, -1024, 1024);
    return TailPrefix{state, 0};
}

std::optional<RespawnGatePrefix> respawnGatePrefix(std::int32_t stateF78, std::int32_t elapsedF84,
    std::int32_t resource4b4d64, SceneSnapshot scene, Position position) noexcept {
    if (stateF78 != 2 || !bounded(position)) return std::nullopt;
    RespawnGatePrefix result{RespawnOutcome::Waiting, 2, std::nullopt, std::nullopt, 0x43b9ee};
    if (elapsedF84 < 30) return result;
    if (resource4b4d64 < 0) {
        if (!scene.present) return std::nullopt;
        const bool sceneOne = scene.mode10 == 1;
        result.outcome = sceneOne ? RespawnOutcome::SceneOneRequired : RespawnOutcome::SceneDefaultRequired;
        result.requiredCall = sceneOne
            ? call(CallKind::Scene437050, 0x43b925, 0x437050, Binding::None)
            : call(CallKind::Scene438030, 0x43b88d, 0x438030, Binding::None);
        result.continuation = sceneOne ? 0x43b92a : 0x43b892;
    } else {
        result.outcome = RespawnOutcome::RespawnFieldRequired;
        result.stateF78 = 0;
        result.clockWrite = 1.0f;
        result.requiredCall = call(CallKind::RespawnField43df90, 0x43b964, 0x43df90,
            Binding::PlayerPositionEcc, {std::bit_cast<std::uint32_t>(32.0f),
                                       std::bit_cast<std::uint32_t>(16.0f), 30, 150, 0});
        result.continuation = 0x43b969;
    }
    return result;
}

AfterFieldPrefix respawnAfterFieldPrefix(std::int32_t difficulty4b4d04) noexcept {
    AfterFieldPrefix result{std::nullopt, 0x43b97c};
    if (difficulty4b4d04 < 2) {
        result.requiredCall = call(CallKind::PostRespawn427190, 0x43b977, 0x427190, Binding::GlobalState4b4cc0);
    }
    return result;
}

std::optional<RespawnPositionPrefix> respawnPositionPrefix(std::int32_t stateF78, Position position,
    Timer protectionTimer, Timer stateTimer) noexcept {
    if (stateF78 != 0 || !bounded(position) || !bounded(protectionTimer) || !bounded(stateTimer)) return std::nullopt;
    return RespawnPositionPrefix{{0, 480, position.depth}, position, 0, 61440,
                                 setTimer(protectionTimer, 280), setTimer(stateTimer, 0)};
}

CallWords serializeCallRequest(const CallRequest& request) noexcept {
    return {static_cast<std::uint32_t>(request.kind), request.callSite, request.callTarget,
            static_cast<std::uint32_t>(request.binding), request.operands[0], request.operands[1],
            request.operands[2], request.operands[3], request.operands[4], request.operands[5]};
}

}
