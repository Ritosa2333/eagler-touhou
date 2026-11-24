#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/AsciiText.hpp"
#include "game/FrameScheduler.hpp"
#include "game/RunPause.hpp"

using namespace th13;
using namespace th13::test;

namespace {
struct NameEntryHost final : RunPausePlatform {
    bool sound(i32 id) override {
        sounds.push_back(id);
        return true;
    }
    bool suspend_music() override { return true; }
    bool restart_run() override { return true; }
    bool return_to_title() override { return true; }
    bool result_needs_name() const override { return true; }
    bool can_save_replay() const override { return true; }
    bool record_result_name(const std::string& name) override {
        ++record_calls;
        last_name = name;
        return true;
    }
    bool save_replay_slot(i32 slot, const std::string& name) override {
        saved_slots.push_back(slot);
        last_name = name;
        return true;
    }
    std::string last_result_name() const override { return last_name; }
    std::string last_name = "        ";
    std::vector<i32> sounds;
    std::vector<i32> saved_slots;
    mutable int record_calls = 0;
    i32 result_ranking(TitlePlayerDataScore rows[10]) const override {
        for (i32 i = 0; i < 10; ++i) {
            rows[i].score = 100000 - i * 10000;
            rows[i].name = "--------";
            rows[i].stage = 1;
            rows[i].timestamp = 0;
            rows[i].available = true;
        }
        return 3;
    }
};

struct GlyphSink final : AsciiGlyphServices {
    struct Glyph {
        Vec3 position;
        u32 color;
        i32 sprite;
    };
    std::vector<Glyph> glyphs;
    bool ascii_glyph(AnmVm& vm) override {
        glyphs.push_back({vm.variables.position, vm.visual.color, vm.visual.sprite});
        return true;
    }
};

struct OverlayHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    FrameScheduler scheduler;
    NameEntryHost host;
    std::unique_ptr<RunPause> pause;
    AsciiText ascii{manager, environment};

    OverlayHarness() {
        CHECK(load_bank(manager, 0, synthetic_anm(8, trivial_scripts(76))));
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(155))));
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(1))));
        CHECK(ascii.initialize(2));
        progress.rate = 1;
        progress.continue_stock = 3;
        pause = std::make_unique<RunPause>(progress, manager, environment, host, scheduler, 3);
        CHECK(pause->prepare());
    }

    void frame(u32 pressed = 0) {
        pause->controls({pressed, 0, false});
        CHECK(scheduler.update() >= 0);
    }

    void advance(int frames, u32 pressed = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed);
    }

    void draw() {
        CHECK(pause->draw_overlay(ascii));
        CHECK(ascii.error.empty());
    }
};

const GlyphSink::Glyph* find_glyph(const GlyphSink& sink, float x, float y) {
    for (const auto& glyph : sink.glyphs)
        if (glyph.position.x == x && glyph.position.y == y) return &glyph;
    return nullptr;
}

const GlyphSink::Glyph* find_sprite(const GlyphSink& sink, float x, float y, i32 sprite) {
    for (const auto& glyph : sink.glyphs)
        if (glyph.position.x == x && glyph.position.y == y && glyph.sprite == sprite) return &glyph;
    return nullptr;
}
}

