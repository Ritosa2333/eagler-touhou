#include "../th10_web/cpp/multiplayer/CallbackWorkTiming.hpp"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <limits>

namespace work = th10::multiplayer::callback_work_timing;
using work::InputRegion;
using work::Phase;
using work::PhaseScope;
using work::State;
using Row = std::array<double, 32>;

namespace {
unsigned checks = 0, failures = 0, cases = 0;
void expect(bool condition, const char* expression, int line) {
    ++checks;
    if (!condition) {
        if (failures < 24) std::printf("FAIL line %d: %s\n", line, expression);
        ++failures;
    }
}
#define EXPECT(condition) expect(bool(condition), #condition, __LINE__)

struct FakeClock {
    std::array<double, 64> values{};
    std::size_t size = 0, position = 0, calls = 0, exhausted = 0;
    void reset(std::initializer_list<double> input) {
        size = position = calls = exhausted = 0;
        for (double value : input) values[size++] = value;
    }
    double read() {
        ++calls;
        if (position < size) return values[position++];
        ++exhausted;
        return std::numeric_limits<double>::quiet_NaN();
    }
};
FakeClock first_clock, second_clock;
double first_now() { return first_clock.read(); }
double second_now() { return second_clock.read(); }

work::Token begin(State& state, double sequence = 7, work::Clock clock = first_now) {
    // owner=1 is deliberately not a dereferenceable Application.
    return state.BeginCallback(true, sequence, 0x34u, 1u, 5u, 59u, clock);
}
Row row(const State& state) {
    Row output{};
    std::memcpy(output.data(), state.Published(), sizeof(output));
    return output;
}
bool same(const Row& expected, const State& state) {
    return std::memcmp(expected.data(), state.Published(), sizeof(expected)) == 0;
}
bool has(const State& state, std::uint32_t flags) {
    return (std::uint32_t(state.Published()[14]) & flags) == flags;
}
void run(const char* name, void (*test)()) {
    const auto before = failures;
    ++cases;
    test();
    std::printf("callback work timing %s: %s\n", name, before == failures ? "PASS" : "FAIL");
}

void initial_and_disabled() {
    first_clock.reset({});
    State state;
    const Row initial = row(state);
    EXPECT(initial.size() == 32);
    EXPECT(initial[0] == 1 && initial[1] == 0 && initial[2] == 0);
    EXPECT(initial[7] == 4294967295. && initial[8] == 4294967295.);
    EXPECT(initial[9] == -1 && initial[14] == work::NotObserved && initial[15] == 0);
    EXPECT(!state.Enabled());
    const auto disabled = begin(state);
    EXPECT(disabled == 0);
    {
        InputRegion input(state);
        PhaseScope phase(state, Phase::ForwardUpdate);
        state.CountElided(false);
        input.Close();
        input.Close();
    }
    EXPECT(!state.Publish(disabled, 0, 0, true, 59));
    EXPECT(first_clock.calls == 0 && same(initial, state));
    EXPECT(state.SetEnabled(true));
    const auto parent_off = state.BeginCallback(false, 8, 0x34, 1, 5, 59, first_now);
    EXPECT(parent_off == 0);
    {
        InputRegion input(state);
        PhaseScope phase(state, Phase::Restore);
    }
    EXPECT(!state.Publish(parent_off, 0, 0, true, 59));
    EXPECT(first_clock.calls == 0 && same(initial, state));
}

void multiple_steps_and_elision() {
    first_clock.reset({10, 10.5, 11, 12.5, 20, 21, 22, 24, 25, 26.25, 27, 27.75});
    State state;
    state.SetEnabled(true);
    const Row previous = row(state);
    const auto token = begin(state);
    {
        PhaseScope outside(state, Phase::Restore);
        state.CountElided(true);
        InputRegion input(state);
        { PhaseScope phase(state, Phase::ForwardCapture); }
        { PhaseScope phase(state, Phase::ForwardUpdate); }
        state.CountElided(false);
        input.Close();
        input.Close();
        { PhaseScope after_close(state, Phase::HistorySemanticDraw); }
    }
    {
        InputRegion input(state);
        { PhaseScope phase(state, Phase::ForwardCapture); }
        { PhaseScope phase(state, Phase::HistoryUpdate); }
        { PhaseScope phase(state, Phase::HistorySemanticDraw); }
        { PhaseScope phase(state, Phase::ConfirmedCommit); }
        state.CountElided(false);
        state.CountElided(true);
    }
    EXPECT(same(previous, state)); // Getter never exposes a partial callback.
    EXPECT(first_clock.calls == 12 && first_clock.exhausted == 0);
    EXPECT(state.Publish(token, 14, 0, true, 61));
    const auto actual = row(state);
    EXPECT(actual[0] == 1 && actual[1] == 1 && actual[2] == 1);
    EXPECT(actual[3] == 7 && actual[4] == 0x34 && actual[5] == 1 && actual[6] == 5);
    EXPECT(actual[7] == 59 && actual[8] == 61 && actual[9] == 0);
    EXPECT(actual[10] == 14 && actual[11] == 7);
    EXPECT(actual[12] == 2 && actual[13] == 1 && actual[14] == 0 && actual[15] == 0);
    EXPECT(actual[16] == 2 && actual[17] == 1.5); // ForwardCapture
    EXPECT(actual[18] == 0 && actual[19] == 0);   // HistoryCapture
    EXPECT(actual[20] == 1 && actual[21] == 1.5); // ForwardUpdate
    EXPECT(actual[22] == 1 && actual[23] == 2);   // HistoryUpdate
    EXPECT(actual[24] == 0 && actual[25] == 0);   // ForwardSemanticDraw
    EXPECT(actual[26] == 1 && actual[27] == 1.25);// HistorySemanticDraw
    EXPECT(actual[28] == 0 && actual[29] == 0);   // Restore
    EXPECT(actual[30] == 1 && actual[31] == .75); // ConfirmedCommit
    EXPECT(!state.Publish(token, 999, 9, false, 999));
    EXPECT(same(actual, state) && first_clock.calls == 12);
}

void all_phase_slots_and_eligibility() {
    first_clock.reset({0, .125, 1, 1.25, 2, 2.375, 3, 3.5, 4, 4.625,
                       5, 5.75, 6, 6.875, 7, 8});
    State state;
    state.SetEnabled(true);
    const auto token = begin(state);
    {
        InputRegion input(state);
        for (unsigned phase = 0; phase < 8; ++phase) {
            PhaseScope skipped(state, Phase(phase), false);
            PhaseScope measured(state, Phase(phase));
        }
    }
    EXPECT(state.Publish(token, 10, 0, true, 60));
    const auto actual = row(state);
    EXPECT(actual[2] == 1 && actual[11] == 5.5);
    for (unsigned phase = 0; phase < 8; ++phase) {
        EXPECT(actual[16 + 2 * phase] == 1);
        EXPECT(actual[17 + 2 * phase] == double(phase + 1) / 8.);
    }
    EXPECT(first_clock.calls == 16 && first_clock.exhausted == 0);
}

int early_return(State& state) {
    InputRegion input(state);
    PhaseScope phase(state, Phase::HistoryCapture);
    return 13;
}
void early_return_and_latched_toggle() {
    first_clock.reset({20, 21.75, 30, 31});
    State state;
    state.SetEnabled(true);
    auto token = begin(state);
    EXPECT(early_return(state) == 13);
    EXPECT(state.Publish(token, 2, 5, true, 60));
    EXPECT(state.Published()[2] == 1 && state.Published()[9] == 5);
    EXPECT(state.Published()[18] == 1 && state.Published()[19] == 1.75);
    EXPECT(state.Published()[11] == .25);
    token = begin(state, 8);
    {
        InputRegion input(state);
        EXPECT(!state.SetEnabled(false));
        PhaseScope phase(state, Phase::ForwardUpdate);
    }
    EXPECT(state.Publish(token, 2, 0, true, 61));
    EXPECT(state.Published()[1] == 1 && state.Published()[2] == 1);
    EXPECT(state.Published()[20] == 1 && state.Published()[21] == 1);
    const auto preserved = row(state);
    EXPECT(begin(state, 9) == 0);
    state.SetEnabled(true); // Enabling without a new Begin cannot open a region.
    { InputRegion input(state); PhaseScope phase(state, Phase::Restore); }
    EXPECT(first_clock.calls == 4 && same(preserved, state));
}

void nested_input() {
    first_clock.reset({10, 12});
    State state;
    state.SetEnabled(true);
    const auto token = begin(state);
    {
        InputRegion outer(state);
        { InputRegion inner(state); inner.Close(); inner.Close(); }
        { PhaseScope phase(state, Phase::ForwardUpdate); }
    }
    EXPECT(state.Publish(token, 3, 0, true, 60));
    EXPECT(state.Published()[2] == 0 && has(state, work::NestedInput));
    EXPECT(state.Published()[20] == 1 && state.Published()[21] == 2);
    EXPECT(first_clock.calls == 2 && first_clock.exhausted == 0);
}

void nested_phase() {
    first_clock.reset({10, 13});
    State state;
    state.SetEnabled(true);
    const auto token = begin(state);
    {
        InputRegion input(state);
        PhaseScope outer(state, Phase::ForwardUpdate);
        { PhaseScope inner(state, Phase::HistorySemanticDraw); }
    }
    EXPECT(state.Publish(token, 4, 0, true, 60));
    EXPECT(state.Published()[2] == 0 && has(state, work::NestedPhase));
    EXPECT(state.Published()[20] == 1 && state.Published()[21] == 3);
    EXPECT(state.Published()[26] == 1 && state.Published()[27] == 0);
    EXPECT(state.Published()[11] == 1 && first_clock.calls == 2);
}

void unclosed_publish_and_stale_destructors() {
    first_clock.reset({10, 20, 23});
    State state;
    state.SetEnabled(true);
    const auto first = begin(state);
    Row complete{};
    {
        InputRegion escaped_input(state);
        PhaseScope escaped_phase(state, Phase::ForwardUpdate);
        EXPECT(state.Publish(first, 4, 0, true, 60));
        EXPECT(state.Published()[2] == 0);
        EXPECT(has(state, work::InputNotClosed | work::PhaseNotClosed));
        EXPECT(state.Published()[20] == 1 && first_clock.calls == 1);
        const auto second = begin(state, 8);
        {
            InputRegion input(state);
            PhaseScope phase(state, Phase::Restore);
        }
        EXPECT(!state.Publish(first, 1000, 8, false, 900));
        EXPECT(state.Publish(second, 4, 0, true, 61));
        EXPECT(state.Published()[2] == 1 && state.Published()[14] == 0);
        EXPECT(state.Published()[28] == 1 && state.Published()[29] == 3);
        complete = row(state);
        // Old destructors now run AFTER a newer complete callback.
    }
    EXPECT(same(complete, state));
    EXPECT(first_clock.calls == 3 && first_clock.exhausted == 0);
}

void begin_before_publish() {
    first_clock.reset({10, 20, 22});
    State state;
    state.SetEnabled(true);
    const auto abandoned = begin(state);
    Row complete{};
    {
        InputRegion old_input(state);
        PhaseScope old_phase(state, Phase::HistoryUpdate);
        const auto next = begin(state, 8);
        {
            InputRegion input(state);
            PhaseScope phase(state, Phase::ForwardUpdate);
        }
        EXPECT(!state.Publish(abandoned, 9, 0, true, 60));
        EXPECT(state.Publish(next, 3, 0, true, 61));
        EXPECT(state.Published()[2] == 0);
        EXPECT(has(state, work::PreviousCallbackOpen | work::InputNotClosed | work::PhaseNotClosed));
        EXPECT(state.Published()[20] == 1 && state.Published()[21] == 2);
        EXPECT(state.Published()[22] == 0 && state.Published()[23] == 0);
        complete = row(state);
    }
    EXPECT(same(complete, state) && first_clock.calls == 3);
}

void phase_escaping_input() {
    first_clock.reset({10, 20, 21});
    State state;
    state.SetEnabled(true);
    const auto token = begin(state);
    {
        InputRegion input(state);
        PhaseScope escaped(state, Phase::ForwardUpdate);
        input.Close(); // Must not read a clock or extend into cadence work.
        EXPECT(first_clock.calls == 1);
        {
            InputRegion next_input(state);
            PhaseScope next_phase(state, Phase::Restore);
        }
    }
    EXPECT(first_clock.calls == 3 && first_clock.exhausted == 0);
    EXPECT(state.Publish(token, 4, 0, true, 60));
    EXPECT(state.Published()[2] == 0 && has(state, work::PhaseNotClosed | work::StaleScope));
    EXPECT(state.Published()[20] == 1 && state.Published()[21] == 0);
    EXPECT(state.Published()[28] == 1 && state.Published()[29] == 1);
}

void owner_change_and_invalid_frame() {
    first_clock.reset({1, 1.75});
    State state;
    state.SetEnabled(true);
    const auto token = state.BeginCallback(true, 2, 9, 1, 4, work::InvalidFrame, first_now);
    { InputRegion input(state); PhaseScope phase(state, Phase::ConfirmedCommit); }
    EXPECT(state.Publish(token, 1, 7, false, work::InvalidFrame));
    EXPECT(state.Published()[2] == 0 && has(state, work::OwnerChanged));
    EXPECT(state.Published()[5] == 1 && state.Published()[7] == 4294967295.);
    EXPECT(state.Published()[8] == 4294967295. && state.Published()[9] == 7);
    EXPECT(state.Published()[30] == 1 && state.Published()[31] == .75);
    EXPECT(first_clock.calls == 2);
}

void nonfinite_and_backwards_clocks() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    const std::array<std::array<double, 2>, 3> bad = {{{nan, 1}, {1, inf}, {inf, inf}}};
    for (const auto& values : bad) {
        first_clock.reset({values[0], values[1]});
        State state;
        state.SetEnabled(true);
        const auto token = begin(state);
        { InputRegion input(state); PhaseScope phase(state, Phase::ForwardCapture); }
        EXPECT(state.Publish(token, 3, 0, true, 60));
        EXPECT(state.Published()[2] == 0 && has(state, work::NonFiniteClock | work::NonFiniteDuration));
        EXPECT(state.Published()[16] == 1 && !std::isfinite(state.Published()[17]));
        EXPECT(!std::isfinite(state.Published()[11]) && first_clock.calls == 2);
    }
    first_clock.reset({2, 1});
    State state;
    state.SetEnabled(true);
    const auto token = begin(state);
    { InputRegion input(state); PhaseScope phase(state, Phase::Restore); }
    EXPECT(state.Publish(token, 3, 0, true, 60));
    EXPECT(state.Published()[2] == 0 && has(state, work::BackwardsClock));
    EXPECT(state.Published()[29] == -1 && state.Published()[11] == 4);
}

