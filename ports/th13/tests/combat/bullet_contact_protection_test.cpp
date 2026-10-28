#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Host : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Provider : EclResourceProvider {
    std::vector<u8> bytes;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> body;
        th13test::emit(body, 0, 23, 0, 255, 0, {1000});
        th13test::emit(body, 0, 10, 0, 255, 0, {});
        bytes = th13test::build_scpt({{"main", body}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string&, std::vector<u8>& out) override { out = bytes; return true; }
    bool animation(u32, const std::string& name) override { names->push_back(name); return true; }
};

TEST(contact_protection_does_not_prevent_player_hit_but_preserves_bullet) {
    for (bool protected_bullet : {false, true}) {
        Provider provider;
        Host host;
        SessionState progress;
        HudPlayerState player;
        HudScoreState score;
        StageBattle battle;
        provider.names = &battle.sim.anim_list;
        CHECK(battle.load(provider, "test.ecl", 2, host, progress, player, score));
        battle.sim.enemies.clear();
        battle.set_player_pos(0, 200);
        battle.player_phase = StageBattle::PlayerPhase::Active;
        battle.invulnerable = 0;
        StageBullet bullet;
        bullet.x = 0; bullet.y = 200;
        bullet.cancel_protection = protected_bullet ? 5 : 0;
        battle.sim.bullets.push_back(bullet);
        CHECK(battle.step(GameInput{}));
        CHECK(battle.player_phase == StageBattle::PlayerPhase::Deathbomb);
        CHECK_EQ(battle.sim.bullets.size(), size_t(1));
        CHECK_EQ(battle.sim.bullets[0].alive, protected_bullet);
    }
}
}
int main() { return th13test::run_all("bullet contact protection"); }
