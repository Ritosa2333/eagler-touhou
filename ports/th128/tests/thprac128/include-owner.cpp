#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
using namespace ecl_fixture;
static unsigned checks=0,failures=0,starts=0;
void check(bool b,const std::string& s){++checks;if(!b){++failures;std::cerr<<"ASSERT_FAIL "<<s<<"\n";}}
Bytes read(const std::string& p){std::ifstream f(p,std::ios::binary);require(f.good(),"owned fixture missing");return Bytes(std::istreambuf_iterator<char>(f),{});}
void load(th128::StageWorld& w,const std::string& folder,const std::string& omit=""){
 std::ifstream f(folder+"/files.txt");require(f.good(),"ECL list");std::string name,error;while(std::getline(f,name)){if(name==omit)continue;const auto b=read(folder+"/"+name);require(w.load(name,b.data(),b.size(),error),error);}
}
bool start(th128::StageWorld& w,const char* name,int rank){++starts;return w.start(name,rank,34772);}
int main(int argc,char**argv){try{
 require(argc==2,"owned fixture dir");const std::string home=argv[1];std::string error;
 for(const auto name:{"default.ecl","st_a1_1b.ecl"}){
  th128::StageWorld w;load(w,home+"/ja");auto bad=read(home+"/ja/"+name);bad.back()^=1;
  require(w.load(name,bad.data(),bad.size(),error),error);
  const bool accepted=w.prepareThpracChapter(1,2,1,error);check(!accepted,std::string("practice rejects changed reachable include: ")+name);
  if(accepted){check(start(w,"st_a1_1.ecl",1),w.error());check(w.thpracChapter().has_value(),"old unsafe caller actually adopts changed provider");}
  else{const auto good=read(home+"/ja/"+name);check(w.load(name,good.data(),good.size(),error),"failed validation leaves provider loading open");check(w.prepareThpracChapter(1,2,1,error),"correct original can prepare after failed closure validation");check(start(w,"st_a1_1.ecl",1),w.error());}
 }
 {th128::StageWorld w;load(w,home+"/ja","default.ecl");check(!w.prepareThpracChapter(1,2,1,error),"missing reachable include rejected before pending commit");check(!w.thpracChapter(),"missing provider makes no active identity");}
 {th128::StageWorld w;load(w,home+"/ja");const auto irrelevant=program({{"unrelated",instruction(10)}});require(w.load("unrelated.ecl",irrelevant.data(),irrelevant.size(),error),error);check(w.prepareThpracChapter(1,2,1,error),"unrelated loaded source is outside actual dependency closure");check(start(w,"st_a1_1.ecl",1)&&w.linkedSources()==3,w.error());}
 {th128::StageWorld w;load(w,home+"/ja");auto bad=read(home+"/ja/default.ecl");bad.back()^=1;require(w.load("default.ecl",bad.data(),bad.size(),error),error);check(start(w,"st_a1_1.ecl",1)&&!w.thpracChapter(),"ordinary original loader is not changed into a new practice resource gate");}
 for(const auto variant:{"ja","zh"})for(unsigned id=1;id<=16;++id){th128::StageWorld w;load(w,home+"/"+variant);check(w.prepareThpracChapter(id,2,id==16?4:1,error),std::string("existing supported ")+variant+" chapter2 stage"+std::to_string(id)+": "+error);}
 {th128::StageWorld w;load(w,home+"/zh");if(w.prepareThpracChapter(3,2,1,error)){
   check(start(w,"st_a1_3.ecl",1),w.error());const auto profile=read(home+"/bullet-profile-8.bin");
   check(w.configureBullets(profile.data(),profile.size(),{0,200,0},[](const th128::bullet::Birth&,std::string& e){e="unexpected bullet beyond first birth scope";return false;},error),error);
   for(unsigned tick=0;tick<90&&checks<10000;++tick)require(w.tick(),w.error());
   require(w.tick(),w.error());check(w.frame()==91&&w.spawnCount()>0,"translated third-stage root reaches its actual chapter first birth");
 }else check(false,"translated third-stage actual start selection must prepare");}
 std::cout<<"{\"passed\":"<<(failures==0?"true":"false")<<",\"checks\":"<<checks<<",\"failures\":"<<failures<<",\"actualStageWorldStartCalls\":"<<starts<<",\"entryTrampoline\":false}\n";return failures?1:0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" failures="<<failures<<" starts="<<starts<<"\n";return 1;}}
