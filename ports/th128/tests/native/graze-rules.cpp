#include "game/GrazeRules.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <variant>

using namespace th128::graze;

float readFloat() {
    std::uint32_t raw = 0;
    std::cin >> raw;
    return std::bit_cast<float>(raw);
}

Position readPosition() {
    const auto horizontal = readFloat();
    const auto vertical = readFloat();
    const auto depth = readFloat();
    return {horizontal, vertical, depth};
}

void printRequest(const CallRequest& request) {
    for (const auto word : serializeCallRequest(request)) std::cout << ' ' << word;
}

void checkDomainsAndSequence() {
    for (const auto value : std::array{std::numeric_limits<float>::quiet_NaN(),
             std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
             65537.0f, -65537.0f}) {
        assert(!planGraze({0, 0}, {value, 0, 0}));
        assert(!planGraze({0, 0}, {0, value, 0}));
        assert(!planGraze({0, 0}, {0, 0, value}));
    }
    assert(!planGraze({0, 2147483548}, {0, 0, 0}));
    assert(!planGraze({0, std::numeric_limits<std::int32_t>::max()}, {0, 0, 0}));
    const auto upper = planGraze({2147483647, 2147483547}, {0, 0, 0});
    assert(upper && upper->updated.grazeCount == 2147483647 && upper->updated.powerRaw == 10000);
    assert(!grazeCallerDecision(static_cast<UpdateSite>(2), 2, 4));
    for (const auto helper : std::array{-2147483647, -1, 1, 3, 2147483647}) {
        assert(!grazeCallerDecision(UpdateSite::HalfStep, helper, 0x17));
    }
    const auto already = grazeCallerDecision(UpdateSite::HalfStep, 2, 0x17);
    assert(already && already->action == CallerAction::NoGrazeCall
           && already->flagsSetAfterSuccess == 0 && already->continuation == 0x40888b);
    const auto crossing = planGraze({42, 2999}, {-133.25f, 7.5f, .1f});
    assert(crossing && crossing->requests().size() == 3);
    assert(std::holds_alternative<ThresholdSoundRequest>(crossing->requests()[0].payload));
    assert(std::holds_alternative<EffectRequest>(crossing->requests()[1].payload));
    assert(std::holds_alternative<PositionalSoundRequest>(crossing->requests()[2].payload));
    assert(std::get<ThresholdSoundRequest>(crossing->requests()[0].payload).soundId == 73);
    const auto& effect = std::get<EffectRequest>(crossing->requests()[1].payload);
    assert(effect.scriptId == 175 && effect.layer == 23
           && effect.bankSource == EffectBankSource::BulletManagerAnm8a7f9c);
    const auto maximum = planGraze({42, 9999}, {0, 0, 0});
    assert(maximum && std::get<ThresholdSoundRequest>(maximum->requests()[0].payload).soundId == 74);
}

int main() {
    checkDomainsAndSequence();
    std::string kind;
    while (std::cin >> kind) {
        if (kind == "plan") {
            Globals state{};
            std::cin >> state.grazeCount >> state.powerRaw;
            const auto coordinates = readPosition();
            if (!std::cin) return 2;
            const auto plan = planGraze(state, coordinates);
            assert(plan);
            assert(plan->requests().size() >= 2 && plan->requests().size() <= 3);
            std::cout << kind << ' ' << plan->updated.grazeCount << ' ' << plan->updated.powerRaw
                      << ' ' << plan->requests().size();
            for (const auto& request : plan->requests()) printRequest(request);
        } else if (kind == "caller" || kind == "post") {
            unsigned site = 0;
            std::int32_t helper = 0;
            std::uint32_t flags = 0;
            std::cin >> site >> helper >> flags;
            if (!std::cin) return 3;
            const auto result = grazeCallerDecision(static_cast<UpdateSite>(site),
                kind == "post" ? 2 : helper, kind == "post" ? flags & ~4U : flags);
            assert(result);
            if (kind == "post") {
                assert(result->action == CallerAction::GrazeCallRequired);
                std::cout << kind << ' ' << (flags | result->flagsSetAfterSuccess);
            } else {
                std::cout << kind << ' ' << static_cast<int>(result->action) << ' ' << result->callSite
                          << ' ' << result->callTarget << ' ' << result->flagsSetAfterSuccess
                          << ' ' << result->continuation;
            }
        } else if (kind == "serialize-effect") {
            const auto coordinates = readPosition();
            printRequest({0x43e119, 0x407ec0, EffectRequest{175, 23,
                EffectBankSource::BulletManagerAnm8a7f9c, coordinates}});
        } else if (kind == "serialize-sound") {
            const auto horizontal = readFloat();
            printRequest({0x43e129, 0x4584a0, PositionalSoundRequest{44, horizontal}});
        } else {
            return 4;
        }
        std::cout << '\n';
    }
}
