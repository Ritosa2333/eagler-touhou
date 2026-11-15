#include "../ecl/ecl_test_fixtures.hpp"

#include "game/StageBattle.hpp"
#include "game/StageMath.hpp"

#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

using namespace th13;

// 00445c40:77-80：出膛先 polar 得初速度，出生位 = anchor+offset−初速度。
// 004464d0:25-30：同帧（age0）对全部弹做回调→polar→move，并经 00473780
// 量化到 0.01。故：
//   1. 直接注入弹在 age0 也位移；直线弹不转向/不改速但会 move；
//   2. 生产 .sht 出膛后，直线弹首帧净位移回到出膛锚点（补偿抵消初速度）；
//   3. 引导弹 age0 回调改变速度/角度，首帧位置 = 锚点−初速度+回调速度，
//      与未补偿位置相差一个初速度（oracle homing trace 的 cb_in 补偿）。
namespace {

constexpr float kUp = -1.5707963705062866f;

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        th13test::emit(main, 0, 23, 0, 0xff, 0, {600});  // 等待 600 帧
        th13test::emit(main, 0, 10, 0, 0xff, 0, {});     // 末尾 return
        stage_ = th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }

    void set_sht(std::vector<u8> sht) { sht_ = std::move(sht); }

    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "geometry.ecl") {
            out = stage_;
            return true;
        }
        if (name == "test.sht" && !sht_.empty()) {
            out = sht_;
            return true;
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        animations->push_back(name);
        return true;
    }

    std::vector<std::string>* animations = nullptr;

private:
    std::vector<u8> stage_;
    std::vector<u8> sht_;
};

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;

    bool init(const char* sht_name = nullptr) {
        provider.animations = &battle.sim.anim_list;
        if (!battle.load(provider, "geometry.ecl", 2, host, progress, player, score,
                         nullptr, sht_name))
            return false;
        if (!battle.error().empty()) return false;
        // 只保留本夹具目标：stage root（serial 0）既不是引导弹候选，
        // 也不参与本测试的伤害/碰撞观察。
        battle.sim.enemies.clear();
        return true;
    }
};

// 0x38 字节记录：+0 u8 interval、+1 u8 phase、+2 i16 damage、+4/+8 f32 offset、
// +0x14 f32 angle、+0x18 f32 speed、+0x20 i8 anchor、+0x2c u32 behavior。
struct ShtRecordSpec {
    u8 interval = 3;
    u8 phase = 0;
    i16 damage = 0;
    float x = 0, y = 0, width = 8, height = 8;
    float angle = kUp;
    float speed = 6.0f;
    i8 anchor = 0;
    u8 type = 0;
    i16 script = 0;  // 解析值 = 原始 i16 + 10
    u32 behavior = 0;
};

std::vector<u8> build_sht(const ShtRecordSpec& spec) {
    std::vector<u8> sht(0x140, 0);
    const u16 blocks = 1;
    std::memcpy(sht.data() + 2, &blocks, sizeof blocks);
    const u32 block_offset = 0;  // 块 0 记录从 0xf4 起
    std::memcpy(sht.data() + 0xc8, &block_offset, sizeof block_offset);
    const size_t at = 0xf4;
    sht[at] = spec.interval;
    sht[at + 1] = spec.phase;
    const u16 damage = u16(spec.damage);
    std::memcpy(sht.data() + at + 2, &damage, sizeof damage);
    std::memcpy(sht.data() + at + 4, &spec.x, sizeof spec.x);
    std::memcpy(sht.data() + at + 8, &spec.y, sizeof spec.y);
    std::memcpy(sht.data() + at + 0x0c, &spec.width, sizeof spec.width);
    std::memcpy(sht.data() + at + 0x10, &spec.height, sizeof spec.height);
    std::memcpy(sht.data() + at + 0x14, &spec.angle, sizeof spec.angle);
    std::memcpy(sht.data() + at + 0x18, &spec.speed, sizeof spec.speed);
    sht[at + 0x20] = u8(spec.anchor);
    sht[at + 0x21] = spec.type;
    const u16 script = u16(spec.script);
    std::memcpy(sht.data() + at + 0x22, &script, sizeof script);
    std::memcpy(sht.data() + at + 0x2c, &spec.behavior, sizeof spec.behavior);
    sht[at + 0x38] = 0xff;  // 终止记录（首字节 i8 < 0）
    return sht;
}

