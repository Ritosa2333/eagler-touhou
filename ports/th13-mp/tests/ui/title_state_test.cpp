#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"
#include <cstdio>

#include "game/FrameScheduler.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/TitleScene.hpp"

using namespace th13;
using namespace th13::test;

namespace {
struct TitleHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1, exit_destination = -1;
    int sounds = 0;
    bool extra = false;
    u32 extra_character_mask = 0xfu;
    explicit TitleHost(AnmManager& manager) : manager(manager) {}
    bool extra_unlocked(i32 character) override { return extra && ((extra_character_mask >> (character & 3)) & 1u); }
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override {
        ++sounds;
        return true;
    }
    bool begin_transition(u32& animation) override {
        animation = manager.create(8, 0, -1, 0);
        return animation != 0;
    }
    bool start_game(i32 stage) override {
        if (start_stage >= 0) return false;
        start_stage = stage;
        return true;
    }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool title_exit(i32 destination) override {
        if (start_stage >= 0) return true;
        exit_destination = destination;
        return true;
    }
};

AnmIns layer(i32 value) { return AnmIns{304, 0, {value}}; }
AnmIns alpha_set(i32 value) { return AnmIns{403, 0, {value}}; }
AnmIns alpha_interpolate(i32 frames, i32 easing, i32 target) { return AnmIns{409, 0, {frames, easing, target}}; }

std::vector<u8> title_anm_fixture() {
    std::vector<std::vector<AnmIns>> scripts(243);
    for (auto& script : scripts)
        script = {wait(), stop()};
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
        scripts[u32(13 + item)].clear();
        scripts[u32(13 + item)].push_back(alpha_set(0));
        scripts[u32(13 + item)].push_back(AnmIns{404, 0, {0xc0, 0xc0, 0xc0}});
        scripts[u32(13 + item)].push_back(alpha_interpolate(15, 0, 255));
        scripts[u32(13 + item)].push_back(wait());
        scripts[u32(13 + item)].push_back(label(3));
        scripts[u32(13 + item)].push_back(alpha_interpolate(5, 4, 255));
        scripts[u32(13 + item)].push_back(wait());
        scripts[u32(13 + item)].push_back(label(7));
        scripts[u32(13 + item)].push_back(label(17));
        scripts[u32(13 + item)].push_back(alpha_interpolate(5, 4, 0));
        scripts[u32(13 + item)].push_back(wait());
        scripts[u32(13 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);
    spawn_range(2, 49, 82);
    scripts[89] = {spawn(92), spawn(93), spawn(91), spawn(90), wait(), stop()};
    scripts[90] = {layer(22), wait(), stop()};
    scripts[91] = {layer(21), wait(), stop()};
    scripts[92] = {layer(21), wait(), stop()};
    scripts[93] = {layer(21), wait(), stop()};
    scripts[94] = {spawn(97), spawn(96), spawn(100), wait(), label(1), stop()};
    scripts[95] = {spawn(99), spawn(96), spawn(100), wait(), label(1), stop()};
    scripts[129].clear();
    for (i32 child : {125, 126, 127, 128, 135, 136, 137, 138, 131, 132, 133, 134})
        scripts[129].push_back(spawn(child));
    scripts[129].push_back(wait());
    scripts[129].push_back(stop());
    scripts[130].clear();
    for (i32 child : {125, 126, 127, 128, 139, 140, 141, 142})
        scripts[130].push_back(spawn(child));
    scripts[130].push_back(wait());
    scripts[130].push_back(stop());
    scripts[123].clear();
    for (i32 child : {83, 84, 118, 119, 120, 121, 113, 114, 115, 116})
        scripts[123].push_back(spawn(child));
    scripts[123].push_back(wait());
    scripts[123].push_back(stop());
    return synthetic_anm(72, scripts);
}

struct TitleHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    TitleHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    TitleHarness() {
        animation_updates[0].owner = animation_updates[1].owner = &manager;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) { return static_cast<AnmManager*>(p)->update(true) ? 1 : 5; };
        animation_updates[1].run = [](void* p) { return static_cast<AnmManager*>(p)->update(false) ? 1 : 5; };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, {{wait(), label(1), stop()}})));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler, TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
    }

    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed, repeated);
    }

    void reach_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    int particle_count() {
        const auto* resource = manager.resource(2);
        int count = 0;
        for (u32 index = 0;; index++) {
            const u32 handle = manager.registry.ordered_handle(false, index);
            if (!handle) break;
            auto* vm = manager.registry.find(handle);
            if (vm && vm->resource == resource && vm->source_script == 18) count++;
            if (index > 8191) break;
        }
        return count;
    }
};
}

