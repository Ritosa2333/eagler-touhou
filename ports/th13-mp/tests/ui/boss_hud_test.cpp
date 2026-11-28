#include "../ecl/ecl_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/BossPresentation.hpp"
#include "game/SpellPresentation.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> read_raw(const char* name) {
    std::ifstream file(name, std::ios::binary);
    if (!file) return {};
    return std::vector<u8>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

struct SoundLog {
    std::vector<i32> ids;
    bool play(i32 id) {
        ids.push_back(id);
        return true;
    }
};

struct BossHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    BossPresentation boss{manager, {3, 2, 8}};
    BossHarness() {
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(46))));
        CHECK(load_bank(manager, 3, synthetic_anm(16, trivial_scripts(244))));
    }
    AnmVm* vm(u32 handle) { return manager.registry.find(handle); }
};

i32 flags_of(AnmVm* vm) { return vm ? i32(vm->visual.flags) : -1; }
bool shown(AnmVm* vm) { return vm && (vm->visual.flags & 3u) == 3u; }

}

TEST(timer_fraction_follows_live_digit_animation_position) {
    BossHarness h;
    CHECK(h.boss.initialize());
    auto* digit = h.vm(h.boss.digit_tens_handle());
    CHECK(digit);
    digit->variables.position = {208, 51, 0};
    CHECK_EQ(h.boss.timer_text_x(), 224.f);
    CHECK_EQ(h.boss.timer_text_y(), 44.f);
    digit->variables.position.y = 23;
    CHECK_EQ(h.boss.timer_text_y(), 16.f);
}

TEST(boss_name_uses_stage_table_script_and_retires_with_boss) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.life = 1000;
    BossFrameInput input;
    input.boss = &enemy;
    input.name_script = 155;
    input.player_y = 400;
    input.boss_life = input.life_initial = 1000;
    CHECK(h.boss.update(input, {}));
    u32 name = 0;
    for (u32 i = 0; i < h.manager.registry.ordered_count(false); ++i) {
        const auto handle = h.manager.registry.ordered_handle(false, i);
        const auto* vm = h.manager.registry.find(handle);
        if (vm && vm->source_script == 155) name = handle;
    }
    CHECK(name);
    input.boss = nullptr;
    input.boss_slot_present = false;
    CHECK(h.boss.update(input, {}));
    CHECK(h.manager.update());
    CHECK(!h.manager.registry.find(name));
}

TEST(boss_timer_stays_visible_while_slot_occupied_after_capture) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 20;
    input.life_initial = 0;
    CHECK(h.boss.update(input, {}));
    CHECK(h.boss.timer_visible());
    input.boss = nullptr;
    CHECK(h.boss.update(input, {}));
    CHECK(h.boss.timer_visible());
    input.boss_slot_present = false;
    CHECK(h.boss.update(input, {}));
    CHECK(!h.boss.timer_visible());
}

TEST(boss_timer_retires_when_ecl_returns_without_explicit_unregister) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageSim sim;
    std::vector<u8> routine;
    th13test::emit(routine, 0, 512, 0, 255, 0, {0});
    th13test::emit(routine, 4, 10, 0, 255, 0, {});
    const auto data = th13test::build_scpt({{"Boss", routine}});
    CHECK_EQ(sim.program.attach(data.data(), u32(data.size())), 0);
    sim.commands.sim = &sim;
    sim.gvars.sim = &sim;
    CHECK(sim.spawn_enemy("Boss", 0, 100, 1000, nullptr, false));
    CHECK(sim.boss_slot[0] >= 0);
    BossFrameInput input;
    input.timer_seconds = 32;
    input.timer_centiseconds = 91;
    const auto update = [&] {
        input.boss = sim.boss();
        input.boss_slot_present = sim.boss_slot[0] >= 0;
        CHECK(h.boss.update(input, {}));
    };
    update();
    CHECK(h.boss.timer_visible());
    for (int frame = 0; frame < 8 && !sim.enemies.empty(); ++frame) {
        CHECK(sim.step_frame());
        update();
        CHECK_EQ(h.boss.timer_visible(), !sim.enemies.empty());
    }
    CHECK(sim.enemies.empty());
    CHECK_EQ(sim.boss_slot[0], -1);
    CHECK(!shown(h.vm(h.boss.digit_tens_handle())));
    CHECK(!shown(h.vm(h.boss.digit_ones_handle())));
}

