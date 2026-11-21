#pragma once

#include "BulletRules.hpp"

#include <cstdint>
#include <optional>
#include <string>

namespace th128::bullet {

struct Extension800000Snapshot {
    float speed = 0;
    float angle = 0;
    Position position;
};

struct Extension800000Context {
    int extensionIndex = 0;
    std::uint32_t activeExtensions = 0;
    float speed = 0;
    float angle = 0;
    Position position;
};

struct Extension800000Output {
    Extension800000Snapshot snapshot;
    int nextExtensionIndex = 0;
    bool consumed = false;
};

bool extension800000DispatchAllowed(const ExtensionRaw& record, int extensionIndex,
                                    std::uint32_t activeExtensions) noexcept;
std::optional<Extension800000Output> consumeExtension800000(const ExtensionRaw& record,
                                                            const Extension800000Context& context,
                                                            std::string& error);
}
