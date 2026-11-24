#include "game/BulletRules.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

int main() {
    using namespace th128::bullet;
    const std::array<std::int32_t, 4> limits{3, 3, 5, 7};
    for (std::int32_t difficulty = 0; difficulty <= 4; ++difficulty) {
        assert(chooseDifficulty(difficulty, limits) == limits[difficulty < 3 ? difficulty : 3]);
    }
    assert(!chooseDifficulty(-1, limits));
    assert(!chooseDifficulty(5, limits));
    auto emitter = resetEmitter();
    assert(emitter.type == 0 && emitter.color == 0 && emitter.aimMode == 0);
    assert(emitter.angle == 0 && emitter.spacing == 0);
    assert(emitter.speed == 2 && emitter.endSpeed == 0);
    assert(emitter.count == 1 && emitter.layers == 1);
    assert(emitter.flags == 35 && emitter.shotSound == 22 && emitter.transformSound == 40);
    setTypeColor(emitter, 8, 10);
    setAimMode(emitter, 65536);
    setAngles(emitter, 0, 0.03926991f);
    const std::array<float, 4> speeds{1, 1, 2, 2};
    const std::array<float, 4> endSpeeds{1, 1, 1, 1};
    const std::array<std::int32_t, 4> counts{1, 1, 1, 1};
    for (std::int32_t difficulty = -1; difficulty <= 5; ++difficulty) {
        setDifficultySpeeds(emitter, difficulty, speeds, endSpeeds);
        setDifficultyCounts(emitter, difficulty, counts, counts);
        assert(emitter.speed == (difficulty == 0 || difficulty == 1 ? 1 : 2));
        assert(emitter.endSpeed == 1 && emitter.count == 1 && emitter.layers == 1);
    }
    const std::array<std::int32_t, 4> wrapped{1, 32767, 65535, 32768};
    const std::array<std::int32_t, 4> layers{4, 5, 65536, -1};
    setDifficultyCounts(emitter, -1, wrapped, layers);
    assert(emitter.count == -32768 && emitter.layers == -1);
    setAimMode(emitter, -1);
    assert(emitter.aimMode == 65535);
    emitter = resetEmitter();
    assert(emitter.extension0.type == 0 && emitter.extension0.control == 0);
    ExtensionRaw extension;
    extension.parameterR = .625f;
    extension.parameterS = -8.25f;
    extension.parameterA = -123456;
    extension.parameterB = 987654;
    extension.type = 37;
    extension.control = 42;
    assert(setFirstExtensionRaw(emitter, 0, extension));
    assert(emitter.extension0.parameterR == .625f && emitter.extension0.parameterS == -8.25f);
    assert(emitter.extension0.parameterA == -123456 && emitter.extension0.parameterB == 987654);
    assert(emitter.extension0.type == 37 && emitter.extension0.control == 42);
    assert(!setFirstExtensionRaw(emitter, -1, ExtensionRaw{}));
    assert(!setFirstExtensionRaw(emitter, 1, ExtensionRaw{}));
    assert(emitter.extension0.type == 37);
    extension.parameterR = std::numeric_limits<float>::quiet_NaN();
    assert(!setFirstExtensionRaw(emitter, 0, extension));
    assert(emitter.extension0.parameterR == .625f);
    assert(prefixCount(emitter) == 1);
    emitter.count = 32767;
    emitter.layers = 32767;
    assert(!prefixCount(emitter));
    assert(prefixCount(emitter, 1073676289U) == 1073676289U);
    emitter.count = -1;
    assert(prefixCount(emitter) == 0);
    emitter = resetEmitter();
    emitter.count = 3;
    emitter.spacing = .25f;
    emitter.speed = 4;
    emitter.endSpeed = 1;
    emitter.layers = 3;
    const auto middle = emissionPrefix(emitter, 1, 1, 1);
    assert(middle && middle->rawAngle == .75f && middle->angle == .75f && middle->speed == 2.5f);
    assert(!emissionPrefix(emitter, -1, 0, 1));
    assert(!emissionPrefix(emitter, 3, 0, 1));
    assert(!emissionPrefix(emitter, 0, 3, 1));
    assert(!emissionPrefix(emitter, 0, 0, std::numeric_limits<float>::infinity()));
    emitter.aimMode = 4;
    assert(!emissionPrefix(emitter, 0, 0, 1));
    emitter = resetEmitter();
    emitter.angle = 4096;
    assert(!emissionPrefix(emitter, 0, 0, 1));
    std::cout << "bullet scalar vectors passed\n";
    std::string operation;
    while (std::cin >> operation) {
        if (operation == "choose") {
            std::int32_t difficulty = 0;
            std::array<std::int32_t, 4> choices{};
            if (!(std::cin >> difficulty >> choices[0] >> choices[1] >> choices[2] >> choices[3])) return 5;
            const auto chosen = chooseDifficulty(difficulty, choices);
            std::cout << "choose " << (chosen ? 1 : 0) << ' ' << chosen.value_or(0) << '\n';
            continue;
        }
        if (operation == "ranked") {
            std::int32_t difficulty = 0;
            std::array<float, 8> speedArguments{};
            std::array<std::int32_t, 8> countArguments{};
            if (!(std::cin >> difficulty)) return 6;
            for (auto& argument : speedArguments) {
                std::uint32_t bits = 0;
                if (!(std::cin >> bits)) return 7;
                argument = std::bit_cast<float>(bits);
            }
            for (auto& argument : countArguments) if (!(std::cin >> argument)) return 8;
            emitter = resetEmitter();
            setDifficultySpeeds(emitter, difficulty,
                {speedArguments[0], speedArguments[1], speedArguments[2], speedArguments[3]},
                {speedArguments[4], speedArguments[5], speedArguments[6], speedArguments[7]});
            setDifficultyCounts(emitter, difficulty,
                {countArguments[0], countArguments[1], countArguments[2], countArguments[3]},
                {countArguments[4], countArguments[5], countArguments[6], countArguments[7]});
            std::cout << "ranked " << std::bit_cast<std::uint32_t>(emitter.speed) << ' '
                      << std::bit_cast<std::uint32_t>(emitter.endSpeed) << ' '
                      << emitter.count << ' ' << emitter.layers << '\n';
            continue;
        }
        if (operation == "extension") {
            std::uint32_t parameterRBits = 0, parameterSBits = 0;
            if (!(std::cin >> extension.control >> extension.type >> extension.parameterA >> extension.parameterB
                  >> parameterRBits >> parameterSBits)) return 9;
            extension.parameterR = std::bit_cast<float>(parameterRBits);
            extension.parameterS = std::bit_cast<float>(parameterSBits);
            emitter = resetEmitter();
            if (!setFirstExtensionRaw(emitter, 0, extension)) return 10;
            std::cout << "extension " << emitter.extension0.control << ' ' << emitter.extension0.type << ' '
                      << emitter.extension0.parameterA << ' ' << emitter.extension0.parameterB << ' '
                      << std::bit_cast<std::uint32_t>(emitter.extension0.parameterR) << ' '
                      << std::bit_cast<std::uint32_t>(emitter.extension0.parameterS) << '\n';
            continue;
        }
        if (operation != "geometry") return 2;
        std::int32_t count = 0, layersCount = 0, mode = 0, bulletIndex = 0, layerIndex = 0;
        std::array<std::uint32_t, 5> inputs{};
        if (!(std::cin >> count >> layersCount >> mode >> bulletIndex >> layerIndex
              >> inputs[0] >> inputs[1] >> inputs[2] >> inputs[3] >> inputs[4])) return 3;
        emitter = resetEmitter();
        emitter.count = static_cast<std::int16_t>(count);
        emitter.layers = static_cast<std::int16_t>(layersCount);
        emitter.aimMode = static_cast<std::uint16_t>(mode);
        emitter.angle = std::bit_cast<float>(inputs[0]);
        emitter.spacing = std::bit_cast<float>(inputs[1]);
        emitter.speed = std::bit_cast<float>(inputs[2]);
        emitter.endSpeed = std::bit_cast<float>(inputs[3]);
        const auto sample = emissionPrefix(emitter, bulletIndex, layerIndex, std::bit_cast<float>(inputs[4]));
        if (!sample) return 4;
        std::cout << "geometry " << std::bit_cast<std::uint32_t>(sample->rawAngle) << ' '
                  << std::bit_cast<std::uint32_t>(sample->angle) << ' '
                  << std::bit_cast<std::uint32_t>(sample->speed) << ' '
                  << std::bit_cast<std::uint32_t>(sample->velocityX) << ' '
                  << std::bit_cast<std::uint32_t>(sample->velocityY) << '\n';
    }
}
