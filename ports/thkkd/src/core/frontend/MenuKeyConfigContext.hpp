// 暂存按键绑定；确认Quit才提交SaveData，取消保留原实际绑定。
#pragma once
#include "core/frontend/MenuOptionsContext.hpp"
namespace kaikeidou {
struct MenuKeyConfigContext:MenuOptionsSharedContext {
    std::uint32_t selected=0; // A87F3C。
    std::array<std::uint32_t,9> pending{}; // A773B4..A773D4。
};
}
