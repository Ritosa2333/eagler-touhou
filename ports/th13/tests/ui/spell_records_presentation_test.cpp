#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/SpellPresentation.hpp"

#include <fstream>

using namespace th13;
using namespace th13::test;
namespace {
struct Fixture {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SpellPresentation presentation{manager};
    SpellState state;
    std::vector<i32> sounds;
    float boss[3] = {0, 128, 0};
    Fixture() {
        CHECK(load_bank(manager, 0, synthetic_anm(1, trivial_scripts(75))));
        CHECK(load_bank(manager, 2, synthetic_anm(1, trivial_scripts(2))));
        auto effects = trivial_scripts(21);
        effects[13] = {spawn(11), spawn(12), wait(), stop()};
        CHECK(load_bank(manager, 8, synthetic_anm(1, effects)));
        CHECK(load_bank(manager, 4, synthetic_anm(1, trivial_scripts(158))));
    }
    bool consume(const SpellEvent& event) {
        return presentation.consume(event, boss, [&](i32 sound) {
            sounds.push_back(sound); return true;
        });
    }
};
struct TextSink : HudDrawServices {
    std::vector<HudTextDraw> texts;
    bool hud_text(const HudTextDraw& text) override { texts.push_back(text); return true; }
};
std::vector<u8> read_asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}
}

TEST(spell_presentation_consumes_event_snapshots_and_original_script_roles) {
    Fixture fixture;
    const auto event = fixture.state.begin(0, "synthetic", 3000, 1, 0);
    CHECK(fixture.consume(event));
    CHECK(fixture.presentation.active());
    CHECK_EQ(fixture.sounds.size(), size_t(1));
    CHECK_EQ(fixture.sounds[0], 0x21);
    for (i32 i = 0; i < 3; ++i) CHECK(fixture.presentation.display_handle(i) != 0);
    auto* first = fixture.manager.registry.find(fixture.presentation.display_handle(0));
    auto* name = fixture.manager.registry.find(fixture.presentation.display_handle(1));
    auto* bonus = fixture.manager.registry.find(fixture.presentation.display_handle(2));
    CHECK(first && name && bonus);
    CHECK_EQ(first->source_script, 0);
    CHECK(first->resource == fixture.manager.resource(2));
    CHECK_EQ(name->source_script, 74);
    CHECK(name->resource == fixture.manager.resource(0));
    CHECK_EQ(bonus->source_script, 1);
    auto* background = fixture.manager.registry.find(fixture.presentation.background_handle());
    CHECK(background != nullptr);
    CHECK_EQ(background->source_script, 13);
    CHECK(background->resource == fixture.manager.resource(8));
    CHECK_EQ(fixture.manager.registry.find_child_script(*background, 11, 0)->variables.integers[2], 3000);
    CHECK_EQ(fixture.manager.registry.find_child_script(*background, 12, 0)->variables.integers[2], 3000);
    CHECK_EQ(fixture.manager.registry.find(fixture.presentation.stage_background_handle())->source_script, 150);
    CHECK_EQ(fixture.manager.registry.find(fixture.presentation.decoration_handle())->source_script, 157);
    for (i32 i = 0; i < 301; ++i) CHECK(fixture.state.step(false));
    fixture.boss[0] = 100;
    fixture.presentation.sync(fixture.state.state(), fixture.boss);
    CHECK_EQ(fixture.presentation.bonus_base(), 999720);
    CHECK_EQ(fixture.presentation.snapshot().current_bonus, fixture.state.state().current_bonus);
    CHECK(background->visual.translation.x == 5.0f); // original 0.05 smoothing
    CHECK_EQ(event.state.current_bonus, 1000000); // value event is not live state
}

TEST(spell_presentation_end_interrupts_displays_and_timeout_sound_is_independent) {
    Fixture fixture;
    CHECK(fixture.consume(fixture.state.begin(0, "synthetic", 3000, 1, 0)));
    u32 display[3];
    for (i32 i = 0; i < 3; ++i) display[i] = fixture.presentation.display_handle(i);
    const u32 stage = fixture.presentation.stage_background_handle();
    const u32 effect = fixture.presentation.background_handle();
    const u32 decoration = fixture.presentation.decoration_handle();
    for (i32 i = 0; i < 60; ++i) CHECK(fixture.state.step(false));
    fixture.state.phase_timeout(false);
    const auto event = fixture.state.end();
    CHECK(!event.captured());
    CHECK(fixture.consume(event));
    CHECK(!fixture.presentation.active());
    CHECK_EQ(fixture.sounds.size(), size_t(2));
    CHECK_EQ(fixture.sounds[1], 0x45);
    for (u32 handle : display) {
        const auto* vm = fixture.manager.registry.find(handle);
        CHECK(vm != nullptr);
        CHECK_EQ(vm->pending_interrupt, 1);
        CHECK(!(vm->visual.render_flags & 0x20u));
    }
    CHECK(fixture.manager.registry.find(stage)->visual.render_flags & 0x20u);
    CHECK(fixture.manager.registry.find(effect)->visual.render_flags & 0x20u);
    CHECK(!(fixture.manager.registry.find(decoration)->visual.render_flags & 0x20u));
}

