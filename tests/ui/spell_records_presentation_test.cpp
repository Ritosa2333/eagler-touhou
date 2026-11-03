#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/SpellPresentation.hpp"

#include <cstring>
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
        // st01 名单 [enemy.anm(139), st01enm.anm(19)] 两文件合并：文件槽3
        //（B 行 bank）基号 139 → cardbg 139+11=150、装饰 139+18=157。
        // synthetic_anm 的脚本表 source_id 恒 0（无文件边界沿），这里按真实
        // ANM 格式重标为文件内本地序号，合并 bank 才能按 source_id 回落沿
        // 切分文件基号（AnmResource::append/parse_entries 同式）。
        const auto stage_file = [](u32 scripts) {
            auto bytes = synthetic_anm(1, trivial_scripts(scripts));
            for (u32 i = 0; i < scripts; ++i) {
                const u32 at = 64 + 4 + i * 8;  // 脚本表项 source_id（ns=1）
                std::memcpy(bytes.data() + at, &i, 4);
            }
            return bytes;
        };
        AnmResource stage;
        const auto enemy = stage_file(139);
        const auto stage_enm = stage_file(19);
        CHECK(stage.append(enemy.data(), u32(enemy.size())));
        CHECK(stage.append(stage_enm.data(), u32(stage_enm.size())));
        CHECK(manager.adopt(4, std::move(stage)));
    }
    bool consume(const SpellEvent& event) {
        return presentation.consume(event, boss, [&](i32 sound) {
            sounds.push_back(sound); return true;
        }, 6);
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
    CHECK_EQ(sink.texts[0].style.alignment, 1);
    CHECK_EQ(sink.texts[0].style.vertical_alignment, 1);
    // 0x413080 的 AsciiInf 坐标空间 1 在原版=屏幕空间；port 的
    // coordinate_space==1 会再加 playfield (32,16)。原生 draw 捕获
    // terra-reimu-s1 f6486 的 bonus 数字在 x=363.5/y=34.5（=笔位 357/35
    // + 字形 bearing），无 (32,16)，故 port 必须用 0。
    CHECK_EQ(sink.texts[0].style.coordinate_space, 0);
    CHECK_EQ(sink.texts[0].style.color, 0x7fffffffu);
    CHECK(sink.texts[1].text == "03/07");
    CHECK(sink.texts[1].position.x == 452.0f);
    CHECK_EQ(sink.texts[1].style.alignment, 1);
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
    CHECK(!missing.consume(state.begin(0, "synthetic", 3000, 1, 0), position, {}, 6));
    CHECK(!missing.error.empty());
    SpellPresentation other(manager, {4, 2});
    CHECK(!other.consume(state.begin(14, "synthetic", 3000, 2, 0), position, {}, 6));
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
    CHECK(presentation.consume(state.begin(0, "synthetic", 3000, 1, 0), position, {}, 6));
    CHECK_EQ(manager.registry.count(), 26u);
    for (i32 n = 0; n < 320; ++n) {
        CHECK(manager.update());
        CHECK(state.step(false));
        presentation.sync(state.state(), position);
    }
    CHECK(presentation.consume(state.end(), position, {}, 6));
    for (i32 n = 0; n < 60; ++n) CHECK(manager.update());
    CHECK_EQ(manager.registry.count(), 0u);
}

TEST(spell_presentation_capture_bonus_digits_match_42a430_case0) {
    // 0x42a430 case0（捕获结算）：8 个数字 VM = ascii script 4..11
    //（0x42a54b lea eax,[edi+4]），除数 10^7 递除，sprite=270+数位
    //（0x42a5d6），前导零 0x470030 清 bit1 隐藏；≥10^6 建 script12、
    // ≥10^3 建 script13（sprite 0x11c=284 分隔符）；尾建 front script95。
    // begin(0,"",3000,stage=1,diff=0) 无衰减 → current_bonus=10^6：
    // 数位 = 0,1,0,0,0,0,0,0，仅 i=0 前导隐藏，双分隔符都在。
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    for (const auto& asset : {std::pair<i32, const char*>{0, "text.anm"},
                              {2, "ascii.anm"}, {3, "front.anm"}, {8, "effect.anm"}}) {
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
    CHECK(presentation.consume(state.begin(0, "synthetic", 3000, 1, 0), position, {}, 6));
    CHECK(presentation.consume(state.end(), position, {}, 6));
    // 句柄断言（layers 桶仅 update() 时重填，这里按创建即查）。
    const int expected_digit[8] = {0, 1, 0, 0, 0, 0, 0, 0};
    for (i32 i = 0; i < 8; ++i) {
        AnmVm* vm = manager.registry.find(presentation.bonus_digit_handle(i));
        CHECK(vm != nullptr);
        if (vm) {
            CHECK_EQ(vm->source_script, 4 + i);
            CHECK_EQ(vm->visual.sprite, 270 + expected_digit[i]);
            CHECK_EQ((vm->visual.flags & 2u) != 0u, i != 0);  // i=0 前导隐藏
        }
    }
    if (AnmVm* vm = manager.registry.find(presentation.bonus_separator_million_handle()))
        CHECK_EQ(vm->visual.sprite, 284);
    else
        CHECK(false);
    if (AnmVm* vm = manager.registry.find(presentation.bonus_separator_thousand_handle()))
        CHECK_EQ(vm->visual.sprite, 284);
    else
        CHECK(false);
    CHECK(manager.registry.find(presentation.bonus_capture_effect_handle()) != nullptr);
}

int main() { return th13test::run_all("spell_records_presentation_test"); }
