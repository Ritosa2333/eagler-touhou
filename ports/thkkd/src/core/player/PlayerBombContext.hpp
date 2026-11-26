// 炸弹输入绑定与屏幕着色；组合运行时分别映射bindings[5]和LifecycleContext.flash。
#pragma once
#include <cstdint>

namespace kaikeidou {
struct PlayerBombContext {
    std::uint32_t bomb_binding = 4;
    std::uint8_t screen_tint = 0;
};
}