TEST(boss_out_of_bounds_cleanup_clears_only_its_own_slot) {
    StageSim sim;
    auto departing = std::make_unique<StageEnemy>();
    departing->script_id = 11;
    departing->flags = 0x1020000u;
    departing->vel.pos[0] = -256.f;
    departing->vel.pos[1] = 64.f;
    auto survivor = std::make_unique<StageEnemy>();
    survivor->script_id = 12;
    survivor->vel.pos[1] = 100.f;
    sim.enemies.push_back(std::move(departing));
    sim.enemies.push_back(std::move(survivor));
    sim.boss_slot[0] = 11;
    sim.boss_slot[1] = 12;
    CHECK(sim.step_frame());
    CHECK_EQ(sim.enemies.size(), size_t(1));
    CHECK_EQ(sim.boss_slot[0], -1);
    CHECK_EQ(sim.boss_slot[1], 12);
    CHECK(sim.boss() == sim.enemies.front().get());
}

TEST(real_stage4_seiga_departure_retires_timer_before_setboss_minus_one) {
    struct Provider final : EclResourceProvider {
        bool read(const std::string& name, std::vector<u8>& data) override {
            data = read_raw(("artifacts/reference/raw/" + name).c_str());
            return !data.empty();
        }
        bool animation(u32, const std::string&) override { return true; }
    } provider;
    StageSim sim;
    CHECK(sim.program.load("st04.ecl", provider));
    sim.commands.sim = &sim;
    sim.gvars.sim = &sim;
    auto* enemy = sim.spawn_enemy("MBossDead", 0, 128, 1000, nullptr, false);
    CHECK(enemy);
    CHECK(sim.error().empty());
    const i32 id = enemy->script_id;
    enemy->flags |= 0x1000000u;
    sim.boss_slot[0] = id;
    BossHarness h;
    CHECK(h.boss.initialize());
    BossFrameInput input;
    input.timer_seconds = 32;
    bool departed = false;
    for (int frame = 0; frame < 260 && !departed; ++frame) {
        CHECK(sim.step_frame());
        departed = true;
        for (const auto& entry : sim.enemies)
            if (entry->script_id == id) departed = false;
        input.boss = sim.boss();
        input.boss_slot_present = sim.boss_slot[0] >= 0;
        CHECK(h.boss.update(input, {}));
        CHECK_EQ(h.boss.timer_visible(), !departed);
    }
    CHECK(departed);
    CHECK_EQ(sim.boss_slot[0], -1);
    CHECK(!h.boss.timer_visible());
}

TEST(boss_timer_digits_hidden_until_gate_opens) {
    BossHarness h;
    CHECK(h.boss.initialize());
    CHECK(h.boss.digit_tens_handle() != 0);
    CHECK(h.boss.digit_ones_handle() != 0);
    CHECK(h.boss.ring_arc_handle() == 0);
    CHECK(!h.boss.timer_visible());
}

TEST(boss_gate_opens_digits_and_swaps_digit_sprites) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.life = 1000;
    enemy.position[0] = 0;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 35;
    input.timer_frames = 30;
    input.boss_life = 700;
    input.life_initial = 1000;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.timer_visible());
    CHECK(h.boss.timer_seconds() == 35);
    CHECK(h.boss.timer_centiseconds() == 50);
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->visual.sprite, 273);
    CHECK_EQ(h.vm(h.boss.digit_ones_handle())->visual.sprite, 275);
    CHECK(shown(h.vm(h.boss.digit_tens_handle())));
    CHECK(sound.ids.empty());
}

