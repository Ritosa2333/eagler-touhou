#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include "game/AnmOverlay.hpp"
#include "game/RunTransitionEffect.hpp"
#include "ui_test_fixtures.hpp"

#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th13;

namespace {

bool read_asset(const char* name, std::vector<u8>& bytes) {
    const char* roots[] = {
        "artifacts/reference/raw/",
        "eagler-touhou/ports/th13/web/sdl-runtime/assets/",
        "ports/th13/web/sdl-runtime/assets/",
    };
    for (const char* root : roots) {
        std::ifstream file(std::string(root) + name, std::ios::binary);
        if (!file) continue;
        bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return !bytes.empty();
    }
    return false;
}

bool load_bank(AnmManager& manager, i32 id, const char* name) {
    std::vector<u8> bytes;
    CHECK(read_asset(name, bytes));
    if (bytes.empty()) return false;
    if (!manager.load(id, bytes.data(), u32(bytes.size()))) {
        std::printf("%s load error: %s\n", name, manager.error.c_str());
        return false;
    }
    return true;
}

bool load_prefix_banks(AnmManager& manager, AnmEnvironment& environment) {
    CHECK(load_bank(manager, 2, "ascii.anm"));
    if (!manager.resource(2)) return false;
    environment.fallback_sprite_resource = manager.resource(2);
    CHECK(load_bank(manager, 8, "effect.anm"));
    return manager.resource(8) != nullptr;
}

bool load_astral_bank(AnmManager& manager) {
    CHECK(load_bank(manager, 25, "astral.anm"));
    if (!manager.resource(25)) return false;
    manager.name_resource(25, "astral.anm");
    return true;
}

bool load_real_banks(AnmManager& manager, AnmEnvironment& environment) {
    return load_prefix_banks(manager, environment) && load_astral_bank(manager);
}

AnmVm* root_for(AnmManager& manager, u32 handle) { return manager.registry.find(handle); }

AnmOverlay* overlay_for(AnmManager& manager, u32 handle) {
    auto* root = root_for(manager, handle);
    return root ? root->geometry.overlay.get() : nullptr;
}

}

TEST(run_transition_is_title_owned_effect_script0_overlay) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    CHECK(load_prefix_banks(manager, environment));

    RunTransitionEffect four_panel(manager);
    CHECK(four_panel.create());
    auto* four = overlay_for(manager, four_panel.handle());
    CHECK(four != nullptr);
    if (four) {
        CHECK(four->panels[4] != nullptr);
        if (four->panels[4]) CHECK(four->panels[4]->resource == nullptr);
    }
    CHECK(four_panel.release());

    const u32 astral_before = random.index();
    const u16 astral_seed = random.seed();
    CHECK(load_astral_bank(manager));
    CHECK(manager.resource(25) != nullptr);
    CHECK_EQ(random.index(), astral_before);
    CHECK_EQ(random.seed(), astral_seed);

    const u32 rng_index = random.index();
    const u16 rng_seed = random.seed();

    RunTransitionEffect title_effect(manager);
    CHECK(title_effect.create());
    CHECK(title_effect.active());
    const u32 handle = title_effect.handle();
    CHECK(handle != 0);
    auto* root = root_for(manager, handle);
    CHECK(root != nullptr);
    if (!root) return;
    CHECK_EQ(root->source_script, 0);
    CHECK_EQ(root->visual.layer, 26);
    CHECK(root->geometry.overlay != nullptr);
    auto* overlay = overlay_for(manager, handle);
    CHECK(overlay != nullptr);
    if (!overlay) return;
    CHECK_EQ(overlay->mode, 0);
    for (u32 i = 0; i < 4; ++i) {
        CHECK(overlay->panels[i] != nullptr);
        if (overlay->panels[i]) CHECK_EQ(overlay->panels[i]->source_script, i32(i + 3));
        CHECK_NEAR(overlay->panels[i]->visual.translation.x, 320.f, 1e-6);
        CHECK_NEAR(overlay->panels[i]->visual.translation.y, 240.f, 1e-6);
    }
    CHECK(overlay->panels[4] != nullptr);
    if (overlay->panels[4]) {
        CHECK_EQ(overlay->panels[4]->source_script, 0);
        CHECK_EQ(manager.resource_id(overlay->panels[4]->resource), 25);
    }
    CHECK_EQ(random.index(), rng_index);
    CHECK_EQ(random.seed(), rng_seed);

    const u32 transferred = title_effect.detach();
    CHECK_EQ(transferred, handle);
    CHECK(!title_effect.active());
    RunTransitionEffect run_effect(manager);
    CHECK(run_effect.adopt(transferred));
    CHECK_EQ(run_effect.handle(), handle);
    CHECK(root_for(manager, handle) != nullptr);
    CHECK_EQ(random.index(), rng_index);
    CHECK_EQ(random.seed(), rng_seed);

    CHECK(run_effect.stage_start());
    overlay = overlay_for(manager, handle);
    CHECK(overlay != nullptr);
    if (!overlay) return;
    const i32 expected_scripts[] = {7, 8, 9, 10};
    for (u32 i = 0; i < 4; ++i) {
        CHECK(overlay->panels[i] != nullptr);
        if (overlay->panels[i]) {
            CHECK_EQ(overlay->panels[i]->source_script, expected_scripts[i]);
            CHECK(overlay->panels[i]->instruction_offset >= 0);
        }
    }
    CHECK_EQ(random.index(), rng_index);
    CHECK_EQ(random.seed(), rng_seed);
    CHECK(run_effect.active());
}

TEST(run_transition_naturally_retires_and_stale_stage_interrupt_is_noop) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    CHECK(load_real_banks(manager, environment));

    RunTransitionEffect transition(manager);
    CHECK(transition.create());
    const u32 handle = transition.handle();
    CHECK(root_for(manager, handle) != nullptr);
    const u32 rng_index = random.index();
    const u16 rng_seed = random.seed();
    CHECK(transition.stage_start());
    for (i32 frame = 0; frame < 80; ++frame)
        CHECK(manager.update(false));
    CHECK(root_for(manager, handle) == nullptr);
    CHECK(!transition.active());
    CHECK_EQ(random.index(), rng_index);
    CHECK_EQ(random.seed(), rng_seed);

    CHECK(transition.stage_start());
    CHECK(root_for(manager, handle) == nullptr);
    CHECK(!transition.active());
    CHECK_EQ(random.index(), rng_index);
    CHECK_EQ(random.seed(), rng_seed);
    CHECK(transition.release());
    CHECK_EQ(transition.handle(), 0u);
}

TEST(run_transition_new_launch_replaces_owner_and_release_collects) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    CHECK(load_real_banks(manager, environment));
    RunTransitionEffect transition(manager);
    CHECK(transition.create());
    const u32 first = transition.handle();
    CHECK(transition.create());
    const u32 second = transition.handle();
    CHECK(first != second);
    CHECK(manager.update(false));
    CHECK(root_for(manager, first) == nullptr);
    CHECK(root_for(manager, second) != nullptr);
    CHECK(transition.release());
    CHECK_EQ(transition.handle(), 0u);
    CHECK(manager.update(false));
    CHECK(root_for(manager, second) == nullptr);
}

int main() { return th13test::run_all("title run transition"); }
