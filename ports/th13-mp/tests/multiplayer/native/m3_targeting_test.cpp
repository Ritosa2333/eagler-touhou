// M3 原生固定 case（目标解析：最近合格玩家）。
// 被测生产面（RULES R-G/R-I7/R-I8）：
//   - StageSim::nearest_player（平距平方最小、平局取小席位、全不合格
//     回退 seat 0）；
//   - StageSim.seat_x/seat_y/seat_eligible（step_mp 帧尾写入的多席目标面）；
//   - StageGvars -9965/-9964/-9991/-9990/-9989/-9944（ECL 变量按敌机位置
//     取最近合格玩家）；
//   - StageCommands fire_et 出生瞄准角（发弹时点求值，不缓存）。
//
// 固定 case 映射（workflow-cases.json m3.*）：
//   target_all_consumers_ties → nearest 单元语义 + gvars/发弹消费者 + 平局
//   drop_rng_unchanged        → 多席位置不影响掉落 RNG（敌死亡掉灵流一致）
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
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
                // 文件名 → 工作根 /work：逐次探测含 artifacts/ 的根。
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

// 把每席摆到指定位（直接写权威槽位；不经过输入循环）。
void place_seats(StageBattle& battle, std::array<float, 3> xs, std::array<float, 3> ys) {
    for (i32 seat = 0; seat < battle.seat_count; ++seat) {
        battle.seats[size_t(seat)].x = xs[size_t(seat)];
        battle.seats[size_t(seat)].y = ys[size_t(seat)];
        battle.seats[size_t(seat)].fixed_x = i32(xs[size_t(seat)] * 128.0f);
        battle.seats[size_t(seat)].fixed_y = i32(ys[size_t(seat)] * 128.0f);
    }
    battle.sim.seat_count = battle.seat_count;
    for (i32 seat = 0; seat < battle.seat_count; ++seat) {
        battle.sim.seat_x[size_t(seat)] = battle.seats[size_t(seat)].x;
        battle.sim.seat_y[size_t(seat)] = battle.seats[size_t(seat)].y;
        battle.sim.seat_eligible[size_t(seat)] = true;
    }
}

}  // namespace

