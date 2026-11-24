#include "game/StageSettlementHost.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

std::string readText(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return std::string(std::istreambuf_iterator<char>(file), {});
}

std::vector<std::uint8_t> readBytes(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(file), {});
}

std::vector<std::int32_t> fixtureInts(const std::string& text, const std::string& key) {
    const auto label = text.find("\"" + key + "\"");
    assert(label != std::string::npos);
    auto at = text.find('[', label);
    assert(at != std::string::npos);
    std::vector<std::int32_t> values;
    while (true) {
        at = text.find_first_of("-0123456789]", at);
        assert(at != std::string::npos);
        if (text[at] == ']') break;
        std::size_t end = at;
        if (text[end] == '-') ++end;
        while (end < text.size() && text[end] >= '0' && text[end] <= '9') ++end;
        values.push_back(std::stoi(text.substr(at, end - at)));
        at = end;
    }
    return values;
}

struct Owners {
    std::array<std::array<std::uint32_t, 2>, 8> ids{};
    std::array<std::vector<std::uint32_t>, 16> entries;
    std::vector<std::string> events;
    bool failFlag = false, failWalk = false, failRetire = false, failFinish = false;

    bool flag(std::string&) {
        events.push_back("flag");
        return !failFlag;
    }
    std::uint32_t slot(std::uint32_t group, std::uint32_t index) {
        events.push_back("slot:" + std::to_string(group) + ":" + std::to_string(index));
        return ids[group][index];
    }
    bool walk(std::uint32_t group, std::uint32_t index,
              const th128::stage::StageSettlementCallbacks::Visit& visit, std::string& error) {
        events.push_back("walk:" + std::to_string(group) + ":" + std::to_string(index));
        if (failWalk) {
            error = "walk failed";
            return false;
        }
        for (const auto entity : entries[group * 2 + index])
            if (!visit(entity)) return false;
        return true;
    }
    bool retire(std::uint32_t entity, std::string& error) {
        events.push_back("retire:" + std::to_string(entity));
        if (failRetire) {
            error = "retire failed";
            return false;
        }
        return true;
    }
    bool finish(std::string&) {
        events.push_back("finish");
        return !failFinish;
    }
    th128::stage::StageSettlementCallbacks callbacks() {
        return {
            [this](std::string& error) { return flag(error); },
            [this](std::uint32_t group, std::uint32_t index) { return slot(group, index); },
            [this](std::uint32_t group, std::uint32_t index,
                   const th128::stage::StageSettlementCallbacks::Visit& visit, std::string& error) {
                return walk(group, index, visit, error);
            },
            [this](std::uint32_t entity, std::string& error) { return retire(entity, error); },
            [this](std::string& error) { return finish(error); }};
    }
};

th128::stage::StageSettlementContext context(int stage, std::int32_t score) {
    th128::stage::StageSettlementContext value;
    value.stage = stage;
    value.score = score;
    return value;
}

}

