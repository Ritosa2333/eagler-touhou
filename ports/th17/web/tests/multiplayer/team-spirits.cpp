#include "../../cpp/game/ItemSpirits.hpp"
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

namespace {
using namespace th17;

void require(bool value, const char* message) {
    if (!value) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

struct Host final : ItemSpiritHost {
    unsigned collected = 0, activated = 0;
    bool options_changed() override { return true; }
    bool notice(i32) override { return true; }
    bool sound(i32 id, bool) override {
        collected += id == 63;
        activated += id == 77;
        return true;
    }
    bool popup(const Vec3&, i32, u32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool trophy(i32) override { return true; }
    bool cancel_bullets(const Vec3&, float, i32) override { return true; }
    bool cancel_lasers(const Vec3&, float, i32, bool) override { return true; }
    bool active_player_shot() const override { return false; }
};

void load_bank(AnmManager& animations, const char* asset) {
    if (asset) {
        const auto load = [&](i32 bank, const std::filesystem::path& name) {
            std::ifstream source(name, std::ios::binary);
            const std::vector<u8> bytes{std::istreambuf_iterator<char>(source), {}};
            return animations.load(bank, bytes.data(), u32(bytes.size()));
        };
        require(load(2, std::filesystem::path(asset).parent_path() / "ascii.anm"), "load ascii.anm");
        require(animations.sprite_fallback(2), "configure fallback sprites");
        require(load(30, asset), "load beast.anm");
        return;
    }
    // A frozen script keeps the game VM and handle lifetime active without
    // requiring original artwork in the native test.
    AnmResource beast;
    beast.scripts.resize(80);
    for (auto& script : beast.scripts) {
        script.bytes = {2,0,8,0,0,0,0,0,255,255,8,0,0,0,0,0};
    }
    require(animations.load(30, std::move(beast)), "load test animation bank");
}

void shared_inventory(const char* asset) {
    AnmEnvironment env;
    Rng visual;
    AnmManager animations(visual, env);
    load_bank(animations, asset);
    EffectManager effects(animations, 30);
    PlayerMotion motion;
    PlayerBounds bounds;
    PlayerLifeSession life;
    PlayerSpellStatus spell;
    DamageSources damage;
    ItemScoreState score;
    Host host;
    auto spirits = std::make_unique<ItemSpirits>(animations, effects, env.game_rng, visual, motion,
        bounds, life, motion.barrier_timer, spell, damage, score, host, 0, 30, 30);

    motion.position = {-96, 256, 0};
    AnimalSpiritTargets team{{{motion.position, true}, {{96, 256, 0}, true}, {{0, 256, 0}, true}}};
    const std::array<int, 5> seats{1, 0, 2, 1, 2};
    for (unsigned slot = 0; slot < seats.size(); ++slot) {
        const auto& player = team[seats[slot]];
        require(spirits->spirits.spawn(1, player.position, 0, false, true) != 0, "spawn spirit");
        if (!spirits->update_spirits({}, team)) {
            std::cerr << spirits->roaring.error << '\n' << animations.error << '\n';
            require(false, spirits->error.c_str());
        }
        require(spirits->roaring.state.count == i32(slot + 1), "all seats add to one inventory");
    }
    require(host.collected == 5 && host.activated == 1, "five pickups activate one Roaring mode");
    require(spirits->roaring.state.kind == 1 && spirits->roaring.state.flags & 2, "shared mode active");
    const auto time = spirits->roaring.state.countdown.current;
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(spirits->roaring.state.countdown.current == time - 1, "team countdown advances once");
}

void team_contact(const char* asset) {
    AnmEnvironment env;
    Rng visual;
    AnmManager animations(visual, env);
    load_bank(animations, asset);
    EffectManager effects(animations, 30);
    PlayerMotion motion;
    PlayerBounds bounds;
    PlayerLifeSession life;
    PlayerSpellStatus spell;
    DamageSources damage;
    ItemScoreState score;
    Host host;
    auto spirits = std::make_unique<ItemSpirits>(animations, effects, env.game_rng, visual, motion,
        bounds, life, motion.barrier_timer, spell, damage, score, host, 0, 30, 30);

    AnimalSpiritTargets team{{{{-12,256,0},false},{{12,256,0},false},{}}};
    const auto id = spirits->spirits.spawn(1, {0,256,0}, 0, false, true);
    require(id != 0, "spawn contact test spirit");
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(host.collected == 0, "inactive players cannot collect");
    const auto center = spirits->spirits.find(id)->position;
    team[0] = {{center.x - 12,center.y,0},true};
    team[1] = {{center.x + 12,center.y,0},true};
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(host.collected == 1, "two contacts collect one spirit");
    const auto collected_x = spirits->spirits.find(id)->position.x;
    team[0] = {{-64,256,0},true};
    team[1] = {{collected_x,256,0},true};
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(spirits->spirits.find(id)->position.x < collected_x, "tie chooses first seat and retains it");
}
}

int main(int argc, char** argv) {
    const char* asset = argc > 1 ? argv[1] : nullptr;
    shared_inventory(asset);
    team_contact(asset);
    std::cout << "TH17 shared spirit inventory, activation, countdown and tie-breaking passed\n";
}
