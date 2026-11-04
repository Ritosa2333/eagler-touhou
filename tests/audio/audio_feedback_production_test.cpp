#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/SpellPresentation.hpp"
#include "sdl/AudioDevice.hpp"
#include <algorithm>
#include <fstream>
#include <memory>

using namespace th13;
namespace {
struct Provider : EclResourceProvider {
    std::vector<u8> bytes;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> b;
        th13test::emit(b, 0, 23, 0, 255, 0, {10000});
        th13test::emit(b, 0, 10, 0, 255, 0, {});
        bytes = th13test::build_scpt({{"main", b}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string&, std::vector<u8>& b) override { b = bytes; return true; }
    bool animation(u32, const std::string& n) override {
        if (names) names->push_back(n);
        return true;
    }
};
struct Host : StageBattleServices {
    struct E { i32 id; float x; bool positional; };
    std::vector<E> requests;
    std::vector<i32> notices;
    sdl::AudioDevice audio;
    GameRandom random;
    AnmEnvironment environment;
    AnmManager animations{random, environment};
    SpellPresentation presentation{animations};
    bool reject_pickup_sound = false;
    bool initialize() {
        std::ifstream file("artifacts/reference/raw/front.anm", std::ios::binary);
        std::vector<u8> bytes{std::istreambuf_iterator<char>(file), {}};
        return animations.load(3, bytes.data(), u32(bytes.size())) &&
               audio.initialize_effects_for_test("artifacts/reference/raw");
    }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32 id) override {
        requests.push_back({id, 0, false});
        return audio.enqueue(id);
    }
    bool hud_sound(i32 id, float x) override {
        requests.push_back({id, x, true});
        if (reject_pickup_sound) return false;
        return audio.enqueue(id, x);
    }
    bool hud_notice(i32 script) override {
        notices.push_back(script);
        return presentation.show_notice(script);
    }
    bool hud_spirit_notice(i32 level, float x, float y, float z) override {
        notices.push_back(107 + level);
        return presentation.show_spirit_notice(level, x, y, z);
    }
};
struct Rig {
    Provider p;
    Host h;
    SessionState s;
    HudPlayerState player;
    HudScoreState score;
    StageBattle b;
    GameInput in;
    bool init() {
        p.names = &b.sim.anim_list;
        if (!h.initialize() || !b.load(p, "synthetic.ecl", 1, h, s, player, score)) return false;
        b.sim.enemies.clear();
        b.set_player_pos(0, 200);
        h.requests.clear();
        return true;
    }
};

void check_audible(Host& host, i32 voice) {
    // Isolate the award voice: pickup PCM alone must not satisfy audibility.
    for (const auto& request : host.requests)
        if (request.id != voice) CHECK(host.audio.stop(request.id));
    host.audio.update();
    CHECK(host.audio.error.empty());
    CHECK_EQ(host.audio.statistics().active_voices, 1u);
    CHECK_EQ(host.audio.voice_pan(voice), 0);
    std::vector<float> pcm(4096 * 2);
    CHECK(host.audio.mix(pcm.data(), 4096));
    CHECK(std::any_of(pcm.begin(), pcm.end(), [](float v) { return v != 0.f; }));
}
}

TEST(spirit_birth_flag_and_pan) {
    auto r = std::make_unique<Rig>();
    CHECK(r->init());
    r->b.item_manager.spawn_value_item(1, 96, 200, 0, 2, true);
    CHECK_EQ(r->h.requests.size(), size_t(1));
    CHECK_EQ(r->h.requests[0].id, 60);
    CHECK(r->h.requests[0].positional);
    r->h.audio.update();
    CHECK_EQ(r->h.audio.voice_pan(60), 500);
    r->h.requests.clear();
    r->b.item_manager.spawn_value_item(3, -96, 200, 0, 2, false);
    CHECK(r->h.requests.empty());
}

TEST(spirit_collect_cross_segment_order) {
    auto r = std::make_unique<Rig>();
    CHECK(r->init());
    r->s.ending_points = 198;
    r->b.item_manager.spawn_value_item(1, 0, 200, 0, .5f, false);
    r->h.requests.clear();
    CHECK(r->b.step(r->in));
    CHECK_EQ(r->h.requests.size(), size_t(2));
    CHECK_EQ(r->h.requests[0].id, 63);
    CHECK(r->h.requests[0].positional);
    CHECK_EQ(r->h.requests[1].id, 68);
    CHECK(!r->h.requests[1].positional);
}

