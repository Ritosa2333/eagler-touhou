#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
using namespace ecl_fixture;static unsigned checks=0,starts=0;
extern "C" unsigned th128_host_capsule_next_serial(){static unsigned serial=0;return ++serial;}
void check(bool ok,const std::string&why){require(ok,why);++checks;}
Bytes read(const std::string&p){std::ifstream f(p,std::ios::binary);require(f.good(),"missing owned native resource");return Bytes(std::istreambuf_iterator<char>(f),{});}
void fresh(const std::string&home){th128_world_reset();std::ifstream list(home+"/files.txt");std::string name;while(std::getline(list,name)){const auto b=read(home+"/"+name);check(th128_world_load(name.data(),unsigned(name.size()),b.data(),unsigned(b.size()))==1,th128_world_error());}}
bool start(const char*name,int rank){++starts;return th128_world_start(name,unsigned(std::strlen(name)),rank,34772)==1;}
void setup(const std::string&home){fresh(home);const std::array<std::int32_t,11> p{1,2,10302,0,0,1,123456789,54321,12345,6789,800};check(th128_world_prepare_thprac128(p.data(),44)==1,th128_world_error());check(start("st_a1_3.ecl",1),th128_world_error());
 const auto profile=read(home+"/bullet-profile-8.bin"),bullet=read(home+"/bullet.anm"),text=read(home+"/text.anm");
 for(const auto [bank,name]:std::vector<std::pair<int,const char*>>{{1,"enemy.anm"},{2,"enemy_bf.anm"},{3,"boss03.anm"}}){const auto b=read(home+"/"+name);check(th128_world_load_enemy_anm(bank,b.data(),unsigned(b.size()))==1,th128_world_error());}
 check(th128_world_enable_geometry526(text.data(),unsigned(text.size()))==1,th128_world_error());
 check(th128_world_enable_bullets(profile.data(),unsigned(profile.size()),bullet.data(),unsigned(bullet.size()),0,400)==1,th128_world_error());
 const auto anm=read(home+"/pl00.anm"),shot=read(home+"/pl00a.sht");const std::array<unsigned,th128::shotenemy::DamageSourceCount> flags{};
 check(th128_world_enable_graze(0,6789,0)==1,th128_world_error());check(th128_world_enable_player_context(anm.data(),unsigned(anm.size()),1,0,120,0,0,0,0,0,12345)==1,th128_world_error());check(th128_world_enable_player_motion(0,51200)==1,th128_world_error());check(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),800,1,0,0,0,0)==1,th128_world_error());check(th128_world_enable_shot_hits(54321,0,0,0,0)==1,th128_world_error());}
#if defined(TH128_THPRAC_REPLAY_ABI)
void feature(const std::string&home){
 check(th128_world_thprac_replay_begin(10)==1,th128_world_error());check(th128_world_thprac_replay_begin(10)==0,"duplicate recorder owner rejected");
 check(th128_world_tick()==0&&world->frame()==0,"recording requires an actual frame clock before tick");
 check(th128_world_thprac_replay_clock(10.0+1./60,60)==1,th128_world_error());check(th128_world_thprac_replay_clock(11,60)==0,"duplicate pre-tick clock rejected");
 check(th128_world_input(16,16,0)==1&&th128_world_tick()==1,th128_world_error());
 check(th128_world_thprac_replay_clock(10.0+2./60,60)==1&&th128_world_input(16,0,0)==1&&th128_world_tick()==1,th128_world_error());
 check(th128_world_thprac_replay_clock(10.0+3./60,60)==1&&th128_world_input(0,0,16)==1&&th128_world_tick()==1,th128_world_error());
 auto state=std::string(th128_world_thprac_replay_state());check(state.find("\"frames\":3")!=std::string::npos&&state.find("\"active\":true")!=std::string::npos,"actual successful ticks recorded");
 const std::array<std::uint8_t,8> label{'T','E','S','T',' ',' ',' ',' '};
 const auto size=th128_world_thprac_replay_snapshot(1,0,label.data(),8);check(size==311,"one native RPY stage3/3inputframes/1fps sample bytes");const auto* bytes=static_cast<const std::uint8_t*>(th128_world_thprac_replay_data());check(bytes!=nullptr,"native snapshot owned bytes");Bytes copy(bytes,bytes+size);
 check(get32(copy,36+88)==1&&get32(copy,148+4)==3,"actual native one-stage history");check(copy[148]==3&&copy[149]==0&&get32(copy,148+12)==123456789,"arbitrary stage3 and actual initialscore");
 const Bytes want{16,0,16,0,0,0,16,0,0,0,0,0,0,0,0,0,16,0};check(Bytes(copy.begin()+292,copy.begin()+310)==want,"actual held/pressed/released original6byte packets");
 check(normalReplay==nullptr&&normalRecord==nullptr&&debugRunTainted,"practice history never acquires ordinary record qualification");
 check(th128_world_thprac_replay_clock(9,60)==0&&th128_world_thprac_replay_clock(11,-1)==0,"bad clock/fps rejected without advancing");
 std::ofstream saved(home+"/actual-practice-native.rpyraw",std::ios::binary);saved.write(reinterpret_cast<const char*>(copy.data()),copy.size());saved.close();
 const auto recordedPosition=playerRaw;setup(home);
 const Bytes packet(copy.begin()+148,copy.begin()+310);
 check(th128_world_thprac_replay_playback_load(packet.data(),unsigned(packet.size()))==1,th128_world_error());
 check(th128_world_input(16,16,0)==0&&th128_world_set_debug_infinite_respawn(1)==0,"actual playback playLock refuses live input and assist mutations");
 for(unsigned tick=0;tick<3;++tick)check(th128_world_tick()==1,th128_world_error());
 check(playerRaw.horizontal==recordedPosition.horizontal&&playerRaw.vertical==recordedPosition.vertical,"actual native playback resimulates same bounded recorded movement");
 check(replayPlayback.consumed()==3&&thpracReplay.playback&&normalRecord==nullptr&&normalReplay==nullptr,"actual practice input cursor exhausted without normal qualification");
 check(th128_world_replay_playback_cancel()==1&&!thpracReplay.playback,"actual cancellation releases playLock");
 fresh(home);check(std::string(th128_world_thprac_replay_state()).find("\"active\":false")!=std::string::npos&&th128_world_thprac_replay_snapshot(1,0,label.data(),8)==0,"reset removes recording and snapshots");
 check(start("st_a1_1.ecl",1)&&th128_world_thprac_replay_begin(0)==0,"ordinary run cannot use practice recorder");
 th128::record::NativeRunReplay ordinary;bool rejected=false;try{ordinary.begin(3,1,34772,6,0,0,100000,100000,30000,800,0,51200);}catch(...){rejected=true;}check(rejected,"ordinary routefirst guard remains strict");
}
#endif
int main(int argc,char**argv){try{check(argc==3,"owned resources and outputdir");const std::string home=argv[1];setup(home);check(world->thpracChapter().has_value()&&world->frame()==0&&debugRunTainted,"actual practice control readybeforeRED");
#if defined(TH128_THPRAC_REPLAY_ABI)
 feature(home);
#else
 check(false,"actual practice Replay recorder consumer is missing");
#endif
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldExportsStartCalls\":"<<starts<<",\"wholeWorldExportsCpp\":true,\"Factories\":0,\"browser\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
