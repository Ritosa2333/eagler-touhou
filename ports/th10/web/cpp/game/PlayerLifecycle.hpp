#pragma once
#include "Player.hpp"
#include "AnmManager.hpp"
namespace th10 {
struct PlayerLifecycleEnvironment {
    GameEconomy* economy;
    AnmManager* manager;
    AnmFile* effect_file;
    AnmEnvironment* animations;
    AnmAllocationEnvironment* allocation;
    const float* default_rate;
    const i32* spell_elapsed;
    u32* spell_flags;
    i32* spell_bonus;
    u32* spell_animation_flags[7];
    bool death_sound_enabled;
    i32 replay_mode;
#ifdef TH_ENABLE_THPRAC
    PracticeState* practice=nullptr;
#endif
    virtual void play_death_sound()=0;
    virtual void update_lives(i32 lives)=0;
    virtual void show_caution(const Vec3& position)=0;
    void cancel_spell_capture() const noexcept;
};
}
