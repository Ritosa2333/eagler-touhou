// PR159 HUD 专项回归（合成 ANM，不读私有资产）：
//   1. 0x427910:253-274 符卡计时 "."/百分秒必须左/上对齐，颜色整 dword
//      取数字 VM +0x528（含 alpha）。
//   2. 0x4265a0:85-96 灵界槽左下淡化：y>400 && x<-64 下发 label5；
//      y<384 || x>-64 恢复 label4；400/384 迟滞；只传播一层子链；
//      仅 fresh stage ctx 清 ctx+0x180 bit0；换关延续/缺槽不清。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/GameHud.hpp"
#include "game/AsciiText.hpp"
#include "game/HudDrawData.hpp"
#include "game/HudFrameData.hpp"

#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

AnmIns alpha(i32 frames, i32 type, i32 value) {
    return AnmIns{409, 0, {frames, type, value}};
}

std::vector<std::vector<AnmIns>> front_scripts() {
    auto scripts = trivial_scripts(177);
    // 灵界槽根 171：原版 ins_500 spawn 172-175。
    scripts[171] = {spawn(172), spawn(173), spawn(174), spawn(175), wait(), stop()};
    // 172-175：原版 label4=alpha 255/30f、label5=alpha 32/10f。
    // 172 额外 spawn 176，用第二层后代验证原版只打一层子链、不递归。
    scripts[172] = {spawn(176), label(4), alpha(30, 0, 255), wait(),
                    label(5), alpha(10, 0, 32), wait(), stop()};
    for (i32 script = 173; script <= 175; ++script) {
        scripts[u32(script)] = {label(4), alpha(30, 0, 255), wait(),
                                label(5), alpha(10, 0, 32), wait(), stop()};
    }
    scripts[176] = {label(4), alpha(30, 0, 255), wait(),
                    label(5), alpha(10, 0, 32), wait(), stop()};
    return scripts;
}

std::vector<std::vector<AnmIns>> ascii_scripts() {
    auto scripts = trivial_scripts(4);
    // 原版 ascii script2/3 的出生位置；ins_421(0,0) 中心锚点。
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

const HudTextDraw* find_text(const std::vector<HudTextDraw>& entries,
                             const std::string& text) {
    for (const auto& entry : entries)
        if (entry.text == text) return &entry;
    return nullptr;
}

}  // namespace

TEST(hud_timer_text_is_left_top_aligned_with_digit_alpha) {
    HudHarness h;
    auto* tens = h.manager.registry.find(h.hud.boss_hud.digit_tens_handle());
    auto* ones = h.manager.registry.find(h.hud.boss_hud.digit_ones_handle());
    CHECK(tens != nullptr);
    CHECK(ones != nullptr);
    if (!tens || !ones) return;
    // 原版 script2/3 的出生位置；同时给十位一个非 0xff alpha，证明文本
    // 颜色直接消费 VM+0x528 的整 dword，而不是 HUD 根 VM 的 alpha。
    tens->variables.position = {208, 39, 0};
    ones->variables.position = {220, 39, 0};
    tens->visual.color = 0x80abcdefu;

    StageEnemy enemy;
    enemy.life = 100000.f;  // 关闭血环门，避免合成 front 缺 240-243。
    enemy.position[1] = 128.f;
    BossFrameInput input;
    input.boss = &enemy;
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
    // 0x427910:256-257/263-264：digit(208,39)+16/-7，再 +8/+6。
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
        struct Glyph { i32 sprite; Vec3 position; Vec2 scale; u32 color; };
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
    CHECK_EQ(glyph_sink.glyphs[0].position.x, 224.f);
    CHECK_EQ(glyph_sink.glyphs[0].position.y, 32.f);
    CHECK_EQ(glyph_sink.glyphs[1].sprite, 275);
    CHECK_EQ(glyph_sink.glyphs[1].position.x, 232.f);
    CHECK_EQ(glyph_sink.glyphs[1].position.y, 38.f);
    CHECK_NEAR(glyph_sink.glyphs[2].position.x, 239.2f, 1e-5);
    CHECK_EQ(glyph_sink.glyphs[2].position.y, 38.f);
    for (const auto& glyph : glyph_sink.glyphs) CHECK_EQ(glyph.color, 0x80abcdefu);
}

TEST(trance_gauge_bottom_left_hysteresis_and_one_level_interrupt) {
    HudHarness h;
    auto* root = h.gauge_root();
    CHECK(root != nullptr);
    if (!root) return;
    HudFrameContext frame;
    Vec3 player{-64.f, 401.f, 0};  // x == -64 不进入（严格 <）
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
    // 原版 FUN_0046fc30 只传播一层：第二层后代不应收到 label5。
    auto* grandchild = h.gauge_grandchild(176);
    CHECK(grandchild != nullptr);
    if (grandchild) CHECK_EQ(grandchild->pending_interrupt, 0);
    CHECK(h.manager.update());  // 消费 label5，进入 alpha 32 插值

    // y=390 在 400/384 迟滞带内：保持淡化，不恢复。
    player = {-100.f, 390.f, 0};
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK_EQ(root->pending_interrupt, 0);

    // x == -64 仍不满足恢复条件（严格 > -64）。
    player = {-64.f, 401.f, 0};
    CHECK(h.hud.update(frame, h.sound));
    CHECK((h.hud.flags & 1u) != 0u);
    CHECK_EQ(root->pending_interrupt, 0);

    // y=383 < 384：恢复 label4，只到一层子链；第二层后代保持 0。
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
    CHECK(h.manager.update());  // 消费 label5，槽进入 alpha 32 插值

    // 原版 0x42bfa0(0xc)->0x42b990(bit1)->0x424fd0 复用 stage ctx；
    // 0x424fd0 不写 ctx+0x180，换关延续必须保留淡化状态与槽 alpha。
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
    // 原版 0x42b030 缺槽只清 ctx+0x11c，不访问 ctx+0x180。
    CHECK(h.hud.update_trance());
    CHECK((h.hud.flags & 1u) != 0u);
}

TEST(trance_gauge_fresh_run_clears_dim_state) {
    HudHarness h;
    h.hud.flags |= 1u;
    // fresh stage ctx 由 0x4264f0 memset 清零；只有这条路径清 bit0。
    CHECK(h.hud.initialize(HudEntryContext{}));
    CHECK((h.hud.flags & 1u) == 0u);
}

int main() { return th13test::run_all("HUD PR159 regression"); }
