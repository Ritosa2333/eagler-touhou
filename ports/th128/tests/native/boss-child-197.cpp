#include "game/EnemyVisuals.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {
using th128::enemy::EnemyVisuals;

std::vector<unsigned char> read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { std::cerr << "missing " << path << '\n'; std::exit(2); }
    return {std::istreambuf_iterator<char>(file), {}};
}

void require(bool condition, const std::string& message) {
    if (condition) return;
    std::cerr << "FAIL " << message << '\n';
    std::exit(1);
}

const EnemyVisuals::Snapshot* findScript(const std::vector<EnemyVisuals::Snapshot>& snapshots, int script) {
    const auto found = std::find_if(snapshots.begin(), snapshots.end(),
        [script](const auto& node) { return node.state.script == script; });
    return found == snapshots.end() ? nullptr : &*found;
}

const EnemyVisuals::Snapshot* findChild(const std::vector<EnemyVisuals::Snapshot>& snapshots, int script) {
    const auto found = std::find_if(snapshots.begin(), snapshots.end(),
        [script](const auto& node) { return node.state.script == script && node.parent != 0; });
    return found == snapshots.end() ? nullptr : &*found;
}

bool nearly(float first, float second) { return std::fabs(first - second) < 0.001f; }

void checkPose(const EnemyVisuals::Snapshot& root, const EnemyVisuals::Snapshot& child) {
    const float x = root.world.x + child.state.x + child.state.engineX + child.state.offsetX;
    const float y = root.world.y + child.state.y + child.state.engineY + child.state.offsetY;
    const float z = root.world.z + child.state.z + child.state.engineZ + child.state.offsetZ;
    require(nearly(child.world.x, x) && nearly(child.world.y, y) && nearly(child.world.z, z),
        "child " + std::to_string(child.state.script) + " must compose root197 pose plus local offsets");
    const float rotation = child.state.rotation + (child.state.mode == 1 ? root.world.rotation : 0);
    require(nearly(child.world.rotation, rotation),
        "child " + std::to_string(child.state.script) + " rotation must compose its authored ancestry");
}
}

int main(int argc, char** argv) {
    require(argc == 2 || argc == 3, "pass actual bullet.anm and optional unknown-child probe");
    const auto bullet = read(argv[1]);
    std::string error;
    constexpr std::uint32_t entity = 1970;
    constexpr int slot = 4;

    EnemyVisuals actual;
    require(actual.load(0, bullet.data(), bullet.size(), error), error);
    unsigned randomCalls = 0;
    std::uint32_t randomState = 0x85ebca6bu;
    actual.setRandomSources(
        [&] { ++randomCalls; randomState ^= randomState << 13; randomState ^= randomState >> 17; randomState ^= randomState << 5; return randomState; },
        [&] { ++randomCalls; return 3u; });
    actual.setUpdateControl({true, 0});
    require(actual.create(entity, slot, 0, 197, 7, error), "owner197 create failed: " + error);

    auto snapshots = actual.snapshots();
    const auto* root = findScript(snapshots, 197);
    const auto* child194 = findChild(snapshots, 194);
    require(root && child194, "owner197 must birth its original t0 child194");
    require(root->parent == 0 && root->entity == entity && root->slot == slot,
        "root197 must retain its owner slot identity");
    require(root->state.bank == 0 && root->state.layer == 7 && root->state.sprite < 0,
        "root197 must be the original bank0 transform root");
    require(child194->parent == root->node && child194->entity == entity && child194->slot == slot,
        "child194 must attach to root197 and inherit owner slot");
    require(child194->state.bank == 0 && child194->state.script == 194 && child194->state.layer == 6,
        "child194 must retain its authored bank/script/layer");
    require(child194->state.spriteBank == 0 && child194->state.sprite == 472,
        "child194 must retain its authored bank0 sprite472");
    require(child194->state.mode == 0, "child194 authored mode");

    require(actual.position(entity, 100, 200, 3), "root197 owner pose");
    for (int tick = 0; tick < 4; ++tick) require(actual.tickAll(error), "owner197 tick failed: " + error);
    snapshots = actual.snapshots();
    root = findScript(snapshots, 197);
    child194 = findChild(snapshots, 194);
    const auto* child195 = findChild(snapshots, 195);
    const auto* child196 = findChild(snapshots, 196);
    require(root && child194 && child195 && child196, "owner197 must birth children at t0/t3/t4");
    require(randomCalls != 0, "owner197/children must consume the authored RNG calls");
    require(child195->parent == root->node && child196->parent == root->node,
        "all children must attach to root197");
    require(child195->entity == entity && child195->slot == slot && child196->entity == entity && child196->slot == slot,
        "all children must inherit the owner slot");
    require(child195->state.bank == 0 && child195->state.layer == 4 && child195->state.mode == 1
            && child195->state.spriteBank == 0 && child195->state.sprite == 473,
        "child195 authored bank/layer/mode/sprite");
    require(child196->state.bank == 0 && child196->state.layer == 6 && child196->state.mode == 1
            && child196->state.spriteBank == 0 && child196->state.sprite == 473,
        "child196 authored bank/layer/mode/sprite");
    checkPose(*root, *child194);
    checkPose(*root, *child195);
    checkPose(*root, *child196);

    const auto first194 = child194->node;
    const auto first195 = child195->node;
    const auto first196 = child196->node;
    const auto waitRetired = [&](std::uint32_t identity, const char* label) {
        for (int tick = 0; tick < 50; ++tick) {
            require(actual.tickAll(error), "owner197 lifecycle failed: " + error);
            const auto current = actual.snapshots();
            if (std::none_of(current.begin(), current.end(), [identity](const auto& node) { return node.node == identity; }))
                return;
        }
        require(false, std::string("owner197 child did not retire at its authored t30: ") + label);
    };
    waitRetired(first194, "194");
    waitRetired(first195, "195");
    waitRetired(first196, "196");
    require(findScript(actual.snapshots(), 197),
        "root197 authored op4 loop keeps the root alive after child retirement");
    require(actual.removeOwner(entity), "owner197 removeOwner");
    require(actual.snapshots().empty(), "root retirement must clean up all attached children and repeated generations");

    if (argc == 3) {
        const auto unknownBullet = read(argv[2]);
        EnemyVisuals unknown;
        require(unknown.load(0, unknownBullet.data(), unknownBullet.size(), error), error);
        unknown.setRandomSources([] { return 5u; }, [] { return 1u; });
        unknown.setUpdateControl({true, 0});
        const bool created = unknown.create(77, 1, 0, 197, 7, error);
        require(!created, "unknown child193 topology must be rejected");
        require(error.find("owner=197 child=193 bank=0 opcode=88 flags=0 detached=0 offset=0") != std::string::npos,
            "unknown topology must retain the exact rejection cause: " + error);
    }

    std::cout << "PASS boss child197 t0/t3/t4 births, layers/sprites, parent pose, lifecycle/retire and unknown reject\n";
    return 0;
}
