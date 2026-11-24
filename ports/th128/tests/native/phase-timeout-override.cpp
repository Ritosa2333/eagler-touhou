#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <iostream>
using namespace ecl_fixture;
int main(){try{
    auto registration=std::vector<std::uint32_t>{0,50,1};const auto health=stringWords("health");registration.insert(registration.end(),health.begin(),health.end());
    auto overrideWords=std::vector<std::uint32_t>{0};const auto escape=stringWords("escape");overrideWords.insert(overrideWords.end(),escape.begin(),escape.end());
    const auto data=program({{"main",join({instruction(411,{100}),instruction(414,registration,0,0,255,4),instruction(421,overrideWords,0,0,255,2),instruction(0,{},100)})},
        {"health",join({instruction(411,{10}),instruction(0,{},100)})},{"escape",join({instruction(411,{200}),instruction(0,{},100)})}});
    th128::StageWorld world;std::string error;require(world.load("override.ecl",data.data(),data.size(),error)&&world.start("override.ecl",1,0),error);
    require(world.setPhaseSpellContext({0,0,0,0},error),error);require(world.enableLocalEnemyPasses(error),world.error());
    const auto record=world.entities()[0].phases[0];require(record.healthTarget=="health"&&record.timeTarget=="escape","421 must change only the time target");
    for(int tick=0;tick<4;++tick)require(world.tick(),world.error());
    require(world.entities()[0].life==200,"real timeout must schedule escape instead of health continuation");
    std::cout<<"native421 timeout override preserves health ownership and executes real escape entry\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
