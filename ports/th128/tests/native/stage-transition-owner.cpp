#include "game/StageTransitionHost.hpp"
#include <iostream>

int main() {
    if (th128::stage::stageTransitionMessageCompletionApplies(true, true, 0)) {
        std::cerr << "pre-Boss entry0 must not complete the stage transition\n";
        return 1;
    }
    if (!th128::stage::stageTransitionMessageCompletionApplies(true, true, 1)) {
        std::cerr << "post-Boss entry1 must complete the stage transition\n";
        return 1;
    }
    if (th128::stage::stageTransitionMessageCompletionApplies(true, false, 1)) {
        std::cerr << "configured but unstarted transition must not complete\n";
        return 1;
    }
    if (th128::stage::stageTransitionMessageCompletionApplies(false, true, 1)) {
        std::cerr << "unconfigured transition must not complete\n";
        return 1;
    }
    std::cout << "pre-Boss MSG0 entry0 is excluded from stage-transition completion\n";
    return 0;
}
