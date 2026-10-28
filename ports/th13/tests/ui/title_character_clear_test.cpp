// 角色 clear-tag 可见门单测（原版 0x451450 case0→1 → 00470030 递归隐藏）：
//   score_data + 0x56dc*character + 0x7e4 + difficulty*4 == 0 → 隐藏
//   root129 直接子脚本 131+character（含子树）；!= 0（该角色当前难度通关）
//   → 保持可见。数据源经 TitleMenuServices::character_cleared 注入，
//   默认 false = 零进度全隐藏（原版无存档行为）。
// 夹具：131-134 子脚本含 op300（sprite 置 visible），使「隐藏/未隐藏」可判别；
// 主链→难度(Hard=2)→角色屏路径复用 title_state_test 时序锚。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

#include <memory>
#include <utility>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct ClearHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    i32 cleared_character = -1, cleared_difficulty = -1;  // 唯一通关组合
    std::vector<std::pair<i32, i32>> queries;
    explicit ClearHost(AnmManager& manager) : manager(manager) {}
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
    bool character_cleared(i32 character, i32 difficulty) override {
        queries.push_back({character, difficulty});
        return character == cleared_character && difficulty == cleared_difficulty;
    }
};

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
    scripts[129].clear();
    for (i32 child : {125, 126, 127, 128, 135, 136, 137, 138, 131, 132, 133, 134})
        scripts[129].push_back(spawn(child));
    scripts[129].push_back(wait());
    scripts[129].push_back(stop());
    // clear-tag 131-134：op300 置可见后停车（未隐藏时应保持 visible=true）。
    for (i32 tag = 131; tag <= 134; tag++) {
        scripts[u32(tag)].clear();
        scripts[u32(tag)].push_back(sprite(1));
        scripts[u32(tag)].push_back(wait());
        scripts[u32(tag)].push_back(stop());
    }
    scripts[123].clear();
    for (i32 child : {83, 84, 118, 119, 120, 121, 113, 114, 115, 116})
        scripts[123].push_back(spawn(child));
    scripts[123].push_back(wait());
    scripts[123].push_back(stop());
    return synthetic_anm(8, scripts);
}
}  // namespace

struct ClearHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    ClearHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    ClearHarness() {
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
        scene->state.return_reason = 0;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }
    void reach_character_screen() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        frame(0x01);        // Z：開始 → 难度
        advance(25);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Difficulty));
        advance(10);
        frame(0x20);        // ↓ 到 Hard(2)
        CHECK_EQ(scene->state.menu.cursor, 2);
        frame(0x01);        // Z → 角色屏
        advance(20);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Character));
        CHECK_EQ(progress.difficulty, 2);
    }
    AnmVm* clear_tag(i32 character) {
        AnmVm* menu = manager.registry.find(scene->state.handles[129]);
        return menu ? manager.registry.find_child_script(*menu, 131 + character, 0) : nullptr;
    }
};

// 零进度（默认查询 false）：131-134 全隐藏且跨帧保持；查询携带当前难度 2。
TEST(character_clear_tags_hidden_without_clears) {
    ClearHarness harness;
    harness.reach_character_screen();
    for (i32 c = 0; c < 4; c++) {
        AnmVm* tag = harness.clear_tag(c);
        CHECK(tag != nullptr);
        CHECK(tag->visible == false);
        CHECK((tag->visual.flags & 1u) == 0u);
    }
    // 跨帧保持（tick 会以 visual.visible() 重算，flags bit0 被清才持久）。
    harness.advance(30);
    for (i32 c = 0; c < 4; c++) {
        AnmVm* tag = harness.clear_tag(c);
        CHECK(tag != nullptr);
        CHECK(tag->visible == false);
    }
    CHECK_EQ(int(harness.progress.difficulty), 2);
    bool queried_at_difficulty = false;
    for (const auto& query : harness.host.queries)
        if (query.second == 2) queried_at_difficulty = true;
    CHECK(queried_at_difficulty);
    CHECK(harness.scene->error.empty());
}

// 角色 2（早苗）Hard 通关：133 保持可见，其余隐藏（0x451450 按角色×难度判定）。
TEST(character_clear_tag_visible_only_for_cleared_character) {
    ClearHarness harness;
    harness.host.cleared_character = 2;
    harness.host.cleared_difficulty = 2;
    harness.reach_character_screen();
    for (i32 c = 0; c < 4; c++) {
        AnmVm* tag = harness.clear_tag(c);
        CHECK(tag != nullptr);
        if (c == 2) {
            CHECK(tag->visible == true);
            CHECK((tag->visual.flags & 1u) != 0u);
        } else {
            CHECK(tag->visible == false);
        }
    }
    harness.advance(30);
    CHECK(harness.clear_tag(2)->visible == true);   // 跨帧保持
    CHECK(harness.clear_tag(0)->visible == false);
    CHECK(harness.scene->error.empty());
}

int main() { return th13test::run_all("角色 clear-tag 门"); }
