// 发射所需机体、绑定、锁定字段及 Replay 按钮位图，输入计数由调用方提供。
#pragma once
#include "core/player/OptionsPool.hpp"

namespace kaikeidou {
struct PlayerShotFireContext {
    OptionsContext options;
    std::uint32_t fire_binding = 5, slow_binding = 11, fire_lock = 0;
    bool replay_variant = false;
    std::uint32_t replay_mode = 0, replay_buttons = 0;
};
struct PlayerShotFireInput {
    virtual ~PlayerShotFireInput() = default;
    virtual std::uint32_t read_counter(std::uint32_t binding) = 0;
};
}