void rewind_between_phases() {
    first_clock.reset({10, 11, 9, 10});
    State state;
    state.SetEnabled(true);
    const auto token = begin(state);
    {
        InputRegion input(state);
        { PhaseScope phase(state, Phase::ForwardUpdate); }
        { PhaseScope phase(state, Phase::ForwardSemanticDraw); }
    }
    EXPECT(state.Publish(token, 4, 0, true, 60));
    EXPECT(state.Published()[2] == 0 && has(state, work::BackwardsClock));
    EXPECT(state.Published()[21] == 1 && state.Published()[25] == 1);
    EXPECT(first_clock.calls == 4);
}

void residual_and_invalid_input_time() {
    for (double input : {1., 2. - .5e-6}) {
        first_clock.reset({1, 3});
        State state;
        state.SetEnabled(true);
        const auto token = begin(state);
        { InputRegion region(state); PhaseScope phase(state, Phase::ForwardUpdate); }
        EXPECT(state.Publish(token, input, 0, true, 60));
        EXPECT(state.Published()[11] == input - 2. && state.Published()[11] < 0);
        EXPECT(state.Published()[2] == (input == 1. ? 0. : 1.));
        EXPECT(has(state, work::NegativeResidual) == (input == 1.));
    }
    for (double input : {-1., std::numeric_limits<double>::quiet_NaN(),
                         std::numeric_limits<double>::infinity()}) {
        first_clock.reset({});
        State state;
        state.SetEnabled(true);
        const auto token = begin(state);
        EXPECT(state.Publish(token, input, 0, true, 59));
        EXPECT(state.Published()[2] == 0 && has(state, work::InvalidInputTime));
        EXPECT(input == -1. ? state.Published()[11] == -1. : !std::isfinite(state.Published()[11]));
        EXPECT(first_clock.calls == 0);
    }
}

