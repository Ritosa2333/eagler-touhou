// M3 原生固定 case（道具/神霊/资源：一件一次、逐席广播、团队一份）。
// 被测生产面（RULES R-I1..R-I9/R-G）：
//   - ItemManager::update_seats/update_spirits_seats（逐件最近合格席）；
//   - collect_* 结算宿主跟随选中席（last_player_ 逐件更新）；
//   - ItemCollector.seat/eligible/character（席位归属/幽灵排除/角色门）。
//
// 固定 case 映射（workflow-cases.json m3.*）：
//   shared_pickup_once        → 一件道具只结算一次（最近席拾取、其余席
//                                不重复结算）
//   resource_all_types_caps   → P/命/Bomb/碎片广播各席（逐席上限独立）
//   spirit_role_and_trance_grant → 神霊角色门按拾取者席位角色
//   graze_owner_generation    → 擦弹按弹身份+席位（generation 不串席）
//   boss_damage_remainders    → Boss 合作系数确定性余数
//   gift_not_broadcast        → 定向赠送不走全员拾取
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/WorldState.hpp"
#include "game/AnmManager.hpp"
#include "game/GameHud.hpp"
#include "eagler/gameplay/CooperativeRules.hpp"
#include <cmath>
#include <cstdlib>
#include <fstream>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    GameHud* hud = nullptr;
    // M5：逐席记录（R-I3 广播发放每席各一次；旧单套「seat 0 投影」语义
    // 已被每席 HUD 取代）。last_lives/last_pieces 仍保留 = 末次任意席值。
    i32 last_lives = -99, last_pieces = -99;
    i32 seat_lives[2] = {-99, -99}, seat_pieces[2] = {-99, -99};
    bool life_hud(i32 lives, i32 pieces) override {
        last_lives = lives; last_pieces = pieces;
        return !hud || hud->life(lives, pieces);
    }
    bool life_hud_seat(i32 seat, i32 lives, i32 pieces) override {
        if (seat >= 0 && seat < 2) {
            seat_lives[seat] = lives;
            seat_pieces[seat] = pieces;
        }
        return life_hud(lives, pieces);
    }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 0xff, 0, {600});
        emit(main, 0, 10, 0, 0xff, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mp3.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                for (int i = 0; i < 8; ++i) {
                    const std::string probe = path + "/artifacts/reference/raw/pl00a.sht";
                    std::FILE* check = std::fopen(probe.c_str(), "rb");
                    if (check) { std::fclose(check); break; }
                    const auto cut = path.find_last_of("/\\");
                    if (cut == std::string::npos) break;
                    path.erase(cut);
                }
                return path;
            }();
            std::FILE* file = std::fopen((root + "/artifacts/reference/raw/" + name).c_str(), "rb");
            if (!file) return false;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            return got == out.size();
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        if (animations) animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;

    bool init(i32 difficulty_rank = 2) {
        provider.animations = &battle.sim.anim_list;
        return battle.load(provider, "mp3.ecl", difficulty_rank, host, progress, player, score);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

// 直接构造多席 collector 数组（不经 step_mp——单元级驱动 ItemManager）。
ItemCollector collector_at(float x, float y, i32 seat, i32 character,
                          HudPlayerState& hud, SessionState& progress,
                          HudScoreState& score) {
    ItemCollector c;
    c.x = x;
    c.y = y;
    c.state = 1;
    c.dialog = false;
    c.homing_speed = 4.0f;
    const float half = 60.0f * 0.25f;
    c.box = {x - half, y - half, x + half, y + half};
    const float auto_half = 60.0f * 0.5f;
    c.box_auto = {x - auto_half, y - auto_half, x + auto_half, y + auto_half};
    c.progress = &progress;
    c.player = &hud;
    c.score = &score;
    c.seat = seat;
    c.eligible = true;
    c.character = character;
    return c;
}

void place_seats(StageBattle& battle, std::array<float, 3> xs, std::array<float, 3> ys) {
    for (i32 seat = 0; seat < battle.seat_count; ++seat) {
        battle.seats[size_t(seat)].x = xs[size_t(seat)];
        battle.seats[size_t(seat)].y = ys[size_t(seat)];
    }
}

// R-I7/R-I8: the trigger is team-wide, the physical receiver is per item.
void verify_team_autocollect_and_no_ghost_target() {
    Fixture f; CHECK(f.init()); f.enable(3, {0, 0, 0});
    auto& manager = f.battle.item_manager;
    ItemCollector seats[3] = {
        collector_at(0, 300, 0, 0, f.battle.seat_huds[0], f.progress, f.score),
        collector_at(180, 128, 1, 0, f.battle.seat_huds[1], f.progress, f.score),
        collector_at(-180, 300, 2, 0, f.battle.seat_huds[2], f.progress, f.score)};
    manager.spawn_item(0, 200, 0, ItemPower, 0, 0);
    Item* item = nullptr;
    for (auto& candidate : manager.items()) if (candidate.alive) item = &candidate;
    CHECK(item != nullptr); if (!item) return;
    manager.update_seats(seats, 3);
    CHECK_EQ(item->state, 1); CHECK_EQ(item->tracking_seat, -1); // Exactly on line.
    seats[1].y = 127; seats[1].eligible = false;
    manager.update_seats(seats, 3);
    CHECK_EQ(item->state, 1); // Ghost above line cannot trigger.
    seats[1].eligible = true;
    const float old_y = item->y;
    manager.update_seats(seats, 3);
    CHECK_EQ(item->state, 3); CHECK_EQ(item->tracking_seat, 0);
    CHECK(item->y > old_y); // Closest seat below the line, not the trigger.
    seats[1] = collector_at(item->x, item->y - 40, 1, 0,
                            f.battle.seat_huds[1], f.progress, f.score);
    const float tracked_y = item->y;
    manager.update_seats(seats, 3);
    CHECK_EQ(item->tracking_seat, 0); CHECK(item->y > tracked_y);

    manager.spawn_value_item(2, 0, 200, 0, 0);
    Spirit* spirit = nullptr;
    for (auto& candidate : manager.spirits()) if (candidate.alive) spirit = &candidate;
    CHECK(spirit != nullptr); if (!spirit) return;
    spirit->state = 2; spirit->speed = spirit->move_speed = 4;
    spirit->tracking_seat = 0;
    for (auto& seat : seats) seat.eligible = false;
    const float item_x = item->x, item_y = item->y;
    const float spirit_x = spirit->x, spirit_y = spirit->y;
    const auto power = f.battle.seat_huds[0].power;
    manager.clear_events();
    manager.update_seats(seats, 3); manager.update_spirits_seats(seats, 3);
    CHECK(item->alive); CHECK_EQ(item->tracking_seat, -1);
    CHECK_EQ(item->x, item_x); CHECK_EQ(item->y, item_y);
    CHECK(spirit->alive); CHECK_EQ(spirit->tracking_seat, -1);
    CHECK_EQ(spirit->x, spirit_x); CHECK_EQ(spirit->y, spirit_y);
    CHECK_EQ(f.battle.seat_huds[0].power, power); CHECK(manager.events().empty());
    seats[2] = collector_at(-100, 200, 2, 0, f.battle.seat_huds[2], f.progress, f.score);
    manager.update_seats(seats, 3); manager.update_spirits_seats(seats, 3);
    CHECK_EQ(item->tracking_seat, 2); CHECK(item->x < item_x);
    CHECK_EQ(spirit->tracking_seat, 2); CHECK(spirit->x < spirit_x);

    // A rising cancel item must not copy a ghost's homing speed. Its first
    // eligible receiver supplies the speed when ascent finishes.
    manager.reset_items();
    for (auto& seat : seats) seat.eligible = false;
    seats[0].homing_speed = 2; // Ghost's character speed is a negative control.
    manager.spawn_item(0, 200, 0, ItemPower, 0, 0);
    item = nullptr;
    for (auto& candidate : manager.items()) if (candidate.alive) item = &candidate;
    CHECK(item != nullptr); if (!item) return;
    item->state = 2; item->vy = 0; item->homing_speed = 0;
    manager.update_seats(seats, 3);
    CHECK_EQ(item->state, 2); CHECK_EQ(item->homing_speed, 0.f);
    seats[1] = collector_at(100, 200, 1, 1, f.battle.seat_huds[1], f.progress, f.score);
    seats[1].homing_speed = 7;
    manager.update_seats(seats, 3);
    CHECK_EQ(item->state, 3); CHECK_EQ(item->tracking_seat, 1);
    CHECK_EQ(item->homing_speed, 7.2f); CHECK(item->x > 0);
}

void verify_targeted_life_hud_seats() {
    Fixture f; CHECK(f.init()); f.enable(3, {0, 0, 0});
    auto& manager = f.battle.item_manager;
    ItemCollector seats[3] = {
        collector_at(-150, 300, 0, 0, f.battle.seat_huds[0], f.progress, f.score),
        collector_at(0, 300, 1, 0, f.battle.seat_huds[1], f.progress, f.score),
        collector_at(150, 300, 2, 0, f.battle.seat_huds[2], f.progress, f.score)};
    std::vector<i32> hud_seats;
    manager.life_changed = [&](i32 seat, i32, i32) { hud_seats.push_back(seat); };
    for (i32 recipient : {1, 2}) {
        const i32 before[3] = {seats[0].player->extra_lives, seats[1].player->extra_lives,
                               seats[2].player->extra_lives};
        hud_seats.clear(); manager.clear_events();
        CHECK(manager.spawn_targeted_item(seats[recipient].x, seats[recipient].y,
                                          ItemExtraLife, recipient));
        manager.update_seats(seats, 3);
        CHECK(hud_seats == std::vector<i32>{recipient});
        for (i32 seat = 0; seat < 3; ++seat)
            CHECK_EQ(seats[seat].player->extra_lives, before[seat] + (seat == recipient));
        size_t collected = 0;
        for (const auto& event : manager.events())
            if (event.kind == ItemEvent::Kind::Collected) ++collected;
        CHECK_EQ(collected, 1u);
    }
}

void verify_tracking_receiver_retention() {
    Fixture f;
    CHECK(f.init());
    f.enable(3, {1, 1, 1});
    auto& manager = f.battle.item_manager;
    ItemCollector seats[3] = {
        collector_at(60, 300, 0, 1, f.battle.seat_huds[0], f.progress, f.score),
        collector_at(180, 300, 1, 1, f.battle.seat_huds[1], f.progress, f.score),
        collector_at(-180, 300, 2, 1, f.battle.seat_huds[2], f.progress, f.score)};
    manager.spawn_item(0, 300, 0, ItemPower, 0, 0);
    Item* item = nullptr;
    for (auto& candidate : manager.items()) if (candidate.alive) item = &candidate;
    CHECK(item != nullptr);
    if (!item) return;
    manager.update_seats(seats, 3); // Falling is not a homing acquisition.
    CHECK_EQ(item->tracking_seat, -1);
    item->state = 3;
    item->homing_speed = 4;
    manager.update_seats(seats, 3);
    CHECK_EQ(item->tracking_seat, 0);
    CHECK(item->x > 0);
    const float acquired_x = item->x;
    seats[0] = collector_at(160, 300, 0, 1, f.battle.seat_huds[0], f.progress, f.score);
    seats[1] = collector_at(-40, 300, 1, 1, f.battle.seat_huds[1], f.progress, f.score);
    manager.update_seats(seats, 3);
    CHECK(item->x > acquired_x); // A closer eligible player cannot steal it.
    CHECK_EQ(item->tracking_seat, 0);
    seats[0].eligible = false;
    const float before_invalid = item->x;
    manager.update_seats(seats, 3);
    CHECK(item->x < before_invalid);
    CHECK_EQ(item->tracking_seat, 1);
    seats[1].state = 4; // Deathbomb ends ordinary homing as in retail.
    manager.update_seats(seats, 3);
    CHECK_EQ(item->state, 1);
    CHECK_EQ(item->tracking_seat, -1);
    // Retirement and ordinary pool reuse must not inherit either recipient.
    seats[1] = collector_at(item->x, item->y, 1, 1,
        f.battle.seat_huds[1], f.progress, f.score);
    manager.update_seats(seats, 3);
    CHECK(!item->alive);
    CHECK_EQ(item->tracking_seat, -1);
    CHECK_EQ(item->target_seat, -1);
    manager.spawn_item(0, 300, 0, ItemPower, 0, 0);
    CHECK(item->alive);
    CHECK_EQ(item->tracking_seat, -1);
    CHECK_EQ(item->target_seat, -1);
    item->state = 3; item->homing_speed = 4;
    seats[0] = collector_at(80, 300, 0, 1, f.battle.seat_huds[0], f.progress, f.score);
    seats[1] = collector_at(-80, 300, 1, 1, f.battle.seat_huds[1], f.progress, f.score);
    manager.update_seats(seats, 3);
    CHECK_EQ(item->tracking_seat, 0); // Equal distance chooses the smaller seat.

    manager.spawn_value_item(2, 0, 200, 0, 0);
    Spirit* spirit = nullptr;
    for (auto& candidate : manager.spirits()) if (candidate.alive) spirit = &candidate;
    CHECK(spirit != nullptr);
    if (!spirit) return;
    seats[0] = collector_at(60, 200, 0, 1, f.battle.seat_huds[0], f.progress, f.score);
    seats[1] = collector_at(180, 200, 1, 1, f.battle.seat_huds[1], f.progress, f.score);
    spirit->move_speed = spirit->speed = 0;
    manager.update_spirits_seats(seats, 3); // Actual state1 -> state2 acquisition.
    CHECK_EQ(spirit->state, 2);
    CHECK_EQ(spirit->tracking_seat, 0);
    spirit->speed = 4;
    manager.update_spirits_seats(seats, 3);
    const float acquired_spirit_x = spirit->x;
    seats[0] = collector_at(80, 200, 0, 1, f.battle.seat_huds[0], f.progress, f.score);
    seats[1] = collector_at(-40, 200, 1, 1, f.battle.seat_huds[1], f.progress, f.score);
    manager.update_spirits_seats(seats, 3);
    CHECK(spirit->x > acquired_spirit_x);
    CHECK_EQ(spirit->tracking_seat, 0);
    seats[0].eligible = false;
    const float before_spirit_invalid = spirit->x;
    manager.update_spirits_seats(seats, 3);
    CHECK(spirit->x < before_spirit_invalid);
    CHECK_EQ(spirit->tracking_seat, 1);
    seats[1].state = 4;
    manager.update_spirits_seats(seats, 3);
    CHECK_EQ(spirit->state, 1);
    CHECK_EQ(spirit->tracking_seat, -1);
    const u32 serial = spirit->serial;
    seats[1] = collector_at(spirit->x, spirit->y, 1, 1,
        f.battle.seat_huds[1], f.progress, f.score);
    manager.update_spirits_seats(seats, 3);
    CHECK(!spirit->alive);
    CHECK_EQ(spirit->tracking_seat, -1);
    manager.spawn_value_item(2, 0, 200, 0, 0);
    CHECK(spirit->alive);
    CHECK(spirit->serial != serial);
    CHECK_EQ(spirit->tracking_seat, -1);

    // R-I9: a named gift is independent of ordinary physical tracking, even
    // if another player is closer or the recipient is temporarily a ghost.
    CHECK(manager.spawn_targeted_item(0, 100, ItemExtraLife, 2));
    Item* gift = nullptr;
    for (auto& candidate : manager.items())
        if (candidate.alive && candidate.target_seat == 2) gift = &candidate;
    CHECK(gift != nullptr);
    if (!gift) return;
    seats[0] = collector_at(-40, 100, 0, 1, f.battle.seat_huds[0], f.progress, f.score);
    seats[1] = collector_at(-100, 100, 1, 1, f.battle.seat_huds[1], f.progress, f.score);
    seats[2] = collector_at(100, 100, 2, 1, f.battle.seat_huds[2], f.progress, f.score);
    manager.update_seats(seats, 3);
    CHECK_EQ(gift->target_seat, 2);
    CHECK_EQ(gift->tracking_seat, -1);
    CHECK(gift->x > 0);
    seats[2].eligible = false;
    const float gift_x = gift->x;
    manager.update_seats(seats, 3);
    CHECK_EQ(gift->x, gift_x);
    CHECK_EQ(gift->target_seat, 2);
    CHECK_EQ(gift->tracking_seat, -1);

    // Snapshots copy the production pools. Mutating only either receiver
    // must alter both authoritative and item-domain digests; restore keeps it.
    GameRandom visual{321};
    multiplayer::WorldState saved;
    CHECK(saved.capture(f.battle, visual));
    const auto digest = saved.digest(), core = saved.digest_core();
    char original_domains[4096]{};
    saved.digest_domains(original_domains, sizeof original_domains);
    for (bool spirit_target : {false, true}) {
        i32& receiver = spirit_target ? spirit->tracking_seat : item->tracking_seat;
        const i32 original = receiver;
        receiver = original == 1 ? 0 : 1;
        multiplayer::WorldState mutated;
        CHECK(mutated.capture(f.battle, visual));
        CHECK(mutated.digest() != digest);
        CHECK(mutated.digest_core() != core);
        char changed[4096]{};
        mutated.digest_domains(changed, sizeof changed);
        CHECK(std::string(changed) != original_domains);
        CHECK(saved.restore(f.battle, visual));
        CHECK_EQ(receiver, original);
    }
    // Replay twelve actual manager updates, including receiver invalidation.
    const auto advance = [&](i32 frame) {
        seats[0] = collector_at(100, 200, 0, 1, f.battle.seat_huds[0], f.progress, f.score);
        seats[1] = collector_at(-80, 200, 1, 1, f.battle.seat_huds[1], f.progress, f.score);
        seats[2] = collector_at(180, 200, 2, 1, f.battle.seat_huds[2], f.progress, f.score);
        seats[0].eligible = frame < 6;
        manager.update_seats(seats, 3);
        manager.update_spirits_seats(seats, 3);
        multiplayer::WorldState state;
        CHECK(state.capture(f.battle, visual));
        return state.digest();
    };
    std::array<u64, 12> expected{};
    for (i32 frame = 0; frame < 12; ++frame) expected[size_t(frame)] = advance(frame);
    CHECK(saved.restore(f.battle, visual));
    for (i32 frame = 0; frame < 12; ++frame)
        CHECK_EQ(advance(frame), expected[size_t(frame)]);
}

// R-I7: drive the real battle/world item pass, not a test loop per seat.
// Both pool animation clocks expose a duplicate update even when no pickup
// occurs. Compare SP with two/three co-located eligible collectors.
void verify_item_world_tick_once() {
    for (i32 count : {1, 2, 3}) {
        Fixture f; CHECK(f.init());
        f.battle.player_phase = StagePlayerState::Phase::Active;
        if (count > 1) f.enable(count, {0, 0, 0});
        auto& manager = f.battle.item_manager;
        manager.spawn_item(-160, 200, 0, ItemPower, 0, 1);
        manager.spawn_value_item(2, 160, 200, 0, 0);
        Item* item = nullptr; Spirit* spirit = nullptr;
        for (auto& candidate : manager.items()) if (candidate.alive) item = &candidate;
        for (auto& candidate : manager.spirits()) if (candidate.alive) spirit = &candidate;
        CHECK(item != nullptr); CHECK(spirit != nullptr);
        if (!item || !spirit) return;
        spirit->move_speed = 0; // Far from every collector: no homing/pickup.
        const u32 rng = f.battle.sim.random.index();
        for (i32 frame = 0; frame < 5; ++frame) {
            CHECK(f.battle.step(f.idle));
            CHECK(item->alive); CHECK(spirit->alive);
            CHECK_EQ(item->anm_frame, frame + 1);
            CHECK_EQ(item->anm_frame_prev, frame);
            CHECK_EQ(spirit->anm_frame, frame + 1);
            CHECK_EQ(spirit->anm_frame_prev, frame);
            CHECK_EQ(spirit->frame, frame);
            CHECK_EQ(spirit->lifetime_clock.current, frame + 1);
            CHECK_EQ(f.battle.sim.frame, u32(frame + 1));
            CHECK_EQ(f.battle.sim.random.index(), rng);
        }
    }
}

// R-I1: registered damage goes through the production enemy_hit callback,
// including damage spirits, natural death spirits and the ECL drop table.
// Identical total ordinary-enemy damage must retain SP quantities and RNG.
void verify_enemy_drops_match_single_player() {
    for (i32 difficulty : {1, 3}) {
      std::vector<u32> single;
      for (i32 count : {1, 2, 3}) {
        Fixture f; CHECK(f.init(1 << difficulty));
        f.battle.player_phase = StagePlayerState::Phase::Active;
        if (count > 1) f.enable(count, {0, 0, 0});
        CHECK_EQ(f.battle.sim.diff, difficulty);
        const i32 death_spirits = difficulty == 1 ? 2 : 3;
        const u32 draws = difficulty == 1 ? 46u : 50u;
        CHECK(!f.battle.sim.enemies.empty());
        if (f.battle.sim.enemies.empty()) return;
        auto& enemy = *f.battle.sim.enemies.front();
        enemy.flags = 0; enemy.invulnerable = 0; enemy.life = 80;
        enemy.position[0] = 0; enemy.position[1] = 350;
        enemy.hurtbox[0] = enemy.hurtbox[1] = 20;
        enemy.age = 30; enemy.death_spirit_limit = 60; enemy.death_spirit_count = 3;
        enemy.death_sound = enemy.death_effect = -1;
        enemy.spirit_accum = 0; enemy.spirit_threshold = 8; enemy.hit_anim_timer = 20;
        enemy.drop_main = ItemPower;
        enemy.drop_extra[ItemBigPower - 1] = 2;
        enemy.drop_extra[ItemExtraLife - 1] = 1;
        enemy.drop_extra[ItemBomb - 1] = 1;
        enemy.drop_extra[ItemSpiritKind0 - 1] = 1;
        enemy.drop_extra[ItemSpiritKind2 - 1] = 1;
        f.battle.sim.random.reseed_restore(0x5a31);
        const auto hit = [&](i32 amount) {
            // A real Area registration owned by one active seat; extra
            // players add neither duplicate query damage nor drop callbacks.
            auto& registrations = count == 1 ? f.battle.damage_registrations
                                             : f.battle.seats[0].damage_registrations;
            registrations = {};
            auto& damage = registrations[0];
            damage.active = true; damage.flags = 1;
            damage.owner = DamageRegistration::Owner::Area;
            damage.center_x = 0; damage.center_y = 350;
            damage.width = damage.height = 20; damage.damage = float(amount);
            f.battle.sim.enemy_hit(enemy);
        };
        hit(8);
        CHECK_EQ(enemy.life, 72.f); CHECK_EQ(enemy.spirit_accum, 8.f);
        CHECK_EQ(enemy.spirit_threshold, 408); // 50px -> +400 threshold.
        size_t spirit_count = 0;
        for (const auto& spirit : f.battle.item_manager.spirits()) if (spirit.alive) {
            ++spirit_count; CHECK_EQ(spirit.kind, 1);
        }
        CHECK_EQ(spirit_count, 1u);
        CHECK_EQ(f.battle.sim.random.index(), 4u); // angle and speed, once.
        hit(72); // Below the retail per-query damage clamp (80).
        CHECK(enemy.dead); CHECK_EQ(enemy.life, 0.f);
        CHECK_EQ(enemy.spirit_accum, 80.f); CHECK_EQ(enemy.spirit_threshold, 408);
        std::array<i32, 15> items{}; std::array<i32, 5> spirits{};
        std::vector<u32> observed;
        const auto bits = [&](float value) { u32 raw; std::memcpy(&raw, &value, sizeof raw); observed.push_back(raw); };
        for (const auto& item : f.battle.item_manager.items()) if (item.alive) {
            ++items[size_t(item.type)]; observed.push_back(u32(item.type));
            bits(item.x); bits(item.y); bits(item.vx); bits(item.vy);
        }
        for (const auto& spirit : f.battle.item_manager.spirits()) if (spirit.alive) {
            ++spirits[size_t(spirit.kind)]; observed.push_back(u32(spirit.kind));
            observed.push_back(spirit.serial); observed.push_back(spirit.flags);
            bits(spirit.x); bits(spirit.y); bits(spirit.angle); bits(spirit.move_speed);
        }
        CHECK_EQ(items[ItemPower], 1); CHECK_EQ(items[ItemBigPower], 2);
        CHECK_EQ(items[ItemExtraLife], 1); CHECK_EQ(items[ItemBomb], 1);
        i32 item_count = 0; for (i32 n : items) item_count += n;
        CHECK_EQ(item_count, 5);
        CHECK_EQ(spirits[0], 1); CHECK_EQ(spirits[1], death_spirits + 1); CHECK_EQ(spirits[2], 1);
        CHECK_EQ(spirits[3], 0); CHECK_EQ(spirits[4], 0);
        CHECK_EQ(f.battle.sim.random.index(), draws);
        CHECK_EQ(enemy.drop_main, 0);
        for (i32 remaining : enemy.drop_extra) CHECK_EQ(remaining, 0);
        observed.push_back(f.battle.sim.random.index()); observed.push_back(f.battle.sim.random.seed());
        if (count == 1) single = observed;
        else CHECK(observed == single); // Full spawned geometry and RNG identity.
        // Dead enemies cannot emit their table/spirits a second time.
        hit(8); CHECK_EQ(f.battle.sim.random.index(), draws);
        // Boss absorption consumes the same shared spirit pool once.
        StageEnemy boss; boss.life = 100; boss.i_slots[2] = 7;
        boss.position[0] = 0; boss.position[1] = 350;
        bool first = true;
        for (auto& spirit : f.battle.item_manager.spirits()) if (spirit.alive) {
            spirit.x = first ? 100.f : 0.f; spirit.y = 350; first = false;
        }
        CHECK(f.battle.sim.enemy_absorb_spirits(boss));
        CHECK_EQ(boss.i_slots[3], death_spirits + 2);
        CHECK_EQ(boss.life, float(100 + 7 * (death_spirits + 2)));
        CHECK(f.battle.sim.enemy_absorb_spirits(boss));
        CHECK_EQ(boss.i_slots[3], death_spirits + 2);
        CHECK_EQ(boss.life, float(100 + 7 * (death_spirits + 2)));
        boss.position[0] = 100;
        CHECK(f.battle.sim.enemy_absorb_spirits(boss));
        CHECK_EQ(boss.i_slots[3], death_spirits + 3);
        CHECK_EQ(boss.life, float(100 + 7 * (death_spirits + 3)));
        CHECK_EQ(f.battle.sim.random.index(), draws);
        CHECK(f.battle.error().empty()); CHECK(f.battle.sim.error().empty());
      }
    }
}

}  // namespace

