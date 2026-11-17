#include "game/StageWorld.hpp"
#include "practice/Thprac128Section.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
using namespace ecl_fixture;static unsigned checks=0,starts=0;
void check(bool b,const std::string&e){require(b,e);++checks;}
Bytes read(const std::string&p){std::ifstream f(p,std::ios::binary);require(f.good(),"owned source missing");return Bytes(std::istreambuf_iterator<char>(f),{});}
std::unordered_map<std::string,th128::ecl::Program> sources(const std::string&home){std::ifstream f(home+"/files.txt");std::string n,e;std::unordered_map<std::string,th128::ecl::Program> map;while(std::getline(f,n)){const auto b=read(home+"/"+n);check(map[n].load(b.data(),b.size(),e),e);}return map;}
void load(th128::StageWorld&w,const std::string&home){for(const auto&[n,p]:sources(home)){std::string e;check(w.load(n,p.resource().bytes.data(),p.resource().bytes.size(),e),e);}}
struct Hosts {bool held=false;std::vector<int> messages;unsigned spells=0,sounds=0,effects=0,geometry=0;};
void hosts(th128::StageWorld&w,const std::string&home,Hosts&h){std::string e;const auto p=read(home+"/bullet-profile-8.bin");
 check(w.configureBullets(p.data(),p.size(),{0,200,0},[](const th128::bullet::Birth&,std::string& e){e="bullet outside initial section scope";return false;},e),e);
 check(w.setEclRadiusCancellation([](const th128::bullet::Birth&,std::string&){return true;},0,e),e);
 check(w.setMessageHost([&](int n,std::string&){h.messages.push_back(n);return true;},[&](std::string&)->std::optional<bool>{return h.held;},[](const th128::bullet::Birth&,std::string&){return true;},e),e);
 check(w.setPhaseSpellContext({},e),e);check(w.setSpellHost([&](const th128::SpellEvent& event,std::string&){if(event.starting)++h.spells;return true;},e),e);
 check(w.setEclSoundSink([&](int,float,std::string&){++h.sounds;return true;},e),e);
 check(w.setEclEffectHost([&](int,int,std::array<float,3>,int,bool,std::string&){++h.effects;return true;},e),e);
 // Visual ownership is an explicit resource seam; actual World geometry state still executes.
 check(w.setGeometryHost([&](std::uint32_t,float,std::uint32_t,std::string&){++h.geometry;return true;},
  [](std::uint32_t,const std::array<float,3>&,std::string&){return true;},[](std::uint32_t,std::string&){return true;},e),e);
}
bool start(th128::StageWorld&w,const char*n,int rank){++starts;return w.start(n,rank,34772);}
int main(int argc,char**argv){try{check(argc==2,"owned assets");const std::string home=argv[1];std::string e;
 const auto stageRows=[&]{std::ifstream f(home+"/section-stages.txt");std::array<unsigned,157>a{};unsigned i,s;while(f>>i>>s)a[i]=s;return a;}();
 for(const auto variant:{"ja","zh"})for(unsigned id=1;id<=156;++id){th128::StageWorld w;load(w,home+"/"+variant);check(w.prepareThpracSection(stageRows[id],id,0,true,stageRows[id]==16?4:1,{},e),e);}
 for(const auto variant:{"ja","zh"})for(unsigned phase=1;phase<=3;++phase)for(bool dlg:{false,true}){th128::StageWorld w;load(w,home+"/"+variant);check(w.prepareThpracSection(16,156,phase,dlg,4,{},e),e);}
 {th128::StageWorld w;load(w,home+"/ja");for(const auto a:std::vector<std::array<unsigned,3>>{{1,0,0},{1,157,0},{2,4,0},{1,4,1},{16,156,4}})check(!w.prepareThpracSection(a[0],a[1],a[2],false,a[0]==16?4:1,{},e)&&!w.thpracChapter(),"invalid section/stage/phase stays atomic");
  check(w.prepareThpracSection(1,4,0,true,1,{},e),e);check(start(w,"st_a1_1.ecl",1),w.error());const auto own=w.thpracChapter();check(own&&own->section==4&&own->chapter==0&&own->dialogue&&own->nonNormal,"actual section identity adopted");
  Hosts h;h.held=true;hosts(w,home,h);for(int i=0;i<3;++i)check(w.tick()&&w.spawnCount()==0,"actual initial dialogue gate holds Boss creation");
  check(h.messages==std::vector<int>{0},"upstream starts dialogue entry0 once");h.held=false;check(w.tick(),w.error());check(w.spawnCount()==1&&w.entities()[1].entry=="Boss","Boss created after real gate releases");
  w.reset();check(!w.thpracChapter(),"reset removes section/dialogue/phase identity");}
 for(const auto variant:{"ja","zh"}){th128::StageWorld w;load(w,home+"/"+variant);check(w.prepareThpracSection(1,5,0,false,1,{},e),e);check(start(w,"st_a1_1.ecl",1),w.error());Hosts h;hosts(w,home,h);
  for(int tick=0;tick<100&&h.spells==0;++tick)check(w.tick(),w.error());check(w.entities().size()>1&&w.entities()[1].entry=="Boss"&&w.entities()[1].life==1200,"actual first card Boss HP1200 from upstream include sequence");
  check(h.spells>0&&w.phaseSpellContext()&&w.phaseSpellContext()->id==1,"actual original first spell section uses Normal spell id1");}
 {th128::StageWorld w;load(w,home+"/ja");check(w.prepareThpracSection(3,18,0,false,1,{},e),e);check(start(w,"st_a1_3.ecl",1),w.error());Hosts h;hosts(w,home,h);check(w.tick()&&w.spawnCount()==1&&w.entities()[1].entry=="BossA","triple root time0 creates BossA");check(w.tick(),w.error());check(w.spawnCount()==3&&w.entities()[2].entry=="BossB"&&w.entities()[3].entry=="BossC","triple root time1 creates B/C");}
 for(unsigned phase=0;phase<4;++phase){const auto map=sources(home+"/ja");th128::ecl::Program root;std::unordered_map<std::string,th128::ecl::Program> prepared;
  check(th128::practice::prepareOwnedSection(map,16,156,phase,false,root,prepared,e),e);auto boss=prepared.at("st_exb.ecl");std::vector<std::pair<std::string,th128::ecl::Program>> providers(prepared.begin(),prepared.end());check(boss.linkIncludes(providers,e),e);
  th128::ecl::Limits lim;lim.maxInstructionsPerTick=10000;th128::ecl::Vm vm(boss,lim);int life=0;bool armed=false;
  check(vm.enableOriginalHostMissDomain(e),e);
  std::unordered_set<unsigned> installed;
  const std::unordered_set<unsigned> core{0,1,10,11,12,13,14,15,16,17,21,40,41,42,43,44,45,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,81,82,83,84,85,86,87,89};
  for(const auto& sub:boss.subroutines())for(const auto& ins:sub.instructions)if(!core.contains(ins.opcode)&&installed.insert(ins.opcode).second)
   check(vm.registerHostOpcode(ins.opcode,[&](th128::ecl::HostCall& call){if(call.instruction().opcode==411){int n=0;if(!call.readInteger(0,n))return false;life=n;if(call.instruction().offset==384){armed=true;return call.holdCurrentCommand();}}return true;},e),e);
  // Semantic host seam: execute original initialization first, hold exactly at the patched HP command.
  // Resource/animation/message hosts here are no-ops; this is not actual EX World gameplay.
  check(vm.start("Boss"),"actual EX Boss Vm start");for(int tick=0;tick<200&&!armed&&vm.status()==th128::ecl::VmStatus::Running;++tick)vm.runTick();
  check(armed,"original Boss prelude reaches exact HP patch");
  const int expected[]={7000,4000,1600,7000};check(life==expected[phase],"independent fixed EXfinalphase HP");
 }
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualWorldStarts\":"<<starts<<",\"phaseVmHeldAtHP\":4,\"nativeFactory\":0,\"browser\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<" starts="<<starts<<"\n";return 1;}}
