#include "game/PlayerDeathRules.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

using namespace th128::player::death;

float readFloat() {
    std::uint32_t raw = 0;
    std::cin >> raw;
    return std::bit_cast<float>(raw);
}

Timer readTimer() {
    Timer timer{};
    std::cin >> timer.previous >> timer.current;
    timer.fractional = readFloat();
    std::cin >> timer.clockSource >> timer.flags;
    return timer;
}

Position readPosition() {
    const auto horizontal = readFloat();
    const auto vertical = readFloat();
    const auto depth = readFloat();
    return {horizontal, vertical, depth};
}

void printTimer(Timer timer) {
    std::cout << ' ' << timer.previous << ' ' << timer.current
              << ' ' << std::bit_cast<std::uint32_t>(timer.fractional)
              << ' ' << timer.clockSource << ' ' << timer.flags;
}

void printPosition(Position coordinates) {
    std::cout << ' ' << std::bit_cast<std::uint32_t>(coordinates.horizontal)
              << ' ' << std::bit_cast<std::uint32_t>(coordinates.vertical)
              << ' ' << std::bit_cast<std::uint32_t>(coordinates.depth);
}

void printCall(const CallRequest& request) {
    for (const auto word : serializeCallRequest(request)) std::cout << ' ' << word;
}

void printOptionalCall(const std::optional<CallRequest>& request) {
    std::cout << ' ' << request.has_value();
    if (request) printCall(*request);
}

void checkUnsupported() {
    const Timer timer{17, 9, 9, 0x12345678, 0};
    assert(!motivationAfterDeath({-2147483648, 0}));
    assert(motivationAfterDeath({-2147483648, 1}));
    assert(!chargeAwardPrefix(2147481148));
    assert(!chargeAwardPrefix(2147483647));
    assert(!tailPrefix({0, 0, 0, -2147482625}, {false, 0}));
    assert(!tailPrefix({1, 59, 0, 0}, {false, 0}));
    assert(!respawnGatePrefix(1, 30, 0, {true, 0}, {0, 0, 0}));
    assert(!respawnGatePrefix(2, 30, -1, {false, 0}, {0, 0, 0}));
    assert(!cleanupRecordPrefix({0, 0, 0}, static_cast<CleanupEntry>(2)));
    assert(!respawnPositionPrefix(2, {0, 0, 0}, timer, timer));
    for (const auto value : std::array{std::numeric_limits<float>::quiet_NaN(),
             std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
             65537.0f, -65537.0f}) {
        auto invalid = timer;
        invalid.fractional = value;
        assert(!stateTwoSetupPrefix(invalid, timer));
        assert(!stateTwoSetupPrefix(timer, invalid));
        assert(!respawnPositionPrefix(0, {0, 0, 0}, invalid, timer));
        assert(!respawnPositionPrefix(0, {0, 0, 0}, timer, invalid));
        assert(!respawnPositionPrefix(0, {value, 0, 0}, timer, timer));
        assert(!respawnPositionPrefix(0, {0, value, 0}, timer, timer));
        assert(!respawnPositionPrefix(0, {0, 0, value}, timer, timer));
        assert(!respawnGatePrefix(2, 30, 0, {true, 0}, {value, 0, 0}));
    }
}

int main() {
    checkUnsupported();
    std::string kind;
    while (std::cin >> kind) {
        std::cout << kind;
        if (kind == "commit-call" || kind == "charge-suffix") {
            unsigned stage = 0;
            std::cin >> stage;
            assert(stage < 2);
            const auto calls = kind == "commit-call" ? commitCalls() : chargeCompletionCalls();
            printCall(calls[stage]);
        } else if (kind == "motivation") {
            MotivationSnapshot state{};
            std::cin >> state.value4b4d64 >> state.flag4b4d68;
            const auto result = motivationAfterDeath(state);
            assert(result);
            std::cout << ' ' << result->value4b4d64 << ' ' << result->flag4b4d68;
        } else if (kind == "charge") {
            std::int32_t value = 0;
            std::cin >> value;
            const auto result = chargeAwardPrefix(value);
            assert(result);
            std::cout << ' ' << result->updatedCharge << ' ' << result->requestCount;
            for (std::uint32_t index = 0; index < result->requestCount; ++index) printCall(result->requiredCalls[index]);
            std::cout << ' ' << result->completedIndependentHelper;
        } else if (kind == "after-awards") {
            AwardControl control{};
            std::cin >> control.selector4b4cfc >> control.value4b4d00;
            const auto result = afterAwardsPrefix(control);
            std::cout << ' ' << result.rawPower;
            printOptionalCall(result.requiredCall);
            std::cout << ' ' << result.continuation;
        } else if (kind == "state-two") {
            const auto stateTimer = readTimer();
            const auto protectionTimer = readTimer();
            const auto result = stateTwoSetupPrefix(stateTimer, protectionTimer);
            assert(result);
            std::cout << ' ' << result->stateF78;
            printTimer(result->stateTimer);
            printTimer(result->protectionTimer);
            printCall(result->requiredCall);
        } else if (kind == "cleanup-record") {
            CleanupRecord record{};
            unsigned entry = 0;
            std::cin >> record.active >> record.primaryHandle >> record.secondaryHandle >> entry;
            const auto result = cleanupRecordPrefix(record, static_cast<CleanupEntry>(entry));
            assert(result);
            std::cout << ' ' << result->activeAfter << ' ' << result->requiredHandle << ' ' << result->phase
                      << ' ' << result->lookupInstruction << ' ' << result->continuation;
        } else if (kind == "tail") {
            TailSnapshot state{};
            TailControl control{};
            std::cin >> state.flags7c >> state.counter28 >> state.value80 >> control.present >> control.mode3c
                     >> state.scalar4b4d30;
            const auto result = tailPrefix(state, control);
            assert(result);
            std::cout << ' ' << result->updated.flags7c << ' ' << result->updated.value80
                      << ' ' << result->updated.scalar4b4d30 << ' ' << result->field438f8;
        } else if (kind == "respawn-gate") {
            std::int32_t state = 0, elapsed = 0, resource = 0;
            SceneSnapshot scene{};
            std::cin >> state >> elapsed >> resource >> scene.present >> scene.mode10;
            const auto coordinates = readPosition();
            const auto result = respawnGatePrefix(state, elapsed, resource, scene, coordinates);
            assert(result);
            std::cout << ' ' << static_cast<int>(result->outcome) << ' ' << result->stateF78
                      << ' ' << result->clockWrite.has_value();
            if (result->clockWrite) std::cout << ' ' << std::bit_cast<std::uint32_t>(*result->clockWrite);
            printOptionalCall(result->requiredCall);
            std::cout << ' ' << result->continuation;
        } else if (kind == "after-field") {
            std::int32_t difficulty = 0;
            std::cin >> difficulty;
            const auto result = respawnAfterFieldPrefix(difficulty);
            printOptionalCall(result.requiredCall);
            std::cout << ' ' << result.continuation;
        } else if (kind == "respawn-position") {
            const auto coordinates = readPosition();
            const auto protectionTimer = readTimer();
            const auto stateTimer = readTimer();
            const auto result = respawnPositionPrefix(0, coordinates, protectionTimer, stateTimer);
            assert(result);
            printPosition(result->position);
            printPosition(result->savedPosition);
            std::cout << ' ' << result->rawHorizontal << ' ' << result->rawVertical;
            printTimer(result->protectionTimer);
            printTimer(result->stateTimer);
        } else {
            return 2;
        }
        if (!std::cin) return 3;
        std::cout << '\n';
    }
}
