#include "game/PlayerHitRules.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

using namespace th128::player::hit;

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

void printTimer(Timer timer) {
    std::cout << ' ' << timer.previous << ' ' << timer.current
              << ' ' << std::bit_cast<std::uint32_t>(timer.fractional)
              << ' ' << timer.clockSource << ' ' << timer.flags;
}

void printRequest(const std::optional<CallRequest>& request) {
    std::cout << ' ' << request.has_value();
    if (request) for (const auto word : serializeCallRequest(*request)) std::cout << ' ' << word;
}

void checkUnsupported() {
    const Timer timer{17, 7, 9, 0x12345678, 0};
    const WindowControl control{true, 0, 10000, 2};
    for (const auto value : std::array{std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
            65537.0f, -65537.0f}) {
        assert(!entryPrefix(1, {value, 0, 0}));
        assert(!entryPrefix(1, {0, value, 0}));
        assert(!entryPrefix(1, {0, 0, value}));
    }
    for (const auto value : std::array{std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
            2147483648.0f, -2147483904.0f}) {
        auto invalid = timer;
        invalid.fractional = value;
        assert(!timerResetPrefix(invalid, {false, 0}));
        assert(!counterResetPrefix(invalid));
        assert(!windowPrefix(4, invalid, timer, control));
        assert(!windowPrefix(4, timer, invalid, control));
        assert(!normalInitPrefix(invalid, timer, timer, 0));
        assert(!normalInitPrefix(timer, invalid, timer, 0));
        assert(!normalInitPrefix(timer, timer, invalid, 0));
        assert(!constructorCounterPrefix(invalid));
        assert(!counterStep(invalid, 1.0f));
    }
    for (const auto state : std::array{-1, 0, 1, 2, 3, 5, 2147483647}) {
        assert(!windowPrefix(state, timer, timer, control));
    }
    assert(!tailPrefix({1, 59, 17}, {false, 0}));
    assert(tailPrefix({1, 60, 17}, {false, 0}));
    assert(tailPrefix({0, 59, 17}, {false, 0}));
    assert(!constructorCounterPrefix(timer));
    assert(!counterStep(timer, 1.0f));
    auto nativeClockTimer = timer;
    nativeClockTimer.clockSource = 0x4b6f58;
    assert(!counterStep(nativeClockTimer, std::nullopt));
    assert(!counterStep(nativeClockTimer, -1.0f));
    assert(!counterStep(nativeClockTimer, 4.01f));
    assert(!counterStep(nativeClockTimer, std::numeric_limits<float>::quiet_NaN()));
    nativeClockTimer.fractional = -2147483904.0f;
    assert(!counterStep(nativeClockTimer, 4.0f));
    auto stopped = timer;
    stopped.current = 0;
    assert(counterStep(stopped, std::nullopt));
}

int main() {
    checkUnsupported();
    std::string kind;
    while (std::cin >> kind) {
        std::cout << kind;
        if (kind == "gate") {
            PlayerGate player{};
            HitControl control{};
            std::cin >> player.stateF78 >> player.flags438ec >> player.counter438dc
                     >> control.present >> control.counter5a1c;
            const auto result = gateDecision(player, control);
            std::cout << ' ' << static_cast<int>(result.outcome) << ' ' << result.helperReturn.value_or(-1)
                      << ' ' << result.callSite << ' ' << result.callTarget;
        } else if (kind == "entry") {
            std::int32_t state = 0;
            std::cin >> state;
            const auto horizontal = readFloat();
            const auto vertical = readFloat();
            const auto depth = readFloat();
            const auto result = entryPrefix(state, {horizontal, vertical, depth});
            assert(result);
            std::cout << ' ' << result->stateF78
                      << ' ' << std::bit_cast<std::uint32_t>(result->deferredEffectPosition.horizontal)
                      << ' ' << std::bit_cast<std::uint32_t>(result->deferredEffectPosition.vertical)
                      << ' ' << std::bit_cast<std::uint32_t>(result->deferredEffectPosition.depth);
            printRequest(result->requiredCall);
        } else if (kind == "timer-reset" || kind == "counter-reset") {
            const auto timer = readTimer();
            std::optional<TimerPrefix> result;
            if (kind == "timer-reset") {
                SoundControl control{};
                std::cin >> control.present >> control.flags60;
                result = timerResetPrefix(timer, control);
            } else {
                result = counterResetPrefix(timer);
            }
            assert(result);
            printTimer(result->updated);
            printRequest(result->requiredCall);
            std::cout << ' ' << result->continuation;
        } else if (kind == "window") {
            std::int32_t state = 0;
            std::cin >> state;
            const auto stateTimer = readTimer();
            const auto heldTimer = readTimer();
            WindowControl control{};
            std::cin >> control.present >> control.mode3c >> control.perfectCharge4b4d70 >> control.keys4db210;
            const auto result = windowPrefix(state, stateTimer, heldTimer, control);
            assert(result);
            std::cout << ' ' << static_cast<int>(result->outcome);
            printTimer(result->stateTimer);
            printTimer(result->heldTimer);
            printRequest(result->requiredCall);
            std::cout << ' ' << result->continuation;
        } else if (kind == "normal-init") {
            const auto stateTimer = readTimer();
            const auto focusTimer = readTimer();
            const auto rechargeTimer = readTimer();
            std::uint32_t flags = 0;
            std::cin >> flags;
            const auto result = normalInitPrefix(stateTimer, focusTimer, rechargeTimer, flags);
            assert(result);
            std::cout << ' ' << result->stateF78 << ' ' << result->flags438ec;
            printTimer(result->stateTimer);
            printTimer(result->focusTimer);
            printTimer(result->rechargeTimer);
            std::cout << ' ' << result->fieldF70 << ' ' << result->fieldF74;
            printRequest(result->requiredCall);
        } else if (kind == "constructor-counter" || kind == "counter-step") {
            const auto timer = readTimer();
            const auto result = kind == "constructor-counter" ? constructorCounterPrefix(timer)
                : counterStep(timer, readFloat());
            assert(result);
            printTimer(*result);
        } else if (kind == "tail") {
            TailState state{};
            TailControl control{};
            std::cin >> state.flags7c >> state.counter28 >> state.value80 >> control.present >> control.mode3c;
            const auto result = tailPrefix(state, control);
            assert(result);
            std::cout << ' ' << result->flags7c << ' ' << result->counter28 << ' ' << result->value80;
        } else {
            return 2;
        }
        if (!std::cin) return 3;
        std::cout << '\n';
    }
}
