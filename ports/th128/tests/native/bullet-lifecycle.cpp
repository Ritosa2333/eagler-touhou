#include "game/BulletRules.hpp"

#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

float readFloat() {
    std::uint32_t raw = 0;
    std::cin >> raw;
    return std::bit_cast<float>(raw);
}

th128::bullet::Position readPosition() {
    const auto horizontal = readFloat();
    const auto vertical = readFloat();
    const auto depth = readFloat();
    return {horizontal, vertical, depth};
}

void printFloat(float value) {
    std::cout << ' ' << std::bit_cast<std::uint32_t>(value);
}

void printPosition(const th128::bullet::Position& position) {
    printFloat(position.x);
    printFloat(position.y);
    printFloat(position.z);
}

int main() {
    using namespace th128::bullet;
    const auto invalid = std::numeric_limits<float>::quiet_NaN();
    assert(!targetAnglePrefix({invalid, 0, 0}, {0, 0, 0}));
    assert(!motionPrefix({0, 0, 0}, {1, 1, 1}, 6, 1));
    assert(!motionPrefix({0, 0, 0}, {1, 1, 1}, 2, invalid));
    auto emitter = resetEmitter();
    const SpawnPrefixProfile profile{8, 2.4f, 16, 4};
    assert(!spawnTailPrefix(emitter, profile, {}, {}, 0x13));
    emitter.type = 8;
    emitter.color = 16;
    assert(!spawnTailPrefix(emitter, profile, {}, {}, 0x13));
    emitter.color = 10;
    assert(!spawnTailPrefix(emitter, profile, {invalid, 0, 0}, {}, 0x13));
    const auto tailFlag = spawnTailPrefix(emitter, profile, {}, {}, 3);
    assert(tailFlag && tailFlag->flags == 0x13);
    for (const auto state : {std::uint16_t(1), std::uint16_t(2)}) {
        for (const auto sign : {-1.0f, 1.0f}) {
            assert(!motionPrefix({}, {sign * 1e20f, 0, 0}, state, 1e20f));
        }
    }
    std::string operation;
    while (std::cin >> operation) {
        if (operation == "aim") {
            const auto position = readPosition();
            const auto player = readPosition();
            const auto result = targetAnglePrefix(position, player);
            if (!result) return 2;
            std::cout << "aim";
            printFloat(*result);
        } else if (operation == "spawn") {
            std::cin >> emitter.flags >> emitter.transformSound >> emitter.extension0.parameterA
                     >> emitter.extension0.type;
            emitter.extension0.parameterR = readFloat();
            emitter.extension0.parameterS = readFloat();
            std::uint32_t postAnimationFlags = 0;
            std::cin >> emitter.extension0.parameterB >> emitter.extension0.control >> postAnimationFlags;
            const auto position = readPosition();
            const auto velocity = readPosition();
            const auto result = spawnTailPrefix(emitter, profile, position, velocity, postAnimationFlags);
            if (!result) return 3;
            std::cout << "spawn " << result->motionState << ' ' << result->animationInterrupt
                      << ' ' << result->extensionIndex << ' ' << result->flags << ' ' << result->activeExtensions
                      << ' ' << result->emitterFlags << ' ' << result->transformSound << ' ' << result->profileValue;
            printFloat(result->hitboxSize);
            printFloat(result->hitboxSize);
            printFloat(result->auxiliarySize);
            printPosition(result->position);
            printPosition(result->velocity);
            printFloat(result->extension0.parameterR);
            printFloat(result->extension0.parameterS);
            std::cout << ' ' << result->extension0.parameterA << ' ' << result->extension0.parameterB
                      << ' ' << result->extension0.type << ' ' << result->extension0.control;
        } else if (operation == "motion") {
            std::uint16_t state = 0;
            std::cin >> state;
            const auto scale = readFloat();
            const auto position = readPosition();
            const auto velocity = readPosition();
            const auto result = motionPrefix(position, velocity, state, scale);
            if (!result) return 4;
            std::cout << "motion";
            printPosition(*result);
        } else {
            return 5;
        }
        if (!std::cin) return 6;
        std::cout << '\n';
    }
    return 0;
}