StageEnemy* spawn_live(Fixture& fixture, float x, float y) {
    StageEnemy* enemy = fixture.battle.sim.spawn_enemy("main", x, y, 100.0f, nullptr, false);
    CHECK(enemy != nullptr);
    if (!enemy) return nullptr;
    // serial 0 是引导弹“未锁定”哨兵；夹具目标必须是真实非 0 序号。
    CHECK(enemy->serial != 0);
    CHECK(enemy->threads != nullptr);
    return enemy;
}

PlayerShot make_shot(u32 target_serial, float speed) {
    PlayerShot shot;
    shot.x = 0;
    shot.y = 200;  // 场内坐标，远离上下边界
    shot.angle = kUp;
    shot.speed = speed;
    shot.alive = true;
    shot.behavior = 1;
    shot.target_serial = target_serial;
    shot.width = 8;
    shot.height = 8;
    shot.damage = 0;  // 本测试只观察弹道，不制造命中
    return shot;
}

const PlayerShot* only_shot(const Fixture& fixture) {
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    return fixture.battle.shots.empty() ? nullptr : &fixture.battle.shots.front();
}

// 004464d0 move 后的 00473780 floor 量化：期望值按生产同一运算序计算。
void check_move_from(const PlayerShot& shot, float from_x, float from_y) {
    const float c = std::cos(shot.angle);
    const float s = std::sin(shot.angle);
    CHECK_EQ(shot.x, round001(from_x + shot.speed * c));
    CHECK_EQ(shot.y, round001(from_y + shot.speed * s));
}

void fire_first_sht_shot(Fixture& fixture) {
    GameInput input;
    input.update(0x01);
    for (int frame = 1; frame <= 20; ++frame) CHECK(fixture.battle.step(input));
}

float initial_velocity_x(float angle, float speed) {
    return float(std::cos(double(angle)) * double(speed));
}
float initial_velocity_y(float angle, float speed) {
    return float(std::sin(double(angle)) * double(speed));
}

}  // namespace

TEST(player_shot_polar_velocity_rounds_after_speed_multiplication) {
    // Retail474f00 outputs with32-bit inputs. These origins exercise the
    // subsequent473780 floor boundary; old float-rounded trig shifts0.01.
    struct Case { float angle, x, y; u32 axis, expected; };
    const Case cases[] = {
        {-1.3f,-17.594993591308594f,200.f,0,3246391296u},
        {-.83f,-31.949254989624023f,200.f,0,3252635566u},
        {-.83f,0.f,-27.56241226196289f,1,3254774661u},
        {2.3f,0.f,-127.89423370361328f,1,3270958121u},
    };
    for (const auto& c : cases) {
        Fixture f;
        CHECK(f.init());
        PlayerShot shot;
        shot.alive=true;shot.angle=c.angle;shot.speed=6;
        shot.x=c.x;shot.y=c.y;shot.script=0;
        f.battle.shots.push_back(shot);
        CHECK(f.battle.step(f.idle));
        CHECK_EQ(f.battle.shots.size(),size_t(1));
        if (!f.battle.shots.empty()) {
            const auto& after=f.battle.shots.front();
            CHECK_EQ(th13test::bits_of(c.axis ? after.y : after.x),c.expected);
        }
    }
}

TEST(homing_behavior1_updates_on_age_zero) {
    Fixture fixture;
    CHECK(fixture.init());
    if (!fixture.battle.error().empty()) return;

    StageEnemy* target = spawn_live(fixture, 0.0f, 100.0f);
    if (!target) return;
    fixture.battle.shots.push_back(make_shot(target->serial, 6.0f));

    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.error().empty());
    const PlayerShot* shot = only_shot(fixture);
    if (!shot) return;
    CHECK(shot->alive);
    CHECK_EQ(shot->age, 1);
    CHECK_EQ(shot->target_serial, target->serial);
    CHECK(shot->speed > 6.0f);       // 正前方目标：回调加速
    CHECK(shot->y < 200.0f);         // age0 已 move
    check_move_from(*shot, 0.0f, 200.0f);
}

