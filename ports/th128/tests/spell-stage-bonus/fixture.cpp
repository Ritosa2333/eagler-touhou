#include "game/StageWorld.hpp"
#include "ecl-fixture.hpp"
#include <array>
#include <iostream>
#include <functional>
using namespace ecl_fixture;
unsigned checks=0,failures=0,worldStarts=0,worldTicks=0;
std::string json(const std::string& s){std::string o="\"";for(unsigned char c:s){if(c=='"'||c=='\\'){o+='\\';o+=char(c);}else if(c=='\n')o+="\\n";else if(c=='\r')o+="\\r";else o+=char(c);}return o+'"';}
Bytes programFor(int opcode,bool extra=false){
 auto t=stringWords("Stage bonus");std::uint8_t key=0x77,step=7;
 for(std::size_t n=4;n<t.size()*4;++n){const auto i=n/4,s=n%4*8;const auto byte=std::uint8_t(t[i]>>s);t[i]=(t[i]&~(255u<<s))|(std::uint32_t(byte^key)<<s);key=std::uint8_t(key+step);step=std::uint8_t(step+16);}
 std::vector<std::uint32_t> words{extra?240u:70u,2280,500000};words.insert(words.end(),t.begin(),t.end());
 return program({{"main",join({instruction(411,{1200}),instruction(std::uint16_t(opcode),words,0,0,255,4),instruction(83,{4}),instruction(423),instruction(0,{},1000)})}});
}
void load(th128::StageWorld& w,const std::string& name,int op,bool ex=false){const auto b=programFor(op,ex);std::string error;require(w.load(name,b.data(),b.size(),error),error);}
void execute(th128::StageWorld& w,const std::string& name,int rank,int expected,int opcode=428,bool failedStart=false){
 require(w.start(name,rank,34772),w.error());++worldStarts;std::string error;require(w.setPhaseSpellContext({},error),error);
 unsigned starts=0,ends=0;th128::SpellEvent begin,end;
 require(w.setSpellHost([&](const th128::SpellEvent& e,std::string&){if(e.starting){++starts;begin=e;}else{++ends;end=e;}return true;},error),error);
 require(w.enableLocalEnemyPasses(error),error);require(starts==1&&ends==0,"one real start event");
 const int expectedId=rank==4?240:opcode==437?70+rank:opcode==438?69+rank:opcode==439?68+rank:70;
 require(begin.state.id==expectedId&&begin.state.flags==7&&begin.state.limit==2280,"identity flags/timeout changed");
 require(begin.title==std::vector<std::uint8_t>({'S','t','a','g','e',' ','b','o','n','u','s'}),"title changed");
 if(begin.state.baseBonus!=expected||begin.state.value80!=unsigned(expected)){std::cerr<<"stage="<<name<<" rank="<<rank<<" bonus="<<begin.state.baseBonus<<" expected="<<expected<<'\n';throw std::runtime_error("original stage-dependent bonus differs");}
 require(w.phaseSpellContext()->baseBonus==expected,"live producer committed wrong bonus");
 require(w.entities().front().lifeWords[3]==8400&&(w.entities().front().lifeWords[5]&1u),"expanded HP changed");
 if(failedStart){const auto before=w.phaseSpellContext();require(!w.start("missing.ecl",rank,1),"missing stage accepted");require(w.phaseSpellContext()->baseBonus==before->baseBonus&&w.phaseSpellContext()->id==before->id,"rejected start mutated existing spell");}
 const auto random=w.gameRandomState();
 for(int i=0;i<8;++i){++worldTicks;require(w.tick(),w.error());}
 require(ends==1&&end.result.captured&&end.result.notice==7,"actual423 end ownership");
 require(end.result.bonus==expected&&end.result.rawScore==expected/10,"stage bonus did not reach actual end reward");
 require(!(w.phaseSpellContext()->flags&1u)&&!(w.entities().front().lifeWords[5]&1u),"actual423 did not clear spell mode");
 require(w.gameRandomState().seed==random.seed&&w.gameRandomState().calls==random.calls,"bonus changed RNG");
}
void run(const std::string& name,const std::function<void()>& f){++checks;try{f();std::cout<<"{\"name\":"<<json(name)<<",\"passed\":true}\n";}catch(const std::exception& e){++failures;std::cout<<"{\"name\":"<<json(name)<<",\"passed\":false,\"error\":"<<json(e.what())<<"}\n";}}
int main(){
 struct Stage{const char* name;int depth;};
 constexpr std::array<Stage,15> stages{{{"st_a1_1.ecl",1},{"st_a1_2.ecl",2},{"st_a1_3.ecl",3},{"st_a2_2.ecl",2},{"st_a2_3.ecl",3},{"st_b1_1.ecl",1},{"st_b1_2.ecl",2},{"st_b1_3.ecl",3},{"st_b2_2.ecl",2},{"st_b2_3.ecl",3},{"st_c1_1.ecl",1},{"st_c1_2.ecl",2},{"st_c1_3.ecl",3},{"st_c2_2.ecl",2},{"st_c2_3.ecl",3}}};
 constexpr int expected[3][4]={{100000,200000,300000,400000},{200000,300000,400000,500000},{300000,400000,500000,600000}};
 constexpr int opcodes[5]={422,428,437,438,439};
 for(unsigned i=0;i<stages.size();++i)for(int rank=0;rank<4;++rank){const auto s=stages[i];const int op=opcodes[(i+rank)%5];run(std::string(s.name)+"-rank"+std::to_string(rank),[&]{th128::StageWorld w;load(w,s.name,op);execute(w,s.name,rank,expected[s.depth-1][rank],op);});}
 run("EX-flat500000",[]{th128::StageWorld w;load(w,"st_ex.ecl",428,true);execute(w,"st_ex.ecl",4,500000);});
 for(const auto name:{"component.ecl","st_fake_3.ecl"})run(std::string("unknown-name-")+name,[&]{th128::StageWorld w;load(w,name,428);execute(w,name,2,300000);});
 run("reuse-third-to-first",[]{th128::StageWorld w;load(w,"st_a1_3.ecl",428);load(w,"st_a1_1.ecl",428);execute(w,"st_a1_3.ecl",1,400000);execute(w,"st_a1_1.ecl",1,200000);});
 run("failed-start-then-second",[]{th128::StageWorld w;load(w,"st_a1_3.ecl",428);load(w,"st_a1_2.ecl",428);execute(w,"st_a1_3.ecl",1,400000,428,true);execute(w,"st_a1_2.ecl",1,300000);});
 run("explicit-reset-to-generic",[]{th128::StageWorld w;load(w,"st_a1_3.ecl",428);execute(w,"st_a1_3.ecl",1,400000);w.reset();load(w,"component.ecl",428);execute(w,"component.ecl",1,200000);});
 std::cout<<"{\"summary\":true,\"checks\":"<<checks<<",\"failed\":"<<failures<<",\"WorldStarts\":"<<worldStarts<<",\"worldTickCalls\":"<<worldTicks<<",\"syntheticECL\":true,\"syntheticEffectCallback\":true,\"originalGameplay\":false}\n";
 return failures?1:0;
}
