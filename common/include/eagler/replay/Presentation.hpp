#pragma once

namespace eagler::replay {
// Replay acceleration may coalesce window swaps. Native Calc, authored Draw,
// offscreen rendering and screenshot/capture requests must still run normally.
inline bool& deferred_presentation() {
    static bool deferred = false;
    return deferred;
}

class DeferPresentation {
    bool previous;
public:
    explicit DeferPresentation(bool defer) : previous(deferred_presentation()) {
        deferred_presentation() = defer;
    }
    ~DeferPresentation() { deferred_presentation() = previous; }
    DeferPresentation(const DeferPresentation&) = delete;
    DeferPresentation& operator=(const DeferPresentation&) = delete;
};
}
