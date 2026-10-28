// PlayerData/SpellPractice 标题路由骨架接线测试（TitleScene/TitleFrame owner）：
//   - 主菜单 entry PlayerData(5) → 屏值 10（0x44c570 case a → 0x453170）；
//   - 主菜单 entry SpellPractice(3) → 屏值 17（case 0x11/0x12/0x13）；
//   - PlayerData X/Esc → case2→case3 收面 → Main + 光标栈 pop（回到 5）；
//   - SpellPractice Cancel → Main + 光标栈 pop（回到 3）；
//   - SpellPractice update 的 false（普通导航/禁用卡）不是帧错误；
//   - Launch 只保留原版启动拍命令 + 显式未实现状态，不 start_game、不切屏、不 fatal；
//   - 空 catalog / 未接 PlayerData 宿主如实失败，不伪造解锁、记录或分数。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct RouteHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    // PlayerData 宿主（默认成功，测试可按用例置失败/空表）。
    bool player_open_ok = true;
    i32 player_record_count = 3;
    int player_opens = 0, player_closes = 0;
    // SpellPractice 宿主：1 关、4 槽、仅 slot0 解锁（0x456af0）。
    i32 spell_stages = 1;

    explicit RouteHost(AnmManager& manager) : manager(manager) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = manager.create(8, 0, -1, 0);
        return animation != 0;
    }
    bool start_game(i32 stage) override {
        start_stage = stage;
        return true;
    }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool title_exit(i32) override { return true; }

    // TitlePlayerDataPlatform
    bool open_player_data() override {
        ++player_opens;
        return player_open_ok;
    }
    void close_player_data() override { ++player_closes; }
    i32 difficulty_count() const override { return kPlayerDataDifficulties; }
    // 同时满足 TitleSpellPracticePlatform 的同名虚函数；Spell 侧只做上限钳制。
    i32 character_count() const override { return kPlayerDataCharacters; }
    i32 record_count(i32 difficulty) const override {
        return difficulty == 0 ? player_record_count : 0;
    }
    bool record(i32 difficulty, i32 character, i32 ordinal,
                TitlePlayerDataRecord& out) const override {
        if (difficulty != 0 || ordinal < 0 || ordinal >= player_record_count) return false;
        out.slot = ordinal;
        out.name = "route-record-" + std::to_string(ordinal);
        out.score = 1000 + ordinal;
        out.number_a = ordinal + 1;
        out.number_b = 6;
        out.character_flagged = character == 0;
        return true;
    }

    // TitleSpellPracticePlatform
    i32 stage_count() const override { return spell_stages; }
    bool stage_available(i32 stage) const override {
        return stage >= 1 && stage <= spell_stages;
    }
    std::string stage_name(i32 stage) const override {
        return "stage " + std::to_string(stage);
    }
    i32 spell_count(i32 stage) const override {
        return stage >= 1 && stage <= spell_stages ? kSpellPracticeSlots[stage - 1] : 0;
    }
    bool spell_unlocked(i32 stage, i32 slot) const override {
        return stage == 1 && slot == 0;
    }
    std::string spell_name(i32 stage, i32 slot) const override {
        return "spell " + std::to_string(stage) + "-" + std::to_string(slot);
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        return stage * 100 + slot * 10 + difficulty;
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        (void)stage;
        (void)slot;
        return difficulty;
    }
    std::string character_name(i32 character) const override {
        return "char " + std::to_string(character);
    }
};