TEST(game_over_name_entry_menu_deferred_and_original_face_drawn) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    CHECK_EQ(harness.pause->state.menu_animation, 0u);
    harness.advance(12);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK(harness.pause->state.replay_name.empty());
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    CHECK(harness.ascii.error.empty());
    const auto* header = find_glyph(sink, 156, 64);
    CHECK(header != nullptr);
    if (header) {
        CHECK_EQ(header->sprite, 51);
        CHECK_EQ(header->color, 0xffffffffu);
    }
    const auto* grid_a = find_glyph(sink, 112, 320);
    CHECK(grid_a != nullptr);
    if (grid_a) {
        CHECK_EQ(grid_a->sprite, 33);
        CHECK_EQ(grid_a->color, 0xffffff00u);
    }
    const auto* grid_m = find_glyph(sink, 328, 320);
    CHECK(grid_m != nullptr);
    if (grid_m) CHECK_EQ(grid_m->sprite, 'M' - 32);
    const auto* grid_end = find_glyph(sink, 328, 416);
    CHECK(grid_end != nullptr);
    if (grid_end) {
        CHECK_EQ(grid_end->sprite, 96);
        CHECK_EQ(grid_end->color, 0xff808080u);
    }
    const auto* bracket = find_glyph(sink, 238.f, 400.f);
    CHECK(bracket != nullptr);
    if (bracket) {
        CHECK_EQ(bracket->sprite, 59);
        CHECK_EQ(bracket->color, 0xff808080u);
    }
    const auto* row0 = find_glyph(sink, 57, 96);
    CHECK(row0 != nullptr);
    if (row0) {
        CHECK_EQ(row0->sprite, '1' - 32);
        CHECK_EQ(row0->color, 0xff808080u);
    }
    const auto* row3 = find_glyph(sink, 57, 150);
    CHECK(row3 != nullptr);
    if (row3) {
        CHECK_EQ(row3->sprite, '4' - 32);
        CHECK_EQ(row3->color, 0xffffff00u);
    }
    const auto* underscore = find_glyph(sink, 75, 150);
    CHECK(underscore != nullptr);
    if (underscore) CHECK_EQ(underscore->color, 0xffffff00u);
    CHECK(find_glyph(sink, 75, 54) == nullptr);
    int grid_glyphs = 0;
    for (const auto& glyph : sink.glyphs) {
        CHECK(glyph.sprite <= 97);
        if (glyph.position.y >= 320 && glyph.position.x >= 112) ++grid_glyphs;
    }
    CHECK(grid_glyphs >= 182);
}

TEST(game_over_name_entry_characters_follow_ranking_row_and_emit_menu_sounds) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    harness.advance(24);
    CHECK_EQ(harness.pause->state.phase, 18);
    harness.host.sounds.clear();
    harness.pause->state.name_cursor = 0;
    harness.frame(0x80);
    CHECK_EQ(harness.host.sounds.back(), 10);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.replay_name, std::string("B"));
    CHECK_EQ(harness.host.sounds.back(), 7);
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    bool name_found = false, underscore_found = false;
    for (const auto& glyph : sink.glyphs) {
        if (glyph.position.x == 75 && glyph.position.y == 150 && glyph.sprite == 'B' - 32) name_found = true;
        if (glyph.position.x == 84 && glyph.position.y == 150 && glyph.sprite == '_' - 32) underscore_found = true;
    }
    CHECK(name_found);
    CHECK(underscore_found);
    harness.frame(2);
    CHECK(harness.pause->state.replay_name.empty());
    CHECK_EQ(harness.host.sounds.back(), 9);
    const auto count = harness.host.sounds.size();
    harness.frame(2);
    CHECK_EQ(harness.host.sounds.size(), count);
    CHECK_EQ(harness.pause->state.phase, 18);
    harness.pause->state.replay_name = "B";
    harness.pause->state.name_cursor = 89;
    harness.frame(1);
    CHECK(harness.pause->state.replay_name.empty());
    CHECK_EQ(harness.host.sounds.back(), 7);
    const auto deleted_count = harness.host.sounds.size();
    harness.frame(1);
    CHECK_EQ(harness.host.sounds.size(), deleted_count);
    const auto end_count = harness.host.sounds.size();
    harness.pause->state.name_cursor = 90;
    harness.frame(1);
    CHECK_EQ(harness.host.sounds.size(), end_count);
    CHECK_EQ(harness.pause->state.phase, 6);
}

TEST(replay_save_name_entry_emits_replay_variant_sounds) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    harness.pause->state.select_phase(17);
    harness.advance(12);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK(!harness.pause->state.name_is_result);
    harness.advance(12);
    harness.host.sounds.clear();
    harness.pause->state.replay_name = "B";
    harness.pause->state.name_cursor = 89;
    harness.frame(1);
    CHECK(harness.pause->state.replay_name.empty());
    CHECK_EQ(harness.host.sounds.back(), 9);
    harness.frame(2);
    CHECK_EQ(harness.host.sounds.back(), 9);
    CHECK_EQ(harness.pause->state.phase, 17);
    harness.pause->state.select_phase(18);
    harness.advance(12);
    harness.pause->state.replay_name = "B";
    harness.pause->state.name_cursor = 90;
    harness.frame(1);
    CHECK_EQ(harness.host.sounds.back(), 17);
    CHECK_EQ(harness.host.saved_slots.size(), 1u);
    CHECK_EQ(harness.pause->state.phase, 17);
}