// ---- target_all_consumers_ties：最近合格玩家全语义 ----
TEST(target_all_consumers_ties) {
    // ① nearest_player 单元语义（直接驱动共享面）。
    {
        StageSim sim;
        sim.seat_count = 3;
        sim.seat_x = {-100.0f, 0.0f, 100.0f};
        sim.seat_y = {400.0f, 400.0f, 400.0f};
        sim.seat_eligible = {true, true, true};
        float x = 0, y = 0;
        sim.nearest_player(90.0f, 400.0f, x, y);   // 距 seat2=10、seat1=90
        CHECK_EQ(x, 100.0f);
        CHECK_EQ(y, 400.0f);
        sim.nearest_player(-90.0f, 400.0f, x, y);  // 距 seat0=10
        CHECK_EQ(x, -100.0f);
        // 平局取小席位：(-50,400) 距 seat0=50、seat1=50。
        sim.nearest_player(-50.0f, 400.0f, x, y);
        CHECK_EQ(x, -100.0f);  // seat 0 胜出
        // 不合格席排除：seat2 幽灵位 → (90,400) 最近变 seat1。
        sim.seat_eligible = {true, true, false};
        sim.nearest_player(90.0f, 400.0f, x, y);
        CHECK_EQ(x, 0.0f);
        // 全不合格回退 seat 0。
        sim.seat_eligible = {false, false, false};
        sim.nearest_player(90.0f, 400.0f, x, y);
        CHECK_EQ(x, -100.0f);
        // 单席退化：seat_count==0 → player_x/y。
        StageSim single;
        single.player_x = 12.5f;
        single.player_y = 300.0f;
        single.nearest_player(0.0f, 0.0f, x, y);
        CHECK_EQ(x, 12.5f);
        CHECK_EQ(y, 300.0f);
    }
    // ② 战斗面：2 席分立，敌机侧 gvars 瞄准取最近席。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        place_seats(dual.battle, {-120.0f, 120.0f, 0.0f}, {400.0f, 400.0f, 0.0f});
        // ECL 变量 -9989（ANGLE_PLAYER）：敌机在 (110,400) → 最近 seat1
        // (120,400)：角度 atan2(0,10)=0（正右）。
        auto enemy = std::make_unique<StageEnemy>();
        enemy->serial = 1;
        enemy->script_id = 1;
        enemy->position[0] = 110.0f;
        enemy->position[1] = 400.0f;
        StageSim& sim = dual.battle.sim;
        sim.current = enemy.get();
        float angle = 0;
        sim.gvars.floating(-9989, angle);
        CHECK(std::fabs(angle - 0.0f) < 1e-4f);
        // 敌机在 (-110,400) → 最近 seat0 (-120,400)：atan2(0,-10)=π。
        enemy->position[0] = -110.0f;
        sim.gvars.floating(-9989, angle);
        CHECK(std::fabs(angle - 3.1415927f) < 1e-4f);
        // -9944 距离：敌机 (110,400) → seat1 距离 10。
        enemy->position[0] = 110.0f;
        float distance = 0;
        sim.gvars.floating(-9944, distance);
        CHECK(std::fabs(distance - 10.0f) < 1e-3f);
        // 坐标变量 -9991/-9990：敌机 (110,400) → seat1 (120,400)。
        float px = 0, py = 0;
        sim.gvars.floating(-9991, px);
        sim.gvars.floating(-9990, py);
        CHECK_EQ(px, 120.0f);
        CHECK_EQ(py, 400.0f);
        sim.current = nullptr;
    }
    // ③ 幽灵位消费者：seat1 不合格 → 敌机 (110,400) 瞄准回 seat0。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        place_seats(dual.battle, {-120.0f, 120.0f, 0.0f}, {400.0f, 400.0f, 0.0f});
        dual.battle.sim.seat_eligible[1] = false;
        auto enemy = std::make_unique<StageEnemy>();
        enemy->serial = 1;
        enemy->script_id = 1;
        enemy->position[0] = 110.0f;
        enemy->position[1] = 400.0f;
        StageSim& sim = dual.battle.sim;
        sim.current = enemy.get();
        float px = 0;
        sim.gvars.floating(-9991, px);
        CHECK_EQ(px, -120.0f);
        sim.current = nullptr;
    }
}

// ---- drop_rng_unchanged：掉落 RNG 与席位数无关 ----
// 同一合成 stage（无掉落表，敌机恒零掉落）下：单席与 2/3 席实例的
// game RNG index 逐帧一致（目标面写入不消耗 RNG；步进序不变）。
TEST(drop_rng_unchanged) {
    Fixture single, dual, triple;
    CHECK(single.init()); CHECK(dual.init()); CHECK(triple.init());
    dual.enable(2, {0, 0, 0});
    triple.enable(3, {0, 0, 0});
    // 摆出非对称席位（若目标选择错误地消耗 RNG 或影响敌步进序会显形）。
    place_seats(dual.battle, {-100.0f, 100.0f, 0.0f}, {400.0f, 300.0f, 0.0f});
    place_seats(triple.battle, {-100.0f, 0.0f, 100.0f}, {400.0f, 300.0f, 200.0f});
    GameInput none;
    for (int frame = 0; frame < 120; ++frame) {
        CHECK(single.battle.step(none));
        CHECK(dual.battle.step(none));
        CHECK(triple.battle.step(none));
        CHECK_EQ(dual.battle.sim.random.index(), single.battle.sim.random.index());
        CHECK_EQ(triple.battle.sim.random.index(), single.battle.sim.random.index());
        CHECK_EQ(dual.battle.sim.frame, single.battle.sim.frame);
        CHECK_EQ(triple.battle.sim.frame, single.battle.sim.frame);
    }
}

int main() { return th13test::run_all("m3.targeting"); }
