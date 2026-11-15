#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cstdio>
#include <exception>
#include <string>

using namespace ecl_fixture;

namespace {
Bytes waitLoop() {
    return join({instruction(83, {100}, 0, 0, 255, 1),
                 instruction(12, {0xffffffecU, 0}, 0, 0, 255, 2)});
}

Bytes actualCopyProgram() {
    return join({
        instruction(500, {3}, 0, 0, 255, 1),
        instruction(502, {3, 8, 10}, 0, 0, 255, 3),
        instruction(504, {3, floatBits(0.25f), floatBits(0.5f)}, 0, 0, 255, 3),
        instruction(507, {3, 1}, 0, 0, 255, 2),
        instruction(503, {3, floatBits(1.25f), floatBits(-2.5f)}, 0, 0, 255, 3),
        instruction(525, {3, floatBits(3.5f), floatBits(-4.5f)}, 0, 0, 255, 3),
        instruction(524, {3, floatBits(1.5f)}, 0, 0, 255, 2),
        instruction(509, {3, 0, 42, 37, static_cast<std::uint32_t>(-123456), 987654,
                          floatBits(.625f), floatBits(-8.25f)}, 0, 0, 255, 8),
        instruction(537, {3, 1, 43, 38, 77, 88, 99, 111, floatBits(.75f), floatBits(-1.25f),
                          floatBits(2.25f), floatBits(-3.5f)}, 0, 0, 255, 12),
        instruction(511, {3, 3}, 0, 0, 255, 2),
        instruction(511, {4, 3}, 0, 0, 255, 2),
        instruction(511, {0, 4}, 0, 0, 255, 2),
        waitLoop(),
    });
}
}

int main() {
    try {
        std::string error;
        {
            th128::StageWorld copied;
            const auto data = program({{"main", actualCopyProgram()}});
            require(copied.load("emitter511.ecl", data.data(), data.size(), error), error);
            require(copied.start("emitter511.ecl", 1, 0), error);
            require(copied.tick(), copied.error());
            const auto& emitter = copied.entities()[0].emitter0;
            require(copied.frame() == 1, "511 must not add a tick");
            require(copied.bullets().activeCount() == 0 && copied.entities()[0].x == 0,
                    "511 must not emit a bullet or move the entity");
            require(emitter.type == 8 && emitter.color == 10 && emitter.angle == 0.25f &&
                    emitter.spacing == 0.5f && emitter.aimMode == 1 &&
                    emitter.offsetX == 1.25f && emitter.offsetY == -2.5f &&
                    emitter.absoluteX == 3.5f && emitter.absoluteY == -4.5f &&
                    emitter.absoluteOrigin && emitter.spawnRadius == 1.5f &&
                    emitter.speed == 2 && emitter.endSpeed == 0 && emitter.count == 1 &&
                    emitter.layers == 1 && emitter.flags == 35 && emitter.shotSound == 22 &&
                    emitter.transformSound == 40,
                    "511(4,3) must copy all tested typed fields and self-copy must not corrupt");
            require(emitter.extension0.control == 42 && emitter.extension0.type == 37 &&
                    emitter.extension0.parameterA == -123456 && emitter.extension0.parameterB == 987654 &&
                    emitter.extension0.parameterR == .625f && emitter.extension0.parameterS == -8.25f &&
                    emitter.laterExtensions[0].control == 43 && emitter.laterExtensions[0].type == 38 &&
                    emitter.laterExtensions[0].parameterC == 99 && emitter.laterExtensions[0].parameterD == 111 &&
                    emitter.laterExtensions[0].parameterT == 2.25f && emitter.laterExtensions[0].parameterU == -3.5f,
                    "511 must copy extension records by value");
        }
        {
            th128::StageWorld isolated;
            const auto data = program({{"main", join({
                instruction(500, {5}, 0, 0, 255, 1),
                instruction(502, {5, 9, 11}, 0, 0, 255, 3),
                instruction(511, {4, 3}, 0, 0, 255, 2),
                instruction(511, {0, 5}, 0, 0, 255, 2),
                waitLoop(),
            })}});
            require(isolated.load("emitter511-isolated.ecl", data.data(), data.size(), error), error);
            require(isolated.start("emitter511-isolated.ecl", 1, 0), error);
            require(isolated.tick(), isolated.error());
            require(isolated.entities()[0].emitter0.type == 9 &&
                    isolated.entities()[0].emitter0.color == 11,
                    "511 must not overwrite unrelated emitter slot 5");
        }
        {
            th128::StageWorld invalid;
            const auto data = program({{"main", join({instruction(511, {16, 3}, 0, 0, 255, 2), waitLoop()})}});
            require(invalid.load("emitter511-invalid.ecl", data.data(), data.size(), error), error);
            require(invalid.start("emitter511-invalid.ecl", 1, 0), error);
            require(!invalid.tick(), "out-of-range 511 slot must fail");
        }
        {
            th128::StageWorld negative;
            const auto data = program({{"main", join({instruction(511, {4, static_cast<std::uint32_t>(-1)}, 0, 0, 255, 2), waitLoop()})}});
            require(negative.load("emitter511-negative.ecl", data.data(), data.size(), error), error);
            require(negative.start("emitter511-negative.ecl", 1, 0), error);
            require(!negative.tick(), "negative 511 slot must fail");
        }
        std::puts("MBoss1At3 offset408 opcode511(4,3) typed copy passed");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