TEST(boss_timer_warnings_fire_each_decremented_second) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.life_initial = 0;
    SoundLog sound;
    input.timer_seconds = 12;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(sound.ids.empty());
    input.timer_seconds = 9;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 1u);
    CHECK_EQ(sound.ids[0], 0xb);
    input.timer_seconds = 8;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 2u);
    CHECK_EQ(sound.ids.back(), 11);
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->pending_interrupt, 8);
    input.timer_seconds = 4;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 3u);
    CHECK_EQ(sound.ids.back(), 12);
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->pending_interrupt, 9);
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 3u);
    input.timer_seconds = 20;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 3u);
    CHECK_EQ(h.boss.warn_state(), 0);
    CHECK_EQ(h.vm(h.boss.digit_tens_handle())->pending_interrupt, 7);
    input.timer_seconds = 9;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(sound.ids.size(), 4u);
}

TEST(boss_ring_is_independent_of_timer_and_empty_boss_retires_it) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.life = 1000;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.player_y = 400;
    input.boss_life = 1000;
    input.life_initial = 1000;
    input.timer_seconds = -1;
    CHECK(h.boss.update(input, {}));
    CHECK(!h.boss.timer_visible());
    CHECK(h.boss.ring_arc_handle() != 0);
    input.boss = nullptr;
    CHECK(h.boss.update(input, {}));
    CHECK_EQ(h.boss.ring_arc_handle(), 0u);
    input.boss = &enemy;
    enemy.invulnerable = 20;
    CHECK(h.boss.update(input, {}));
    CHECK_EQ(h.boss.ring_arc_handle(), 0u);
    enemy.invulnerable = 0;
    CHECK(h.boss.update(input, {}));
    CHECK(h.boss.ring_arc_handle() != 0);
}

TEST(remaining_card_stars_use_original_front_scripts_and_exit_interrupt) {
    BossHarness h;
    CHECK(h.boss.initialize());
    BossFrameInput input;
    input.remaining_stars = 3;
    CHECK(h.boss.update(input, {}));
    std::vector<AnmVm*> stars;
    for (u32 i = 0; i < h.manager.registry.ordered_count(false); ++i)
        if (auto* vm = h.manager.registry.find(h.manager.registry.ordered_handle(false, i)))
            if (vm->resource == h.manager.resource(3) && vm->source_script >= 68 && vm->source_script <= 77)
                stars.push_back(vm);
    CHECK_EQ(stars.size(), 3u);
    input.remaining_stars = 1;
    CHECK(h.boss.update(input, {}));
    for (auto* vm : stars)
        CHECK_EQ(vm->pending_interrupt, vm->source_script == 68 ? 0 : 1);
}

TEST(boss_ring_ratio_smooths_up_drops_instantly) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.ring_arc_handle() != 0);
    CHECK(h.boss.ring_overlay_handle(0) != 0);
    CHECK(h.boss.ring_overlay_handle(1) != 0);
    CHECK(h.boss.displayed_ratio() == 0.025f);
    for (int i = 0; i < 31; i++)
        CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_NEAR(h.boss.displayed_ratio(), 0.8, 1e-4);
    input.boss_life = 500;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() == 0.5f);
    input.boss_life = 1000;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() > 0.5f);
    CHECK(h.boss.displayed_ratio() <= 0.525f);
    auto* arc = h.vm(h.boss.ring_arc_handle());
    CHECK(arc != nullptr);
    CHECK(arc->variables.rotation.x < 0.f);
    CHECK(arc->visual.translation.y == 128.f);
    auto* overlay = h.vm(h.boss.ring_overlay_handle(0));
    CHECK(overlay != nullptr);
    CHECK(overlay->visual.translation.y == 128.f);
}

TEST(boss_ratio_crosses_phase_markers) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    SoundLog sound;
    input.markers[0].ratio = 0.5f;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(!shown(h.vm(h.boss.star_handles()[0])));
    input.boss_life = 400;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(!shown(h.vm(h.boss.star_handles()[0])));
    input.boss_life = 1000;
    for (int i = 0; i < 40; i++)
        CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() >= 0.5f);
    CHECK(shown(h.vm(h.boss.star_handles()[0])));
    input.boss_life = 500;
    CHECK(h.boss.update(input, {}));
    CHECK(!shown(h.vm(h.boss.star_handles()[0])));
}

