#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"

using namespace th13;

namespace {
class Provider final : public EclResourceProvider {
public:
    bool read(const std::string& name, std::vector<u8>& bytes) override {
        return files.read("artifacts/reference/raw/" + name, bytes);
    }
    bool animation(u32, const std::string& name) override {
        std::vector<u8> bytes;
        if (!read(name, bytes)) return false;
        animations->push_back(name);
        return true;
    }
    th13test::TestFileReader files;
    std::vector<std::string>* animations = nullptr;
};

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
}

TEST(firing_waits_for_original_startup_gate) {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    provider.animations = &battle.sim.anim_list;
    CHECK(battle.load(provider, "st01.ecl", 2, host, progress, player, score,
                      nullptr, "pl00a.sht"));
    if (!battle.error().empty()) return;

    GameInput input;
    input.update(1);
    for (int frame = 1; frame <= 19; ++frame) {
        CHECK(battle.step(input));
        CHECK(battle.shots.empty());
    }
    CHECK(battle.step(input));
    CHECK_EQ(battle.sim.frame, 20u);
    CHECK_EQ(battle.shots.size(), 3u);
    CHECK_EQ(battle.shots[0].x, -10.0f);
    CHECK_EQ(battle.shots[0].y, 392.0f);
    CHECK_EQ(battle.shots[0].damage, 15.0f);

    progress.scene_flags |= 0x4000;
    battle.shots.clear();
    CHECK(battle.step(input));
    CHECK(battle.shots.empty());
    progress.scene_flags &= ~0x4000u;
    CHECK(battle.step(input));
    CHECK_EQ(battle.shots.size(), 3u);
}

int main() { return th13test::run_all("player shot startup"); }
