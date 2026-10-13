#include "../../th15_web/cpp/game/GameBattle.hpp"
#include "../../th15_web/cpp/game/StageAssets.hpp"
#include "../../th15_web/cpp/game/Archive.hpp"
#include "../../th15_web/cpp/game/ChapterReward.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <cstring>
using namespace th15;
void require(bool valid,const std::string& reason){if(!valid){std::fprintf(stderr,"FAIL: %s\n",reason.c_str());std::exit(1);}}
struct RetailAssets final:AssetSource {
 std::vector<u8> bytes;Archive archive;
 explicit RetailAssets(const char* path){std::ifstream file(path,std::ios::binary);require(bool(file),"TH15_TEST_ARCHIVE unavailable");bytes.assign(std::istreambuf_iterator<char>(file),{});require(archive.open(bytes.data(),bytes.size()),"Retail archive rejected");}
 bool read(const std::string& name,std::vector<u8>& out)override{for(unsigned i=0;i<archive.entries.size();++i)if(archive.entries[i].name==name)return archive.read(i,out);return false;}
};
// Headless fixture endpoints acknowledge presentation requests only. The test
// runs production ECL, ANM, players, shots, bombs, bullets, lasers and items;
// it does not certify GPU/audio output or retail dialogue rendering.
struct Presentation final:BattleWorldServices {
 unsigned game_overs=0;
 bool audio(i32,float,PlayerShots::SoundAction,bool)override{return true;}
 bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}
 bool game_over()override{++game_overs;return true;}bool notice(i32)override{return true;}
 bool popup(const Vec3&,i32,u32)override{return true;}bool cancellation_effect(i32,const Vec3&,const Vec3&)override{return true;}
 bool graze_spark(const Vec3&)override{return true;}bool graze_flash()override{return true;}bool graze_resonance(float)override{return true;}
 bool shake(const ScreenShakeSpec&)override{return true;}bool nudge(const ScreenNudgeSpec&)override{return true;}
 bool bomb_damage(const DamageQuery&,i32& out)override{out=0;return true;}
 int enemy_callback(EnemyRuntime&,float)override{return 0;}
 bool enemy_additional_damage(EnemyState&,i32,i32& out)override{out=0;return true;}
 bool enemy_contact(EnemyState&,AnmVm*,i32&,bool& handled)override{handled=false;return true;}
 bool enemy_distortion(EnemyState&,float)override{return true;}bool enemy_death_callback(EnemyRuntime&)override{return true;}
 bool background_fog(i32,i32,const StageFog&)override{return true;}bool background_interrupt(i32)override{return true;}
 bool scene_message(i32)override{return true;}bool message_complete(bool& out)override{out=true;return true;}
 bool begin_enemy_distortion(EnemyState&)override{return true;}bool retire_enemy_distortion(EnemyState&)override{return true;}
 bool boss_segment(i32,i32,float,u32)override{return true;}bool boss_segments(i32)override{return true;}bool stage_logo()override{return true;}
 bool spell_background_visible(bool)override{return true;}bool spell_prepare_hud(bool)override{return true;}
 bool spell_title(AnmVm&,const std::string&)override{return true;}bool spell_history_begin(i32,const std::string&)override{return true;}
 bool spell_history_capture(i32)override{return true;}bool spell_result(i32,bool)override{return true;}bool spell_sound(i32)override{return true;}
};
struct World {
 Rng visual{417,0,0};AnmEnvironment environment;AnmManager animations{visual,environment};StageAssets assets;Presentation output;SessionState progress;std::unique_ptr<GameBattle> battle;
 World(RetailAssets& files,unsigned count,unsigned local,std::array<int,3> selection):assets(files,animations){
  environment.game_rng={417,0,0};environment.playfield_origin={192,224};environment.screen_offsets={32,16,32,16};
  require(assets.load(1,selection[0])&&assets.load_multiplayer(),assets.error);
  battle=std::make_unique<GameBattle>(animations,environment,environment.game_rng,visual,output,assets.scripts,assets.shots,selection[0],BattleResources{assets.player,assets.bullet,assets.effect});
  auto& b=*battle;progress.character=selection[0];progress.difficulty=1;b.bind_progress(progress);b.session.power=400;b.score.point_value=1000000;b.score.max_point_value=50000000;b.enemy_world.difficulty=1;
  require(b.initialize(),b.error);for(unsigned i=0;i<6;++i)b.enemy_visuals.map_resource(i,assets.enemy_banks[i]);b.spell_visuals=assets.spell_resources(0);
  require(b.mp_enable({count,local,1,417,1,false,selection},assets.multiplayer_shots,assets.multiplayer_players),b.error);
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s];p.player->life.state=1;p.player->life.invulnerability.set(30000);p.player->frame.ready_age.set(30);}
 }
 void step(const std::array<MultiplayerInput,3>& input){battle->mp_input(input);BattleFrame frame;frame.focus_allowed=true;require(battle->step(frame),battle->error);}
 std::array<u32,34> state(){auto& b=*battle;std::array<u32,34> out{};unsigned at=0;out[at++]=environment.game_rng.seed;out[at++]=environment.game_rng.calls;out[at++]=b.enemies->count();out[at++]=b.bullet_scene->manager.visible_count;
  out[at++]=b.score.graze_total;out[at++]=b.score.graze_chapter;out[at++]=b.score.point_value;out[at++]=b.score.point_items;out[at++]=b.score.chapter_value;out[at++]=b.score.chapter_count;
  for(unsigned s=0;s<3;++s){if(!b.pilots[s]){at+=8;continue;}auto& p=*b.pilots[s];out[at++]=u32(p.player->motion.position_fixed.x);out[at++]=u32(p.player->motion.position_fixed.y);out[at++]=p.session.power;out[at++]=p.session.bombs;out[at++]=p.session.extra_lives;out[at++]=p.player->life.state;out[at++]=p.score.score;out[at++]=p.player->frame.input_age.current;}return out;
 }
};
#include "native-items.hpp"
#include "native-hud.hpp"
int main(int argc,char** argv){
 require(argc==2,"Pass a legally supplied Japanese TH15 1.00b archive");RetailAssets files(argv[1]);unsigned ticks=0;
 item_rules(files);hud_rules(files);
 for(const auto selections:{std::array<int,3>{0,1,2},std::array<int,3>{3,3,0}}){
  auto left=std::make_unique<World>(files,3,0,selections),right=std::make_unique<World>(files,3,2,selections);
  auto& b=*left->battle;
  require(b.pilots[0]->player!=b.pilots[1]->player&&b.pilots[1]->player!=b.pilots[2]->player,"Pilot objects alias");
  for(auto* w:{left.get(),right.get()}){EnemySpawnRequest main;main.routine="main";require(w->battle->spawn(main),w->battle->error);}
  for(unsigned frame=0;frame<1800;++frame){std::array<MultiplayerInput,3> in{};for(unsigned s=0;s<3;++s){in[s].held=1|8|(frame%240<120?(s==1?0x40:0x80):(s==1?0x80:0x40));if(frame==180+s*30)in[s].held|=2;}
   left->step(in);right->step(in);require(left->state()==right->state(),"Local-seat-dependent simulation at "+std::to_string(frame));++ticks;
  }

 }
 {
  World grazing(files,3,2,{0,1,3});auto& team=*grazing.battle;
  for(unsigned s=0;s<3;++s){auto& p=*team.pilots[s]->player;p.motion.position={0,400,0};p.motion.position_fixed={0,51200};p.life.invulnerability.set(0);}
  BulletShooter near;near.position={20,400,0};near.count=near.rows=1;near.speed=0;
  require(team.bullet_scene->manager.emit(near,0),team.bullet_scene->manager.error);
  grazing.step({});require(team.score.graze_total==3,"One bullet must credit each grazing pilot once");
  grazing.step({});require(team.score.graze_total==3,"Bullet graze cooldown was lost between team seats");
 }
 for(unsigned count:{2u,3u}){
  World pickups(files,count,0,{0,1,3});auto& team=*pickups.battle;team.session.extra_lives=8;
  team.items->spawn(4,team.player->motion.position,0,0);pickups.step({});
  require(team.session.bomb_pieces==1&&team.session.life_pieces==0,"Full-life pickup did not convert for its owner");
  for(unsigned s=1;s<count;++s)require(team.pilots[s]->session.life_pieces==0&&team.pilots[s]->session.bomb_pieces==0,"Fragment pickup leaked into teammate stock");
  auto& recipient=*team.pilots[1];recipient.session.life_pieces=2;team.items->spawn(4,recipient.player->motion.position,0,0);pickups.step({});
  require(recipient.session.extra_lives==3&&recipient.session.life_pieces==0&&team.session.extra_lives==8&&team.session.bomb_pieces==1,"Pickup Extend must affect only its recipient");
 }
 auto world=std::make_unique<World>(files,3,0,std::array<int,3>{0,1,3});auto& b=*world->battle;
 require(&b.pilots[0]->score==&b.pilots[1]->score&&&b.pilots[2]->score==&b.score,"Team score owners must alias");
 const auto grazes=b.score.graze_total,point_value=b.score.point_value;
 for(unsigned s:{0u,2u}){b.mp_contact_seat=s;require(b.record_graze({0,200,0}),b.error);}
 require(b.score.graze_total==grazes+2&&b.score.graze_chapter==2&&b.score.point_value==point_value+200,"Graze or point value not pooled");
 ItemState picked;picked.kind=2;picked.state=3;const auto points=b.score.point_items,points_score=b.score.score;
 require(b.pilots[0]->collection->award(picked)&&b.pilots[2]->collection->award(picked),"Point pickups failed");
 require(b.score.point_items==points+2&&b.score.score>points_score,"Team point pickups overwritten");
 b.pilots[1]->session.power=100;picked.kind=1;require(b.pilots[1]->collection->award(picked),"Power pickup failed");
 require(b.pilots[1]->session.power==101&&b.session.power==400&&b.pilots[2]->session.power==400,"Power not independent");
 picked.kind=6;for(unsigned n=0;n<5;++n)require(b.pilots[2]->collection->award(picked),"Bomb piece failed");
 require(b.pilots[2]->session.bombs==4&&b.session.bombs==3&&b.pilots[1]->session.bomb_pieces==0,"Bomb fragments not independent");
 std::array<MultiplayerInput,3> in{};in[1].held=0x40;world->step(in);require(b.pilots[1]->player->motion.position.x<0&&b.pilots[0]->player->motion.position.x==-48,"Input leaked across seats");
 b.bomb_enemies.available=true;require(b.pilots[1]->begin_bomb(),"Remote bomb did not begin");require(b.pilots[1]->session.bombs==2&&b.session.bombs==3&&b.pilots[2]->session.bombs==4,"Bomb stock not independent");
 const auto location=b.pilots[2]->player->motion.position;
 require(b.mp_nearest(location)==2,"Nearest target ownership");
 for(unsigned s=0;s<3;++s)b.pilots[s]->player->damage.circle({0,100,0},30,0,30,10*(s+1));
 DamageQuery query;query.position={0,100,0};query.radius=10;query.target=77;DamageResult damage;
 const auto damage_score=b.score.score;require(b.mp_damage(query,damage)&&damage.amount==60,"Damage from all pilots must reach the shared enemy");
 require(b.score.score==damage_score+3,"Remote damage scores lost or counted twice");
 b.items->spawn(5,location,0,0);world->step({});require(b.pilots[2]->session.extra_lives==3&&b.session.extra_lives==2,"Pickup awarded to wrong pilot");
 b.enemy_world.chapter_total=b.enemy_world.chapter_defeated=10;b.pilots[2]->score.graze_chapter=200;
 ChapterReward reward(world->animations,b.pilots[2]->score,b.pilots[2]->session,b.enemy_world,*b.pilots[2]->collection,world->assets.front);
 const auto chapter_score=b.score.score;
 picked.kind=4;require(reward.award(false,0,false)&&b.mp_reward_item(picked),"Chapter reward failed");
 require(b.pilots[2]->session.life_pieces==1&&b.session.life_pieces==1&&b.pilots[1]->session.life_pieces==1,"Chapter reward must grant one individual fragment per pilot");
 require(b.score.score==chapter_score+100000&&reward.state.grazes==200,"Shared chapter reward multiplied by seat count");
 require(b.pilots[0]->collection->award(picked)&&b.pilots[1]->collection->award(picked),"Life fragments failed");
 require(b.session.life_pieces==2&&b.pilots[1]->session.life_pieces==2&&b.pilots[2]->session.life_pieces==1,"Life fragments leaked to another pilot");
 require(b.pilots[1]->collection->award(picked),"Individual Extend failed");
 require(b.session.extra_lives==2&&b.pilots[1]->session.extra_lives==3&&b.pilots[2]->session.extra_lives==3&&b.pilots[1]->owned_life_piece_tier==1&&b.score.life_piece_tier==0,"Extend or tier leaked between pilots");
 b.session.extra_lives=8;require(b.pilots[0]->collection->award(picked),"Capped recipient fragment failed");
 require(b.session.life_pieces==0&&b.session.bomb_pieces==1&&b.pilots[1]->session.bomb_pieces==0&&b.pilots[2]->session.life_pieces==1,"Capped recipient must convert its fragment regardless of teammates' lives");
 const auto second_chapter_score=b.score.score;require(reward.award(false,0,false)&&b.mp_reward_item(picked),"Mixed-stock chapter reward failed");
 require(b.score.score==second_chapter_score+100000&&b.session.bomb_pieces==2&&b.pilots[1]->session.life_pieces==1&&b.pilots[2]->session.life_pieces==2,"Chapter resource conversion must use each recipient's stock");
 b.session.bombs=8;b.session.bomb_pieces=4;require(b.pilots[0]->collection->award(picked)&&b.session.bombs==8&&b.session.bomb_pieces==0&&b.pilots[1]->session.bomb_pieces==0,"Both-full conversion must respect the recipient Bomb cap");
 b.score.difficulty=4;b.score.life_piece_tier=0;b.pilots[1]->owned_life_piece_tier=4;b.pilots[2]->owned_life_piece_tier=0;
 b.pilots[1]->session.life_pieces=0;b.pilots[2]->session.life_pieces=0;
 const auto extra_lives=b.pilots[1]->session.extra_lives;
 for(unsigned n=0;n<4;++n)require(b.pilots[1]->collection->award(picked),"Extra fragment failed");
 require(b.pilots[1]->session.extra_lives==extra_lives&&b.pilots[1]->session.life_pieces==4&&b.pilots[2]->session.life_pieces==0,"Extra threshold or fragments not independent");
 require(b.pilots[1]->collection->award(picked)&&b.pilots[1]->session.extra_lives==extra_lives+1&&b.pilots[1]->owned_life_piece_tier==5,"Fifth Extra Extend missing");
 for(unsigned n=0;n<5;++n)require(b.pilots[2]->collection->award(picked),"Teammate Extra fragment failed");
 require(b.pilots[2]->session.extra_lives==4&&b.pilots[2]->owned_life_piece_tier==1,"One pilot's Extra tier blocked a teammate's Extend");
 require(b.pilots[1]->collection->award(picked)&&b.pilots[1]->session.extra_lives==extra_lives+1&&b.pilots[1]->session.life_pieces==1,"Extra sixth Extend incorrectly granted");
 b.score.difficulty=1;
 BulletShooter aimed;aimed.position={location.x,location.y-100,0};aimed.count=aimed.rows=1;aimed.speed=1;
 require(b.bullet_scene->manager.emit(aimed,0),b.bullet_scene->manager.error);
 require(b.bullet_scene->manager.context.player.x==location.x&&b.bullet_scene->manager.context.player.y==location.y,"ECL emission bypassed nearest-pilot aim");
 b.pilots[2]->player->life.invulnerability.set(0);const auto& collision=b.mp_collision(2);require(collision.circle({location.x,location.y},8,false,b.pilots[2]->player)==PlayerContact::hit,"Remote pilot collision missing");
 require(b.pilots[2]->player->life.state==4&&b.player->life.state==1,"Remote collision hit host pilot");
 const auto host_lives=b.session.extra_lives,remote_lives=b.pilots[2]->session.extra_lives;
 require(b.pilots[2]->player->life.commit_death(),b.pilots[2]->player->life.error);
 require(b.session.extra_lives==host_lives&&b.pilots[2]->session.extra_lives==remote_lives-1,"Miss consumed a teammate life");
 require(b.pilots[0]->game_over()&&world->output.game_overs==0,"Host death ended live teammates");
 const auto ghost_pieces=b.session.life_pieces,ghost_bomb_pieces=b.session.bomb_pieces;
 require(b.mp_reward_item(picked)&&b.pilots[0]->ghost&&b.session.life_pieces==ghost_pieces&&b.session.bomb_pieces==ghost_bomb_pieces,"Chapter reward must skip ghosts");
 const auto exhausted_pieces=b.pilots[1]->session.life_pieces,retained_lives=b.pilots[1]->session.extra_lives;b.pilots[1]->session.extra_lives=-1;
 require(b.mp_reward_item(picked)&&b.pilots[1]->session.life_pieces==exhausted_pieces&&b.pilots[1]->session.extra_lives==-1,"Chapter reward revived an exhausted pilot before ghost transition");
 b.pilots[1]->session.extra_lives=retained_lives;
 require(b.pilots[1]->game_over()&&world->output.game_overs==0,"Second death ended live teammate");
 require(b.pilots[2]->game_over()&&world->output.game_overs==1,"All-ghost completion missing");
 {
  World transition(files,3,1,{0,1,3});auto& run=*transition.battle;auto* retained=run.pilots[2]->player;
  run.pilots[2]->session.power=137;run.pilots[2]->session.extra_lives=4;run.pilots[2]->session.bombs=1;run.pilots[2]->session.life_pieces=2;run.pilots[2]->session.bomb_pieces=3;run.pilots[2]->owned_life_piece_tier=4;
  StageAssets next(files,transition.animations,&transition.assets);require(next.load(2,0),next.error);transition.progress.stage=2;
  require(run.replace_stage(next.scripts,next.enemy_banks,next.spell_resources(0)),run.error);
  require(run.pilots[2]->player==retained&&run.pilots[2]->session.power==137&&run.pilots[2]->session.extra_lives==4&&run.pilots[2]->session.bombs==1,"Next stage discarded remote resources");
  require(run.pilots[2]->session.life_pieces==2&&run.pilots[2]->session.bomb_pieces==3&&run.pilots[2]->owned_life_piece_tier==4,"Next stage discarded individual fragments or Extend progress");
  for(unsigned f=0;f<30;++f)transition.step({});
 }
 std::printf("{\"passed\":true,\"ticks\":%u,\"characters\":4,\"localSeats\":[0,2],\"checks\":[\"independent-input-bombs-lives-power-fragments\",\"shared-score-graze-point-value\",\"shared-damage-score\",\"eligible-underpowered-pickup-retail-P-rewards\",\"individual-HUD-shared-statistics-no-battle-RNG\",\"remote-collision\",\"single-team-chapter-score-individual-resources\",\"individual-extend-tiers-caps-extra-threshold\",\"nearest-ECL-aim\",\"next-stage-resources\",\"all-player-game-over\",\"retail-ECL-determinism\"]}\n",ticks);
}
