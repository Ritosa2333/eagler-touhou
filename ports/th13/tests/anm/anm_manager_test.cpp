#include "anm_test_fixtures.hpp"

#include <cmath>

using namespace th13;

namespace {

struct Fixture {
    th13::GameRandom rng{0};
    th13::AnmEnvironment environment;
    std::unique_ptr<th13::AnmManager> manager;

    Fixture() { manager = std::make_unique<th13::AnmManager>(rng, environment); }

    bool load(const std::vector<th13::u8>& bytes) { return manager->load(1, bytes.data(), th13::u32(bytes.size())); }
};

TEST(create_first_tick_and_semantics) {
    Fixture fixture;
    std::vector<th13::u8> script;
    th13test::push_insn(script, 300, 0, 0, {0});
    th13test::push_insn(script, 304, 0, 0, {5});
    th13test::push_insn(script, 2, 0);
    th13test::push_terminator(script);
    CHECK(fixture.load(th13test::build_simple_anm({script})));

    const th13::u32 handle = fixture.manager->create(1, 0, 7, 0, {100.f, 100.f, 0.f});
    CHECK(handle != 0);
    auto* vm = fixture.manager->registry.find(handle);
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK(vm->visible);
    CHECK(vm->sprite != nullptr);
    CHECK_EQ(vm->sprite->source_id, 0u);
    CHECK_EQ(vm->visual.layer, 5);
    CHECK(vm->visual.render_flags & 0x200);
    CHECK(vm->visual.render_flags & 0x40000);

    std::vector<th13::u8> idle;
    th13test::push_insn(idle, 2, 1);
    th13test::push_terminator(idle);
    Fixture other;
    CHECK(other.load(th13test::build_simple_anm({idle})));
    const auto low = other.manager->create(1, 0, 20, 0);
    const auto high = other.manager->create(1, 0, 25, 0);
    CHECK(other.manager->registry.find(low)->visual.render_flags & 0x40000);
    CHECK(!(other.manager->registry.find(high)->visual.render_flags & 0x40000));

    const auto shifted = other.manager->create(1, 0, -1, 0, {64.f, 80.f, 2.f}, 0.f, true);
    auto* moved = other.manager->registry.find(shifted);
    CHECK_NEAR(moved->visual.translation.x, 384.0, 1e-6);
    CHECK_NEAR(moved->visual.translation.y, 96.0, 1e-6);
    CHECK_NEAR(moved->visual.translation.z, 2.0, 1e-6);
    const auto argful = other.manager->create(1, 0, -1, 0, {}, 5.5f);
    CHECK_NEAR(other.manager->registry.find(argful)->variables.rotation.z, 5.5, 1e-6);
}

TEST(handle_generation_and_find) {
    Fixture fixture;
    std::vector<th13::u8> script;
    th13test::push_insn(script, 2, 1);
    th13test::push_terminator(script);
    CHECK(fixture.load(th13test::build_simple_anm({script})));
    const th13::u32 a = fixture.manager->create(1, 0);
    const th13::u32 b = fixture.manager->create(1, 0);
    CHECK(a != 0 && b != 0 && a != b);
    CHECK((a & 0x1fff) != (b & 0x1fff));
    CHECK_EQ(((a >> 13) + 1) & 0x7ffff, (b >> 13) & 0x7ffff);
    CHECK(fixture.manager->registry.find(a) != nullptr);
    CHECK(fixture.manager->registry.find(0x7fffffff) == nullptr);
}

TEST(update_buckets_and_alternate_clamp) {
    Fixture fixture;
    std::vector<th13::u8> main_script, alt_script;
    th13test::push_insn(main_script, 304, 0, 0, {5});
    th13test::push_insn(main_script, 3, 0);
    th13test::push_terminator(main_script);
    th13test::push_insn(alt_script, 304, 0, 0, {3});
    th13test::push_insn(alt_script, 3, 0);
    th13test::push_terminator(alt_script);
    std::vector<std::vector<th13::u8>> scripts{main_script, alt_script};
    CHECK(fixture.load(th13test::build_simple_anm(scripts)));

    CHECK(fixture.manager->create(1, 0, 0, 0) != 0);
    CHECK(fixture.manager->create(1, 1, 0, 4) != 0);
    CHECK(fixture.manager->update());
    CHECK_EQ(fixture.manager->registry.layer(5).size(), 1u);
    CHECK_EQ(fixture.manager->registry.layer(3).size(), 0u);
    CHECK_EQ(fixture.manager->registry.layer(31).size(), 1u);
    std::vector<th13::u8> alt32;
    th13test::push_insn(alt32, 304, 0, 0, {32});
    th13test::push_insn(alt32, 3, 0);
    th13test::push_terminator(alt32);
    Fixture single;
    CHECK(single.load(th13test::build_simple_anm({alt32})));
    CHECK(single.manager->create(1, 0, 32, 4) != 0);
    CHECK(single.manager->update());
    CHECK_EQ(single.manager->registry.layer(32).size(), 1u);
    CHECK(single.manager->create(1, 0, 31, 0) != 0);
    CHECK(!single.manager->update());
    CHECK(single.manager->error.find("layer") != std::string::npos);
}

TEST(recycle_on_script_end) {
    Fixture fixture;
    std::vector<th13::u8> script;
    th13test::push_insn(script, 402, 0, 0, {0x40000000, 0x40000000});
    th13test::push_insn(script, 1, 2);
    th13test::push_terminator(script);
    CHECK(fixture.load(th13test::build_simple_anm({script})));
    const th13::u32 handle = fixture.manager->create(1, 0);
    CHECK(handle != 0);
    CHECK_EQ(fixture.manager->registry.count(), 1u);
    CHECK(fixture.manager->update());
    CHECK_EQ(fixture.manager->registry.count(), 1u);
    CHECK(fixture.manager->update());
    CHECK_EQ(fixture.manager->registry.count(), 0u);
    CHECK(fixture.manager->registry.find(handle) == nullptr);
    const th13::u32 steady = fixture.manager->create(1, 0);
    CHECK(steady != 0);
    th13::u32 held = steady;
    CHECK(fixture.manager->retire(held));
    CHECK_EQ(held, 0u);
    CHECK(fixture.manager->update());
    CHECK_EQ(fixture.manager->registry.count(), 0u);
}

TEST(spawn_inherits_layer_and_child_interrupt) {
    Fixture fixture;
    std::vector<th13::u8> parent, child;
    th13test::push_insn(parent, 500, 0, 0, {1});
    th13test::push_insn(parent, 3, 0);
    th13test::push_terminator(parent);
    th13test::push_insn(child, 3, 0);
    th13test::push_label(child, 7, 0);
    th13test::push_insn(child, 402, 0, 0, {0x41100000, 0x41100000});
    th13test::push_insn(child, 7, 0);
    th13test::push_terminator(child);
    CHECK(fixture.load(th13test::build_simple_anm({parent, child})));

    const th13::u32 handle = fixture.manager->create(1, 0, 9, 0);
    CHECK(handle != 0);
    auto* root = fixture.manager->registry.find(handle);
    CHECK(root != nullptr);
    auto children = fixture.manager->registry.children(*root);
    CHECK_EQ(children.size(), 1u);
    CHECK_EQ(children[0]->visual.layer, 9);
    CHECK(children[0]->rotation_parent == root);
    CHECK_NEAR(children[0]->visual.scale.x, 1.0, 1e-6);
    CHECK(fixture.manager->interrupt(handle, 7, true));
    CHECK_NEAR(children[0]->visual.scale.x, 9.0, 1e-6);
    CHECK(children[0]->visual.flags & 0x4000);
    auto* detached = fixture.manager->spawn_detached(*root, 1);
    CHECK(detached != nullptr);
    CHECK_EQ(detached->visual.layer, 9);
    CHECK_NEAR(detached->visual.translation.x, root->visual.translation.x, 1e-6);
    CHECK_NEAR(detached->visual.translation.y, root->visual.translation.y, 1e-6);
    CHECK_NEAR(detached->visual.child_anchor.x, root->variables.position.x, 1e-6);
}

TEST(load_validation_and_template_tick) {
    Fixture fixture;
    std::vector<th13::u8> valid;
    th13test::push_insn(valid, 316, 0);
    th13test::push_terminator(valid);
    CHECK(fixture.load(th13test::build_simple_anm({valid})));
    Fixture truncated;
    std::vector<th13::u8> body;
    th13::u8 head[8];
    const th13::i16 opcode = 402;
    const th13::u16 wrong = 16;
    std::memcpy(head, &opcode, 2);
    std::memcpy(head + 2, &wrong, 2);
    std::memset(head + 4, 0, 4);
    body.insert(body.end(), head, head + 8);
    th13test::push_terminator(body);
    CHECK(!truncated.load(th13test::build_simple_anm({body})));
    CHECK(truncated.manager->error.find("Invalid animation resource") != std::string::npos);
    Fixture semantic;
    std::vector<th13::u8> starved;
    th13test::push_insn(starved, 402, -1);
    th13test::push_terminator(starved);
    CHECK(!semantic.load(th13test::build_simple_anm({starved})));
    CHECK(semantic.manager->error.find("template") != std::string::npos);
}

TEST(after_main_update_hook_contract) {
    Fixture fixture;
    std::vector<th13::u8> script;
    th13test::push_insn(script, 3, 0);
    th13test::push_terminator(script);
    CHECK(fixture.load(th13test::build_simple_anm({script})));

    int calls = 0;
    fixture.manager->after_main_update = [&calls] {
        ++calls;
        return true;
    };
    CHECK(fixture.manager->update(true));
    CHECK_EQ(calls, 0);
    CHECK(fixture.manager->update(false));
    CHECK_EQ(calls, 1);
    CHECK(fixture.manager->update());
    CHECK_EQ(calls, 2);

    fixture.manager->after_main_update = [&calls] {
        ++calls;
        return false;
    };
    CHECK(!fixture.manager->update(false));
    CHECK_EQ(calls, 3);
    CHECK(fixture.manager->error.find("post-update") != std::string::npos);

    fixture.manager->error.clear();
    fixture.manager->after_main_update = [&calls, &fixture] {
        ++calls;
        fixture.manager->error = "specific post-chain failure";
        return false;
    };
    CHECK(!fixture.manager->update(false));
    CHECK_EQ(calls, 4);
    CHECK(fixture.manager->error.find("specific post-chain failure") != std::string::npos);

    int failed_calls = 0;
    fixture.manager->after_main_update = [&failed_calls] {
        ++failed_calls;
        return true;
    };
    CHECK(fixture.manager->create(1, 0, 31, 0) != 0);
    CHECK(!fixture.manager->update(false));
    CHECK_EQ(failed_calls, 0);
}

}

int main() { return th13test::run_all("anm-manager"); }
