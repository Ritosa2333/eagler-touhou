// 通关后标题结算流程回归：原版 0x44c570 case0 DAT_004dc670==3 →
// 流程 0xe (0x454050 名前录入) → 流程 0xf (0x4549d0 回放槽保存) / 主菜单。
// 合成 ANM 只支撑状态机与文本队列断言，不声称视觉还原。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/TitleScene.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {
// 与 title_practice_flow_test.cpp 同族标题资源：结果屏本身不建 ANM，
// 但回主菜单路径需要 89/94/95 根脚本可建。
std::vector<u8> result_title_fixture() {
    auto scripts = trivial_scripts(243);
    auto children = [&](i32 root, std::initializer_list<i32> ids) {
        scripts[u32(root)].clear();
        for (i32 id : ids) scripts[u32(root)].push_back(spawn(id));
        scripts[u32(root)].push_back(wait());
        scripts[u32(root)].push_back(stop());
    };
    scripts[0].clear();
    for (i32 id = 3; id <= 22; ++id) scripts[0].push_back(spawn(id));
    scripts[0].push_back(wait());
    scripts[0].push_back(stop());
    children(89, {92, 93, 91, 90});
    children(94, {97, 96, 100});
    children(95, {99, 96, 100});
    children(123, {83, 84, 118, 119, 120, 121, 113, 114, 115, 116});
    children(129, {125, 126, 127, 128, 135, 136, 137, 138, 131, 132, 133, 134});
    return synthetic_anm(8, scripts);
}

struct ResultHost final : TitleScenePlatform {
    bool replay_available = false;
    bool record_ok = true;
    bool save_ok = true;
    std::vector<std::string> recorded;
    std::vector<i32> saved_slots;
    std::vector<std::string> saved_names;
    std::vector<i32> sounds;
    std::vector<std::string> music;
    std::vector<HudTextDraw> texts;
    bool provide_result_data = false;
    TitleResultScreenData result_data;
    bool result_screen_data(TitleResultScreenData& out) const override {
        if (!provide_result_data) return false;
        out = result_data;
        return true;
    }

    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string& name) override {
        music.push_back(name);
        return true;
    }
    bool clear_current_wave() override { return true; }
    bool title_exit(i32) override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32 id) override { sounds.push_back(id); return true; }
    bool begin_transition(u32&) override { return false; }
    bool start_game(i32) override { return false; }
    bool hud_text(const HudTextDraw& text) override { texts.push_back(text); return true; }
    bool save_replay_slot(i32 slot, const std::string& name) override {
        saved_slots.push_back(slot);
        saved_names.push_back(name);
        return save_ok;
    }
    bool can_save_replay() const override { return true; }
    bool record_result_name(const std::string& name) override {
        recorded.push_back(name);
        return record_ok;
    }
    // 全通结算条目走 clear 槽（0x454050:209 → stage=8，与
    // ApplicationState::record_clear_result_name 同构落同一记录面）。
    bool record_clear_result_name(const std::string& name) override {
        recorded.push_back(name);
        return record_ok;
    }
    bool result_replay_save_available() const override { return replay_available; }
};