// title.anm 合成镜像（同 title_state_test）：root0 spaw 3-22、灰条 label 7+k、
// 背景 89、LOGO 94/95。PlayerData/SpellPractice 路由不需要难度/角色树。
std::vector<u8> title_anm_fixture() {
    std::vector<std::vector<AnmIns>> scripts(243);
    for (auto& script : scripts) script = {wait(), stop()};
    auto spawn_range = [&](i32 root, i32 first, i32 last) {
        scripts[u32(root)].clear();
        for (i32 child = first; child <= last; child++)
            scripts[u32(root)].push_back(spawn(child));
        scripts[u32(root)].push_back(wait());
        scripts[u32(root)].push_back(stop());
    };
    spawn_range(0, 3, 22);
    for (i32 item = 0; item < 10; item++) {
        scripts[u32(3 + item)].clear();
        scripts[u32(3 + item)].push_back(label(7 + item));
        scripts[u32(3 + item)].push_back(wait());
        scripts[u32(3 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);
    spawn_range(2, 49, 82);
    scripts[89] = {spawn(92), spawn(93), spawn(91), spawn(90), wait(), stop()};
    scripts[94] = {spawn(97), spawn(96), spawn(100), wait(), stop()};
    scripts[95] = {spawn(99), spawn(96), spawn(100), wait(), stop()};
    return synthetic_anm(8, scripts);
}

struct RouteHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    RouteHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    RouteHarness() {
        animation_updates[0].owner = animation_updates[1].owner = &manager;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler,
                                             TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;  // 冷启动
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }
    void reach_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    void open_entry(i32 entry) {
        reach_menu();
        for (int i = 0; i < 12 && scene->state.menu.cursor != entry; i++) frame(0x20);
        CHECK_EQ(scene->state.menu.cursor, entry);
        frame(0x01);       // Z 确认
        advance(30);       // 主菜单 Z 序 + enter() 转屏
    }
};

}  // namespace

// 主菜单 entry 5 → 屏值 10；成功开表后 case1→case2；X/Esc 收面回 Main，
// 光标栈 pop 恢复主菜单 cursor=5/count=10。
TEST(player_data_entry_and_escape_restore_main_menu_stack) {
    RouteHarness harness;
    harness.open_entry(i32(TitleMainEntry::PlayerData));
    CHECK_EQ(int(harness.scene->state.screen), 10);
    CHECK_EQ(harness.host.player_opens, 1);
    CHECK(harness.scene->player_data_menu() != nullptr);
    CHECK(harness.scene->player_data_menu()->opened());
    CHECK_EQ(harness.scene->state.menu.depth, 1);
    for (int i = 0; i < 10 && harness.scene->state.substate != 2; i++) harness.frame();
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK(harness.scene->player_data_menu()->error.empty());

    harness.frame(0x100);  // X/Esc：case2 → case3
    CHECK_EQ(harness.scene->state.substate, 3);
    for (int i = 0; i < 10 && int(harness.scene->state.screen) == 10; i++) harness.frame();
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.depth, 0);
    CHECK_EQ(harness.scene->state.menu.cursor, i32(TitleMainEntry::PlayerData));
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    CHECK_EQ(harness.host.player_closes, 1);
    CHECK(harness.scene->player_data_menu() == nullptr);  // 下次进屏重新开表
    harness.advance(15);  // 返回路径 11 帧菜单就绪
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK(harness.scene->error.empty());
}

// 宿主未接/读取失败：PlayerData open 如实报错，不造记录、不空白假成功。
TEST(player_data_unavailable_reports_error_without_invented_records) {
    RouteHarness harness;
    harness.host.player_open_ok = false;
    harness.reach_menu();
    for (int i = 0; i < 12 && harness.scene->state.menu.cursor != 5; i++)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 5);
    harness.scene->controls({0, 0x01, 0, 0});
    bool errored = harness.scheduler.update() < 0;
    for (int i = 0; i < 40 && !errored; i++) {
        harness.scene->controls({0, 0, 0, 0});
        errored = harness.scheduler.update() < 0;
    }
    CHECK(errored);
    CHECK_EQ(int(harness.scene->state.screen), 10);
    CHECK(!harness.scene->error.empty());
    CHECK_EQ(harness.host.player_opens, 1);
    CHECK(harness.scene->player_data_menu() != nullptr);
    CHECK(!harness.scene->player_data_menu()->opened());
    CHECK(harness.scene->player_data_menu()->records().empty());
    CHECK_EQ(harness.host.player_closes, 0);  // 未打开即未关闭
}

