#include "FrameState.hpp"
#include <algorithm>

namespace th17::multiplayer {
bool FrameState::capture(const FrameScheduler& scheduler) {
    valid_ = false;
    for (auto& pass : entries_) pass.clear();
    const std::array<FrameCallback*, 2> heads{scheduler.updates, scheduler.draws};
    bool has_continuation = !scheduler.continuation;
    for (unsigned pass = 0; pass < heads.size(); ++pass) {
        FrameCallback* previous = nullptr;
        for (auto* callback = heads[pass]; callback; callback = callback->next) {
            if (callback->scheduler != &scheduler || callback->previous != previous ||
                callback->pass != FramePass(pass)) return false;
            entries_[pass].push_back({callback, callback->run, callback->initialize,
                callback->cleanup, callback->owner, callback->priority, callback->enabled});
            has_continuation |= callback == scheduler.continuation;
            previous = callback;
        }
    }
    continuation_ = scheduler.continuation;
    closing_ = scheduler.closing;
    return valid_ = has_continuation;
}

void FrameState::detach(FrameScheduler& scheduler) noexcept {
    for (auto* head : {scheduler.updates, scheduler.draws}) {
        for (auto* callback = head; callback;) {
            auto* next = callback->next;
            callback->previous = callback->next = nullptr;
            callback->scheduler = nullptr;
            callback = next;
        }
    }
    scheduler.updates = scheduler.draws = scheduler.continuation = nullptr;
}

bool FrameState::restore(FrameScheduler& scheduler, const CallbackResolver& callbacks,
                         const OwnerResolver& owners) const {
    if (!valid_) return false;
    struct Target {FrameCallback* callback; void* owner;};
    std::vector<Target> targets;
    targets.reserve(entries_[0].size() + entries_[1].size());
    for (const auto& pass : entries_) {
        for (const auto& entry : pass) {
            auto* target = callbacks ? callbacks(entry.callback) : entry.callback;
            auto* owner = entry.owner && owners ? owners(entry.owner) : entry.owner;
            if (!target || (entry.owner && !owner) ||
                (target->scheduler && target->scheduler != &scheduler) ||
                (callbacks && std::any_of(targets.begin(), targets.end(),
                    [&](const Target& value) { return value.callback == target; }))) return false;
            targets.push_back({target, owner});
        }
    }

    // Rebuild links without executing init, cleanup or draw callbacks.
    detach(scheduler);
    unsigned index = 0;
    for (unsigned pass = 0; pass < entries_.size(); ++pass) {
        auto** head = pass == 0 ? &scheduler.updates : &scheduler.draws;
        FrameCallback* previous = nullptr;
        for (const auto& entry : entries_[pass]) {
            const auto [target, owner] = targets[index++];
            target->run = entry.run;
            target->initialize = entry.initialize;
            target->cleanup = entry.cleanup;
            target->owner = owner;
            target->enabled = entry.enabled;
            target->priority = entry.priority;
            target->pass = FramePass(pass);
            target->scheduler = &scheduler;
            target->previous = previous;
            target->next = nullptr;
            if (previous) previous->next = target;
            else *head = target;
            if (entry.callback == continuation_) scheduler.continuation = target;
            previous = target;
        }
    }
    scheduler.closing = closing_;
    return true;
}
}