struct ResultHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings settings;
    FrameScheduler scheduler;
    ResultHost host;
    std::array<FrameCallback, 2> updates{};
    std::unique_ptr<TitleScene> scene;

    ResultHarness() {
        updates[0].owner = updates[1].owner = &manager;
        updates[0].enabled = updates[1].enabled = true;
        updates[0].run = +[](void* p) {
            return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
        };
        updates[1].run = +[](void* p) {
            return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, result_title_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, settings, manager, host, scheduler);
        scene->state.return_reason = 3;  // Ending 结束后的标题直入（dc670=3）
    }

    void frame(u32 pressed = 0) {
        scene->controls({0, pressed, 0, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scene->error.empty());
    }
    void advance(i32 count) { for (i32 i = 0; i < count; ++i) frame(); }

    // 0x454050：case0 建面 → case1 >6 帧 → case2 名字编辑。
    void enter_name_editor() {
        frame();
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::ResultName));
        CHECK_EQ(scene->state.substate, 1);
        advance(8);
        CHECK_EQ(scene->state.substate, 2);
    }

    // 光标 90(END) → 右到行首(78) → 上 6 行到 0('A') → 输入 "AAA" →
    // 下 6 行回 78 → 左回 END(90)。
    void type_aaa_to_end() {
        frame(0x80);
        for (i32 i = 0; i < 6; ++i) frame(0x10);
        CHECK_EQ(scene->state.result_name_cursor, 0);
        for (i32 i = 0; i < 3; ++i) frame(1);
        CHECK_EQ(scene->state.result_name, std::string("AAA"));
        for (i32 i = 0; i < 6; ++i) frame(0x20);
        frame(0x40);
        CHECK_EQ(scene->state.result_name_cursor, 90);
    }

    // 0x4549d0：case0 建槽表 → case1 >6 帧 → case2 槽选择。
    void enter_replay_slots() {
        frame();  // ResultReplaySlot substate0
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::ResultReplaySlot));
        CHECK_EQ(scene->state.substate, 1);
        advance(8);
        CHECK_EQ(scene->state.substate, 2);
    }
};
}  // namespace

TEST(result_return_reason_three_enters_name_entry_before_main) {
    ResultHarness h;
    h.frame();
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::ResultName));
    CHECK_EQ(h.scene->state.substate, 1);
    CHECK_EQ(h.scene->state.flags & 2u, 2u);  // 直入菜单态，跳过 180 帧 intro
    h.advance(8);
    CHECK_EQ(h.scene->state.substate, 2);
    CHECK_EQ(h.scene->state.result_name_cursor, 90);
    // 0x454050 case0 建面：背景 root89 + script107 + 角色条 143+c + 难度条 147+d。
    CHECK(h.scene->state.handles[89] != 0);
    CHECK(h.scene->state.handles[107] != 0);
    CHECK(h.scene->state.handles[143 + h.scene->state.result_character] != 0);
    CHECK(h.scene->state.handles[147 + h.scene->state.result_difficulty] != 0);
    CHECK(h.host.recorded.empty());
}

TEST(result_name_confirm_records_typed_name_then_returns_main_without_replay) {
    ResultHarness h;
    h.host.replay_available = false;  // DAT_004be7c8 != 0（或宿主未接查询）
    h.enter_name_editor();
    h.type_aaa_to_end();
    h.frame(1);  // END：record_result_name → 0x454050 case3（不立即收面）
    CHECK_EQ(h.host.recorded.size(), size_t(1));
    if (!h.host.recorded.empty()) CHECK_EQ(h.host.recorded[0], std::string("AAA"));
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::ResultName));
    CHECK_EQ(h.scene->state.substate, 3);
    CHECK(h.scene->state.handles[107] != 0);  // case3 age<=5：面未收
    CHECK(h.host.music.empty());  // 结算屏停留期不换曲（Player's Score 不退场）
    h.advance(7);  // case3 age>5 才 retire + 回主菜单
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK_EQ(h.scene->state.handles[107], 0u);
    CHECK(h.host.saved_slots.empty());
    CHECK_EQ(h.scene->state.return_reason, 1);
    // 0x454658：回主菜单重选标题曲 th13_00。
    CHECK_EQ(h.host.music, std::vector<std::string>{"th13_00.wav"});
}