TEST(big_power_full_plays_level_sound_before_pickup) {
    for (int type : {1, 3, 8}) {
        auto r = std::make_unique<Rig>();
        CHECK(r->init());
        r->player.power = 400;
        r->b.item_manager.spawn_item(0, 200, 0, type, 0, 0);
        r->h.requests.clear();
        CHECK(r->b.step(r->in));
        CHECK_EQ(r->h.requests.size(), size_t(type == 1 ? 1 : 2));
        if (type != 1) CHECK_EQ(r->h.requests[0].id, 13);
        CHECK_EQ(r->h.requests.back().id, 37);
    }
}

// 0043d530: crossing each 200 segment requests 0x44, then creates
// front108/109/110 at the player's world position minus 32 on Y.
TEST(spirit_segments_create_real_notices_and_audible_pcm) {
    for (i32 level = 1; level <= 3; ++level) {
        auto r = std::make_unique<Rig>();
        CHECK(r->init());
        r->s.ending_points = level * 200 - 2;
        r->b.item_manager.spawn_value_item(1, 0, 200, 0, .5f, false);
        CHECK(r->b.step(r->in));
        CHECK_EQ(r->s.ending_points, level * 200);
        CHECK_EQ(r->h.notices.size(), size_t(1));
        CHECK_EQ(r->h.notices[0], 107 + level);
        const auto* vm = r->h.animations.registry.find(r->h.presentation.spirit_notice_handle());
        CHECK(vm != nullptr);
        if (vm) {
            CHECK_EQ(vm->source_script, 107 + level);
            CHECK_EQ(vm->visual.translation.x, r->b.player_x);
            CHECK_EQ(vm->visual.translation.y, r->b.player_y - 32.f);
        }
        check_audible(r->h, 0x44);
        r->h.requests.clear();
        r->h.notices.clear();
        CHECK(r->b.step(r->in));
        CHECK(r->h.requests.empty());
        CHECK(r->h.notices.empty());
    }
}

// 0043d330: the threshold award plays 0x11 and creates front63 once.
TEST(life_piece_extend_creates_real_banner_and_audible_pcm) {
    auto r = std::make_unique<Rig>();
    CHECK(r->init());
    r->player.life_pieces = life_piece_threshold(r->s.life_piece_threshold_index) - 1;
    const i32 lives = r->player.extra_lives;
    r->b.item_manager.spawn_item(0, 200, 0, 4, 0, 0);
    CHECK(r->b.step(r->in));
    CHECK_EQ(r->player.extra_lives, lives + 1);
    CHECK_EQ(r->h.requests.size(), size_t(2));
    CHECK_EQ(r->h.requests[0].id, 0x11);
    CHECK_EQ(r->h.requests[1].id, 0x25);
    CHECK_EQ(r->h.notices.size(), size_t(1));
    CHECK_EQ(r->h.notices[0], 63);
    const auto* vm = r->h.animations.registry.find(r->h.presentation.notice_handle());
    CHECK(vm != nullptr);
    if (vm) CHECK_EQ(vm->source_script, 63);
    check_audible(r->h, 0x11);
}

TEST(failed_pickup_sound_does_not_swallow_notice_or_hide_the_failure) {
    auto r = std::make_unique<Rig>();
    CHECK(r->init());
    r->s.ending_points = 198;
    r->h.reject_pickup_sound = true;
    r->b.item_manager.spawn_value_item(1, 0, 200, 0, .5f, false);
    CHECK(!r->b.step(r->in));
    CHECK_EQ(r->b.error(), std::string("Item sound service failed"));
    CHECK_EQ(r->h.requests.size(), size_t(2));
    CHECK_EQ(r->h.requests.back().id, 0x44);
    CHECK_EQ(r->h.notices.size(), size_t(1));
    CHECK_EQ(r->h.notices[0], 108);
    CHECK(r->h.presentation.spirit_notice_handle() != 0);
    CHECK(r->b.item_manager.events().empty());
}

int main() { return th13test::run_all("audio feedback production"); }
