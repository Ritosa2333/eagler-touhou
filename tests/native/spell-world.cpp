#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <iostream>
using namespace ecl_fixture;
int main(){try{
 auto title=stringWords("Original spell");std::uint8_t key=0x77,step=7;for(std::size_t index=4;index<title.size()*4;++index){const auto word=index/4,shift=(index%4)*8;const auto byte=std::uint8_t(title[word]>>shift);title[word]=(title[word]&~(255u<<shift))|(std::uint32_t(byte^key)<<shift);key=std::uint8_t(key+step);step=std::uint8_t(step+16);}
 std::vector<std::uint32_t> words{0,2280,500000};words.insert(words.end(),title.begin(),title.end());
 const auto data=program({{"main",join({instruction(411,{1200}),instruction(437,words,0,0,255,4),instruction(83,{4}),instruction(423),instruction(0,{},1000)})}});
 th128::StageWorld world;std::string error;require(world.load("spell.ecl",data.data(),data.size(),error)&&world.start("spell.ecl",1,0),error);require(world.setPhaseSpellContext({0,0,0,0},error),error);
 unsigned starts=0,ends=0;require(world.setSpellHost([&](const th128::SpellEvent& event,std::string&){if(event.starting){++starts;require(event.state.id==1&&event.state.limit==2280&&event.state.value80==200000,"native spell fields");}else ++ends;return true;},error),error);
 require(world.enableLocalEnemyPasses(error),world.error());require(starts==1&&(world.entities()[0].lifeWords[5]&1u),"437 must initialize expanded spell HP");
 for(int tick=0;tick<8;++tick)require(world.tick(),world.error());require(ends==1&&!(world.phaseSpellContext()->flags&1u)&&!(world.entities()[0].lifeWords[5]&1u),"423 must own real end and leave nonspell damage mode");
 std::cout<<"real437/423 start, expanded HP, active lifetime and end ownership passed\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
