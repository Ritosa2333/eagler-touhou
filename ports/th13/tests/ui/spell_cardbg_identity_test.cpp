#include "../harness/test_framework.hpp"
#include "game/EclResource.hpp"
#include "game/SpellPresentation.hpp"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
namespace {

std::vector<u8> asset(const std::string& name) {
    std::ifstream file("artifacts/reference/raw/" + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

bool anm_script_count(const std::vector<u8>& bytes, u32& count) {
    u32 base = 0, total = 0;
    for (;;) {
        if (base + 64 > bytes.size()) return false;
        u32 version = 0, stride = 0;
        u16 scripts = 0;
        std::memcpy(&version, bytes.data() + base, 4);
        std::memcpy(&scripts, bytes.data() + base + 6, 2);
        std::memcpy(&stride, bytes.data() + base + 36, 4);
        if (version != 8) return false;
        total += scripts;
        if (!stride) break;
        base += stride;
    }
    count = total;
    return true;
}

struct Fixture {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    std::vector<std::string> anim_list;
    std::vector<i32> script_base;
    bool build(i32 stage_number) {
        const auto ecl = asset("st0" + std::to_string(stage_number) + ".ecl");
        EclResource head;
        if (!head.open(ecl.data(), u32(ecl.size()))) return false;
        anim_list = head.animations;
        AnmResource merged;
        i32 base = 0;
        for (const auto& name : anim_list) {
            const auto bytes = asset(name);
            u32 scripts = 0;
            if (!bytes.empty() && anm_script_count(bytes, scripts) && merged.append(bytes.data(), u32(bytes.size()))) {
                script_base.push_back(base);
                base += i32(scripts);
            } else {
                return false;
            }
        }
        if (!manager.adopt(4, std::move(merged))) return false;
        for (const auto& item : {std::pair<i32, const char*>{0, "text.anm"}, {2, "ascii.anm"}, {8, "effect.anm"}}) {
            const auto bytes = asset(item.second);
            if (bytes.empty() || !manager.load(item.first, bytes.data(), u32(bytes.size()))) return false;
            if (item.first == 2 && !manager.sprite_fallback(2)) return false;
        }
        return true;
    }
    i32 global(i32 bank, i32 local) const { return script_base[size_t(bank - 2)] + local; }
};

std::unique_ptr<SpellPresentation> consume_card(Fixture& fixture, i32 stage_number, i32 chapter, bool& ok) {
    auto presentation = std::make_unique<SpellPresentation>(fixture.manager, SpellPresentation::Banks{4, stage_number});
    SpellState state;
    const float position[3] = {0, 128, 0};
    ok = presentation->consume(
        state.begin(0, "synthetic", 3000, stage_number, 0), position, [](i32) { return true; }, chapter);
    return presentation;
}

i32 source_script_of(AnmManager& manager, u32 handle) {
    auto* vm = manager.registry.find(handle);
    return vm ? i32(vm->source_script) : -1;
}

}

TEST(stage3_midboss_kogasa_cardbg_uses_menm_at_chapter4) {
    Fixture fixture;
    CHECK(fixture.build(3));
    CHECK_EQ(fixture.anim_list[2], "st03menm.anm");
    bool ok = false;
    auto presentation = consume_card(fixture, 3, 4, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, presentation->stage_background_handle()), fixture.global(4, 12));
    CHECK_EQ(source_script_of(fixture.manager, presentation->decoration_handle()), fixture.global(4, 19));
    CHECK(source_script_of(fixture.manager, presentation->stage_background_handle()) != fixture.global(3, 12));
}

TEST(stage3_chapter5_still_uses_a_row) {
    Fixture fixture;
    CHECK(fixture.build(3));
    bool ok = false;
    auto presentation = consume_card(fixture, 3, 5, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, presentation->stage_background_handle()), fixture.global(4, 12));
}

TEST(stage3_boss_yoshika_cardbg_uses_enm_at_chapter6) {
    Fixture fixture;
    CHECK(fixture.build(3));
    bool ok = false;
    auto presentation = consume_card(fixture, 3, 6, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, presentation->stage_background_handle()), fixture.global(3, 12));
    CHECK_EQ(source_script_of(fixture.manager, presentation->decoration_handle()), fixture.global(3, 19));
}

TEST(stage1_has_no_a_row_even_below_chapter6) {
    Fixture fixture;
    CHECK(fixture.build(1));
    CHECK_EQ(fixture.anim_list.size(), size_t(2));
    bool ok = false;
    auto presentation = consume_card(fixture, 1, 4, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, presentation->stage_background_handle()), fixture.global(3, 11));
    CHECK_EQ(source_script_of(fixture.manager, presentation->decoration_handle()), fixture.global(3, 18));
}

TEST(stage5_midboss_swaps_decoration_to_menm_script9) {
    Fixture fixture;
    CHECK(fixture.build(5));
    CHECK_EQ(fixture.anim_list[2], "st05menm.anm");
    bool ok = false;
    auto midboss = consume_card(fixture, 5, 4, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, midboss->stage_background_handle()), fixture.global(3, 13));
    CHECK_EQ(source_script_of(fixture.manager, midboss->decoration_handle()), fixture.global(4, 9));
    auto boss = consume_card(fixture, 5, 6, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, boss->decoration_handle()), fixture.global(3, 19));
}

TEST(stage7_extra_midboss_cardbg_without_decoration) {
    Fixture fixture;
    CHECK(fixture.build(7));
    CHECK_EQ(fixture.anim_list[2], "st07menm.anm");
    bool ok = false;
    auto midboss = consume_card(fixture, 7, 4, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, midboss->stage_background_handle()), fixture.global(4, 14));
    CHECK_EQ(midboss->decoration_handle(), 0u);
    auto boss = consume_card(fixture, 7, 6, ok);
    CHECK(ok);
    CHECK_EQ(source_script_of(fixture.manager, boss->stage_background_handle()), fixture.global(3, 24));
    CHECK_EQ(source_script_of(fixture.manager, boss->decoration_handle()), fixture.global(3, 30));
}

int main() { return th13test::run_all("spell_cardbg_identity_test"); }
