#include "../ecl/ecl_test_fixtures.hpp"

#include "game/StageBattle.hpp"

#include <cstdio>
#include <string>
#include <vector>

using namespace th13;

namespace {

class Provider final : public EclResourceProvider {
public:
    bool read(const std::string& name, std::vector<u8>& data) override {
        return reader.read("artifacts/reference/raw/" + name, data);
    }
    bool animation(u32, const std::string& name) override {
        std::vector<u8> probe;
        if (!read(name, probe)) return false;
        if (anim_list) anim_list->push_back(name);
        return true;
    }
    th13test::TestFileReader reader;
    std::vector<std::string>* anim_list = nullptr;
};

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) {
        std::printf("FAIL %s\n", what);
        ++failures;
    }
}

}

int main() {
    Provider provider;
    Host host;
    SessionState progress;
    progress.character = 3;
    HudPlayerState player;
    player.bombs = 2;
    player.extra_lives = 99;
    HudScoreState score;
    StageBattle battle;
    battle.sim.anm_effects_noop = true;
    provider.anim_list = &battle.sim.anim_list;
    if (!battle.load(provider, "st01.ecl", 1 << 3, host, progress, player, score, nullptr, "pl03a.sht")) {
        std::printf("FAIL load: %s\n", battle.error().c_str());
        return 1;
    }
    for (int frame = 0; frame < 70; ++frame) {
        GameInput idle;
        if (!battle.step(idle)) {
            std::printf("FAIL warmup: %s\n", battle.error().c_str());
            return 1;
        }
    }
    battle.on_bomb_key();
    bool saw_spawn = false;
    int saw_damage_frame = -1;
    float last_life = -1.0f;
    u32 last_serial = 0;
    for (int frame = 0; frame < 40; ++frame) {
        GameInput input;
        input.held = 0x01;
        if (frame == 0) input.pressed = 0x02;
        if (!battle.step(input)) {
            std::printf("FAIL step %d: %s\n", frame, battle.error().c_str());
            return 1;
        }
        for (int s = 0; s < YoumuBomb::kSlots; ++s)
            if (battle.youmu_bomb_.has_anm(s)) battle.youmu_bomb_.set_vm(s, true, -0.7853981f);
        if (!battle.youmu_slash_spawns_.empty()) {
            saw_spawn = true;
            for (const auto& spawn : battle.youmu_slash_spawns_) {
                check(spawn.script == 19 || spawn.script == 20, "spawn script 19/20");
                check(spawn.y >= 0.0f && spawn.y <= 448.0f, "spawn y in field");
            }
        }
        for (const auto& enemy : battle.sim.enemies) {
            if (enemy->dead || enemy->life <= 0.0f || (enemy->flags & 0x21u)) continue;
            if (last_serial == 0) last_serial = enemy->serial;
            if (enemy->serial != last_serial) continue;
            if (last_life >= 0.0f && enemy->life < last_life) {
                const float delta = last_life - enemy->life;
                check(delta >= 30.0f && (int(delta * 1000.0f) % 30000) == 0, "damage is 30x per frame");
                saw_damage_frame = frame;
            }
            last_life = enemy->life;
            break;
        }
    }
    check(saw_spawn, "W1 spawn events during active frames");
    check(saw_damage_frame >= 0, "W2 slash damage reached an enemy");
    for (int i = 0; i < 8; ++i) {
        StageBullet probe;
        probe.x = -168.0f + float(i) * 48.0f;
        probe.y = 40.0f + float(i % 4) * 110.0f;
        probe.flags = 0x12;
        battle.sim.bullets.push_back(probe);
    }
    for (int frame = 0; frame < 3; ++frame) {
        GameInput input;
        input.held = 0x01;
        if (!battle.step(input)) break;
    }
    int survivors = 0, cancelled = 0;
    for (const auto& bullet : battle.sim.bullets) {
        if (!bullet.alive) continue;
        if (bullet.state == 4) {
            ++cancelled;
            continue;
        }
        if (bullet.y > 30.0f) ++survivors;
    }
    check(survivors > 0, "W3 local clear leaves in-field bullets");
    check(cancelled > 0, "W3 covered bullets enter state4 (not erased)");
    check(battle.error().empty(), "W4 no battle error");
    std::printf(failures ? "# youmu wiring: %d FAIL\n" : "# youmu wiring: all pass\n", failures);
    return failures ? 1 : 0;
}
