#include "game/StageControl.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace {
const char* requestStatus(th128::stage::RequestStatus status) {
    return status == th128::stage::RequestStatus::Written ? "written" : "rejected";
}

const char* commitStatus(th128::stage::CommitStatus status) {
    switch (status) {
    case th128::stage::CommitStatus::NoChange: return "no-change";
    case th128::stage::CommitStatus::Applied: return "applied";
    case th128::stage::CommitStatus::Unsupported: return "unsupported";
    case th128::stage::CommitStatus::MissingSession: return "missing-session";
    }
    return "unknown";
}

const char* actionName(th128::stage::ActionKind kind) {
    return kind == th128::stage::ActionKind::RecreateSession ? "recreate" : "none";
}
}

int main() {
    th128::stage::Control control;
    std::string operation;
    while (std::cin >> operation) {
        if (operation == "INIT") {
            std::uint32_t flags = 0;
            assert(std::cin >> flags);
            control.initialize(flags);
            std::cout << "init flags=" << control.flags()
                      << " current=" << control.current()
                      << " pending=" << control.pending()
                      << " session=" << control.sessionFlag()
                      << " gate=" << (control.gateSet() ? 1 : 0) << '\n';
        } else if (operation == "SYNC") {
            std::uint32_t flags = 0;
            assert(std::cin >> flags);
            control.syncFlags(flags);
            std::cout << "sync flags=" << control.flags()
                      << " gate=" << (control.gateSet() ? 1 : 0) << '\n';
        } else if (operation == "REQUEST") {
            std::int32_t state = 0;
            assert(std::cin >> state);
            const auto result = control.request(state);
            std::cout << "request status=" << requestStatus(result.status)
                      << " requested=" << result.requested
                      << " pending=" << result.pending
                      << " collapsed=" << (result.collapsed ? 1 : 0) << '\n';
        } else if (operation == "RELEASE") {
            const bool changed = control.releaseStageGate();
            std::cout << "release changed=" << (changed ? 1 : 0)
                      << " flags=" << control.flags()
                      << " gate=" << (control.gateSet() ? 1 : 0) << '\n';
        } else if (operation == "CONSUME") {
            int present = 0;
            std::int32_t field74 = 0;
            assert(std::cin >> present >> field74);
            const auto result = control.consume({present != 0, field74});
            std::cout << "consume status=" << commitStatus(result.status)
                      << " previous=" << result.previous
                      << " current=" << result.current
                      << " pending=" << result.pending
                      << " session=" << (result.sessionFlag ? 1 : 0)
                      << " action=" << actionName(result.action.kind)
                      << " destroy=" << (result.action.destroyExisting ? 1 : 0)
                      << " arg=" << result.action.sessionArgument << '\n';
        }
    }
}
