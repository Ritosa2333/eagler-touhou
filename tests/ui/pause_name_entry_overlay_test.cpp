// GameOver 名前录入覆盖层（原版 0x43fa90 态 0xe→0x13 + 0x43de80 case 0x13
// + 0x43dd10）：
// 1. 进榜路径 open() 不建菜单脚本 142（LAB_00440520 在录入 END 才建）；
// 2. 覆盖层文本 = 原版逐帧面："Score Ranking!!"(48,64)、名字行 (75,96+rank*18)
//    + 黄 _、10 行榜 (48,96+18i) 高亮=rank、91 字符网格 (112,320) 列步 18
//    行步 16，光标黄 0xffffff00/余 0xff808180，全 font0+阴影；
// 3. SPACE/DEL/END 槽画 font0 专用字形（字符 0x81/0x7f/0x80 → sprite
//    95/96/97）——旧占位 "NAME [END]" 的 font4 '['→sprite 313 曾令生产帧
//    硬失败（2026-11-06 用户实测，ascii.anm 全文件仅 302+3=305 sprite）；
// 4. END → 建菜单 142 → phase 6 续关菜单 + "Credit %d"(184,448)。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/AsciiText.hpp"
#include "game/FrameScheduler.hpp"
#include "game/RunPause.hpp"

using namespace th13;
using namespace th13::test;

namespace {
struct NameEntryHost final : RunPausePlatform {
    bool sound(i32 id) override { sounds.push_back(id); return true; }
    bool suspend_music() override { return true; }
    bool restart_run() override { return true; }
    bool return_to_title() override { return true; }
    bool result_needs_name() const override { return true; }
    bool record_result_name(const std::string&) override { ++record_calls; return true; }
    bool save_replay_slot(i32 slot, const std::string&) override {
        saved_slots.push_back(slot);
        return true;
    }
    std::vector<i32> sounds;
    std::vector<i32> saved_slots;
    mutable int record_calls = 0;
    // 固定榜：默认分 100000-10000i、名 "--------"、rank=3（0x43f8f0 语义）。
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
    struct Glyph { Vec3 position; u32 color; i32 sprite; };
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
        // 与生产形状对齐：text.anm（bank0，s75 背景）、front.anm（bank3，
        // 根 142）、ascii.anm（bank2，302 sprite = 生产 chunk1 数量）。
        CHECK(load_bank(manager, 0, synthetic_anm(8, trivial_scripts(76))));
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(155))));
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(1))));
        CHECK(ascii.initialize(2));
        progress.rate = 1;
        progress.continue_stock = 3;
        pause = std::make_unique<RunPause>(progress, manager, environment,
                                           host, scheduler, 3);
        CHECK(pause->prepare());
    }

    void frame(u32 pressed = 0) {
        pause->controls({pressed, 0, false});
        CHECK(scheduler.update() >= 0);
    }

    void advance(int frames, u32 pressed = 0) {
        for (int i = 0; i < frames; i++) frame(pressed);
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
}  // namespace

