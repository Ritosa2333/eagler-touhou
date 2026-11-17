#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <concepts>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
using namespace ecl_fixture;
static unsigned checks=0,starts=0;
void check(bool ok,const std::string& why){require(ok,why);++checks;}
Bytes read(const std::string& p){std::ifstream f(p,std::ios::binary);require(f.good(),"original resource missing: "+p);return Bytes(std::istreambuf_iterator<char>(f),{});}
template<class W> concept Available=requires(W& w,std::string& error){
 {w.prepareThpracChapter(1u,2u,1,error)}->std::same_as<bool>;
 w.thpracChapter();
};
bool start(th128::StageWorld& w,const char* name,int rank){++starts;return w.start(name,rank,34772);}
void loadAll(th128::StageWorld& w,const std::string& home){
 std::ifstream list(home+"/files.txt");check(list.good(),"owned original DAT ECL file list");
 std::string name,error;while(std::getline(list,name)){auto b=read(home+"/"+name);check(w.load(name,b.data(),b.size(),error),error);}
}
void playerContext(th128::StageWorld& w,const std::string& home){
 const auto profile=read(home+"/bullet-profile-8.bin");std::string error;
 check(w.configureBullets(profile.data(),profile.size(),{0,200,0},
  [](const th128::bullet::Birth&,std::string& reason){reason="unexpected bullet birth outside first-enemy scope";return false;},error),error);
}
template<class W>void feature(const std::string& home){
 if constexpr(!Available<W>)check(false,"production StageWorld TH128 thprac owner/start consumer is missing");
 else{
  std::string error;
  W missing;check(!missing.prepareThpracChapter(1,2,1,error),"missing actual resource rejected");
  W authored;const auto synthetic=program({{"main",join({instruction(0,{},1000),instruction(10)})}});
  check(authored.load("st_a1_1.ecl",synthetic.data(),synthetic.size(),error),error);
  check(!authored.prepareThpracChapter(1,2,1,error),"foreign synthetic named ECL rejected by whole original identity");
  W changed;auto wrong=read(home+"/st_a1_1.ecl");wrong.back()^=1;
  check(changed.load("st_a1_1.ecl",wrong.data(),wrong.size(),error),error);
  check(!changed.prepareThpracChapter(1,2,1,error),"parse-valid same-size mutated original SHA rejected");
  W owned;loadAll(owned,home);
  for(const auto args:std::vector<std::array<unsigned,3>>{{0,2,1},{17,1,1},{1,0,1},{1,8,1},{1,2,4},{16,2,1}}){
   check(!owned.prepareThpracChapter(args[0],args[1],int(args[2]),error),"illegal stage/chapter/rank rejected");
   check(!owned.thpracChapter(),"failed prepare never invents active receipt");
  }
  check(owned.prepareThpracChapter(1,2,1,error),error);
  check(!owned.thpracChapter(),"prepared is not an active World");
  check(!owned.prepareThpracChapter(1,2,1,error)&&!owned.prepareThpracChapter(1,3,1,error),"repeated/conflicting prepare rejected");
  const auto original=read(home+"/st_a1_1.ecl");
  check(!owned.load("st_a1_1.ecl",original.data(),original.size(),error),"pending owner locks loaded source set");
  check(!start(owned,"st_a1_1.ecl",0)&&owned.frame()==0&&owned.entities().empty(),"wrong rank start is atomic");
  check(!start(owned,"st_ex.ecl",4)&&owned.entities().empty(),"foreign stage cannot consume pending selection");
  check(start(owned,"st_a1_1.ecl",1),owned.error());
  playerContext(owned,home); // Explicit real profile/player context; no physical player or bullet output.
  auto receipt=owned.thpracChapter();check(receipt&&receipt->stageId==1&&receipt->chapter==2&&receipt->difficulty==1&&receipt->nonNormal,"actual started World practice receipt");
  check(owned.linkedSources()==3&&owned.spawnCount()==0,"actual original root/default/boss sources linked before ticks");
  check(!owned.prepareThpracChapter(1,3,1,error),"late live configuration rejected");
  for(unsigned tick=0;tick<90;++tick){check(owned.tick(),owned.error());check(owned.spawnCount()==0,"real chapter time90 has no early birth");}
  check(owned.tick(),owned.error());check(owned.frame()==91&&owned.spawnCount()==1&&owned.entities().at(1).entry=="GirlB1","A1 chapter2 executes actual MainSub01, not ordinary MainSub00");
  const auto retainedFrame=owned.frame();const auto retainedCount=owned.entities().size();
  check(!start(owned,"st_a1_1.ecl",1)&&owned.frame()==retainedFrame&&owned.entities().size()==retainedCount,"active practice requires reset before another start");
  owned.reset();check(!owned.thpracChapter(),"explicit reset clears practice identity");
  check(start(owned,"st_a1_1.ecl",1),owned.error());playerContext(owned,home);
  for(unsigned tick=0;tick<100;++tick){check(owned.tick(),owned.error());check(owned.spawnCount()==0,"ordinary original wait100 unchanged after practice reset");}
  check(owned.tick(),owned.error());check(!owned.thpracChapter()&&owned.frame()==101&&owned.entities().at(1).entry=="GirlA1b","reset restores original owned Program without inherited chapter");
  W multi;loadAll(multi,home);check(multi.prepareThpracChapter(2,8,1,error),error);check(start(multi,"st_a1_2.ecl",1),multi.error());playerContext(multi,home);
  for(unsigned tick=0;tick<90;++tick){check(multi.tick(),multi.error());check(multi.spawnCount()==0,"multijump waits original eclTime90");}
  check(multi.tick(),multi.error());check(multi.frame()==91&&multi.entities().at(1).entry=="GirlD2","both actual multijump sites select MainSub07 first birth");
  W extra;loadAll(extra,home);check(extra.prepareThpracChapter(16,2,4,error),error);check(start(extra,"st_ex.ecl",4),extra.error());playerContext(extra,home);
  for(unsigned tick=0;tick<160;++tick){check(extra.tick(),extra.error());check(extra.spawnCount()==0,"EX chapter2 retains MainSub01 wait70 after branch90");}
  check(extra.tick(),extra.error());check(extra.frame()==161&&extra.spawnCount()==2&&extra.entities().at(1).entry=="GirlD1"&&extra.entities().at(2).entry=="GirlD1","EX original pair births through actual World/Vm");
  W incomplete;check(incomplete.load("st_a1_1.ecl",original.data(),original.size(),error),error);check(incomplete.prepareThpracChapter(1,2,1,error),error);
  check(!start(incomplete,"st_a1_1.ecl",1)&&incomplete.entities().empty()&&!incomplete.thpracChapter(),"missing include start fails before World commit");
  incomplete.reset();loadAll(incomplete,home);check(start(incomplete,"st_a1_1.ecl",1)&&!incomplete.thpracChapter(),"reset after failed prepared start permits ordinary fresh owner");
  W retired;loadAll(retired,home);check(retired.prepareThpracChapter(1,2,1,error)&&start(retired,"st_a1_1.ecl",1),error);
  check(retired.retireEntitiesForSession(error),error);check(!retired.thpracChapter()&&!retired.prepareThpracChapter(1,2,1,error),"retirement drops practice and rejects old owner reconfiguration");
  W reentrant;const auto event=program({{"main",join({instruction(259,{0,1}),instruction(0,{},100)})}});
  check(reentrant.load("event.ecl",event.data(),event.size(),error)&&start(reentrant,"event.ecl",1),error);
  bool rejected=false;check(reentrant.setAnimationHost([&](const th128::AnimationBirth&,std::string&){std::string why;rejected=!reentrant.prepareThpracChapter(1,2,1,why);return true;},[](std::uint32_t){},error),error);
  check(reentrant.tick()&&rejected&&!reentrant.thpracChapter(),"host reentry cannot replace active World with practice");
 }
}
int main(int argc,char**argv){try{
 check(argc==2,"owned original resource directory required");
 {th128::StageWorld control;std::string error;const auto raw=program({{"main",join({instruction(0,{},100),instruction(10)})}});
  check(control.load("control.ecl",raw.data(),raw.size(),error)&&start(control,"control.ecl",1)&&control.tick()&&control.frame()==1,"actual ordinary StageWorld control before missing feature RED");}
 feature<th128::StageWorld>(argv[1]);
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualStageWorldStartCalls\":"<<starts<<",\"entryTrampoline\":false,\"Factories\":0,\"browser\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