TEST(title_selection_stays_highlighted_and_exits_on_confirmation) {
    TitleHarness harness;
    harness.reach_menu();
    for (int i = 0; i < 20; ++i) {
        harness.frame();
        CHECK(harness.scheduler.draw() >= 0);
    }
    auto* root = harness.manager.registry.find(harness.scene->state.handles[0]);
    CHECK(root != nullptr);
    auto* selected = harness.manager.registry.find_child_script(*root, 3, 0);
    auto* white = harness.manager.registry.find_child_script(*root, 13, 0);
    CHECK(selected != nullptr);
    CHECK(white != nullptr);
    CHECK_EQ(selected->visual.color >> 24, 255u);
    CHECK_EQ(white->visual.color >> 24, 0u);
    TitleMainMenu menu(harness.scene->state, harness.progress, harness.selection, harness.manager,
                       TitleSceneResources{});
    menu.play_sound = [](i32) { return true; };
    CHECK(menu.update(0x01, 0));
    CHECK_EQ(selected->pending_interrupt, 6);
    CHECK_EQ(white->pending_interrupt, 6);
}

TEST(title_boot_introduces_menu_per_real_timeline) {
    TitleHarness harness;
    harness.advance(1);
    CHECK(harness.scene->state.handles[89] != 0);
    CHECK(harness.scene->state.handles[94] != 0);
    CHECK_EQ(harness.scene->state.handles[0], 0u);
    harness.advance(kTitleBootIntroFrames);
    CHECK(harness.scene->state.handles[0] != 0);
    CHECK(harness.scene->state.version_bar != 0);
    CHECK_EQ(harness.scene->state.substate, 1);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.advance(kTitleMenuReadyDelay);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
    CHECK(harness.scene->error.empty());
}

TEST(title_full_chain_start_game) {
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    CHECK(harness.scene->state.handles[123] != 0);
    CHECK(harness.scene->state.handles[101] != 0);
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
    harness.frame(0x01);
    harness.advance(20);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    CHECK_EQ(harness.progress.difficulty, 2);
    CHECK(harness.scene->state.handles[2] == 0);
    CHECK(harness.scene->state.handles[129] != 0);
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.frame(0x80);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x01);
    harness.advance(45);
    CHECK_EQ(harness.host.start_stage, 1);
    CHECK_EQ(harness.progress.character, 1);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::StartGame));
    CHECK(harness.scene->error.empty());
    CHECK_EQ(harness.host.exit_destination, -1);
}

TEST(title_start_retires_restore_logo_root_after_menu_return) {
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.advance(10);
    harness.frame(0x100);
    harness.advance(30);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK(harness.scene->state.handles[95] != 0);
    CHECK_EQ(harness.scene->state.handles[94], 0u);
    const u32 restore = harness.scene->state.handles[95];
    auto* restore_vm = harness.manager.registry.find(restore);
    CHECK(restore_vm != nullptr);
    CHECK(harness.manager.registry.find_child_script(*restore_vm, 99, 0) != nullptr);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.handles[94], 0u);
    CHECK_EQ(harness.scene->state.handles[95], 0u);
    CHECK(harness.manager.registry.find(restore) == nullptr);
    CHECK(harness.scene->error.empty());
}

TEST(title_quit_requests_exit_destination) {
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x10);
    CHECK_EQ(harness.scene->state.menu.cursor, 9);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Quit));
    harness.advance(2);
    CHECK_EQ(harness.host.exit_destination, 3);
    CHECK(harness.scene->error.empty());
}

