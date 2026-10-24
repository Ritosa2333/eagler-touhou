#include "PendingSave.hpp"
#include "NetplayRuntime.hpp"

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

// Use the production room gate and input/correction core. No browser or socket
// is needed to reproduce a run whose native start precedes peer readiness.
struct Peers {
    std::array<th15::multiplayer::NetplayRuntime, 3> peers;
    unsigned count;
    Peers(unsigned players, unsigned mode) : count(players) {
        for (unsigned seat = 0; seat < count; ++seat) {
            th15::multiplayer::SessionSetup setup;
            setup.version = th15::multiplayer::SessionSetup::Version;
            setup.session_id = 0x12345678; setup.seed = 417; setup.difficulty = 1;
            setup.player_count = count; setup.local_player = seat; setup.mode = mode;
            setup.automatic = false; setup.input_delay = setup.requested_delay = mode == 0 ? 0 : 2;
            setup.selections = {0, 1, count == 3 ? 3u : 0u}; setup.build = {1, 2, 3, 4};
            require(peers[seat].Reset(setup), "native setup accepted");
        }
    }
    auto& runtime() { return peers[0]; }
    void ready() {
        for (unsigned to = 0; to < count; ++to) for (unsigned from = 0; from < count; ++from)
            if (to != from) require(peers[to].ApplySession(peers[from].Hello()) == Netplay::SessionPacketResult::Accepted,
                                    "native Hello accepted");
        for (unsigned seat = 0; seat < count; ++seat) {
            require(peers[seat].CanSendReady(), "native Ready admission"); peers[seat].MarkLocalReady();
        }
        for (unsigned to = 0; to < count; ++to) for (unsigned from = 0; from < count; ++from)
            if (to != from) require(peers[to].ApplySession(peers[from].Ready()) == Netplay::SessionPacketResult::Accepted,
                                    "native Ready accepted");
        require(runtime().CanStart(), "native room gate released");
    }
    void remote(unsigned frame, Netplay::FrameInput input) {
        for (unsigned seat = 1; seat < count; ++seat) {
            const auto result = runtime().SubmitRemote(seat, frame, input);
            require(result == Netplay::RemoteInputResult::Accepted || result == Netplay::RemoteInputResult::PredictionCorrect ||
                    result == Netplay::RemoteInputResult::RollbackRequired, "native remote input accepted");
        }
    }
    void capture() {
        auto& r = runtime(); require(r.NeedsCapture(), "physical frontier admits a sample");
        require(r.Capture(r.CaptureFrame(), Netplay::FrameInput(1)), "native physical capture accepted");
    }
    void simulate() {
        auto& r = runtime(); const auto frame = r.NextFrame(); const auto decision = r.Prepare(frame);
        require(decision.canAdvance, "native core admits the frame");
        require(r.MarkSimulated(frame, decision), "native core commits the frame");
    }
};
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

    for (unsigned players : {2u, 3u}) for (unsigned mode : {0u, 2u}) {
        run("untouched run saves before the real Hello/Ready gate opens", [=] {
            Peers fixture(players, mode); auto& r = fixture.runtime(); PendingSave pending; unsigned writes = 0;
            require(!r.CanStart() && !r.InputsReconciledBeforeNext(), "peer gate really is closed");
            const auto token = pending.Request();
            for (unsigned i = 0; i < 3; ++i)
                require(pending.CompleteForRuntime(true, true, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Completed,
                        "empty prefix performs the requested save without peer confirmation");
            require(writes == 1 && pending.Poll(token) == PendingSave::Completed, "same token writes and completes exactly once");
            require(!r.CanStart() && !r.InputsReconciledBeforeNext(), "saving cannot admit gameplay or weaken input reconciliation");
            fixture.ready(); fixture.capture(); fixture.remote(0, Netplay::FrameInput()); fixture.simulate();
            require(r.NextFrame() == 1, "normal room admission can still advance after the save");
            require(pending.Poll(token) == PendingSave::Completed && writes == 1, "simulation does not repeat a completed save");
        });
    }

    run("empty-prefix exception cannot ignore retained game or journal state", [] {
        Peers fixture(2, 0); auto& r = fixture.runtime();
        for (const auto retained : {0u, 1u}) for (const auto journal : {0u, 1u}) if (retained || journal) {
            PendingSave pending; unsigned writes = 0; const auto token = pending.Request();
            require(pending.CompleteForRuntime(true, true, r, retained, journal, [&] { ++writes; return true; }) == PendingSave::Pending,
                    "undo records remain fenced even at frame zero");
            require(writes == 0 && pending.Poll(token) == PendingSave::Pending, "unsafe state performs no write");
            require(pending.CompleteForRuntime(true, true, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Completed,
                    "same request can complete after all retained state is gone");
            require(writes == 1, "released empty prefix writes once");
        }
    });

    run("an owner that already began simulation or failed cannot claim an empty prefix", [] {
        Peers fixture(2, 0); auto& r = fixture.runtime(); PendingSave pending; unsigned writes = 0;
        const auto token = pending.Request();
        require(pending.CompleteForRuntime(true, false, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Pending,
                "sticky owner flag forbids the exception even with zero cursors");
        require(!r.Capture(9, Netplay::FrameInput()) && *r.Error(), "actual runtime failure while waiting for a peer");
        require(pending.CompleteForRuntime(true, true, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Pending,
                "runtime error cannot be bypassed by the empty-prefix exception");
        require(writes == 0 && pending.Cancel(token), "failure stays unwritten and cancellable");
    });

    for (unsigned mode : {0u, 2u}) run("a simulated frame must reconcile and discard undo before saving", [=] {
        Peers fixture(2, mode); auto& r = fixture.runtime(); PendingSave pending; unsigned writes = 0;
        fixture.ready(); require(r.SetWorldReady(true), "prediction world admitted"); fixture.capture(); fixture.simulate();
        require(r.NextFrame() == 1 && !r.InputsReconciledBeforeNext(), "actual predicted frame awaits remote input");
        const auto token = pending.Request();
        require(pending.CompleteForRuntime(true, true, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Pending,
                "a simulated native frontier cannot use the exception even if the owner flag is wrong");
        fixture.remote(0, Netplay::FrameInput());
        require(r.InputsReconciledBeforeNext(), "matching actual input confirms the simulated prefix");
        require(pending.CompleteForRuntime(true, false, r, 1, 1, [&] { ++writes; return true; }) == PendingSave::Pending,
                "confirmation alone cannot bypass retained undo");
        require(pending.CompleteForRuntime(true, false, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Completed,
                "original confirmed save fence completes after undo release");
        require(writes == 1 && pending.Poll(token) == PendingSave::Completed, "confirmed save writes once");
    });

    run("a mismatch and correction to frame zero never reopen the startup exception", [] {
        Peers fixture(2, 0); auto& r = fixture.runtime(); PendingSave pending; unsigned writes = 0;
        fixture.ready(); require(r.SetWorldReady(true), "prediction world admitted"); fixture.capture(); fixture.simulate();
        fixture.remote(0, Netplay::FrameInput(1));
        require(r.RollbackFrame() == 0 && !r.InputsReconciledBeforeNext(), "late mismatch requests a real rollback");
        const auto token = pending.Request();
        require(pending.CompleteForRuntime(true, true, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Pending,
                "rollback request prevents storage before restoration");
        require(r.BeginCorrection(0), "native correction rewinds frame zero");
        require(r.NextFrame() == 0 && r.LastFrame() == Netplay::INVALID_FRAME && r.Correcting(), "rewind looks empty only by its cursors");
        require(pending.CompleteForRuntime(true, true, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Pending,
                "zero cursors cannot admit storage during correction");
        require(!r.EndCorrection(), "historical frame still has to run"); fixture.simulate();
        require(pending.CompleteForRuntime(true, false, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Pending,
                "completed history still needs explicit correction completion");
        require(r.EndCorrection(), "actual correction completes");
        require(pending.CompleteForRuntime(true, false, r, 0, 1, [&] { ++writes; return true; }) == PendingSave::Pending,
                "restored journal still needs release");
        require(pending.CompleteForRuntime(true, false, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Completed,
                "corrected confirmed prefix finally saves");
        require(writes == 1 && pending.Poll(token) == PendingSave::Completed, "correction cannot duplicate token completion");
    });

    run("pure delay and a not-yet-started MP runtime keep their immediate save path", [] {
        for (unsigned mode : {0u, 1u}) {
            Peers fixture(2, mode); auto& r = fixture.runtime(); PendingSave pending; unsigned writes = 0;
            require(!r.CanStart(), "room is still waiting"); const auto token = pending.Request();
            require(pending.CompleteForRuntime(mode == 1, false, r, 0, 0, [&] { ++writes; return true; }) == PendingSave::Completed,
                    "nonrollback owner remains independent of the room gate");
            require(writes == 1 && pending.Poll(token) == PendingSave::Completed, "normal save writes once");
        }
    });

    run("real storage failure on an empty prefix is not converted into success", [] {
        Peers fixture(2, 2); PendingSave pending; unsigned writes = 0; const auto token = pending.Request();
        require(pending.CompleteForRuntime(true, true, fixture.runtime(), 0, 0, [&] { ++writes; return false; }) == PendingSave::Failed,
                "actual settings callback can fail before the first frame");
        require(pending.CompleteForRuntime(true, true, fixture.runtime(), 0, 0, [&] { ++writes; return true; }) == PendingSave::Failed,
                "failed token is terminal");
        require(writes == 1 && pending.Poll(token) == PendingSave::Failed, "failed empty save is attempted once");
    });

    std::printf("PASS TH15 native PendingSave: %u cases, %u checks\n", cases, checks);
}
