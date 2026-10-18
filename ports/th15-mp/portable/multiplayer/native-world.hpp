#pragma once
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
 unsigned game_overs=0;bool dialogue=false;
 bool dialogue_present()const noexcept override{return dialogue;}
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
 World(RetailAssets& files,unsigned count,unsigned local,std::array<int,3> selection,bool challenge=false):assets(files,animations){
  environment.game_rng={417,0,0};environment.playfield_origin={192,224};environment.screen_offsets={32,16,32,16};
  require(assets.load(1,selection[0])&&assets.load_multiplayer(),assets.error);
  battle=std::make_unique<GameBattle>(animations,environment,environment.game_rng,visual,output,assets.scripts,assets.shots,selection[0],BattleResources{assets.player,assets.bullet,assets.effect});
  auto& b=*battle;progress.character=selection[0];progress.difficulty=1;b.bind_progress(progress);b.session.power=400;b.score.point_value=1000000;b.score.max_point_value=50000000;b.enemy_world.difficulty=1;
  require(b.initialize(),b.error);for(unsigned i=0;i<6;++i)b.enemy_visuals.map_resource(i,assets.enemy_banks[i]);b.spell_visuals=assets.spell_resources(0);
  require(b.player->life.base_bombs==3,"Retail base Bomb stock changed before co-op setup");
  require(b.mp_enable({count,local,1,417,1,challenge,selection},assets.multiplayer_shots,assets.multiplayer_players),b.error);
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s];p.player->life.state=1;p.player->life.invulnerability.set(30000);p.player->frame.ready_age.set(30);}
 }
 void step(const std::array<MultiplayerInput,3>& input){battle->mp_input(input);BattleFrame frame;frame.focus_allowed=true;require(battle->step(frame),battle->error);}
 std::array<u32,34> state(){auto& b=*battle;std::array<u32,34> out{};unsigned at=0;out[at++]=environment.game_rng.seed;out[at++]=environment.game_rng.calls;out[at++]=b.enemies->count();out[at++]=b.bullet_scene->manager.visible_count;
  out[at++]=b.score.graze_total;out[at++]=b.score.graze_chapter;out[at++]=b.score.point_value;out[at++]=b.score.point_items;out[at++]=b.score.chapter_value;out[at++]=b.score.chapter_count;
  for(unsigned s=0;s<3;++s){if(!b.pilots[s]){at+=8;continue;}auto& p=*b.pilots[s];out[at++]=u32(p.player->motion.position_fixed.x);out[at++]=u32(p.player->motion.position_fixed.y);out[at++]=p.session.power;out[at++]=p.session.bombs;out[at++]=p.session.extra_lives;out[at++]=p.player->life.state;out[at++]=p.score.score;out[at++]=p.player->frame.input_age.current;}return out;
 }
};
