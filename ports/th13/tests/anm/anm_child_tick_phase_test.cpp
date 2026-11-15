// 子 VM 创建相位回归：0x46f990 spawn_child 的创建首 tick 与 0x46f170 主链
// 尾插同帧遍历必须各发生一次；子脚本 time20 的 op117 视觉 RNG 抽签应落在
// 原版相位（创建帧 + 19），父 VM 的创建首 tick 相位不得被改动。
//
// 原版证据（artifacts/decomp/jp/functions/0046f990.c / 00462450.c，
// capstone 0x46f990 / 0x46365a / 0x46f3f0 / 0x46f170）：
//   * 0x46f990 先复制脚本模板、调用一次 0x462450（创建首 tick），再把子
//     VM 经 0x46f3f0 尾插到主链；
//   * 主链 0x46f170 在每次 tick 前先取 next，同一帧稍后执行的主链遍历会
//     tick 到刚尾插的子 VM；
//   * 原生探针（artifacts/reimu-s1-childtick-20261031）中 2395 创建的子 VM
//     在 2395 有两条 0x462450 记录（创建首 tick + 主链 tick），time20 的
//     op117 抽签落在 2414（创建帧 + 19）。
#include "anm_test_fixtures.hpp"
#include "game/AnmEnvironment.hpp"

using namespace th13;
namespace th13 {
using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;
}
namespace {

i32 bits(float value) { i32 result; std::memcpy(&result, &value, 4); return result; }

struct Draw { i32 frame; i32 variable; };
std::vector<Draw> draws;
i32 current_frame = 0;

void observe(const AnmVm&, i32 opcode, i32 variable, i32, u32 before, u32, u32 after,
             u32, bool) {
    if (opcode == 117 && before != after) draws.push_back({current_frame, variable});
}

struct Fixture {
    GameRandom rng{0};
    AnmEnvironment environment;
    AnmManager manager{rng, environment};
    AnmVm root;
    bool ready = false;

    Fixture() {
        // 父脚本 0：time0 spawn 子脚本1，time5 停等。
        std::vector<u8> parent, child;
        th13test::push_insn(parent, 500, 0, 0, {1});
        th13test::push_insn(parent, 2, 5);
        th13test::push_terminator(parent);
        // 子脚本 1：time0 抽签、time20 抽签（目标）、time21 停等。
        th13test::push_insn(child, 117, 0, 3, {bits(10010.f), bits(10011.f), bits(0.5f)});
        th13test::push_insn(child, 117, 20, 3, {bits(10010.f), bits(10011.f), bits(0.5f)});
        th13test::push_insn(child, 2, 21);
        th13test::push_terminator(child);
        const auto bytes = th13test::build_simple_anm({parent, child});
        if (!manager.load(7, bytes.data(), u32(bytes.size()))) return;
        auto* resource = manager.resource(7);
        if (!resource) return;
        // 原版弹体根 VM 由 0x461cb0 直接 memset/绑定，不在 0x46f170 主链；
        // 这里同样用独立 VM，只把 spawn 出的子 VM 交给 registry 主链。
        if (!root.bind(*resource, 0)) return;
        root.object_host = &manager;
        root.environment = &environment;
        ready = true;
    }

    void creation_frame() {
        // StagePresentation::initialize_bullet 的首 tick + 同帧 tick_bullet
        // 尾 tick，然后 Application 主链 update（priority 34）。
        CHECK(root.tick(rng, 1.f) >= 0);
        CHECK(root.tick(rng, 1.f) >= 0);
        CHECK(manager.update());
    }

    void frame() {
        CHECK(root.tick(rng, 1.f) >= 0);
        CHECK(manager.update());
    }
};

}  // namespace

