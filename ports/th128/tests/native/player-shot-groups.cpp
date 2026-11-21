#include "game/PlayerShotGroups.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<unsigned char> readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
}

}

int main(int argc, char** argv) {
    assert(argc == 3);
    const auto sht = readFile(argv[1]);
    const auto animation = readFile(argv[2]);
    std::string error;
    th128::playershot::PlayerShotGroups groups;
    assert(!groups.configureSelection(1700.f, 1, 0, {320, 416, 0}, {}, error));
    assert(groups.load(sht, animation, error));
    std::vector<int> codes;
    std::vector<std::uint32_t> retired;
    const auto lifecycle = [&](std::uint32_t handle, th128::anm::VM&, int code, std::string&) {
        codes.push_back(code);
        retired.push_back(handle);
        return true;
    };
    assert(groups.configureSelection(1700.f, 1, 0, {320, 416, 0}, lifecycle, error));
    auto state = groups.state();
    assert(state.loaded && state.configured && !state.failed);
    assert(state.group == 11 && state.selector == 0);
    assert(state.activeGroups == 8 && state.activeHandles == 16);
    for (std::uint32_t group = 0; group < 8; ++group) {
        assert(groups.slot(group, 0) != 0 && groups.slot(group, 1) != 0);
        assert(groups.vm(groups.slot(group, 0)) != nullptr);
        assert(groups.vm(groups.slot(group, 1)) != nullptr);
    }
    auto snapshots = groups.snapshots();
    assert(snapshots.size() >= 16);
    bool saw0b = false, saw0e = false, sawSprite = false;
    for (const auto& snapshot : snapshots) {
        if (snapshot.script == 0xb && snapshot.slot == 0) saw0b = true;
        if (snapshot.script == 0xe && snapshot.slot == 1) saw0e = true;
        if (snapshot.sprite) sawSprite = true;
    }
    assert(saw0b && saw0e && sawSprite);
    for (int frame = 0; frame < 5; ++frame) assert(groups.tick({320, 416, 0}, error));
    assert(groups.state().activeHandles == 16);
    std::vector<std::uint32_t> visited;
    assert(groups.walk(groups.slot(0, 0), [&](std::uint32_t handle, th128::anm::VM&, std::string&) {
        visited.push_back(handle);
        return true;
    }, error));
    assert(!visited.empty() && visited.front() == groups.slot(0, 0));
    const auto oldHandle = groups.slot(0, 0);
    codes.clear();
    retired.clear();
    assert(groups.updateSelection(0.f, 1, 0, {300, 400, 0}, error));
    state = groups.state();
    assert(state.group == 0 && state.activeGroups == 2 && state.activeHandles == 4);
    assert(!codes.empty() && codes.front() == 1);
    assert(groups.nativeState3f8(oldHandle) == 1);
    assert(groups.slot(0, 0) != 0 && groups.slot(1, 1) != 0 && groups.slot(7, 1) == 0);
    const auto single = groups.slot(0, 0);
    const auto finalHandle = groups.slot(1, 1);
    codes.clear();
    retired.clear();
    assert(groups.retireWithChildren(single, 3, error));
    assert(groups.nativeState3f8(single) == 3 && groups.slot(0, 0) == 0);
    assert(groups.vm(single) == nullptr);
    assert(!groups.walk(single, [](std::uint32_t, th128::anm::VM&, std::string&) { return true; }, error));
    assert(groups.clear(3, error));
    state = groups.state();
    assert(state.retired && state.activeHandles == 0 && groups.snapshots().empty());
    assert(groups.slot(0, 0) == 0 && groups.slot(1, 1) == 0);
    assert(groups.nativeState3f8(finalHandle) == 3);
    assert(!codes.empty() && codes.front() == 3);
    assert(!retired.empty());

    th128::playershot::PlayerShotGroups failing;
    assert(failing.load(sht, animation, error));
    const auto reject = [](std::uint32_t, th128::anm::VM&, int, std::string& reason) {
        reason = "required player shot-group lifecycle rejected";
        return false;
    };
    assert(failing.configureSelection(1700.f, 1, 0, {320, 416, 0}, reject, error));
    assert(!failing.clear(3, error));
    assert(failing.state().failed && failing.state().activeHandles == 16);
    const auto message = error;
    assert(!failing.tick({320, 416, 0}, error) && error == message);
    assert(failing.state().activeHandles == 16);

    th128::playershot::PlayerShotGroups unloaded;
    assert(!unloaded.updateSelection(0.f, 0, 0, {0, 0, 0}, error));
    assert(!unloaded.configureSelection(0.f, 0, 0, {0, 0, 0}, lifecycle, error));

    th128::playershot::PlayerShotGroups unsupported;
    assert(unsupported.load(sht, animation, error));
    assert(!unsupported.configureSelection(0.f, 1, 1, {320, 416, 0}, lifecycle, error));
    assert(error.find("selector0") != std::string::npos);

    th128::playershot::PlayerShotGroups throwing;
    assert(throwing.load(sht, animation, error));
    const auto throwLifecycle = [](std::uint32_t, th128::anm::VM&, int, std::string&) -> bool {
        throw std::runtime_error("player shot-group lifecycle throw");
    };
    assert(throwing.configureSelection(0.f, 1, 0, {320, 416, 0}, throwLifecycle, error));
    assert(!throwing.clear(3, error) && throwing.state().failed);
    assert(throwing.state().activeHandles == 4);

    th128::playershot::PlayerShotGroups erroring;
    assert(erroring.load(sht, animation, error));
    const auto errorLifecycle = [](std::uint32_t, th128::anm::VM&, int, std::string& reason) {
        reason = "player shot-group lifecycle wrote an error";
        return true;
    };
    assert(erroring.configureSelection(0.f, 1, 0, {320, 416, 0}, errorLifecycle, error));
    assert(!erroring.clear(3, error) && erroring.state().failed);
    assert(erroring.state().activeHandles == 4);

    th128::playershot::PlayerShotGroups badVisitor;
    assert(badVisitor.load(sht, animation, error));
    assert(badVisitor.configureSelection(0.f, 1, 0, {320, 416, 0}, lifecycle, error));
    const auto visitorThrow = [](std::uint32_t, th128::anm::VM&, std::string&) -> bool {
        throw std::runtime_error("player shot-group visitor throw");
    };
    assert(!badVisitor.walk(badVisitor.slot(0, 0), visitorThrow, error) && badVisitor.state().failed);
    assert(badVisitor.state().activeHandles == 4);

    th128::playershot::PlayerShotGroups missing;
    const std::vector<unsigned char> shortSht(8);
    assert(!missing.load(shortSht, animation, error));
    assert(!missing.configureSelection(0.f, 0, 0, {0, 0, 0}, lifecycle, error));

    std::cout << "actual pl00a SHT player shot groups, nonzero 8x2 Scene/VM ownership and callbacks passed\n";
}