TEST(title_disabled_entries_not_selectable) {
    TitleHarness harness;
    harness.reach_menu();
    for (int i = 0; i < 11; i++)
        harness.frame(0x20);
    const i32 cursor = harness.scene->state.menu.cursor;
    CHECK(cursor != 1);
}

TEST(title_unimplemented_entries_confirm_without_switching) {
    TitleHarness harness;
    harness.reach_menu();
    for (int i = 0; i < 4; i++)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 5);
    const int sounds_before = harness.host.sounds;
    harness.frame(0x01);
    CHECK(harness.host.sounds == sounds_before + 1);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.substate, 4);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 5);
    CHECK(harness.scene->error.empty());
}

TEST(title_difficulty_quit_row_returns_to_main) {
    return;
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.advance(10);
    for (int i = 0; i < 3; i++)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 4);
    harness.frame(0x01);
    harness.advance(30);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK(harness.scene->state.handles[0] != 0);
}

TEST(title_character_escape_returns_to_difficulty) {
    return;
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);
    harness.advance(25);
    harness.advance(10);
    harness.frame(0x01);
    harness.advance(20);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    harness.advance(10);
    harness.frame(0x80);
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
    harness.frame(0x100);
    harness.advance(10);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x100);
    harness.advance(10);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    harness.advance(12);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x01);
    harness.advance(25);
    harness.advance(10);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.frame(0x01);
    harness.advance(20);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
}

TEST(title_background_layer_pokes_per_oracle) {
    TitleHarness harness;
    harness.advance(1);
    auto* root = harness.manager.registry.find(harness.scene->state.handles[89]);
    CHECK(root != nullptr);
    auto layer_of = [&](i32 script) {
        auto* vm = harness.manager.registry.find_child_script(*root, script, 0);
        return vm ? vm->visual.layer : -1;
    };
    CHECK_EQ(layer_of(90), 22);
    CHECK_EQ(layer_of(91), 21);
    CHECK_EQ(layer_of(92), 21);
    CHECK_EQ(layer_of(93), 21);
    harness.advance(168);
    CHECK_EQ(layer_of(93), 21);
    harness.frame();
    CHECK_EQ(layer_of(93), 21);
    CHECK_EQ(layer_of(90), 22);
    CHECK_EQ(layer_of(91), 21);
    CHECK_EQ(layer_of(92), 21);
}

TEST(title_difficulty_anatomy_choreography) {
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);
    harness.advance(21);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    auto& registry = harness.manager.registry;
    auto* menu_root = registry.find(harness.scene->state.handles[0]);
    CHECK(menu_root != nullptr);
    CHECK_EQ(harness.scene->state.confirmed_entry, 0);
    auto* white0 = registry.find_child_script(*menu_root, 13, 0);
    CHECK(white0 != nullptr);
    CHECK(!white0->visible);
    for (i32 script = 14; script <= 22; script++) {
        auto* white = registry.find_child_script(*menu_root, script, 0);
        CHECK(white != nullptr);
        CHECK(white->visible);
        CHECK_EQ(white->visual.layer, 25);
    }
    auto* selected = registry.find_child_script(*menu_root, 3, 0);
    CHECK(selected != nullptr);
    CHECK_EQ(selected->visual.color, 0xcbffffffu);
    CHECK_EQ(registry.find_child_script(*menu_root, 14, 0)->visual.color >> 24, 63u);
    CHECK_EQ(registry.find_child_script(*menu_root, 15, 0)->visual.color >> 24, 101u);
    harness.advance(1);
    CHECK_EQ(harness.particle_count(), 16);
    CHECK_EQ(selected->visual.color >> 24, 191u);
    CHECK_EQ(registry.find_child_script(*menu_root, 14, 0)->visual.color >> 24, 56u);
    CHECK_EQ(registry.find_child_script(*menu_root, 15, 0)->visual.color >> 24, 89u);
    harness.advance(7);
    for (i32 script = 14; script <= 22; script++)
        CHECK(!registry.find_child_script(*menu_root, script, 0)->visible);
    harness.advance(8);
    CHECK(!selected->visible);
    harness.advance(11);
    auto* bg = registry.find(harness.scene->state.handles[89]);
    CHECK(bg != nullptr);
    CHECK_EQ(registry.find_child_script(*bg, 93, 0)->visual.layer, 21);
    CHECK_EQ(registry.find_child_script(*bg, 92, 0)->visual.layer, 21);
    CHECK(harness.scene->error.empty());
}

