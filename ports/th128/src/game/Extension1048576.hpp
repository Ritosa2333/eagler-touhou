#pragma once
#include "BulletRules.hpp"
#include <cstdint>
#include <optional>
#include <string>
namespace th128::bullet {
struct Extension1048576Result {
    std::uint32_t flags = 0;
    int nextExtensionIndex = 0;
};
std::optional<Extension1048576Result> consumeExtension1048576(const ExtensionRaw& record, int extensionIndex,
                                                              std::uint32_t flags, std::string& error);
}
