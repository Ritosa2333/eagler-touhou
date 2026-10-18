#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace th10::multiplayer::callback_work_timing {

using Token = std::uint64_t;
using Clock = double (*)();

enum class Phase : std::uint32_t {
    ForwardCapture, HistoryCapture, ForwardUpdate, HistoryUpdate,
    ForwardSemanticDraw, HistorySemanticDraw, Restore, ConfirmedCommit, Count
};

enum Field : std::size_t {
    Schema, Enabled, Valid, Sequence, Epoch, Owner, Generation, FrameBefore,
    FrameAfter, ExitReason, NativeInputAndStepMs, ResidualMs, ForwardElidedCalls,
    HistoryElidedCalls, DiagnosticFlags, Reserved, PhaseValues, WordCount = 32
};

enum DiagnosticFlag : std::uint32_t {
    NotObserved = 1u << 0,
    MissingClock = 1u << 1,
    NonFiniteClock = 1u << 2,
    BackwardsClock = 1u << 3,
    NestedInput = 1u << 4,
    NestedPhase = 1u << 5,
    InputNotClosed = 1u << 6,
    PhaseNotClosed = 1u << 7,
    OwnerChanged = 1u << 8,
    NegativeResidual = 1u << 9,
    InvalidInputTime = 1u << 10,
    PreviousCallbackOpen = 1u << 11,
    InvalidPhase = 1u << 12,
    StaleScope = 1u << 13,
    NonFiniteDuration = 1u << 14
};

inline constexpr std::uint32_t InvalidFrame = 0xffffffffu;
inline constexpr double ResidualToleranceMs = 1.e-6;
inline constexpr std::size_t CallsIndex(Phase phase) noexcept {
    return PhaseValues + std::size_t(phase) * 2;
}
inline constexpr std::size_t MillisecondsIndex(Phase phase) noexcept {
    return CallsIndex(phase) + 1;
}
static_assert(PhaseValues == 16 && std::size_t(Phase::Count) == 8 &&
              MillisecondsIndex(Phase::ConfirmedCommit) + 1 == WordCount);

class InputRegion;
class PhaseScope;

// Pure diagnostic values, outside every rollback/hash owner. The clock is only
// read by accepted PhaseScopes while an InputRegion is open. There is no heap,
// no game pointer dereference, and no clock call at Begin/Publish/InputRegion.
// Each row is callback-local. Published() never exposes the row under assembly.
// Disabling is latched by the NEXT BeginCallback and leaves the last row intact.
class State {
public:
    State() noexcept {
        published_[Schema] = 1;
        published_[FrameBefore] = published_[FrameAfter] = double(InvalidFrame);
        published_[ExitReason] = -1;
        published_[DiagnosticFlags] = NotObserved;
    }
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    bool SetEnabled(bool enabled) noexcept { enabled_ = enabled; return enabled_; }
    bool Enabled() const noexcept { return enabled_; }
    const double* Published() const noexcept { return published_.data(); }

    Token BeginCallback(bool parentEnabled, double sequence, std::uintptr_t epoch,
                        std::uintptr_t owner, std::uint32_t generation,
                        std::uint32_t frameBefore, Clock clock) noexcept {
        std::uint32_t abandoned = callback_active_ ? std::uint32_t(PreviousCallbackOpen) : 0u;
        if (callback_active_ && input_open_) abandoned |= InputNotClosed;
        if (callback_active_ && phase_open_) abandoned |= PhaseNotClosed;
        // Also invalidate outstanding scopes across a disabled callback.
        if (++next_callback_ == 0) ++next_callback_;
        callback_ = next_callback_;
        callback_active_ = false;
        input_open_ = phase_open_ = false;
        input_serial_ = phase_serial_ = next_scope_ = 0;
        clock_ = nullptr;
        if (!enabled_ || !parentEnabled) return 0;

        work_.fill(0);
        work_[Schema] = work_[Field::Enabled] = 1;
        work_[Sequence] = sequence;
        work_[Epoch] = double(epoch);
        work_[Owner] = double(owner);
        work_[Generation] = double(generation);
        work_[FrameBefore] = double(frameBefore);
        work_[FrameAfter] = double(InvalidFrame);
        work_[ExitReason] = -1;
        flags_ = abandoned;
        clock_ = clock;
        have_clock_ = false;
        if (!clock_) flags_ |= MissingClock;
        callback_active_ = true;
        return callback_;
    }

    // True means a row was published, not that its measurement is valid.
    // A rejected/stale token is explicit false and cannot overwrite a newer row.
    bool Publish(Token token, double inputStepMs, std::uint32_t exitReason,
                 bool ownerValid, std::uint32_t frameAfter) noexcept {
        if (!token || !callback_active_ || token != callback_) return false;
        if (input_open_) flags_ |= InputNotClosed;
        if (phase_open_) flags_ |= PhaseNotClosed;
        if (!ownerValid) flags_ |= OwnerChanged;
        work_[FrameAfter] = double(frameAfter);
        work_[ExitReason] = double(exitReason);
        work_[NativeInputAndStepMs] = inputStepMs;
        if (!std::isfinite(inputStepMs) || inputStepMs < 0.) flags_ |= InvalidInputTime;
        double measured = 0.;
        for (std::size_t index = PhaseValues + 1; index < WordCount; index += 2)
            measured += work_[index];
        work_[ResidualMs] = inputStepMs - measured; // Keep negative/nonfinite raw values.
        if (!std::isfinite(measured) || !std::isfinite(work_[ResidualMs]))
            flags_ |= NonFiniteDuration;
        if (work_[ResidualMs] < -ResidualToleranceMs) flags_ |= NegativeResidual;
        work_[DiagnosticFlags] = double(flags_);
        work_[Valid] = flags_ == 0 ? 1. : 0.;
        published_ = work_;
        callback_active_ = false;
        input_open_ = phase_open_ = false;
        input_serial_ = phase_serial_ = 0;
        clock_ = nullptr;
        return true;
    }

