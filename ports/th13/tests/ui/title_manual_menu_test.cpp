// マニュアル屏单测（FUN_00455330 对合 + help 页名资源断言）：
//   open 失败如实 fail 不假成功；成功 → substate1；结束位 → close + 回 Main；
//   页名必须 help_%.2d.png（0x42da10:129 sprintf 实读）。
#include "../harness/test_framework.hpp"

#include "game/TitleManualMenu.hpp"

#include <string>

using namespace th13;

namespace {

struct ManualHost final : TitleManualPlatform {
    bool open_ok = true, finished = false;
    i32 opens = 0, closes = 0;
    bool open_manual() override { ++opens; return open_ok; }
    bool manual_finished() const override { return finished; }
    void close_manual() override { ++closes; }
};

}  // namespace

TEST(manual_page_name_resource_assertion) {
    ManualHost host;
    CHECK_EQ(host.help_page_name(0), std::string("help_01.png"));
    CHECK_EQ(host.help_page_name(9), std::string("help_10.png"));
    CHECK_EQ(host.help_page_name(11), std::string("help_12.png"));
}

TEST(manual_open_failure_is_honest) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenManual);
    ManualHost host;
    host.open_ok = false;
    TitleManualMenu menu{state, host};
    CHECK(!menu.update(0, 0));
    CHECK(!menu.error.empty());
    CHECK_EQ(state.substate, 0);  // 不前进、不假成功
}

TEST(manual_lifecycle_to_main) {
    TitleState state;
    state.screen = TitleScreen(kTitleScreenManual);
    ManualHost host;
    TitleManualMenu menu{state, host};
    CHECK(menu.update(0, 0));
    CHECK_EQ(state.substate, 1);
    CHECK_EQ(host.opens, 1);
    CHECK(menu.update(0xffff, 0xffff));  // 本屏不消费任何输入
    CHECK_EQ(int(state.screen), kTitleScreenManual);
    host.finished = true;
    CHECK(menu.update(0, 0));
    CHECK_EQ(host.closes, 1);
    CHECK_EQ(int(state.screen), int(TitleScreen::Main));
    CHECK(menu.error.empty());
}

int main() {
    std::printf("# title_manual_menu_test\n");
    return th13test::run_all("title_manual_menu_test");
}
