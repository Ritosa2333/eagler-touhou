#pragma once
#include "EnemyPopulation.hpp"
#include "EnemyDeath.hpp"
#include "EnemyDamage.hpp"
#include "EnemyScoreCancel.hpp"
#include "EclSpawn.hpp"
#include "Chain.hpp"
namespace th08 {
struct EnemySimulationInput {EnemyFamiliarContext familiar;u8 focused=0,spell_bomb_damage=0;};
struct EnemySimulationState {
    Timer timer{0,0,0};i32 frames=0,unfocused_frames=0,active_frames=0,active_count=0;
    EnemyDropSequence drops;EclVm* layers[4]{};EclTimelineVm timelines[16];
};
struct EnemySimulationActions:EnemyAppearanceActions,EnemyFamiliarActions,EnemyContactActions,EnemyDamageActions,EnemyScoreActions {
    virtual void effect(i32,const Vec3&,i32,u32)override=0;
    virtual void item(const Vec3&,i32,i32)override=0;
    virtual void panned_sound(i32,float)override=0;
    virtual AnmVm* overlay(i32,const Vec3&,i32,u32)=0;
    virtual void move_overlay(AnmVm&,const Vec3&)=0;
    virtual bool cancel_projectiles(i32 maximum,bool reward,i32& score)=0;
    virtual void popup_bonus(i32 score)=0;
    virtual void message(i32 entry)=0;
    virtual void set_power(i32 value)=0;
};
// Owns the original EnemyManager::OnUpdate order around recovered ECL,
// enemy and player-collision components. Rendering consumes its layer lists.
class EnemySimulation:private EnemyPhaseActions,private EnemySpawnActions,private EnemyDeathActions,private TimelineActions {
    EnemySimulationState& state;EnemySimulationInput& input;EnemyPopulation& population;
    EclProgram& program;EclExecutor& executor;const FrameTiming& timing;EclGlobals& globals;
    GameGlobals& numbers;GameValues& values;GameRank& rank;PlayerFrameState& player;EnemySimulationActions& actions;
    EclTimelineExecutor timeline_executor;EnemyFamiliars familiars;EnemyDeath death;bool failed=false;
    bool dialogue_active()const noexcept;
    void sync_familiars()noexcept;
    bool finish_enemy(EclVm&,bool hit);
    void clear_familiars(EclVm&,bool reward)override;
    EnemySpawnResult spawn(const TimelineSpawn&,const EclContext::Locals&)override;
    void spawn(const TimelineSpawn&)override;
    AnmVm* overlay(i32 k,const Vec3& p,i32 n,u32 color)override{return actions.overlay(k,p,n,color);}
    void effect(i32 k,const Vec3& p,i32 n,u32 color)override{actions.effect(k,p,n,color);}
    void sound(i32 k,i32 mode)override{actions.sound(k,mode);}
    void panned_sound(i32 k,float x)override{actions.panned_sound(k,x);}
    i32 cancel_projectiles(i32 maximum,bool reward)override;
    i32 cancel_enemies(i32 maximum,i32 score)override;
    void popup_bonus(i32 score)override{actions.popup_bonus(score);}
    void message(i32 entry)override;
    void set_power(i32 value)override{actions.set_power(value);}
public:
    EnemySimulation(EnemySimulationState&,EnemySimulationInput&,EnemyPopulation&,EclProgram&,EclExecutor&,const FrameTiming&,Rng&,GameGlobals&,GameValues&,GameRank&,GameGauge&,DamageRegions&,PlayerFrameState&,EnemySimulationActions&);
    ~EnemySimulation();
    void bind()noexcept;
    JobResult update();
    bool invalid()const noexcept{return failed;}
};
}
