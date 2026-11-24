#include "anm_test_fixtures.hpp"
#include "game/AnmEnvironment.hpp"

using namespace th13;
namespace th13 {
using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;
}
namespace {

i32 bits(float value) {
    i32 result;
    std::memcpy(&result, &value, 4);
    return result;
}

struct Draw {
    i32 frame;
    i32 variable;
};
std::vector<Draw> draws;
i32 current_frame = 0;

void observe(const AnmVm&, i32 opcode, i32 variable, i32, u32 before, u32, u32 after, u32, bool) {
    if (opcode == 117 && before != after) draws.push_back({current_frame, variable});
}

struct Fixture {
    GameRandom rng{0};
    AnmEnvironment environment;
    AnmManager manager{rng, environment};
    AnmVm root;
    bool ready = false;

    Fixture() {
        std::vector<u8> parent, child;
        th13test::push_insn(parent, 500, 0, 0, {1});
        th13test::push_insn(parent, 2, 5);
        th13test::push_terminator(parent);
        th13test::push_insn(child, 117, 0, 3, {bits(10010.f), bits(10011.f), bits(0.5f)});
        th13test::push_insn(child, 117, 20, 3, {bits(10010.f), bits(10011.f), bits(0.5f)});
        th13test::push_insn(child, 2, 21);
        th13test::push_terminator(child);
        const auto bytes = th13test::build_simple_anm({parent, child});
        if (!manager.load(7, bytes.data(), u32(bytes.size()))) return;
        auto* resource = manager.resource(7);
        if (!resource) return;
        if (!root.bind(*resource, 0)) return;
        root.object_host = &manager;
        root.environment = &environment;
        ready = true;
    }

    void creation_frame() {
        CHECK(root.tick(rng, 1.f) >= 0);
        CHECK(root.tick(rng, 1.f) >= 0);
        CHECK(manager.update());
    }

    void frame() {
        CHECK(root.tick(rng, 1.f) >= 0);
        CHECK(manager.update());
    }
};

}

TEST(child_time20_draw_keeps_native_spawn_phase) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    draws.clear();
    current_frame = 0;
    anm_random_observer = observe;
    fixture.creation_frame();
    CHECK_EQ(fixture.root.timer.current, 2);
    CHECK(fixture.root.instruction_offset >= 0);
    const auto children = fixture.manager.registry.children(fixture.root);
    CHECK_EQ(children.size(), size_t(1));
    if (children.size() == 1) {
        CHECK_EQ(children[0]->timer.current, 2);
        CHECK_EQ(children[0]->instruction_offset, 0x14);
    }
    for (current_frame = 1; current_frame <= 25; ++current_frame) {
        fixture.frame();
        if (current_frame == 4) {
            CHECK_EQ(fixture.root.instruction_offset, -1);
        }
    }
    anm_random_observer = nullptr;
    CHECK_EQ(draws.size(), size_t(2));
    if (draws.size() != 2) return;
    CHECK_EQ(draws[0].frame, 0);
    CHECK_EQ(draws[0].variable, 10011);
    CHECK_EQ(draws[1].frame, 19);
    CHECK_EQ(draws[1].variable, 10011);
}

TEST(missing_same_frame_chain_tick_shifts_time20_by_one) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    draws.clear();
    current_frame = 0;
    anm_random_observer = observe;
    CHECK(fixture.root.tick(fixture.rng, 1.f) >= 0);
    CHECK(fixture.root.tick(fixture.rng, 1.f) >= 0);
    const auto children = fixture.manager.registry.children(fixture.root);
    CHECK_EQ(children.size(), size_t(1));
    if (children.size() == 1) CHECK_EQ(children[0]->timer.current, 1);
    for (current_frame = 1; current_frame <= 25; ++current_frame)
        fixture.frame();
    anm_random_observer = nullptr;
    CHECK_EQ(draws.size(), size_t(2));
    if (draws.size() != 2) return;
    CHECK_EQ(draws[0].frame, 0);
    CHECK_EQ(draws[1].frame, 20);
}

