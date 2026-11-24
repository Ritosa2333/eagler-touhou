#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/AsciiText.hpp"
#include "game/GameHud.hpp"
#include "game/HudDrawData.hpp"
#include "game/HudFrameData.hpp"

#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

AnmIns alpha(i32 frames, i32 type, i32 value) { return AnmIns{409, 0, {frames, type, value}}; }

std::vector<std::vector<AnmIns>> front_scripts() {
    auto scripts = trivial_scripts(177);
    scripts[171] = {spawn(172), spawn(173), spawn(174), spawn(175), wait(), stop()};
    scripts[172] = {spawn(176), label(4), alpha(30, 0, 255), wait(), label(5), alpha(10, 0, 32), wait(), stop()};
    for (i32 script = 173; script <= 175; ++script) {
        scripts[u32(script)] = {label(4), alpha(30, 0, 255), wait(), label(5), alpha(10, 0, 32), wait(), stop()};
    }
    scripts[176] = {label(4), alpha(30, 0, 255), wait(), label(5), alpha(10, 0, 32), wait(), stop()};
    return scripts;
}

std::vector<std::vector<AnmIns>> ascii_scripts() {
    auto scripts = trivial_scripts(4);
    scripts[2] = {AnmIns{400, 0, {208, 39, 0}}, AnmIns{421, 0, {0, 0}}, wait(), stop()};
    scripts[3] = {AnmIns{400, 0, {220, 39, 0}}, AnmIns{421, 0, {0, 0}}, wait(), stop()};
    return scripts;
}

struct HudHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    GameHud hud{manager, progress, player, score, 3, 2};
    struct Sound : HudFrameServices {
        bool hud_sound(i32) override { return true; }
    } sound;
    struct Sink : HudDrawServices {
        std::vector<HudTextDraw> entries;
        bool hud_text(const HudTextDraw& draw) override {
            entries.push_back(draw);
            return true;
        }
    } sink;

    HudHarness() {
        CHECK(load_bank(manager, 3, synthetic_anm(177, front_scripts())));
        CHECK(load_bank(manager, 2, synthetic_anm(285, ascii_scripts())));
        CHECK(hud.initialize(HudEntryContext{}));
    }

    AnmVm* gauge_root() { return manager.registry.find(hud.gauge_main); }
    AnmVm* gauge_child(i32 script) {
        auto* root = gauge_root();
        return root ? manager.registry.find_child_script(*root, script, 0) : nullptr;
    }
    AnmVm* gauge_grandchild(i32 script) {
        auto* child = gauge_child(172);
        return child ? manager.registry.find_child_script(*child, script, 0) : nullptr;
    }
};

const HudTextDraw* find_text(const std::vector<HudTextDraw>& entries, const std::string& text) {
    for (const auto& entry : entries)
        if (entry.text == text) return &entry;
    return nullptr;
}

}

TEST(hud_timer_text_is_left_top_aligned_with_digit_alpha) {
    HudHarness h;
    auto* tens = h.manager.registry.find(h.hud.boss_hud.digit_tens_handle());
    auto* ones = h.manager.registry.find(h.hud.boss_hud.digit_ones_handle());
    CHECK(tens != nullptr);
    CHECK(ones != nullptr);
    if (!tens || !ones) return;
    tens->variables.position = {208, 39, 0};
    ones->variables.position = {220, 39, 0};
    tens->visual.color = 0x80abcdefu;

    StageEnemy enemy;
    enemy.life = 100000.f;
    enemy.position[1] = 128.f;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 35;
    input.timer_frames = 30;
    input.life_initial = 0;
    CHECK(h.hud.boss_hud.update(input, [](i32) { return true; }));
    CHECK(h.hud.boss_hud.timer_visible());
    CHECK(h.hud.draw(HudDrawContext{}, h.sink));

    const auto* dot = find_text(h.sink.entries, ".");
    const auto* fraction = find_text(h.sink.entries, "50");
    CHECK(dot != nullptr);
    CHECK(fraction != nullptr);
    if (!dot || !fraction) return;
    CHECK_EQ(dot->position.x, 224.f);
    CHECK_EQ(dot->position.y, 32.f);
    CHECK_EQ(dot->style.alignment, 1);
    CHECK_EQ(dot->style.vertical_alignment, 1);
    CHECK_EQ(dot->style.font, 4);
    CHECK_EQ(dot->style.color, 0x80abcdefu);
    CHECK_EQ(fraction->position.x, 232.f);
    CHECK_EQ(fraction->position.y, 38.f);
    CHECK_EQ(fraction->style.alignment, 1);
    CHECK_EQ(fraction->style.vertical_alignment, 1);
    CHECK_NEAR(fraction->style.scale.x, 0.6, 1e-6);
    CHECK_NEAR(fraction->style.scale.y, 0.6, 1e-6);

    AsciiText captions(h.manager, h.environment);
    CHECK(captions.initialize(2));
    CHECK(captions.hud_text(*dot));
    CHECK(captions.hud_text(*fraction));
    struct GlyphSink : AsciiGlyphServices {
        struct Glyph {
            i32 sprite;
            Vec3 position;
            Vec2 scale;
            u32 color;
        };
        std::vector<Glyph> glyphs;
        bool ascii_glyph(AnmVm& vm) override {
            glyphs.push_back({vm.visual.sprite, vm.variables.position, vm.visual.scale, vm.visual.color});
            return true;
        }
    } glyph_sink;
    CHECK(captions.draw(0, glyph_sink));
    CHECK_EQ(glyph_sink.glyphs.size(), size_t(3));
    if (glyph_sink.glyphs.size() != 3) return;
    CHECK_EQ(glyph_sink.glyphs[0].sprite, 281);
    CHECK_NEAR(glyph_sink.glyphs[0].position.x, 224.f, 1e-4);
    CHECK_NEAR(glyph_sink.glyphs[0].position.y, 32.f, 1e-4);
    CHECK_EQ(glyph_sink.glyphs[1].sprite, 275);
    CHECK_NEAR(glyph_sink.glyphs[1].position.x, 232.f, 1e-4);
    CHECK_NEAR(glyph_sink.glyphs[1].position.y, 38.f, 1e-4);
    CHECK_NEAR(glyph_sink.glyphs[2].position.x, 239.2f, 1e-4);
    CHECK_NEAR(glyph_sink.glyphs[2].position.y, 38.f, 1e-4);
    for (const auto& glyph : glyph_sink.glyphs)
        CHECK_EQ(glyph.color, 0x80abcdefu);
}

