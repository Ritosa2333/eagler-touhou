#include "../harness/test_framework.hpp"
#include "game/TitleScene.hpp"
#include "ui_test_fixtures.hpp"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

bool read_file(const std::string& path, std::vector<u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    if (size < 0) {
        std::fclose(file);
        return false;
    }
    out.resize(size_t(size));
    const bool ok = std::fread(out.data(), 1, out.size(), file) == out.size();
    std::fclose(file);
    return ok;
}

struct SpellFlowHost final : TitleScenePlatform {
    SessionState& progress;
    AnmManager& animations;
    explicit SpellFlowHost(SessionState& p, AnmManager& m) : progress(p), animations(m) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool title_exit(i32) override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = animations.create(8, 0, -1, 0, {});
        return animation != 0;
    }
    bool start_game(i32) override { return true; }
    bool record_menu_views() const override { return true; }
    bool rasterize_title_text(AnmVm&, u32, const std::string&, AnmTextAlignment) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    i32 stage_count() const override { return kSpellPracticeStages; }
    bool stage_available(i32 stage) const override { return stage >= 1 && stage <= 7; }
    std::string stage_name(i32) const override { return "stage"; }
    i32 spell_count(i32 stage) const override { return kSpellPracticeSlots[stage - 1]; }
    bool spell_unlocked(i32 stage, i32 slot) const override { return stage == 1 && slot == 0; }
    std::string spell_name(i32, i32) const override { return "spell"; }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override { return stage * 100 + slot * 10 + difficulty; }
    i32 spell_rank(i32, i32, i32 difficulty) const override { return difficulty; }
    bool spell_variant_available(i32, i32, i32) const override { return true; }
    std::string spell_variant_name(i32, i32, i32) const override { return "variant"; }
    bool spell_record(i32, i32, SpellPracticeRecord& record) const override {
        record = SpellPracticeRecord{};
        record.encountered = true;
        return true;
    }
    i32 character_count() const override { return kSpellPracticeCharacters; }
    std::string character_name(i32) const override { return "char"; }
};

struct SpellFlowHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings settings;
    FrameScheduler scheduler;
    SpellFlowHost host{progress, manager};
    std::array<FrameCallback, 2> updates{};
    std::unique_ptr<TitleScene> scene;

    SpellFlowHarness() {
        for (i32 i = 0; i < 2; ++i) {
            updates[u32(i)].owner = &manager;
            updates[u32(i)].enabled = true;
            updates[u32(i)].run = i == 0 ? +[](void* p) { return static_cast<AnmManager*>(p)->update(true) ? 1 : 5; }
                                         : +[](void* p) { return static_cast<AnmManager*>(p)->update(false) ? 1 : 5; };
            CHECK(scheduler.add(updates[u32(i)], FramePass::Update, i == 0 ? 9 : 34) >= 0);
        }
        std::vector<u8> title_bytes, ascii_bytes;
        CHECK(read_file("artifacts/reference/raw/title.anm", title_bytes));
        CHECK(read_file("artifacts/reference/raw/ascii.anm", ascii_bytes));
        CHECK(load_bank(manager, 16, title_bytes));
        CHECK(load_bank(manager, 2, ascii_bytes));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, settings, manager, host, scheduler);
        scene->state.return_reason = 0;
    }
    void frame(u32 pressed = 0) {
        scene->controls({0, pressed, 0, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scene->error.empty());
    }
    void advance(i32 count) {
        for (i32 i = 0; i < count; ++i)
            frame();
    }
    void ready_main() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    const TitleSpellPracticeMenu* menu() const { return scene->spell_practice_menu(); }
    i32 level() const { return menu() ? i32(menu()->level()) : -1; }
};

bool on_screen(const AnmVm* vm) {
    return vm && (vm->visual.flags & 3u) == 3u && !(vm->visual.render_flags & 0x20u) &&
           ((vm->visual.color >> 24) != 0u || (vm->visual.secondary_color >> 24) != 0u);
}

constexpr u32 kLayerBuckets = 33;

std::vector<i32> drawn_scripts(const AnmManager& manager, i32 first, i32 last) {
    std::vector<i32> scripts;
    for (u32 bucket = 0; bucket < kLayerBuckets; ++bucket)
        for (const AnmVm* vm : manager.registry.layer(bucket))
            if (on_screen(vm) && vm->source_script >= first && vm->source_script <= last &&
                std::find(scripts.begin(), scripts.end(), vm->source_script) == scripts.end())
                scripts.push_back(vm->source_script);
    std::sort(scripts.begin(), scripts.end());
    return scripts;
}

u32 drawn_alpha(const AnmManager& manager, i32 script) {
    u32 alpha = 0;
    for (u32 bucket = 0; bucket < kLayerBuckets; ++bucket)
        for (const AnmVm* vm : manager.registry.layer(bucket))
            if (on_screen(vm) && vm->source_script == script) alpha = std::max(alpha, vm->visual.color >> 24);
    return alpha;
}

