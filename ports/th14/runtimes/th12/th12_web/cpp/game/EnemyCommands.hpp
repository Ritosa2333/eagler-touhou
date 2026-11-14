#pragma once
#include "EnemyState.hpp"
#include "LoadingWordTh14.hpp"
namespace th11 {
struct EnemyCommandEnvironment;
i32 enemy_th16_spell_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&);
// 0x427ba9 (601) and 0x428a49 (702) share this reading, and it is measured rather than inferred:
// the branch compares the *second* triple's z (native +0x3e98 + index*12 + 8) with the constant at
// [0x4c19e8] - its four bytes are 0x3f666666 - and `comiss`/`jbe` sends "at or below" to adding
// the entity's own +0x44 position, while "above" adds base + offset and forces z to zero.
// A diagnostic, not a machine fact: 601's record write is measured and checked, but the port's
// fire path below it can still fail. These two say how far it got, and GameBattle puts them in the
// failure text, so a boot names the cause instead of only the opcode.
extern i32 g_th14_fire_stage,g_th14_fire_bullet_error;
// Bullet-transform diagnostic: what the machine's transform program asked for when the port could not
// run it. g_th14_bullet_program is 18 pairs (opcode, concurrent) of the record the spawner got.
extern i32 g_th14_bullet_sprite,g_th14_bullet_transform_index,g_th14_bullet_transform_opcode;
extern i32 g_th14_bullet_program[36];
// The machine's transform walker (0x18140) advances past an opcode it has no case for: its chains
// end at 0x4190e9, `inc [bullet+0xc20]; jmp next`. This records the opcodes the port walked past, so
// a boot names what is still unhandled instead of failing on it.
extern u32 g_th14_bullet_unhandled;
extern u32 g_th14_bullet_unsupported_native;
inline void enemy_th14_fire_position(EnemyState& e,u32 index)noexcept{
#if PORT_TITLE == 14
    auto& record=e.th14_emitters[index];
    const auto& base=e.th14_base_vectors[index];const auto& offset=e.th14_offset_vectors[index];
    u32 bits=0x3f666666u;float limit;std::memcpy(&limit,&bits,4);
    if(offset.z>limit)record.position={base.x+offset.x,base.y+offset.y,0};
    else record.position={base.x+e.current.position.x,base.y+e.current.position.y,base.z+e.current.position.z};
#else
    (void)e;(void)index;
#endif
}
}
namespace th11 {
class BulletManager;
#if PORT_TITLE == 14
struct BulletState;
struct BulletEnvironment;
void enemy_th14_closest_bullet(EnemyState&,const BulletState*)noexcept;
void enemy_th14_freeze_bullets(const EnemyState&,BulletState*,Vec3,BulletEnvironment&)noexcept;
void enemy_th14_random_acceleration(BulletState*,Rng&)noexcept;
struct AnmVm;
void enemy_th14_visual_interrupt(const EnemyState&,AnmVm&)noexcept;
#endif
class EnemyAnimations;
class EnemyManager;
struct EnemySpawn;
class LaserManager;
class SpellController;
class Stage;
struct DialogueControl;
struct DeformationControl;
struct ScreenShakes;
struct BulletCancelContext;
struct EnemyCallbackControl;
// Returns false for commands whose game systems have not yet been reconstructed.
// Callers must report that condition instead of silently advancing the script.
bool enemy_movement_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
struct EnemyHealthSegment {float fraction; i32 type;};
struct EnemyCommandEnvironment {
    // TH14's registration rows occupy native +5c..+98, before flags +9c.
    static constexpr u32 boss_capacity=kPortTitle==14?16:kPortTitle==16?4:8;
    EnemyState* bosses[boss_capacity]{};u32 manager_flags=0;
#if PORT_TITLE == 16
    // Retail manager +48..+54 stores four instance IDs, not enemy pointers.
    i32 th16_boss_ids[4]{};
    i32 th16_runtime_mode=0;u32 th16_run_flags=0;
    i32 th16_chapter_spawned=0,th16_chapter_removed=0;
    i32 th16_game_chapter=0;
    // Retail GUI rows use a 0x54 stride; only the four observed markers
    // per boss are exposed until the rest of that row is reconstructed.
    EnemyHealthSegment th16_health_segments[4][4]{};
#endif
    // DAT_004b43e4+0x6cf4, the word before the 4-slot 8-byte table at +0x6cf8. 0x1b8 writes i(0)
    // here. Named after its offset because what it means has not been established.
    u32 value_6cf4=0;
    EnemyHealthSegment health_segments[4]{};i32 remaining_phases=0;
    void (*animation_visibility)(u32 id,bool visible,void* user)=nullptr;
    void* animation_user=nullptr;
    BulletManager* bullets=nullptr;
    EnemyAnimations* animations=nullptr;
    EnemyManager* manager=nullptr;
#if PORT_TITLE == 14
    EnemyManager* th14_owner()const noexcept{
#if defined(TH14_CPP_STAGE_WORKER)
        return __atomic_load_n(&manager,__ATOMIC_ACQUIRE);
#else
        return manager;
#endif
    }
    void th14_publish_owner(EnemyManager* owner)noexcept{
#if defined(TH14_CPP_STAGE_WORKER)
        __atomic_store_n(&manager,owner,__ATOMIC_RELEASE);
#else
        manager=owner;
#endif
    }
#endif
    LaserManager* lasers=nullptr;
    const BulletCancelContext* cancellation=nullptr;
    // 0x4a5730 / 0x4a5738: stage section and its elapsed frames. Original
    // replay checkpoints use the same section selected by ECL 0x158.
    LoadingWordTh14<i32> stage_section=0,section_frame=0;
#if PORT_TITLE == 14
    // Native42de20: selecting the same section preserves its elapsed word.
    // This also runs in the private initial worker after the audio-head wait.
    void th14_reset_section_frame()noexcept{section_frame=0;} // Retail435b62 before replay restore.
    void th14_set_stage_section(i32 next)noexcept{
        if(stage_section!=next)section_frame=0;
        stage_section=next;
    }
#endif
    SpellController* spells=nullptr;
    ScreenShakes* shakes=nullptr;
    DialogueControl* dialogue=nullptr;
    DeformationControl* deformations=nullptr;
    EnemyCallbackControl* callbacks=nullptr;
    Stage* stage=nullptr;
#if PORT_TITLE == 14
    // Native Supervisor mode (4d964c) and run flags (4f58b8). Ordinary play
    // starts at zero; alternate front ends must supply their native values.
    i32 th14_runtime_mode=0;LoadingWordTh14<u32> th14_run_flags=0;
    // 436060 clears only transient bits0/1/3; preserve the other mode bits.
    void th14_complete_loading_flags()noexcept{th14_run_flags&=~0xbu;}
    // ECL 547 writes 4d8f58 immediately; ANM and enemy timers share it.
    float* th14_rate_target=nullptr;
    // Native game +1b8, three 0x54-stride boss rows; 430195 clears four marks.
    EnemyHealthSegment th14_health_segments[3][4]{};
#endif
};
// 0 = handled, -1 = wait on the current instruction, -2 = unsupported.
i32 enemy_state_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
// TH14 502/503: flags and all sixteen animation visibility slots.
i32 enemy_th14_flags(EnemyState&,u16 opcode,u32 bits,EnemyCommandEnvironment&)noexcept;
i32 enemy_th14_state_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
i32 enemy_th14_movement_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_th14_visibility(EnemyState&)noexcept;
i32 enemy_th14_emitter_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_th14_fire_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
// TH14's named, two-dimensional spawn descriptor at 42a1e0.
bool enemy_th14_spawn_parameters(EnemyState&,EclContext&,EnemyGlobals&,EnemySpawn&,const char*&)noexcept;
bool enemy_th16_spawn_parameters(EnemyState&,EclContext&,EnemyGlobals&,EnemySpawn&,const char*&)noexcept;
i32 enemy_th16_time_drop_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
void enemy_th16_time_drop_tick(EnemyState&)noexcept;
bool enemy_th16_time_drop_decay(EnemyState&)noexcept;
i32 enemy_th16_hitbox_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
i32 enemy_th16_difficulty_command(EclContext&,EnemyGlobals&)noexcept;
i32 enemy_th16_health_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_th16_drop_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_th16_health_marker(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
i32 enemy_th16_interrupt_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
i32 enemy_th16_deformation_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
struct Th16EnemyItemServices {
    virtual ~Th16EnemyItemServices()=default;
    virtual bool spawn(Vec3,float angle,float speed)=0;
};
bool enemy_th16_damage_drop(EnemyState&,Rng&,Th16EnemyItemServices&)noexcept;
i32 enemy_th16_movement_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_th16_visibility(EnemyState&)noexcept;
bool enemy_th16_direction_request(const EnemyState&,i32& direction,i32& script)noexcept;
struct AnmVm;
void enemy_th16_mark_animation(AnmVm&)noexcept;
bool enemy_th16_clear_drops(EnemyState&)noexcept;
i32 enemy_th16_chapter_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
i32 enemy_th16_cancel_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
void enemy_th16_chapter_death(EnemyState&,EnemyCommandEnvironment&)noexcept;
void enemy_th16_bind_spawn(EnemyState&,const EnemySpawn&,const float*,i32)noexcept;
void enemy_th16_death_metadata(EnemyState&)noexcept;
i32 enemy_th16_emitter_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_th16_fire_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
bool enemy_th16_fire_position(EnemyState&,u32)noexcept;
bool enemy_th16_laser_request(const EnemyState&,u32,u8*)noexcept;
struct Th16LooseAnimationServices {
    virtual ~Th16LooseAnimationServices()=default;
    virtual bool construct(i32,i32)=0;
    virtual bool register_effect()=0;
    virtual bool lookup_effect()=0;
    virtual bool tick()=0;
};
i32 enemy_th16_loose_animation_command(EclContext&,EnemyGlobals&,Th16LooseAnimationServices&);
i32 enemy_th16_death_script_command(EnemyState&,EclContext&);
i32 enemy_th16_callback_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
struct BulletState;
void enemy_th16_nearby_bullets(BulletState*,float,Vec3)noexcept;
void enemy_th14_death_metadata(EnemyState&)noexcept;
bool enemy_th14_clear_drops(EnemyState&)noexcept;
i32 enemy_th14_bomb_damage(const EnemyState&,i32 damage,i32 bomb_state,bool& resistant_sound)noexcept;
i32 enemy_th14_hit_sound(const EnemyState&,u32 spell_flags)noexcept;
i32 enemy_th14_hit_flash(EnemyState&,AnmVm&,u32 spell_flags)noexcept;
bool enemy_emitter_command(EnemyState&,EclContext&,EnemyGlobals&)noexcept;
i32 enemy_fire_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
i32 enemy_laser_command(EnemyState&,EclContext&,EnemyGlobals&,EnemyCommandEnvironment&)noexcept;
}