struct ChainFixture {
    GameRandom rng{0};
    AnmEnvironment environment;
    AnmManager manager{rng, environment};
    bool ready = false;
    u32 parent_handle = 0;
    ChainFixture() {
        std::vector<u8> parent, child, tail;
        th13test::push_insn(parent, 6, 0, 0, {2});
        th13test::push_insn(parent, 500, 0, 0, {1});
        th13test::push_insn(parent, 2, 0);
        th13test::push_terminator(parent);
        th13test::push_insn(child, 6, 0, 0, {20});
        th13test::push_insn(child, 2, 0);
        th13test::push_terminator(child);
        th13test::push_insn(tail, 6, 0, 0, {1000});
        th13test::push_insn(tail, 2, 0);
        th13test::push_terminator(tail);
        const auto bytes = th13test::build_simple_anm({parent, child, tail});
        if (!manager.load(7, bytes.data(), u32(bytes.size()))) return;
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
    CHECK(fixture.manager.create(7, 2, -1, 0, Vec3{}, 0, false) != 0);
    CHECK(fixture.manager.update());
    CHECK(fixture.manager.update());
    const AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->timer.current, -18);
}

TEST(chain_tail_parent_missing_same_frame_tick_shifts_child) {
    ChainFixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    CHECK(fixture.manager.update());
    CHECK(fixture.manager.update());
    const AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->timer.current, -19);
}

namespace {
std::vector<std::pair<u32, i32>> phase_draws;
AnmManager* phase_manager = nullptr;
void observe_phase(const AnmVm& vm, i32 opcode, i32, i32, u32 before, u32, u32 after, u32, bool) {
    if (opcode == 117 && before != after && phase_manager)
        phase_draws.emplace_back(phase_manager->registry.handle(vm), vm.instruction_offset);
}
}

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
        environment.paused = false;
        parent_handle = manager.create(7, 167, -1, 0, Vec3{}, 0, false);
        if (!parent_handle) {
            failure = "parent creation failed: " + manager.error;
            return;
        }
        if (with_follower && !manager.create(8, 0, -1, 0, Vec3{}, 0, false)) {
            failure = "follower creation failed";
            return;
        }
        ready = true;
    }

    u32 first_child_handle() {
        auto* parent = manager.registry.find(parent_handle);
        if (!parent) return 0;
        const auto kids = manager.registry.children(*parent);
        return kids.empty() ? 0 : manager.registry.handle(*kids.front());
    }

    bool run() {
        if (first_child_handle()) spawn_update = 0;
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
                if (draw.first == child_handle && draw.second == 296 && draw_update < 0) draw_update = updates;
            }
            const AnmVm* child = manager.registry.find(child_handle);
            if (child && child->instruction_offset < 0) break;
            if (!child && draw_update >= 0) break;
        }
        return spawn_update >= 0 && draw_update >= 0;
    }
};

TEST(reimu2406_real_bullet_anm_child_phase_with_follower) {
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
        std::printf("spawn=%d draw=%d timer=%d (%s)\n", fixture.spawn_update, fixture.draw_update,
                    fixture.child_timer_at_spawn, fixture.failure.c_str());
        return;
    }
    CHECK_EQ(fixture.child_timer_at_spawn, 2);
    CHECK_EQ(fixture.draw_update - fixture.spawn_update, 19);
}

TEST(reimu2406_real_bullet_anm_tail_parent_phase_late_by_one) {
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
        std::printf("spawn=%d draw=%d timer=%d (%s)\n", fixture.spawn_update, fixture.draw_update,
                    fixture.child_timer_at_spawn, fixture.failure.c_str());
        return;
    }
    CHECK_EQ(fixture.child_timer_at_spawn, 1);
    CHECK_EQ(fixture.draw_update - fixture.spawn_update, 20);
}

int main() { return th13test::run_all("anm child tick phase"); }
