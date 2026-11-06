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
    beast.scripts.resize(150);
    for (auto& script : beast.scripts) {
        script.bytes = {2,0,8,0,0,0,0,0,255,255,8,0,0,0,0,0};
    }
    require(animations.load(30, std::move(beast)), "load test animation bank");
}

void shared_fragments(unsigned seats) {
    AnmEnvironment env;
    Rng random;
    AnmManager animations(random, env);
    load_bank(animations, nullptr);
    EffectManager effects(animations, 30);
    Host host;
    ItemScoreState score;
    std::array<PlayerMotion,3> motion{};
    std::array<PlayerBounds,3> bounds{};
    std::array<PlayerLifeSession,3> stocks{};
    std::array<i32,3> tiers{};
    auto items=std::make_unique<ItemManager>(animations,effects,30);
    items->sound=items->immediate_sound=[](i32){return true;};
    for(unsigned seat=0;seat<seats;++seat){
        motion[seat].position={-120.f+120.f*seat,260,0};
        const auto p=motion[seat].position;
        bounds[seat].item_near={{p.x-8,p.y-8,0},{p.x+8,p.y+8,0}};
    }
    stocks[0].extra_lives=8;stocks[0].bomb_pieces=2;
    stocks[1].life_pieces=2;
    stocks[2].bombs=8;
    ItemCollection one(stocks[0],score,motion[0],host,0,&tiers[0]);
    ItemCollection two(stocks[1],score,motion[1],host,1,&tiers[1]);
    ItemCollection three(stocks[2],score,motion[2],host,2,&tiers[2]);
    ItemFrameContext first{motion[0],bounds[0],one,random,1,5,0,1};
    ItemFrameContext second{motion[1],bounds[1],two,random,1,5,1,1};
    ItemFrameContext third{motion[2],bounds[2],three,random,1,5,2,1};
    const std::array<ItemFrameContext*,2> partners{&second,seats==3?&third:nullptr};
    const auto pickup=[&](i32 kind){
        const auto id=items->spawn(kind,motion[1].position,0,0);
        require(id!=0,"spawn cooperative item");
        if(!items->update(first,partners))require(false,items->error.c_str());
        require(!items->find(id)->state.state,"cooperative item collected once");
    };
    pickup(4);
    require(stocks[0].extra_lives==8&&stocks[0].bombs==4&&stocks[0].bomb_pieces==0,"full life overflow is local");
    require(stocks[1].extra_lives==3&&stocks[1].life_pieces==0&&tiers[1]==1,"partner earns life independently");
    if(seats==3)require(stocks[2].life_pieces==1&&stocks[2].bombs==8,"third pilot receives life fragment");
    pickup(6);
    require(stocks[0].bomb_pieces==1&&stocks[1].bomb_pieces==1,"both pilots receive bomb fragment");
    if(seats==3)require(stocks[2].bomb_pieces==0,"full bomb stock discards its own fragment");
    pickup(1);
    require(stocks[0].power==100&&stocks[1].power==100+i32(seats)&&stocks[2].power==100,
            "small P increases only the collector by the seat count");
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
    shared_fragments(2);
    shared_fragments(3);
    std::cout << "TH17 shared spirit inventory, activation, countdown and tie-breaking passed\n";
}
