#include "BulletExtension800000.hpp"

namespace th128::bullet {

bool extension800000DispatchAllowed(const ExtensionRaw& record, int extensionIndex,
                                    std::uint32_t activeExtensions) noexcept {
    if (record.type != 0x800000 || extensionIndex < 0 || extensionIndex >= 18) return false;
    if (record.control == 0 && (activeExtensions & ~0x100u) != 0) return false;
    return (std::uint32_t(record.type) & activeExtensions) == 0;
}

std::optional<Extension800000Output> consumeExtension800000(const ExtensionRaw& record,
                                                            const Extension800000Context& context,
                                                            std::string& error) {
    error.clear();
    if (record.type != 0x800000) {
        error = "extension800000 requires the exact 0x800000 type";
        return std::nullopt;
    }
    if (!extension800000DispatchAllowed(record, context.extensionIndex, context.activeExtensions)) {
        error = "extension800000 is not dispatchable in this control/active/index context";
        return std::nullopt;
    }
    Extension800000Output output;
    output.snapshot.speed = context.speed;
    output.snapshot.angle = context.angle;
    output.snapshot.position = context.position;
    output.nextExtensionIndex = context.extensionIndex + 1;
    output.consumed = true;
    return output;
}
}
