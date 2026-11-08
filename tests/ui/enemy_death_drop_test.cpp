#include "../ecl/ecl_test_fixtures.hpp"
#include "game/ItemManager.hpp"

#include <memory>

using namespace th13;

namespace {
struct Fixture {
    GameRandom random;
    StageSim sim;
    ItemManager items{&sim.random};
    StageEnemy enemy;

    Fixture() {
        sim.items = &items;
        sim.diff = 1;
        enemy.life = 40;
        enemy.position[0] = -126.57f;
        enemy.position[1] = 104.54f;
        enemy.drop_main = ItemPower;
    }

    int count() {
        int result = 0;
        for (const auto& item : items.items()) result += item.alive;
        return result;
    }

    int spirits() {
        int result = 0;
        for (const auto& spirit : items.spirits()) result += spirit.alive;
        return result;
    }
};

struct Provider final : EclResourceProvider {
    std::vector<u8> bytes;
    StageSim& sim;
    Provider(StageSim& value, const std::vector<u8>& stream) : sim(value) {
        bytes = th13test::build_scpt({{"main", stream}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "spirits.ecl") return false;
        out = bytes;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        sim.anim_list.push_back(name);
        return true;
    }
};

TEST(enemy_lethal_damage_spawns_configured_drop_once) {
    auto f = std::make_unique<Fixture>();
    f->sim.damage(f->enemy, 15);
    CHECK_EQ(f->count(), 0);
    CHECK(!f->enemy.dead);
    f->sim.damage(f->enemy, 30);
    CHECK(f->enemy.dead);
    CHECK_EQ(f->count(), 1);
    CHECK_EQ(f->items.items()[599].type, ItemPower);
    CHECK_EQ(f->items.items()[599].x, -126.57f);
    CHECK_EQ(f->items.items()[599].y, 104.54f);
    CHECK_EQ(f->enemy.drop_main, 0);
    f->sim.damage(f->enemy, 30);
    CHECK_EQ(f->count(), 1);
}

TEST(enemy_no_destroy_flag_does_not_drop_at_zero_life) {
    auto f = std::make_unique<Fixture>();
    f->enemy.flags = 0x80;
    f->sim.damage(f->enemy, 80);
    CHECK(!f->enemy.dead);
    CHECK_EQ(f->count(), 0);
    CHECK_EQ(f->enemy.drop_main, ItemPower);
}

TEST(enemy_death_consumes_extra_drop_table) {
    auto f = std::make_unique<Fixture>();
    f->enemy.drop_extra[ItemPoint - 1] = 2;
    f->sim.damage(f->enemy, 80);
    CHECK_EQ(f->count(), 3);
    CHECK_EQ(f->enemy.drop_extra[ItemPoint - 1], 0);
}
TEST(enemy_death_spirit_timer_and_difficulty_match_original) {
    struct Case { i32 difficulty, age, expected; };
    for (const auto value : {Case{1, 37, 1}, Case{1, 60, 1}, Case{1, 61, 0},
                             Case{2, 37, 2}, Case{3, 60, 2}, Case{1, 0, 2}}) {
        auto f = std::make_unique<Fixture>();
        f->sim.diff = value.difficulty;
        f->enemy.age = float(value.age);
        f->sim.damage(f->enemy, 80);
        CHECK(f->sim.error().empty());
        CHECK_EQ(f->spirits(), value.expected);
        CHECK_EQ(f->count(), 1);
        f->sim.damage(f->enemy, 80);
        CHECK_EQ(f->spirits(), value.expected);
    }
}

TEST(enemy_death_spirits_consume_rng_before_scatter_table) {
    auto f = std::make_unique<Fixture>();
    f->sim.random.reseed_restore(60189);
    f->enemy.age = 37;
    GameRandom expected(60189);
    constexpr float pi = 3.1415927410125732f;
    const float angle = expected.signed_unit() * pi * 0.25f + 0.7853982f;
    const float speed = expected.unit() * 3.5f + 0.5f;
    expected.signed_unit();
    f->sim.damage(f->enemy, 80);
    CHECK_EQ(f->spirits(), 1);
    CHECK_EQ(f->items.spirits()[0].kind, 1);
    CHECK_EQ(f->items.spirits()[0].angle, angle);
    CHECK_EQ(f->items.spirits()[0].move_speed, speed);
    CHECK_EQ(f->sim.random.seed(), expected.seed());
    CHECK_EQ(f->sim.random.index(), expected.index());
}

TEST(enemy_spirit_commands_and_post_constructor_threshold) {
    auto f = std::make_unique<Fixture>();
    std::vector<u8> stream;
    th13test::emit(stream, 0, 1001, 0, 0xff, 0, {120});
    th13test::emit(stream, 0, 1002, 0, 0xff, 0, {3});
    th13test::emit(stream, 0, 1003, 0, 0xff, 0, {1});
    th13test::emit(stream, 0, 1000, 0, 0xff, 0, {3});
    th13test::emit(stream, 0, 23, 0, 0xff, 0, {600});
    th13test::emit(stream, 600, 10, 0, 0xff, 0, {});
    Provider provider(f->sim, stream);
    CHECK(f->sim.begin(provider, "spirits.ecl", 2));
    auto* enemy = f->sim.spawn_enemy("main", 0, 0, 40, nullptr, false);
    CHECK(enemy != nullptr);
    if (!enemy) return;
    CHECK_EQ(enemy->death_spirit_limit, 120);
    CHECK_EQ(enemy->death_spirit_count, 3);
    CHECK(enemy->damage_spirits_disabled);
    CHECK_EQ(enemy->spirit_threshold, 1000);
    CHECK_EQ(f->spirits(), 1);
    CHECK_EQ(f->items.spirits()[0].kind, 3);
    enemy->age = 50;
    enemy->invulnerable = 0;
    CHECK(!enemy->dead);
    CHECK_EQ(enemy->life, 40.0f);
    f->sim.damage(*enemy, 80);
    CHECK(f->sim.error().empty());
    CHECK(enemy->dead);
    CHECK_EQ(f->spirits(), 3);
}

TEST(enemy_invalid_death_spirit_divisor_is_reported) {
    auto f = std::make_unique<Fixture>();
    f->enemy.death_spirit_count = 0;
    f->sim.damage(f->enemy, 80);
    CHECK(!f->sim.error().empty());
    CHECK_EQ(f->spirits(), 0);
    CHECK_EQ(f->count(), 0);
}
}  // namespace

int main() { return th13test::run_all("Enemy death drops"); }
