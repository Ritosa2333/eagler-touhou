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

bool require(bool condition, const std::string& message) {
    if (condition) return true;
    std::cerr << "FAIL " << message << '\n';
    std::exit(1);
}

const EnemyVisuals::Snapshot* findScript(const std::vector<EnemyVisuals::Snapshot>& snapshots, int script) {
    const auto found = std::find_if(snapshots.begin(), snapshots.end(),
        [script](const auto& node) { return node.state.script == script; });
    return found == snapshots.end() ? nullptr : &*found;
}

const EnemyVisuals::Snapshot* findParent(const std::vector<EnemyVisuals::Snapshot>& snapshots, int script) {
    const auto found = std::find_if(snapshots.begin(), snapshots.end(),
        [script](const auto& node) { return node.state.script == script && node.parent != 0; });
    return found == snapshots.end() ? nullptr : &*found;
}

bool nearly(float first, float second) { return std::fabs(first - second) < 0.001f; }
}

int main(int argc, char** argv) {
    if (!require(argc == 3 || argc == 4, "pass actual bullet.anm, ascii.anm and optional mutated unknown-child probe")) return 1;
    const auto bullet = read(argv[1]);
    const auto font = read(argv[2]);
    std::string error;
    EnemyVisuals actual;
    if (!require(actual.load(0, bullet.data(), bullet.size(), error), error)
        || !require(actual.load(11, font.data(), font.size(), error), error))
        return 1;
    unsigned randomCalls = 0;
    std::uint32_t randomState = 0x9e3779b9u;
    actual.setRandomSources(
        [&] { ++randomCalls; randomState ^= randomState << 13; randomState ^= randomState >> 17; randomState ^= randomState << 5; return randomState; },
        [&] { ++randomCalls; return 7u; });
    actual.setUpdateControl({true, 0});

    constexpr std::uint32_t entity = 1270;
    constexpr int slot = 3;
    if (!require(actual.create(entity, slot, 0, 127, 7, error), "owner127 create failed: " + error)) return 1;
    auto snapshots = actual.snapshots();
    const auto* root = findScript(snapshots, 127);
    const auto* child126 = findParent(snapshots, 126);
    if (!require(root && child126, "owner127 must birth its original t0 child126")) return 1;
    require(root->parent == 0 && root->entity == entity && root->slot == slot,
        "root127 must retain its owner slot identity");
    require(root->state.bank == 0 && root->state.layer == 7 && root->state.sprite < 0,
        "root127 must be the original bank0 transform root");
    require(child126->parent == root->node && child126->entity == entity && child126->slot == slot,
        "child126 must attach to root127 and inherit owner slot");
    require(child126->state.bank == 0 && child126->state.script == 126 && child126->state.layer == 16,
        "child126 must retain its authored bank/script/layer");
    require(child126->state.spriteBank == 11 && child126->state.sprite == 313,
        "child126 negative sprite must resolve through real ascii313");
    require(child126->state.mode == 16 && child126->state.geometryCount == 36,
        "child126 must retain the original mode16/36 geometry");

    require(actual.position(entity, 100, 200, 3), "root127 owner pose");
    if (!require(actual.tickAll(error), "owner127 t1 tick failed: " + error)) return 1;
    snapshots = actual.snapshots();
    root = findScript(snapshots, 127);
    child126 = findParent(snapshots, 126);
    const auto* child121 = findParent(snapshots, 121);
    if (!require(root && child126 && child121, "owner127 t1 must birth the second original child121")) return 1;
    require(randomCalls != 0, "owner127 t1 must consume the authored RNG calls");
    require(child121->parent == root->node && child121->entity == entity && child121->slot == slot,
        "child121 must attach to root127 and inherit owner slot");
    require(child121->state.bank == 0 && child121->state.spriteBank == 0 && child121->state.sprite == 452,
        "child121 must retain its authored bank0 sprite452");
    require(child121->state.layer == 16 && child121->state.mode == 1, "child121 layer/mode");
    require(nearly(child126->world.x, 100) && nearly(child126->world.y, 200) && nearly(child126->world.z, 3),
        "child126 must inherit root127 parent-relative pose; got " + std::to_string(child126->world.x) + ","
        + std::to_string(child126->world.y) + "," + std::to_string(child126->world.z));
    const float child121X = root->world.x + child121->state.x + child121->state.engineX + child121->state.offsetX;
    const float child121Y = root->world.y + child121->state.y + child121->state.engineY + child121->state.offsetY;
    require(nearly(child121->world.x, child121X) && nearly(child121->world.y, child121Y)
            && nearly(child121->world.z, 3),
        "child121 must compose root127 pose plus its local authored offsets; got "
        + std::to_string(child121->world.x) + "," + std::to_string(child121->world.y) + ","
        + std::to_string(child121->world.z) + " expected " + std::to_string(child121X) + ","
        + std::to_string(child121Y));
    require(nearly(child126->world.rotation, child126->state.rotation)
            && nearly(child121->world.rotation, child121->state.rotation),
        "mode1 children must compose the zero parent rotation exactly once");

    const auto oldRoot = root->node, old126 = child126->node, old121 = child121->node;
    if (!require(actual.create(entity, slot, 0, 127, 7, error), "owner127 reload failed: " + error)) return 1;
    require(actual.tickAll(error), error);
    snapshots = actual.snapshots();
    root = findScript(snapshots, 127);
    child126 = findParent(snapshots, 126);
    child121 = findParent(snapshots, 121);
    if (!require(root && child126 && child121, "reloaded owner127 must recreate both children")) return 1;
    require(root->node != oldRoot && child126->node != old126 && child121->node != old121,
        "owner127 reload must replace the old generation identities");
    require(std::none_of(snapshots.begin(), snapshots.end(), [&](const auto& node) {
            return node.node == oldRoot || node.node == old126 || node.node == old121; }),
        "old owner127 generation must be retired");
    require(actual.removeOwner(entity), "owner127 retirement");
    require(actual.snapshots().empty(), "owner127 retirement must remove the whole attached tree");

    EnemyVisuals budget;
    if (!require(budget.load(0, bullet.data(), bullet.size(), error), error)
        || !require(budget.load(11, font.data(), font.size(), error), error)) return 1;
    unsigned budgetRandom = 1;
    budget.setRandomSources([&] { return budgetRandom++; }, [&] { return 1u; });
    budget.setUpdateControl({true, 0});
    for (std::size_t index = 0; index + 2 < EnemyVisuals::MaxNodes; ++index)
        if (!require(budget.create(10000 + std::uint32_t(index), 0, 0, 99, 7, error),
                     "budget filler failed: " + error)) return 1;
    if (!require(budget.create(1, 1, 0, 127, 7, error), "budget root127 failed: " + error)) return 1;
    require(!budget.tickAll(error) && error.find("budget") != std::string::npos,
        "owner127 second child must fail at the genuine node budget");

    if (argc == 4) {
        const auto unknownBullet = read(argv[3]);
        EnemyVisuals unknown;
        if (!require(unknown.load(0, unknownBullet.data(), unknownBullet.size(), error), error)
            || !require(unknown.load(11, font.data(), font.size(), error), error)) return 1;
        unsigned unknownRandom = 1;
        unknown.setRandomSources([&] { return unknownRandom++; }, [&] { return 1u; });
        unknown.setUpdateControl({true, 0});
        const bool created = unknown.create(77, 1, 0, 127, 7, error);
        require(!created, "unknown child125 topology must be rejected");
        require(error.find("owner=127 child=125 bank=0 opcode=88 flags=0 detached=0 offset=0") != std::string::npos,
            "unknown topology must retain the exact rejection cause: " + error);
    }

    std::cout << "PASS boss child127 t0/t1 births, ascii313, parent pose/layer/slot, reload/retire and budget\n";
    return 0;
}
