#pragma once

namespace eagler::cooperative {
constexpr unsigned TransferTaps = 5;
constexpr unsigned TransferTapWindow = 24;
// One byte holds both the tap count and the remaining logical-frame window.
// Titles keep this value in their existing journalled transfer counter.
constexpr unsigned TapCount(unsigned state) noexcept { return state / 32; }
template<class Counter>
bool AdvanceLifeTransfer(Counter& state, bool pressed, bool eligible) noexcept {
    if (!eligible) { state = 0; return false; }
    unsigned taps = TapCount(state), remaining = unsigned(state) % 32;
    if (taps) { if (!remaining) taps = 0; else --remaining; }
    if (pressed) {
        if (++taps == TransferTaps) { state = 0; return true; }
        remaining = TransferTapWindow;
    }
    state = Counter(taps * 32 + remaining);
    return false;
}
template<class Eligible, class Power, class Step>
int LowestPower(unsigned count, Eligible eligible, Power power, Step step) noexcept {
    int best = -1;
    for (unsigned seat = 0; seat < count; ++seat) {
        if (!eligible(seat)) continue;
        if (best < 0 || static_cast<long long>(power(seat)) * step(unsigned(best)) <
                static_cast<long long>(power(unsigned(best))) * step(seat)) best = int(seat);
    }
    return best;
}
constexpr int DeathbombFrames(int native) noexcept { return native * 2; }
} // namespace eagler::cooperative
