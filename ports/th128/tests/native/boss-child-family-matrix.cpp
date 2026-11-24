#include "game/EnemyVisuals.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {
using th128::enemy::EnemyVisuals;

struct Family {
    int root;
    std::array<int, 3> children;
    std::array<int, 3> layers;
    std::array<int, 3> sprites;
    std::array<int, 3> modes;
};

struct Birth {
    int script;
    std::uint32_t node;
};

constexpr std::array<Family, 5> families{{
    {193, {190, 191, 192}, {6, 4, 6}, {472, 473, 473}, {0, 1, 1}},
    {201, {198, 199, 200}, {6, 4, 6}, {472, 473, 473}, {0, 1, 1}},
    {205, {202, 203, 204}, {6, 4, 6}, {472, 473, 473}, {0, 1, 1}},
    {213, {210, 211, 212}, {6, 4, 6}, {472, 473, 473}, {0, 1, 1}},
    {217, {214, 215, 216}, {6, 4, 6}, {472, 473, 473}, {0, 1, 1}},
}};

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

const EnemyVisuals::Snapshot* findNode(const std::vector<EnemyVisuals::Snapshot>& snapshots,
                                       std::uint32_t node) {
    const auto found = std::find_if(snapshots.begin(), snapshots.end(),
        [node](const auto& item) { return item.node == node; });
    return found == snapshots.end() ? nullptr : &*found;
}

const EnemyVisuals::Snapshot* findScript(const std::vector<EnemyVisuals::Snapshot>& snapshots,
                                         int script) {
    const auto found = std::find_if(snapshots.begin(), snapshots.end(),
        [script](const auto& node) { return node.state.script == script; });
    return found == snapshots.end() ? nullptr : &*found;
}

const EnemyVisuals::Snapshot* findChild(const std::vector<EnemyVisuals::Snapshot>& snapshots,
                                        int script) {
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
        "child " + std::to_string(child.state.script) + " pose composition");
    const float rotation = child.state.rotation + (child.state.mode == 1 ? root.world.rotation : 0);
    require(nearly(child.world.rotation, rotation),
        "child " + std::to_string(child.state.script) + " rotation ancestry");
}

void runFamily(const Family& family, const std::vector<unsigned char>& bullet, std::string& error) {
    EnemyVisuals actual;
    require(actual.load(0, bullet.data(), bullet.size(), error), error);
    unsigned randomCalls = 0;
    std::uint32_t randomState = 0x85ebca6bu;
    actual.setRandomSources(
        [&] { ++randomCalls; randomState ^= randomState << 13; randomState ^= randomState >> 17; randomState ^= randomState << 5; return randomState; },
        [&] { ++randomCalls; return 3u; });
    actual.setUpdateControl({true, 0});
    constexpr int slot = 4;
    const std::uint32_t entity = 3000u + std::uint32_t(family.root);
    require(actual.create(entity, slot, 0, family.root, 7, error),
        "root " + std::to_string(family.root) + " create failed: " + error);

    auto snapshots = actual.snapshots();
    const auto* root = findScript(snapshots, family.root);
    require(root && root->parent == 0 && root->entity == entity && root->slot == slot,
        "root " + std::to_string(family.root) + " identity");
    require(root->state.bank == 0 && root->state.layer == 7,
        "root " + std::to_string(family.root) + " bank/layer");
    const std::uint32_t rootNode = root->node;
    const auto* firstSnapshot = findChild(snapshots, family.children[0]);
    require(firstSnapshot, "child0 must be born at t0");
    const std::uint32_t first = firstSnapshot->node;
    require(!findChild(snapshots, family.children[1]) && !findChild(snapshots, family.children[2]),
        "only child0 may exist at t0");
    require(actual.position(entity, 100, 200, 3), "root owner pose");

    for (int tick = 1; tick <= 2; ++tick) {
        require(actual.tickAll(error), "early tick failed: " + error);
        snapshots = actual.snapshots();
        require(findNode(snapshots, first), "child0 must remain alive before t3");
        require(!findChild(snapshots, family.children[1]) && !findChild(snapshots, family.children[2]),
            "no child1/child2 before their authored births");
    }
    require(actual.tickAll(error), "t3 tick failed: " + error);
    snapshots = actual.snapshots();
    require(findNode(snapshots, first), "child0 must remain alive at t3");
    const auto* secondSnapshot = findChild(snapshots, family.children[1]);
    require(secondSnapshot, "child1 must be born at t3");
    const std::uint32_t second = secondSnapshot->node;
    require(!findChild(snapshots, family.children[2]), "child2 must not exist at t3");
    require(actual.tickAll(error), "t4 tick failed: " + error);
    snapshots = actual.snapshots();
    const auto* thirdSnapshot = findChild(snapshots, family.children[2]);
    require(thirdSnapshot, "child2 must be born at t4");
    const std::uint32_t third = thirdSnapshot->node;
    const std::array<Birth, 3> births{{{family.children[0], first},
                                       {family.children[1], second},
                                       {family.children[2], third}}};
    require(first != second && second != third && first != third, "first-generation node ids must be unique");
    require(randomCalls != 0, "family must consume authored RNG");
    root = findNode(snapshots, rootNode);
    require(root, "root must remain alive after first births");

    for (int index = 0; index < 3; ++index) {
        const auto* child = findNode(snapshots, births[index].node);
        require(child, "first-generation child missing");
        require(child->parent == rootNode && child->entity == entity && child->slot == slot,
            "child attach/slot");
        require(child->state.bank == 0 && child->state.script == family.children[index]
                && child->state.layer == family.layers[index],
            "child bank/script/layer");
        require(child->state.spriteBank == 0 && child->state.sprite == family.sprites[index],
            "child sprite");
        require(child->state.mode == family.modes[index], "child mode");
        checkPose(*root, *child);
    }

    for (const Birth& birth : births) {
        bool retired = false;
        for (int tick = 0; tick < 50; ++tick) {
            require(actual.tickAll(error), "lifecycle tick failed: " + error);
            if (!findNode(actual.snapshots(), birth.node)) {
                retired = true;
                break;
            }
        }
        require(retired, "first-generation child " + std::to_string(birth.script) +
                         " must retire by its authored lifetime");
    }
    require(findNode(actual.snapshots(), rootNode), "root op4 loop must survive first-generation retirement");
    require(actual.removeOwner(entity), "removeOwner");
    require(actual.snapshots().empty(), "root removal must clean all generations");
}
}

int main(int argc, char** argv) {
    require(argc == 2 || argc == 3, "pass actual bullet.anm and optional unknown-child probe");
    const auto bullet = read(argv[1]);
    std::string error;
    for (const auto& family : families) runFamily(family, bullet, error);
    if (argc == 3) {
        const auto unknown = read(argv[2]);
        EnemyVisuals rejected;
        require(rejected.load(0, unknown.data(), unknown.size(), error), error);
        rejected.setRandomSources([] { return 5u; }, [] { return 1u; });
        rejected.setUpdateControl({true, 0});
        const bool created = rejected.create(77, 1, 0, 193, 7, error);
        require(!created, "mutated root193 child189 must be rejected");
        require(error.find("owner=193 child=189") != std::string::npos,
            "unknown topology must expose owner/child cause: " + error);
    }
    std::cout << "PASS boss child families 193/201/205/213/217 first-gen tick births, identity retirement, pose, layers/sprites/modes and removal\n";
    return 0;
}
