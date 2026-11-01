#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/AnmVm.hpp"
using namespace th13;

namespace {
struct Provider final : EclResourceProvider {
    std::vector<std::string>* animations = nullptr;
    bool read(const std::string&, std::vector<u8>& out) override {
        std::vector<u8> target, interrupt, broken, root;
        th13test::emit(target, 0, 515, 0, 255, 0, {0});
        th13test::emit(target, 0, 23, 0, 255, 0, {600});
        th13test::emit(target, 0, 10, 0, 255, 0, {});
        // Health interrupt 0 at 790 HP whose target routine exists in the
        // SCPT but has a zero-length body. EclThreads::restart then fails at
        // bind_code (size < 16); a missing label instead takes start()'s
        // id<0 path, which terminates the main thread without an error.
        const char label[] = "BrokenInterrupt";
        th13test::emit(interrupt, 0, 515, 0, 255, 0, {0});
        th13test::emit(interrupt, 0, 514, 0, 255, 0,
                       {0, 790, 9999, sizeof(label)}, label, sizeof(label));
        th13test::emit(interrupt, 0, 23, 0, 255, 0, {600});
        th13test::emit(interrupt, 0, 10, 0, 255, 0, {});
        th13test::emit(root, 0, 502, 0, 255, 0, {0x20});
        th13test::emit(root, 0, 23, 0, 255, 0, {600});
        th13test::emit(root, 0, 10, 0, 255, 0, {});
        // find_index binary-searches definitions, so keep the SCPT order sorted.
        out = th13test::build_scpt({{"BrokenInterrupt", broken},
                                    {"InterruptTarget", interrupt},
                                    {"Target", target}, {"main", root}},
                                 {"enemy.anm", "st01enm.anm"});
        return true;
    }
    bool animation(u32, const std::string& name) override {
        animations->push_back(name); return true;
    }
};
struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Rig {
    Provider provider; Host host; SessionState progress;
    HudPlayerState player; HudScoreState score; StageBattle battle;
    AnmVm vm; StageEnemy* target = nullptr; std::vector<i32> sounds;
    Rig(const char* routine = "Target") {
        provider.animations = &battle.sim.anim_list;
        CHECK(battle.load(provider, "feedback.ecl", 2, host, progress, player, score));
        battle.player_phase = StageBattle::PlayerPhase::Active;
        target = battle.sim.spawn_enemy(routine, 0, 160, 800, nullptr, false);
        target->tick_gate = false;
        battle.sim.enemy_animation = [&](StageEnemy& e, i32) -> AnmVm* {
            return &e == target ? &vm : nullptr;
        };
        battle.sim.enemy_hit_sound = [&](i32 id, float x) { CHECK_EQ(x, target->position[0]); sounds.push_back(id); return true; };
    }
    void shot(float x = 0, float damage = 10) {
        PlayerShot p; p.alive = true; p.x = x; p.y = 160;
        p.angle = 0; p.speed = 0; p.damage = damage;
        battle.shots.push_back(p);
    }
};

TEST(accepted_shots_drive_one_enemy_voice_and_one_frame_flash) {
    Rig r;
    r.shot(); r.shot();
    CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.target->life, 780.f);
    CHECK_EQ(r.sounds.size(), size_t(1));
    if (!r.sounds.empty()) CHECK_EQ(r.sounds[0], 34);
    CHECK_EQ(r.vm.visual.secondary_color, 0xff0000ffu);
    CHECK_EQ(r.vm.visual.flags & 0x60000u, 0x20000u);
    for (int n = 0; n < 4; ++n) {
        r.shot(); CHECK(r.battle.sim.step_frame());
        CHECK_EQ(r.sounds.size(), size_t(1));
        CHECK_EQ(r.vm.visual.flags & 0x60000u, 0u);
    }
    r.shot(); CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.sounds.size(), size_t(2));
    CHECK_EQ(r.target->life, 730.f);
}

TEST(missed_shot_and_lethal_hit_do_not_emit_survivor_hit_feedback) {
    Rig r;
    r.shot(160); CHECK(r.battle.sim.step_frame());
    CHECK(r.sounds.empty()); CHECK_EQ(r.target->life, 800.f);
    r.target->life = 10;
    r.shot(0, 20); CHECK(r.battle.sim.step_frame());
    CHECK(r.sounds.empty());
}

TEST(hit_sound_override_is_read_from_ecl_and_low_health_boss_uses_voice35) {
    Rig r;
    r.target->flags |= 0x1000000;
    r.shot(); CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.sounds.size(), size_t(1));
    if (!r.sounds.empty()) CHECK_EQ(r.sounds[0], 35);
    std::vector<u8> ins;
    th13test::emit(ins, 0, 553, 0, 255, 0, {38});
    EclContext c; c.instruction = reinterpret_cast<const EclInstruction*>(ins.data());
    r.battle.sim.current = r.target;
    CHECK_EQ(r.battle.sim.commands.execute(c, 553), 0);
    r.target->hit_feedback_cooldown = 0;
    r.shot(); CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.sounds.size(), size_t(2));
    if (r.sounds.size() == 2) CHECK_EQ(r.sounds[1], 38);
}
// 00419a90 走到尾部 0041a39b 时才做 hit_anim_timer(+0x4004)-1；存活敌机
// 受击当帧 30 递减为 29，下一无命中帧为 28。
TEST(surviving_hit_timer_decrements_once_on_hit_frame_and_once_after) {
    Rig r;
    r.shot(); CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.target->hit_anim_timer, 29);
    CHECK(r.battle.sim.step_frame());
    CHECK_EQ(r.target->hit_anim_timer, 28);
}

// 0041a39b 的 +0x4004 递减只在 0041a8a0 尾部执行；0x419f07 call 0x478db0
// 返回非零时 0x419f10 直接 ret -1，跳过 0x41a39b。中断 restart 失败使
// StageSim 置 error_，本帧不得再把刚置入的 30 递减为 29。
TEST(restart_failure_keeps_hit_timer_at_thirty) {
    Rig r("InterruptTarget");
    r.shot();
    CHECK(!r.battle.sim.step_frame());
    CHECK(!r.battle.sim.error().empty());
    CHECK_EQ(r.target->interrupts[0].life, -1);
    CHECK_EQ(r.target->hit_anim_timer, 30);
    CHECK_EQ(r.target->life, 790.f);
}

// 00419a90 在 41aad0 击杀/retire 路径上提前返回，不执行 0041a39b 尾递减，
// 死亡帧 +0x4004 保持刚置入的 30。retire_sink 在摘链前同步调用；本测试
// 只在回调内存活读取，step 之后只用快照，避免悬垂指针。此测试覆盖自机弹
// 致死的 retire 早退；中断 restart 失败的早退见上一测试。
TEST(lethal_hit_retire_sink_sees_unclamped_00419a90_hit_timer) {
    Rig r;
    i32 timer_at_retire = -1;
    bool retired = false;
    r.battle.sim.retire_sink = [&](u32 serial) {
        if (r.target && serial == r.target->serial) {
            timer_at_retire = r.target->hit_anim_timer;
            retired = true;
        }
    };
    r.target->life = 10;
    r.shot(0, 20);
    CHECK(r.battle.sim.step_frame());
    CHECK(retired);
    CHECK_EQ(timer_at_retire, 30);
}

}
int main() { return th13test::run_all("enemy-hit-feedback"); }