TEST(name_entry_end_creates_menu_and_credit_text) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    harness.advance(12);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK_EQ(harness.pause->state.menu_animation, 0u);
    harness.advance(12);
    harness.frame(0x102);
    CHECK_EQ(harness.pause->state.phase, 18);
    harness.pause->state.name_cursor = 90;
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 6);
    CHECK(harness.pause->state.menu_animation != 0);
    CHECK_EQ(harness.host.record_calls, 1);
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    bool credit = false;
    for (const auto& glyph : sink.glyphs)
        if (glyph.position.x == 184 && glyph.position.y == 448) credit = true;
    CHECK(credit);
}

TEST(replay_save_name_entry_uses_slot_face) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    CHECK_EQ(harness.pause->state.phase, 6);
    harness.pause->state.select_phase(17);
    harness.advance(12);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK(!harness.pause->state.name_is_result);
    harness.advance(10);
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    CHECK(find_glyph(sink, 102, 224) != nullptr);
    CHECK(find_glyph(sink, 112, 320) != nullptr);
    CHECK(find_glyph(sink, 48, 224) != nullptr);
}

TEST(game_over_replay_save_switches_faces_and_restores_menu_on_cancel) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    harness.advance(24);
    harness.pause->state.name_cursor = 90;
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 6);
    auto* menu = harness.manager.registry.find(harness.pause->state.menu_animation);
    CHECK(menu != nullptr);
    if (!menu) return;
    menu->visual.flags |= 2u;
    harness.frame(0x20);
    harness.frame(0x20);
    CHECK_EQ(harness.pause->state.menu.cursor, 2);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 17);
    CHECK_EQ(menu->visual.flags & 2u, 0u);
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    CHECK(find_glyph(sink, 48, 64) != nullptr);
    CHECK(find_glyph(sink, 48, 424) != nullptr);

    harness.advance(10);
    harness.frame(0x20);
    CHECK_EQ(harness.pause->state.replay_slot, 2);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK_EQ(menu->visual.flags & 2u, 0u);
    harness.advance(10);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.replay_name, std::string("A"));
    harness.pause->state.name_cursor = 90;
    harness.frame(1);
    CHECK_EQ(harness.host.saved_slots, std::vector<i32>{2});
    CHECK_EQ(harness.pause->state.phase, 17);
    CHECK_EQ(menu->visual.flags & 2u, 0u);
    harness.advance(10);
    harness.frame(2);
    CHECK_EQ(harness.pause->state.phase, 6);
    CHECK_EQ(harness.pause->state.menu.cursor, 2);
    CHECK_EQ(menu->visual.flags & 2u, 2u);
}

TEST(every_name_entry_starts_from_the_last_confirmed_name) {
    OverlayHarness harness;
    harness.host.last_name = "MYSTIA  ";
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    harness.advance(12);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK_EQ(harness.pause->state.replay_name, std::string("MYSTIA"));
    CHECK_EQ(harness.pause->state.name_cursor, 90);
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    const auto* first = find_sprite(sink, 75, 150, 'M' - 32);
    CHECK(first != nullptr);
    if (first) CHECK_EQ(first->color, 0xffffffffu);
    const auto* underscore = find_sprite(sink, 75 + 9 * 6, 150, '_' - 32);
    CHECK(underscore != nullptr);
    if (underscore) CHECK_EQ(underscore->color, 0xffffff00u);
    harness.advance(12);
    harness.pause->state.name_cursor = 'R' - 'A';
    harness.frame(1);
    CHECK_EQ(harness.pause->state.replay_name, std::string("MYSTIAR"));
    harness.pause->state.name_cursor = 90;
    harness.frame(1);
    CHECK_EQ(harness.host.record_calls, 1);
    CHECK_EQ(harness.host.last_name, std::string("MYSTIAR"));

    harness.pause->state.select_phase(17);
    harness.pause->state.name_is_result = false;
    harness.advance(12);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK_EQ(harness.pause->state.replay_name, std::string("MYSTIAR"));
    CHECK_EQ(harness.pause->state.name_cursor, 90);
}

TEST(blank_last_name_starts_the_editor_on_the_first_character) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    harness.pause->state.select_phase(17);
    harness.advance(12);
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK(harness.pause->state.replay_name.empty());
    CHECK_EQ(harness.pause->state.name_cursor, 0);
}

int main() { return th13test::run_all("暂停名前录入覆盖层"); }
