#include "native-world.hpp"
#include "../../web/cpp/game/MultiplayerChapterReward.hpp"
int main(int argc,char** argv){
 require(argc==2,"Retail archive required");RetailAssets files(argv[1]);
 for(unsigned count:{2u,3u}){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;
  require(multiplayer_chapter_threshold(count)==(count==2?1250000:1500000),"Wrong default chapter threshold");
  ChapterReward reward(w.animations,b.score,b.session,b.enemy_world,*b.collection,w.assets.front);
  b.enemy_world.chapter_total=b.enemy_world.chapter_defeated=10;
  for(i32 threshold=500000;threshold<=5000000;threshold+=250000)for(i32 graze:{threshold/5000-1,threshold/5000}){
   b.mp_options.chapter_threshold=threshold;require(b.mp_options.valid(),"Valid custom threshold rejected");
   b.mp_fragments=0;for(unsigned s=0;s<count;++s){b.pilots[s]->session.life_pieces=0;b.pilots[s]->session.extra_lives=2;}
   b.score.graze_chapter=graze;const auto before=b.score.score;
   const auto effective=multiplayer_chapter_threshold(count,b.mp_options.chapter_threshold);
   require(reward.complete(false,0,false,effective),reward.error);
   require(reward.state.bonus==graze*5000&&b.score.score==before+graze*500,"Threshold changed native chapter scoring");
   if(reward.state.bonus>=effective){ItemState fragment;fragment.kind=4;require(b.mp_reward_item(fragment),b.error);}
   require(b.mp_fragments==(graze==threshold/5000?1:0),"Chapter fragment threshold boundary incorrect");
  }
  for(i32 value:{-250000,250000,500001,5250000}){b.mp_options.chapter_threshold=value;require(!b.mp_options.valid(),"Invalid native chapter threshold accepted");}
  // Exercise the production shared-reward dispatcher with a different stock
  // for every recipient, then compare repeated events with native collection.
  b.score.difficulty=1;b.score.life_piece_tier=0;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s];p.session.extra_lives=s==0?8:s==1?7:-1;p.session.life_pieces=2;p.session.bombs=2;p.session.bomb_pieces=s==0?4:0;p.owned_life_piece_tier=0;p.ghost=s==2;}
  ItemState fragment;fragment.kind=4;require(b.pilots[1]->collection->award(fragment),b.error);
  require(b.session.extra_lives==8&&b.session.life_pieces==0&&b.session.bombs==3&&b.session.bomb_pieces==0,"Full-life chapter recipient did not convert through the native Bomb rule");
  require(b.pilots[1]->session.extra_lives==8&&b.pilots[1]->session.life_pieces==0&&b.pilots[1]->session.bomb_pieces==0,"A teammate reaching the cap received the wrong reward");
  if(count==3)require(b.pilots[2]->ghost&&b.pilots[2]->session.extra_lives==0,"Ghost reserve did not bank its native Extend");
  require(b.mp_reward_item(fragment)&&b.session.bomb_pieces==1&&b.pilots[1]->session.bomb_pieces==1,"Capped recipients kept receiving life fragments");
  b.session.extra_lives=7;require(b.mp_reward_item(fragment)&&b.session.life_pieces==1&&b.session.extra_lives==7,"A recipient below the cap inherited a teammate's fragment progress");
  b.session.bombs=8;b.session.bomb_pieces=4;b.session.extra_lives=8;
  require(b.mp_reward_item(fragment)&&b.session.bombs==8&&b.session.bomb_pieces==0,"Both-full recipient exceeded the Bomb cap");
  b.score.difficulty=4;b.score.life_piece_tier=0;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s];p.session.extra_lives=2;p.session.life_pieces=s==2?0:4;p.owned_life_piece_tier=s==1?4:s==2?5:0;}
  require(b.mp_reward_item(fragment)&&b.session.extra_lives==3&&b.score.life_piece_tier==1,"Shared Extra fragment missed the host's native Extend");
  require(b.pilots[1]->session.extra_lives==3&&b.pilots[1]->owned_life_piece_tier==5,"Shared Extra fragment used another pilot's Extend tier");
  if(count==3)require(b.pilots[2]->session.extra_lives==2&&b.pilots[2]->session.life_pieces==1&&b.pilots[2]->owned_life_piece_tier==5,"Shared Extra fragment granted a sixth Extend");
  // The admitted historical rule-20 Replay must retain its recorded economy.
  b.mp_options.replay_legacy_stage_flow=true;b.score.difficulty=1;b.score.life_piece_tier=0;b.mp_fragments=2;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s];p.session.extra_lives=s?2:8;p.session.life_pieces=2;p.session.bombs=2;p.session.bomb_pieces=0;}
  require(b.pilots[1]->collection->award(fragment),b.error);
  require(b.mp_fragments==0&&b.session.extra_lives==8&&b.session.bomb_pieces==0&&b.pilots[1]->session.extra_lives==3,"Historical Replay economy changed under the new reward rule");
 }
 require(multiplayer_chapter_threshold(1)==1000000,"Single-player threshold changed");
 std::puts("PASS: all 19 thresholds at actual chapter-reward boundaries, 2P/3P defaults, native scoring and single-player default");
}