void missing_clock_invalid_phase_and_empty_callback() {
    first_clock.reset({});
    State state;
    state.SetEnabled(true);
    auto token = begin(state, 1, nullptr);
    { InputRegion input(state); PhaseScope phase(state, Phase::Restore); }
    EXPECT(state.Publish(token, 2, 0, true, 60));
    EXPECT(state.Published()[2] == 0 && has(state, work::MissingClock | work::NonFiniteDuration));
    EXPECT(state.Published()[28] == 1 && std::isnan(state.Published()[29]));
    token = begin(state, 2);
    {
        InputRegion input(state);
        PhaseScope skipped(state, Phase(99), false);
        PhaseScope invalid(state, Phase(99));
    }
    EXPECT(state.Publish(token, 0, 0, true, 60));
    EXPECT(state.Published()[2] == 0 && has(state, work::InvalidPhase));
    token = begin(state, 3);
    EXPECT(state.Publish(token, 0, 3, true, 60));
    EXPECT(state.Published()[2] == 1 && state.Published()[10] == 0 && state.Published()[11] == 0);
    EXPECT(state.Published()[14] == 0 && first_clock.calls == 0);
}

void independent_instances() {
    first_clock.reset({1, 3});
    second_clock.reset({100, 104});
    State first, second;
    first.SetEnabled(true);
    second.SetEnabled(true);
    const auto a = begin(first, 1, first_now);
    const auto b = begin(second, 2, second_now);
    {
        InputRegion input_a(first);
        PhaseScope phase_a(first, Phase::ForwardUpdate);
        {
            InputRegion input_b(second);
            PhaseScope phase_b(second, Phase::HistoryUpdate);
        }
    }
    EXPECT(first.Publish(a, 3, 0, true, 60));
    EXPECT(second.Publish(b, 5, 0, true, 60));
    EXPECT(first.Published()[2] == 1 && second.Published()[2] == 1);
    EXPECT(first.Published()[20] == 1 && first.Published()[21] == 2);
    EXPECT(first.Published()[22] == 0 && second.Published()[20] == 0);
    EXPECT(second.Published()[22] == 1 && second.Published()[23] == 4);
    EXPECT(first_clock.calls == 2 && second_clock.calls == 2);
    EXPECT(!work::state.Enabled()); // Local units never alter the inline service.
}

