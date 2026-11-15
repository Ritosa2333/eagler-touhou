#pragma once
#include "../game/FrameScheduler.hpp"
#include <array>
#include <functional>
#include <vector>

namespace th17::multiplayer {
// Typed callback graph snapshot following TH15's scheduler restore order.
// The stage holds callback owners alive or rebinds them before restoration.
class FrameState {
    struct Entry {
        FrameCallback* callback;
        FrameCallback::Function run, initialize, cleanup;
        void* owner;
        i32 priority;
        bool enabled;
    };

    std::array<std::vector<Entry>, 2> entries_;
    FrameCallback* continuation_ = nullptr;
    bool closing_ = false, valid_ = false;

public:
    using CallbackResolver = std::function<FrameCallback*(FrameCallback*)>;
    using OwnerResolver = std::function<void*(void*)>;

    bool capture(const FrameScheduler&);
    static void detach(FrameScheduler&) noexcept;
    bool restore(FrameScheduler&, const CallbackResolver& = {}, const OwnerResolver& = {}) const;
};
}