TEST(straight_shot_moves_without_turning_or_speed_change_on_age_zero) {
    Fixture fixture;
    CHECK(fixture.init());
    if (!fixture.battle.error().empty()) return;

    // 与正控相同的活目标/初速/注入位置，只把回调换成直线弹 0。
    StageEnemy* target = spawn_live(fixture, 0.0f, 100.0f);
    if (!target) return;

    PlayerShot straight = make_shot(0, 6.0f);
    straight.behavior = 0;
    fixture.battle.shots.push_back(straight);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.error().empty());
    const PlayerShot* shot = only_shot(fixture);
    if (!shot) return;
    CHECK(shot->alive);
    CHECK_EQ(shot->age, 1);
    CHECK_EQ(shot->angle, kUp);      // 不转向
    CHECK_EQ(shot->speed, 6.0f);     // 不改速
    CHECK(shot->y < 200.0f);         // 但 age0 会 move
    CHECK_EQ(shot->target_serial, u32(0));
    check_move_from(*shot, 0.0f, 200.0f);
}

TEST(homing_behavior1_turns_toward_side_target_on_age_zero) {
    Fixture fixture;
    CHECK(fixture.init());
    if (!fixture.battle.error().empty()) return;

    StageEnemy* target = spawn_live(fixture, 100.0f, 200.0f);
    if (!target) return;
    fixture.battle.shots.push_back(make_shot(target->serial, 6.0f));

    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.error().empty());
    const PlayerShot* shot = only_shot(fixture);
    if (!shot) return;
    CHECK_EQ(shot->target_serial, target->serial);
    CHECK(shot->speed < 6.0f);                       // 大角度误差减速
    CHECK(shot->angle > kUp);                        // 朝右目标转过
    CHECK(shot->x > 0.0f);
    CHECK(shot->y < 200.0f);
    CHECK_EQ(shot->age, 1);
    check_move_from(*shot, 0.0f, 200.0f);
}

TEST(invalid_target_clears_serial_then_relocks_on_next_frame) {
    Fixture fixture;
    CHECK(fixture.init());
    if (!fixture.battle.error().empty()) return;

    StageEnemy* stale = spawn_live(fixture, 0.0f, 100.0f);
    StageEnemy* fresh = spawn_live(fixture, 30.0f, 150.0f);
    if (!stale || !fresh) return;
    CHECK(stale->serial != fresh->serial);
    const u32 stale_serial = stale->serial;
    const u32 fresh_serial = fresh->serial;

    stale->dead = true;  // 旧目标失效，弹仍持旧 serial
    fixture.battle.shots.push_back(make_shot(stale_serial, 6.0f));

    // 首帧：只清 target_serial，走无目标分支 speed+0.1 并按旧角度 move；
    // 不转向、不重新锁定。
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.error().empty());
    const PlayerShot* first = only_shot(fixture);
    if (!first) return;
    CHECK_EQ(first->target_serial, u32(0));
    CHECK(first->speed > 6.0f);
    CHECK(first->y < 200.0f);
    CHECK_EQ(first->age, 1);
    check_move_from(*first, 0.0f, 200.0f);

    // 下一帧才从 0 哨兵重新锁定近目标。
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.error().empty());
    const PlayerShot* second = only_shot(fixture);
    if (!second) return;
    CHECK_EQ(second->target_serial, fresh_serial);
    CHECK_EQ(second->age, 2);
}

TEST(sht_spawn_straight_first_frame_returns_to_launch_anchor) {
    Fixture fixture;
    ShtRecordSpec spec;
    spec.behavior = 0;
    fixture.provider.set_sht(build_sht(spec));
    fixture.player.power = 0;  // 块 0
    CHECK(fixture.init("test.sht"));
    if (!fixture.battle.error().empty()) return;

    fire_first_sht_shot(fixture);
    CHECK_EQ(fixture.battle.sim.frame, 20u);
    const PlayerShot* shot = only_shot(fixture);
    if (!shot) return;
    CHECK_EQ(shot->age, 1);
    CHECK_EQ(shot->behavior, 0);
    CHECK_EQ(shot->speed, spec.speed);
    CHECK_EQ(shot->angle, spec.angle);

    const float anchor_x = fixture.battle.player_x + spec.x;
    const float anchor_y = fixture.battle.player_y + spec.y;
    const float v0x = initial_velocity_x(spec.angle, spec.speed);
    const float v0y = initial_velocity_y(spec.angle, spec.speed);
    const float v1x = shot->speed * std::cos(shot->angle);
    const float v1y = shot->speed * std::sin(shot->angle);

    // 首帧位置 = 锚点 − 初速度 + 本帧速度；直线弹两速度相同，净位移为 0。
    CHECK_EQ(shot->x, round001(anchor_x - v0x + v1x));
    CHECK_EQ(shot->y, round001(anchor_y - v0y + v1y));
    CHECK_EQ(shot->y, round001(anchor_y));
    // 若未减去初速度，位置会比实测低一个初速度；实测必须带真实补偿。
    CHECK(shot->y > round001(anchor_y + v1y));
}

