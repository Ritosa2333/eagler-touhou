#include "Extension1048576.hpp"
namespace th128::bullet {
std::optional<Extension1048576Result> consumeExtension1048576(const ExtensionRaw& record, int extensionIndex,
                                                              std::uint32_t flags, std::string& error) {
    error.clear();
    if (record.type != 0x100000) {
        error = "extension1048576 requires exact type 0x100000";
        return std::nullopt;
    }
    if (record.control != 0 && record.control != 1) {
        error = "extension1048576 requires control 0 or 1";
        return std::nullopt;
    }
    if (extensionIndex < 0 || extensionIndex >= 18) {
        error = "extension1048576 index outside 0..17";
        return std::nullopt;
    }
    if (record.parameterA != 0 && record.parameterA != 1) {
        error = "extension1048576 requires native selector A0 or A1";
        return std::nullopt;
    }
    Extension1048576Result result;
    result.nextExtensionIndex = extensionIndex + 1;
    if (record.parameterA == 0) {
        result.flags = flags & ~0xE0u;
    } else {
        result.flags = (flags & ~0xC0u) | 0x20u;
    }
    return result;
}
}
