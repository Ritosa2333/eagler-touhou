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
int main(int argc,char**argv){try{check(argc==3,"resources");const std::string home=argv[1];
 for(bool bind:{false,true}){setup(home,5);const auto msg=read(home+"/st_a1.msg"),text=read(home+"/text.anm"),player=read(home+"/pl00.anm"),boss=read(home+"/boss01.anm"),front=read(home+"/front.anm");
 check(th128_world_enable_messages(msg.data(),unsigned(msg.size()),text.data(),unsigned(text.size()),player.data(),unsigned(player.size()),boss.data(),unsigned(boss.size()))==1,th128_world_error());
 check(th128_world_enable_message_front(front.data(),unsigned(front.size()))==1,th128_world_error());
 if(bind)check(th128_world_enable_route_choice(front.data(),unsigned(front.size()),1,0,0)==1,th128_world_error());
 check(!messages->recordAuxiliaryLive().has_value(),"actual MSG contains routechoice requiring owning host");
 check(th128_world_thprac_replay_begin(10)==1&&th128_world_thprac_replay_clock(10+1./60,60)==1,th128_world_error());const auto advanced=th128_world_tick();
 if(bind){check(advanced==1&&world->frame()==1,th128_world_error());check(thpracReplay.record->frames()==1&&normalRecord==nullptr&&normalReplay==nullptr,"actual routechoiceknownfalse recorded withoutnormalqualification");}
 else check(advanced==0&&world->frame()==0&&std::string(th128_world_error()).find("auxiliary ownership unknown")!=std::string::npos,"unknown routechoice owner remains rejected");}
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldExportsStartCalls\":"<<starts<<",\"wholeWorldExportsCpp\":true,\"Factories\":0}\n";return 0;}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
