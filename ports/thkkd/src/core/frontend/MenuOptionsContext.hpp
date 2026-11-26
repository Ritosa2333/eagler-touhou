// 两个菜单共有的外层字段；由调用方按当前活动scene同步。
#pragma once
#include <array>
#include <cstdint>
namespace kaikeidou {
struct MenuOptionsSharedContext {
    std::uint32_t outer_scene=0,phase=0,timer=0; // A8CE58、A87F48、A87F4C。
    std::array<std::uint32_t,3> sound_requests{}; // A73230确认、A73234取消、A73238移动。
};
struct MenuOptionsContext:MenuOptionsSharedContext {
    std::uint32_t selected=0; // A87F40，与Replay槽位选择同址，路由由外层负责。
    std::array<std::uint32_t,5> pending{}; // A7A238..A7A248，对应保存options顺序。
};
}
