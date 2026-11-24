#include "game/PostBossEventRules.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

namespace {

using namespace th128::postboss;

const char* statusName(Status status) {
    switch (status) {
    case Status::Rejected: return "rejected";
    case Status::NoOperation: return "no-operation";
    case Status::RequiresOwner: return "requires-owner";
    }
    return "unknown";
}

const char* kindName(OperationKind kind) {
    switch (kind) {
    case OperationKind::StageClearEntry: return "stage-clear-entry";
    case OperationKind::ClearSlots: return "clear-slots";
    case OperationKind::SaveReplay: return "save-replay";
    case OperationKind::ResetPath: return "reset-path";
    case OperationKind::AddScore: return "add-score";
    case OperationKind::SetObjectState: return "set-object-state";
    case OperationKind::RouteAdvance: return "route-advance";
    case OperationKind::TimeScale: return "time-scale";
    case OperationKind::SpawnAnm: return "spawn-anm";
    }
    return "unknown";
}

const char* targetName(WriteTarget target) {
    switch (target) {
    case WriteTarget::GameManager: return "game-manager";
    case WriteTarget::Message: return "message";
    case WriteTarget::Stage: return "stage";
    case WriteTarget::StageMirror: return "stage-mirror";
    case WriteTarget::NextStageRecord: return "next-stage-record";
    case WriteTarget::PlayCount: return "play-count";
    }
    return "unknown";
}

const char* modeName(WriteMode mode) {
    switch (mode) {
    case WriteMode::OrMask: return "or-mask";
    case WriteMode::Assign: return "assign";
    case WriteMode::AssignHandle: return "assign-handle";
    case WriteMode::Add: return "add";
    case WriteMode::IncrementCapped: return "increment-capped";
    }
    return "unknown";
}

const char* effectName(EffectKind kind) {
    switch (kind) {
    case EffectKind::Operation: return "operation";
    case EffectKind::Write: return "write";
    }
    return "unknown";
}

}

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command;
        if (!(input >> command) || command != "CASE") {
            std::cout << "PROTOCOL\n";
            return 2;
        }
        Input value;
        int suspend = 0, session = 0, gate = 0, mode = 0;
        if (!(input >> value.opcode >> value.stage >> value.statsStage >> value.difficulty
                  >> value.rateA >> value.rateB >> value.messageTimer >> value.messageField78
                  >> value.playCount >> suspend >> session >> gate >> mode)) {
            std::cout << "PROTOCOL\n";
            return 2;
        }
        value.suspendFlags = suspend;
        value.sessionGate = session;
        value.flowGate = gate;
        value.flowMode = mode;
        const Result result = plan(value);
        std::cout << "PLAN " << statusName(result.status) << '\n';
        std::cout << "REASON " << result.reason << '\n';
        std::cout << "BONUS " << result.bonusRaw << '\n';
        for (const auto& operation : result.operations) {
            char address[16];
            char receiverBase[16];
            std::snprintf(address, sizeof(address), "0x%08x", operation.address);
            std::snprintf(receiverBase, sizeof(receiverBase), "0x%08x", operation.receiverBase);
            std::cout << "OP " << kindName(operation.kind) << ' ' << address << ' '
                      << operation.a << ' ' << operation.b << ' ' << operation.c << ' '
                      << receiverBase << ' ' << operation.receiverOffset << ' '
                      << operation.outputSlotOffset << ' ' << operation.sequence << '\n';
        }
        for (const auto& write : result.writes)
            std::cout << "WRITE " << targetName(write.target) << ' ' << write.offset << ' '
                      << modeName(write.mode) << ' ' << write.value << ' ' << write.cap << ' '
                      << write.sequence << '\n';
        for (const auto& effect : result.effects)
            std::cout << "EFFECT " << effectName(effect.kind) << ' ' << effect.index << ' '
                      << effect.sequence << ' ' << effect.afterOperation << '\n';
        std::cout << "END\n";
    }
    return 0;
}
