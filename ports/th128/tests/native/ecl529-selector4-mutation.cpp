#include "game/BulletManager.hpp"
#include "game/EnemyBulletCallbackRules.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <limits>
#include <span>
#include <string>
#include <vector>

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
        if (argc != 2) return 2;
        const auto bytes = readFile(argv[1]);
        th128::bullet::Profile profile;
        std::string error;
        if (!profile.load(bytes.data(), bytes.size(), error)) return 1;
        th128::bullet::Manager manager;
        if (!manager.setConstructor([](const th128::bullet::Birth&, std::string&) { return true; })) return 1;
        th128::bullet::Emitter emitter;
        th128::bullet::setTypeColor(emitter, 8, 10);
        th128::bullet::setAngles(emitter, 0, 0);
        emitter.speed = 2; emitter.endSpeed = 2; emitter.count = 2; emitter.layers = 1;
        emitter.positionX = 0; emitter.positionY = 0;
        if (!manager.emit(emitter, profile, {0, 400}, 0, error)) return 1;
        auto before = snapshots(manager);
        if (before.size() != 2) return 1;
        const th128::bullet::CallbackMutation good{before[0].slot, before[0].id, .75f, {1, -2}, 3.0f};
        const th128::bullet::CallbackMutation stale{before[1].slot, before[1].id + 1000, .25f, {3, 4}, 4.0f};
        const std::vector<th128::bullet::CallbackMutation> atomic{good, stale};
        if (manager.applyCallbackMutations(atomic, error)) return 1;
        const auto afterAtomic = snapshots(manager);
        if (afterAtomic.size() != 2 || afterAtomic[0].speed != before[0].speed
            || afterAtomic[0].angle != before[0].angle || afterAtomic[0].velocity.x != before[0].velocity.x) {
            std::fprintf(stderr, "stale mutation was not atomic\n");
            return 1;
        }
        const th128::bullet::CallbackMutation bad{before[0].slot, before[0].id, .75f, {1, -2},
            std::numeric_limits<float>::quiet_NaN()};
        if (manager.applyCallbackMutations(std::span<const th128::bullet::CallbackMutation>(&bad, 1), error))
            return 1;
        const auto afterBad = snapshots(manager);
        if (afterBad[0].speed != before[0].speed || afterBad[0].angle != before[0].angle) {
            std::fprintf(stderr, "non-finite mutation was not atomic\n");
            return 1;
        }
        if (!manager.applyCallbackMutations(std::span<const th128::bullet::CallbackMutation>(&good, 1), error))
            return 1;
        const auto afterGood = snapshots(manager);
        if (afterGood[0].speed != 3.0f || afterGood[0].angle != .75f) {
            std::fprintf(stderr, "valid mutation did not apply\n");
            return 1;
        }
        std::vector<th128::enemybullet::BulletState> states(2);
        states[0].active = true; states[0].motionState = 1;
        states[0].position = {0, 0}; states[0].speed = 2; states[0].angle = 0;
        states[1].active = true; states[1].motionState = 1;
        states[1].position = {200, 0}; states[1].speed = 9; states[1].angle = 0;
        const th128::enemybullet::EntityContext entity{0, 0, .25f, .5f};
        const auto curve = th128::enemybullet::evaluate(th128::enemybullet::Selector::Curve, entity, states);
        if (!curve.supported || !curve.effects[0].matched || curve.effects[0].speed
            || curve.effects[1].matched) {
            std::fprintf(stderr, "selector2 focused vector changed\n");
            return 1;
        }
        std::puts("PASS selector4 Manager mutation atomic speed + selector2 unchanged");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL %s\n", error.what());
        return 1;
    }
}
