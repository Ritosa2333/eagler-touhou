// 妖梦 Bomb → StageBattle 接线定向测试（r4/combat 窗口2 收尾）。
// 同源真实资源（artifacts/reference/raw 的 st01.ecl + pl03a.sht）：
//   W1 施放后 youmu_bomb_ 进入生成期且产出 spawn 事件（slot/script/y 字段域）。
//   W2 敌机伤害 = 30×覆盖斩数、每敌每帧恰一次（相邻帧生命差为 30 的整数倍，
//      且不为旧近似 3.0/帧）。
//   W3 消弹为局部斩矩形（存活弹可以>0；不触发全屏清空语义的断言不设——
//      只验证不清为 0 的场内远处弹：构造一枚远处弹应存活）。
//   W4 全程 battle.error() 空、无崩溃（分支不崩底线）。
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

}  // namespace

int main() {
    Provider provider;
    Host host;
    SessionState progress;  // character 3 = 妖梦（0x441dd0 判定尺寸表下标）
    progress.character = 3;
    HudPlayerState player;
    player.bombs = 2;
    player.extra_lives = 99;
    HudScoreState score;
    StageBattle battle;
    battle.sim.anm_effects_noop = true;
    provider.anim_list = &battle.sim.anim_list;
    if (!battle.load(provider, "st01.ecl", 1 << 3, host, progress, player, score,
                     nullptr, "pl03a.sht")) {
        std::printf("FAIL load: %s\n", battle.error().c_str());
        return 1;
    }
    // 先推进到敌机在场（st01 Lunatic 首妖精 ~f61 出现）再施放。
    for (int frame = 0; frame < 70; ++frame) {
        GameInput idle;
        if (!battle.step(idle)) {
            std::printf("FAIL warmup: %s\n", battle.error().c_str());
            return 1;
        }
    }
    // W1/W2/W4：施放（X 沿）后步进 40 帧（斩池生成期前段）。
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
        // 表现层（Items/pl03 VM）回填仿真：真实链 = battle.step 生成 →
        // ANM tick → set_vm(alive, rotation.z) → 下帧命中/消弹查询消费。
        // 无头测试以恒活 VM、rotation 0 仿真（寿命/冻结由 r4/bomb A7 覆盖）。
        for (int s = 0; s < YoumuBomb::kSlots; ++s)
            if (battle.youmu_bomb_.has_anm(s)) battle.youmu_bomb_.set_vm(s, true, -0.7853981f);  // 仿真斜向斩（真源=pl03 VM ins_119 rotation.z）
        if (!battle.youmu_slash_spawns_.empty()) {
            saw_spawn = true;
            for (const auto& spawn : battle.youmu_slash_spawns_) {
                check(spawn.script == 19 || spawn.script == 20, "spawn script 19/20");
                check(spawn.y >= 0.0f && spawn.y <= 448.0f, "spawn y in field");
            }
        }
        // 取首个活敌的生命轨迹。
        for (const auto& enemy : battle.sim.enemies) {
            if (enemy->dead || enemy->life <= 0.0f || (enemy->flags & 0x21u)) continue;
            if (last_serial == 0) last_serial = enemy->serial;
            if (enemy->serial != last_serial) continue;
            if (last_life >= 0.0f && enemy->life < last_life) {
                const float delta = last_life - enemy->life;
                check(delta >= 30.0f && (int(delta * 1000.0f) % 30000) == 0,
                      "damage is 30x per frame");
                saw_damage_frame = frame;
            }
            last_life = enemy->life;
            break;
        }
    }
    check(saw_spawn, "W1 spawn events during active frames");
    check(saw_damage_frame >= 0, "W2 slash damage reached an enemy");
    // W3：局部消弹（旧 bug=全屏清）：早期小 scale 帧撒 8 枚场内弹，至少
    // 一枚存活即非全屏清空（斩矩形只覆盖局部）。
    for (int i = 0; i < 8; ++i) {
        StageBullet probe;
        probe.x = -168.0f + float(i) * 48.0f;
        probe.y = 40.0f + float(i % 4) * 110.0f;
        probe.flags = 0x12;
        battle.sim.bullets.push_back(probe);
    }
    for (int frame = 0; frame < 3; ++frame) {  // bomb 早段（scale 小）
        GameInput input;
        input.held = 0x01;
        if (!battle.step(input)) break;
    }
    int survivors = 0;
    for (const auto& bullet : battle.sim.bullets)
        if (bullet.alive && bullet.y > 30.0f) ++survivors;
    check(survivors > 0, "W3 local clear leaves in-field bullets");
    check(battle.error().empty(), "W4 no battle error");
    std::printf(failures ? "# youmu wiring: %d FAIL\n" : "# youmu wiring: all pass\n",
                failures);
    return failures ? 1 : 0;
}
