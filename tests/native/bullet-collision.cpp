#include "game/BulletCollisionRules.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

using namespace th128::bullet::collision;

float readFloat() {
    std::uint32_t raw = 0;
    std::cin >> raw;
    return std::bit_cast<float>(raw);
}

Vec2 readVec2() {
    const auto horizontal = readFloat();
    const auto vertical = readFloat();
    return {horizontal, vertical};
}

void printFloat(float value) {
    std::cout << ' ' << std::bit_cast<std::uint32_t>(value);
}

void printBounds(RectBounds bounds) {
    printFloat(bounds.minX);
    printFloat(bounds.minY);
    printFloat(bounds.maxX);
    printFloat(bounds.maxY);
}

void printDecision(const Decision& decision) {
    std::cout << ' ' << static_cast<int>(decision.outcome) << ' ' << decision.helperReturn.value_or(-1)
              << ' ' << decision.callSite << ' ' << decision.callTarget;
}

void checkUnsupportedInputs() {
    const PlayerSnapshot player{{0, 0}, {-2, -2, 2, 2}, 2, 0, 0, 1};
    const ControlSnapshot control{false, 0};
    for (const auto value : std::array{std::numeric_limits<float>::quiet_NaN(),
             std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
             65537.0f, -65537.0f, 0x1p-17f, -0x1p-17f, std::numeric_limits<float>::denorm_min()}) {
        if(!std::isfinite(value)||std::abs(value)>65536)assert(!circleGeometry({value, 0}, 2.4f, {0, 0}, 2));
        assert(!circleGeometry({0, 0}, value, {0, 0}, 2));
        if(!std::isfinite(value)||std::abs(value)>65536)assert(!circleGeometry({0, 0}, 2.4f, {0, value}, 2));
        assert(!circleGeometry({0, 0}, 2.4f, {0, 0}, value));
        assert(!rectangleDecision({value, 0}, {2, 2}, player, control));
        if(!std::isfinite(value)||std::abs(value)>65536)assert(!circleDecision({0, value}, 2.4f, player, control));
        assert(!circleDecision({0, 0}, value, player, control));
        auto invalid = player;
        invalid.position.horizontal = value;
        if(!std::isfinite(value)||std::abs(value)>65536)assert(!circleDecision({0, 0}, 2.4f, invalid, control));
        invalid = player;
        invalid.bounds.maxX = value;
        assert(!rectangleDecision({0, 0}, {2, 2}, invalid, control));
        invalid = player;
        invalid.profileHitRadius = value;
        assert(!circleDecision({0, 0}, 2.4f, invalid, control));
        assert(!updateCollisionPrefix(UpdateSite::FullStep, {{0, 0}, {2.4f, 4}, 0x52, 8, value},
                                      player, control));
    }
    assert(!rectangleDecision({0, 0}, {-2, 2}, player, control));
    assert(!circleDecision({0, 0}, -2.4f, player, control));
    assert(!circleGeometry({0, 0}, -2.4f, {0, 0}, 2));
    assert(!circleGeometry({0, 0}, 2.4f, {0, 0}, -2));
    assert(circleGeometry({100, 100}, 2.4f, {0, 0}, 2) == Geometry::Miss);
    assert(circleGeometry({0, 0}, 2.4f, {0, 0}, 2) == Geometry::PotentialHit);
    assert(circleGeometry({10, 0}, 2.4f, {0, 0}, 2) == Geometry::PotentialGraze);
    auto reversed = player;
    reversed.bounds = {2, -2, -2, 2};
    assert(!rectangleDecision({0, 0}, {2, 2}, reversed, control));
    assert(!updateCollisionPrefix(static_cast<UpdateSite>(9), {{0, 0}, {2, 2}, 2, 8, 1}, player, control));
    assert(!updateCollisionPrefix(UpdateSite::HalfStep, {{0, 0}, {65536, 2}, 0x42, 8, 2}, player, control));
    assert(!updateCollisionPrefix(UpdateSite::HalfStep, {{0, 0}, {0x1p-16f, 2}, 0x42, 8, .5f}, player, control));
    const auto absent = updateCollisionPrefix(UpdateSite::HalfStep, {{0, 0}, {2.4f, 4}, 0x13, 7, 1},
                                               player, control);
    assert(absent && absent->intent == UpdateIntent::NoHelper && !absent->helper && !absent->effectiveSize);
}

int main() {
    checkUnsupportedInputs();
    std::string kind;
    while (std::cin >> kind) {
        if (kind == "circle-geometry") {
            const auto bulletPosition = readVec2();
            const auto hitboxSize = readFloat();
            const auto playerPosition = readVec2();
            const auto profileRadius = readFloat();
            if (!std::cin) return 5;
            const auto result = circleGeometry(bulletPosition, hitboxSize, playerPosition, profileRadius);
            assert(result);
            std::cout << kind << ' ' << static_cast<int>(*result) << '\n';
            continue;
        }
        const auto center = readVec2();
        const auto size = readVec2();
        PlayerSnapshot player{};
        player.position = readVec2();
        player.bounds.minX = readFloat();
        player.bounds.minY = readFloat();
        player.bounds.maxX = readFloat();
        player.bounds.maxY = readFloat();
        player.profileHitRadius = readFloat();
        ControlSnapshot control{};
        std::cin >> player.stateF78 >> player.flags438ec >> player.counter438dc
                 >> control.present >> control.counter5a1c;
        if (!std::cin) return 2;
        if (kind == "rectangle") {
            const auto result = rectangleDecision(center, size, player, control);
            assert(result);
            std::cout << kind;
            printDecision(result->decision);
            printBounds(result->hitBounds);
            std::cout << ' ' << result->grazeBounds.has_value();
            if (result->grazeBounds) printBounds(*result->grazeBounds);
        } else if (kind == "circle") {
            const auto result = circleDecision(center, size.horizontal, player, control);
            assert(result);
            std::cout << kind;
            printDecision(result->decision);
            printFloat(result->distanceSquared);
            std::cout << ' ' << result->grazeSize.has_value();
            if (result->grazeSize) {
                assert(result->dividedGrazeSize);
                printFloat(*result->dividedGrazeSize);
                printFloat(*result->grazeSize);
            }
        } else if (kind == "update-prefix") {
            unsigned site = 0;
            BulletSnapshot bullet{center, size, 0, 0, 0};
            std::cin >> site >> bullet.flags >> bullet.age1188;
            bullet.scale1180 = readFloat();
            if (!std::cin) return 3;
            const auto result = updateCollisionPrefix(static_cast<UpdateSite>(site), bullet, player, control);
            assert(result);
            std::cout << kind << ' ' << static_cast<int>(result->route) << ' ' << static_cast<int>(result->intent)
                      << ' ' << result->helper.has_value();
            if (result->helper) printDecision(*result->helper);
            std::cout << ' ' << result->callSite << ' ' << result->callTarget
                      << ' ' << result->effectiveSize.has_value();
            if (result->effectiveSize) {
                printFloat(result->effectiveSize->horizontal);
                printFloat(result->effectiveSize->vertical);
            }
            std::cout << ' ' << (result->pendingMotionState ? static_cast<int>(*result->pendingMotionState) : -1)
                      << ' ' << result->pendingFlagsSetAfterCall;
        } else {
            return 4;
        }
        std::cout << '\n';
    }
}