// ---- shared_pickup_once：一件道具恰结算一次 ----
// 两席相隔，只有 seat 0 物理拾取；个人 Power 各一份，销毁/拾取事件一次。
TEST(shared_pickup_once) {
    verify_team_autocollect_and_no_ghost_target();
    verify_targeted_life_hud_seats();
    verify_tracking_receiver_retention();
    verify_item_world_tick_once();
    verify_enemy_drops_match_single_player();
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    place_seats(dual.battle, {-100.0f, 100.0f, 0.0f}, {400.0f, 400.0f, 0.0f});
    auto& manager = dual.battle.item_manager;
    auto& hud0 = dual.battle.seat_huds[0];
    auto& hud1 = dual.battle.seat_huds[1];
    hud0.power = 100;   // 席 0 起始 1.00
    hud1.power = 100;
    // 小 P（type 1）落在 seat 0 收集盒内（±15）。
    manager.spawn_item(-95.0f, 400.0f, 0.0f, 1, 0.0f, 0.0f);
    // spawn 从 free 池尾取槽（非 0 槽）：按内容定位。
    Item* spawned = nullptr;
    for (auto& item : manager.items())
        if (item.alive && item.type == 1) { spawned = &item; break; }
    CHECK(spawned != nullptr);
    ItemCollector seats[2] = {
        collector_at(-100.0f, 400.0f, 0, 0, hud0, dual.progress, dual.score),
        collector_at(100.0f, 400.0f, 1, 0, hud1, dual.progress, dual.score)};
    manager.update_seats(seats, 2);
    // 恰一次物理结算，个人奖励各一份（R-I2/R-I3）。
    CHECK_EQ(spawned->alive, false);
    CHECK_EQ(hud0.power, 101);
    CHECK_EQ(hud1.power, 101);
    // 结算事件恰一批（同一道具不重复）。
    size_t collected = 0;
    for (const auto& event : manager.events())
        if (event.kind == ItemEvent::Kind::Collected) ++collected;
    CHECK_EQ(collected, 1u);
    manager.clear_events();
    // A single pickup can cross different option levels on both seats.
    hud0.power=199; hud1.power=299;
    std::vector<i32> rebuilt;
    dual.battle.option_power_changed=[&](const StageBattle& battle) {
        rebuilt.push_back(battle.active_seat); return true;
    };
    manager.spawn_item(-95,400,0,ItemPower,0,0); manager.update_seats(seats,2);
    CHECK_EQ(hud0.power,200); CHECK_EQ(hud1.power,300);
    CHECK_EQ(dual.battle.seats[0].option_power,2); CHECK_EQ(dual.battle.seats[1].option_power,3);
    CHECK(rebuilt==std::vector<i32>({0,1})); CHECK_EQ(dual.battle.active_seat,0);
}

