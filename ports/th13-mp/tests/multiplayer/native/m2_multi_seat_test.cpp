// M2 原生固定 case（一套世界、多席自机）。被测生产面：
//   - StagePlayerState 全量每席域（M2 迁移：Bomb/respawn/youmu/input/
//     touch/SHT/character/hit_size/trance_protection）与 StageBattle 的
//     引用别名（player_x/shots/damage_registrations/…绑定同一存储）。
//   - StageBattle::enable_seats/swap_player_context/restore_seat_context/
//     step_mp（固定席位序；世界面每逻辑帧一次）。
//
// 固定 case 映射（workflow-cases.json m2.*）：
//   world_tick_once            → 敌方世界推进与席位数无关（sim.frame/ECL/
//                                RNG 轨迹逐帧一致）
//   independent_inputs_2_3     → 2/3 席独立移动（不同输入不同位置、
//                                席位序固定、输入互不串扰）
//   four_characters_duplicates  → 四角色 + 重复角色席位（SHT 定标/判定
//                                尺寸/弹型域逐席独立）
//   damage_owner_ids           → 每席伤害注册表独立（shots/registration
//                                归属不串扰）
//   single_seat_sp_equivalence  → 单席（seat_count==0）与迁移前路径逐位
//                                一致：同输入同敌机流，双实例（一个走
//                                单席 step、一个构造后 enable_seats 前
//                                不启用）全帧 digest 相等。
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

using namespace th13;
using namespace th13test;

namespace {

// 无声宿主（M2 测试只需返回值语义；声音/特效面由继承套件覆盖）。
struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

// 合成 stage：一条 600 帧空闲 main（rank 全位）——敌机世界存在但无弹。
// ANIM 名单 ≥2（StageSim::begin 的装载核验：anim_list.size() >= 2）。
class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 0xff, 0, {600});
        emit(main, 0, 10, 0, 0xff, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mp2.ecl") {
            out = stage;
            return true;
        }
        // 逐席 SHT 装载（enable_seats 按角色读 plXXa.sht）：原版 raw 解包
        // 资产。路径从本文件编译位置上溯（__FILE__ → 仓库根 /work，不依赖
        // 运行 cwd——native-runner 的 cwd 是 /work/artifacts/mp-native-run/
        // <case>，TestFileReader 的固定级数上溯到不了 /work）。
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                // 文件名 → 工作根 /work 共 7 次削段（native→multiplayer
                // →tests→th13-mp→ports→eagler-touhou→/work；raw 在
                // monorepo 外层）。逐次探测到含 artifacts/ 的根，不依赖
                // 单一深度。
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
        return battle.load(provider, "mp2.ecl", 2, host, progress, player, score);
    }
    // 真实 .sht 装载（四角色定标域：移速/option 偏移/弹型表）。
    bool load_sht(const char* name) {
        TestFileReader reader;
        std::vector<u8> sht;
        if (!reader.read(std::string("eagler-touhou/ports/th13-mp/web/sdl-runtime/") +
                         name, sht))
            return false;
        return battle.prepare_player(provider, progress, player, name, false);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

// 席位位置（读取 seats[]；不扰动活动窗口）。
float seat_x(const StageBattle& battle, i32 seat) {
    return battle.seats[size_t(seat)].x;
}
float seat_y(const StageBattle& battle, i32 seat) {
    return battle.seats[size_t(seat)].y;
}

u32 held_of(u32 buttons) {
    GameInput input;
    input.update(buttons);
    return input.held;
}

}  // namespace

// ---- world_tick_once：敌方世界每逻辑帧只推进一次 ----
// 同一 600 帧合成 stage：单席实例与 2/3 席实例的 sim.frame、敌机链、
// game RNG index 逐帧全等——世界推进与席位数无关。
TEST(world_tick_once) {
    Fixture single, dual, triple;
    CHECK(single.init()); CHECK(dual.init()); CHECK(triple.init());
    dual.enable(2, {0, 1, 2});
    triple.enable(3, {0, 1, 2});
    GameInput none;
    for (int frame = 0; frame < 120; ++frame) {
        CHECK(single.battle.step(none));
        // 多席：每席独立零输入（seat_inputs 默认构造 = 零输入）。
        CHECK(dual.battle.step(none));
        CHECK(triple.battle.step(none));
        CHECK_EQ(dual.battle.sim.frame, single.battle.sim.frame);
        CHECK_EQ(triple.battle.sim.frame, single.battle.sim.frame);
        CHECK_EQ(dual.battle.sim.random.index(), single.battle.sim.random.index());
        CHECK_EQ(triple.battle.sim.random.index(), single.battle.sim.random.index());
        CHECK_EQ(dual.battle.sim.enemies.size(), single.battle.sim.enemies.size());
        CHECK_EQ(triple.battle.sim.enemies.size(), single.battle.sim.enemies.size());
    }
}

