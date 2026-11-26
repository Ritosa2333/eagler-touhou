#pragma once
#include "core/runtime/CombinedGameHost.hpp"
#include "core/frontend/MenuSceneWorld.hpp"

namespace kaikeidou {
struct GameState : MenuSceneWorld {
    CollisionContext collision;
    EndingCreditsAssets ending;
    MenuBootstrapContext bootstrap;
    MenuBootstrapGaps bootstrap_gaps;
    PauseGameoverContext pause_context;
    ApplicationStartupContext startup;
    GameplayHudTailContext hud;
    ObjectRecord<1200> ex_vectors;

    CombinedGameHostRefs combined_refs() {
        return {references(), gaps, replay, frame, scene, collision, ending,
                bootstrap, bootstrap_gaps, pause_context, startup, hud, ex_vectors};
    }
    void initialize(CombinedGameHost& host, bool replay_variant) {
        auto& memory = host.memory();
        for (auto region : memory.regions()) memory.clear(region[0], region[1]);
        if (replay_variant) initialize_menu_replay_shared_cold(memory);
        constexpr std::array<std::uint32_t, 9> bindings{{1,2,3,0,5,4,11,13,6}};
        constexpr std::array<std::uint32_t, 5> settings{{0,100,80,1,0}};
        for (unsigned i = 0; i < bindings.size(); ++i) memory.word(0xa733dc + i * 4, bindings[i]);
        for (unsigned i = 0; i < settings.size(); ++i) memory.word(0xa7ebf8 + i * 4, settings[i]);
        memory.word(0xa78e28, 0xffffffffu);
        startup.security_cookie = 0x13572468;
        memory.reconcile();
        host.seal();
    }
};
}
