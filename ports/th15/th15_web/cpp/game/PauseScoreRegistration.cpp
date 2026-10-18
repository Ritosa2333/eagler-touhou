#include "PauseScoreRegistration.hpp"
namespace th15 {
bool PauseScoreRegistration::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseScoreRegistration::register_result(){
 if(!error.empty())return false;const i32 character=progress.character+progress.subcharacter,difficulty=progress.difficulty;const bool legacy=(player.mode_flags&0x300)==0;const u32 mode=(player.mode_flags>>4)&3;
 if(character<0||character>=5||difficulty<0||difficulty>=5)return check(false,"Invalid result record selection");
 if(mode){if(mode!=2&&!check(records.practice_score(character,difficulty,progress.stage,score.score),"Practice result score failed"))return false;state.name_not_required=true;return true;}
 const i32 slot=records.score_rank(character,legacy,difficulty,score.score);if(slot<0){state.name_not_required=true;return true;}
 PauseScoreDetails details;if(!check(host.capture_score_details(details),"Result timestamp/performance unavailable"))return false;
 RunScoreSubmission row;row.score=score.score;row.stage=progress.stage==7&&state.result_mode?9:progress.stage;row.continues=progress.continues;row.timestamp=details.timestamp;row.slowdown=score_slowdown(details.delivered,details.requested);row.deaths=progress.stage_deaths[0];
 if(records.insert_score(character,legacy,difficulty,row)!=slot)return check(false,"Result score insertion changed its rank");
 state.menu.count=25;state.menu.wrapping=true;state.menu.select(slot);if(!editor.prepare())return check(false,"Result score name preparation failed");state.name_not_required=false;return true;
}
}
