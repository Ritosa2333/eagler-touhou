#pragma once
#include "SessionState.hpp"
namespace th15 {
struct CompletionRecords {
 virtual ~CompletionRecords()=default;
 virtual bool stage_clear(i32 character,i32 stage,i32 difficulty)=0;
 virtual bool finished_run(i32 character,bool legacy,i32 difficulty,bool without_continue)=0;
 virtual bool spell_score(i32 character,bool legacy,i32 spell,i32 score)=0;
 virtual bool remove_checkpoint(i32 character,i32 difficulty)=0;
};
struct CompletionServices {
 virtual ~CompletionServices()=default;
 virtual bool all_clear_bonus()const{return false;}
 virtual bool synchronize_clear_score(){return true;}
 virtual bool clear_notice()=0;virtual bool finish_player_options()=0;virtual bool end_bomb()=0;
 virtual bool prepare_ending()=0;virtual bool finish_replay()=0;virtual bool finish_practice()=0;
 virtual bool queue_next_scene()=0;virtual bool select_stage_resources(i32 stage)=0;
};
class SessionCompletion {
 SessionState& progress;PlayerLifeSession& player;ItemScoreState& score;u32& hud_flags;i32& clear_bonus;i32& ending_frames;
 CompletionRecords& records;CompletionServices& services;bool check(bool,const char*);void award(i32);bool record_finish();
public:
 std::string error;
 SessionCompletion(SessionState& p,PlayerLifeSession& s,ItemScoreState& v,u32& flags,i32& bonus,i32& ending,CompletionRecords& r,CompletionServices& h):progress(p),player(s),score(v),hud_flags(flags),clear_bonus(bonus),ending_frames(ending),records(r),services(h){}
 bool complete();
};
}