TEST(child_time20_draw_keeps_native_spawn_phase) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    draws.clear();
    current_frame = 0;
    anm_random_observer = observe;
    fixture.creation_frame();
    // 父 VM 创建相位：首 tick + 同帧尾 tick = 2 次推进，且父仍在 time5 等待。
    CHECK_EQ(fixture.root.timer.current, 2);
    CHECK(fixture.root.instruction_offset >= 0);
    // 子 VM 在创建帧必须恰好推进两次：0x46f990 创建首 tick + 0x46f170
    // 主链尾插同帧 tick。只有一次（timer==1）即区分出缺的是哪一条路径。
    const auto children = fixture.manager.registry.children(fixture.root);
    CHECK_EQ(children.size(), size_t(1));
    if (children.size() == 1) {
        CHECK_EQ(children[0]->timer.current, 2);
        CHECK_EQ(children[0]->instruction_offset, 0x14);  // time20 op117
    }
    for (current_frame = 1; current_frame <= 25; ++current_frame) {
        fixture.frame();
        if (current_frame == 4) {
            // time5 停等在创建帧 + 4 帧执行（与原生 5 帧等待窗一致）。
            CHECK_EQ(fixture.root.instruction_offset, -1);
        }
    }
    anm_random_observer = nullptr;
    // 子 VM：创建帧首 tick 的 time0 抽签 + 创建帧主链 tick；time20 抽签在 +19。
    CHECK_EQ(draws.size(), size_t(2));
    if (draws.size() != 2) return;
    CHECK_EQ(draws[0].frame, 0);
    CHECK_EQ(draws[0].variable, 10011);
    CHECK_EQ(draws[1].frame, 19);
    CHECK_EQ(draws[1].variable, 10011);
}

TEST(missing_same_frame_chain_tick_shifts_time20_by_one) {
    // 负控：创建帧不跑主链 update（等价于主链遍历已越过尾插点），
    // time20 抽签应晚一帧到 +20——证明该断言能区分同帧尾插遍历。
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    draws.clear();
    current_frame = 0;
    anm_random_observer = observe;
    CHECK(fixture.root.tick(fixture.rng, 1.f) >= 0);  // 仅创建首 tick
    CHECK(fixture.root.tick(fixture.rng, 1.f) >= 0);
    const auto children = fixture.manager.registry.children(fixture.root);
    CHECK_EQ(children.size(), size_t(1));
    if (children.size() == 1) CHECK_EQ(children[0]->timer.current, 1);  // 缺主链同帧 tick
    for (current_frame = 1; current_frame <= 25; ++current_frame) fixture.frame();
    anm_random_observer = nullptr;
    CHECK_EQ(draws.size(), size_t(2));
    if (draws.size() != 2) return;
    CHECK_EQ(draws[0].frame, 0);
    CHECK_EQ(draws[1].frame, 20);
}


// ---- 生产链形态对照：父 VM 在 registry 主链尾插（0x46f3f0）时的同帧 tick ----
// 正控：父之后还有尾插节点（0x46f3f0 的旧 tail 非父），子 VM 尾插后同一
// 0x46f170 遍历到达旧 tail 的 next，子 VM 创建帧 tick 两次。
// 负控：父本身就是链尾（0x46f170 在 tick 前已把 next 读成 0），子 VM 只拿
// 到 0x46f990 的创建首 tick，time20 相位顺延一帧——这正是端口与原生
// 2395→2414 相位差的可复现最小形态。
struct ChainFixture {
    GameRandom rng{0};
    AnmEnvironment environment;
    AnmManager manager{rng, environment};
    bool ready = false;
    u32 parent_handle = 0;
    ChainFixture() {
        std::vector<u8> parent, child, tail;
        // 父脚本 0：time0 等 2 帧后 spawn 子脚本 1（对齐 script167 op6/op500）。
        th13test::push_insn(parent, 6, 0, 0, {2});
        th13test::push_insn(parent, 500, 0, 0, {1});
        th13test::push_insn(parent, 2, 0);
        th13test::push_terminator(parent);
        // 子脚本 1：time0 进入 20 帧等待（timer 可区分一次/两次 tick）。
        th13test::push_insn(child, 6, 0, 0, {20});
        th13test::push_insn(child, 2, 0);
        th13test::push_terminator(child);
        // 长寿命尾节点：保持父 VM 不在链尾。
        th13test::push_insn(tail, 6, 0, 0, {1000});
        th13test::push_insn(tail, 2, 0);
        th13test::push_terminator(tail);
        const auto bytes = th13test::build_simple_anm({parent, child, tail});
        if (!manager.load(7, bytes.data(), u32(bytes.size()))) return;
        // 0x4114f0→0x46f820 ordering=0：主链尾插（与取消弹表现 VM 同式）。
        parent_handle = manager.create(7, 0, -1, 0, Vec3{}, 0, false);
        ready = parent_handle != 0;
    }
    AnmVm* parent() { return manager.registry.find(parent_handle); }
    const AnmVm* child() {
        auto* p = parent();
        if (!p) return nullptr;
        const auto children = manager.registry.children(*p);
        return children.empty() ? nullptr : children[0];
    }
};

