#include "game/StageWorld.hpp"
#include "game/MessageHost.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
using namespace ecl_fixture;
int main(int count,char** arguments){try{
    require(count==2,"original MSG required");std::ifstream file(arguments[1],std::ios::binary);const Bytes msg{std::istreambuf_iterator<char>(file),{}};
    const auto data=program({{"main",join({instruction(418,{0}),instruction(419),instruction(411,{100}),instruction(0,{},10000)})}});
    th128::StageWorld world;th128::message::Host host;std::string error;require(host.load(msg,error),error);
    require(host.setEventHost([](const auto&,std::string&){return true;},error),error);
    require(world.load("message.ecl",data.data(),data.size(),error)&&world.start("message.ecl",1,0),error);
    require(world.setMessageHost([&](int entry,std::string& reason){return host.start(entry,reason);},[&](std::string&)->std::optional<bool>{return host.state().active&&!host.state().eclReleased;},
        [](th128::bullet::Birth&,std::string&){return true;},error),error);
    require(world.enableLocalEnemyPasses(error),world.error());require(world.entities()[0].life==0,"419 must not invent completed message after418");
    for(int tick=0;tick<12000&&world.entities()[0].life!=100;++tick){require(host.tick({},error),error);require(world.tick(),world.error());}
    require(world.entities()[0].life==100&&host.state().active,"MSG12 releases419 before the full dialogue ends");
    std::cout<<"real418/419 owning MSG gate releases on actual12 instead of fake callback completion\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