int main(int argc, char** argv) {
    assert(argc == 3);
    const auto text = readText(argv[2]);
    const auto table = fixtureInts(text, "stageBaseScoreTable");
    assert(table.size() == 24);
    for (std::size_t index = 0; index < table.size(); ++index)
        assert(th128::stage::StageSettlementHost::stageBaseScore(int(index)) == table[index]);
    const auto safe = fixtureInts(text, "safeOrdinaryStages");
    assert(safe.size() == 9);
    for (int stage = 0; stage < 24; ++stage) {
        const bool expected = std::find(safe.begin(), safe.end(), stage) != safe.end();
        assert(th128::stage::StageSettlementHost::supportedStage(stage) == expected);
    }
    assert(!th128::stage::StageSettlementHost::supportedStage(3));
    assert(!th128::stage::StageSettlementHost::supportedStage(5));
    assert(!th128::stage::StageSettlementHost::supportedStage(16));
    assert(th128::stage::kStageSettlementScoreCap == 999999999u);
    assert(th128::stage::kStageSettlementScoreClamp == 1000000000u);
    const auto bytes = readBytes(argv[1]);
    std::string error;

    {
        th128::stage::StageSettlementHost host;
        assert(host.load(bytes, error));
        Owners owners;
        owners.ids[0][0] = 101;
        owners.entries[0] = {1001, 1002};
        owners.ids[1][0] = 102;
        owners.entries[2] = {2001};
        owners.ids[7][1] = 108;
        owners.entries[15] = {7008};
        assert(host.begin(context(1, 0), owners.callbacks(), error));
        const auto state = host.state();
        assert(state.loaded && state.begun && !state.failed);
        assert(state.scalarApplied && state.presentationBound && state.cleanupCalled);
        assert(!state.presentationComplete && !state.stageTransitionClaimed);
        assert(state.context.score == 100000);
        assert(state.context.manager.baseScore5a38 == 1000000);
        assert((state.context.manager.flags5a04 & 0x100u) != 0);
        assert(state.context.manager.handle58cc != 0);
        assert(state.frontSpawnLayer == 23);
        assert(state.context.manager.field5a08 == 0xffffffffu);
        assert(state.context.manager.field5a0c == 0);
        assert(state.context.manager.field5a10 == 0.0f);
        assert(state.context.manager.field5a14 == 0x4b6f58u);
        assert((state.context.manager.field5a18 & 1u) != 0);
        assert(state.ownerSlotsVisited == 16 && state.retiredEntities == 4);
        const auto born = host.snapshots();
        assert(born.size() == 1);
        assert(born[0].state.script == 82 && born[0].state.layer == 17);
        assert(born[0].state.sprite >= 0);
        std::vector<std::string> expected{"flag"};
        for (std::uint32_t group = 0; group < 8; ++group) {
            for (std::uint32_t index = 0; index < 2; ++index) {
                expected.push_back("slot:" + std::to_string(group) + ":" + std::to_string(index));
                if (owners.ids[group][index] == 0) continue;
                expected.push_back("walk:" + std::to_string(group) + ":" + std::to_string(index));
                for (const auto entity : owners.entries[group * 2 + index])
                    expected.push_back("retire:" + std::to_string(entity));
            }
        }
        expected.push_back("finish");
        assert(owners.events == expected);
        std::size_t retireCount = 0;
        for (const auto& event : owners.events) retireCount += event.starts_with("retire:");
        assert(retireCount == 4);
        for (int tick = 0; tick < 60; ++tick) assert(host.tick(error));
        const auto held = host.state();
        assert(!held.presentationComplete);
        assert(held.presentationTicks == 60);
        const auto heldSnapshot = host.snapshots();
        assert(heldSnapshot.size() == 1);
        assert(heldSnapshot[0].state.clock == 40);
        assert(heldSnapshot[0].state.held);
        assert(host.interrupt(1, error));
        for (int tick = 0; tick < 200 && !host.state().presentationComplete; ++tick)
            assert(host.tick(error));
        assert(host.state().presentationComplete);
        assert(host.snapshots().empty());
        assert(host.tick(error));
        const auto size = owners.events.size();
        assert(!host.begin(context(1, 0), owners.callbacks(), error));
        assert(owners.events.size() == size);
        assert(!host.state().failed);
    }

    {
        th128::stage::StageSettlementHost host;
        assert(host.load(bytes, error));
        Owners owners;
        auto state = context(2, 999999900);
        state.manager.field5a14 = 0xdeadbeefu;
        state.manager.field5a18 = 1;
        assert(host.begin(state, owners.callbacks(), error));
        const auto complete = host.state();
        assert(complete.context.score == 999999999);
        assert(complete.context.manager.baseScore5a38 == 2000000);
        assert(complete.context.manager.field5a14 == 0xdeadbeefu);
        assert((complete.context.manager.field5a18 & 1u) != 0);
        assert(complete.context.manager.field5a08 == 0xffffffffu);
    }

    {
        th128::stage::StageSettlementHost host;
        assert(host.load(bytes, error));
        Owners owners;
        assert(!host.begin(context(3, 0), owners.callbacks(), error));
        assert(!host.state().begun && !host.state().failed);
        assert(host.snapshots().empty());
        assert(host.begin(context(1, 0), owners.callbacks(), error));
        assert(host.state().begun);
    }

    {
        th128::stage::StageSettlementHost host;
        assert(host.load(bytes, error));
        Owners owners;
        assert(!host.tick(error));
        assert(!host.state().failed);
        owners.failFlag = true;
        assert(!host.begin(context(1, 0), owners.callbacks(), error));
        const auto failed = host.state();
        assert(failed.failed && failed.presentationBound && failed.scalarApplied);
        assert(!failed.cleanupCalled && !failed.presentationComplete);
        assert(!failed.stageTransitionClaimed);
        assert(host.snapshots().empty());
        const auto message = error;
        assert(!host.begin(context(1, 0), owners.callbacks(), error));
        assert(error == message);
        assert(!host.tick(error));
        assert(error == message);
    }

    {
        th128::stage::StageSettlementHost host;
        assert(host.load(bytes, error));
        Owners owners;
        owners.ids[0][0] = 101;
        owners.entries[0] = {1001};
        owners.failRetire = true;
        assert(!host.begin(context(1, 0), owners.callbacks(), error));
        const auto failed = host.state();
        assert(failed.failed && !failed.cleanupCalled && !failed.presentationComplete);
        assert(failed.retiredEntities == 0 && failed.ownerSlotsVisited == 1);
        assert(!failed.stageTransitionClaimed);
        assert(host.snapshots().empty());
    }

    {
        th128::stage::StageSettlementHost host;
        assert(host.load(bytes, error));
        Owners owners;
        owners.ids[0][0] = 101;
        owners.entries[0] = {1001};
        owners.failWalk = true;
        assert(!host.begin(context(1, 0), owners.callbacks(), error));
        assert(host.state().failed && !host.state().cleanupCalled);
        assert(host.snapshots().empty());
    }

    {
        th128::stage::StageSettlementHost host;
        assert(host.load(bytes, error));
        Owners owners;
        owners.ids[0][0] = 101;
        owners.entries[0] = {1001, 1002};
        owners.failFinish = true;
        assert(!host.begin(context(1, 0), owners.callbacks(), error));
        const auto failed = host.state();
        assert(failed.failed && !failed.cleanupCalled);
        assert(failed.retiredEntities == 2);
        assert(!failed.presentationComplete && !failed.stageTransitionClaimed);
        assert(host.snapshots().empty());
    }

    {
        th128::stage::StageSettlementHost host;
        const std::array<std::uint8_t, 8> truncated{};
        assert(!host.load(truncated, error));
        assert(!host.state().failed);
        assert(host.load(bytes, error));
    }

    std::cout << "actual front82 stage-clear ownership, eight-group cleanup, score cap and sticky failures passed\n";
}
