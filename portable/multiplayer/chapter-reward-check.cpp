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
   b.mp_fragments=0;for(unsigned s=0;s<count;++s)b.pilots[s]->session.life_pieces=0;
   b.score.graze_chapter=graze;const auto before=b.score.score;
   const auto effective=multiplayer_chapter_threshold(count,b.mp_options.chapter_threshold);
   require(reward.complete(false,0,false,effective),reward.error);
   require(reward.state.bonus==graze*5000&&b.score.score==before+graze*500,"Threshold changed native chapter scoring");
   if(reward.state.bonus>=effective){ItemState fragment;fragment.kind=4;require(b.mp_reward_item(fragment),b.error);}
   require(b.mp_fragments==(graze==threshold/5000?1:0),"Chapter fragment threshold boundary incorrect");
  }
  for(i32 value:{-250000,250000,500001,5250000}){b.mp_options.chapter_threshold=value;require(!b.mp_options.valid(),"Invalid native chapter threshold accepted");}
 }
 require(multiplayer_chapter_threshold(1)==1000000,"Single-player threshold changed");
 std::puts("PASS: all 19 thresholds at actual chapter-reward boundaries, 2P/3P defaults, native scoring and single-player default");
}