TEST(result_name_grid_uses_original_layout_and_special_glyph_bytes) {
    ResultHarness h;
    h.enter_name_editor();
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    CHECK(h.scene->error.empty());
    // 原版 0x454690：不画 "RESULT NAME"/"SAVE REPLAY" 长文本（字框是
    // title.anm script107/108 位图）；91 格起点 (212,360)，列+18。
    bool grid_a = false, space_glyph = false, del_glyph = false, end_glyph = false;
    bool legacy_banner = false;
    for (const auto& text : h.host.texts) {
        if (text.text == "RESULT NAME" || text.text == "SAVE REPLAY") legacy_banner = true;
        if (text.text == "A" && text.position.x == 212 && text.position.y == 360) grid_a = true;
        if (text.text == std::string(1, char(0x81))) space_glyph = true;
        if (text.text == std::string(1, char(0x80))) del_glyph = true;
        if (text.text == std::string(1, char(0x7f))) end_glyph = true;
    }
    CHECK(!legacy_banner);
    CHECK(grid_a);
    CHECK(space_glyph);
    CHECK(del_glyph);
    CHECK(end_glyph);
}

TEST(result_name_confirm_opens_replay_slots_and_saves_one_based_slot) {
    ResultHarness h;
    h.host.replay_available = true;  // DAT_004be7c8 == 0 → 流程 0xf
    h.host.provide_result_data = true;
    h.host.result_data.rank = 0;
    h.host.result_data.current.available = true;
    h.host.result_data.current.timestamp = 1700000000;
    h.enter_name_editor();
    h.type_aaa_to_end();
    h.frame(1);  // END → 0x454050 case3
    CHECK_EQ(h.host.recorded.size(), size_t(1));
    if (!h.host.recorded.empty()) CHECK_EQ(h.host.recorded[0], std::string("AAA"));
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::ResultName));
    CHECK_EQ(h.scene->state.substate, 3);
    h.advance(7);  // case3 age>5 → 0xf
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::ResultReplaySlot));
    h.enter_replay_slots();
    CHECK_EQ(h.scene->state.result_slot, 0);
    CHECK(h.scene->state.handles[108] != 0);
    CHECK(h.scene->state.handles[89] != 0);

    // 0x454f90 state2：25 行列表 (58,80) 行距 15，不再有移植自造的
    // "REPLAY SLOT n / 25" 单行文本。
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    CHECK(h.scene->error.empty());
    i32 slots = 0;
    bool first_row = false, legacy_line = false, legacy_name_grid = false;
    for (const auto& text : h.host.texts) {
        if (text.text.rfind("No.", 0) == 0) ++slots;
        if (text.text == "No.01 -------- --/--/-- --:-- ------- ------- --- ---%")
            first_row = true;
        if (text.text.rfind("REPLAY SLOT ", 0) == 0) legacy_line = true;
        if (text.text == "RESULT NAME") legacy_name_grid = true;
        if (text.text == "A" && text.position.x == 212 && text.position.y == 360)
            legacy_name_grid = true;
    }
    CHECK_EQ(slots, 25);
    CHECK(first_row);
    CHECK(!legacy_line);
    CHECK(!legacy_name_grid);

    h.frame(0x20);
    h.frame(0x20);
    h.frame(0x20);
    CHECK_EQ(h.scene->state.result_slot, 3);
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    bool selected_row = false;
    for (const auto& text : h.host.texts)
        if (text.text.rfind("No.04 ", 0) == 0 && text.position.x == 58 &&
            text.position.y == 80 + 3 * 15 && text.style.color == 0xffffff00u)
            selected_row = true;
    CHECK(selected_row);

    // 0x4549d0 case2 Z：进本屏 8 字名字编辑，不立即落盘。
    h.frame(1);
    CHECK_EQ(h.scene->state.substate, 3);
    CHECK(h.host.saved_slots.empty());
    h.advance(10);  // 0045507b: age>=10 before names/grid appear.
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    bool editor_name = false, editor_grid = false, selected_line = false;
    bool selected_named = false;
    for (const auto& text : h.host.texts) {
        if (text.text == "AAA" && text.position.x == 112 && text.position.y == 240)
            editor_name = true;
        if (text.text == "A" && text.position.x == 212 && text.position.y == 360)
            editor_grid = true;
        if (text.text.rfind("No.04 ", 0) == 0 && text.position.x == 58 &&
            text.position.y == 240) {
            selected_line = true;
            // 00454f90 state3：头部行读当前待存 replay 头的名字，而非空格。
            selected_named = text.text.find("AAA") != std::string::npos;
        }
    }
    CHECK(editor_name);
    CHECK(editor_grid);
    CHECK(selected_line);
    CHECK(selected_named);

    // END（光标初值 0x5a）→ 写 slot+1 = 4，名字沿用 0x454050 编辑结果。
    h.frame(1);
    CHECK_EQ(h.host.saved_slots.size(), size_t(1));
    if (!h.host.saved_slots.empty()) CHECK_EQ(h.host.saved_slots[0], 4);
    if (!h.host.saved_names.empty()) CHECK_EQ(h.host.saved_names[0], std::string("AAA"));
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::ResultReplaySlot));
    CHECK_EQ(h.scene->state.substate, 2);

    // X → case4；age>5 后回主菜单。
    h.frame(0x100);
    CHECK_EQ(h.scene->state.substate, 4);
    h.advance(7);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
}