    void CountElided(bool history) noexcept {
        if (callback_active_ && input_open_)
            ++work_[history ? HistoryElidedCalls : ForwardElidedCalls];
    }

private:
    friend class InputRegion;
    friend class PhaseScope;
    struct ScopeToken { Token callback = 0; std::uint64_t serial = 0; };

    ScopeToken EnterInput() noexcept {
        if (!callback_active_) return {};
        if (input_open_) { flags_ |= NestedInput; return {}; }
        input_open_ = true;
        input_serial_ = ++next_scope_;
        return {callback_, input_serial_};
    }

    void LeaveInput(ScopeToken token) noexcept {
        if (!token.callback || token.callback != callback_ || !callback_active_) return;
        if (!input_open_ || token.serial != input_serial_) { flags_ |= StaleScope; return; }
        if (phase_open_) {
            // InputRegion has no clock. An escaped phase is invalid, never
            // completed by measuring cadence/Flush work outside InputAndStep.
            flags_ |= PhaseNotClosed;
            phase_open_ = false;
            phase_serial_ = 0;
        }
        input_open_ = false;
        input_serial_ = 0;
    }

    double ReadClock() noexcept {
        if (!clock_) {
            flags_ |= MissingClock;
            return std::numeric_limits<double>::quiet_NaN();
        }
        const double value = clock_();
        if (!std::isfinite(value)) flags_ |= NonFiniteClock;
        else {
            if (have_clock_ && value < last_clock_) flags_ |= BackwardsClock;
            last_clock_ = value;
            have_clock_ = true;
        }
        return value;
    }

    ScopeToken EnterPhase(Phase phase) noexcept {
        // Draw wrappers also run outside the Host InputAndStep region.
        // Those ordinary non-measured calls must remain a pure bypass.
        if (!callback_active_ || !input_open_) return {};
        if (std::size_t(phase) >= std::size_t(Phase::Count)) {
            flags_ |= InvalidPhase;
            return {};
        }
        ++work_[CallsIndex(phase)]; // Attempts, including a rejected nested phase.
        if (phase_open_) { flags_ |= NestedPhase; return {}; }
        phase_open_ = true;
        phase_serial_ = ++next_scope_;
        phase_ = phase;
        phase_start_ = ReadClock();
        return {callback_, phase_serial_};
    }

    void LeavePhase(ScopeToken token) noexcept {
        if (!token.callback || token.callback != callback_ || !callback_active_) return;
        if (!input_open_ || !phase_open_ || token.serial != phase_serial_) {
            flags_ |= StaleScope;
            return;
        }
        const double elapsed = ReadClock() - phase_start_;
        auto& total = work_[MillisecondsIndex(phase_)];
        total += elapsed;
        if (!std::isfinite(elapsed) || !std::isfinite(total)) flags_ |= NonFiniteDuration;
        phase_open_ = false;
        phase_serial_ = 0;
    }

    bool enabled_ = false;
    bool callback_active_ = false, input_open_ = false, phase_open_ = false;
    bool have_clock_ = false;
    Token callback_ = 0, next_callback_ = 0;
    std::uint64_t next_scope_ = 0, input_serial_ = 0, phase_serial_ = 0;
    Clock clock_ = nullptr;
    double last_clock_ = 0., phase_start_ = 0.;
    Phase phase_ = Phase::ForwardCapture;
    std::uint32_t flags_ = 0;
    std::array<double, WordCount> work_{}, published_{};
};

inline State state;

class InputRegion {
public:
    explicit InputRegion(State& target = state) noexcept
        : target_(target), token_(target.EnterInput()) {}
    InputRegion(const InputRegion&) = delete;
    InputRegion& operator=(const InputRegion&) = delete;
    ~InputRegion() { Close(); }
    void Close() noexcept {
        if (!token_.callback) return;
        target_.LeaveInput(token_);
        token_ = {};
    }
private:
    State& target_;
    State::ScopeToken token_;
};

class PhaseScope {
public:
    PhaseScope(State& target, Phase phase, bool eligible = true) noexcept
        : target_(target), token_(eligible ? target.EnterPhase(phase) : State::ScopeToken{}) {}
    PhaseScope(const PhaseScope&) = delete;
    PhaseScope& operator=(const PhaseScope&) = delete;
    ~PhaseScope() { if (token_.callback) target_.LeavePhase(token_); }
private:
    State& target_;
    State::ScopeToken token_;
};

} // namespace th10::multiplayer::callback_work_timing