bool same_scripts(const std::vector<i32>& actual, const std::vector<i32>& expected) {
    if (actual == expected) return true;
    std::printf("    上屏 {");
    for (i32 script : actual)
        std::printf(" %d", script);
    std::printf(" } 期望 {");
    for (i32 script : expected)
        std::printf(" %d", script);
    std::printf(" }\n");
    return false;
}

}

TEST(spell_practice_esc_recedes_exactly_one_level_per_cancel) {
    SpellFlowHarness h;
    h.ready_main();
    h.frame(0x20);
    h.frame();
    h.frame(0x20);
    h.frame();
    CHECK_EQ(h.scene->state.menu.cursor, 3);
    h.frame(1);
    h.advance(45);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::SpellPractice));
    CHECK(h.menu() != nullptr);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Stage));

    h.frame(1);
    h.advance(45);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Spell));
    h.frame(1);
    h.advance(45);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Difficulty));

    h.frame(0x100);
    for (i32 i = 0; i < 12; ++i) {
        h.frame();
        CHECK(h.level() == i32(SpellPracticeLevel::Difficulty) || h.level() == i32(SpellPracticeLevel::Spell));
    }
    h.advance(40);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Spell));

    h.frame(0x100);
    h.advance(52);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Stage));
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::SpellPractice));

    h.frame(0x100);
    h.advance(52);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK(h.menu() == nullptr);
}

TEST(spell_run_return_cascade_and_esc_chain_leave_no_stale_rows) {
    SpellFlowHarness h;
    h.progress.run_mode_flags = (h.progress.run_mode_flags & ~0x70u) | 0x20u;
    h.progress.starting_stage = 1;
    h.progress.selected_spell_id = 100;
    h.progress.character = 0;
    h.progress.difficulty = 0;
    h.scene->state.return_reason = 5;
    h.frame();
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::SpellPractice));
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Difficulty));
    CHECK(same_scripts(drawn_scripts(h.manager, 89, 93), {92, 93}));
    CHECK_EQ(drawn_alpha(h.manager, 92), 4u);
    CHECK_EQ(drawn_alpha(h.manager, 93), 4u);

    h.advance(50);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Difficulty));
    CHECK(same_scripts(drawn_scripts(h.manager, 89, 93), {92, 93}));
    CHECK_EQ(drawn_alpha(h.manager, 92), 79u);
    CHECK_EQ(drawn_alpha(h.manager, 93), 79u);
    CHECK(same_scripts(drawn_scripts(h.manager, 189, 202), {189}));
    CHECK(same_scripts(drawn_scripts(h.manager, 203, 228), {203}));
    CHECK_EQ(drawn_alpha(h.manager, 189), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 203), 255u);
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 186), {182, 183, 184, 185, 186}));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {229, 230, 231, 232, 233}));
    CHECK_EQ(drawn_alpha(h.manager, 103), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 109), 0u);

    h.frame(0x100);
    h.advance(79);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Spell));
    CHECK(same_scripts(drawn_scripts(h.manager, 89, 93), {92, 93}));
    CHECK_EQ(drawn_alpha(h.manager, 92), 199u);
    CHECK_EQ(drawn_alpha(h.manager, 93), 199u);
    CHECK(same_scripts(drawn_scripts(h.manager, 203, 228), {203, 217, 218, 219}));
    CHECK(same_scripts(drawn_scripts(h.manager, 189, 202), {189}));
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 186), {182, 183, 184, 185, 186}));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {229, 230, 231, 232, 233}));
    CHECK_EQ(drawn_alpha(h.manager, 103), 255u);

    h.frame(0x100);
    i32 settle = 79;
    for (; settle > 0 && h.level() != i32(SpellPracticeLevel::Stage); --settle)
        h.frame();
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Stage));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {}));

    h.advance(settle);
    CHECK(same_scripts(drawn_scripts(h.manager, 89, 93), {92, 93}));
    CHECK_EQ(drawn_alpha(h.manager, 92), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 93), 255u);
    CHECK(same_scripts(drawn_scripts(h.manager, 189, 202), {189, 197, 198, 199, 200, 201, 202}));
    CHECK(same_scripts(drawn_scripts(h.manager, 203, 228), {}));
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 186), {}));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {}));
    CHECK_EQ(drawn_alpha(h.manager, 109), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 103), 0u);

    h.frame(0x100);
    h.advance(129);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK(h.menu() == nullptr);
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 242), {}));
    CHECK_EQ(drawn_alpha(h.manager, 103), 0u);
    CHECK_EQ(drawn_alpha(h.manager, 109), 0u);
    CHECK(!drawn_scripts(h.manager, 89, 100).empty());
    CHECK_EQ(drawn_alpha(h.manager, 90), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 91), 255u);
}

int main() { return th13test::run_all("Spell practice ESC flow"); }
