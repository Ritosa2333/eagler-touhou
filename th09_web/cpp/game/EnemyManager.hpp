#pragma once
#include "PoolCheckpoint.hpp"
#include "EnemyTimeline.hpp"
#include "EnemyFrame.hpp"
#include <array>
namespace th09 {
class EnemyManager {
    EclWorldState& world;EclPlayfieldState& field;EclPlayfieldState& opponent;
public:
    PoolCheckpoint* checkpoint=nullptr;
    static constexpr u32 capacity=128;
    EclProgram common_program,character_program;
    EclVm prototype;std::array<EclVm,capacity+1> enemies;
    std::array<EclVm*,8> bosses{};
    EnemyTimeline timeline;i32 timeline_events[4]={-1,-1,-1,-1};
    i32 timeline_control=0,attack_control=0;
    i32 alive=0,normal_alive=0,attack_alive=0,spirit_alive=0,capture_lifetime=0;
    Timer focus_time{0,0,0},frame_time{0,0,0};u8 pattern_index=0;
    EclVm* priority_target=nullptr;EclVm* first_target=nullptr;
    // Lists are stored in reverse insertion order by step_frame, matching the
    // original draw chain while retaining stable C++ object references.
    std::array<std::vector<EclVm*>,4> draw_lists;
    EnemyPlayerState* player=nullptr;EnemyFrameActions* frame_actions=nullptr;
    bool allocation_failed=false;EclBindings bindings;
    FrameTiming timing;float frame_step=1;u32 difficulty_mask=1;
    EnemyManager(EclWorldState&,EclPlayfieldState&,EclPlayfieldState&);
    EclVm* create(const EnemySpawn&,const EclLocals* inherited=nullptr);
    bool run_script(EclVm&);
    bool step_frame(const EnemyFrameSettings&);
    void remove(EclVm&);
    EclVm* find_attack(u32 kind)noexcept{for(u32 n=0;n<capacity;++n){auto& e=enemies[n];if((e.behavior_flags&1)&&((e.values.flags>>10)&3)==kind)return &e;}return nullptr;}
};
}
