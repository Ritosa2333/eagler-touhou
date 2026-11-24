#include "../ui/ui_test_fixtures.hpp"
#include "anm_test_fixtures.hpp"

#include "game/AnmEnvironment.hpp"
#include "game/AnmRenderer.hpp"
#include "game/StagePresentation.hpp"
#include "game/StageSim.hpp"
#include "game/ZunGraphics.hpp"

#include <cstdio>
#include <cstring>
#include <vector>

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

struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 0; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}

private:
    touhou::graphics::PipelineState pipeline_state;
};

struct Draw {
    const AnmVm* vm = nullptr;
    i32 variable = 0;
};
std::vector<Draw> draws;
void observe(const AnmVm& vm, i32 opcode, i32 variable, i32, u32 before, u32, u32 after, u32, bool) {
    if (opcode == 117 && before != after) draws.push_back({&vm, variable});
}
size_t draws_for(const AnmVm* vm) {
    size_t count = 0;
    for (const auto& draw : draws)
        if (draw.vm == vm) ++count;
    return count;
}

struct Rig {
    GameRandom random{12345};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation{manager, renderer, StagePresentation::Banks{4, 5, 6}, 0};
    bool ready = false;

    Rig() {
        auto scripts = th13::test::trivial_scripts(36);
        scripts[35] = {
            {117, 0, {bits(10010.f), bits(10011.f), bits(0.5f)}, 3},
            {500, 0, {1}, 0},
            {117, 1, {bits(10010.f), bits(10011.f), bits(0.5f)}, 3},
            {6, 5, {1000}, 0},
            {-1, 0, {}, 0},
        };
        scripts[1] = {
            {6, 0, {20}, 0},
            {2, 0, {}, 0},
            {-1, 0, {}, 0},
        };
        const auto data = th13::test::synthetic_anm(8, scripts);
        ready = th13::test::load_bank(manager, 5, data);
    }

    bool initialize(StageBullet& bullet) {
        draws.clear();
        anm_random_observer = observe;
        return presentation.initialize_bullet(bullet, 0);
    }

    const AnmVm* embedded() const {
        for (const auto& draw : draws)
            if (draw.variable == 10011) return draw.vm;
        return nullptr;
    }

    const AnmVm* first_child(const AnmVm* parent) {
        if (!parent) return nullptr;
        const auto children = manager.registry.children(*parent);
        return children.empty() ? nullptr : children[0];
    }
};

}

TEST(embedded_bullet_vm_is_not_a_main_chain_node) {
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    StageBullet bullet;
    bullet.sprite = 0;
    bullet.subtype = 0;
    bullet.generation = 1;

    CHECK(rig.initialize(bullet));
    const AnmVm* embedded = rig.embedded();
    CHECK(embedded != nullptr);
    if (!embedded) return;
    CHECK_EQ(draws_for(embedded), size_t(1));
    CHECK_EQ(rig.manager.registry.ordered_count(false), 1u);
    CHECK_EQ(rig.manager.registry.handle(*embedded), 0u);
    for (u32 i = 0; i < rig.manager.registry.ordered_count(false); ++i) {
        const u32 handle = rig.manager.registry.ordered_handle(false, i);
        CHECK(handle != 0);
        CHECK(rig.manager.registry.find(handle) != embedded);
    }
    const i32 offset = embedded->instruction_offset;
    CHECK(rig.manager.update());
    CHECK_EQ(draws_for(embedded), size_t(1));
    CHECK_EQ(embedded->instruction_offset, offset);
    CHECK(rig.presentation.tick_bullet(bullet, false));
    CHECK_EQ(draws_for(embedded), size_t(2));
    CHECK_EQ(rig.manager.registry.handle(*embedded), 0u);
    anm_random_observer = nullptr;
}

TEST(op500_child_of_embedded_bullet_vm_is_tail_inserted_ordering_zero) {
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    const u32 marker = rig.manager.create(5, 2, -1, 2);
    CHECK(marker != 0);
    StageBullet bullet;
    bullet.sprite = 0;
    bullet.subtype = 0;
    bullet.generation = 1;
    CHECK(rig.initialize(bullet));
    const AnmVm* embedded = rig.embedded();
    CHECK(embedded != nullptr);
    if (!embedded) return;
    const auto children = rig.manager.registry.children(*embedded);
    CHECK_EQ(children.size(), size_t(1));
    if (children.size() != 1) return;
    CHECK_EQ(children[0]->ordering, 0u);
    const u32 child = rig.manager.registry.handle(*children[0]);
    CHECK(child != 0);
    CHECK_EQ(rig.manager.registry.ordered_count(false), 2u);
    CHECK_EQ(rig.manager.registry.ordered_handle(false, 0), marker);
    CHECK_EQ(rig.manager.registry.ordered_handle(false, 1), child);
    anm_random_observer = nullptr;
}

TEST(bullet_manager_tick_before_main_chain_reaches_child_same_frame) {
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    StageBullet bullet;
    bullet.sprite = 0;
    bullet.subtype = 0;
    bullet.generation = 1;
    CHECK(rig.initialize(bullet));
    const AnmVm* embedded = rig.embedded();
    const AnmVm* child = rig.first_child(embedded);
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->timer.current, -19);
    CHECK(rig.manager.update());
    CHECK_EQ(child->timer.current, -18);
    CHECK_EQ(draws_for(embedded), size_t(1));
    anm_random_observer = nullptr;
}

TEST(main_chain_before_bullet_manager_tick_misses_child_until_next_frame) {
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    CHECK(rig.manager.update());
    StageBullet bullet;
    bullet.sprite = 0;
    bullet.subtype = 0;
    bullet.generation = 1;
    CHECK(rig.initialize(bullet));
    const AnmVm* embedded = rig.embedded();
    const AnmVm* child = rig.first_child(embedded);
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->timer.current, -19);
    CHECK(rig.manager.update());
    CHECK_EQ(child->timer.current, -18);
    anm_random_observer = nullptr;
}

int main() { return th13test::run_all("anm bullet embedded vm phase"); }
