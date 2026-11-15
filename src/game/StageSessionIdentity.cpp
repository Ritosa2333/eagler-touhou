#include "StageSessionIdentity.hpp"
#include "RouteHandoffRules.hpp"

namespace th128::stage {
namespace {

bool safeBasename(const std::string& name) {
    if (name.empty() || name.size() > 128 || name.find("..") != std::string::npos)
        return false;
    for (const char character : name) {
        if (!((character >= 'a' && character <= 'z') ||
              (character >= 'A' && character <= 'Z') ||
              (character >= '0' && character <= '9') ||
              character == '_' || character == '.' || character == '-'))
            return false;
    }
    return name.size() > 4 && name.compare(name.size() - 4, 4, ".ecl") == 0;
}

bool hasResourceFields(const StageResourceRecord& record) {
    return !record.background.empty() && !record.message.empty() && !record.logo.empty() &&
           !record.music[0].empty() && !record.music[1].empty();
}

}

std::optional<Role> roleForStage(int stageId) noexcept {
    switch (stageId) {
    case 1:
    case 6:
    case 11:
        return Role::Start;
    case 2:
    case 4:
    case 7:
    case 9:
    case 12:
    case 14:
        return Role::Second;
    case 3:
    case 5:
    case 8:
    case 10:
    case 13:
    case 15:
        return Role::Terminal;
    case 16:
        return Role::Extra;
    default:
        return std::nullopt;
    }
}

Capability capability(Role role) noexcept {
    switch (role) {
    case Role::Start:
        return {false, true, true};
    case Role::Second:
        return {true, true, true};
    case Role::Terminal:
        return {true, false, true};
    case Role::Extra:
        return {false, false, false};
    }
    return {};
}

bool canAcceptCarry(Role role) noexcept {
    return capability(role).acceptCarry;
}

bool canRequestOrdinaryHandoff(Role role) noexcept {
    return capability(role).ordinaryHandoff;
}

bool validateSession(const SessionIdentity& identity, const StageResourceRecord& record,
                     std::string& error) {
    error.clear();
    const auto role = roleForStage(identity.stageId);
    if (!role) {
        error = "stage id is outside the original playable table";
        return false;
    }
    if (record.id != identity.stageId) {
        error = "catalog record id does not match caller stage id";
        return false;
    }
    if (identity.eclName.empty() || identity.eclName != record.ecl ||
        !safeBasename(identity.eclName) || !safeBasename(record.ecl)) {
        error = "catalog record ecl does not match the safe requested basename";
        return false;
    }
    if (!hasResourceFields(record)) {
        error = "catalog record is not the complete original resource record";
        return false;
    }
    if (identity.difficulty < 0 || identity.difficulty > 3) {
        error = "difficulty is outside the normal 0..3 range";
        return false;
    }
    return true;
}

bool validateCarryTransition(const SessionIdentity& source, const StageResourceRecord& sourceRecord,
                             const SessionIdentity& target, const StageResourceRecord& targetRecord,
                             int gate74, std::int32_t message78, std::string& error) {
    error.clear();
    if (!validateSession(source, sourceRecord, error)) {
        error = "source session: " + error;
        return false;
    }
    if (!validateSession(target, targetRecord, error)) {
        error = "target session: " + error;
        return false;
    }
    const auto sourceRole = roleForStage(source.stageId);
    const auto targetRole = roleForStage(target.stageId);
    if (!sourceRole || !targetRole) {
        error = "transition role is unavailable";
        return false;
    }
    if (!canRequestOrdinaryHandoff(*sourceRole)) {
        error = "source stage cannot request an ordinary handoff";
        return false;
    }
    if (!canAcceptCarry(*targetRole)) {
        error = "target stage cannot receive carry";
        return false;
    }
    if (source.difficulty != target.difficulty) {
        error = "carry difficulty must remain unchanged";
        return false;
    }
    const auto plan = planRouteHandoff({source.stageId, source.progress, gate74, message78});
    if (!plan) {
        error = "original route plan rejected the source transition";
        return false;
    }
    if (plan->nextId != target.stageId) {
        error = "target stage id does not match the original route plan";
        return false;
    }
    if (plan->nextProgress != target.progress) {
        error = "target progress does not match the original route plan";
        return false;
    }
    return true;
}

}