TEST(trance_gauge_bottom_left_hysteresis_and_one_level_interrupt) {
    HudHarness h;
    auto* root = h.gauge_root();
    CHECK(root != nullptr);
    if (!root) return;
    HudFrameContext frame;
    Vec3 player{-64.f, 401.f, 0};
    frame.player_position = &player;
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) == 0u);
    CHECK_EQ(root->pending_interrupt, 0);

    player = {-65.f, 401.f, 0};
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK_EQ(root->pending_interrupt, 5);
    for (i32 script : {172, 173, 174, 175}) {
        auto* child = h.gauge_child(script);
        CHECK(child != nullptr);
        if (child) CHECK_EQ(child->pending_interrupt, 5);
    }
    auto* grandchild = h.gauge_grandchild(176);
    CHECK(grandchild != nullptr);
    if (grandchild) CHECK_EQ(grandchild->pending_interrupt, 0);
    CHECK(h.manager.update());

    player = {-100.f, 390.f, 0};
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK_EQ(root->pending_interrupt, 0);

    player = {-64.f, 401.f, 0};
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK_EQ(root->pending_interrupt, 0);

    player = {-100.f, 383.f, 0};
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) == 0u);
    CHECK_EQ(root->pending_interrupt, 4);
    for (i32 script : {172, 173, 174, 175}) {
        auto* child = h.gauge_child(script);
        CHECK(child != nullptr);
        if (child) CHECK_EQ(child->pending_interrupt, 4);
    }
    grandchild = h.gauge_grandchild(176);
    CHECK(grandchild != nullptr);
    if (grandchild) CHECK_EQ(grandchild->pending_interrupt, 0);
}

TEST(trance_gauge_stage_continuation_retains_dim_state) {
    HudHarness h;
    auto* root = h.gauge_root();
    CHECK(root != nullptr);
    if (!root) return;
    HudFrameContext frame;
    Vec3 player{-65.f, 401.f, 0};
    frame.player_position = &player;
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK_EQ(root->pending_interrupt, 5);
    CHECK(h.manager.update());

    CHECK(h.hud.initialize(HudEntryContext{0, false}));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK_EQ(root->pending_interrupt, 0);
}

TEST(trance_gauge_missing_root_does_not_clear_dim_state) {
    HudHarness h;
    HudFrameContext frame;
    Vec3 player{-65.f, 401.f, 0};
    frame.player_position = &player;
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK(h.manager.retire(h.hud.gauge_main));
    CHECK_EQ(h.hud.gauge_main, 0u);
    CHECK(h.hud.update_trance());
    CHECK((h.hud.flags & 1u) != 0u);
}

TEST(trance_gauge_fresh_run_clears_dim_state) {
    HudHarness h;
    h.hud.flags |= 1u;
    CHECK(h.hud.initialize(HudEntryContext{}));
    CHECK((h.hud.flags & 1u) == 0u);
}

int main() { return th13test::run_all("HUD PR159 regression"); }
