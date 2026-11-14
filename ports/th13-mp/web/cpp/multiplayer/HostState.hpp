#pragma once

#include "../game/AnmEnvironment.hpp"
#include "../game/AnmRegistry.hpp"
#include "../game/PauseFrame.hpp"
#include "../game/SessionState.hpp"
#include "../game/SpellElapsedTime.hpp"

#include <memory>
#include <string>
#include <vector>

namespace th13::multiplayer {

// The Application checkpoint and confirmed host digest consume this same
// value object. Device addresses and raw animation handles are restored but
// are not semantic identities for the digest.
struct HostState {
    struct FadeState {
        i32 kind = 0, duration = 0, time = 0, alpha = 0;
        bool playfield_only = false;
    };
    struct StateFields {
        u32 frames = 0;
        i32 pending_destination = -1;
        unsigned pending_load = 0;
        bool record_edge_reset_pending = false;
        i32 spell_time_stage = 0;
        bool spell_clock_active = false;
        i32 spell_clock_index = 0;
        bool stage_transition_loading = false, stage_transition_active = false;
        i32 stage_startup_age = -1;
        bool transition_gate_active = false;
        u32 transition_gate_frames = 0;
        HudPlayerState selection_player;
        std::string game_over_music;
        AnmEnvironment loading_environment;
        float loading_animation_rate = 1.f;
        u32 loading_signature = 0, loading_prayer = 0;
        std::shared_ptr<AnmRegistry::SnapshotData> loading_registry;
        bool has_pause = false;
        PauseState pause;
        PauseFrame::RollbackState pause_frame{};
        std::vector<FadeState> fades;
    } state_fields;

    // Keep the original rollback behavior for these values. They measure
    // local wall-clock presentation/audio and feed save/display metadata,
    // never battle progression. Comparing their bits across peers would
    // compare different clocks/mixers. Clock active/index above remain shared.
    struct LocalFields {
        SpellTimeClock spell_time_clock;
        i32 spell_time_packed = 0;
        u32 game_over_music_cursor = 0;
        float last_fps = 60.f;
        double slow_window_time = 0;
        u32 slow_window_frames = 0;
        float slow_window_fps = 60.f;
        double slow_actual = 0, slow_budget = 0, slow_num = 0, slow_den = 0;
    } local_fields;

    u64 dialogue_digest = 0;
};

}  // namespace th13::multiplayer
