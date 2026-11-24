#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
using namespace ecl_fixture;
extern "C" unsigned th128_host_capsule_next_serial(){static unsigned serial=0;return ++serial;}
static unsigned checks=0,starts=0;
void check(bool ok,const std::string& why){require(ok,why);++checks;}
Bytes read(const std::string& p){std::ifstream f(p,std::ios::binary);require(f.good(),"owned asset missing: "+p);return Bytes(std::istreambuf_iterator<char>(f),{});}
void fresh(const std::string& home){
 th128_world_reset();check(!world->thpracChapter(),"actual reset removes practice owner");
 std::ifstream list(home+"/files.txt");std::string name;
 while(std::getline(list,name)){const auto b=read(home+"/"+name);check(th128_world_load(name.data(),unsigned(name.size()),b.data(),unsigned(b.size()))==1,th128_world_error());}
}
bool begin(const char* name,int rank){++starts;return th128_world_start(name,unsigned(std::strlen(name)),rank,34772)==1;}
template<class W>concept HasInitial=requires(W& w,std::string& e){typename W::ThpracInitial;w.prepareThpracChapter(1u,2u,1,typename W::ThpracInitial{},e);};
void geometry(const std::string& home){
 const auto p=read(home+"/bullet-profile-8.bin");std::string error;
 check(world->configureBullets(p.data(),p.size(),{0,200,0},[](const th128::bullet::Birth&,std::string& e){e="unexpected bullet outside frame0 scope";return false;},error),error);
 check(world->enableLocalEnemyPasses(error),error);
}
template<class W>void exercise(const std::string& home,const typename W::ThpracInitial& input,unsigned stage,unsigned expectedGroup){
 fresh(home);W& owned=*world;std::string error;auto copy=input;
 check(owned.prepareThpracChapter(stage,2,stage==16?4:1,copy,error),error);copy.motivation=0;copy.ice_area=0;
 check(!owned.prepareThpracChapter(stage,2,stage==16?4:1,input,error),"prepared resource owner rejects repeats");
 check(begin(stage==16?"st_ex.ecl":"st_a1_1.ecl",stage==16?4:1),th128_world_error());
 const auto id=owned.thpracChapter();check(id&&id->initial.score==input.score/10*10&&id->initial.motivation==input.motivation&&id->initial.ice_area==input.ice_area,"initial group copied and score floor canonicalized");
 check(scoreRaw==input.score/10&&debugRunTainted&&cumulativeFrozenArea==float(input.ice_area),"actual full world_start uses initial score/area/non-normal taint before qualification");
 check(th128_world_records_restore(nullptr,0)==0&&!normalRecord,"actual ordinary record restore rejects practice owner before allocation");
 geometry(home);
 check(th128_world_enable_graze(0,input.ice_power==0?1:input.ice_power-1,0)==0&&!grazeConfigured,"mismatched power rejects before graze context install");
 check(th128_world_enable_graze(0,input.ice_power,0)==1,th128_world_error());
 const auto anm=read(home+"/pl00.anm"),shot=read(home+"/pl00a.sht");const std::array<unsigned,th128::shotenemy::DamageSourceCount> flags{};
 check(th128_world_enable_player_context(anm.data(),unsigned(anm.size()),1,0,120,0,0,0,0,0,input.perfect_freeze==0?1:input.perfect_freeze-1)==0&&!playerConfigured,"mismatched charge preserves missing player context");
 check(th128_world_enable_player_context(anm.data(),unsigned(anm.size()),1,0,120,0,0,0,0,0,input.perfect_freeze)==1,th128_world_error());
 check(th128_world_enable_player_motion(0,51200)==1,th128_world_error());
 check(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),float(input.ice_area==0?1:input.ice_area-1),stage==16?4:1,0,0,0,0)==0&&!playerShotsConfigured,"mismatched area cannot create player-shot owners");
 check(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),float(input.ice_area),stage==16?3:0,0,0,0,0)==0&&!playerShotsConfigured,"practice native shot difficulty cannot disagree with its actual stage/rank owner");
 check(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),float(input.ice_area),stage==16?4:1,0,0,0,0)==1,th128_world_error());
 check(th128_world_enable_shot_hits(input.motivation==0?1:input.motivation-1,0,0,0,0)==0&&!shotHitsConfigured,"mismatched motivation rejects before impact context install");
 check(th128_world_enable_shot_hits(input.motivation,0,0,0,0)==1,th128_world_error());
 check(grazeGlobals.powerRaw==input.ice_power&&playerWindowControl.perfectCharge4b4d70==input.perfect_freeze&&motivationRaw==input.motivation&&cumulativeFrozenArea==float(input.ice_area)&&scoreRaw==input.score/10,"five actual gameplay resource globals agree with immutable initial group");
 check(playerShotGroupsConfigured&&playerShotGroups->state().configured&&playerShotGroups->state().group==expectedGroup&&playerShotGroups->state().activeHandles>0,"real original SHT/ANM native shot group uses configured ice area");
 check(!playerShotGroups->snapshots().empty()&&playerHit->snapshot().gate.stateF78==1,"actual configured native owners have animation/player state");
 check(!owned.prepareThpracChapter(stage,2,stage==16?4:1,input,error),"late initial group cannot replace active owner");
}
template<class W>void feature(const std::string& home,int scenario){
 if constexpr(!HasInitial<W>)check(false,"actual TH128 five-resource owner and full native consumer are missing");
 else{
  using I=typename W::ThpracInitial;const I defaults{};
  check(defaults.score==0&&defaults.motivation==100000&&defaults.perfect_freeze==30000&&defaults.ice_power==10000&&defaults.ice_area==3000,"original TH128 thprac resource defaults");
  if(scenario==0)exercise<W>(home,defaults,1,15);
  if(scenario==1)exercise<W>(home,I{0,0,0,0,0},1,0);
  if(scenario==2)exercise<W>(home,I{9999999990LL,100000,30000,10000,3000},16,15);
  if(scenario==3)exercise<W>(home,I{1234567899,54321,12345,6789,800},1,6);
  if(scenario<=3){fresh(home);check(begin("st_a1_1.ecl",1)&&!debugRunTainted&&scoreRaw==0&&cumulativeFrozenArea==0,"actual gameplay reset clears prior practice scalar/taint owner");}
  if(scenario==4){
  fresh(home);W& owned=*world;std::string error;
  for(const auto value:std::vector<I>{{-1,1,1,1,1},{9999999991LL,1,1,1,1},{0,-1,1,1,1},{0,100001,1,1,1},{0,1,-1,1,1},{0,1,30001,1,1},{0,1,1,-1,1},{0,1,1,10001,1},{0,1,1,1,-1},{0,1,1,1,3001}})
   check(!owned.prepareThpracChapter(1,2,1,value,error)&&!owned.thpracChapter(),"invalid typed bounds reject atomically before pending owner");
  check(begin("st_a1_1.ecl",1)&&!debugRunTainted&&scoreRaw==0&&cumulativeFrozenArea==0,"ordinary reset/start has no inherited practice resources or qualification taint");
  }
  if(scenario==5){
  fresh(home);check(begin("st_ex.ecl",4)&&!debugRunTainted&&cumulativeFrozenArea==0,"natural EX keeps original untainted area0");geometry(home);
  const auto anm=read(home+"/pl00.anm"),shot=read(home+"/pl00a.sht");const std::array<unsigned,th128::shotenemy::DamageSourceCount> flags{};
  check(th128_world_enable_graze(0,10000,0)&&th128_world_enable_player_context(anm.data(),unsigned(anm.size()),1,0,120,0,0,0,0,0,0)&&th128_world_enable_player_motion(0,51200),"natural EX native context setup");
  check(!th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),3000,4,0,0,0,0)&&!playerShotsConfigured,"natural EX cannot use practice area without actual owner");
  }
 }
}
int main(int argc,char**argv){try{check(argc==3,"owned resources and fresh-process scenario required");const std::string home=argv[1];const int scenario=std::stoi(argv[2]);check(scenario>=0&&scenario<=5,"bounded scenario");
 feature<th128::StageWorld>(home,scenario);
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldExportsStartCalls\":"<<starts<<",\"wholeWorldExportsCpp\":true,\"actualSHTANM\":true,\"Factories\":0,\"browser\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
