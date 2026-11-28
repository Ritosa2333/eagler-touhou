#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

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
        for (i32 i = 0; i < frames; ++i)
            CHECK(viewer.update(pressed, repeated));
    }
};

}

TEST(manual_viewer_open_and_menu_input_gate) {
    Harness h;
    CHECK(h.viewer.open());
    CHECK_EQ(h.viewer.current_page(), 0);
    CHECK(!h.viewer.reading_page());
    CHECK(!h.viewer.finished());
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
    CHECK(h.viewer.update(0x80001, 0));
    CHECK(h.viewer.reading_page());
    h.advance(19);
    CHECK(h.viewer.update(0x10, 0));
    CHECK_EQ(h.viewer.current_page(), 0);
    CHECK_EQ(h.loaded.size(), 1u);
    for (i32 page = 1; page < 8; ++page) {
        h.advance(19);
        CHECK(h.viewer.update(0x20, 0));
        CHECK_EQ(h.viewer.current_page(), page);
        CHECK_EQ(h.loaded.back(), page);
    }
    h.advance(19);
    CHECK(h.viewer.update(0x20, 0));
    CHECK_EQ(h.viewer.current_page(), 7);
    CHECK_EQ(h.loaded.size(), 8u);
    CHECK(h.viewer.update(0x10, 0));
    CHECK_EQ(h.viewer.current_page(), 6);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
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
    CHECK(h.viewer.update(0x102, 0));
    CHECK(!h.viewer.finished());
    h.advance(29);
    CHECK(!h.viewer.finished());
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
