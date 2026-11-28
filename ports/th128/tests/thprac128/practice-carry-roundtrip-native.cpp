#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include "practice/Thprac128SectionData.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
using namespace ecl_fixture;static unsigned checks=0,starts=0;
extern "C" unsigned th128_host_capsule_next_serial(){static unsigned serial=0;return ++serial;}
void check(bool ok,const std::string&why){require(ok,why);++checks;}
Bytes read(const std::string&p){std::ifstream f(p,std::ios::binary);require(f.good(),"missing owned resource");return Bytes(std::istreambuf_iterator<char>(f),{});}
void fresh(const std::string&home){th128_world_reset();std::ifstream list(home+"/files.txt");std::string name;while(std::getline(list,name)){auto b=read(home+"/"+name);check(th128_world_load(name.data(),unsigned(name.size()),b.data(),unsigned(b.size()))==1,th128_world_error());}}
bool start(const char*name,int rank,unsigned seed=34772){++starts;return th128_world_start(name,unsigned(std::strlen(name)),rank,seed)==1;}
void setup(const std::string& home,unsigned section=10302,int charge=10000,unsigned carryToken=0,const std::string& replayInitial=""){
 const unsigned stage=carryToken?unsigned(stageCarry.nextId):section>=10000?(section-10000)/100:th128::practice::section_data::stage[section];fresh(home);const std::array<std::int32_t,11> p{1,int(stage-1),int(section),0,0,1,123456789,54321,charge,6789,800};
 if(carryToken)check(th128_world_prepare_thprac_carry(carryToken)==1,th128_world_error());else check(th128_world_prepare_thprac128(p.data(),44)==1,th128_world_error());const auto replay=replayInitial.empty()?Bytes{}:read(replayInitial);const unsigned seed=replay.empty()?34772u:unsigned(replay[2])|(unsigned(replay[3])<<8);check(start(kStageEclNames[stage],1,seed),th128_world_error());
 if(!replay.empty()){auto bad=replay;bad[0]=99;check(th128_world_prepare_thprac_replay_initial(bad.data(),unsigned(bad.size()))==0,"foreignnativeReplayinitialstage rejected");bad=replay;std::uint32_t invalid=10001;std::memcpy(bad.data()+144,&invalid,4);check(th128_world_prepare_thprac_replay_initial(bad.data(),unsigned(bad.size()))==0,"nativeReplayinitialpowerbound rejected");check(th128_world_prepare_thprac_replay_initial(replay.data(),unsigned(replay.size()))==1,th128_world_error());}
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
void writeFile(const std::string&p,const Bytes&b){check(!std::filesystem::exists(p),"owned output must be new");std::ofstream o(p,std::ios::binary);o.write(reinterpret_cast<const char*>(b.data()),std::streamsize(b.size()));check(o.good(),"owned exact output");}
Bytes projection(){const auto g=world->gameRandomState();const std::array<unsigned,15> words{unsigned(world->frame()),g.seed,g.calls,worldDisplayRandom.seed,worldDisplayRandom.calls,unsigned(playerRaw.horizontal),unsigned(playerRaw.vertical),unsigned(scoreRaw),unsigned(pointValueRaw),unsigned(motivationRaw),unsigned(playerWindowControl.perfectCharge4b4d70),std::bit_cast<unsigned>(cumulativeFrozenArea),unsigned(grazeGlobals.powerRaw),unsigned(grazeGlobals.grazeCount),playerShotGroups->state().group};Bytes b(sizeof(words));std::memcpy(b.data(),words.data(),b.size());return b;}
Bytes currentBoss(const std::string&home){th128::ecl::Program p;std::string e;const auto b=read(home+"/"+kStageEclNames[world->thpracChapter()->stageId]);check(p.load(b.data(),b.size(),e),e);return read(home+"/"+p.resource().animationIncludes.at(2));}
void messagesForCurrent(const std::string&home){const auto msg=read(home+"/"+stageRecordForId(int(world->thpracChapter()->stageId))->message),text=read(home+"/text.anm"),player=read(home+"/pl00.anm"),boss=currentBoss(home),front=read(home+"/front.anm");check(th128_world_enable_messages(msg.data(),unsigned(msg.size()),text.data(),unsigned(text.size()),player.data(),unsigned(player.size()),boss.data(),unsigned(boss.size()))==1,th128_world_error());check(th128_world_enable_message_front(front.data(),unsigned(front.size()))==1,th128_world_error());}
int main(int argc,char**argv){try{check(argc==4,"original assets, roundtrip prefix and scenario");const std::string home=argv[1],prefix=argv[2],scenario=argv[3];
 if(scenario=="replay"){const auto initial=read(prefix+".initial");const unsigned stage=unsigned(initial[0])|(unsigned(initial[1])<<8);setup(home,10000+stage*100+1,30000,0,prefix+".initial");messagesForCurrent(home);const auto front=read(home+"/front.anm");check(th128_world_enable_stage_transition(front.data(),unsigned(front.size()),int(stage),0,0,0,1,0)==1,th128_world_error());check(th128_world_restore_thprac_replay_initial()==1,th128_world_error());check(th128_world_restore_thprac_replay_initial()==0,"initialrestoreconsumedonce");check(th128_world_thprac_tools_set(1,worldGeneration)==1,th128_world_error());const auto raw=read(prefix+".native");const unsigned frames=std::uint32_t(raw[152])|(std::uint32_t(raw[153])<<8)|(std::uint32_t(raw[154])<<16)|(std::uint32_t(raw[155])<<24);Bytes packet(raw.begin()+148,raw.begin()+148+144+frames*6);check(th128_world_thprac_replay_playback_load(packet.data(),unsigned(packet.size()))==1,th128_world_error());check(th128_world_input(16,16,0)==0,"nativeplayLockrejects liveinput");for(unsigned i=0;i<frames;++i)check(th128_world_tick()==1,th128_world_error());check(replayPlayback.consumed()==frames&&projection()==read(prefix+".projection"),"actualcontinuednativeReplay matchesall15projections");std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldStarts\":"<<starts<<",\"actualTicks\":"<<frames<<",\"nativeCarryReplay\":true}\n";return 0;}
 setup(home,scenario=="second"?15:7,30000);const auto sourceStage=world->thpracChapter()->stageId;
 const auto front=read(home+"/front.anm");messagesForCurrent(home);
 if(sourceStage==1)check(th128_world_enable_route_choice(front.data(),unsigned(front.size()),1,0,0)==1,th128_world_error());
 check(th128_world_enable_stage_transition(front.data(),unsigned(front.size()),int(sourceStage),0,0,0,1,0)==1,th128_world_error());check(th128_world_thprac_tools_set(1,worldGeneration)==1,th128_world_error());
 unsigned previous=0,ticks=0;std::ofstream trace(prefix+".record.trace");
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
 const auto saved=stageCarry;setup(home,0,30000,token);messagesForCurrent(home);
 check(th128_world_enable_stage_transition(front.data(),unsigned(front.size()),saved.nextId,saved.nextProgress,0,0,1,0)==1,th128_world_error());
 check(th128_world_restore_stage_carry(token)==1,th128_world_error());check(th128_world_acknowledge_stage_transition(token)==1,th128_world_error());
 check(world->thpracChapter()&&world->thpracChapter()->stageId==unsigned(saved.nextId)&&debugRunTainted&&!normalRecord&&!normalReplay,"actualnextWorldprivatepracticeidentity");
 check(scoreRaw==saved.scoreRaw&&motivationRaw==saved.motivationRaw&&grazeGlobals.powerRaw==saved.powerRaw&&grazeGlobals.grazeCount==saved.grazeCount&&cumulativeFrozenArea==saved.cumulativeFrozenArea&&playerWindowControl.perfectCharge4b4d70==saved.perfectCharge,"exactrealcarryresourcesretained");
 check(th128_world_records_restore(0,0)==0,"practicecarrycannotbecomeordinaryrecord");check(th128_world_prepare_thprac_carry(token)==0,"consumedcarrycannotprepareagain");
 check(th128_world_thprac_tools_set(1,worldGeneration)==1,th128_world_error());check(th128_world_thprac_replay_begin(10)==1,th128_world_error());const auto power=grazeGlobals.powerRaw;
 for(unsigned step=0;step<3;++step){check(th128_world_thprac_replay_clock(10+double(step+1)/60,60)==1,th128_world_error());check(th128_world_input(0,0,0)==1,th128_world_error());check(th128_world_tick()==1,th128_world_error());++ticks;}
 const std::array<unsigned char,8> name{'T','E','S','T',' ',' ',' ',' '};const auto n=th128_world_thprac_replay_snapshot(1281268800,0,name.data(),8);check(n>=292,th128_world_error());const auto* raw=static_cast<const std::uint8_t*>(th128_world_thprac_replay_data());Bytes native(raw,raw+n),initial(160);std::copy(native.begin()+148,native.begin()+292,initial.begin());
 const std::array<unsigned,4> extras{unsigned(power),stageCarry.preInitGameRandom.calls,stageCarry.preInitDisplayRandom.seed,stageCarry.preInitDisplayRandom.calls};std::memcpy(initial.data()+144,extras.data(),16);writeFile(prefix+".native",native);writeFile(prefix+".initial",initial);writeFile(prefix+".projection",projection());
 trace<<"actualtargetstage="<<saved.nextId<<" targetframe="<<world->frame()<<" preservedtaint="<<debugRunTainted<<" nativeReplayframes="<<thpracReplay.record->frames()<<"\n";
#else
 check(false,"actual practice carry consumer missing after original lastspell/routechoice/MSG21 token");
#endif
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldStarts\":"<<starts<<",\"actualTicks\":"<<ticks<<"}\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
