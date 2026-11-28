#pragma once
#include "StageResourceCatalog.hpp"
#include <cstdint>
#include <optional>
#include <string>

namespace th128::stage {

enum class Role { Start, Second, Terminal, Extra };

struct Capability {
    bool acceptCarry = false;
    bool ordinaryHandoff = false;
    bool normalPlayable = false;
};

struct SessionIdentity {
    int stageId = 0;
    std::string eclName;
    int difficulty = 0;
    std::int32_t progress = 0;
};

std::optional<Role> roleForStage(int stageId) noexcept;
Capability capability(Role role) noexcept;
bool canAcceptCarry(Role role) noexcept;
bool canRequestOrdinaryHandoff(Role role) noexcept;
bool validateSession(const SessionIdentity& identity, const StageResourceRecord& record,
                     std::string& error);
bool validateCarryTransition(const SessionIdentity& source, const StageResourceRecord& sourceRecord,
                             const SessionIdentity& target, const StageResourceRecord& targetRecord,
                             int gate74, std::int32_t message78, std::string& error);

}