TEST(game_over_name_entry_menu_deferred_and_original_face_drawn) {
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    // 进榜路径：菜单脚本 142 不建（态 0xe→0x13 直落名前录入）。
    CHECK_EQ(harness.pause->state.menu_animation, 0u);
    harness.advance(12);  // case0：result_needs_name → 直落 phase18
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK(harness.pause->state.replay_name.empty());
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    CHECK(harness.ascii.error.empty());
    // 头部 (48,64)：12 前导空格 ×9px 推进后 'S' 落 (156,64)，sprite 51，白。
    const auto* header = find_glyph(sink, 156, 64);
    CHECK(header != nullptr);
    if (header) {
        CHECK_EQ(header->sprite, 51);
        CHECK_EQ(header->color, 0xffffffffu);
    }
    // 网格原点 (112,320) = 'A'（sprite 33）；列 12 行 0 = 'M' (328,320)；
    // END 槽 (328,416) = 字符 0x80 → sprite 96；初值光标 90=END → 黄。
    const auto* grid_a = find_glyph(sink, 112, 320);
    CHECK(grid_a != nullptr);
    if (grid_a) CHECK_EQ(grid_a->sprite, 33);
    const auto* grid_m = find_glyph(sink, 328, 320);
    CHECK(grid_m != nullptr);
    if (grid_m) CHECK_EQ(grid_m->sprite, 'M' - 32);
    const auto* grid_end = find_glyph(sink, 328, 416);
    CHECK(grid_end != nullptr);
    if (grid_end) {
        CHECK_EQ(grid_end->sprite, 96);
        CHECK_EQ(grid_end->color, 0xffffff00u);
    }
    // 普通网格字符 0xff808180；'['（字符表索引 72，sprite 59）在行 5 列 7
    // → (238,400)。
    const auto* bracket = find_glyph(sink, 238.f, 400.f);
    CHECK(bracket != nullptr);
    if (bracket) {
        CHECK_EQ(bracket->sprite, 59);
        CHECK_EQ(bracket->color, 0xff808180u);
    }
    // 榜行 0 (48,96)/高亮行 3 (48,150)："%2d" 前导空格推进 9px，首字形
    // '1'/'4' 落 x=57；行 0 灰、高亮行黄。名字空 → 名字行无字形，黄 _
    // 落 (75,150=96+rank*18，与榜行同基 0x43e1bd)。shadow=true 每字符主/影
    // 两笔——全部字形 sprite 必须 ≤ 97（font0 'z'→90 与专用 95/96/97；旧
    // font4 '['→313 即在此被排除）。
    const auto* row0 = find_glyph(sink, 57, 96);
    CHECK(row0 != nullptr);
    if (row0) {
        CHECK_EQ(row0->sprite, '1' - 32);
        CHECK_EQ(row0->color, 0xff808180u);
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
    // 91 网格字符 × 主/影 2 笔（影子 +2,+2 不越过下一格判定带）。
    CHECK(grid_glyphs >= 182);
}

TEST(game_over_name_entry_characters_follow_ranking_row_and_emit_menu_sounds) {
    // 计分命名（0x43fa90 态 0x13）：字符/网格 DEL → 0x43ff55 共尾 sound(7)；
    // 光标位移 → sound(10)（0x4403f1）；Esc 退格 → sound(9)（0x4405b7）、
    // 空名 Esc 原地无声；END 直接落入 LAB_00440520，无任何音效。
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    harness.advance(24);
    CHECK_EQ(harness.pause->state.phase, 18);
    harness.host.sounds.clear();
    harness.pause->state.name_cursor = 0;
    harness.frame(0x80);  // 重复右移：光标位移
    CHECK_EQ(harness.host.sounds.back(), 10);
    harness.frame(1);     // Z：录入 'B'
    CHECK_EQ(harness.pause->state.replay_name, std::string("B"));
    CHECK_EQ(harness.host.sounds.back(), 7);
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    bool name_found = false, underscore_found = false;
    for (const auto& glyph : sink.glyphs) {
        if (glyph.position.x == 75 && glyph.position.y == 150 && glyph.sprite == 'B' - 32)
            name_found = true;
        if (glyph.position.x == 84 && glyph.position.y == 150 && glyph.sprite == '_' - 32)
            underscore_found = true;
    }
    CHECK(name_found);
    CHECK(underscore_found);
    harness.frame(2);     // Esc：退格 + sound(9)
    CHECK(harness.pause->state.replay_name.empty());
    CHECK_EQ(harness.host.sounds.back(), 9);
    const auto count = harness.host.sounds.size();
    harness.frame(2);     // Esc 空名：原地不动、无声
    CHECK_EQ(harness.host.sounds.size(), count);
    CHECK_EQ(harness.pause->state.phase, 18);
    harness.pause->state.replay_name = "B";
    harness.pause->state.name_cursor = 89;
    harness.frame(1);     // 网格 DEL 非空：删字 + sound(7)（0x43ff55）
    CHECK(harness.pause->state.replay_name.empty());
    CHECK_EQ(harness.host.sounds.back(), 7);
    const auto deleted_count = harness.host.sounds.size();
    harness.frame(1);     // 网格 DEL 空名：无操作无声（0x4404a3）
    CHECK_EQ(harness.host.sounds.size(), deleted_count);
    const auto end_count = harness.host.sounds.size();
    harness.pause->state.name_cursor = 90;
    harness.frame(1);     // END：建 142 进 phase 6，无音效（0x4404bd→0x440520）
    CHECK_EQ(harness.host.sounds.size(), end_count);
    CHECK_EQ(harness.pause->state.phase, 6);
}

TEST(replay_save_name_entry_emits_replay_variant_sounds) {
    // 回放保存命名（0x43fa90 态 0x12 / 0x43e780 态 10 同式）：网格 DEL →
    // sound(9)（0x440217/0x43f205）；END 保存前 sound(17)（0x440244/
    // 0x43f231：0x5a-0x49）；Esc 恒 sound(9)（0x44018e/0x43f17c），空名
    // 退回选槽（态 0x12 → 0x11 / 态 10 → 态 9）。
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    harness.pause->state.select_phase(17);
    harness.advance(12);
    harness.frame(1);  // Z → phase 18（回放保存变体）
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK(!harness.pause->state.name_is_result);
    harness.advance(12);  // phase 18 入场延迟（age > 9 才消费输入）
    harness.host.sounds.clear();
    harness.pause->state.replay_name = "B";
    harness.pause->state.name_cursor = 89;
    harness.frame(1);  // 网格 DEL 非空：删字 + sound(9)
    CHECK(harness.pause->state.replay_name.empty());
    CHECK_EQ(harness.host.sounds.back(), 9);
    harness.frame(2);  // Esc 空名：sound(9) + 退回选槽
    CHECK_EQ(harness.host.sounds.back(), 9);
    CHECK_EQ(harness.pause->state.phase, 17);
    harness.pause->state.select_phase(18);
    harness.advance(12);
    harness.pause->state.replay_name = "B";
    harness.pause->state.name_cursor = 90;
    harness.frame(1);  // END：sound(17) + 落盘回槽列表
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
    harness.advance(12);  // phase18 age > 9（入场延迟共用暂停时钟）
    // 空名 Esc：0x43fa90 态 0x13 原地不动。
    harness.frame(0x102);
    CHECK_EQ(harness.pause->state.phase, 18);
    // 光标 90=END 直接 Z：END → LAB_00440520 建 142 → phase 6。
    harness.frame(1);
    CHECK_EQ(harness.pause->state.phase, 6);
    CHECK(harness.pause->state.menu_animation != 0);
    CHECK_EQ(harness.host.record_calls, 1);
    // 续关菜单 Credit 计数（0x43de80 case 0xf：(184,448)）。
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    bool credit = false;
    for (const auto& glyph : sink.glyphs)
        if (glyph.position.x == 184 && glyph.position.y == 448) credit = true;
    CHECK(credit);
}

TEST(replay_save_name_entry_uses_slot_face) {
    // 回放保存名前录入（!name_is_result，A 机态 10）：名字行 (102,224)+黄 _
    // + 网格；不画 Score Ranking 榜面。
    OverlayHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);                    // case0 → phase 6（菜单已建）
    CHECK_EQ(harness.pause->state.phase, 6);
    harness.pause->state.select_phase(17);  // 选槽
    harness.advance(12);                    // case17 age > 9
    harness.frame(1);                       // Z → phase 18（回放保存变体）
    CHECK_EQ(harness.pause->state.phase, 18);
    CHECK(!harness.pause->state.name_is_result);
    harness.draw();
    GlyphSink sink;
    CHECK(harness.ascii.draw(0, sink));
    CHECK(find_glyph(sink, 102, 224) != nullptr);
    CHECK(find_glyph(sink, 112, 320) != nullptr);
    CHECK(find_glyph(sink, 48, 64) == nullptr);  // 无 Score Ranking 头
}

int main() { return th13test::run_all("暂停名前录入覆盖层"); }
