#pragma once
#include "BulletRules.hpp"
#include <cstdint>
#include <optional>

namespace th128::bullet {
struct Extension4096 {
    std::int32_t counter=0;
    std::int32_t limit=0;
    std::uint32_t flags=0;
};
struct Extension4096Step {
    Extension4096 next;
    Position position;
    bool wrapped=false;
    bool finished=false;
};
std::optional<Extension4096> initializeExtension4096(const ExtensionRaw& record) noexcept;
std::optional<Extension4096Step> stepExtension4096(const Extension4096& state,Position position,float width,float height) noexcept;
}
