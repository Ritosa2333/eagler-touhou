#include "completion.hpp"
#include "../gameplay/player_state.hpp"
#include "../progress_state/records.hpp"
#include <algorithm>
namespace th20::source::gameplay {
namespace ps=player_state;namespace n=recovered;
namespace {
int stage(CompletionEnvironment& e){return ps::stage(e.session().player_table);}
int difficulty(CompletionEnvironment& e){return ps::difficulty(e.session().player_table);}
game_session::Player& selected(CompletionEnvironment& e){return *e.session().contexts[0].current_player;}
int stone(CompletionEnvironment& e){return ps::read<int>(selected(e),12);}
int character(CompletionEnvironment& e){return ps::read<int>(selected(e),8);}
int continues(CompletionEnvironment& e){auto& value=e.session().player_table.continue_count;value=std::clamp(value,0,9);return value;}
int deaths(CompletionEnvironment& e){auto& table=e.session().player_table;const int value=std::clamp(ps::read<int>(table,0x208),0,99999);ps::write(table,0x208,value);return value;}
void increment_profile(CompletionEnvironment& e,unsigned field){auto& p=e.profile();const unsigned offset=field+static_cast<unsigned>(difficulty(e))*4u;const int value=progress::read<int>(p.bytes,offset);if(value<99999)progress::write(p.bytes,offset,n::signed_bits(static_cast<unsigned>(value)+1u));}
void mark_stage(CompletionEnvironment& e){auto& p=e.profile();const unsigned offset=0x7700u+static_cast<unsigned>(difficulty(e))*0x90u+static_cast<unsigned>(stage(e)-1)*0x10u;p.bytes[offset]=1;}
void award_stone(CompletionEnvironment& e){if(e.stone_count(static_cast<unsigned>(stone(e)))<9){e.notify_unlock(static_cast<unsigned>(stone(e))+1u);e.grant_stone(static_cast<unsigned>(stone(e)));}}
void record_clear(CompletionEnvironment& e,bool extra){
 if(e.game().restart()!=0)return;increment_profile(e,0x76b8);
 if(continues(e)==0){increment_profile(e,0x76d4);if(extra){progress::write(e.profile().bytes,0x76f4,1u);}else if(difficulty(e)>0){progress::write(e.profile().bytes,0x76f0,1u);const unsigned index=static_cast<unsigned>(stone(e))+9u+static_cast<unsigned>(character(e))*8u;if(!e.unlock_flag(index))e.notify_unlock(static_cast<unsigned>(stone(e))+9u+static_cast<unsigned>(character(e))*8u);}}
}
}
void mark_game_complete(GameController& game) noexcept{game.game_flags|=0x80u;game.field_10c=0;}
int complete_stage(GameController& target,CompletionEnvironment& e){
 if(e.game().restart()!=0&&(stage(e)==7||!e.replay_has_stage(stage(e)+1))){e.finish_replay();return 0;}
 e.finish_player();
 if(e.session().mode!=0){
  if(e.game().restart()==0){
   if(e.session().mode==2){auto& profile=e.profile();const unsigned offset=0xb08u+static_cast<unsigned>(ps::spell(e.session().player_table))*0xe0u+0xd8u;const std::uint64_t rounded=(ps::score(e.session().player_table.players[0])/10u)*10u;if(progress::read<std::uint64_t>(profile.bytes,offset)<rounded)progress::write(profile.bytes,offset,rounded);}
   if(e.game().restart()==0&&e.session().mode!=2)mark_stage(e);
   e.finish_practice();
  }else e.finish_replay();
  return 0;
 }
 if(stage(e)==6){
  mark_game_complete(target);e.mark_hud_clear();e.update_playtime();if(continues(e)==0)e.achievement(difficulty(e)+0x22);
  if(deaths(e)==0){e.achievement(0x26);if(difficulty(e)==3)e.achievement(0x27);}
  award_stone(e);record_clear(e,false);
 }else if(stage(e)==7){
  e.mark_hud_clear();if(e.replay_selection()<0){e.achievement(stone(e)+0x12+character(e)*8);award_stone(e);deaths(e);record_clear(e,true);e.update_playtime();mark_game_complete(target);}else{e.select_scene(4,true);e.replay_selection()=-1;}
 }else{
  if(e.game().restart()==0&&e.game().restart()==0)mark_stage(e);
  e.select_scene(12,false);e.advance_stage();e.session().flags=(e.session().flags&~4u)|4u;
 }
 return 0;
}
}
