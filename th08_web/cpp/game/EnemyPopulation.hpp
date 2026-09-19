#pragma once
#include "EclVm.hpp"
#include <array>
#include <memory>
namespace th08 {
struct EnemyScoreActions;
// Original allocation order: 480 live enemy slots and a separate overflow
// result. Each enemy owns its ECL locals, timers and call stack.
class EnemyPopulation {
    std::array<std::unique_ptr<EclVm>,481> enemies;
    EclExecutor& executor;EclProgram& program;
    void kill_non_bosses();
    void protect_player(const EclVm&);
    void cancel_async(EclVm&);
    void reset_emitter(EclVm&);
    EclVm* spawn_impl(const TimelineSpawn&,const EclContext::Locals* inherited);
public:
    bool spawn_failed=false;u16 replay_flags=0;i32 initial_time_items=0;
    i32 practice_familiar=0;
    EnemyPopulation(EclExecutor& executor,EclProgram& program):executor(executor),program(program){}
    void reset(i32 time_items=0)noexcept;
    EclVm* spawn(const TimelineSpawn& request);
    EclVm* spawn_inherited(const TimelineSpawn& request,const EclContext::Locals& locals){return spawn_impl(request,&locals);}
    EclVm* at(u32 index)const noexcept{return index<enemies.size()?enemies[index].get():nullptr;}
    u32 active_count()const noexcept;
    bool check_life(EclVm&);
    bool check_timeout(EclVm&);
    bool cancel_for_score(i32 maximum,i32& score,EnemyScoreActions&);
    // This is the recovered script/position phase, not the complete enemy
    // lifecycle: collision, death callbacks, culling and ANM phases remain.
    void update_scripts(const FrameTiming& timing);
};
}
