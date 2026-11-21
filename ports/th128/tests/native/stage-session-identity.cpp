#include "game/StageSessionIdentity.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace {
const char* roleName(th128::stage::Role role) {
    switch (role) {
    case th128::stage::Role::Start: return "start";
    case th128::stage::Role::Second: return "second";
    case th128::stage::Role::Terminal: return "terminal";
    case th128::stage::Role::Extra: return "extra";
    }
    return "none";
}

const th128::stage::StageResourceRecord* findRecord(
    const std::vector<th128::stage::StageResourceRecord>& records, int id) {
    for (const auto& record : records)
        if (record.id == id)
            return &record;
    return nullptr;
}
}

int main() {
    std::vector<th128::stage::StageResourceRecord> records;
    std::string operation;
    th128::stage::SessionIdentity defaultIdentity;
    while (std::cin >> operation) {
        if (operation == "STAGE") {
            th128::stage::StageResourceRecord record;
            assert(std::cin >> record.id >> record.background >> record.ecl >> record.message
                   >> record.logo >> record.music[0] >> record.music[1]);
            records.push_back(record);
        } else if (operation == "DEFAULT") {
            const auto role = th128::stage::roleForStage(defaultIdentity.stageId);
            std::cout << "default stage=" << defaultIdentity.stageId
                      << " ecl=" << defaultIdentity.eclName
                      << " difficulty=" << defaultIdentity.difficulty
                      << " progress=" << defaultIdentity.progress
                      << " role=" << (role ? roleName(*role) : "none") << '\n';
        } else if (operation == "CHECK") {
            int id = 0, difficulty = 0;
            std::int32_t progress = 0;
            std::string ecl;
            assert(std::cin >> id >> ecl >> difficulty >> progress);
            const auto* record = findRecord(records, id);
            if (!record) {
                std::cout << "invalid catalog-record-missing\n";
                continue;
            }
            const th128::stage::SessionIdentity identity{id, ecl, difficulty, progress};
            std::string error;
            if (!th128::stage::validateSession(identity, *record, error)) {
                std::cout << "invalid " << error << '\n';
                continue;
            }
            const auto role = th128::stage::roleForStage(id);
            const auto cap = th128::stage::capability(*role);
            std::cout << "valid role=" << roleName(*role)
                      << " carry=" << (cap.acceptCarry ? 1 : 0)
                      << " handoff=" << (cap.ordinaryHandoff ? 1 : 0)
                      << " playable=" << (cap.normalPlayable ? 1 : 0) << '\n';
        } else if (operation == "TRANSITION") {
            int sourceId = 0, sourceDifficulty = 0, targetId = 0, targetDifficulty = 0;
            int gate74 = 0;
            std::int32_t sourceProgress = 0, targetProgress = 0, message78 = 0;
            std::string sourceEcl, targetEcl;
            assert(std::cin >> sourceId >> sourceEcl >> sourceDifficulty >> sourceProgress
                   >> targetId >> targetEcl >> targetDifficulty >> targetProgress
                   >> gate74 >> message78);
            const auto* sourceRecord = findRecord(records, sourceId);
            const auto* targetRecord = findRecord(records, targetId);
            if (!sourceRecord || !targetRecord) {
                std::cout << "rejected catalog-record-missing\n";
                continue;
            }
            const th128::stage::SessionIdentity source{
                sourceId, sourceEcl, sourceDifficulty, sourceProgress};
            const th128::stage::SessionIdentity target{
                targetId, targetEcl, targetDifficulty, targetProgress};
            std::string error;
            std::cout << (th128::stage::validateCarryTransition(
                              source, *sourceRecord, target, *targetRecord, gate74, message78, error)
                              ? "accepted\n"
                              : "rejected " + error + "\n");
        }
    }
}
