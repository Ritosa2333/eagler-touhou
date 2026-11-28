#include "game/MessageTimeControl.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace {
const char* reasonName(th128::message::Msg22Reason reason) {
    switch (reason) {
    case th128::message::Msg22Reason::None: return "none";
    case th128::message::Msg22Reason::InvalidStage: return "invalid-stage";
    case th128::message::Msg22Reason::InvalidClock: return "invalid-clock";
    case th128::message::Msg22Reason::ConversionOverflow: return "conversion-overflow";
    case th128::message::Msg22Reason::DurationOverflow: return "duration-overflow";
    }
    return "unknown";
}
}

int main() {
    std::string operation;
    while (std::cin >> operation) {
        if (operation == "PLAN") {
            int stage = 0;
            std::string clockText;
            assert(std::cin >> stage >> clockText);
            const auto plan = th128::message::planMsg22({stage, std::stof(clockText)});
            if (plan.status != th128::message::Msg22Status::BgmFadeOut) {
                std::cout << "unsupported " << reasonName(plan.reason) << '\n';
                continue;
            }
            const auto& request = plan.request;
            std::cout << "fade op=" << th128::message::BgmFadeOutRequest::QueueOperation
                      << " base=" << request.requestedSeconds
                      << " scaled=" << request.scaledSeconds
                      << " frames=" << request.durationFrames
                      << " seconds=" << request.durationSeconds
                      << " start=" << request.volumeStartRaw
                      << " end=" << request.volumeEndRaw
                      << " mode=" << request.fadeMode
                      << " immediate=" << (request.immediate ? 1 : 0) << '\n';
        } else if (operation == "STEP") {
            std::int32_t remaining = 0, duration = 0;
            assert(std::cin >> remaining >> duration);
            const auto step = th128::message::fadeOutStep(remaining, duration);
            std::cout << "step active=" << (step.active ? 1 : 0)
                      << " remaining=" << step.remaining
                      << " volume=" << step.volumeRaw << '\n';
        }
    }
}
