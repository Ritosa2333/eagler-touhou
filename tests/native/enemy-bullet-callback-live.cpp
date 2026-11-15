#include "game/StageWorld.hpp"
#include "game/BulletManager.hpp"
#include "game/EnemyBulletCallbackRules.hpp"
#include "../ecl-fixture.hpp"

#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <vector>

using namespace ecl_fixture;

static std::vector<unsigned char> readFile(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("missing profile: " + path);
    return {std::istreambuf_iterator<char>(input), {}};
}
static std::vector<th128::bullet::CallbackSnapshot> snapshots(const th128::bullet::Manager& manager) {
    std::vector<th128::bullet::CallbackSnapshot> result;
    for (std::size_t slot = 0; slot < th128::bullet::Manager::Capacity; ++slot)
        if (const auto snapshot = manager.callbackSnapshot(slot)) result.push_back(*snapshot);
    return result;
}
int main(int argc, char** argv) {
    try {
        require(argc == 2, "usage: enemy-bullet-callback-live profile");
        const auto profileBytes = readFile(argv[1]);
        th128::bullet::Profile profile;
        std::string error;
        require(profile.load(profileBytes.data(), profileBytes.size(), error), error);

        {
            th128::bullet::Manager manager;
            require(manager.setConstructor([](const th128::bullet::Birth&, std::string&) { return true; }), "constructor");
            th128::bullet::Emitter emitter;
            th128::bullet::setTypeColor(emitter, 8, 10);
            th128::bullet::setAngles(emitter, 0, 0);
            emitter.speed = 2; emitter.endSpeed = 2; emitter.count = 3; emitter.layers = 1;
            emitter.positionX = 0; emitter.positionY = 0;
            require(manager.emit(emitter, profile, {0, 400, 0}, 0, error), error);
            auto before = snapshots(manager);
            require(before.size() == 3, "real manager must expose all emitted callbacks");
            const auto first = before[0], second = before[1];
            const th128::bullet::CallbackMutation good{first.slot, first.id, .75f,
                {1, -2, std::numeric_limits<float>::quiet_NaN()}};
            const th128::bullet::CallbackMutation wrong{second.slot, second.id + 1000, .25f, {3, 4, second.velocity.z}};
            const std::array<th128::bullet::CallbackMutation, 2> invalid{good, wrong};
            require(!manager.applyCallbackMutations(invalid, error), "bad second identity must reject atomically");
            require(manager.callbackSnapshot(first.slot)->angle == first.angle, "atomic rejection leaves first unchanged");
            const std::array<th128::bullet::CallbackMutation, 1> valid{good};
            require(manager.applyCallbackMutations(valid, error), error);
            const auto after = manager.callbackSnapshot(first.slot);
            require(after->angle == .75f && after->velocity.x == 1 && after->velocity.y == -2 &&
                    after->velocity.z == first.velocity.z && after->type == first.type && after->id == first.id,
                    "manager mutation preserves owner z/identity/type despite caller z");
            require(manager.applyCallbackMutations({}, error), "empty mutation pass is valid");
            bool reentrant = false;
            require(manager.setUpdater([&](th128::bullet::Birth&, bool&, std::string& reason) {
                reentrant = !manager.applyCallbackMutations({}, reason) && reason.find("reentrant") != std::string::npos;
                return true;
            }), "updater");
            require(manager.tick(error), error);
            require(reentrant, "manager callback mutation rejects reentrancy");
        }

        const auto data = program({{"main", join({
            instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
            instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
            instruction(529, {2}),
            instruction(500, {0}), instruction(502, {0, 8, 10}),
            instruction(504, {0, floatBits(0), floatBits(0)}),
            instruction(505, {0, floatBits(2), floatBits(2)}),
            instruction(506, {0, 3, 1}), instruction(501, {0}),
            instruction(0, {}, 1), instruction(304, {floatBits(-9981), floatBits(-9980)}, 0, 3),
            instruction(301, {1, 4, floatBits(200), floatBits(0)}),
            instruction(0, {}, 100)})}});
        th128::StageWorld world;
        require(world.load("callback.ecl", data.data(), data.size(), error) && world.start("callback.ecl", 1, 0), error);
        require(world.setAppearanceBoundsProvider([](int, int) -> std::optional<th128::enemy::Bounds> {
            return th128::enemy::Bounds{1, 1};
        }, error), error);
        require(world.configureBullets(profileBytes.data(), profileBytes.size(), {0, 400, 0},
            [](const th128::bullet::Birth&, std::string&) { return true; }, error), error);
        require(world.configureBulletUpdater([](th128::bullet::Birth&, bool&, std::string&) { return true; }, error), error);
        require(world.enableLocalEnemyPasses(error), error);
        require(world.tick(), error);
        const auto live = snapshots(world.bullets());
        require(live.size() == 3, "selector2 callback must see three real emitted bullets");
        for (const auto& bullet : live) require(bullet.angle != 0.f || bullet.velocity.x != 0.f, "velocity updated");
        require(world.consumeFrozenBullet(live[0].slot, live[0].id, error), error);
        require(snapshots(world.bullets()).size() == 2, "consume makes one live slot inactive");
        require(world.tick(), error);
        require(std::abs(world.entities().at(0).angle - .75f) < 1e-6f,
                "selector2 advances variables4 exactly once per callback tick");
        const auto beforeFar = snapshots(world.bullets());
        require(world.tick(), error);
        const auto afterFar = snapshots(world.bullets());
        require(beforeFar.size() == afterFar.size() && beforeFar.size() == 2, "far-pass bullet count");
        for (std::size_t index = 0; index < beforeFar.size(); ++index)
            require(beforeFar[index].angle == afterFar[index].angle &&
                    beforeFar[index].velocity.x == afterFar[index].velocity.x &&
                    beforeFar[index].velocity.y == afterFar[index].velocity.y,
                    "out-of-range live bullets are excluded without mutation");

        const auto clearData = program({{"main", join({
            instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
            instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
            instruction(529, {2}), instruction(500, {0}), instruction(502, {0, 8, 10}),
            instruction(504, {0, floatBits(0), floatBits(0)}),
            instruction(505, {0, floatBits(2), floatBits(2)}),
            instruction(506, {0, 3, 1}), instruction(501, {0}), instruction(529, {0}),
            instruction(0, {}, 1), instruction(304, {floatBits(-9981), floatBits(-9980)}, 0, 3),
            instruction(0, {}, 100)})}});
        th128::StageWorld cleared;
        require(cleared.load("clear.ecl", clearData.data(), clearData.size(), error) && cleared.start("clear.ecl", 1, 0), error);
        require(cleared.setAppearanceBoundsProvider([](int, int) -> std::optional<th128::enemy::Bounds> {
            return th128::enemy::Bounds{1, 1};
        }, error), error);
        require(cleared.configureBullets(profileBytes.data(), profileBytes.size(), {0, 400, 0},
            [](const th128::bullet::Birth&, std::string&) { return true; }, error), error);
        require(cleared.configureBulletUpdater([](th128::bullet::Birth&, bool&, std::string&) { return true; }, error), error);
        require(cleared.enableLocalEnemyPasses(error), error);
        require(cleared.tick(), error);
        require(cleared.tick(), error);
        require(cleared.entities().at(0).angle == .25f, "selector0 clears callback and leaves variables4 untouched");

        for (const std::int32_t selector : {3, -1, 1000}) {
            const auto bad = program({{"main", join({instruction(529, {std::uint32_t(selector)}),
                instruction(257, stringWords("child"), 0, 0, 255, 6), instruction(0, {}, 100)})},
                {"child", join({instruction(0, {}, 100)})}});
            th128::StageWorld rejected;
            require(rejected.load("bad.ecl", bad.data(), bad.size(), error) && rejected.start("bad.ecl", 1, 0), error);
            require(!rejected.enableLocalEnemyPasses(error) && rejected.spawnCount() == 0 &&
                    error.find("unsupported native enemy-update callback selector") != std::string::npos,
                    "selector3/unknown must fail explicitly");
        }
        std::puts("PASS real StageWorld/Manager selector2 live owner + selector0 clear + selector3 rejection");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
