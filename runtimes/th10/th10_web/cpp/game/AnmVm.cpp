#include "AnmVm.hpp"
namespace th10 {
namespace {
template<class T> void field(AnmVm& vm, std::size_t offset, T value) noexcept {
    std::memcpy(reinterpret_cast<u8*>(&vm) + offset, &value, sizeof(value));
}
}
// 0x402050. Clearing interpolation flags is subsumed by clearing the VM.
void AnmVm::clear() noexcept { std::memset(this, 0, sizeof(*this)); sprite_index = -1; }
// 0x401de0. Preserve the owner tag and position when restarting an animation.
void AnmVm::initialize() noexcept {
    const auto saved_position = position;
    const auto saved_owner = owner_tag;
    std::memset(this, 0, sizeof(*this));
    position = saved_position;
    owner_tag = saved_owner;
    color = 0xffffffff;
    scale = {1.0f, 1.0f};
    sprite_matrix.identity();
    field(*this, 0x35c, u16{7});
    script_timer.reset();
    registry_node.initialize(this);
    child_node.initialize(this);
}
// 0x4020b0. Flags belong to eight different interpolation records.
void AnmVm::stop_interpolators() noexcept {
    script_timer_flags &= ~1u;
    position_interpolation.flags &= ~1u;
    color_interpolation.flags &= ~1u;
    alpha_interpolation.flags &= ~1u;
    rotation_interpolation.flags &= ~1u;
    scale_interpolation.flags &= ~1u;
    color2_interpolation.flags &= ~1u;
    alpha2_interpolation.flags &= ~1u;
}
}
