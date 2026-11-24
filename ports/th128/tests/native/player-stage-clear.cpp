#include "game/PlayerHitHost.hpp"
#include "game/PlayerStageClearSlots.hpp"
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace {

std::vector<unsigned char> readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
}

th128::anm::Runtime runtime(std::string& error) {
    th128::anm::Runtime context;
    context.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    context.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    context.unsupported = [&error](int, int, int opcode, const char* reason) {
        if (error.empty()) error = "player slot ANM opcode=" + std::to_string(opcode) + ": " + reason;
    };
    return context;
}

}

int main(int argc, char** argv) {
    assert(argc == 2);
    const auto animation = readFile(argv[1]);
    const th128::player::hit::Timer zero{-1, 0, 0, 0x4b6f58, 1};
    const th128::player::HitHostSnapshot snapshot{
        {1, 0, 0}, zero, zero, zero, {true, 0}, {0, 0, 0}, {true, 0}};
    std::string error;
    th128::player::PlayerHitHost player;
    assert(player.configure(animation.data(), animation.size(), snapshot,
        [](int, th128::player::hit::Position, int, std::string&) { return true; },
        [](int, std::string&) { return true; }, error));
    const auto before = player.snapshot();
    assert(player.syncPlayerFlags(0x20, error));
    assert(player.snapshot().gate.flags438ec == 0x20);
    assert(player.applyStageClearStart(error));
    auto stage = player.snapshot();
    assert(stage.gate.flags438ec == 0x30 && stage.stageClearCounter438f0 == 0);
    assert(stage.stateTimer.current == before.stateTimer.current);
    assert(stage.protection.current == before.protection.current);
    for (unsigned tick = 0; tick < 30; ++tick) assert(player.advanceStageClearUnit(error));
    assert(player.snapshot().stageClearCounter438f0 == 30);
    assert(player.syncPlayerFlags(0x20, error));
    assert(player.advanceStageClearUnit(error));
    assert(player.snapshot().stageClearCounter438f0 == 30);
    assert(player.applyStageClearStart(error));
    assert(player.snapshot().gate.flags438ec == 0x30);
    assert(player.snapshot().stageClearCounter438f0 == 0);
    assert(player.advanceAnimationUnit(error));
    assert(player.animationState() != nullptr);

    th128::anm::Registry registry;
    assert(registry.loadBank(7, animation.data(), animation.size(), error));
    th128::anm::VM headA, childA, headB;
    auto slotRuntime = runtime(error);
    assert(headA.bind(registry, 7, 0, slotRuntime));
    slotRuntime = runtime(error);
    assert(childA.bind(registry, 7, 0, slotRuntime));
    slotRuntime = runtime(error);
    assert(headB.bind(registry, 7, 0, slotRuntime));

    std::array<std::array<th128::player::StageClearSlot, 2>, 8> groups{};
    groups[0][0] = {0x1001, &headA};
    groups[1][0] = {0x1002, &headB};
    std::vector<std::string> events;
    const auto retire = [&](std::uint32_t handle, th128::anm::VM& vm, int code, std::string&) {
        assert(code == 3);
        if (handle == 0x1001) assert(&vm == &headA);
        if (handle == 0x1002) assert(&vm == &headB);
        if (handle == 0x2001) assert(&vm == &childA);
        events.push_back("retire:" + std::to_string(handle));
        vm.interrupt(3);
        return true;
    };
    const auto children = [&](std::uint32_t handle, th128::anm::VM&,
                              const th128::player::PlayerStageClearSlots::ChildVisit& visit,
                              std::string& childError) {
        if (handle != 0x1001) return true;
        events.push_back("children:1001");
        return visit(0x2001, childA, childError);
    };
    th128::player::PlayerStageClearSlots slots;
    assert(slots.configure(groups, false, retire, children, error));
    assert(slots.state().configuredHandles == 2 && !slots.state().emptyContext);
    assert(slots.clear(error));
    assert(slots.state().retired && slots.state().retiredHandles == 3);
    assert((events == std::vector<std::string>{"retire:4097", "children:1001", "retire:8193",
                                               "retire:4098"}));
    assert(slots.clear(error));
    assert(slots.state().retiredHandles == 3);

    th128::player::PlayerStageClearSlots zeroCandidate;
    assert(!zeroCandidate.configure({}, false, retire, children, error));
    assert(error.find("explicit") != std::string::npos);
    th128::player::PlayerStageClearSlots empty;
    assert(empty.configure({}, true, retire, children, error));
    assert(empty.state().emptyContext && empty.state().configuredHandles == 0);
    assert(empty.clear(error) && empty.state().retired && empty.state().retiredHandles == 0);

    auto invalid = groups;
    invalid[0][0].vm = nullptr;
    th128::player::PlayerStageClearSlots missingVm;
    assert(!missingVm.configure(invalid, false, retire, children, error));
    th128::player::PlayerStageClearSlots missingCallbacks;
    assert(!missingCallbacks.configure(groups, false, {}, children, error));

    th128::player::PlayerStageClearSlots failing;
    const auto failingRetire = [](std::uint32_t handle, th128::anm::VM&, int, std::string& reason) {
        if (handle == 0x1002) {
            reason = "required player stage-clear retire rejected";
            return false;
        }
        return true;
    };
    assert(failing.configure(groups, false, failingRetire, children, error));
    assert(!failing.clear(error));
    const auto message = error;
    assert(failing.state().failed && !failing.state().retired);
    assert(failing.state().retiredHandles == 2);
    assert(!failing.clear(error) && error == message);
    assert(failing.state().retiredHandles == 2);

    std::cout << "player stage-clear flag/counter and explicit 8x2 ANM handle ownership passed\n";
}