// 主菜单 entry 3 → 屏值 17；三级确认产生 Launch 命令但不启动对局（显式未实现
// 状态、非帧错误）；逐级 Cancel + 顶层 Cancel → Main + 光标栈 pop → cursor=3。
TEST(spell_practice_launch_is_explicit_and_cancel_restores_menu_stack) {
    RouteHarness harness;
    harness.open_entry(i32(TitleMainEntry::SpellPractice));
    CHECK_EQ(int(harness.scene->state.screen), 17);
    CHECK(harness.scene->spell_practice_menu() != nullptr);
    CHECK_EQ(harness.scene->state.menu.depth, 1);
    CHECK_EQ(harness.scene->state.menu.cursor, i32(TitleMainEntry::SpellPractice));
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    CHECK(!harness.scene->spell_practice_launch_pending());

    harness.frame(0x01);  // stage 1
    CHECK_EQ(i32(harness.scene->spell_practice_menu()->level()),
             i32(SpellPracticeLevel::Spell));
    harness.frame(0x01);  // unlocked spell 0
    CHECK_EQ(i32(harness.scene->spell_practice_menu()->level()),
             i32(SpellPracticeLevel::Difficulty));
    harness.frame(0x01);  // variant confirm → Launch terminal event
    CHECK_EQ(int(harness.scene->state.screen), 17);  // 不切屏
    CHECK(harness.scene->spell_practice_launch_pending());
    CHECK(!harness.scene->spell_practice_launch_status().empty());
    CHECK_EQ(harness.host.start_stage, -1);  // 不启动对局
    CHECK_EQ(i32(harness.scene->spell_practice_menu()->command().action),
             i32(SpellPracticeAction::Launch));
    CHECK_EQ(harness.scene->spell_practice_menu()->command().stage, 1);
    CHECK_EQ(harness.scene->spell_practice_menu()->command().spell_slot, 0);
    CHECK_EQ(harness.scene->spell_practice_menu()->command().character, 0);
    CHECK(harness.scene->error.empty());

    harness.frame(0x100);  // Difficulty → Spell
    CHECK_EQ(i32(harness.scene->spell_practice_menu()->level()),
             i32(SpellPracticeLevel::Spell));
    harness.frame(0x100);  // Spell → Stage
    CHECK_EQ(i32(harness.scene->spell_practice_menu()->level()),
             i32(SpellPracticeLevel::Stage));
    harness.frame(0x100);  // Stage Cancel → Main
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.depth, 0);
    CHECK_EQ(harness.scene->state.menu.cursor, i32(TitleMainEntry::SpellPractice));
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    CHECK(harness.scene->spell_practice_menu() == nullptr);  // 下次进屏从 Stage 起步
    CHECK(!harness.scene->spell_practice_launch_pending());
    harness.advance(15);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK(harness.scene->error.empty());
}

// 空 catalog：TitleSpellPracticeMenu 的 false（blocked）不是帧错误；X/Esc
// 在 Stage 级 Cancel 直接回 Main（宿主未接解锁/卡表时不假造卡）。
TEST(spell_practice_empty_catalog_is_not_a_frame_error) {
    RouteHarness harness;
    harness.host.spell_stages = 0;
    harness.open_entry(i32(TitleMainEntry::SpellPractice));
    CHECK_EQ(int(harness.scene->state.screen), 17);
    CHECK(harness.scene->spell_practice_menu() != nullptr);
    CHECK(!harness.scene->spell_practice_menu()->error.empty());
    harness.frame(0x01);  // confirm blocked（stage_rows 为空）
    CHECK_EQ(int(harness.scene->state.screen), 17);
    CHECK(harness.scene->error.empty());
    CHECK(!harness.scene->spell_practice_menu()->blocked.empty());
    harness.frame(0x100);  // Stage Cancel → Main
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.depth, 0);
    CHECK_EQ(harness.scene->state.menu.cursor, i32(TitleMainEntry::SpellPractice));
}

int main() { return th13test::run_all("PlayerData/SpellPractice 路由接线"); }