TEST(boss_stars_created_and_hidden_when_passed) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 10;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    input.markers[0].ratio = 0.25f;
    input.markers[1].ratio = 0.5f;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() == 0.025f);
    CHECK(!shown(h.vm(h.boss.star_handles()[0])));
    CHECK(!shown(h.vm(h.boss.star_handles()[1])));
    input.boss_life = 500;
    for (int i = 0; i < 20; i++)
        CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.displayed_ratio() >= 0.5f);
    CHECK(shown(h.vm(h.boss.star_handles()[0])));
    CHECK(!shown(h.vm(h.boss.star_handles()[1])));
    input.boss_life = 900;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    auto* star = h.vm(h.boss.star_handles()[0]);
    CHECK(star != nullptr);
    CHECK(shown(star));
    BossHarness fresh;
    CHECK(fresh.boss.initialize());
    BossFrameInput fi;
    StageEnemy enemy2;
    enemy2.position[0] = 0;
    enemy2.position[1] = 128;
    fi.boss = &enemy2;
    fi.timer_seconds = 60;
    fi.life_initial = 1000;
    fi.boss_life = 1000;
    fi.markers[0].ratio = 0.25f;
    SoundLog fsound;
    for (int frame = 0; frame < 20; ++frame)
        CHECK(fresh.boss.update(fi, [&](i32 id) { return fsound.play(id); }));
    auto* star0 = fresh.vm(fresh.boss.star_handles()[0]);
    CHECK(star0 != nullptr);
    CHECK(shown(star0));
    const float t = star0->variables.rotation.z;
    CHECK_NEAR(double(t), 1.5707963, 1e-4);
    CHECK_NEAR(double(star0->visual.translation.y), double(128.f + 56.f * std::cos(t)), 1e-3);
    CHECK_NEAR(double(star0->visual.translation.x), double(-56.f * std::sin(t)), 1e-3);
}

TEST(boss_proximity_hides_ring_with_hysteresis) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 0;
    enemy.position[1] = 0;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    input.player_x = 200;
    input.player_y = 300;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.ring_arc_handle() != 0);
    CHECK(!h.boss.proximity_hidden());
    const u32 arc = h.boss.ring_arc_handle();
    const auto stars = h.boss.star_handles();
    input.player_x = 40;
    input.player_y = 0;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.proximity_hidden());
    CHECK_EQ(h.boss.ring_arc_handle(), arc);
    CHECK_EQ(h.vm(arc)->pending_interrupt, 3);
    for (u32 star : stars)
        CHECK_EQ(h.vm(star)->pending_interrupt, 3);
    input.player_x = 90;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(h.boss.proximity_hidden());
    input.player_x = 100;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK(!h.boss.proximity_hidden());
    CHECK_EQ(h.boss.ring_arc_handle(), arc);
    CHECK_EQ(h.vm(arc)->pending_interrupt, 2);
    for (u32 star : stars)
        CHECK_EQ(h.vm(star)->pending_interrupt, 2);
}

TEST(boss_marker_glyph_tracks_horizontal_distance) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 30;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 30;
    input.player_x = 0;
    SoundLog sound;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(h.boss.marker_glyph(), 0x40 + 90);
    enemy.position[0] = 100;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(h.boss.marker_glyph(), 0xff);
    enemy.position[0] = -200;
    CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(h.boss.marker_glyph(), 0);
    auto* marker = h.vm(h.boss.marker_handle());
    CHECK(marker != nullptr);
    CHECK(marker->visual.translation.x == 24.f);
    CHECK(marker->visual.translation.y == 480.f);
    CHECK_EQ(marker->source_script, 111);
    CHECK_EQ(marker->visual.color >> 24, 0u);
}