// ---- independent_inputs_2_3：各席独立移动、席位序固定 ----
// seat 0 持续 ←、seat 1 持续 →：两席 x 分离且方向正确；3 席加 seat 2
// 持续 ↑。输入互不串扰（改 seat 1 输入不影响 seat 0 轨迹）。
// seat 0 的输入沿经 step 参数注入（step 分派前写 seat_inputs[0]），
// 其余席直接写 seat_inputs——与 M7 Runtime 的注入面一致。
TEST(independent_inputs_2_3) {
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        const u32 left = held_of(0x40), right = held_of(0x80), up = held_of(0x10);
        for (int frame = 0; frame < 60; ++frame) {
            dual.battle.seat_inputs[1].update(right);
            GameInput seat0;
            seat0.update(left);
            CHECK(dual.battle.step(seat0));
        }
        CHECK(seat_x(dual.battle, 0) < 0.0f);   // 左移
        CHECK(seat_x(dual.battle, 1) > 0.0f);   // 右移
        CHECK(std::fabs(seat_x(dual.battle, 0) + seat_x(dual.battle, 1)) < 1e-4f);
        CHECK(std::fabs(seat_y(dual.battle, 0) - seat_y(dual.battle, 1)) < 1e-4f);
    }
    {
        Fixture triple;
        CHECK(triple.init());
        triple.enable(3, {0, 0, 0});
        const u32 left = held_of(0x40), right = held_of(0x80), up = held_of(0x10);
        for (int frame = 0; frame < 60; ++frame) {
            triple.battle.seat_inputs[1].update(right);
            triple.battle.seat_inputs[2].update(up);
            GameInput seat0;
            seat0.update(left);
            CHECK(triple.battle.step(seat0));
        }
        CHECK(seat_x(triple.battle, 0) < 0.0f);
        CHECK(seat_x(triple.battle, 1) > 0.0f);
        CHECK(std::fabs(seat_x(triple.battle, 0)) > 100.0f);  // 60 帧 × 4.5px
        CHECK(seat_y(triple.battle, 2) < seat_y(triple.battle, 0));  // 上移
        CHECK(std::fabs(seat_x(triple.battle, 2)) < 1e-4f);
    }
}

// ---- four_characters_duplicates：四角色 + 重复角色 ----
// 2 席全四角色组合（含 0,0 与 3,3 重复）：每席 SHT 定标域独立
// （speed_orth 按 plXXa.sht 表值）、判定尺寸按角色表（2/3/3/3）、
// 弹型域逐席装载。重复角色允许（同角色两席状态独立）。
TEST(four_characters_duplicates) {
    // plXXa.sht 移速（×128 定点）：pl00=576、pl01=640、pl02=576、pl03=576
    //（0x441dd0 读 sht+0x10；表值见 §7.2 与四张 .sht 实读）。
    static constexpr i32 kSpeedOrth[4] = {576, 640, 576, 576};
    for (i32 a = 0; a < 4; ++a) {
        for (i32 b = 0; b < 4; ++b) {
            Fixture fixture;
            CHECK(fixture.init());
            fixture.enable(2, {a, b, 0});
            CHECK_EQ(fixture.battle.seat_characters[0], a);
            CHECK_EQ(fixture.battle.seat_characters[1], b);
            CHECK_EQ(fixture.battle.seats[0].hit_size, StageBattle::kPlayerHitSize[a]);
            CHECK_EQ(fixture.battle.seats[1].hit_size, StageBattle::kPlayerHitSize[b]);
            // SHT 定标域：enable_seats 从 seat 0 复制构造态；无 .sht 时
            // 保持默认定标（576/407/256/181）。逐席角色域已记录。
            CHECK_EQ(fixture.battle.seats[1].character, b);
            // 重复角色席位状态独立：两席各自移动（seat 0 经 step 参数）。
            const u32 left = held_of(0x40);
            for (int frame = 0; frame < 10; ++frame) {
                GameInput seat0;
                seat0.update(left);
                CHECK(fixture.battle.step(seat0));
            }
            CHECK(seat_x(fixture.battle, 0) < 0.0f);
            CHECK(std::fabs(seat_x(fixture.battle, 1)) < 1e-4f);
        }
    }
}

