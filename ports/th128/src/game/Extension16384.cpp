#include "Extension16384.hpp"

namespace th128::bullet {

std::optional<Extension16384> initializeExtension16384(const ExtensionRaw& record, std::int32_t index) {
    if (record.type != 16384 || (record.control != 0 && record.control != 1)
        || record.parameterA != 1 || index < 0 || index > 17) return std::nullopt;
    Extension16384 state;
    state.packedA = std::uint32_t(record.parameterA);
    state.control = record.control;
    state.sourceIndex = index;
    state.nextExtensionIndex = index + 1;
    return state;
}

std::optional<Extension16384Step> consumeExtension16384(const Extension16384& state) {
    if (state.consumed || state.sourceIndex < 0 || state.nextExtensionIndex != state.sourceIndex + 1
        || state.packedA != 1 || (state.control != 0 && state.control != 1)) return std::nullopt;
    Extension16384Step result;
    result.next = state;
    result.next.consumed = true;
    result.activeChanged = false;
    result.tick = false;
    result.retired = false;
    result.parentChanged = false;
    result.hostCalls = 0;
    result.rngCalls = 0;
    return result;
}
}
