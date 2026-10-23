#pragma once
#include "EnemyState.hpp"
namespace th11 {
class BulletManager;
class EnemyAnimations;
class EnemyManager;
class LaserManager;
class SpellController;
class Stage;
struct DialogueControl;
struct DeformationControl;
struct ScreenShakes;
struct PracticeState;
struct BulletCancelContext;
struct EnemyCallbackControl;
// Returns false for commands whose game systems have not yet been reconstructed.
// Callers must report that condition instead of silently advancing the script.
bool enemy_movement_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
struct EnemyHealthSegment {float fraction; i32 type;};
struct EnemyCommandEnvironment {
    PracticeState* practice=nullptr;
    bool practice_time_lock=false;
    bool practice_boss_move_down=false;
    float practice_boss_move_down_range=.5f;
    EnemyState* bosses[8]{};u32 manager_flags=0;
    EnemyHealthSegment health_segments[4]{};i32 remaining_phases=0;
    void (*animation_visibility)(u32 id,bool visible,void* user)=nullptr;
    void* animation_user=nullptr;
    BulletManager* bullets=nullptr;
    EnemyAnimations* animations=nullptr;
    EnemyManager* manager=nullptr;
    LaserManager* lasers=nullptr;
    const BulletCancelContext* cancellation=nullptr;
    // 0x4a5730 / 0x4a5738: stage section and its elapsed frames. Original
    // replay checkpoints use the same section selected by ECL 0x158.
    i32 stage_section=0,section_frame=0;
    SpellController* spells=nullptr;
    ScreenShakes* shakes=nullptr;
    DialogueControl* dialogue=nullptr;
    DeformationControl* deformations=nullptr;
    EnemyCallbackControl* callbacks=nullptr;
    Stage* stage=nullptr;
};
// 0 = handled, -1 = wait on the current instruction, -2 = unsupported.
i32 enemy_state_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
bool enemy_emitter_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_fire_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
i32 enemy_laser_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
}
