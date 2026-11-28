#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
using namespace ecl_fixture;static unsigned checks=0,starts=0;
extern "C" unsigned th128_host_capsule_next_serial(){static unsigned serial=0;return ++serial;}
void check(bool ok,const std::string&why){require(ok,why);++checks;}
Bytes read(const std::string&p){std::ifstream f(p,std::ios::binary);require(f.good(),"owned fixture missing: "+p);return Bytes(std::istreambuf_iterator<char>(f),{});}
void fresh(const std::string&home){th128_world_reset();std::ifstream list(home+"/files.txt");std::string name;while(std::getline(list,name)){const auto b=read(home+"/"+name);check(th128_world_load(name.data(),unsigned(name.size()),b.data(),unsigned(b.size()))==1,th128_world_error());}}
bool start(const char*name,int rank){++starts;return th128_world_start(name,unsigned(std::strlen(name)),rank,34772)==1;}
#if defined(TH128_THPRAC_WEB_ABI)
void feature(const std::string&home,int scenario){
 fresh(home);std::array<std::int32_t,11> words{1,0,10102,0,0,1,123456789,54321,12345,6789,800};
 if(scenario==1)words={1,0,5,0,0,1,0,0,0,0,0};
 if(scenario==2)words={1,15,156,3,0,4,999999999,100000,30000,10000,3000};
 const auto original=words;
 check(th128_world_prepare_thprac128(nullptr,44)==0,"null payload rejected");
 check(th128_world_prepare_thprac128(words.data(),43)==0&&th128_world_prepare_thprac128(words.data(),45)==0,"exact11word byte length");
 if(scenario==3){
  for(const auto [index,value]:std::vector<std::pair<unsigned,int>>{{0,2},{1,16},{1,-1},{2,10202},{2,10108},{2,157},{3,1},{4,2},{5,4},{6,1000000000},{6,-1},{7,100001},{8,30001},{9,10001},{10,3001}}){
   auto bad=words;bad[index]=value;check(th128_world_prepare_thprac128(bad.data(),44)==0&&!world->thpracChapter(),"invalid Web ABI keeps owner absent");
  }
  check(start("st_a1_1.ecl",1)&&!debugRunTainted&&scoreRaw==0,"failed Web payload never taints ordinary start");return;
 }
 check(th128_world_prepare_thprac128(words.data(),44)==1,th128_world_error());words.fill(0);
 check(th128_world_prepare_thprac128(original.data(),44)==0,"repeat Web preparation rejected");
 check(!start(scenario==2?"st_ex.ecl":"st_a1_1.ecl",scenario==2?3:0),"pending Web rank mismatch rejected before start");
 check(start(scenario==2?"st_ex.ecl":"st_a1_1.ecl",original[5]),th128_world_error());
 const auto owner=world->thpracChapter();check(owner&&owner->stageId==unsigned(original[1]+1)&&owner->difficulty==original[5]&&owner->nonNormal,"actual Web owner reaches StageWorld start");
 check(owner->initial.score==std::int64_t(original[6])*10&&owner->initial.motivation==original[7]&&owner->initial.ice_area==original[10],"typed immutable payload copied before caller mutation");
 check(debugRunTainted&&scoreRaw==original[6]&&cumulativeFrozenArea==float(original[10]),"actual WorldExports initial score/area/taint");
 check(th128_world_records_restore(nullptr,0)==0&&!normalRecord,"actual practice cannot acquire ordinary record owner");
 const auto profile=read(home+"/bullet-profile-8.bin");std::string error;
 check(world->configureBullets(profile.data(),profile.size(),{0,200,0},[](const th128::bullet::Birth&,std::string&e){e="frame0 component boundary";return false;},error),error);
 check(world->enableLocalEnemyPasses(error),error);
 check(world->spawnCount()==0,"practice ECL must not spawn before actual player/resource hosts are configured");
 const auto anm=read(home+"/pl00.anm"),shot=read(home+"/pl00a.sht");const std::array<unsigned,th128::shotenemy::DamageSourceCount> flags{};
 check(th128_world_enable_graze(0,original[9],0)==1,th128_world_error());
 check(th128_world_enable_player_context(anm.data(),unsigned(anm.size()),1,0,120,0,0,0,0,0,original[8])==1,th128_world_error());
 check(th128_world_enable_player_motion(0,51200)==1,th128_world_error());
 check(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),anm.data(),unsigned(anm.size()),flags.data(),unsigned(flags.size()),float(original[10]),original[5],0,0,0,0)==1,th128_world_error());
 check(th128_world_enable_shot_hits(original[7],0,0,0,0)==1,th128_world_error());
 const unsigned expectedGroups[]={6,0,15};check(playerShotGroups->state().group==expectedGroups[scenario]&&playerShotGroups->state().activeHandles>0,"actual original SHT/ANM group selected by Web fields");
 check(grazeGlobals.powerRaw==original[9]&&playerWindowControl.perfectCharge4b4d70==original[8]&&motivationRaw==original[7],"actual PlayerHitHost/shot/graze globals read Web values");
 check(th128_world_prepare_thprac128(original.data(),44)==0,"late Web payload cannot replace started owner");
 const std::string receipt=th128_world_thprac128_state();check(receipt.find("\"contexts\":15")!=std::string::npos&&receipt.find("\"nonNormal\":true")!=std::string::npos,"actual configured receipt fields");
 std::cout<<"NATIVE_THPRAC_RECEIPT "<<scenario<<" "<<receipt<<"\n";
 fresh(home);check(!world->thpracChapter()&&!debugRunTainted,"actual gameplay reset removes Web owner/taint");
 check(std::string(th128_world_thprac128_state()).find("\"active\":false")!=std::string::npos,"reset receipt no longer active");
 check(start("st_a1_1.ecl",1)&&!debugRunTainted&&scoreRaw==0&&cumulativeFrozenArea==0,"actual ordinary restart after practice retains original defaults");
}
#endif
int main(int argc,char**argv){try{check(argc==3,"owned assets and scenario");const std::string home=argv[1];const int scenario=std::stoi(argv[2]);check(scenario>=0&&scenario<=3,"bounded fresh scenario");
#if defined(TH128_THPRAC_WEB_ABI)
 feature(home,scenario);
#else
 fresh(home);check(start("st_a1_1.ecl",1)&&!debugRunTainted,"actual ordinary WorldExports control before Web feature RED");check(false,"actual TH128 Web native prepare/receipt consumers are missing");
#endif
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldExportsStartCalls\":"<<starts<<",\"wholeWorldExportsCpp\":true,\"actualSHTANM\":true,\"Factories\":0,\"browser\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