TEST(chain_tail_parent_child_gets_same_frame_tick) {
    ChainFixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    // 正控：父之后再尾插一个长寿命 VM，父不是链尾。
    CHECK(fixture.manager.create(7, 2, -1, 0, Vec3{}, 0, false) != 0);
    CHECK(fixture.manager.update());  // 父 timer -1→0（创建首 tick 后为 -1）
    CHECK(fixture.manager.update());  // 父执行 op500，子创建
    const AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    // 创建首 tick + 同帧主链 tick = 两次（-20→-19→-18）。
    CHECK_EQ(child->timer.current, -18);
}

TEST(chain_tail_parent_missing_same_frame_tick_shifts_child) {
    ChainFixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    // 负控：父本身就是链尾，0x46f170 遍历不会到达新尾插子 VM。
    CHECK(fixture.manager.update());
    CHECK(fixture.manager.update());
    const AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->timer.current, -19);  // 仅创建首 tick，少一帧
}

// ---- Reimu S1 2406 场景：真实 bullet.anm script167→166 的原生相位 ----
// 原版事实（artifacts/reimu-s1-childtick-20261031 原始流 + 反汇编）：
//   * script167 = 取消弹粒子：ins0 op120(var10000←10022 运算) 抽 r，
//     op6 置倒计时 -r，op500(time0) 在 timer≥0 的 tick 生成 script166 子；
//   * 取消弹创建（0x411663→0x46f820 flags=0）尾插主链；神霊/道具 VM
//     （0x438ec0/0x433870，同为 flags=0 尾插）在其后持续尾插——原生探针
//     窗口里 2385 建父、2386 神霊尾插、2387 父生成子并当帧被 0x46f170
//     遍历第二次 tick（anm-ticks 2387 两条 0x462450 记录）；
//   * script166 子：创建帧 2 tick，wait5/10/15/20（脚本内偏移 248/260/
//     272/284）各 +5 tick，偏移 296 的 op117 视觉抽签落在第 21 tick =
//     生成帧 + 19（原生 2387 生成 → 2406 抽签，即 Reimu S1 视觉流首差）。
namespace {
std::vector<std::pair<u32, i32>> phase_draws;
AnmManager* phase_manager = nullptr;
void observe_phase(const AnmVm& vm, i32 opcode, i32, i32, u32 before, u32, u32 after, u32, bool) {
    if (opcode == 117 && before != after && phase_manager)
        phase_draws.emplace_back(phase_manager->registry.handle(vm), vm.instruction_offset);
}
}  // namespace

struct BulletAnmFixture {
    GameRandom rng{12345};
    AnmEnvironment environment;
    AnmManager manager{rng, environment};
    bool ready = false;
    u32 parent_handle = 0;
    u32 child_handle = 0;
    i32 spawn_update = -1;
    i32 draw_update = -1;
    i32 child_timer_at_spawn = -1;
    std::string failure;

    BulletAnmFixture(bool with_follower) {
        th13test::TestFileReader reader;
        std::vector<u8> bullet;
        if (!reader.read("eagler-touhou/ports/th13/web/sdl-runtime/assets/bullet.anm", bullet)) {
            failure = "bullet.anm unavailable";
            return;
        }
        if (!manager.load(7, bullet.data(), u32(bullet.size()))) {
            failure = "bullet.anm load failed: " + manager.error;
            return;
        }
        if (with_follower) {
            // 神霊/道具同构尾插跟随者（0x438ec0 flags=0）：长等待脚本。
            std::vector<u8> follower;
            th13test::push_insn(follower, 6, 0, 0, {1000});
            th13test::push_insn(follower, 2, 0);
            th13test::push_terminator(follower);
            const auto bytes = th13test::build_simple_anm({follower});
            if (!manager.load(8, bytes.data(), u32(bytes.size()))) {
                failure = "follower bank load failed";
                return;
            }
        }
        // 0x411663→0x46f820 flags=0：主链尾插创建取消弹父 VM（script167）。
        environment.paused = false;
        parent_handle = manager.create(7, 167, -1, 0, Vec3{}, 0, false);
        if (!parent_handle) {
            failure = "parent creation failed: " + manager.error;
            return;
        }
        if (with_follower &&
            !manager.create(8, 0, -1, 0, Vec3{}, 0, false)) {
            failure = "follower creation failed";
            return;
        }
        ready = true;
    }