void disabled_callback_retires_old_tokens() {
    first_clock.reset({10, 20, 21});
    State state;
    state.SetEnabled(true);
    const auto old = begin(state);
    Row complete{};
    {
        InputRegion old_input(state);
        PhaseScope old_phase(state, Phase::ForwardCapture);
        EXPECT(state.BeginCallback(false, 8, 1, 1, 5, 59, first_now) == 0);
        EXPECT(!state.Publish(old, 2, 0, true, 60));
        { InputRegion ignored(state); PhaseScope bypass(state, Phase::Restore); }
        EXPECT(first_clock.calls == 1);
        const auto next = begin(state, 9);
        { InputRegion input(state); PhaseScope phase(state, Phase::ForwardUpdate); }
        EXPECT(state.Publish(next, 2, 0, true, 60));
        EXPECT(state.Published()[2] == 1 && state.Published()[3] == 9);
        EXPECT(state.Published()[16] == 0 && state.Published()[20] == 1);
        complete = row(state);
    }
    EXPECT(same(complete, state) && first_clock.calls == 3);
}
} // namespace

int main() {
    run("initial-disabled-parent-off", initial_and_disabled);
    run("multiple-steps-elision-published-row", multiple_steps_and_elision);
    run("eight-ABI-slots-eligibility", all_phase_slots_and_eligibility);
    run("early-return-latched-toggle", early_return_and_latched_toggle);
    run("nested-input-negative-control", nested_input);
    run("nested-phase-negative-control", nested_phase);
    run("unclosed-publish-stale-destructors", unclosed_publish_and_stale_destructors);
    run("new-callback-before-publish", begin_before_publish);
    run("phase-escapes-input", phase_escaping_input);
    run("owner-change-invalid-frame", owner_change_and_invalid_frame);
    run("nonfinite-backwards-clock", nonfinite_and_backwards_clocks);
    run("clock-rewind-between-phases", rewind_between_phases);
    run("raw-residual-invalid-input-time", residual_and_invalid_input_time);
    run("missing-clock-invalid-phase-empty-callback", missing_clock_invalid_phase_and_empty_callback);
    run("independent-state-instances", independent_instances);
    run("disabled-callback-retires-tokens", disabled_callback_retires_old_tokens);
    std::printf("callback work timing: %s %u cases / %u checks / %u failures\n",
                failures ? "FAIL" : "PASS", cases, checks, failures);
    return failures ? 1 : 0;
}
