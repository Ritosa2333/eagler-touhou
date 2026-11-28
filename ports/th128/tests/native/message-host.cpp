#include "game/MessageHost.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
using namespace ecl_fixture;
int main(int count,char** arguments){try{
    require(count==2,"original st_a1.msg path required");std::ifstream file(arguments[1],std::ios::binary);require(bool(file),"missing original MSG");
    Bytes bytes{std::istreambuf_iterator<char>(file),{}};th128::message::Host host;std::string error;
    require(host.load(bytes,error),error);std::vector<th128::message::Event> events;
    require(host.setEventHost([&](const th128::message::Event& event,std::string&){events.push_back(event);return true;},error),error);
    require(host.start(0,error),error);require(host.state().active&&!host.state().eclReleased,"authored message starts with a closed ECL gate");
    for(int tick=0;tick<35;++tick)require(host.tick({},error),error);
    require(std::count_if(events.begin(),events.end(),[](const auto& event){return event.opcode==17;})==2,"real34 tick speech lines must execute");
    require(host.state().textLines.size()==2&&host.state().textLines[0]==std::vector<unsigned char>({0x82,0xb3,0x81,0x5b,0x82,0xc1,0x82,0xc4,0x82,0xc6}),"raw CP932 line must decode the original XOR stream");
    for(int tick=0;tick<6;++tick)require(host.tick({},error),error);
    require(host.state().waiting&&host.state().clock==38,"MSG11 holds the same command and clock while its600 timer counts down");
    require(host.tick({1,1,0},error),error);
    require(!host.state().waiting&&host.state().clock==39,"fresh Z press consumes actual11 wait and advances exactly once");
    for(int tick=0;tick<15000&&host.state().active;++tick)require(host.tick({},error),error);
    require(!host.state().active&&host.state().eclReleased,"authored0 must finish after all real waits");
    require(std::any_of(events.begin(),events.end(),[](const auto& event){return event.opcode==12;})&&std::any_of(events.begin(),events.end(),[](const auto& event){return event.opcode==19;}),"actual ECL-release and boss music events are not skipped");
    const auto previous=host.state();auto bad=bytes;bad.resize(12);require(!host.load(bad,error),"truncated MSG must reject");require(host.state().clock==previous.clock,"rejected load retains current state");
    th128::message::Host failure;require(failure.load(bytes,error),error);unsigned calls=0;
    require(failure.setEventHost([&](const auto&,std::string& reason){++calls;reason="owned dialogue event failed";return false;},error),error);
    require(!failure.start(0,error)&&calls==1,"constructor failures remain visible");require(!failure.tick({},error)&&calls==1,"failed dialogue must not retry callbacks");
    std::cout<<"actual MSG clocks, wait11, ECL release12, text XOR and sticky events passed\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
