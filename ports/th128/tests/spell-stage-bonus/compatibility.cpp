#include "game/StageWorld.hpp"
#include "ecl-fixture.hpp"
#include <iostream>
#include <sstream>
#include <functional>
#include <tuple>
using namespace ecl_fixture;
unsigned worldStarts=0,worldTicks=0,checks=0,failures=0;
std::string json(const std::string& s){std::string o="\"";for(unsigned char c:s){if(c=='"'||c=='\\'){o+='\\';o+=char(c);}else if(c=='\n')o+="\\n";else if(c=='\r')o+="\\r";else o+=char(c);}return o+'"';}
std::vector<std::uint32_t> words(int id,int limit,const std::string& title,bool terminated=true){
 auto result=stringWords(title);
 if(!terminated){result={4,0x41414141};}
 std::uint8_t key=0x77,step=7;
 for(std::size_t index=4;index<result.size()*4;++index){const auto word=index/4,shift=(index%4)*8;const auto byte=std::uint8_t(result[word]>>shift);result[word]=(result[word]&~(255u<<shift))|(std::uint32_t(byte^key)<<shift);key=std::uint8_t(key+step);step=std::uint8_t(step+16);}
 std::vector<std::uint32_t> out{std::uint32_t(id),std::uint32_t(limit),500000};out.insert(out.end(),result.begin(),result.end());return out;
}
struct Spec{int opcode=428,rank=0,id=70,limit=2280,expectedId=70,parameters=4;bool accept=true,context=true,host=true,reject=false,throws=false,terminated=true,extra=false;std::string title="Source alias";};
void scenario(const Spec& s){
 const auto file=s.extra?"st_ex.ecl":"component.ecl";
 const auto data=program({{"main",join({instruction(411,{1200}),instruction(std::uint16_t(s.opcode),words(s.id,s.limit,s.title,s.terminated),0,0,255,s.parameters),instruction(83,{4}),instruction(423),instruction(0,{},1000)})}});
 th128::StageWorld world;std::string error;require(world.load(file,data.data(),data.size(),error),error);require(world.start(file,s.rank,34772),world.error());++worldStarts;
 if(s.context)require(world.setPhaseSpellContext({},error),error);
 unsigned starts=0,ends=0;th128::SpellEvent begin,end;
 if(s.host)require(world.setSpellHost([&](const th128::SpellEvent& e,std::string& why){if(e.starting){++starts;begin=e;if(s.throws)throw std::runtime_error("fixture effect failure");if(s.reject){why="fixture effect refusal";return false;}}else{++ends;end=e;}return true;},error),error);
 const bool entered=world.enableLocalEnemyPasses(error);
 if(!s.accept){
  require(!entered,"malformed or unowned spell unexpectedly accepted");require(world.faulted(),"rejected VM must fault explicitly");
  require(starts==unsigned(s.reject||s.throws),"invalid operands reached effect callback");require(ends==0,"rejected entry ended a spell");
  if(s.context)require(!(world.phaseSpellContext()->flags&1u)&&world.phaseSpellContext()->id==-1,"rejected callback committed spell state");
  require(!(world.entities().front().lifeWords[5]&1u),"rejected spell enabled expanded HP");
  return;
 }
 require(entered,error.empty()?world.error():error);require(starts==1&&ends==0,"exact start callback count");
 require(begin.state.id==s.expectedId,"original fixed/rank adjusted identifier");require(begin.state.limit==s.limit,"original spell timeout");
 require(begin.state.baseBonus==(s.rank+1)*100000&&begin.state.value80==unsigned((s.rank+1)*100000),"rank bonus");
 require(begin.title==std::vector<std::uint8_t>(s.title.begin(),s.title.end()),"encrypted source title");
 require(begin.state.flags==7u&&begin.state.age==0,"fresh begin state");
 const auto enemy=world.entities().front();require((enemy.lifeWords[5]&1u)&&enemy.lifeWords[3]==8400,"expanded enemy HP producer");
 require(world.phaseSpellContext()->id==s.expectedId&&world.spellTitle()==begin.title,"actual spell state committed");
 const auto random=world.gameRandomState();
 for(int i=0;i<8;++i){++worldTicks;require(world.tick(),world.error());}
 require(starts==1&&ends==1,"423 owns exactly one end");require(end.title==begin.title&&end.result.card==s.expectedId,"end preserves ID and decoded title");
 require(end.result.captured&&end.result.notice==7&&end.result.bonus==(s.rank+1)*100000,"actual short spell result");
 require(!(world.phaseSpellContext()->flags&1u)&&!(world.entities().front().lifeWords[5]&1u),"423 clears active and expanded HP");
 require(world.gameRandomState().seed==random.seed&&world.gameRandomState().calls==random.calls,"spell component did not consume game RNG");
}
void run(const std::string& name,const Spec& s){++checks;try{scenario(s);std::cout<<"{\"name\":"<<json(name)<<",\"passed\":true}\n";}catch(const std::exception& e){++failures;std::cout<<"{\"name\":"<<json(name)<<",\"passed\":false,\"error\":"<<json(e.what())<<"}\n";}}
void rule(const std::string& name,const std::function<void()>& f){++checks;try{f();std::cout<<"{\"name\":"<<json(name)<<",\"passed\":true}\n";}catch(const std::exception& e){++failures;std::cout<<"{\"name\":"<<json(name)<<",\"passed\":false,\"error\":"<<json(e.what())<<"}\n";}}
auto values(const th128::enemy::PhaseSpell& s){return std::tuple{s.flags,s.age,s.value80,s.playerMode,s.id,s.limit,s.baseBonus};}
int main(){
 for(int rank=0;rank<4;++rank)for(int op:{422,428,437,438,439}){Spec s;s.rank=rank;s.opcode=op;s.expectedId=op==422||op==428?70:op==437?70+rank:op==438?69+rank:68+rank;run("start-"+std::to_string(op)+"-rank-"+std::to_string(rank),s);}
 for(int op:{422,428}){Spec s;s.opcode=op;s.rank=4;s.id=s.expectedId=240;s.extra=true;run("EX-fixed-"+std::to_string(op),s);}
 for(const auto& name:{"id-negative","id-256","limit-300","limit-65536","empty-title","unterminated-title","parameter-count","missing-context","missing-host","effect-refused","effect-threw"}){
  Spec s;s.accept=false;const std::string n=name;
  if(n=="id-negative")s.id=-1;if(n=="id-256")s.id=256;if(n=="limit-300")s.limit=300;if(n=="limit-65536")s.limit=65536;
  if(n=="empty-title")s.title="";if(n=="unterminated-title")s.terminated=false;if(n=="parameter-count")s.parameters=3;
  if(n=="missing-context")s.context=false;if(n=="missing-host")s.host=false;if(n=="effect-refused")s.reject=true;if(n=="effect-threw")s.throws=true;
  run(n,s);
 }
 for(const auto& name:{"EX-id239","EX-id250","EX-rankadjusted437"}){Spec s;s.accept=false;s.extra=true;s.rank=4;s.id=240;const std::string n=name;if(n=="EX-id239")s.id=239;if(n=="EX-id250")s.id=250;if(n=="EX-rankadjusted437")s.opcode=437;run(n,s);}
 constexpr int bonus[3][4]={{100000,200000,300000,400000},{200000,300000,400000,500000},{300000,400000,500000,600000}};
 for(int rank=0;rank<4;++rank)rule("legacy-four-argument-"+std::to_string(rank),[&]{th128::enemy::PhaseSpell s{0x1f0u,17,123u,1,42,900,777};require(th128::spell::begin(s,70,2280,rank),"legacy begin rejected");require(s.baseBonus==bonus[0][rank]&&s.value80==unsigned(bonus[0][rank])&&s.flags==0x147u,"legacy source contract changed");});
 for(int depth=1;depth<=3;++depth)for(int rank=0;rank<4;++rank)rule("stage-depth"+std::to_string(depth)+"-rank"+std::to_string(rank),[&]{th128::enemy::PhaseSpell s{0x1f0u,17,123u,1,42,900,777};require(th128::spell::begin(s,70,2280,rank,depth),"stage begin rejected");require(s.baseBonus==bonus[depth-1][rank]&&s.value80==unsigned(bonus[depth-1][rank])&&s.flags==0x147u&&s.playerMode==1&&s.id==70&&s.limit==2280&&s.age==0,"stage scalar result differs");});
 for(int depth:{0,4})rule("invalid-stage-depth-"+std::to_string(depth),[&]{th128::enemy::PhaseSpell s{0x1f0u,17,123u,1,42,900,777};const auto before=values(s);require(!th128::spell::begin(s,70,2280,2,depth)&&values(s)==before,"invalid stage depth mutated state");});
 std::cout<<"{\"summary\":true,\"checks\":"<<checks<<",\"failed\":"<<failures<<",\"WorldStarts\":"<<worldStarts<<",\"worldTickCalls\":"<<worldTicks<<",\"syntheticECL\":true,\"syntheticEffectCallback\":true,\"originalGameplay\":false}\n";
 return failures?1:0;
}