TEST(result_replay_slot_cancel_returns_main_without_saving) {
    ResultHarness h;
    h.host.replay_available = true;
    h.enter_name_editor();
    h.type_aaa_to_end();
    h.frame(1);  // END → 0x454050 case3
    h.advance(7);  // case3 age>5 → 0xf
    h.enter_replay_slots();
    h.frame(0x100);  // X/Esc：0x4549d0 case2 → case4
    CHECK_EQ(h.scene->state.substate, 4);
    h.advance(7);    // case4 age>5 收面
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK(h.host.saved_slots.empty());
    CHECK(h.host.saved_names.empty());
    // 0x454f0b：槽屏取消回主菜单同样重选标题曲 th13_00。
    CHECK_EQ(h.host.music, std::vector<std::string>{"th13_00.wav"});
}

TEST(result_original_ten_rows_keep_current_name_columns_blank) {
    ResultHarness h;
    h.host.provide_result_data = true;
    auto& data = h.host.result_data;
    data.rank = 9;
    data.character = 3;
    data.difficulty = 2;
    for (auto& record : data.records) {
        record.available = true;
        record.score = 123;
        record.name = "OLD";
    }
    h.enter_name_editor();
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    i32 rows = 0;
    bool current = false;
    for (const auto& text : h.host.texts) {
        if (text.position.x == 48) {
            ++rows;
            if (text.position.y == 322) {
                current = true;
                CHECK(text.text.find("OLD") == std::string::npos);
                CHECK(text.text.find("--------") == std::string::npos);
            }
        }
    }
    CHECK_EQ(rows, 10);
    CHECK(current);
    CHECK(h.scene->state.handles[146] != 0);
    CHECK(h.scene->state.handles[149] != 0);
}

TEST(result_slot_editor_waits_ten_frames_and_uses_original_slide) {
    ResultHarness h;
    h.host.replay_available = true;
    h.enter_name_editor();
    h.frame(1);  // END → 0x454050 case3
    h.advance(7);  // case3 age>5 → 0xf
    h.enter_replay_slots();
    h.frame(1);
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    CHECK_EQ(h.host.texts.size(), size_t(1));
    if (!h.host.texts.empty()) {
        CHECK_EQ(h.host.texts[0].position.x, 58.f);
        CHECK(h.host.texts[0].position.y >= 80.f);
        CHECK(h.host.texts[0].position.y <= 240.f);
    }
    h.advance(10);
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    CHECK(h.host.texts.size() > size_t(90));
    bool end_glyph = false, delete_glyph = false;
    for (const auto& text : h.host.texts) {
        if (text.position.x == 410 && text.position.y == 456)
            delete_glyph = text.text == std::string(1, char(0x7f));
        if (text.position.x == 428 && text.position.y == 456)
            end_glyph = text.text == std::string(1, char(0x80));
    }
    CHECK(delete_glyph);
    CHECK(end_glyph);
}

int main() { return th13test::run_all("Title result flow"); }