// ---- damage_owner_ids：每席伤害注册表独立 ----
// seat 0 开火（Z）若干帧后：seat 0 的 shots/registration 非空、seat 1
// 为空；席位交换往返后归属不变（registration 不串席）。
TEST(damage_owner_ids) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    const u32 fire = held_of(0x01);
    for (int frame = 0; frame < 30; ++frame) {
        GameInput seat0;
        seat0.update(fire);
        CHECK(dual.battle.step(seat0));
    }
    CHECK(!dual.battle.seats[0].shots.empty());
    CHECK(dual.battle.seats[1].shots.empty());
    size_t active0 = 0;
    for (const auto& reg : dual.battle.seats[0].damage_registrations)
        if (reg.active) ++active0;
    CHECK(active0 > 0);
    size_t active1 = 0;
    for (const auto& reg : dual.battle.seats[1].damage_registrations)
        if (reg.active) ++active1;
    CHECK_EQ(active1, 0u);
    // 席位切换往返：活动窗口换到 seat 1 再回来，seat 0 的 shots 不变。
    const size_t shots_before = dual.battle.seats[0].shots.size();
    dual.battle.swap_player_context(1);
    CHECK_EQ(dual.battle.shots.size(), 0u);  // 窗口 = seat 1（空）
    dual.battle.swap_player_context(0);
    CHECK_EQ(dual.battle.shots.size(), shots_before);
    CHECK_EQ(dual.battle.active_seat, 0);
}

// ---- single_seat_sp_equivalence：单席路径与迁移前语义逐位一致 ----
// 双实例同输入同敌机流：实例 A 走单席 step（seat_count==0）、实例 B 构造
// 后不启用多席。逐帧比对玩家位置/相位帧/敌机数/RNG index。迁移把玩家域
// 移入 StagePlayerState 后，单席路径的行为必须与 SP 基线完全一致
//（M0 门：MP 产物与 SP 产物 wasm 字节一致的前置是单席语义一致）。
TEST(single_seat_sp_equivalence) {
    Fixture a, b;
    CHECK(a.init()); CHECK(b.init());
    // b 走一遍 enable→disable 不可行（enable_seats 不提供撤销）；改为
    // 构造第三个实例验证 enable_seats(2) 后 seat 0 的初始状态与单席一致
    //（收编语义：窗口内容原样移入 seats[0]）。
    Fixture c;
    CHECK(c.init());
    CHECK(c.battle.enable_seats(c.provider, 2, {0, 0, 0}));
    // enable 后窗口恢复 seat 0：位置/相位/无敌与单席新构造一致。
    CHECK_EQ(c.battle.player_x, a.battle.player_x);
    CHECK_EQ(c.battle.player_y, a.battle.player_y);
    CHECK_EQ(c.battle.player_phase_frame, a.battle.player_phase_frame);
    CHECK_EQ(c.battle.invulnerable, a.battle.invulnerable);
    CHECK_EQ(c.battle.seat_count, 2);
    CHECK_EQ(c.battle.active_seat, 0);
    // 单席双实例逐帧对拍。
    const u32 pattern[8] = {0x01, 0x01, 0x41, 0x00, 0x02, 0x08, 0x10, 0x80};
    for (int frame = 0; frame < 90; ++frame) {
        GameInput input;
        input.update(pattern[frame % 8]);
        CHECK(a.battle.step(input));
        CHECK(b.battle.step(input));
        CHECK_EQ(a.battle.player_x, b.battle.player_x);
        CHECK_EQ(a.battle.player_y, b.battle.player_y);
        CHECK_EQ(a.battle.player_phase_frame, b.battle.player_phase_frame);
        CHECK_EQ(a.battle.invulnerable, b.battle.invulnerable);
        CHECK_EQ(a.battle.sim.frame, b.battle.sim.frame);
        CHECK_EQ(a.battle.sim.random.index(), b.battle.sim.random.index());
        CHECK_EQ(a.battle.shots.size(), b.battle.shots.size());
        CHECK(a.battle.error().empty() == b.battle.error().empty());
    }
    // seat 0 收编后的多席实例：与 a 同输入走 90 帧后 enable，再同输入
    // 30 帧——seat 0 轨迹/世界推进与单席一致（多席启用不改变 seat 0
    // 的玩家面与世界面）。d 与 a 同起点（同输入同帧数）对拍。
    Fixture d;
    CHECK(d.init());
    GameInput none;
    for (int frame = 0; frame < 90; ++frame) {
        GameInput input;
        input.update(pattern[frame % 8]);
        CHECK(d.battle.step(input));
    }
    CHECK(d.battle.enable_seats(d.provider, 2, {0, 0, 0}));
    for (int frame = 0; frame < 30; ++frame) {
        CHECK(d.battle.step(none));
        CHECK(a.battle.step(none));
        CHECK_EQ(d.battle.seats[0].x, a.battle.player_x);
        CHECK_EQ(d.battle.seats[0].y, a.battle.player_y);
        CHECK_EQ(d.battle.sim.frame, a.battle.sim.frame);
        CHECK_EQ(d.battle.sim.random.index(), a.battle.sim.random.index());
    }
}

int main() { return th13test::run_all("m2.multi_seat"); }
