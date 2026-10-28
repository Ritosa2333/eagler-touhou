#include "TitleManualMenu.hpp"

#include <cstdio>

namespace th13 {

bool TitleManualMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

// 0x455330 对合：case0 建查看器（宿主服务，含 help_%.2d.png 逐页装载与
// 翻页输入——supervisor 不碰输入）→ substate1；case1 轮询 +0x128 结束位
// → 停止/释放 + change_screen(1) + 光标栈 pop。pressed/repeated 形参仅为
// 屏接口一致性保留（原版本屏不消费输入）。
bool TitleManualMenu::update(u32 pressed, u32 repeated) {
    (void)pressed;
    (void)repeated;
    switch (state.substate) {
    case 0:
        if (!host.open_manual()) return fail("Title manual viewer unavailable");
        state.change_substate(1);
        frames_ = 0;
        break;
    case 1:
        ++frames_;
        if (host.manual_finished()) {
            host.close_manual();  // 0x42d860 + 0x4a7261
            if (state.menu.depth > 0) state.menu.pop();
            state.change_screen(TitleScreen::Main);
        }
        break;
    default:
        break;
    }
    return true;
}

}  // namespace th13
