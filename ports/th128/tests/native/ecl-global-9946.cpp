#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cstdio>
#include <exception>
#include <string>
#include <vector>

using namespace ecl_fixture;

namespace {
Bytes globalReadProgram(bool spawnChild, bool waitForRetire = false) {
    std::vector<Bytes> parts;
    if (spawnChild) {
        auto spawn = stringWords("child");
        spawn.insert(spawn.end(), {floatBits(0), floatBits(0), 100, 0, 0});
        parts.push_back(instruction(257, spawn, 0, 0, 255, 6));
    }
    if (waitForRetire) parts.push_back(instruction(83, {2}, 0, 0, 255, 1));
    parts.push_back(instruction(42, {0xffffd926u}, 0, 1, 255, 1));
    parts.push_back(instruction(42, {1}, 0, 0, 255, 1));
    parts.push_back(instruction(65, {}, 0, 0, 255, 0));
    const std::int32_t relativeTarget = 24 + 20 + 20 + 24;
    parts.push_back(instruction(13, {static_cast<std::uint32_t>(relativeTarget), 0}, 0, 0, 255, 2));
    parts.push_back(instruction(411, {1}, 0, 0, 255, 1));
    parts.push_back(instruction(83, {100}, 0, 0, 255, 1));
    parts.push_back(instruction(12, {0xffffffecU, 0}, 0, 0, 255, 2));
    parts.push_back(instruction(411, {2}, 0, 0, 255, 1));
    parts.push_back(instruction(83, {100}, 0, 0, 255, 1));
    parts.push_back(instruction(12, {0xffffffecU, 0}, 0, 0, 255, 2));
    return join(parts);
}
}

int main() {
    try {
    std::string error;
    {
        th128::StageWorld one;
        const auto data = program({{"main", globalReadProgram(false)}});
        require(one.load("global9946-one.ecl", data.data(), data.size(), error), error);
        require(one.start("global9946-one.ecl", 1, 0), error);
        require(one.tick(), one.error());
        require(one.entities().size() == 1 && one.entities()[0].life == 1,
                "actual st_a1_2 MainSub03 -9946 read must branch at count 1");
    }
    {
        th128::StageWorld two;
        const auto data = program({{"main", globalReadProgram(true)},
                                   {"child", join({instruction(83, {60}, 0, 0, 255, 1),
                                                   instruction(12, {0xffffffecU, 0}, 0, 0, 255, 2)})}});
        require(two.load("global9946-two.ecl", data.data(), data.size(), error), error);
        require(two.start("global9946-two.ecl", 1, 0), error);
        require(two.tick(), two.error());
        require(two.entities().size() == 2 && two.entities()[0].life == 2,
                "actual st_a1_2 MainSub03 -9946 read must branch at count 2");
    }
    {
        th128::StageWorld retired;
        const auto data = program({{"main", globalReadProgram(true, true)},
                                   {"child", join({instruction(83, {1}, 0, 0, 255, 1),
                                                   instruction(1, {}, 0, 0, 255, 0)})}});
        require(retired.load("global9946-retired.ecl", data.data(), data.size(), error), error);
        require(retired.start("global9946-retired.ecl", 1, 0), error);
        for (int tick = 0; tick < 6 && retired.entities()[0].life != 1; ++tick)
            require(retired.tick(), retired.error());
        require(retired.entities()[0].life == 1 && retired.entities().size() == 2 &&
                !retired.entities()[1].active,
                "after child retirement -9946 must return 1, not historical vector size 2");
    }
    {
        th128::StageWorld unsupported;
        const auto data = program({{"main", instruction(42, {0xffffd8f9u}, 0, 1, 255, 1)}});
        require(unsupported.load("global9946-unsupported.ecl", data.data(), data.size(), error), error);
        require(unsupported.start("global9946-unsupported.ecl", 1, 0), error);
        require(!unsupported.tick(), "unsupported global must remain explicit");
        require(unsupported.error().find("-9991") != std::string::npos, unsupported.error());
    }
    std::puts("actual st_a1_2 MainSub03 offset152 -9946 count boundary passed");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
