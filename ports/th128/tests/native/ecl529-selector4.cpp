#include "game/StageWorld.hpp"
#include "game/BulletManager.hpp"
#include "game/EnemyBulletCallbackRules.hpp"
#include "../ecl-fixture.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

using namespace ecl_fixture;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error(std::string("missing profile: ") + path);
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
        require(argc == 2, "usage: ecl529-selector4 profile");
        const auto profileBytes = readFile(argv[1]);
        th128::bullet::Profile profile;
        std::string error;
        require(profile.load(profileBytes.data(), profileBytes.size(), error), error);

        const auto data = program({{"main", join({
            instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
            instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
            instruction(529, {4}),
            instruction(500, {0}), instruction(502, {0, 8, 10}),
            instruction(504, {0, floatBits(0), floatBits(0)}),
            instruction(505, {0, floatBits(2), floatBits(2)}),
            instruction(506, {0, 3, 1}), instruction(501, {0}),
            instruction(0, {}, 1), instruction(304, {floatBits(-9981), floatBits(-9980)}, 0, 3),
            instruction(0, {}, 100)})}});
        th128::StageWorld world;
        require(world.load("selector4.ecl", data.data(), data.size(), error)
            && world.start("selector4.ecl", 1, 0), error);
        require(world.setAppearanceBoundsProvider([](int, int) -> std::optional<th128::enemy::Bounds> {
            return th128::enemy::Bounds{1, 1};
        }, error), error);
        require(world.configureBullets(profileBytes.data(), profileBytes.size(), {0, 400},
            [](const th128::bullet::Birth&, std::string&) { return true; }, error), error);
        require(world.configureBulletUpdater([](th128::bullet::Birth&, bool&, std::string&) { return true; },
            error), error);
        require(world.enableLocalEnemyPasses(error), error);
        require(world.tick(), error);
        const auto live = snapshots(world.bullets());
        require(live.size() == 3, "selector4 callback must see three real emitted bullets");
        for (const auto& bullet : live) {
            require(std::abs(bullet.speed - 2.25f) < 1e-6f,
                    "selector4 adds entity +0x24c once to speed; got " + std::to_string(bullet.speed));
            require(std::abs(bullet.velocity.x - std::cos(double(bullet.angle)) * 2.25) < 1e-5f
                && std::abs(bullet.velocity.y - std::sin(double(bullet.angle)) * 2.25) < 1e-5f,
                    "selector4 recomputes velocity from unchanged angle; got " + std::to_string(bullet.velocity.x)
                    + "," + std::to_string(bullet.velocity.y) + " angle=" + std::to_string(bullet.angle));
        }
        th128::enemybullet::EntityContext entity{0, 0, .25f, .5f};
        std::vector<th128::enemybullet::BulletState> states(7);
        states[0].active = true; states[0].motionState = 1;
        states[0].position = {0, 0}; states[0].speed = 2; states[0].angle = 0;
        states[1].active = true; states[1].motionState = 1;
        states[1].position = {95.999f, 0}; states[1].speed = 2; states[1].angle = 0;
        states[2].active = true; states[2].motionState = 1;
        states[2].position = {96, 0}; states[2].speed = 2; states[2].angle = 0;
        states[3].active = true; states[3].motionState = 1;
        states[3].position = {96.001f, 0}; states[3].speed = 2; states[3].angle = 0;
        states[4].active = true; states[4].motionState = 0;
        states[4].position = {0, 0}; states[4].speed = 2; states[4].angle = 0;
        states[5].active = false; states[5].motionState = 1;
        states[5].position = {0, 0}; states[5].speed = 2; states[5].angle = 0;
        states[6].active = true; states[6].motionState = 2;
        states[6].position = {0, 0}; states[6].speed = 2; states[6].angle = 0;
        const auto plan = th128::enemybullet::evaluate(th128::enemybullet::Selector::RadiusCurve,
                                                       entity, states);
        require(plan.supported && plan.effects.size() == states.size(), "selector4 plan cardinality");
        require(plan.effects[0].matched && plan.effects[1].matched
            && !plan.effects[2].matched && !plan.effects[3].matched
            && !plan.effects[4].matched && !plan.effects[5].matched && !plan.effects[6].matched,
            "selector4 strict <96 and active/motion1 filter");
        require(plan.effects[0].speed && std::abs(*plan.effects[0].speed - 2.25f) < 1e-6f,
            "typed selector4 updates in-range speed only");
        require(std::abs(plan.nextVariable4 - .75f) < 1e-6f,
            "typed selector4 advances variable4 once; got " + std::to_string(plan.nextVariable4));
        require(!plan.effects[2].matched && !plan.effects[2].speed,
            "typed selector4 leaves exact-96 speed untouched");
        std::puts("PASS typed ECL529 selector4 ring callback on real StageWorld/Manager");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL %s\n", error.what());
        return 1;
    }
}
