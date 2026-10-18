#include "native-world.hpp"
#include "../../th15_web/cpp/game/PauseMenu.hpp"
#include "../../th15_web/cpp/game/PauseActions.hpp"
#include <algorithm>
struct MenuHost final:PauseMenuServices {
 bool sound(i32)override{return true;}bool synchronize_result_score()override{return true;}
 bool capture_score_details(PauseScoreDetails&)override{return true;}
 bool read_replay_slot(i32,std::shared_ptr<Replay>& r)override{r.reset();return true;}
 bool save_named_replay(i32,const std::array<char,9>&)override{return true;}
 bool prepare_replay_save(bool)override{return true;}bool open_options(float)override{return true;}
 bool options_finished()const override{return true;}bool close_options()override{return true;}
 bool finish_pause_selection()override{return true;}
};
struct ActionHost final:PauseRestorationServices,PauseGameplayActions,PauseActionServices {
 i32 destination_value=-1;unsigned gameplay_calls=0;
 bool restart_elapsed_play_time()override{return true;}bool interrupt_pause_visual(u32,i32)override{return true;}
 bool resume_dialogue()override{return true;}bool resume_chapter_result()override{return true;}
 bool restore_checkpoint()override{++gameplay_calls;return true;}bool options_changed()override{++gameplay_calls;return true;}
 bool life_hud(i32,i32)override{++gameplay_calls;return true;}bool bomb_hud(i32,i32)override{++gameplay_calls;return true;}
 bool power_notice()override{++gameplay_calls;return true;}bool resume_looping_sounds()override{return true;}
 bool resume_music()override{return true;}bool resume_saved_music(const std::string&,double)override{return true;}
 bool reset_audio_slots()override{return true;}bool destination(PauseDestination d)override{destination_value=i32(d);return true;}
};
int main(int argc,char** argv){
 require(argc==2,"Retail archive required");RetailAssets files(argv[1]);
 for(unsigned count:{2u,3u})for(unsigned giver=0;giver<count;++giver){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;w.output.dialogue=true;const unsigned receiver=(giver+1)%count;
  auto& donor=*b.pilots[giver];auto& target=*b.pilots[receiver];target.session.extra_lives=-1;require(target.game_over(),b.error);donor.session.extra_lives=2;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s]->player;p.motion.position={s==giver||s==receiver?0.f:150.f,400,0};p.motion.position_fixed={i32(p.motion.position.x*128),51200};}
  std::array<MultiplayerInput,3> input{};input[giver].held=8;
  for(unsigned n=0;n<89;++n){target.ghost_clock=1;target.ghost_dx=target.ghost_dy=0;w.step(input);}
  require(target.ghost&&donor.life_hold==89&&donor.session.extra_lives==2,"Dialogue rescue did not reach 89 ticks without charging");
  target.ghost_clock=1;target.ghost_dx=target.ghost_dy=0;w.step(input);
  require(!target.ghost&&target.alive()&&donor.session.extra_lives==1&&target.session.extra_lives==0,"Dialogue rescue failed on tick 90");
  require(target.session.power==200&&target.session.bombs==2&&target.player->life.invulnerability.current>=279,"Rescue native resources or protection incorrect");
  w.step(input);require(donor.life_latched&&donor.session.extra_lives==1,"Held Shift charged a second life");
  for(u32 flags:{0x10u,4u,0x4000u,0x10000u}){BattleFrame frame;frame.game_flags=flags;b.mp_input(input);donor.life_hold=89;require(b.step(frame),b.error);require(donor.life_hold==0&&donor.session.extra_lives==1,"Paused/inactive frame permitted life transfer");}
 }
 World w(files,3,0,{0,1,3});auto& b=*w.battle;RecordStore records;
 for(auto& character:records.characters)for(auto& mode:character.modes)for(auto& difficulty:mode.scores)for(auto& row:difficulty)row.score=1000000;
 for(PauseScreen screen:{PauseScreen::GameOver,PauseScreen::Results}){
  PauseState state;state.multiplayer=true;state.select(screen);state.front_bank=w.assets.front;state.phase=2;state.age.set(10);
  MenuHost host;PauseMenu menu(state,w.progress,b.session,b.score,records,w.animations,host);require(menu.update(0,0),menu.error);
  require(state.phase==6&&state.menu.cursor==1,"Multiplayer result did not default to Return");
  for(unsigned n=0;n<10;++n){require(menu.update(0x20,0),menu.error);require(state.menu.cursor==1||state.menu.cursor==2,"End menu admitted Continue/Retry/Options");}
  require(menu.update(0x200000,0),menu.error);require(state.phase==6,"R shortcut restarted a completed generation");
  state.menu.select(2);require(menu.update(1,0),menu.error);require(state.phase==10,"Replay Save unavailable");state.age.set(20);require(menu.update(0,0),menu.error);
  require(state.phase==11&&state.menu.count==25,"Replay slot owner unavailable");state.age.set(10);require(menu.update(0x100,0),menu.error);require(state.phase==6,"Replay cancel did not return to end menu");
  require(menu.update(0,0),menu.error);state.menu.select(0);require(state.menu.cursor==1,"Replay cancel re-enabled Continue");state.menu.select(4);require(state.menu.cursor==1,"Replay cancel re-enabled Retry");
  require(menu.update(0x100,0),menu.error);require(state.phase==16&&state.menu.cursor==1,"Esc resumed a failed generation");
  for(i32 choice:{0,4}){state.menu.cursor=choice;i32 skip=0;ActionHost actions_host;PauseRestoration restore(state,w.progress,skip,actions_host);PauseActions actions(state,w.progress,b.session,b.score,w.animations,restore,actions_host,actions_host,skip);
   b.session.extra_lives=-1;b.session.power=150;b.session.bombs=0;b.score.score=12345;const auto credits=w.progress.continue_budget;
   require(actions.execute(false),actions.error);require(actions_host.destination_value==i32(PauseDestination::Title)&&!actions_host.gameplay_calls,"End action entered single-player Continue/Retry");
   require(b.session.extra_lives==-1&&b.session.power==150&&b.session.bombs==0&&b.score.score==12345&&w.progress.continue_budget==credits,"End action reset only P1 resources");
  }
 }
 {
  PauseState state;state.select(PauseScreen::GameOver);state.front_bank=w.assets.front;state.phase=2;state.age.set(10);MenuHost host;
  PauseMenu menu(state,w.progress,b.session,b.score,records,w.animations,host);require(menu.update(0,0),menu.error);require(state.menu.cursor==0,"Single-player Continue was disabled");
  i32 skip=0;ActionHost actions_host;PauseRestoration restore(state,w.progress,skip,actions_host);PauseActions actions(state,w.progress,b.session,b.score,w.animations,restore,actions_host,actions_host,skip);
  const auto credits=w.progress.continue_budget;require(actions.execute(false),actions.error);
  require(actions_host.destination_value==-1&&actions_host.gameplay_calls&&b.session.extra_lives==2&&b.score.score==0&&w.progress.continue_budget==credits-1,"Single-player native Continue changed");
 }
 std::puts("PASS: dialogue rescue at 89/90 ticks for all 2P/3P donors, one-life cost, protection and pause gates; TH11-style end navigation, R/Esc, Replay cancel and defensive Continue/Retry actions; single-player Continue preserved");
}