TEST(sht_spawn_homing_first_frame_has_real_initial_velocity_compensation) {
    Fixture fixture;
    ShtRecordSpec spec;
    spec.behavior = 1;
    fixture.provider.set_sht(build_sht(spec));
    fixture.player.power = 0;  // 块 0
    CHECK(fixture.init("test.sht"));
    if (!fixture.battle.error().empty()) return;

    // 出生位先减去初速度（y=406）；目标须落在锁定半径 256 内。
    StageEnemy* target = spawn_live(fixture, 0.0f, 200.0f);
    if (!target) return;
    fire_first_sht_shot(fixture);
    const PlayerShot* shot = only_shot(fixture);
    if (!shot) return;
    CHECK_EQ(shot->behavior, 1);
    CHECK_EQ(shot->target_serial, target->serial);
    CHECK(shot->speed > spec.speed);  // age0 回调已执行

    const float anchor_x = fixture.battle.player_x + spec.x;
    const float anchor_y = fixture.battle.player_y + spec.y;
    const float v0x = initial_velocity_x(spec.angle, spec.speed);
    const float v0y = initial_velocity_y(spec.angle, spec.speed);
    const float v1x = shot->speed * std::cos(shot->angle);
    const float v1y = shot->speed * std::sin(shot->angle);

    // 首帧位置 = 锚点 − 初速度 + 回调后速度，不是锚点 + 回调后速度。
    CHECK_EQ(shot->x, round001(anchor_x - v0x + v1x));
    CHECK_EQ(shot->y, round001(anchor_y - v0y + v1y));
    CHECK(shot->y > round001(anchor_y + v1y));  // 补偿量 = −v0y（约 6）
    CHECK(shot->y < anchor_y);                  // 加速后的净位移约 0.2
}

TEST(respawn_options_snap_on_first_active_move_then_resume_interpolation) {
    Fixture fixture;
    auto sht = build_sht(ShtRecordSpec{});
    const float speed = 4.5f;
    std::memcpy(sht.data() + 0x10, &speed, 4);
    // Three-option layout from pl00a.sht. No shots are fired in this case.
    const float offsets[2][3][2] = {
        {{-32, 16}, {32, 16}, {0, 32}},
        {{-16, -20}, {16, -20}, {0, -30}},
    };
    for (int mode = 0; mode < 2; ++mode)
        for (int i = 0; i < 3; ++i)
            std::memcpy(sht.data() + 0x28 + mode * 0x50 + (3 + i) * 8,
                        offsets[mode][i], 8);
    fixture.provider.set_sht(sht);
    fixture.player.power = 300;
    CHECK(fixture.init("test.sht"));
    fixture.battle.restore_replay_player(0, 51200, true);
    fixture.battle.player_phase = StageBattle::PlayerPhase::Entering;
    fixture.battle.player_phase_frame = 59;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.options[0].x, -16.f);
    CHECK_EQ(fixture.battle.options[0].y, 380.f);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK(fixture.battle.player_phase == StageBattle::PlayerPhase::Active);
    // Direct original441c30 ->442960 capture: reset is consumed on this
    // movement call, including when case0 falls through to case1.
    CHECK_EQ(fixture.battle.options[0].x, -32.f);
    CHECK_EQ(fixture.battle.options[0].y, 416.f);
    CHECK_EQ(fixture.battle.options[2].y, 432.f);
    GameInput up;
    up.update(0x10);
    CHECK(fixture.battle.step(up));
    CHECK_EQ(fixture.battle.options[0].y, 53076.f / 128.f);
    CHECK(fixture.battle.step(up));
    CHECK_EQ(fixture.battle.options[0].y, 52782.f / 128.f);
}

int main() {
    return th13test::run_all("player homing");
}
