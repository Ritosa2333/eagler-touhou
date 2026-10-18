// Real menu state machines with asset-free animations and fake storage/audio.
// This is not visual/device or native-execution parity evidence.
#include "../th15_web/cpp/game/PauseMenu.hpp"
#include "../th15_web/cpp/game/TitleReplaySave.hpp"
#include "../th15_web/cpp/game/SessionCompletion.hpp"
#include "../th15_web/cpp/game/EnemyCommands.hpp"
#include <cassert>
#include <iostream>
using namespace th15;
void emission_distance_tests(){
 EnemyState enemy;EnemyWorldState world;EnemyCommands commands(enemy,world);EclContext context;
 struct Instruction {EclInstruction header{};float distance=0;} instruction;
 instruction.header.opcode=526;instruction.header.length=20;context.instruction=&instruction.header;
 for(float distance:{0.f,12.5f,-12.5f}){instruction.distance=distance;assert(commands.execute(context,526)==0);assert(enemy.minimum_bullet_distance_squared==distance*distance);}
 instruction.header.references=1;instruction.distance=0;const float referenced=7.25f;std::memcpy(context.stack.data(),&referenced,4);
 assert(commands.execute(context,526)==0&&enemy.minimum_bullet_distance_squared==referenced*referenced);
 const float retained=enemy.minimum_bullet_distance_squared;instruction.header.length=16;
 assert(commands.execute(context,526)==-2&&enemy.minimum_bullet_distance_squared==retained);
}
struct Host final:PauseMenuServices,TitleReplaySaveServices {
 bool available=true;int reads=0,writes=0,prepared=0,released=0;
 bool replay_save_available()const override{return available;}
 bool sound(i32)override{return true;}
 bool synchronize_result_score()override{return true;}
 bool capture_score_details(PauseScoreDetails&)override{return true;}
 bool read_replay_slot(i32,std::shared_ptr<Replay>&)override{++reads;return true;}
 bool read_slot(i32 s,std::shared_ptr<Replay>& r)override{return read_replay_slot(s,r);}
 bool save_named_replay(i32,const std::array<char,9>&)override{++writes;return true;}
 bool save_slot(i32 s,const std::array<char,9>& n)override{return save_named_replay(s,n);}
 bool prepare_replay_save(bool)override{++prepared;return true;}
 bool prepare_live_replay(bool v)override{return prepare_replay_save(v);}
 bool open_options(float)override{return true;}
 bool options_finished()const override{return false;}
 bool close_options()override{return true;}
 bool finish_pause_selection()override{return true;}
 bool release_live_replay()override{++released;return true;}
 bool music(const std::string&,i32)override{return true;}
};
struct FinishHost final:CompletionServices,CompletionRecords {
 bool acb=false;int sync=0,practice=0,replay=0,ending=0,finished=0,clears=0,spells=0,removed=0,next=0,selected=0;
 std::string order;
 bool all_clear_bonus()const override{return acb;}
 bool synchronize_clear_score()override{++sync;order+='s';return true;}
 bool clear_notice()override{return true;}
 bool finish_player_options()override{return true;}
 bool end_bomb()override{return true;}
 bool prepare_ending()override{++ending;order+='e';return true;}
 bool finish_replay()override{++replay;return true;}
 bool finish_practice()override{++practice;order+='p';return true;}
 bool queue_next_scene()override{++next;return true;}
 bool select_stage_resources(i32 stage)override{selected=stage;return true;}
 bool stage_clear(i32,i32,i32)override{++clears;order+='c';return true;}
 bool finished_run(i32,bool,i32,bool)override{++finished;return true;}
 bool spell_score(i32,bool,i32,i32)override{++spells;return true;}
 bool remove_checkpoint(i32,i32)override{++removed;return true;}
};
void completion_tests(){
 for(bool acb:{false,true})for(bool playback:{false,true})for(bool pointdevice:{false,true})for(u32 mode:{0u,1u,2u,3u})for(i32 stage:{1,6,7}){
  SessionState p;p.stage=stage;p.difficulty=2;p.transition=playback;
  PlayerLifeSession life;life.mode_flags=(mode<<4)|(pointdevice?0x300:0);life.extra_lives=3;life.bombs=2;
  ItemScoreState score;score.score=123;u32 flags=0;i32 bonus=0,ending_frames=99;FinishHost h;h.acb=acb;
  SessionCompletion finish(p,life,score,flags,bonus,ending_frames,h,h);assert(finish.complete());
  const bool early=mode&&(playback||!acb);
  const bool normal=!early;
  const bool final=normal&&stage>=6;
  const bool hook=normal&&acb&&stage!=7;
  const bool hook_practice=hook&&(mode&1);
  assert(h.sync==int(hook));
  assert(h.practice==int((mode&&!playback&&!acb)||hook_practice));
  assert(h.ending==int(final));
  assert(h.replay==int(playback&&(mode||stage>=6)));
  assert(h.spells==int(mode==2&&!playback));
  assert(h.removed==int(normal&&stage==6&&pointdevice));
  assert(h.finished==int(final&&!playback&&!hook_practice));
  assert(h.next==int(normal&&stage==1&&!hook_practice));
  const i32 expected_bonus=final?(stage==7?128000000:pointdevice?6000000:36000000):0;
  assert(bonus==expected_bonus&&score.score==123+expected_bonus/10);
  if(hook_practice&&stage==6)assert(h.order=="cesp"); // prelude, ending, sync, practice return
  if(hook_practice&&stage==1)assert(h.order=="csp");
  if(normal&&mode&&stage==7)assert(h.sync==0&&h.practice==0); // No invented Extra hook.
 }
}
int main(){
 emission_distance_tests();
 completion_tests();
 Rng rng;AnmEnvironment env;AnmManager animations(rng,env);
 SessionState progress;PlayerLifeSession player;ItemScoreState score;RecordStore records;Host host;
 PauseState state;state.screen=PauseScreen::Pause;state.phase=6;state.menu.count=5;state.menu.wrapping=true;state.menu.select(2);
 PauseMenu pause(state,progress,player,score,records,animations,host);
 auto blocked=[&](){for(i32 n=0;n<state.menu.disabled_count;n++)if(state.menu.disabled[n]==2)return true;return false;};
 // Ordinary/unassisted Practice retains the existing save confirmation.
 assert(pause.update(1,0)&&state.phase==9&&host.writes==0);
 state.phase=6;state.menu.select(2);host.available=false;
 assert(pause.update(0,0)&&blocked()&&state.menu.cursor!=2);
 const auto disabled=state.menu.disabled_count;
 for(int n=0;n<100;n++)assert(pause.update(0,0));
 assert(state.menu.disabled_count==disabled&&host.writes==0);
 // Assistance enabled inside slot/name entry unwinds exactly one menu frame.
 for(i32 phase:{11,12}){
  state.menu.disabled_count=0;state.menu.count=5;state.menu.select(2);state.menu.push();state.menu.count=25;state.menu.select(7);
  state.phase=phase;state.flags=phase==11?1:2;
  assert(pause.update(1,0)&&state.phase==6&&state.menu.depth==0&&state.menu.count==5&&!(state.flags&3));
  assert(blocked()&&host.writes==0&&host.prepared==0);
 }
 state.phase=10;state.flags=0;assert(pause.update(1,0)&&state.phase==6&&state.menu.depth==0);
 TitleState title;TitleNameEntry editor;TitleReplaySave save(title,progress,records,animations,editor,host);
 title.screen=TitleScreen::ReplaySave;title.menu.count=5;title.menu.push();
 assert(save.update(1,0)&&title.substate==4&&host.reads==0&&host.writes==0);
 title.age.set(6);assert(save.update(0,0)&&title.screen==TitleScreen::Main&&title.menu.depth==0&&host.released==1);
 // A live toggle during completed-run name entry must not write, either.
 title.screen=TitleScreen::ReplaySave;title.substate=3;editor.names.select(90);
 assert(save.update(1,0)&&title.substate==4&&host.writes==0);
 std::cout<<"PASS real pause/title assisted replay availability, bounded cursor, history unwind and cleanup (no GPU/device parity)\n";
 std::cout<<"PASS 96 real completion cases: all-clear ON/OFF, mode bits, replay early exit, Legacy/Pointdevice, stage 1/6/Extra and native order\n";
}