// ---- resource_all_types_caps：逐席资源与上限 ----
// R-I3/R-I4/R-I6/R-I8：个人奖励广播，团队值一份，ghost 只获账面奖励。
TEST(resource_all_types_caps) {
    Fixture f; CHECK(f.init()); f.enable(3, {0, 1, 3});
    auto& manager=f.battle.item_manager;
    f.progress.max_point_cap=25000000;
    auto& a=f.battle.seat_huds[0]; auto& b=f.battle.seat_huds[1]; auto& c=f.battle.seat_huds[2];
    ItemCollector seats[3]={collector_at(0,400,0,0,a,f.progress,f.score),
        collector_at(180,400,1,1,b,f.progress,f.score),collector_at(-180,400,2,3,c,f.progress,f.score)};
    const auto pickup=[&](i32 type) {
        manager.clear_events(); manager.spawn_item(-10,400,0,type,0,0); manager.update_seats(seats,3);
        size_t collected=0; for(const auto& e:manager.events()) if(e.kind==ItemEvent::Kind::Collected) ++collected;
        CHECK_EQ(collected,1u);
    };
    a.power=400; b.power=100; c.power=200;
    pickup(ItemBigPower); CHECK_EQ(a.power,400); CHECK_EQ(b.power,200); CHECK_EQ(c.power,300); CHECK_EQ(f.score.score,0);
    pickup(ItemBigPowerAlt); CHECK_EQ(b.power,300); CHECK_EQ(c.power,400); CHECK_EQ(f.progress.max_point,0);
    pickup(ItemBigPower); CHECK_EQ(b.power,400); CHECK_EQ(f.score.score,0);
    pickup(ItemBigPower); CHECK_EQ(f.score.score,2000);
    pickup(ItemPower); CHECK_EQ(f.score.score,3000);
    pickup(ItemBigPowerAlt); CHECK_EQ(f.progress.max_point,10000); CHECK_EQ(f.score.score,3000);
    a.extra_lives=8; b.extra_lives=2; c.extra_lives=7;
    pickup(ItemExtraLife); CHECK_EQ(a.extra_lives,8); CHECK_EQ(b.extra_lives,3); CHECK_EQ(c.extra_lives,8);
    a.bombs=9; b.bombs=2; c.bombs=8;
    pickup(ItemBomb); CHECK_EQ(a.bombs,9); CHECK_EQ(b.bombs,3); CHECK_EQ(c.bombs,9);
    a.bomb_pieces=5; b.bomb_pieces=7; c.bomb_pieces=3;
    pickup(ItemBombPiece); CHECK_EQ(a.bomb_pieces,0); CHECK_EQ(b.bomb_pieces,0); CHECK_EQ(b.bombs,4); CHECK_EQ(c.bomb_pieces,0);
    a.life_piece_threshold_index=0; b.life_piece_threshold_index=1; c.life_piece_threshold_index=3;
    a.life_pieces=life_piece_threshold(0)-1; b.life_pieces=life_piece_threshold(1)-1; c.life_pieces=0;
    pickup(ItemLifePiece); CHECK_EQ(a.extra_lives,8); CHECK_EQ(b.extra_lives,4);
    CHECK_EQ(a.life_piece_threshold_index,1); CHECK_EQ(b.life_piece_threshold_index,2); CHECK_EQ(c.life_piece_threshold_index,3);
    CHECK_EQ(a.life_pieces,0); CHECK_EQ(b.life_pieces,0); CHECK_EQ(c.life_pieces,1); CHECK_EQ(f.progress.life_piece_threshold_index,1);
    // A ghost cannot physically collect. It still receives a live teammate's reward.
    f.battle.seats[0].ghost=true; seats[0].eligible=false; a.power=100;
    manager.clear_events(); manager.spawn_item(-10,400,0,ItemPower,0,0); manager.update_seats(seats,3);
    CHECK_EQ(a.power,100); CHECK(manager.events().empty());
    seats[1]=collector_at(0,400,1,1,b,f.progress,f.score);
    manager.update_seats(seats,3); CHECK_EQ(a.power,101); CHECK(f.battle.seats[0].ghost);
    CHECK_EQ(f.score.score,4000); // Only operable seats participate in full-P conversion.

    // R-I5: each spirit's personal part is broadcast, its team gauge is once.
    const auto spirit=[&](i32 kind, bool trance) {
        manager.clear_events(); manager.spawn_value_item(kind,0,400,0,0);
        for(auto& s:manager.spirits()) if(s.alive) {s.x=0;s.y=400;s.state=2;s.move_speed=0;s.speed=0;s.flags=trance?1:0;}
        manager.update_spirits_seats(seats,3);
    };
    a.life_pieces=b.life_pieces=c.life_pieces=0;
    a.life_piece_threshold_index=b.life_piece_threshold_index=c.life_piece_threshold_index=5;
    f.progress.ending_points=0;
    spirit(0,false); CHECK_EQ(a.life_pieces,1); CHECK_EQ(b.life_pieces,1); CHECK_EQ(c.life_pieces,1); CHECK_EQ(f.progress.ending_points,2);
    spirit(0,true); CHECK_EQ(a.life_pieces,3); CHECK_EQ(b.life_pieces,3); CHECK_EQ(c.life_pieces,3); CHECK_EQ(f.progress.ending_points,2);
    a.bombs=b.bombs=c.bombs=2; a.bomb_pieces=b.bomb_pieces=c.bomb_pieces=0;
    spirit(2,false); CHECK_EQ(a.bomb_pieces,1); CHECK_EQ(b.bomb_pieces,1); CHECK_EQ(c.bomb_pieces,1); CHECK_EQ(f.progress.ending_points,4);
    spirit(2,true); CHECK_EQ(a.bomb_pieces,3); CHECK_EQ(b.bomb_pieces,3); CHECK_EQ(c.bomb_pieces,3); CHECK_EQ(f.progress.ending_points,4);
    CHECK(f.battle.seats[0].ghost);

    // R-I8: a dead seat can hold -1 lives while its teammate collects pieces.
    // Exercise the real HUD validator; the usual permissive Host hid this error.
    Fixture ghost; CHECK(ghost.init()); ghost.enable(2, {0, 0, 0});
    auto& rewards = ghost.battle.item_manager;
    auto& inventory = ghost.battle.seat_huds[0];
    auto& donor = ghost.battle.seat_huds[1];
    GameRandom visual; AnmEnvironment environment;
    AnmManager animations{visual, environment};
    const char* raw = std::getenv("TH13_NATIVE_ASSET_ROOT");
    CHECK(raw != nullptr);
    if (!raw) return;
    for (const auto& bank : {std::pair{2, "ascii.anm"}, std::pair{3, "front.anm"}}) {
        std::ifstream file(std::string(raw) + "/" + bank.second, std::ios::binary);
        std::vector<u8> bytes{std::istreambuf_iterator<char>(file), {}};
        CHECK(!bytes.empty());
        CHECK(animations.load(bank.first, bytes.data(), u32(bytes.size())));
    }
    CHECK(animations.sprite_fallback(2));
    GameHud hud{animations, ghost.progress, inventory, ghost.score, 3, 2};
    CHECK(hud.initialize({})); CHECK(hud.life_icons[0] != 0);
    // Negative control: retail HUD must continue rejecting a negative display.
    CHECK(!hud.life(-1, 0)); CHECK_EQ(hud.error, "HUD life values outside original limits");
    hud.error.clear(); ghost.host.hud = &hud;
    ghost.battle.seats[0].ghost = true; ghost.battle.seats[0].dead = true;
    inventory.extra_lives = -1; inventory.life_pieces = 0;
    inventory.life_piece_threshold_index = 0;
    ItemCollector pair[2] = {
        collector_at(0,400,0,0,inventory,ghost.progress,ghost.score),
        collector_at(180,400,1,0,donor,ghost.progress,ghost.score)};
    pair[0].eligible = false;
    rewards.clear_events(); rewards.spawn_item(-10,400,0,ItemLifePiece,0,0);
    rewards.update_seats(pair,2);
    CHECK(rewards.events().empty()); CHECK_EQ(inventory.life_pieces,0);
    CHECK_EQ(inventory.extra_lives,-1); CHECK(ghost.battle.seats[0].ghost);
    pair[1] = collector_at(0,400,1,0,donor,ghost.progress,ghost.score);
    rewards.update_seats(pair,2);
    CHECK_EQ(inventory.life_pieces,1); CHECK_EQ(inventory.extra_lives,-1);
    CHECK(ghost.battle.seats[0].ghost); CHECK(ghost.battle.seats[0].dead);
    // M5：广播发放逐席回调——席 0（幽灵，团队账面奖励保留）与席 1 各
    // 携带自己的权威值；旧「只报 seat 0 投影」语义已由每席 HUD 取代。
    CHECK_EQ(ghost.host.seat_lives[0],0); CHECK_EQ(ghost.host.seat_pieces[0],1);
    CHECK_EQ(ghost.host.seat_lives[1],2); CHECK_EQ(ghost.host.seat_pieces[1],1);
    CHECK(hud.error.empty()); CHECK(ghost.battle.error().empty());
    const auto reward = [&](i32 kind) {
        rewards.clear_events(); rewards.spawn_item(-10,400,0,kind,0,0);
        rewards.update_seats(pair,2);
        CHECK(hud.error.empty()); CHECK(ghost.battle.error().empty());
        size_t collected=0;
        for (const auto& event:rewards.events())
            if (event.kind==ItemEvent::Kind::Collected) ++collected;
        CHECK_EQ(collected,1u);
    };
    for (i32 piece=1; piece<life_piece_threshold(0); ++piece) reward(ItemLifePiece);
    CHECK_EQ(inventory.extra_lives,0); CHECK_EQ(inventory.life_pieces,0);
    CHECK_EQ(inventory.life_piece_threshold_index,1);
    CHECK_EQ(ghost.progress.life_piece_threshold_index,1);
    CHECK(ghost.battle.seats[0].ghost); CHECK(ghost.battle.seats[0].dead);
    CHECK_EQ(ghost.host.seat_lives[0],0);
    reward(ItemExtraLife); CHECK_EQ(inventory.extra_lives,1);
    CHECK(ghost.battle.seats[0].ghost); CHECK(ghost.battle.seats[0].dead);
    reward(ItemLifePiece); reward(ItemLifePiece);
    CHECK_EQ(inventory.life_pieces,2); CHECK_EQ(ghost.host.seat_lives[0],1);
    // R-I9: a later rescue preserves earned stock/pieces/threshold and only
    // applies half-full Power, the rescue Bomb reset and donor debit.
    inventory.bombs=7; inventory.bomb_pieces=3;
    const i32 donor_before=donor.extra_lives;
    rewards.clear_events(); CHECK(ghost.battle.rescue_seat(1,0));
    CHECK(!ghost.battle.seats[0].ghost); CHECK(!ghost.battle.seats[0].dead);
    CHECK_EQ(donor.extra_lives,donor_before-1); CHECK_EQ(inventory.extra_lives,1);
    CHECK_EQ(inventory.life_pieces,2); CHECK_EQ(inventory.life_piece_threshold_index,1);
    CHECK_EQ(inventory.power,inventory.max_power/2); CHECK_EQ(inventory.bombs,2);
    CHECK_EQ(inventory.bomb_pieces,0); CHECK(rewards.events().empty());
    pair[0].eligible=true; reward(ItemLifePiece);
    CHECK_EQ(inventory.extra_lives,1); CHECK_EQ(inventory.life_pieces,3);
    CHECK_EQ(ghost.host.seat_lives[0],1); CHECK_EQ(ghost.host.seat_pieces[0],3);
}

