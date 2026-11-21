#include "BulletExtension256.hpp"
#include <cmath>

namespace th128::bullet {

std::optional<Extension256> initializeExtension256(const ExtensionRaw& record) {
    if (record.type != 256 || record.parameterB != 0
        || record.parameterA < 0 || record.parameterA > 65535) return std::nullopt;
    Extension256 result;
    result.previous = record.parameterA - 1;
    result.remaining = record.parameterA;
    result.accumulator = static_cast<float>(record.parameterA);
    result.flags = 1;
    result.mode = record.parameterB;
    return result;
}

std::optional<Extension256Step> stepExtension256(const Extension256& state) {
    if (state.mode != 0 || !std::isfinite(state.accumulator)
        || state.previous < -1 || state.previous > 65535
        || state.remaining < 0 || state.remaining > 65535
        || state.accumulator < 0.0f || state.accumulator > 65535.0f
        || std::floor(state.accumulator) != state.accumulator
        || state.accumulator != static_cast<float>(state.remaining)) return std::nullopt;
    const float nextAccumulator = std::floor(state.accumulator - 1.0f);
    if (!std::isfinite(nextAccumulator)) return std::nullopt;
    Extension256Step result;
    result.next = state;
    result.next.previous = state.remaining;
    result.next.accumulator = nextAccumulator;
    result.next.remaining = static_cast<int>(nextAccumulator);
    result.finished = result.next.remaining <= 0;
    return result;
}
}
