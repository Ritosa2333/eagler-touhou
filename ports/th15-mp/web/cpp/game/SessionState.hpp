#pragma once
#include "ItemCollection.hpp"
#include "EnemyState.hpp"
namespace th15 {
// Progress belongs to the run. It survives scene factories and is distinct
// from per-frame player input, presentation state and stored records.
struct SessionState {
    i32 stage=1,chapter=0,stage_frame=0,run_clock=0,character=0,subcharacter=0;
    i32 difficulty=0,continues=0,spell_id=-1,initial_point_value=0;
    i32 high_score=0,high_score_extra=0,configured_lives=3;
    std::array<i32,9> stage_deaths{};i32 chapter_deaths=0;
    float rate=1;u32 scene_flags=0;i32 transition=0,startup_frames=0;
    bool new_run=true,replay=false;
    i32 checkpoint_power=0,restart_frames=0;
    i32 alternating_pieces=0;
    i32 continue_budget=5;
    i32 starting_stage=1;
};
enum class RecordScope {FullRun,Stage,Spell};
struct SessionRecordQuery {RecordScope scope; i32 character,difficulty,stage,spell;bool legacy;};
struct SessionHighScore {i32 score=0,extra=0;};
struct SessionRecords {
    virtual ~SessionRecords()=default;
    virtual SessionHighScore high_score(const SessionRecordQuery&)=0;
    virtual bool mark_stage(i32 character,i32 stage,i32 difficulty)=0;
    virtual bool count_play(i32 character,bool legacy)=0;
};
}