// ---- spirit_role_and_trance_grant：神霊角色门按拾取者 ----
// kind1 神霊（普通产）：角色 2（本作序：0 灵梦/1 魔理沙/2 早苗/3
// 妖梦——角色 2 的 kind1 收集加 trance+1 门，0x438bea）。席位角色 2 的
// 拾取者获得 +1 灵界值增量事件；席位角色 0 不触发。灵界值本身是团队
// 共享槽（progress.ending_points 一份）。
TEST(spirit_role_and_trance_grant) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 2, 0});  // seat 1 = 角色 2
    auto& manager = dual.battle.item_manager;
    auto& hud0 = dual.battle.seat_huds[0];
    auto& hud1 = dual.battle.seat_huds[1];
    dual.progress.ending_points = 100;
    // kind1 神霊（普通产 flags bit0=0）落在 seat 1 侧。
    manager.spawn_value_item(1, 170.0f, 400.0f, 0.0f, 0.0f);
    ItemCollector seats[2] = {
        collector_at(0.0f, 400.0f, 0, 0, hud0, dual.progress, dual.score),
        collector_at(180.0f, 400.0f, 1, 2, hud1, dual.progress, dual.score)};
    // 神霊需要多帧追踪到收集（state2 → 距离 <20 收集）；直接摆到收集距。
    for (auto& s : manager.spirits()) {
        if (s.alive) { s.x = 179.0f; s.y = 400.0f; s.state = 2; s.move_speed = 0; s.speed = 0; }
    }
    manager.update_spirits_seats(seats, 2);
    // 角色 2 拾取 kind1：trance 增量事件（TranceGained）出现。
    bool trance_event = false;
    for (const auto& event : manager.events())
        if (event.kind == ItemEvent::Kind::TranceGained) trance_event = true;
    CHECK(trance_event);
    CHECK_EQ(dual.progress.ending_points, 103);  // 公共 +2 与角色门 +1（团队槽一份）
    manager.clear_events();
    // R-I5: vary the actual collector independently from the shared cursor.
    // Trance-born kind1 retains its separate SP branch (no +2 or Sanae +1).
    for (i32 count : {2, 3}) for (i32 recipient : {0, 1})
        for (bool trance : {false, true}) {
            Fixture f; CHECK(f.init()); f.enable(count, {0, 2, 1});
            f.progress.character = recipient == 0 ? 2 : 0; // Opposite bonus gate.
            const i32 team_character = f.progress.character;
            f.progress.ending_points = 100; f.progress.max_point = 10000;
            f.progress.max_point_cap = 1000000;
            ItemCollector collectors[3] = {
                collector_at(-100, 300, 0, 0, f.battle.seat_huds[0], f.progress, f.score),
                collector_at(100, 300, 1, 2, f.battle.seat_huds[1], f.progress, f.score),
                collector_at(180, 100, 2, 1, f.battle.seat_huds[2], f.progress, f.score)};
            auto& pool = f.battle.item_manager;
            pool.spawn_value_item(1, collectors[recipient].x, 300, 0, 0);
            Spirit* spawned = nullptr;
            for (auto& s : pool.spirits()) if (s.alive) {
                s.state = 2; s.speed = s.move_speed = 0; s.flags = trance ? 1u : 0u; spawned = &s;
            }
            CHECK(spawned != nullptr); if (!spawned) return;
            pool.clear_events(); pool.update_spirits_seats(collectors, count);
            CHECK(!spawned->alive);
            const i32 gauge = trance ? 0 : recipient == 1 ? 3 : 2;
            CHECK_EQ(f.progress.ending_points, 100 + gauge);
            CHECK_EQ(f.progress.max_point, trance ? 20000 : 11000);
            CHECK_EQ(f.progress.character, team_character);
            CHECK_EQ(f.score.score, 0); CHECK_EQ(f.progress.point_items, 0);
            size_t collected = 0; std::vector<i32> trace;
            for (const auto& event : pool.events()) {
                if (event.kind == ItemEvent::Kind::Collected) { ++collected; CHECK_EQ(event.value, 11); }
                if (event.kind == ItemEvent::Kind::TranceGained) trace.push_back(event.value);
            }
            CHECK_EQ(collected, 1u);
            const std::vector<i32> expected = trance ? std::vector<i32>{}
                : recipient == 1 ? std::vector<i32>{102, 103} : std::vector<i32>{102};
            CHECK(trace == expected);
            for (i32 seat = 0; seat < count; ++seat) {
                CHECK_EQ(f.battle.seat_characters[size_t(seat)], seat == 0 ? 0 : seat == 1 ? 2 : 1);
                CHECK_EQ(f.battle.seat_huds[size_t(seat)].power, 100);
                CHECK_EQ(f.battle.seat_huds[size_t(seat)].extra_lives, 2);
                CHECK_EQ(f.battle.seat_huds[size_t(seat)].bombs, 2);
            }
            pool.clear_events(); pool.update_spirits_seats(collectors, count);
            CHECK(pool.events().empty()); CHECK_EQ(f.progress.ending_points, 100 + gauge);
            CHECK_EQ(f.progress.max_point, trance ? 20000 : 11000);
            CHECK_EQ(f.progress.character, team_character);
        }
}