TEST(spell_presentation_numeric_text_uses_live_bonus_mode_counters_and_vm_alpha) {
    Fixture fixture;
    CHECK(fixture.consume(fixture.state.begin(0, "synthetic", 3000, 1, 0)));
    auto* bonus = fixture.manager.registry.find(fixture.presentation.display_handle(2));
    bonus->visual.color = 0x7f123456;
    TextSink sink;
    CHECK(fixture.presentation.draw(sink, 3, 7));
    CHECK_EQ(sink.texts.size(), size_t(2));
    CHECK(sink.texts[0].text == " 1000000");
    CHECK(sink.texts[0].position.x == 357.0f && sink.texts[0].position.y == 35.0f);
    CHECK_EQ(sink.texts[0].style.font, 2);
    CHECK_EQ(sink.texts[0].style.alignment, 2);
    CHECK_EQ(sink.texts[0].style.coordinate_space, 1);
    CHECK_EQ(sink.texts[0].style.color, 0x7fffffffu);
    CHECK(sink.texts[1].text == "03/07");
    CHECK(sink.texts[1].position.x == 452.0f);
    sink.texts.clear();
    CHECK(fixture.presentation.draw(sink, 99, 100));
    CHECK(sink.texts[1].text == "99/99+");
    sink.texts.clear();
    CHECK(fixture.presentation.draw(sink, 100, 99999));
    CHECK(sink.texts[1].text == "MASTER");
    for (i32 i = 0; i < 60; ++i) CHECK(fixture.state.step(false));
    fixture.state.player_hit(false);
    fixture.presentation.sync(fixture.state.state());
    sink.texts.clear();
    CHECK(fixture.presentation.draw(sink, 0, 1));
    CHECK(sink.texts[0].text == "$");
    CHECK(sink.texts[0].position.x == 371.0f);
}

TEST(spell_presentation_missing_assets_and_unmapped_stage_fail_explicitly) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    SpellState state;
    SpellPresentation missing(manager);
    const float position[3] = {};
    CHECK(!missing.consume(state.begin(0, "synthetic", 3000, 1, 0), position, {}));
    CHECK(!missing.error.empty());
    SpellPresentation other(manager, {4, 2});
    CHECK(!other.consume(state.begin(14, "synthetic", 3000, 2, 0), position, {}));
    CHECK(!other.error.empty());
}

TEST(spell_presentation_visibility_follows_shared_flag_transitions_via_interrupts) {
    Fixture fixture;
    CHECK(fixture.consume(fixture.state.begin(0, "synthetic", 3000, 1, 0)));
    for (i32 i = 0; i < 120; ++i) CHECK(fixture.state.step(false));
    fixture.state.update_visibility(80);
    fixture.presentation.sync(fixture.state.state());
    for (i32 i = 0; i < 3; ++i)
        CHECK_EQ(fixture.manager.registry.find(fixture.presentation.display_handle(i))->pending_interrupt, 3);
    fixture.state.update_visibility(130);
    fixture.presentation.sync(fixture.state.state());
    for (i32 i = 0; i < 3; ++i)
        CHECK_EQ(fixture.manager.registry.find(fixture.presentation.display_handle(i))->pending_interrupt, 2);
}

TEST(spell_presentation_real_stage1_assets_create_tick_and_complete_exit) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    for (const auto& asset : {std::pair<i32, const char*>{0, "text.anm"},
                              {2, "ascii.anm"}, {8, "effect.anm"}}) {
        const auto bytes = read_asset(asset.second);
        CHECK(!bytes.empty());
        CHECK(manager.load(asset.first, bytes.data(), u32(bytes.size())));
        if (asset.first == 2) CHECK(manager.sprite_fallback(2));
    }
    AnmResource stage;
    for (const char* name : {"enemy.anm", "st01enm.anm"}) {
        const auto bytes = read_asset(name);
        CHECK(!bytes.empty());
        CHECK(stage.append(bytes.data(), u32(bytes.size())));
    }
    CHECK(manager.adopt(4, std::move(stage)));
    SpellPresentation presentation(manager);
    SpellState state;
    const float position[3] = {0, 128, 0};
    CHECK(presentation.consume(state.begin(0, "synthetic", 3000, 1, 0), position, {}));
    CHECK_EQ(manager.registry.count(), 26u);
    for (i32 n = 0; n < 320; ++n) {
        CHECK(manager.update());
        CHECK(state.step(false));
        presentation.sync(state.state(), position);
    }
    CHECK(presentation.consume(state.end(), position, {}));
    for (i32 n = 0; n < 60; ++n) CHECK(manager.update());
    CHECK_EQ(manager.registry.count(), 0u);
}

int main() { return th13test::run_all("spell_records_presentation_test"); }
