// help 查看器（0x42d940/0x42d740/0x42da10）状态机单测：
//   8 页菜单创建；Z 装载页图（宿主 load_page）并进入页视图；页视图 ↑↓
//   0..7 限位 + 音10；X 回菜单；菜单 X → 音9 → age>29 结束位；失败如实
//   报错；close 回收 VM 后可重入。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/ManualViewer.hpp"

#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct Harness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    ManualViewer viewer{manager, 19};
    std::vector<i32> loaded, sounds;
    bool load_ok = true, sound_ok = true;
    Harness() {
        CHECK(load_bank(manager, 19, help_anm_fixture()));
        viewer.load_page = [this](i32 page) {
            loaded.push_back(page);
            return load_ok;
        };
        viewer.sound = [this](i32 id) {
            sounds.push_back(id);
            return sound_ok;
        };
    }
    static std::vector<u8> help_anm_fixture() {
        std::vector<std::vector<AnmIns>> scripts(10);
        for (i32 i = 0; i < 8; ++i)
            scripts[u32(i)] = {label(1), label(2), label(3), wait(), stop()};
        scripts[9] = {wait(), stop()};
        return synthetic_anm(1, scripts);
    }
    void advance(i32 frames, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < frames; ++i) CHECK(viewer.update(pressed, repeated));
    }
};

}  // namespace

TEST(manual_viewer_open_and_menu_input_gate) {
    Harness h;
    CHECK(h.viewer.open());
    CHECK_EQ(h.viewer.current_page(), 0);
    CHECK(!h.viewer.reading_page());
    CHECK(!h.viewer.finished());
    // case1：age>0x13 前不消费输入。
    h.advance(19, 0x80001);
    CHECK(!h.viewer.reading_page());
    CHECK(h.loaded.empty());
    CHECK(h.viewer.update(0x80001, 0));
    CHECK(h.viewer.reading_page());
    CHECK_EQ(h.loaded.size(), 1u);
    CHECK_EQ(h.loaded[0], 0);
    CHECK(!h.sounds.empty() && h.sounds.back() == 7);
}

TEST(manual_viewer_page_navigation_bounds) {
    Harness h;
    CHECK(h.viewer.open());
    h.advance(19);
    CHECK(h.viewer.update(0x80001, 0));  // 页 0
    CHECK(h.viewer.reading_page());
    h.advance(19);
    CHECK(h.viewer.update(0x10, 0));     // 页 0 ↑：限位不动
    CHECK_EQ(h.viewer.current_page(), 0);
    CHECK_EQ(h.loaded.size(), 1u);
    for (i32 page = 1; page < 8; ++page) {
        h.advance(19);                   // case3/4 页切换后的 age>0x13 门
        CHECK(h.viewer.update(0x20, 0));  // ↓ 1..7
        CHECK_EQ(h.viewer.current_page(), page);
        CHECK_EQ(h.loaded.back(), page);
    }
    h.advance(19);
    CHECK(h.viewer.update(0x20, 0));     // 页 7 ↓：限位不动
    CHECK_EQ(h.viewer.current_page(), 7);
    CHECK_EQ(h.loaded.size(), 8u);
    CHECK(h.viewer.update(0x10, 0));     // ↑ 回 6
    CHECK_EQ(h.viewer.current_page(), 6);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
    // X：回菜单（音9），不结束查看器（页切换后同样有 age>0x13 门）。
    h.advance(19);
    CHECK(h.viewer.update(0x102, 0));
    CHECK(!h.viewer.reading_page());
    CHECK(!h.viewer.finished());
    CHECK_EQ(h.sounds.back(), 9);
}

TEST(manual_viewer_menu_cancel_sets_finished_after_thirty_frames) {
    Harness h;
    CHECK(h.viewer.open());
    h.advance(20);
    CHECK(h.viewer.update(0x102, 0));  // 菜单 X → state2
    CHECK(!h.viewer.finished());
    h.advance(29);
    CHECK(!h.viewer.finished());       // age>0x1d 才置位
    CHECK(h.viewer.update(0, 0));
    CHECK(h.viewer.finished());
}

TEST(manual_viewer_failures_are_honest) {
    Harness h;
    CHECK(h.viewer.open());
    h.advance(19);
    h.load_ok = false;
    CHECK(!h.viewer.update(0x80001, 0));
    CHECK(!h.viewer.error.empty());
    CHECK(!h.viewer.reading_page());

    Harness sounds;
    CHECK(sounds.viewer.open());
    sounds.advance(19);
    sounds.sound_ok = false;
    CHECK(!sounds.viewer.update(0x20, 0));
    CHECK(!sounds.viewer.error.empty());
}

TEST(manual_viewer_close_releases_and_allows_reentry) {
    Harness h;
    const u32 baseline = h.manager.registry.count();
    CHECK(h.viewer.open());
    CHECK(h.manager.registry.count() > baseline);
    h.viewer.close();
    CHECK(h.manager.update());
    CHECK_EQ(h.manager.registry.count(), baseline);
    CHECK(!h.viewer.finished());
    // 重入：同一 viewer 再次 open，页图回调继续工作。
    h.loaded.clear();
    CHECK(h.viewer.open());
    h.advance(19);
    CHECK(h.viewer.update(0x80001, 0));
    CHECK_EQ(h.loaded.size(), 1u);
    CHECK_EQ(h.loaded[0], 0);
    CHECK(h.viewer.error.empty());
}

int main() {
    std::printf("# manual_viewer_test\n");
    return th13test::run_all("manual_viewer_test");
}
