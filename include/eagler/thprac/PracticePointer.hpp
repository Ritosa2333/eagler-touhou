// Gesture ownership is decided only on down. Merely crossing a trainer
// window must not cancel a game-owned drag or press a trainer widget.
#pragma once
#include <array>

namespace eagler::thprac {
class PracticePointer {
    struct Slot { int id = 0; bool active = false; };
    std::array<Slot, 32> ui_{};
public:
    void reset() { for (auto& slot : ui_) slot.active = false; }
    bool route(unsigned type, int id, bool hit) {
        for (auto& slot : ui_) if (slot.active && slot.id == id) {
            if (type == 2) slot.active = false;
            return true;
        }
        if (type != 0 || !hit) return false;
        for (auto& slot : ui_) if (!slot.active) {
            slot = {id, true}; return true;
        }
        return false;
    }
};
} // namespace eagler::thprac