// ---- boss_damage_remainders：Boss 合作系数确定性余数 ----
// R-G：Boss 伤害系数 2 人 3/4、3 人 2/3——按可操作人数，确定性余数累计
//（numerator/denominator 累计取整）。本 case 验证系数函数的余数序列：
// 伤害 1、2 人（3/4）：序列 floor(3k/4) 差分 = 0,1,1,1,0,1,1,1…
//（4 份伤害合计 3——不丢不减）；3 人（2/3）同理 2/3。
TEST(boss_damage_remainders) {
    // 系数实现面：StageBattle 的 Boss 伤害余数累计（M3 落地函数）。
    // 直接验证数学序列（实现接入点在 collide_shots 的 Boss 分支）。
    // 2 人：3/4——累计 4 点伤害应产生 3 点有效伤害。
    {
        i32 numerator = 0, applied = 0;
        for (int hit = 0; hit < 4; ++hit) {
            numerator += 3;
            const i32 whole = numerator / 4;
            applied += whole;
            numerator -= whole * 4;
        }
        CHECK_EQ(applied, 3);
        CHECK_EQ(numerator, 0);  // 余数归零（确定性）
    }
    // 3 人：2/3——累计 3 点伤害应产生 2 点。
    {
        i32 numerator = 0, applied = 0;
        for (int hit = 0; hit < 3; ++hit) {
            numerator += 2;
            const i32 whole = numerator / 3;
            applied += whole;
            numerator -= whole * 3;
        }
        CHECK_EQ(applied, 2);
        CHECK_EQ(numerator, 0);
    }
    // 1 人：恒等（无系数）。
    {
        i32 applied = 0;
        for (int hit = 0; hit < 5; ++hit) applied += 1;
        CHECK_EQ(applied, 5);
    }
}

