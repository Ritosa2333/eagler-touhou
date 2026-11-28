#include "../harness/test_framework.hpp"
#include "game/SpellPresentation.hpp"

#include <fstream>
#include <iterator>
#include <vector>

using namespace th13;
namespace {

std::vector<u8> asset(const std::string& name) {
    std::ifstream file("artifacts/reference/raw/" + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

struct Fixture {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    bool build() {
        const auto bytes = asset("front.anm");
        return !bytes.empty() && manager.load(3, bytes.data(), u32(bytes.size()));
    }
};

i32 source_script_of(AnmManager& manager, u32 handle) {
    auto* vm = manager.registry.find(handle);
    return vm ? i32(vm->source_script) : -1;
}

}

TEST(spirit_notice_keeps_previous_levels_alive) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});

    CHECK(presentation.show_spirit_notice(1, 200.0f, 400.0f, 0.0f));
    const u32 first = presentation.spirit_notice_handle();
    CHECK(first != 0);
    CHECK_EQ(source_script_of(fixture.manager, first), 108);

    CHECK(presentation.show_spirit_notice(2, 200.0f, 400.0f, 0.0f));
    const u32 second = presentation.spirit_notice_handle();
    CHECK(second != 0 && second != first);
    CHECK_EQ(source_script_of(fixture.manager, second), 109);
    CHECK_EQ(source_script_of(fixture.manager, first), 108);

    CHECK(presentation.show_spirit_notice(3, 200.0f, 400.0f, 0.0f));
    const u32 third = presentation.spirit_notice_handle();
    CHECK(third != 0 && third != second);
    CHECK_EQ(source_script_of(fixture.manager, third), 110);
    CHECK_EQ(source_script_of(fixture.manager, first), 108);
    CHECK_EQ(source_script_of(fixture.manager, second), 109);
    CHECK_EQ(presentation.spirit_notice_handles().size(), size_t(3));

    auto* vm = fixture.manager.registry.find(first);
    CHECK(vm != nullptr);
    if (vm) {
        CHECK_EQ(vm->visual.translation.x, 200.0f);
        CHECK_EQ(vm->visual.translation.y, 368.0f);
        CHECK_EQ(vm->visual.translation.z, 0.0f);
    }

    CHECK(presentation.show_spirit_notice(0, 0.0f, 0.0f, 0.0f));
    CHECK(presentation.show_spirit_notice(4, 0.0f, 0.0f, 0.0f));
    CHECK_EQ(presentation.spirit_notice_handles().size(), size_t(3));
    CHECK_EQ(presentation.spirit_notice_handle(), third);
}

TEST(spirit_notice_release_retires_every_live_handle) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});
    CHECK(presentation.show_spirit_notice(1, 100.0f, 100.0f, 0.0f));
    CHECK(presentation.show_spirit_notice(2, 100.0f, 100.0f, 0.0f));
    CHECK(presentation.show_spirit_notice(3, 100.0f, 100.0f, 0.0f));
    const std::vector<u32> handles = presentation.spirit_notice_handles();
    CHECK_EQ(handles.size(), size_t(3));
    presentation.release();
    CHECK(fixture.manager.update());
    CHECK_EQ(presentation.spirit_notice_handle(), 0u);
    CHECK(presentation.spirit_notice_handles().empty());
    for (u32 handle : handles)
        CHECK(fixture.manager.registry.find(handle) == nullptr);
}

TEST(spirit_notice_after_natural_end_is_pruned_without_collateral_retire) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});
    CHECK(presentation.show_spirit_notice(1, 0.0f, 0.0f, 0.0f));
    const u32 first = presentation.spirit_notice_handle();
    for (i32 i = 0; i < 200; ++i)
        fixture.manager.update();
    CHECK(fixture.manager.registry.find(first) == nullptr);
    CHECK(presentation.show_spirit_notice(2, 0.0f, 0.0f, 0.0f));
    const u32 second = presentation.spirit_notice_handle();
    CHECK(second != 0);
    CHECK_EQ(source_script_of(fixture.manager, second), 109);
    CHECK_EQ(presentation.spirit_notice_handles().size(), size_t(1));
}

TEST(show_notice_61_to_63_still_replaces_ctx_d0_slot) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});
    CHECK(presentation.show_notice(61));
    const u32 first = presentation.notice_handle();
    CHECK(first != 0);
    CHECK_EQ(source_script_of(fixture.manager, first), 61);
    CHECK(presentation.show_notice(63));
    const u32 second = presentation.notice_handle();
    CHECK(second != 0 && second != first);
    CHECK_EQ(source_script_of(fixture.manager, second), 63);
    const auto* retiring = fixture.manager.registry.find(first);
    CHECK(retiring && (retiring->visual.render_flags & 0x20u));
    CHECK(fixture.manager.update());
    CHECK(fixture.manager.registry.find(first) == nullptr);
    CHECK(presentation.show_notice(62));
    CHECK_EQ(presentation.notice_handle(), second);
}

int main() { return th13test::run_all("spirit notices"); }