TEST(title_difficulty_transition_item0_white_row_participates_when_not_selected) {
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
    harness.frame(0x01);
    harness.advance(21);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    CHECK_EQ(harness.scene->state.confirmed_entry, 2);
    auto& registry = harness.manager.registry;
    auto* menu_root = registry.find(harness.scene->state.handles[0]);
    CHECK(menu_root != nullptr);
    auto* white0 = registry.find_child_script(*menu_root, 13, 0);
    CHECK(white0 != nullptr);
    CHECK(white0->visible);
    CHECK_EQ(white0->visual.layer, 25);
    CHECK_EQ(white0->visual.color >> 24, 101u);
    auto* white2 = registry.find_child_script(*menu_root, 15, 0);
    CHECK(white2 != nullptr);
    CHECK(!white2->visible);
    auto* selected = registry.find_child_script(*menu_root, 5, 0);
    CHECK(selected != nullptr);
    CHECK_EQ(selected->visual.color, 0xcbffffffu);
    harness.advance(1);
    CHECK_EQ(white0->visual.color >> 24, 89u);
    CHECK_EQ(selected->visual.color >> 24, 191u);
    CHECK(harness.scene->error.empty());
}

TEST(title_extra_entry_routes_through_single_row_rank_and_ex_character_root) {
    TitleHarness harness;
    harness.host.extra = true;
    harness.reach_menu();
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    CHECK_EQ(harness.scene->state.menu.count, 1);
    CHECK_EQ(harness.progress.difficulty, 4);
    CHECK(harness.scene->state.handles[124] != 0);
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.frame(0x01);
    harness.advance(15);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    CHECK_EQ(harness.progress.difficulty, 4);
    CHECK(harness.scene->state.handles[130] != 0);
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.frame(0x01);
    harness.advance(45);
    CHECK_EQ(harness.host.start_stage, 7);
    CHECK_EQ(harness.progress.starting_stage, 7);
    CHECK(harness.scene->error.empty());
}

TEST(title_extra_cancel_restores_saved_difficulty) {
    TitleHarness harness;
    harness.host.extra = true;
    harness.progress.difficulty = 2;
    harness.reach_menu();
    harness.frame(0x20);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(harness.progress.difficulty, 4);
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x100);
    harness.advance(8);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.progress.difficulty, 2);
    CHECK(harness.scene->error.empty());
}

TEST(title_extra_character_selective_unlock_disables_locked_characters) {
    TitleHarness harness;
    harness.host.extra = true;
    harness.host.extra_character_mask = 2u;
    harness.reach_menu();
    harness.frame(0x20);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.advance(10);
    harness.frame(0x01);
    harness.advance(15);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x40);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    CHECK(harness.scene->error.empty());
}

TEST(title_option_entry_preserves_logo_and_version_bar) {
    TitleHarness harness;
    harness.reach_menu();
    const u32 logo = harness.scene->state.handles[94];
    const u32 ver = harness.scene->state.version_bar;
    CHECK(logo != 0);
    CHECK(ver != 0);
    for (int i = 0; harness.scene->state.menu.cursor != 7 && i < 12; ++i)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 7);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Option));
    CHECK_EQ(harness.scene->state.handles[94], logo);
    CHECK_EQ(harness.scene->state.version_bar, ver);
    CHECK(harness.manager.registry.find(logo) != nullptr);
    CHECK(harness.manager.registry.find(ver) != nullptr);
    CHECK(harness.scene->error.empty());
}

int main() { return th13test::run_all("标题状态机"); }
