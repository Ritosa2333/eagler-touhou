#include "ui_test_fixtures.hpp"
#include "../ecl/ecl_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/GameHud.hpp"
#include "game/ReplaySession.hpp"
#include "game/StageBattle.hpp"
#include <cstdlib>
#include <fstream>
#include <memory>

using namespace th13;
using namespace th13::test;

namespace {
struct Provider : EclResourceProvider {
    std::vector<u8> ecl;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> body;
        th13test::emit(body, 0, 23, 0, 255, 0, {10000});
        th13test::emit(body, 0, 10, 0, 255, 0, {});
        ecl = th13test::build_scpt({{"main", body}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "hud.ecl") return false;
        out = ecl;
        return true;
    }
    bool animation(u32, const std::string& name) override { names->push_back(name); return true; }
};

struct Rig : StageBattleServices {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager animations{random, environment};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    GameHud hud{animations, progress, player, score, 3, 2};
    StageBattle battle;
    Provider provider;
    bool reject_refresh = false;

    Rig() {
        const char* raw = std::getenv("TH13_TEST_RAW");
        if (raw) {
            for (const auto& entry : {std::pair{0, "text.anm"}, {2, "ascii.anm"}, {3, "front.anm"}}) {
                std::ifstream file(std::string(raw) + "/" + entry.second, std::ios::binary);
                std::vector<u8> bytes{std::istreambuf_iterator<char>(file), {}};
                CHECK(load_bank(animations, entry.first, bytes));
            }
            CHECK(animations.sprite_fallback(2));
        } else {
            auto scripts = trivial_scripts(176);
            // front.anm 29..36 / 37..44: full=label2, empty=3, pieces=7..12.
            for (u32 i = 29; i < 45; ++i) {
                const i32 base = i < 37 ? 12 : 18;
                scripts[i] = {sprite(base), wait(), label(2), sprite(base + 5), wait(),
                              label(3), sprite(base), wait()};
                for (i32 piece = 0; piece <= 5; ++piece) {
                    scripts[i].push_back(label(7 + piece));
                    scripts[i].push_back(sprite(base + piece));
                    scripts[i].push_back(wait());
                }
                scripts[i].push_back(stop());
            }
            CHECK(load_bank(animations, 3, synthetic_anm(302, scripts)));
            CHECK(load_bank(animations, 2, synthetic_anm(302, trivial_scripts(46))));
        }
        provider.names = &battle.sim.anim_list;
        CHECK(battle.load(provider, "hud.ecl", 2, *this, progress, player, score));
        battle.sim.enemies.clear();
        battle.set_player_pos(0, 200);
        battle.player_phase = StageBattle::PlayerPhase::Active;
        battle.invulnerable = 9999;
        CHECK(hud.initialize({}));
        tick_animations();
    }
    bool life_hud(i32 lives, i32 pieces) override { return !reject_refresh && hud.life(lives, pieces); }
    bool bomb_hud(i32 bombs, i32 pieces) override { return !reject_refresh && hud.bombs(bombs, pieces); }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    void tick_animations() { for (int i = 0; i < 45; ++i) CHECK(animations.update()); }
    i32 icon(bool bomb, u32 slot) {
        auto* vm = animations.registry.find((bomb ? hud.bomb_icons : hud.life_icons)[slot]);
        CHECK(vm != nullptr);
        return vm ? vm->visual.sprite : -1;
    }
    void collect(i32 kind) {
        battle.item_manager.spawn_item(0, 200, 0, kind, 0, 0);
        CHECK(battle.step({}));
        tick_animations();
    }
};
}

