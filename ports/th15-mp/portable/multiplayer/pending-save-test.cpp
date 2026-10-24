#include "PendingSave.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

using th15::multiplayer::PendingSave;

namespace {
const char* current_test = "initialization";
unsigned cases = 0;
unsigned checks = 0;

void require(bool condition, const char* message) {
    ++checks;
    if (!condition) {
        std::fprintf(stderr, "FAIL %s: %s\n", current_test, message);
        std::exit(1);
    }
}

template<class Test>
void run(const char* name, Test test) {
    current_test = name;
    test();
    ++cases;
    std::printf("PASS %s\n", name);
}

template<class Save>
PendingSave::Status complete_safe(PendingSave& pending, Save save) {
    return pending.Complete(true, false, true, 0, 0, save);
}
}

int main() {
    run("idle polling cannot create or write a save", [] {
        PendingSave pending;
        unsigned writes = 0;
        require(!pending.Waiting(), "new owner is not waiting");
        require(pending.Poll(0) == PendingSave::Stale, "zero is never a valid token");
        require(pending.Poll(1) == PendingSave::Stale, "unissued token is stale");
        require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Stale,
                "completion without a request stays idle");
        require(writes == 0, "idle completion must not invoke storage");
    });

    run("concurrent requests merge and completion releases the simulation", [] {
        PendingSave pending;
        unsigned writes = 0;
        const auto token = pending.Request();
        require(token != 0, "request returns a usable token");
        require(pending.Waiting(), "request enters pending state");
        for (unsigned i = 0; i < 8; ++i)
            require(pending.Request() == token, "overlapping requests share one token");
        require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Completed,
                "safe frontier completes the pending request");
        require(writes == 1, "merged requests invoke storage once");
        require(!pending.Waiting(), "native completion releases the simulation before Host polling");
        for (unsigned i = 0; i < 8; ++i) {
            require(pending.Poll(token) == PendingSave::Completed, "late polling sees completion");
            require(!pending.Waiting(), "late polling must not hold or requeue the simulation");
            require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Completed,
                    "another native callback preserves completed status");
        }
        require(writes == 1, "completed request never repeats its storage callback");
        require(!pending.Cancel(token), "completed requests cannot be cancelled");
    });

    struct Unsafe {
        const char* name;
        bool correcting;
        bool reconciled;
        std::uint32_t retained;
        std::uint32_t journal_frames;
    };
    const std::array<Unsafe, 4> unsafe{{
        {"active correction defers score storage", true, true, 0, 0},
        {"unreconciled inputs defer score storage", false, false, 0, 0},
        {"retained predicted state defers score storage", false, true, 1, 0},
        {"retained journal frames defer score storage", false, true, 0, 1},
    }};
    for (const auto& gate : unsafe) run(gate.name, [&] {
        PendingSave pending;
        unsigned writes = 0;
        const auto token = pending.Request();
        for (unsigned i = 0; i < 3; ++i) {
            require(pending.Complete(true, gate.correcting, gate.reconciled,
                                     gate.retained, gate.journal_frames,
                                     [&] { ++writes; return true; }) == PendingSave::Pending,
                    "unsafe frontier remains pending");
            require(pending.Poll(token) == PendingSave::Pending, "Host sees pending rather than failure");
            require(pending.Waiting(), "unsafe request remains visible to the frame owner");
            require(pending.Request() == token, "another request coalesces during the wait");
            require(writes == 0, "unsafe frontier must not call storage");
        }
        require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Completed,
                "same request completes once the frontier becomes safe");
        require(writes == 1 && !pending.Waiting(), "storage runs exactly once and releases the owner");
        require(pending.Poll(token) == PendingSave::Completed, "original token observes its result");
    });

    run("real storage failure is terminal until an explicit new request", [] {
        PendingSave pending;
        unsigned writes = 0;
        const auto failed = pending.Request();
        require(complete_safe(pending, [&] { ++writes; return false; }) == PendingSave::Failed,
                "false storage result is a real failure");
        require(pending.Poll(failed) == PendingSave::Failed && !pending.Waiting(),
                "failed storage does not keep the simulation waiting");
        require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Failed,
                "later callbacks do not silently retry a failed request");
        require(writes == 1, "failed request attempted storage once");
        const auto retry = pending.Request();
        require(retry != 0 && retry != failed, "explicit retry receives a new token");
        require(pending.Poll(failed) == PendingSave::Stale, "old caller cannot claim the new request");
        require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Completed,
                "explicit retry can complete");
        require(writes == 2, "only the explicit retry performs a second write");
    });

    run("cancellation only affects the matching pending request", [] {
        PendingSave pending;
        unsigned writes = 0;
        const auto token = pending.Request();
        require(!pending.Cancel(0) && !pending.Cancel(token + 1), "unrelated tokens cannot cancel");
        require(pending.Waiting() && pending.Poll(token) == PendingSave::Pending,
                "unrelated cancellation leaves the request pending");
        require(pending.Cancel(token), "matching pending token is cancellable");
        require(!pending.Waiting() && pending.Poll(token) == PendingSave::Failed,
                "cancelled token reports failure without holding simulation");
        require(!pending.Cancel(token), "cancellation is terminal");
        require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Failed,
                "cancelled request cannot execute later");
        require(writes == 0, "cancellation performs no storage");
        const auto next = pending.Request();
        require(next != token && !pending.Cancel(token), "old cancellation cannot affect a new request");
        require(pending.Poll(next) == PendingSave::Pending, "new request survives stale cancellation");
    });

    run("owner failure does not revoke an already completed save", [] {
        PendingSave pending;
        pending.Fail();
        require(pending.Poll(1) == PendingSave::Stale && !pending.Waiting(), "idle failure stays idle");
        const auto token = pending.Request();
        pending.Fail();
        require(pending.Poll(token) == PendingSave::Failed && !pending.Waiting(),
                "owner failure terminates a pending request");
        const auto next = pending.Request();
        require(complete_safe(pending, [] { return true; }) == PendingSave::Completed,
                "new request can finish after owner failure");
        pending.Fail();
        require(pending.Poll(next) == PendingSave::Completed, "later owner failure does not revoke completion");
    });

    run("world reset invalidates old tokens without recycling their identity", [] {
        PendingSave pending;
        std::uint32_t previous = 0;
        unsigned writes = 0;
        for (unsigned world = 0; world < 8; ++world) {
            const auto token = pending.Request();
            require(token > previous, "new world requests use a different nonzero token");
            if (previous) require(pending.Poll(previous) == PendingSave::Stale, "old world token stays stale");
            if (world % 2)
                require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Completed,
                        "completed tokens are also invalidated at a world boundary");
            pending.Clear();
            require(!pending.Waiting() && pending.Poll(token) == PendingSave::Stale,
                    "world clear removes pending or completed ownership");
            require(!pending.Cancel(token), "old world cannot cancel after clear");
            const auto before = writes;
            require(complete_safe(pending, [&] { ++writes; return true; }) == PendingSave::Stale,
                    "world clear cannot leave a delayed storage callback");
            require(writes == before, "cleared request performs no later storage");
            previous = token;
        }
    });

    run("nonrollback saves complete without rollback frontier requirements", [] {
        PendingSave pending;
        unsigned writes = 0;
        const auto token = pending.Request();
        require(pending.Complete(false, true, false, 12, 32,
                                 [&] { ++writes; return true; }) == PendingSave::Completed,
                "ordinary save does not wait on unused rollback conditions");
        require(writes == 1 && !pending.Waiting() && pending.Poll(token) == PendingSave::Completed,
                "ordinary save completes exactly once");
    });

    run("late Host polling cannot replace another request", [] {
        PendingSave pending;
        const auto old = pending.Request();
        require(complete_safe(pending, [] { return true; }) == PendingSave::Completed,
                "first request completes");
        const auto active = pending.Request();
        require(active != old, "next request has independent ownership");
        for (unsigned i = 0; i < 8; ++i) {
            require(pending.Poll(old) == PendingSave::Stale, "late caller cannot observe another result");
            require(pending.Poll(active) == PendingSave::Pending && pending.Waiting(),
                    "late caller does not requeue or complete the active request");
        }
    });

    std::printf("PASS TH15 native PendingSave: %u cases, %u checks\n", cases, checks);
}
