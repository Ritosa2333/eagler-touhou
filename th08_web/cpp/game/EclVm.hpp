#pragma once
#include "EclProgram.hpp"
#include "Rng.hpp"
#include "Timer.hpp"
#include "CombatRuntime.hpp"
#include "GameValues.hpp"
#include "AnmLayout.hpp"
#include "BulletEmission.hpp"
#include "EnemyTrail.hpp"
#include "SpellState.hpp"
#include <memory>
namespace th08 {
struct EclEvent {
    i16 opcode=0;u16 reserved=0;u32 arguments[6]{};i32 frame=0;
};
struct EclEnemy {
    u8 active=0,kind=0,flags=0,reserved=0;Vec3 position;float life=0,hitbox=0;u32 anm=0;
};
struct EclBullet {
    u8 active=0,kind=0,color=0,reserved=0;Vec3 position,velocity;float angle=0,speed=0,radius=0;u32 flags=0;
};
struct EclLaser {
    u8 active=0,kind=0,flags=0,reserved=0;Vec3 position;float angle=0,length=0,width=0;u32 index=0;
};
struct EclItem {u8 active=0,type=0,reserved[2]{};Vec3 position;u32 count=0;};
struct EclVm;
struct EclSpellActions {virtual ~EclSpellActions()=default;virtual bool begin(EclVm&,const EclInstruction&)=0;virtual bool end()=0;};
struct EnemyPhaseActions {virtual ~EnemyPhaseActions()=default;virtual void clear_familiars(EclVm& enemy,bool reward)=0;};
struct GuiState;struct AsciiState;struct EffectState;struct PlayerFrameState;struct EnemySpawnActions;struct EclSceneActions;struct EclNativeServices;
struct TimelineSpawn {
    i32 subroutine=0;Vec3 position;i32 life=0,item=0,score=0;u32 mirror=0;
    bool multiple_items=false;i32 point_items=0,power_items=0;
};
struct TimelineActions {
    virtual ~TimelineActions()=default;
    virtual void spawn(const TimelineSpawn& request)=0;
    virtual void message(i32 entry)=0;
    virtual void set_power(i32 value)=0;
};
struct EclLiveValues {
    virtual ~EclLiveValues()=default;
    virtual i32 pending_time()const=0;
    virtual i32 uncollected_time_items()const=0;
};
struct EclGlobals:SpellState {
    AnmLoaded* enemy_animation_files[2]{};
    BulletEmissionActions* bullet_actions=nullptr;
    LaserEmissionActions* laser_actions=nullptr;
    EnemyPhaseActions* phase_actions=nullptr;
    PlayerFrameState* player_frame=nullptr;
    EnemySpawnActions* enemy_actions=nullptr;
    EclSceneActions* scene_actions=nullptr;
    EclSpellActions* spell_actions=nullptr;
    i16 current_spell=-1;
    EclNativeServices* native_services=nullptr;
    i32 frame_count=0,timeline_parameter=0;i32* frame_count_value=nullptr;
    BulletEmission emitter_template=[](){BulletEmission value;value.sound=7;value.transform_sound=25;return value;}();
    i8 player_state=0;Timer player_state_timer;
    GuiState* gui=nullptr;AsciiState* ascii=nullptr;u32 game_flags=0;
    Vec3 player;Vec3 boss;u32 stage=0;u32 difficulty=0,difficulty_mask=1;u8 paused=0;
    i32 rank=0,shot=0;bool youkai=false;i32* rank_value=nullptr;
    i32 current_rank()const noexcept{return rank_value?*rank_value:rank;}
    i32 integer_arguments[4]{};float float_arguments[4]{};
    GameGlobals* values=nullptr;i32 pending_time=0,uncollected_time_items=0;const EclLiveValues* live_values=nullptr;
    TimelineActions* timeline_actions=nullptr;EclVm* boss_slots[8]{};
    bool gui_blocks_spawn=false,dialogue_active=false;u8 stage_completion=0;i32 stop_spawn=0,timeline_signals[4]{-1,-1,-1,-1};
    EclEvent events[256]{};u32 event_count=0;u32 bullets_created=0;u32 enemies_created=0;
    EclEnemy enemies[128]{};EclBullet bullets[1024]{};EclLaser lasers[64]{};EclItem items[256]{};
    i32 boss_life=0,boss_lives=0,stage_interrupt=0;u32 bullet_rank=0;float player_protect_range=0;u8 player_nullified=0;
    CombatRuntime* combat=nullptr;
    void clear_bullets() noexcept {for(auto& b:bullets)b.active=0;}
    void clear_lasers() noexcept {for(auto& l:lasers)l.active=0;}
    void record(i16 opcode,const u32* args,u32 count,i32 frame) noexcept {
        if(event_count<256){EclEvent& e=events[event_count++];e.opcode=opcode;e.frame=frame;for(u32 i=0;i<count&&i<6;++i)e.arguments[i]=args[i];}
        if(opcode>=90&&opcode<=104)++bullets_created;
        if(opcode==93||opcode==94)++enemies_created;
    }
};
struct EclContext {
    struct Locals {
        i32 integers[8]{};float floats[8]{};i32 counters[4]{};float extra[2]{};i32 integer_arguments[4]{};float float_arguments[4]{};
    } locals;
    static_assert(sizeof(Locals)==120);
    struct Interpolation {u32 active=0;Timer time{0,0,0};i32 duration=0,curve=0,easing=0;float values[4]{},target=0;};
    static_assert(sizeof(Interpolation)==48);
    Interpolation interpolations[8]{};
    struct CallFrame {Timer time,wait;i32 subroutine;Locals locals;Interpolation interpolations[8];i32 native_callback=-1;const EclInstruction* native_instruction=nullptr;};
    CallFrame call_frames[16]{};
    EclInstruction* instruction=nullptr;EclInstruction* branch_instruction=nullptr;bool branched=false;
    i32 subroutine=-1,call_depth=0;EclInstruction* call_stack[16]{};
    Timer timer,wait_timer;bool returned=false;
    i32 native_callback=-1;const EclInstruction* native_instruction=nullptr;
    i32* integer_variable(i32 id)noexcept{
        if(id>=10000&&id<=10007)return locals.integers+id-10000;
        if(id>=10036&&id<=10039)return locals.counters+id-10036;
        if(id>=10053&&id<=10056)return locals.integer_arguments+id-10053;
        return nullptr;
    }
    float* float_variable(i32 id)noexcept{
        if(id>=10016&&id<=10023)return locals.floats+id-10016;
        if(id>=10094&&id<=10095)return locals.extra+id-10094;
        if(id>=10057&&id<=10060)return locals.float_arguments+id-10057;
        return nullptr;
    }
};
static_assert(sizeof(EclContext)<=0x24b0);
struct EclVm {
    enum class Failure:u8 {None,MissingBoss,MissingAnimation,MissingEnemyActions,MissingEffect,MissingSceneActions,MissingNativeServices,UnsupportedNativeCallback,MissingSpellServices};Failure failure=Failure::None;
    using Interpolation=EclContext::Interpolation;
    EclProgram* program=nullptr;EclContext main_context;EclContext* active_context=nullptr;
    std::unique_ptr<EclContext> asynchronous[4];u32 asynchronous_generations[4]{};i32 active_slot=-1,scratch_depth=0;
    i32 shared_integer[8]{};float shared_real[8]{};
    EclContext& context()noexcept{return active_context?*active_context:main_context;}
    const EclContext& context()const noexcept{return active_context?*active_context:main_context;}
    Rng* random=nullptr;
    EclGlobals* environment=nullptr;
    EclVm* parent=nullptr;EclVm* next_familiar=nullptr;
    i32 life=0,initial_life=0,remaining_life=0,pool_index=0,point_items=0,power_items=0;
    Timer lifetime;i32 last_damage=0,boss_id=0,life_thresholds[4]{},item_reward=0,score_reward=0;i16 pending_interrupt=-1,death_subroutine=0;
    i16 interrupt_subroutines[32]{};
    AnmVm animation[3];i16 poses[6]{-1,-1,-1,-1,-1,-1};i8 pose_direction=0;
    BulletEmission emitter;float rank_speed_low=0,rank_speed_high=0;i16 rank_count_low=0,rank_count_high=0,rank_layers_low=0,rank_layers_high=0;
    alignas(4) u8 repeated_shot[44]{};i32 emission_period=0;Timer emission_time{0,0,0};
    BulletEmission laser_emitter;LaserState* laser_slots[32]{};i32 laser_slot=0;
    i32 life_subroutines[4]{},timeout=0,timeout_subroutine=0;EffectState* effects[24]{};i32 effect_count=0;
    i32 remaining_seconds=0;
    i32 animation_color=0;float effect_radius=0;
    // Native 2db8 is the projectile offset; motion origin lives at 2dc4.
    // A moving enemy may change either independently while firing.
    Vec3 position_offset,resolved_position,emission_offset,origin,target,last_delta,previous_position;u8 difficulty_flags=0;
    float angular_velocity=0,speed=0,acceleration=0,orbit_velocity=0,orbit_growth=0,bounds[4]{};
    Timer movement_time{0,0,0};i32 movement_duration=0;
    Vec3 position,velocity,direction,hitbox,low_damage_hitbox;float player_protect_squared=0,orbit_angle=0,orbit_radius=0;u32 flags=0,flags2=0;AnmVm* familiar_effect=nullptr;bool waiting=false,finished=false,invalid=false;
    Timer damage_protection;
    u8 death_effects[3]{}; // Native 3310..3312; first entry is used as signed.
    EclVm* previous_familiar=nullptr;i32 summoned_familiars=0,practice_familiar=0;
    i32 time_items=0;
    u8 draw_layer=0,hit_flash=0;
    EnemyTrail trail;
    EclVm* next_in_layer=nullptr;
    i32* integer_target(const EclInstruction&,u32 index)noexcept;
    float* float_target(const EclInstruction&,u32 index)noexcept;
    i32* integer_field(i32 id)noexcept;
    float* float_field(i32 id)noexcept;
    i32 read_int(i32 id)const noexcept;float read_float(i32 id)const noexcept;
    void write_int(i32 id,i32 value)noexcept;void write_float(i32 id,float value)noexcept;
    i32 operand_int(const EclInstruction&,u32 index)const noexcept;float operand_float(const EclInstruction&,u32 index)const noexcept;
    i32 variable_int(const EclInstruction&,u32 index)const noexcept;
    i32 variable_float(const EclInstruction&,u32 index)const noexcept;
    Extended resolve_float(float value)const noexcept;
    void interpolate(const FrameTiming& timing);
    i32 familiar_count()const noexcept;
    void refresh_position()noexcept;
    bool execute_motion(const EclInstruction& instruction);
    bool execute_lifecycle(const EclInstruction& instruction);
    void update_motion(const FrameTiming& timing);
    void clamp_position()noexcept;
    void apply_velocity(const FrameTiming& timing)noexcept;
    void integrate_position(const FrameTiming& timing)noexcept;
    void update_attached_effects()noexcept;
};
class EclExecutor {
    Rng& rng;const FrameTiming& timing;EclGlobals& globals;
    bool dispatch(EclVm&,const EclInstruction&);
    void record(EclVm&,const EclInstruction&) noexcept;
    bool jump(EclVm&,const EclInstruction&,i32 time,i32 relative) noexcept;
    bool call(EclVm&,const EclInstruction&,i32 subroutine);
    bool interrupt(EclVm&,const EclInstruction&);
    bool load_animation(EclVm&,u32 slot,i32 file,i32 script);
    bool animate(EclVm&,const EclInstruction&);
    bool update_pose(EclVm&);
    bool configure_emitter(EclVm&,const EclInstruction&);
    bool configure_laser(EclVm&,const EclInstruction&);
    bool shoot(EclVm&,const EclInstruction&);
    bool repeat_shot(EclVm&);
    bool step_context(EclVm&);
public:
    EclExecutor(Rng& r,const FrameTiming& t,EclGlobals& g):rng(r),timing(t),globals(g){}
    EclGlobals& game_state()noexcept{return globals;}
    bool switch_main(EclVm&,i32 subroutine);
    bool update_animations(EclVm&);
    bool start(EclVm&,EclProgram&,i32 subroutine);
    bool step(EclVm&);
};
struct EclTimelineVm {EclProgram* program=nullptr;EclTimelineInstruction* instruction=nullptr;i32 timeline=-1;Timer timer;u32 difficulty=0;bool finished=false,invalid=false;};
class EclTimelineExecutor {
    Rng& rng;const FrameTiming& timing;EclGlobals& globals;
    void record(EclTimelineVm&,const EclTimelineInstruction&) noexcept;
public:
    EclTimelineExecutor(Rng& r,const FrameTiming& t,EclGlobals& g):rng(r),timing(t),globals(g){}
    bool start(EclTimelineVm&,EclProgram&,i32 timeline,u32 difficulty);
    bool step(EclTimelineVm&);
};
}
