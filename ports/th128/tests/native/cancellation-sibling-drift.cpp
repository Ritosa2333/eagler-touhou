#include "game/CancellationEffects.hpp"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace {
using Bytes = std::vector<std::uint8_t>;
using Effects = th128::effects::CancellationEffects;

struct Family {
    int root;
    int child;
};

constexpr std::array<Family, 5> families{{
    {377, 376},
    {369, 368},
    {373, 372},
    {379, 378},
    {383, 382},
}};

Bytes read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "missing " << path << '\n';
        std::exit(2);
    }
    return {std::istreambuf_iterator<char>(file), {}};
}

void require(bool condition, const std::string& message) {
    if (condition) return;
    std::cerr << "FAIL " << message << '\n';
    std::exit(1);
}

const Effects::Snapshot* root(const Effects& effects, Effects::Handle handle) {
    for (const auto& snapshot : effects.snapshots())
        if (snapshot.handle == handle && snapshot.parent == 0)
            return &snapshot;
    return nullptr;
}

std::vector<Effects::Snapshot> children(const Effects& effects, Effects::Handle handle) {
    std::vector<Effects::Snapshot> result;
    for (const auto& snapshot : effects.snapshots())
        if (snapshot.handle == handle && snapshot.parent != 0)
            result.push_back(snapshot);
    return result;
}

void near(float actual, float expected) {
    require(std::isfinite(actual) && std::fabs(actual - expected) < .0001f,
            "position mismatch: " + std::to_string(actual) + " != " + std::to_string(expected));
}

void runFamily(const Family& family, const Bytes& bullet) {
    std::string error;
    Effects effects;
    require(effects.load(bullet.data(), bullet.size(), error), error);
    unsigned calls = 0;
    effects.setGameRandom([&] { ++calls; return 0u; });
    const auto handle = effects.spawn(family.root, 320, 16, 0, 23, error);
    require(handle.has_value(), "spawn " + std::to_string(family.root) + ": " + error);
    require(effects.rootCount() == 1 && effects.nodeCount() == 4, "root plus three children");
    const auto* bornRoot = root(effects, *handle);
    require(bornRoot && bornRoot->state.script == family.root && bornRoot->state.ticks == 1,
            "fresh root identity/ticks");
    require(bornRoot->state.x == 0 && bornRoot->state.y == 0 && bornRoot->state.z == 0,
            "fresh root must have explicit zero authored pose");
    const auto bornChildren = children(effects, *handle);
    require(bornChildren.size() == 3, "exactly three children");
    for (const auto& child : bornChildren) {
        require(child.state.script == family.child && child.state.layer == 15 && child.state.mode == 1,
                "child script/layer/mode");
        require(child.sprite && child.sprite->width > 0, "child sprite metadata");
    }
    const unsigned callsAfterSpawn = calls;
    const std::array<float, 3> velocity{4, -3, 2};
    const std::array<float, 3> zero{0, 0, 0};
    require(!effects.initializeNativeDrift(*handle, velocity, .5f, zero, error),
            "clock must be exactly 1");
    require(!effects.initializeNativeDrift(*handle, velocity, 1.f, {1, 0, 0}, error),
            "nonzero global start must fail");
    require(!effects.initializeNativeDrift(*handle, {std::numeric_limits<float>::infinity(), 0, 0}, 1.f, zero, error),
            "non-finite velocity must fail");
    require(!effects.initializeNativeDrift(*handle, {std::numeric_limits<float>::max(), 0, 0}, 1.f, zero, error),
            "overflowing target must fail");
    require(effects.initializeNativeDrift(*handle, velocity, 1.f, zero, error), error);
    require(calls == callsAfterSpawn, "drift initialization must not consume RNG");
    require(!effects.initializeNativeDrift(*handle, velocity, 1.f, zero, error),
            "already initialized root must be rejected");
    for (int tick = 0; tick < 30; ++tick) require(effects.tick(error), "drift tick failed: " + error);
    const auto* finalRoot = root(effects, *handle);
    require(finalRoot, "root must survive authored drift duration");
    near(finalRoot->state.x, 40);
    near(finalRoot->state.y, -30);
    near(finalRoot->state.z, 20);
    require(calls > callsAfterSpawn,
            "authored drift ticks must consume the original RNG domain");
    bool cleaned = false;
    for (int tick = 0; tick < 70; ++tick) {
        require(effects.tick(error), "cleanup tick failed: " + error);
        if (effects.rootCount() == 0 && effects.nodeCount() == 0) {
            cleaned = true;
            break;
        }
    }
    require(cleaned && effects.snapshots().empty(), "family must retire and clean up");
    require(!effects.spawn(family.child, 320, 16, 0, 23, error), "child cannot be a root");
    require(!effects.spawn(900, 320, 16, 0, 23, error), "unrelated script must be rejected");
}
}

int main(int argc, char** argv) {
    require(argc == 2, "pass actual bullet.anm");
    const auto bullet = read(argv[1]);
    for (const auto& family : families) runFamily(family, bullet);
    std::cout << "PASS cancellation drift 377/376 and sibling 369/373/379/383 native clock1 interpolation/RNG/cleanup\n";
    return 0;
}
