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
void setup(const std::string& home,unsigned section=10302,int charge=10000,unsigned carryToken=0){
 const unsigned stage=carryToken?unsigned(stageCarry.nextId):section<=7?1:section<=15?2:3;fresh(home);const std::array<std::int32_t,11> p{1,int(stage-1),int(section),0,0,1,123456789,54321,charge,6789,800};
 if(carryToken)check(th128_world_prepare_thprac_carry(carryToken)==1,th128_world_error());else check(th128_world_prepare_thprac128(p.data(),44)==1,th128_world_error());check(start(kStageEclNames[stage],1),th128_world_error());
 if(carryToken){const std::string name=kStageEclNames[stage];check(th128_world_bind_stage_carry_target(carryToken,name.data(),unsigned(name.size()),1)==1,th128_world_error());check(th128_world_restore_stage_carry_random(carryToken,int(stage),stageCarry.nextProgress,1)==1,th128_world_error());}
 th128::ecl::Program program;std::string parseError;const auto raw=read(home+"/"+kStageEclNames[stage]);check(program.load(raw.data(),raw.size(),parseError),parseError);const auto& banks=program.resource().animationIncludes;check(banks.size()==3,"actualroot ANIMbanks");
 const auto profile=read(home+"/bullet-profile-8.bin"),bullet=read(home+"/bullet.anm"),text=read(home+"/text.anm");
 for(unsigned index=0;index<banks.size();++index){const auto b=read(home+"/"+banks[index]);check(th128_world_load_enemy_anm(int(index+1),b.data(),unsigned(b.size()))==1,th128_world_error());}
 check(th128_world_enable_geometry526(text.data(),unsigned(text.size()))==1,th128_world_error());check(th128_world_enable_bullets(profile.data(),unsigned(profile.size()),bullet.data(),unsigned(bullet.size()),0,400)==1,th128_world_error());
 const auto anm=read(home+"/pl00.anm"),shot=read(home+"/pl00a.sht"),front=read(home+"/front.anm"),font=read(home+"/ascii.anm"),card=read(home+"/card.anm"),boss=read(home+"/"+banks[2]);
 const std::array<unsigned,th128::shotenemy::DamageSourceCount> flags{};
 check(th128_world_enable_bullet_updates(shot.data(),unsigned(shot.size()))==1,th128_world_error());check(th128_world_enable_graze(0,6789,0)==1,th128_world_error());check(th128_world_enable_player_context(anm.data(),unsigned(anm.size()),1,0,0,0,0,0,0,0,charge)==1,th128_world_error());
 check(th128_world_enable_player_motion(0,51200)==1,th128_world_error());check(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),800,1,0,0,0,0)==1,th128_world_error());check(th128_world_enable_shot_hits(54321,0,0,0,0)==1,th128_world_error());
 check(th128_world_enable_rapid_shot(-1)==1&&th128_world_enable_player_death(-1)==1,th128_world_error());check(th128_world_enable_perfect_hud(front.data(),unsigned(front.size()))==1,th128_world_error());
 check(th128_world_load_score_font(font.data(),unsigned(font.size()))==1,th128_world_error());check(th128_world_enable_spells(bullet.data(),unsigned(bullet.size()),boss.data(),unsigned(boss.size()),text.data(),unsigned(text.size()),font.data(),unsigned(font.size()),card.data(),unsigned(card.size()),anm.data(),unsigned(anm.size()))==1,th128_world_error());
 const std::array<unsigned,8> targets{};check(th128_world_enable_player_respawn(1,0,targets.data(),8)==1,th128_world_error());
 const auto ice=read(home+"/ice.anm"),masks=read(home+"/ice-masks.bin");check(th128_world_enable_z_fire(ice.data(),unsigned(ice.size()),masks.data(),unsigned(masks.size()))==1,th128_world_error());check(perfectConfigured&&playerFireConfigured&&debugRunTainted&&!normalRecord&&!normalReplay,"whole actual hosts and nonNormal identity");std::ifstream profiles(home+"/profiles.txt");int type=0;while(profiles>>type){const auto b=read(home+"/profile-"+std::to_string(type)+".bin");check(th128_world_add_bullet_profile(type,b.data(),unsigned(b.size()))==1,th128_world_error());}
}
int main(int argc,char**argv){try{check(argc==3,"original assets and trace output");const std::string home=argv[1];setup(home,7,30000);
 const auto msg=read(home+"/st_a1.msg"),text=read(home+"/text.anm"),player=read(home+"/pl00.anm"),boss=read(home+"/boss01.anm"),front=read(home+"/front.anm");
 check(th128_world_enable_messages(msg.data(),unsigned(msg.size()),text.data(),unsigned(text.size()),player.data(),unsigned(player.size()),boss.data(),unsigned(boss.size()))==1,th128_world_error());
 check(th128_world_enable_message_front(front.data(),unsigned(front.size()))==1,th128_world_error());check(th128_world_enable_route_choice(front.data(),unsigned(front.size()),1,0,0)==1,th128_world_error());
 check(th128_world_enable_stage_transition(front.data(),unsigned(front.size()),1,0,0,0,1,0)==1,th128_world_error());check(th128_world_thprac_tools_set(1,worldGeneration)==1,th128_world_error());
 unsigned previous=0,ticks=0;std::ofstream trace(argv[2]);
 while(!stageTransition->state().requested){check(ticks<10000,"actual natural lastcard→routechoice→MSG21 bounded10000");unsigned held=messages->state().active?(ticks%2?1u:0u):512u;
 if(routeChoice->status().phase==th128::routechoice::Phase::Select){held=routeChoice->status().message74!=2?32:1;}
 else if(const auto enemy=world->bossEntity(0)){const auto x=world->playerAimPosition().x;if(enemy->x>x+4.5f)held|=128;else if(enemy->x<x-4.5f)held|=64;}
 check(th128_world_input(held,held&~previous,previous&~held)==1,th128_world_error());previous=held;const auto tick=th128_world_tick();++ticks;
 if(!tick||ticks%100==0||stageTransition->state().requested)trace<<"frame="<<world->frame()<<" phase="<<int(routeChoice->status().phase)<<" message="<<messages->state().entry<<":"<<messages->state().clock<<" error="<<th128_world_error()<<"\n";
 check(tick==1,"actual firstlastcard tick="+std::to_string(ticks)+" "+th128_world_error());
 const std::string draws=th128_world_bullet_draws();check(presentationError.empty(),presentationError);
 }
 const auto t=stageTransition->state();check(t.nextId>=2&&t.nextId<=4&&t.controlActionRecreate,"original confirmed route planned actual next stage");
 const auto token=th128_world_capture_stage_carry();check(token>0,th128_world_error());check(!normalRecord&&!normalReplay&&debugRunTainted,"practice never acquired ordinary score or replay owner");
#if __has_include("practice/Thprac128Continuation.hpp")
 // This branch is compiled only once the actual product bridge exists.
 const auto saved=stageCarry;setup(home,0,30000,token);
 const auto nextMessage=read(home+"/"+stageRecordForId(saved.nextId)->message),nextBoss=read(home+"/boss01.anm");
 check(th128_world_enable_messages(nextMessage.data(),unsigned(nextMessage.size()),text.data(),unsigned(text.size()),player.data(),unsigned(player.size()),nextBoss.data(),unsigned(nextBoss.size()))==1,th128_world_error());
 check(th128_world_enable_message_front(front.data(),unsigned(front.size()))==1,th128_world_error());
 check(th128_world_enable_stage_transition(front.data(),unsigned(front.size()),saved.nextId,saved.nextProgress,0,0,1,0)==1,th128_world_error());
 check(th128_world_restore_stage_carry(token)==1,th128_world_error());check(th128_world_acknowledge_stage_transition(token)==1,th128_world_error());
 check(world->thpracChapter()&&world->thpracChapter()->stageId==unsigned(saved.nextId)&&debugRunTainted&&!normalRecord&&!normalReplay,"actualnextWorldprivatepracticeidentity");
 check(scoreRaw==saved.scoreRaw&&motivationRaw==saved.motivationRaw&&grazeGlobals.powerRaw==saved.powerRaw&&grazeGlobals.grazeCount==saved.grazeCount&&cumulativeFrozenArea==saved.cumulativeFrozenArea&&playerWindowControl.perfectCharge4b4d70==saved.perfectCharge,"exactrealcarryresourcesretained");
 check(th128_world_records_restore(0,0)==0,"practicecarrycannotbecomeordinaryrecord");
 check(th128_world_prepare_thprac_carry(token)==0,"consumedcarrycannotprepareagain");for(unsigned step=0;step<3;++step){check(th128_world_input(0,0,0)==1,th128_world_error());const auto tick=th128_world_tick();check(tick==1,th128_world_error());++ticks;}
 trace<<"actualtargetstage="<<saved.nextId<<" targetframe="<<world->frame()<<" preservedtaint="<<debugRunTainted<<"\n";
#else
 check(false,"actual practice carry consumer missing after original lastspell/routechoice/MSG21 token");
#endif
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldStarts\":"<<starts<<",\"actualTicks\":"<<ticks<<"}\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
