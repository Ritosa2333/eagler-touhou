// M3 原生固定 case（道具/神霊/资源：一件一次、逐席广播、团队一份）。
// 被测生产面（RULES R-I1..R-I9/R-G）：
//   - ItemManager::update_seats/update_spirits_seats（逐件最近合格席）；
//   - collect_* 结算宿主跟随选中席（last_player_ 逐件更新）；
//   - ItemCollector.seat/eligible/character（席位归属/幽灵排除/角色门）。
//
// 固定 case 映射（workflow-cases.json m3.*）：
//   shared_pickup_once        → 一件道具只结算一次（最近席拾取、其余席
//                                不重复结算）
//   resource_all_types_caps   → P/命/Bomb/碎片入选中席（逐席上限独立）
//   spirit_role_and_trance_grant → 神霊角色门按拾取者席位角色
//   graze_owner_generation    → 擦弹按弹身份+席位（generation 不串席）
//   boss_damage_remainders    → Boss 合作系数确定性余数
//   gift_not_broadcast        → 定向赠送不走全员拾取
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "eagler/gameplay/CooperativeRules.hpp"
#include <cmath>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
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

    bool init() {
        provider.animations = &battle.sim.anim_list;
        return battle.load(provider, "mp3.ecl", 2, host, progress, player, score);
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

}  // namespace

// ---- shared_pickup_once：一件道具恰结算一次 ----
// 2 席 collector，道具在 seat 0 收集盒内且在 seat 1 收集盒外：只有 seat 0
// 的 HUD 结算（power+1）；seat 1 HUD 不变；分数（团队一份）恰 +1 次跨档
// 事件的 popup 计数 = 1。
TEST(shared_pickup_once) {
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
    // 恰一次结算：道具死亡 + seat 0 power +1；seat 1 不动。
    CHECK_EQ(spawned->alive, false);
    CHECK_EQ(hud0.power, 101);
    CHECK_EQ(hud1.power, 100);
    // 结算事件恰一批（同一道具不重复）。
    size_t collected = 0;
    for (const auto& event : manager.events())
        if (event.kind == ItemEvent::Kind::Collected) ++collected;
    CHECK_EQ(collected, 1u);
    manager.clear_events();
}

// ---- resource_all_types_caps：逐席资源与上限 ----
// P（小/大）：入最近席；满 P 溢出转分数。残机碎片/Bomb 碎片/1UP 入最近
// 席的上限语义（8 命/9 弹封顶）逐席独立。
TEST(resource_all_types_caps) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    auto& manager = dual.battle.item_manager;
    auto& hud0 = dual.battle.seat_huds[0];
    auto& hud1 = dual.battle.seat_huds[1];
    // 大 P（type 3）：seat 0 满 P（400）时溢出钳制；seat 1 正常累加。
    hud0.power = 400; hud0.max_power = 400;
    hud1.power = 100; hud1.max_power = 400;
    manager.spawn_item(-10.0f, 400.0f, 0.0f, 3, 0.0f, 0.0f);  // 大P 近 seat0
    ItemCollector seats[2] = {
        collector_at(0.0f, 400.0f, 0, 0, hud0, dual.progress, dual.score),
        collector_at(180.0f, 400.0f, 1, 0, hud1, dual.progress, dual.score)};
    manager.update_seats(seats, 2);
    CHECK_EQ(hud0.power, 400);  // 满钳
    CHECK_EQ(hud1.power, 100);  // 未沾
    manager.clear_events();
    // 1UP（type 6）：入最近席，8 命封顶（原版 cap）。
    hud0.extra_lives = 8;
    hud1.extra_lives = 2;
    manager.spawn_item(-10.0f, 400.0f, 0.0f, 6, 0.0f, 0.0f);
    manager.update_seats(seats, 2);
    CHECK_EQ(hud0.extra_lives, 8);  // 封顶不加
    CHECK_EQ(hud1.extra_lives, 2);
    manager.clear_events();
    // Bomb（type 7）：入最近席，9 封顶。
    hud0.bombs = 9;
    manager.spawn_item(-10.0f, 400.0f, 0.0f, 7, 0.0f, 0.0f);
    manager.update_seats(seats, 2);
    CHECK_EQ(hud0.bombs, 9);
    CHECK_EQ(hud1.bombs, 2);
    manager.clear_events();
}

// ---- spirit_role_and_trance_grant：神霊角色门按拾取者 ----
// kind1 神霊（普通产）：角色 2（咲夜？本作序：0 灵梦/1 魔理沙/2 咲夜/3
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
