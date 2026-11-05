#include "PauseScoreRegistration.hpp"
namespace th17 {
bool PauseScoreRegistration::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseScoreRegistration::register_result(){
    if(!error.empty())return false;const i32 loadout=progress.character*3+progress.animal,difficulty=progress.difficulty;const u32 mode=(progress.scoring.mode_flags>>4)&3;
    if(loadout<0||loadout>=9||difficulty<0||difficulty>=5)return check(false,"Invalid result record selection");
    state.name_not_required=true;
    if(mode){if(mode!=2){if(u32(progress.stage)>=8)return check(false,"Invalid practice stage record");auto& best=records.game.practice_scores[loadout][difficulty][progress.stage];if(u32(best)<u32(score.score))best=score.score;}return true;}
    const auto& rows=records.scores[loadout][difficulty];i32 slot=0;while(slot<10&&u32(rows[slot].score)>u32(score.score))slot++;if(slot==10)return true;
    PauseScoreDetails details;if(!check(host.capture_score_details(details),"Result timestamp/performance unavailable"))return false;
    RunScoreSubmission row;row.score=score.score;row.stage=progress.stage==7&&state.result_mode?9:progress.stage;row.continues=progress.scoring.continues;row.timestamp=details.timestamp;row.slowdown=float(100.f-float(float(details.delivered/details.requested)*100.f));
    if(records.insert_score(loadout,difficulty,row)!=slot)return check(false,"Result score insertion changed its rank");
    state.menu.count=25;state.menu.wrap=1;state.menu.select(slot);if(!editor.prepare())return check(false,"Result score name preparation failed");state.name_not_required=false;return true;
}
}