TEST(boss_marker_flashes_by_remaining_phase_hp_and_resets_after_phase_change) {
    BossHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.phase_life_remaining = 800;
    BossFrameInput input;
    input.boss = &enemy;
    input.dialog_active = true;
    SoundLog sound;
    auto update = [&] { CHECK(h.boss.update(input, [&](i32 id) { return sound.play(id); })); };
    update();
    auto* marker = h.vm(h.boss.marker_handle());
    CHECK(marker);
    if (!marker) return;
    CHECK(shown(marker));
    CHECK_EQ(marker->pending_interrupt, 0);
    enemy.phase_life_remaining = 699;
    update();
    CHECK_EQ(marker->pending_interrupt, 7);
    enemy.phase_life_remaining = 399;
    update();
    CHECK_EQ(marker->pending_interrupt, 8);
    enemy.phase_life_remaining = 199;
    update();
    CHECK_EQ(marker->pending_interrupt, 9);
    enemy.phase_life_remaining = 1500;
    update();
    CHECK_EQ(marker->pending_interrupt, 10);
    CHECK(sound.ids.empty());
    input.boss = nullptr;
    update();
    CHECK(!shown(marker));
}

TEST(spell_card_lifecycle_bonus_and_sounds) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(151))));
    SpellPresentation spell{manager, {4}};
    SoundLog sound;
    i32 awarded = 0;
    const float pos[3] = {0, 128, 0};
    CHECK(spell.begin(3, "符卡", 60, 1, 1, pos, [&](i32 id) { return sound.play(id); }));
    CHECK(spell.active());
    CHECK(spell.captured());
    CHECK_EQ(spell.bonus_base(), 2000000);
    CHECK_EQ(spell.bonus_display(), 2000000);
    CHECK_EQ(spell.duration_seconds(), 60);
    CHECK_EQ(sound.ids.size(), 1u);
    CHECK_EQ(sound.ids[0], 0x21);
    CHECK(spell.background_handle() != 0);
    CHECK(spell.end(true, false, [&](i32 id) { return sound.play(id); }, [&](i32 amount) { awarded += amount; }));
    CHECK_EQ(awarded, 200000);
    CHECK_EQ(sound.ids.size(), 2u);
    CHECK_EQ(sound.ids[1], 0x2e);
    CHECK(!spell.active());
    CHECK(spell.begin(4, "终符", 99, 6, 4, pos, [&](i32 id) { return sound.play(id); }));
    CHECK_EQ(spell.bonus_base(), 10000000);
    CHECK(spell.end(true, true, [&](i32 id) { return sound.play(id); }, [&](i32 amount) { awarded += amount; }));
    CHECK_EQ(awarded, 1200000);
    CHECK_EQ(sound.ids.size(), 5u);
    CHECK_EQ(sound.ids[2], 0x21);
    CHECK_EQ(sound.ids[3], 0x2e);
    CHECK_EQ(sound.ids[4], 0x45);
}

TEST(spell_break_capture_blocks_award) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(151))));
    SpellPresentation spell{manager, {4}};
    SoundLog sound;
    i32 awarded = 0;
    const float pos[3] = {0, 0, 0};
    CHECK(spell.begin(0, "卡", 30, 1, 0, pos, [&](i32 id) { return sound.play(id); }));
    spell.break_capture();
    CHECK(!spell.captured());
    CHECK(spell.end(true, false, [&](i32 id) { return sound.play(id); }, [&](i32 amount) { awarded += amount; }));
    CHECK_EQ(awarded, 0);
    CHECK(sound.ids.size() <= 1u);
    const float again[3] = {0, 0, 0};
    CHECK(spell.begin(1, "乙", 30, 1, 0, again, [&](i32 id) { return sound.play(id); }));
    CHECK(!spell.begin(2, "丙", 30, 1, 0, again, [&](i32 id) { return sound.play(id); }));
}

