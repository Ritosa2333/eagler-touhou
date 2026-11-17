#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include "D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113/thprac-reference-20261117/upstream-th128-section-recorder-v2.hpp"
#include <fstream>
#include <iostream>
#include <concepts>
using namespace ecl_fixture;static unsigned checks=0,starts=0;
void check(bool v,const std::string& e){require(v,e);++checks;}
Bytes read(const std::string& p){std::ifstream f(p,std::ios::binary);require(f.good(),"missing owned source");return Bytes(std::istreambuf_iterator<char>(f),{});}
void load(th128::StageWorld& w,const std::string& p){std::ifstream f(p+"/files.txt");std::string n,e;while(std::getline(f,n)){const auto b=read(p+"/"+n);require(w.load(n,b.data(),b.size(),e),e);}}
template<class W>concept Sections=requires(W&w,std::string&e){w.prepareThpracSection(1u,4u,0u,false,1,typename W::ThpracInitial{},e);};
void reference(const std::string& out){
 std::ofstream f(out+"/upstream-section-writes.json");f<<"[";bool comma=false;unsigned plans=0;
 for(unsigned id=1;id<=156;++id)for(unsigned dlg=0;dlg<2;++dlg)for(unsigned phase=0;phase<=(id==156?3:0);++phase){
  thprac_fixed_reference::Writer w;w.thPracParam.dlg=dlg;w.thPracParam.phase=phase;thprac_fixed_reference::ECLHelper e;
  w.THPatch(e,thprac_fixed_reference::TH128::th_sections_t(id));if(comma)f<<",";comma=true;++plans;
  f<<"{\"section\":"<<id<<",\"dlg\":"<<dlg<<",\"phase\":"<<phase<<",\"writes\":[";
  bool c=false;for(const auto&x:e.writes()){if(c)f<<",";c=true;f<<"{\"file\":"<<x.file<<",\"offset\":"<<x.offset<<",\"stream\":"<<(x.stream?"true":"false")<<",\"bytes\":[";
   for(std::size_t i=0;i<x.bytes.size();++i){if(i)f<<",";f<<unsigned(x.bytes[i]);}f<<"]}";}f<<"]}";
 }f<<"]\n";check(plans==318,"fixed upstream all156sections/dialogues/EXphase traces");
}
template<class W>void feature(const std::string&home){
 if constexpr(!Sections<W>)check(false,"actual complete TH128 section/Boss/phase StageWorld consumer is missing");
 else{
  std::string e;for(const auto variant:{"ja","zh"})for(unsigned id=1;id<=156;++id){W w;load(w,home+"/"+variant);
   unsigned stage=0;std::ifstream rows(home+"/section-stages.txt");unsigned k=0,s=0;while(rows>>k>>s)if(k==id)stage=s;
   check(w.prepareThpracSection(stage,id,0,false,stage==16?4:1,typename W::ThpracInitial{},e),std::string(variant)+" section"+std::to_string(id)+": "+e);
  }
  W boss;load(boss,home+"/ja");check(boss.prepareThpracSection(1,4,0,false,1,typename W::ThpracInitial{},e),e);
  ++starts;check(boss.start("st_a1_1.ecl",1,34772),boss.error());
  const auto profile=read(home+"/bullet-profile-8.bin");
  check(boss.configureBullets(profile.data(),profile.size(),{0,200,0},[](const th128::bullet::Birth&,std::string&){return false;},e),e);
  check(boss.setMessageHost([](int,std::string&){return true;},[](std::string&)->std::optional<bool>{return false;},[](const th128::bullet::Birth&,std::string&){return true;},e),e);
  check(boss.tick(),boss.error());check(boss.spawnCount()==1&&boss.entities().at(1).entry=="Boss","actual original Boss spawned by upstream non-dialogue entry");
 }
}
int main(int argc,char**argv){try{check(argc==3,"owned fixtures/output");reference(argv[2]);
 {th128::StageWorld w;std::string e;const auto b=program({{"main",join({instruction(0,{},100),instruction(10)})}});check(w.load("control.ecl",b.data(),b.size(),e)&&w.start("control.ecl",1,34772)&&w.tick(),"actual ordinary World before feature RED");++starts;}
 feature<th128::StageWorld>(argv[1]);std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"WorldStarts\":"<<starts<<"}\n";return 0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
