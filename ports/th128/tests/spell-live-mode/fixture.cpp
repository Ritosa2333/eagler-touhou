#include "game/StageWorld.hpp"
#include "ecl-fixture.hpp"
#include <iostream>
using namespace ecl_fixture;
unsigned checks=0,worlds=0,ticks=0;
std::vector<std::uint32_t> title(){auto t=stringWords("Live mode");std::uint8_t key=0x77,step=7;for(std::size_t n=4;n<t.size()*4;++n){unsigned i=n/4,s=n%4*8;const auto v=std::uint8_t(t[i]>>s);t[i]=(t[i]&~(255u<<s))|(std::uint32_t(v^key)<<s);key=std::uint8_t(key+step);step=std::uint8_t(step+16);}return t;}
void one(int initial,bool extra){
 const char* file=extra?"st_ex.ecl":"mode-component.ecl";auto words=std::vector<std::uint32_t>{extra?240u:70u,2280,500000};const auto name=title();words.insert(words.end(),name.begin(),name.end());
 const auto data=program({{"main",join({instruction(411,{1200}),instruction(428,words,0,0,255,4),instruction(0,{},1000)})}});
 th128::StageWorld w;std::string error;require(w.load(file,data.data(),data.size(),error)&&w.start(file,extra?4:0,34772),error);++worlds;
 require(w.setPhaseSpellContext({0,0,0,initial},error),error);unsigned start=0;std::uint32_t beginFlags=0;
 require(w.setSpellHost([&](const th128::SpellEvent& e,std::string&){if(e.starting){++start;beginFlags=e.state.flags;}return true;},error),error);
 require(w.enableLocalEnemyPasses(error),error);require(start==1&&beginFlags==unsigned(initial?0x47:7),"actualbegin mustuse suppliedmode");++checks;
 w.setPhaseSpellPlayerMode(initial);require(w.phaseSpellContext()->playerMode==initial,"modeconsumer notupdated");++checks;
 const auto random=w.gameRandomState();require(w.tick(),w.error());++ticks;require(w.phaseSpellContext()->flags==unsigned(initial?0x47:7),"modeheld tick wronglyclears40");++checks;
 w.setPhaseSpellPlayerMode(0);require(w.tick(),w.error());++ticks;require(w.phaseSpellContext()->flags==7&&w.phaseSpellContext()->playerMode==0,"mode0 nexttickmustclear40");++checks;
 w.setPhaseSpellPlayerMode(1);require(w.tick(),w.error());++ticks;require(w.phaseSpellContext()->flags==7,"mode1 midspell mustnotfabricate40");++checks;
 require(w.gameRandomState().seed==random.seed&&w.gameRandomState().calls==random.calls,"modechangedRNG");++checks;
 w.reset();w.setPhaseSpellPlayerMode(1);require(!w.phaseSpellContext(),"setter mustnotcreatecontextafterreset");++checks;
}
int main(){try{for(bool extra:{false,true})for(int mode:{0,1,-1})one(mode,extra);require(checks==42&&worlds==6&&ticks==18,"coverage");std::cout<<"{\"passed\":true,\"checks\":42,\"WorldStarts\":6,\"ticks\":18,\"syntheticECL\":true,\"syntheticEffectCallback\":true,\"originalGameplay\":false}\n";}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