TEST(item_pickups_refresh_icons_without_recreating_the_stage) {
    auto r = std::make_unique<Rig>();
    const auto life_handles = r->hud.life_icons, bomb_handles = r->hud.bomb_icons;
    CHECK_EQ(r->icon(false, 2), 12);
    r->collect(4); r->collect(4);
    CHECK_EQ(r->player.life_pieces, 2);
    CHECK_EQ(r->icon(false, 2), 13);  // 2/8 -> label8.
    for (int i = 0; i < 6; ++i) r->collect(4);
    CHECK_EQ(r->player.extra_lives, 3);
    CHECK_EQ(r->progress.life_piece_threshold_index, 1);
    CHECK_EQ(r->icon(false, 2), 17);
    CHECK_EQ(r->icon(false, 3), 12);
    r->collect(6);
    CHECK_EQ(r->player.extra_lives, 4);
    CHECK_EQ(r->icon(false, 3), 17);
    r->collect(5);
    CHECK_EQ(r->icon(true, 2), 19);  // 2/8 -> label8.
    for (int i = 0; i < 3; ++i) r->collect(5);
    CHECK_EQ(r->player.bombs, 3);
    CHECK_EQ(r->icon(true, 2), 23);
    CHECK_EQ(r->icon(true, 3), 18);
    r->collect(7);
    CHECK_EQ(r->player.bombs, 4);
    CHECK_EQ(r->icon(true, 3), 23);
    CHECK(r->hud.life_icons == life_handles);
    CHECK(r->hud.bomb_icons == bomb_handles);
}

TEST(replay_restored_stock_receives_spirit_pickup_refreshes) {
    auto r = std::make_unique<Rig>();
    ReplayFile file;
    ReplayStage stage;
    stage.number = 1; stage.header.resize(0x70);
    const auto put = [&](u32 off, i32 value) { std::memcpy(stage.header.data() + off, &value, 4); };
    put(0x1c, 1); put(0x44, 100); put(0x48, 400); put(0x4c, 100);
    put(0x50, 2); put(0x54, 7); put(0x5c, 2); put(0x60, 7);
    file.stages.push_back(stage);
    ReplayRestoreLog log; std::string error;
    CHECK(ReplaySession::restore_stage(file, 0,
        {&r->progress, &r->player, &r->score, &r->battle.sim.random, &r->random}, log, error));
    CHECK(r->hud.reset_for_retry());
    r->tick_animations();
    CHECK_EQ(r->icon(false, 2), 16);
    CHECK_EQ(r->icon(true, 2), 23);
    for (i32 kind : {0, 2}) {
        auto& s = r->battle.item_manager.spirits()[u32(kind)];
        s = Spirit{}; s.alive = true; s.kind = kind; s.state = 4; s.x = 0; s.y = 200;
        s.serial = u32(kind + 1);
    }
    CHECK(r->battle.step({}));
    r->tick_animations();
    CHECK_EQ(r->player.extra_lives, 3);
    CHECK_EQ(r->player.bombs, 3);
    CHECK_EQ(r->icon(false, 2), 17);
    CHECK_EQ(r->icon(true, 2), 23);
    CHECK_EQ(r->icon(false, 3), 12);
    CHECK_EQ(r->icon(true, 3), 18);
}

TEST(item_hud_refresh_failure_is_reported_by_the_battle) {
    for (i32 kind : {4, 5, 6, 7}) {
        auto r = std::make_unique<Rig>();
        r->reject_refresh = true;
        r->battle.item_manager.spawn_item(0, 200, 0, kind, 0, 0);
        CHECK(!r->battle.step({}));
        CHECK(!r->battle.error().empty());
    }
}

TEST(bomb_gain_at_the_icon_limit_preserves_inventory_without_overrunning_icons) {
    auto r = std::make_unique<Rig>();
    r->player.bombs = 8;
    CHECK(r->hud.reset_for_retry());
    r->tick_animations();
    r->collect(7);
    CHECK_EQ(r->player.bombs, 9);
    CHECK(r->battle.error().empty());
    CHECK(r->hud.error.empty());
    for (u32 i = 0; i < 8; ++i) CHECK_EQ(r->icon(true, i), 23);
}

int main() { return th13test::run_all("HUD item refresh"); }
