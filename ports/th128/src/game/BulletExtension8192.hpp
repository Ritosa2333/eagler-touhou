#pragma once

#include "BulletManager.hpp"

#include <cstdint>
#include <optional>

namespace th128::bullet {

struct Extension8192 {
    std::uint32_t packed = 0;
    int type = 0;
    int color = 0;
    int flags = 0;
    int aimMode = 0;
    bool signFlag = false;
    int resolvedParameterB = 0;
    int extensionIndex = 0;
    float speed = 0;
    float endSpeed = 0;
};

struct Extension8192Dispatch {
    int nextIndex = 0;
    std::uint32_t activeExtensions = 0;
};

std::optional<Extension8192> decodeExtension8192(const ExtensionRaw& record) noexcept;
Extension8192Dispatch advanceExtension8192(int index, std::uint32_t activeExtensions) noexcept;
std::optional<NestedEmissionRequest> makeExtension8192Request(const Extension8192& state,
                                                              const Profile& profile,
                                                              const Birth& parent,
                                                              Position aimPlayer,
                                                              float resolvedNextAngle,
                                                              int count) noexcept;
}
