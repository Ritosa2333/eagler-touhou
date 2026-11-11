// Enemy bullets have independent ANM roots in the original 0x40e840 path.
// This catches presentation state transfer after StageSim compacts cull/death.
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

std::vector<u8> bullet_bank() {
    auto scripts = trivial_scripts(36);
    // Type 0 resolves to bullet.anm script 35.  Keep the VM parked so its age
    // is a direct witness of whether the root was preserved across compaction.
    scripts[35] = {sprite(0), wait(), stop()};
    return synthetic_anm(1, scripts);
}

std::string bullet_lines(const std::string& report) {
    std::string lines;
    size_t at = 0;
    while (at < report.size()) {
        const size_t end = report.find('\n', at);
        const size_t limit = end == std::string::npos ? report.size() : end;
        if (report.compare(at, 9, "  bullet ") == 0) {
            lines.append(report, at, limit - at);
            lines.push_back('\n');
        }
        if (end == std::string::npos) break;
        at = end + 1;
    }
    return lines;
}

struct Fixture {
    GameRandom random{17};
    AnmEnvironment environment;
    AnmManager animations{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StageBattle battle;
    StagePresentation presentation{animations, renderer, {4, 5, 6}, 0};

    bool init() {
        const auto empty = synthetic_anm(1, trivial_scripts(1));
        if (!load_bank(animations, 4, empty) || !load_bank(animations, 6, empty) ||
            !load_bank(animations, 5, bullet_bank())) {
            return false;
        }
        battle.player_dead = true;  // isolate enemy bullets from player VMs
        return true;
    }

    bool sync() {
        const bool ok = presentation.sync(battle, 0);
        return ok;
    }
    std::string lines() const { return bullet_lines(presentation.debug_report()); }
};

}  // namespace

TEST(enemy_bullet_vm_follows_birth_generation_after_compaction) {
    Fixture fixture;
    CHECK(fixture.init());

    StageBullet first;
    first.sprite = 0;
    first.subtype = 0;
    first.generation = 11;
    StageBullet survivor = first;
    survivor.generation = 22;
    fixture.battle.sim.bullets.push_back(first);
    CHECK(fixture.sync());
    CHECK(fixture.sync());  // first root is now older than the future survivor

    fixture.battle.sim.bullets.push_back(survivor);
    CHECK(fixture.sync());
    CHECK(fixture.lines().find("gen=11") != std::string::npos);
    CHECK(fixture.lines().find("gen=22") != std::string::npos);

    // StageSim removes dead entries by stable compaction.  The survivor must
    // keep its own root.  原版出生帧双 tick：0x461cb0:35 初始化执行一次
    // 0x462450（其尾部 00462979-0046298f 无条件推进时间三元组），出生序列
    // 0x40e840:273-274 再执行一次（置 +0xbbe=2 → 40f030 → 462450）——
    // 出生 sync 后 age=2，压缩后再 sync = 3；继承 first 的根则会是 5。
    fixture.battle.sim.bullets.erase(fixture.battle.sim.bullets.begin());
    CHECK(fixture.sync());
    const std::string survivor_line = fixture.lines();
    CHECK(survivor_line.find("gen=22") != std::string::npos);
    CHECK(survivor_line.find("age=3") != std::string::npos);

    // A same-script/color birth gets a fresh root after reusing the inactive
    // storage slot.  原版出生帧 age=2（0x461cb0 + 0x40e840:274 双执行），
    // 而非沿用旧槽位的残留年龄。
    StageBullet newborn = survivor;
    newborn.generation = 33;
    fixture.battle.sim.bullets.clear();
    fixture.battle.sim.bullets.push_back(newborn);
    CHECK(fixture.sync());
    const std::string newborn_line = fixture.lines();
    CHECK(newborn_line.find("gen=33") != std::string::npos);
    CHECK(newborn_line.find("age=2") != std::string::npos);
}

TEST(stage_sim_assigns_unique_birth_generation_to_each_emitted_bullet) {
    StageSim sim;
    sim.interactive = true;
    StageEt emitter;
    emitter.reset();
    emitter.sprite = 0;
    emitter.subtype = 0;
    emitter.count = 2;
    emitter.ways = 1;
    emitter.speed1 = 1.0f;
    const float origin[3] = {0, 0, 0};
    sim.emit_et(emitter, 7, 0, origin, 0.0f);
    CHECK_EQ(sim.bullets.size(), size_t(2));
    CHECK(sim.bullets[0].generation != 0);
    CHECK(sim.bullets[1].generation != 0);
    CHECK(sim.bullets[0].generation != sim.bullets[1].generation);
}

int main() { return th13test::run_all("enemy bullet animation identity"); }