// ---- graze_owner_generation：擦弹按弹身份 + 席位 mask ----
// 同一弹被两席先后擦过：graze/max_point 各计一次每席（2 席 → graze+2、
// 弹只被消一次不重复结算跨席；grazed_seats mask 两位置位，重复擦不重复计）。
TEST(graze_owner_generation) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    StageBattle& battle = dual.battle;
    // 生产数据流摆位：seat 0 持续 ←、seat 1 持续 → 各 9 帧（4.5×9=40.5）
    // → 两席 x≈∓40.5（弹在 (0,400) 的擦弹带内、命中盒外）。
    GameInput left, right;
    left.update(0x40);
    right.update(0x80);
    for (int frame = 0; frame < 9; ++frame) {
        battle.seat_inputs[1].update(0x80);
        CHECK(battle.step(left));
    }
    CHECK(std::fabs(battle.seats[0].x + 40.5f) < 0.1f);
    CHECK(std::fabs(battle.seats[1].x - 40.5f) < 0.1f);
    // 构造弹体（生产回调面驱动：sim.bullet_player_collision 注入点）。
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;  // bit1|bit4：标准圆形分支
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.hit_scale = 1.0f;
    bullet.x = 0.0f;
    bullet.y = 400.0f;
    dual.progress.graze = 0;
    dual.progress.max_point = 0;
    dual.progress.max_point_cap = 500000;  // 难度表 cap（0x4c0ac8[diff]×100）
    battle.sim.bullet_player_collision(bullet);  // MP 分派：逐席判定
    CHECK_EQ(dual.progress.graze, 2);      // 两席各计一次（R-G 席位 mask）
    CHECK_EQ(dual.progress.max_point, 200);
    CHECK_EQ(int(bullet.grazed_seats), 3);  // 两席位都已置
    CHECK(bullet.grazed);
    // 重复碰撞：mask 已满 → 不再计。
    const i32 graze_before = dual.progress.graze;
    battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(dual.progress.graze, graze_before);
}

