#include "game/RouteChoiceHost.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {
using th128::routechoice::Host;
using th128::routechoice::Input;
using th128::routechoice::Phase;
using th128::routechoice::Snapshot;

const Snapshot* byScript(const Host& host, int script) {
    for (const auto& node : host.snapshots())
        if (node.state.script == script)
            return &node;
    return nullptr;
}

void ticks(Host& host, int count, std::uint32_t pressed = 0) {
    std::string error;
    for (int index = 0; index < count; ++index)
        assert(host.tick(Input{pressed}, error));
}

std::vector<unsigned char> read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return {std::istreambuf_iterator<char>(file), {}};
}

void selectStage(Host& host, int stage, int first, int second) {
    std::string error;
    assert(host.start({stage, 0, 0}, error));
    assert(host.status().phase == Phase::Intro);
    assert(host.snapshots().size() == 1);
    ticks(host, 40);
    assert(host.status().phase == Phase::Intro);
    assert(host.status().message70 == 40);
    assert(host.snapshots().size() == 4);
    ticks(host, 1);
    assert(host.status().phase == Phase::Intro);
    assert(host.status().message70 == 41);
    ticks(host, 1);
    assert(host.status().phase == Phase::Select);
    assert(host.status().message70 == 42);
    assert(host.status().message74 == 1);
    assert(host.status().message78 == 2);
    assert(host.status().message7c == 0);
    assert(host.status().message80 == 3);
    const auto* child167 = byScript(host, 167);
    const auto* child168 = byScript(host, 168);
    assert(child167 && child168);
    assert(child167->state.sprite == first);
    assert(child168->state.sprite == second);
    assert(!host.result());
}

void finishConfirm(Host& host) {
    std::string error;
    assert(host.status().phase == Phase::Confirm);
    ticks(host, 39);
    assert(!host.result());
    ticks(host, 1);
    assert(host.result());
    assert(*host.result() == host.status().message78);
    for (int index = 0; index < 30 && !host.snapshots().empty(); ++index)
        assert(host.tick({}, error));
    assert(host.snapshots().empty());
    assert(host.status().phase == Phase::Done);
    assert(host.result());
}
}

int main(int argc, char** argv) {
    assert(argc == 2);
    const auto bytes = read(argv[1]);
    std::string error;
    {
        Host host;
        assert(!host.load({}, error));
        assert(!host.load(std::span<const std::uint8_t>(bytes), error));
        assert(host.status().phase == Phase::Failed);
        assert(!host.result());
        assert(host.snapshots().empty());
        assert(!host.start({1, 0, 0}, error));
    }
    {
        Host host;
        assert(host.load(bytes, error));
        assert(host.snapshots().empty());
        assert(!host.start({2, 0, 0}, error));
        assert(host.status().phase == Phase::Failed);
        assert(!host.start({1, 0, 0}, error));
    }
    {
        Host host;
        assert(host.load(bytes, error));
        assert(host.start({1, 0, 0}, error));
        assert(host.status().stage == 1);
        assert(host.status().handle == 1);
        const auto* root = byScript(host, 169);
        assert(root);
        assert(root->state.layer == 25);
        assert(root->state.sprite == 115);
        ticks(host, 40);
        assert(host.snapshots().size() == 4);
        ticks(host, 1);
        assert(host.status().phase == Phase::Intro);
        ticks(host, 1);
        assert(host.status().phase == Phase::Select);
        assert(byScript(host, 167)->state.sprite == 0x70);
        assert(byScript(host, 168)->state.sprite == 0x71);
        ticks(host, 1, th128::routechoice::Host::Confirm);
        assert(host.status().phase == Phase::Select);
        assert(host.status().message74 == 1);
        ticks(host, 120);
        assert(host.status().phase == Phase::Select);
        assert(host.status().message74 == 1);
        assert(!host.result());
        ticks(host, 1, Host::Up);
        assert(host.status().message78 == 0);
        assert(host.status().message74 == 2);
        assert(host.status().message70 == 1);
        assert(host.status().lastInterrupt == 7);
        assert(host.status().interruptCount == 1);
        assert(host.status().lastSound == 0xa);
        assert(host.status().soundCount == 1);
        ticks(host, 1, Host::Up);
        assert(host.status().message78 == 0);
        assert(host.status().interruptCount == 1);
        assert(host.status().message70 == 2);
        ticks(host, 1, Host::Confirm);
        assert(host.status().phase == Phase::Confirm);
        assert(host.status().message70 == 1);
        assert(host.status().message74 == 3);
        assert(host.status().lastInterrupt == 9);
        assert(host.status().lastSound == 7);
        assert(host.status().soundCount == 2);
        finishConfirm(host);
        assert(host.start({6, 0, 0}, error));
        assert(host.status().phase == Phase::Intro);
        assert(host.snapshots().size() == 1);
        assert(!host.result());
    }
    {
        Host host;
        assert(host.load(bytes, error));
        selectStage(host, 6, 0x6f, 0x71);
        assert(!host.destroy(error));
        assert(host.status().phase == Phase::Failed);
        assert(!host.result());
        assert(host.snapshots().empty());
        assert(!host.tick({}, error));
    }
    {
        Host host;
        assert(host.load(bytes, error));
        selectStage(host, 11, 0x6f, 0x70);
        ticks(host, 1, Host::Confirm);
        assert(host.status().phase == Phase::Select);
        ticks(host, 1, Host::Down);
        assert(host.status().message78 == 1);
        assert(host.status().lastInterrupt == 8);
        ticks(host, 1, Host::Confirm);
        assert(host.status().phase == Phase::Confirm);
        assert(host.status().lastInterrupt == 10);
        finishConfirm(host);
        assert(*host.result() == 1);
        assert(host.destroy(error));
    }
    std::cout << "actual front169/166/167/168 route choice lifecycle passed\n";
}