    // 首个子 VM 句柄（attach 用 push_front，children().front() 恰为最新，
    // 生成检测当帧取到的 front 即首个子）。
    u32 first_child_handle() {
        auto* parent = manager.registry.find(parent_handle);
        if (!parent) return 0;
        const auto kids = manager.registry.children(*parent);
        return kids.empty() ? 0 : manager.registry.handle(*kids.front());
    }

    // 逐帧推进到首个子 VM 偏移 296 的 op117 抽签；记录生成帧与抽签帧。
    bool run() {
        if (first_child_handle()) spawn_update = 0;  // r==0：创建帧即生成
        for (i32 frame = 0; frame < 64; ++frame) {
            if (!manager.update()) {
                failure = "manager update failed: " + manager.error;
                return false;
            }
            const i32 updates = frame + 1;
            if (spawn_update < 0 && first_child_handle()) {
                spawn_update = updates;
                child_handle = first_child_handle();
            } else if (spawn_update == 0 && !child_handle) {
                child_handle = first_child_handle();
            }
            if (spawn_update >= 0 && child_timer_at_spawn < 0) {
                const AnmVm* child = manager.registry.find(child_handle);
                if (child) child_timer_at_spawn = child->timer.current;
            }
            for (const auto& draw : phase_draws) {
                if (draw.first == child_handle && draw.second == 296 && draw_update < 0)
                    draw_update = updates;
            }
            const AnmVm* child = manager.registry.find(child_handle);
            if (child && child->instruction_offset < 0) break;  // 子脚本终了
            if (!child && draw_update >= 0) break;
        }
        return spawn_update >= 0 && draw_update >= 0;
    }
};


TEST(reimu2406_real_bullet_anm_child_phase_with_follower) {
    // 正控：父后有尾插跟随者（原生神霊/道具形态），子生成帧拿满两次 tick
    //（timer==2），偏移 296 的 op117 抽签落在生成帧 + 19（原生 2387→2406）。
    BulletAnmFixture fixture(true);
    CHECK(fixture.ready);
    if (!fixture.ready) {
        std::printf("fixture: %s\n", fixture.failure.c_str());
        return;
    }
    phase_draws.clear();
    phase_manager = &fixture.manager;
    anm_random_observer = observe_phase;
    const bool ok = fixture.run();
    anm_random_observer = nullptr;
    phase_manager = nullptr;
    CHECK(ok);
    if (!ok) {
        std::printf("spawn=%d draw=%d timer=%d (%s)\n", fixture.spawn_update,
                    fixture.draw_update, fixture.child_timer_at_spawn,
                    fixture.failure.c_str());
        return;
    }
    CHECK_EQ(fixture.child_timer_at_spawn, 2);
    CHECK_EQ(fixture.draw_update - fixture.spawn_update, 19);
}

TEST(reimu2406_real_bullet_anm_tail_parent_phase_late_by_one) {
    // 负控：父即链尾（无跟随者），子当帧只有创建首 tick（timer==1），
    // 偏移 296 抽签顺延到 +20——即 Reimu S1 2406 一帧相位差的最小复现。
    BulletAnmFixture fixture(false);
    CHECK(fixture.ready);
    if (!fixture.ready) {
        std::printf("fixture: %s\n", fixture.failure.c_str());
        return;
    }
    phase_draws.clear();
    phase_manager = &fixture.manager;
    anm_random_observer = observe_phase;
    const bool ok = fixture.run();
    anm_random_observer = nullptr;
    phase_manager = nullptr;
    CHECK(ok);
    if (!ok) {
        std::printf("spawn=%d draw=%d timer=%d (%s)\n", fixture.spawn_update,
                    fixture.draw_update, fixture.child_timer_at_spawn,
                    fixture.failure.c_str());
        return;
    }
    CHECK_EQ(fixture.child_timer_at_spawn, 1);
    CHECK_EQ(fixture.draw_update - fixture.spawn_update, 20);
}

int main() { return th13test::run_all("anm child tick phase"); }