TEST(real_resources_match_ring_and_digit_script_map) {
    auto ascii = read_raw("artifacts/reference/raw/ascii.anm");
    CHECK(!ascii.empty());
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(manager.load(2, ascii.data(), u32(ascii.size())));
    CHECK(manager.sprite_fallback(2));
    auto front = read_raw("artifacts/reference/raw/front.anm");
    CHECK(!front.empty());
    CHECK(manager.load(3, front.data(), u32(front.size())));
    BossPresentation boss{manager, {3, 2, 8}};
    CHECK(boss.initialize());
    StageEnemy enemy;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 42;
    input.timer_frames = 0;
    input.life_initial = 0;
    SoundLog sound;
    CHECK(boss.update(input, [&](i32 id) { return sound.play(id); }));
    auto* tens = manager.registry.find(boss.digit_tens_handle());
    auto* ones = manager.registry.find(boss.digit_ones_handle());
    CHECK(tens != nullptr && ones != nullptr);
    CHECK_EQ(tens->visual.sprite, 274);
    CHECK_EQ(ones->visual.sprite, 272);
    CHECK(tens->variables.position.x == 208.f);
    CHECK(tens->variables.position.y == 39.f);
    CHECK(ones->variables.position.x == 220.f);

    input.life_initial = 1000;
    input.boss_life = 1000;
    input.markers[0].ratio = 0.5f;
    CHECK(boss.update(input, [&](i32 id) { return sound.play(id); }));
    auto* arc = manager.registry.find(boss.ring_arc_handle());
    CHECK(arc != nullptr);
    CHECK_EQ(arc->source_script, 240);
    CHECK_EQ(arc->visual.sprite, 150);
    CHECK_EQ(arc->visual.layer, 17);
    CHECK_EQ(arc->variables.integers[0], 38);
    auto* overlay = manager.registry.find(boss.ring_overlay_handle(0));
    CHECK(overlay != nullptr);
    CHECK_EQ(overlay->source_script, 241);
    CHECK_EQ(overlay->visual.sprite, 151);
    CHECK_EQ(overlay->variables.integers[0], 38);
    CHECK_NEAR(double(overlay->visual.scale.y), 58.0, 1e-4);
    auto* star = manager.registry.find(boss.star_handles()[0]);
    CHECK(star != nullptr);
    CHECK_EQ(star->source_script, 243);
    CHECK_EQ(star->visual.sprite, 152);
    for (int frame = 0; frame < 30; ++frame)
        CHECK(manager.update());
    const u32 visible_alpha = arc->visual.color >> 24;
    input.player_y = 128;
    CHECK(boss.update(input, {}));
    for (int frame = 0; frame < 30; ++frame)
        CHECK(manager.update());
    const u32 near_alpha = arc->visual.color >> 24;
    CHECK(near_alpha < visible_alpha);
    input.player_y = 300;
    CHECK(boss.update(input, {}));
    for (int frame = 0; frame < 30; ++frame)
        CHECK(manager.update());
    CHECK((arc->visual.color >> 24) > near_alpha);
}

TEST(real_resources_spell_end_restores_digit_row) {
    auto ascii = read_raw("artifacts/reference/raw/ascii.anm");
    CHECK(!ascii.empty());
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(manager.load(2, ascii.data(), u32(ascii.size())));
    CHECK(manager.sprite_fallback(2));
    auto front = read_raw("artifacts/reference/raw/front.anm");
    CHECK(!front.empty());
    CHECK(manager.load(3, front.data(), u32(front.size())));
    BossPresentation boss{manager, {3, 2, 8}};
    CHECK(boss.initialize());
    StageEnemy enemy;
    enemy.position[1] = 128;
    enemy.life = 100000.f;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 42;
    input.timer_frames = 0;
    input.life_initial = 0;
    CHECK(boss.update(input, [](i32) { return true; }));
    auto* tens = manager.registry.find(boss.digit_tens_handle());
    auto* ones = manager.registry.find(boss.digit_ones_handle());
    CHECK(tens != nullptr && ones != nullptr);
    CHECK(tens->variables.position.y == 39.f);

    boss.spell_started();
    for (int frame = 0; frame < 70; ++frame)
        CHECK(manager.update());
    CHECK_NEAR(double(tens->variables.position.y), 51.0, 1e-3);
    CHECK_NEAR(double(ones->variables.position.y), 51.0, 1e-3);
    CHECK_NEAR(double(boss.timer_text_y()), 44.0, 1e-3);

    boss.spell_ended();
    for (int frame = 0; frame < 70; ++frame)
        CHECK(manager.update());
    CHECK_NEAR(double(tens->variables.position.y), 23.0, 1e-3);
    CHECK_NEAR(double(ones->variables.position.y), 23.0, 1e-3);
    CHECK_NEAR(double(boss.timer_text_y()), 16.0, 1e-3);
}

int main() { return th13test::run_all("Boss HUD"); }
