#pragma once
#include "EclOwner.hpp"
#include "Movement.hpp"
#include "Interpolation.hpp"
#include "Rng.hpp"
#include "BulletEmitter.hpp"
#include "PortTitle.hpp"
#if PORT_TITLE == 14
#include "EnemyTh14Emitter.hpp"
#endif
#if PORT_TITLE == 16
#include "EnemyTh16Emitter.hpp"
#endif
namespace th11 {
struct Enemy;
struct EnemyPhaseState;
class ScreenDeformation;
enum class EnemyTick:u32;
enum class EnemyDamage:u32;
enum class EnemyCollision:u32;
struct EnemyLink {Enemy* value;EnemyLink* next;EnemyLink* previous;};
struct EnemyInterrupt {i32 health,time;const char* health_script;const char* timeout_script;};
// TH12 widened the drop table: the constructor FUN_00413230 does
// `_memset(unaff_EDI + 0x998, 0, 0x58)` and 0x998 dwords is byte 0x2660, which is
// EnemyState+0x1620 given TH12's offsetof(Enemy,state)==0x1040. 0x58 = primary(4)
// + counts[19](0x4c) + spread(8), and opcode 406 clears exactly 0x4c bytes from
// 0x1624 (counts[0]) while 0x198 writes the two floats at 0x1670/0x1674. The
// constructor also sets both of those to 0x42000000 (32.0f), matching
// EnemyState::initialize's `drops.spread={32,32}`.
struct EnemyDrop {i32 primary,counts[19];Vec2 spread;};
static_assert(sizeof(EnemyDrop)==0x58);
// Layout recovered from the TH11 constructor and script-variable accessors.
// Unreconstructed effect fields remain explicitly reserved here.
struct EnemyState {
    Movement previous,current,absolute,relative;
    Vec2 hitbox,collision_box;
    u32 animations[10];
    i32 animation_file,bound_animation_file,animation_script,base_animation,direction;
    u32 animation_options; i32 facing;
    i32 integers[4];float floats[4],extra_floats[4];
    Timer lifetime;EnemyLink manager_link;
    Vec3Interpolator absolute_position,relative_position;
    Vec2Interpolator absolute_angle,relative_angle,absolute_radius,relative_radius;
    Vec2Interpolator absolute_ellipse,relative_ellipse;
    BulletEmitter emitters[8];
    Vec3 emitter_offset[8],emitter_origin[8];
    Vec2 visual_size,clamp_center,clamp_size;
    i32 score,health,max_health,interrupt_health,scaled_health,health_baseline;
    u32 health_flags;
    EnemyDrop drops;
    i32 death_sound,death_animation,death_animation_file,damage_flash,damage_reserved;
    Timer damage_immunity,collision_immunity;
    u32 flags; i32 alternate_animation,saved_animation,boss_slot;float exclusion_distance;
    EnemyInterrupt interrupts[8];
    Enemy* script_owner;ScreenDeformation* deformation;
    float deformation_target,deformation_radius;u32 deformation_color;
    float deformation_phase_x,deformation_phase_y;
    EnemyTick tick_callback;EnemyDamage damage_callback;EnemyCollision collision_callback;
    // 411 (0x19b) writes four words from its first argument. FUN_0041aba0 is the consumer:
    // it draws a closed ring of 31 points and reads ring_scale and ring_sides as
    // (scale * 1.875*pi) / sides + pi/8 for the step angle. ring_saved is a copy of the scale
    // and ring_step is seven times it. The port renders no ring yet, so only the state is kept.
    i32 ring_scale,ring_sides,ring_saved,ring_step;
    // 415 (0x19f) does Timer::set(e+0x1690, i(0)): the body pushes i(0) and calls FUN_004067e0 with
    // eax = enemy+0x1690, and FUN_004067e0 is Timer::set, the same function 413 calls. What TH12
    // uses this particular timer for is not established yet, so the field is named after its offset
    // rather than guessed at.
    Timer timer_1690;
    // 404 (0x194) sets flags bit 0x10000 and stores four floats read from arguments 0..3 into
    // +0x15f4..+0x1600; 405 (0x195) clears the flag. What TH12 uses the four floats for is not
    // established, so the field is named after its offset rather than guessed at.
    float quad_15f4[4];
    // 455 (0x1c7) tests whether an enemy with a given instance id exists, and FUN_00412990 shows
    // where that id comes from: enemy[0x27b4] = manager[0x74], a manager-side running count that
    // the port already keeps as EnemyManager::total_created. Assigned in spawn at the same point
    // TH12 does - after the parameter copies, before the first frame.
    i32 instance_id=0;
    // 526 (0x20e) spawns an 18-byte effect instance (factory id 0x11) on the enemy and stores
    // its parameters here. No instance is created because the port has no such system, the same
    // gap already recorded for 0x1c6; the parameters are kept so nothing is lost.
    i32 effect_active;float effect_duration,effect_scale;i32 effect_id;Vec2 effect_position;
    // Bit 0 of TH12's +0x161c: set by 422, cleared by 423, alongside the game-side loader those two
    // are a pair around. The loader itself is not modelled; the flag is enemy state.
    u32 ring_flags;
    // 529/530/531 store a callback index into three slots; 534 invokes one of them with the enemy
    // as its argument. TH12 stores function pointers from DAT_004af0fc/0x4af118/0x4af120; the port
    // keeps the index because the pointers are meaningless in wasm. Slot 1 of callbacks[1] is the
    // ring renderer FUN_0041aba0, whose descriptor 411 already keeps.
    i32 callbacks[3];
    // TH12's field at 0x1e0, written by 0x119 as indexed_values[argument0] = argument1 and reached by the
    // machine through [ebx+eax*4+0x1e0]. In the original it owns 0x1e0..0x324, the 80 floats before its
    // interpolator, and the case bodies index it at least up to 23 - see the run of accesses at 0x220..0x23c
    // in docs/th12/state-offsets.json - so 80 covers every measured use. Declaring it here, at the end, is
    // what removes the collision EnemyCommands.cpp records: a raw store at 0x1e0+index*4 used to land inside
    // this port's relative_position timer and absolute_angle from index 11 onward. It sits at its own offset
    // rather than TH12's, so it is not yet comparable by translation and the oracle does not compare it.
    i32 indexed_values[80]{};
    // TH12's field at 0x120, written by 0x117. The machine stores a three-float record per index through
    // [ebx + index*3*4 + 0x120]: argument 1, argument 2, and a literal zero. Measured over the 43 uses of
    // 0x117 in the nine campaign files, the index is 0 thirty-one times and 9, 10, 11 and 12 three times
    // each, so thirteen records cover every measured use. There is no bounds check in the machine - the bound
    // is the field's extent in the original - and a store past the extent would land on another of this
    // port's fields, so the index is bounded here and the record is kept by name rather than by offset.
    struct IndexedVector {float x=0,y=0,z=0;};
    IndexedVector indexed_vectors[13]{};
    // 453 (0x1c5) is an exact one-liner: enemy[0x168c] = i(0), with no other argument read. TH12's
    // 0x168c sits 0x2c before its flags word at 0x16b8 - the same word the port keeps at 0x159c - and
    // 0x168c is 4 bytes before the timer at 0x1690 that 415 already names by its offset, so it is an
    // integer immediately preceding that timer. Nothing else in the handler set reads it (see
    // docs/th12/state-offsets.json) and no port body reads it yet, so its consumer is still unknown
    // and it is named after its offset rather than guessed at, as the same gap was for timer_1690.
    i32 value_168c=0;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    // TH14 has sixteen independent animation handles at native +0x124.
    // Keep the first ten canonical slots and append the six additional IDs.
    u32 additional_animations[6]{};
    u32 parent_instance_id=0;
#endif
#if PORT_TITLE == 16
    Timer th16_time_drop_timer;
    i32 th16_time_drop_duration=60,th16_time_drop_minimum=1;
    i32 th16_chapter_tag=0,th16_chapter_credit=0;
    Vec2 th16_death_reference{};
    FloatInterpolator th16_motion_interpolation[4];
    Timer th16_phase_timer; // Native data +2bc; ECL 513.
    i32 th16_damage_total=0,th16_damage_drop_step=0,th16_damage_drop_next=0;
    char th16_interrupt_names[8][2][64]{}; // Native interrupt records own both strings.
    char th16_death_script[64]{}; // Native owner +56e4, ECL data +44d8.
    Th16EmitterState th16_emitters{};
#endif
#if PORT_TITLE == 14
    // Native enemy data +4034, initialized to 1 by 422fd5.
    struct Th14WaveMovement {Vec3 anchor;float phase;};
    float th14_bomb_damage_ratio=1;
    const char* th14_death_script=nullptr; // Native data +4108, ECL 556.
    Vec3 animation_offsets[16]{};
    i32 animation_parents[16]{};
    // Native +120 is shared by ECL564, animation velocity orientation and
    // rotated shot collision. Keep one value for all three consumers.
    float animation_angle=0;
    // Native entity +1470/+1474, consumed by 424af0's death-effect angle.
    Vec2 death_reference{};
    Timer timer_2bc;
    // Native +3c8/+3f8/+428/+458 are four independent scalar interpolators.
    FloatInterpolator motion_interpolation[4];
    Th14BulletEmitter th14_emitters[16];
    i32 th14_emitter_cursors[16];
    // Native +0x5a0 is the positioned part of the emitter record above (its +8 into the record at
    // +0x598): 0x2bc (700) writes its four float arguments into that record's +0x08, +0x0c, +0x10
    // and +0x340, and 0x2bd (701) writes its five integer arguments into +0x350..+0x360, with
    // argument 0 as the index, no bounds check (`lea esi,[eax*8]; sub esi,eax; shl esi,7`), and
    // the machine's own 0x380 stride. 0x2be (702) at 0x428a49 copies that record's body (0xc6
    // dwords from +0x28 - the transforms) into a 0x354-byte request, fills the request from the
    // record, and reads the record's own first two words (+0x598/+0x59c, the sprite and the
    // colour) into +0x24/+0x28; 0x259 (601) at 0x427ba9 writes the record's position from the two
    // triples below and hands the whole record to the spawner at 0x4191a0. One array serves all of
    // them, which is what the machine does.
    //
    // Native +0x3dd8 and +0x3e98, sixteen twelve-byte triples each. 603 writes the first (the
    // emitter's own offset), 628 the second (it carries the branch flag in z), and both 0x2be and
    // 0x259 add the second to the first - or add the entity's own position at +0x44/+0x48/+0x4c
    // when that z is not above the constant at [0x4c19e8] (its bits, 0x3f666666, measured).
    struct Th14IndexVector{float x=0,y=0,z=0;};
    Th14IndexVector th14_base_vectors[16],th14_offset_vectors[16];
    // The request 0x2be builds on the stack before calling the spawner at 0x43a750. The bytes are
    // kept raw because the spawner is not transcribed yet; every offset the command writes is named in
    // its body, and keeping the whole buffer is what makes the machine's version comparable.
    struct Th14Request{u8 bytes[0x374]{};};
    Th14Request th14_request;
    i32 th14_attached_knives=0; // Native entity +11ec.
    float th14_knife_slowdown=0; // Native entity +14d4; ECL/ANM clock reduction.
    bool th14_knife_clock_applied=false;
    Th14WaveMovement th14_absolute_wave{},th14_relative_wave{}; // Native movement +c..14 and +30.
    struct Th14PositionModes {i32 axis[3];u32 flags;};
    Th14PositionModes th14_absolute_position_modes{},th14_relative_position_modes{}; // Native position band +54..64.
#endif
    u32& animation_handle(u32 slot)noexcept{
#if PORT_TITLE == 14 || PORT_TITLE == 16
        return slot<10?animations[slot]:additional_animations[slot-10];
#else
        return animations[slot];
#endif
    }
    void initialize(Enemy*,const float* rate)noexcept;
    void combine_movement()noexcept;
    i32 update_movement(const float* rate,const Vec3& camera_delta)noexcept;
    i32 take_damage(i32)noexcept;
    const char* check_interrupts(EnemyPhaseState&)noexcept;
#if PORT_TITLE == 16
    const char* check_health_interrupt16(EnemyPhaseState&)noexcept;
    const char* check_timed_interrupt16(EnemyPhaseState&)noexcept;
#endif
};
struct Enemy {EclOwner script;EnemyState state;};
#if PORT_TITLE == 14
float enemy_th14_knife_rate(float,float)noexcept;
#endif
#if PORT_TITLE == 14
// The record stride is the machine's own multiply (`lea esi,[eax*8]; sub esi,eax; shl esi,7`).
static_assert(sizeof(EnemyState::Th14IndexVector)==12);
#endif
static_assert(offsetof(Enemy,state)==sizeof(EclOwner));
static_assert(offsetof(EnemyState,integers)==0x124);
static_assert(offsetof(EnemyState,lifetime)==0x154);
static_assert(offsetof(EnemyState,absolute_position)==0x174);
static_assert(offsetof(EnemyState,absolute_angle)==0x20c);
static_assert(offsetof(EnemyState,emitters)==0x374);
static_assert(offsetof(EnemyState,emitter_offset)==0x1414);
static_assert(offsetof(EnemyState,emitter_origin)==0x1474);
static_assert(offsetof(EnemyState,visual_size)==0x14d4);
static_assert(offsetof(EnemyState,health)==0x14f0);
// Offsets up to `drops` are TH11's and verified against TH12 where checked
// (emitters 0x374, emitter_offset 0x1414, health 0x14f0 all agree through the
// shared region below 0x1414). TH12's wider drop table then pushes everything
// after it by 0x1c, so the tail asserts below are the port's own layout, not
// TH12 addresses. TH12's real values would be flags 0x16b8 and script_owner
// 0x174c, and offsetof(Enemy,state) would be 0x1040 rather than TH11's 0x103c -
// the port keeps TH11's EclOwner size, so those two remain unreconciled and are
// recorded here rather than silently assumed equal.
static_assert(offsetof(EnemyState,flags)==0x159c);
static_assert(offsetof(EnemyState,script_owner)==0x1630);
// The four ring words above are the port's own addition; the size moved with them. indexed_values adds 320
// bytes (80 i32), and it is a member rather than a raw offset so that opcode 0x119 stops writing over
// relative_position's timer and absolute_angle; the sizes moved with it too.
// 0x168c is a further four bytes on top of the 320 the ring words and indexed_values already add.
// indexed_vectors adds 0x9c (thirteen three-float records, the largest index the shipped scripts use),
// and it is a member for the same reason indexed_values is.
// The two native triples at +0x3dd8/+0x3e98 (0x180) replace the two role-named arrays the port
// used to keep in their place, and 0x2be's 0x354-byte request is the only new storage.
static_assert(sizeof(EnemyState)==0x16b8+0x140+4+0x9c+(kPortTitle==14?500+0x39c0+0x374+12+64:kPortTitle==16?148+4*sizeof(FloatInterpolator)+sizeof(Timer)+1024+0x39c0:0));
static_assert(sizeof(Enemy)==0x26f4+0x140+4+0x9c+sizeof(EclContext)-0x1024+(kPortTitle==14?500+0x39c0+0x374+12+64:kPortTitle==16?148+4*sizeof(FloatInterpolator)+sizeof(Timer)+1024+0x39c0:0));
struct EnemyEnvironment {
    Rng random;float rate=1;Vec3 player_position,camera_delta;
    i32 rank=0,difficulty=0,character=0,subtype=0,shared_integers[3]{},enemy_count=0;
#if PORT_TITLE == 14
    // Retail enemy manager +2c..38 and +3c..58, shared by all owners.
    i32 th14_shared_integers[4]{};float th14_shared_floats[8]{};
    EnemyLink* const* th14_enemy_list=nullptr;
    i32 th14_targetable_enemy_count()const noexcept;
    // Optional read-only reference instrumentation, before an ECL RNG read.
    void (*th14_random_read)(i32 id,i32 enemy,u32 calls)=nullptr;
#endif
#if PORT_TITLE == 16
    // Own retail manager +0c..18 / +1c..38; ECL -9926..-9915.
    i32 th16_shared_integers[4]{};float th16_shared_floats[8]{};
#endif
    // TH14 4f5844 / ECL -9907; ordinary title starts write -1 at 45e428.
    i32 spell_practice_id=-1;
    EnemyState* boss=nullptr;
    i32 (*animation_sprite)(u32 animation,void* user)=nullptr;void* animation_user=nullptr;
    // ECL and ANM/bullets consume one original script RNG, in execution order.
    // Standalone comparison fixtures use random; the game supplies shared_random.
    Rng* shared_random=nullptr;
#if PORT_TITLE == 16
    Rng* th16_visual_random=nullptr; // Own4a6d88; distinct from ECL's stream.
#endif
    Rng& rng()noexcept{return shared_random?*shared_random:random;}
};
struct EnemyGlobals:EclGlobals {
    EnemyState& enemy;EnemyEnvironment& environment;
    EnemyGlobals(EnemyState& e,EnemyEnvironment& w):enemy(e),environment(w){}
    i32 integer(i32)override;
    double floating(i32)override;
    i32* integer_reference(i32)override;
    float* float_reference(i32)override;
};
float aim_angle(const Vec3& origin,const Vec3& player)noexcept;
float position_distance(const Vec3&,const Vec3&)noexcept;
float enemy_th14_motion_sample(FloatInterpolator&,const float*)noexcept;
float enemy_th16_motion_sample(FloatInterpolator&,const float*)noexcept;
Vec3 enemy_th16_position_sample(Vec3Interpolator&,const float*)noexcept;
void enemy_th16_ellipse_position(Movement&)noexcept;
Vec3 enemy_th14_position_sample(Vec3Interpolator&,const float*,const i32* axis_modes=nullptr)noexcept;
Vec2 enemy_th14_radius_sample(Vec2Interpolator&,const float*)noexcept;
void enemy_th14_ellipse_position(Movement&)noexcept;
#if PORT_TITLE == 14
void enemy_th14_wave_position(Movement&,EnemyState::Th14WaveMovement&,const float*)noexcept;
#endif
struct EnemyPhaseState {
    const float* rate; i32* countdown_seconds; i32* countdown_hundredths;
    u32* spell_flags; i32* spell_elapsed; i32* spell_bonus;
    i32* shared_spell_state; i32 special_active,character,subtype;
    i32* chapter=nullptr; i32* chapter_removed=nullptr;
};
}