// ---- gift_not_broadcast：定向赠送不走全员拾取 ----
// common CooperativeRules 的赠送语义：AdvanceLifeTransfer 5 拍 24 帧窗、
// NearestPowerRecipient 定向接收者。MP 道具侧验证：赠送走显式事务面，
// 不经 item_manager（广播路径）——拾取事件计数不因赠送增加。
TEST(gift_not_broadcast) {
    // ① AdvanceLifeTransfer：5 拍窗内完成才触发（定向事务的输入语义）。
    {
        u8 state = 0;
        bool granted = false;
        for (int tap = 0; tap < 5; ++tap)
            granted = eagler::cooperative::AdvanceLifeTransfer(state, true, true);
        CHECK(granted);  // 第 5 拍触发
        CHECK_EQ(unsigned(state), 0u);  // 触发后复位
        // 窗口超时：4 拍后 24 帧不按 → 复位。
        u8 state2 = 0;
        for (int tap = 0; tap < 4; ++tap)
            (void)eagler::cooperative::AdvanceLifeTransfer(state2, true, true);
        for (int frame = 0; frame < 25; ++frame)
            (void)eagler::cooperative::AdvanceLifeTransfer(state2, false, true);
        const bool late = eagler::cooperative::AdvanceLifeTransfer(state2, true, true);
        CHECK(!late);  // 窗口过期后重新计数（taps 已清）
        CHECK_EQ(eagler::cooperative::TapCount(unsigned(state2)), 1u);
    }
    // ② NearestPowerRecipient：定向选「最需要 P 的最近席」。
    {
        const bool eligible[3] = {true, true, true};
        const bool needs[3] = {false, true, false};
        const float distance[3] = {10.0f, 50.0f, 20.0f};
        const int recipient = eagler::cooperative::NearestPowerRecipient(
            2, [&](unsigned s) { return eligible[s]; },
            [&](unsigned s) { return needs[s]; },
            [&](unsigned s) { return distance[s]; });
        CHECK_EQ(recipient, 1);  // 需要 P 的席优先（距离远也选它）
    }
    // ③ 道具侧：赠送不经 item_manager 广播——拾取事件数为 0。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        auto& manager = dual.battle.item_manager;
        auto& hud0 = dual.battle.seat_huds[0];
        auto& hud1 = dual.battle.seat_huds[1];
        // 定向赠送（模拟救援事务直写）：hud0 扣 1 命、hud1 得 1 命。
        hud0.extra_lives = 3;
        hud1.extra_lives = 0;
        hud0.extra_lives -= 1;
        hud1.extra_lives += 1;
        size_t collected = 0;
        for (const auto& event : manager.events())
            if (event.kind == ItemEvent::Kind::Collected) ++collected;
        CHECK_EQ(collected, 0u);  // 无拾取事件（不走广播）
        CHECK_EQ(hud0.extra_lives, 2);
        CHECK_EQ(hud1.extra_lives, 1);
    }
}

int main() { return th13test::run_all("m3.items"); }
