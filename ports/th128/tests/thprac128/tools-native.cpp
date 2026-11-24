#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
using namespace ecl_fixture;static unsigned checks=0,starts=0;
extern "C" unsigned th128_host_capsule_next_serial(){static unsigned serial=0;return ++serial;}
void check(bool ok,const std::string&why){require(ok,why);++checks;}
Bytes read(const std::string&p){std::ifstream f(p,std::ios::binary);require(f.good(),"missing owned resource");return Bytes(std::istreambuf_iterator<char>(f),{});}
void fresh(const std::string&home){th128_world_reset();std::ifstream list(home+"/files.txt");std::string name;while(std::getline(list,name)){auto b=read(home+"/"+name);check(th128_world_load(name.data(),unsigned(name.size()),b.data(),unsigned(b.size()))==1,th128_world_error());}}
bool start(const char*name,int rank){++starts;return th128_world_start(name,unsigned(std::strlen(name)),rank,34772)==1;}
void setup(const std::string& home,unsigned section=10302,int charge=10000){
 fresh(home);const unsigned stage=section==5?1:3;const std::array<std::int32_t,11> p{1,int(stage-1),int(section),0,0,1,123456789,54321,charge,6789,800};
 check(th128_world_prepare_thprac128(p.data(),44)==1,th128_world_error());check(start(stage==1?"st_a1_1.ecl":"st_a1_3.ecl",1),th128_world_error());
 const auto profile=read(home+"/bullet-profile-8.bin"),bullet=read(home+"/bullet.anm"),text=read(home+"/text.anm");
 for(const auto[bank,name]:std::vector<std::pair<int,std::string>>{{1,"enemy.anm"},{2,stage==1?"enemy_ll.anm":"enemy_bf.anm"},{3,stage==1?"boss01.anm":"boss03.anm"}}){const auto b=read(home+"/"+name);check(th128_world_load_enemy_anm(bank,b.data(),unsigned(b.size()))==1,th128_world_error());}
 check(th128_world_enable_geometry526(text.data(),unsigned(text.size()))==1,th128_world_error());check(th128_world_enable_bullets(profile.data(),unsigned(profile.size()),bullet.data(),unsigned(bullet.size()),0,400)==1,th128_world_error());
 const auto anm=read(home+"/pl00.anm"),shot=read(home+"/pl00a.sht"),front=read(home+"/front.anm"),font=read(home+"/ascii.anm"),card=read(home+"/card.anm"),boss=read(home+(stage==1?"/boss01.anm":"/boss03.anm"));
 const std::array<unsigned,th128::shotenemy::DamageSourceCount> flags{};
 check(th128_world_enable_bullet_updates(shot.data(),unsigned(shot.size()))==1,th128_world_error());check(th128_world_enable_graze(0,6789,0)==1,th128_world_error());check(th128_world_enable_player_context(anm.data(),unsigned(anm.size()),1,0,0,0,0,0,0,0,charge)==1,th128_world_error());
 check(th128_world_enable_player_motion(0,51200)==1,th128_world_error());check(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),800,1,0,0,0,0)==1,th128_world_error());check(th128_world_enable_shot_hits(54321,0,0,0,0)==1,th128_world_error());
 check(th128_world_enable_rapid_shot(-1)==1&&th128_world_enable_player_death(-1)==1,th128_world_error());check(th128_world_enable_perfect_hud(front.data(),unsigned(front.size()))==1,th128_world_error());
 check(th128_world_load_score_font(font.data(),unsigned(font.size()))==1,th128_world_error());check(th128_world_enable_spells(bullet.data(),unsigned(bullet.size()),boss.data(),unsigned(boss.size()),text.data(),unsigned(text.size()),font.data(),unsigned(font.size()),card.data(),unsigned(card.size()),anm.data(),unsigned(anm.size()))==1,th128_world_error());
 const std::array<unsigned,8> targets{};check(th128_world_enable_player_respawn(1,0,targets.data(),8)==1,th128_world_error());
 const auto ice=read(home+"/ice.anm"),masks=read(home+"/ice-masks.bin");check(th128_world_enable_z_fire(ice.data(),unsigned(ice.size()),masks.data(),unsigned(masks.size()))==1,th128_world_error());check(perfectConfigured&&playerFireConfigured&&debugRunTainted&&!normalRecord&&!normalReplay,"whole actual hosts and nonNormal identity");
}
void tick(){check(th128_world_tick()==1,th128_world_error());}
void mask(unsigned value){check(th128_world_thprac_tools_set(value,worldGeneration)==1,th128_world_error());}
void hit(){const auto p=world->playerAimPosition();const auto result=playerHit->confirmedOverlap({p.x,p.y,p.z},playerHitControl,operationError);check(result&&*result==1,operationError);}
void release(){check(playerFire->heldTimer({44,45,45,0x4b6f58,1},operationError),operationError);check(playerHit->setHeldTimer(playerFire->state().held,operationError),operationError);tick();}
int main(int argc,char**argv){try{check(argc==3,"owned resources and outputdir");const std::string home=argv[1];fresh(home);check(start("st_a1_1.ecl",1),th128_world_error());check(th128_world_thprac_tools_set(1,worldGeneration)==0,"ordinary caller rejected");
 setup(home);mask(1);const auto effects=cancellationEffects->snapshots().size();hit();check(playerHit->snapshot().gate.stateF78==1&&cancellationEffects->snapshots().size()==effects,"F1 actual harmful hit entry suppressed without effect allocation");
 const auto before=std::string(th128_world_thprac_tools_state());check(th128_world_thprac_tools_set(128,worldGeneration)==0&&std::string(th128_world_thprac_tools_state())==before,"bad mask atomic");check(th128_world_thprac_tools_set(0,worldGeneration+1)==0,"foreign generation rejected");worldTickExecuting=true;check(th128_world_thprac_tools_set(0,worldGeneration)==0,"reentrant tick rejected");worldTickExecuting=false;mask(2);hit();check(playerHit->snapshot().gate.stateF78==4,"turning F1 off restores actual hit window");for(unsigned i=0;i<9;++i)tick();check(deathsCommitted==1&&motivationRaw==54321&&playerWindowControl.perfectCharge4b4d70==12500,"F2 actual death preserves motivation and retains charge award");
 setup(home);hit();for(unsigned i=0;i<9;++i)tick();check(deathsCommitted==1&&motivationRaw==44321,"ordinary death policy restored for practice with tools off");
 setup(home);mask(4);check(th128_world_input(2,2,0)==1,th128_world_error());tick();check(perfectHost->status().active&&perfectHost->status().charge==10000&&playerWindowControl.perfectCharge4b4d70==10000,"F3 actual normal Perfect activates and spends zero");
 setup(home);mask(8);release();check(grazeGlobals.powerRaw==6789&&iceHost->statistics().busy,"F4 actual freeze keeps power and real ice batch");
 setup(home);release();check(grazeGlobals.powerRaw==0&&iceHost->statistics().busy,"F4 off consumes actual power");
 setup(home);mask(32);hit();tick();check(playerHit->snapshot().gate.stateF78==1&&perfectHost->status().active&&perfectHost->status().charge==0&&playerWindowControl.mode3c==1,"F6 actual hit-window Perfect pipeline auto activates");
 setup(home,10302,0);mask(32);hit();tick();check(playerHit->snapshot().gate.stateF78==4&&!perfectHost->status().active,"F6 preserves actual insufficient charge gate");
 setup(home,5);mask(16);for(unsigned i=0;i<3;++i)tick();const auto entity=world->bossEntity(0);check(entity.has_value()&&entity->phaseTimer.current==0,"F5 actual first-card enemy phase timer held while World advances");mask(0);tick();check(world->bossEntity(0)->phaseTimer.current==1,"F5 off resumes timer");
 setup(home);const std::array<unsigned,4> schedule{0,1,1,0};check(th128_world_thprac_tools_playback(schedule.data(),16,worldGeneration)==1,th128_world_error());
 const std::array<unsigned,2> bad{0,128};check(th128_world_thprac_tools_playback(bad.data(),8,worldGeneration)==0,"repeat playback install rejected");
 check(th128_world_thprac_replay_begin(10)==1,th128_world_error());check(th128_world_thprac_replay_clock(10+1./60,60)==1,th128_world_error());tick();check(thpracTools.mask==1&&playerHit->snapshot().gate.stateF78==1,"actual tick consumes scheduled mask");
 check(th128_world_thprac_replay_clock(10+2./60,60)==1,th128_world_error());tick();check(thpracTools.mask==0,"actual next tick consumes disable event");
 const std::array<std::uint8_t,8> label{'T','O','O','L','S',' ',' ',' '};const auto n=th128_world_thprac_replay_snapshot(1,0,label.data(),8);check(n>292,"actual recorder history with tools");const auto* b=static_cast<const std::uint8_t*>(th128_world_thprac_replay_data());Bytes raw(b,b+n);std::ofstream out(home+"/actual-tools-native.rpyraw",std::ios::binary);out.write(reinterpret_cast<const char*>(raw.data()),raw.size());out.close();
 setup(home);check(th128_world_thprac_tools_playback(schedule.data(),16,worldGeneration)==1,th128_world_error());check(th128_world_thprac_replay_playback_load(raw.data()+148,unsigned(raw.size()-149))==1,th128_world_error());check(th128_world_thprac_tools_set(1,worldGeneration)==0,"actual playLock rejects tool mutation");tick();check(thpracTools.mask==1,"actual native playback enables scheduled tool");tick();check(thpracTools.mask==0,"actual playback disables tool at recorded frame");check(th128_world_replay_playback_cancel()==1&&!thpracTools.playback,"cancel releases tools playback");
 const auto generation=worldGeneration;fresh(home);check(thpracTools.mask==0&&thpracTools.events.empty()&&th128_world_thprac_tools_set(1,generation)==0,"reset isolates mask history and stale generation");
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldExportsStartCalls\":"<<starts<<",\"wholeWorldExportsCpp\":true,\"Factories\":0,\"browser\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<" native="<<th128_world_error()<<"\n";return 1;}}
