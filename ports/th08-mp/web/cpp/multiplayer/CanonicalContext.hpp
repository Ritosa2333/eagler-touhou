#pragma once
#include "../game/EclVm.hpp"
#include <cstddef>
#include <cstring>

namespace th08::multiplayer {
// These bytes are alignment gaps, not ECL state. Field writes and native C++
// copies can leave them different after an otherwise identical frame.
inline void NormalizeContextPadding(EclContext& context) noexcept {
    auto* bytes=reinterpret_cast<u8*>(&context);
    const auto gap=[&](std::size_t first,std::size_t last){std::memset(bytes+first,0,last-first);};
    gap(offsetof(EclContext,branched)+sizeof(context.branched),offsetof(EclContext,subroutine));
    gap(offsetof(EclContext,returned)+sizeof(context.returned),offsetof(EclContext,native_callback));
}
}
