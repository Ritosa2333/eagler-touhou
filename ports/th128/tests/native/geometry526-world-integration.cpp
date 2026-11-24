#include "game/StageWorld.hpp"
#include "game/Geometry526Host.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iterator>
#include <iostream>

using namespace ecl_fixture;
int main(int count,char** arguments){
    require(count==2,"actual text resource argument required");
    std::ifstream input(arguments[1],std::ios::binary);
    const std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(input),{}};
    const auto data=program({{"main",join({instruction(526,{floatBits(80),0x00800080u}),
        instruction(526,{floatBits(120),0xff112233u},4),instruction(526,{floatBits(0),0},8),instruction(0,{},100)})}});
    std::string error;
    th128::StageWorld unsupported;
    require(unsupported.load("unsupported.ecl",data.data(),data.size(),error)&&unsupported.start("unsupported.ecl",1,0),error);
    require(!unsupported.enableLocalEnemyPasses(error)&&unsupported.error().find("unregistered host opcode 526")!=std::string::npos,
        "without an owning text host526 must remain an explicit stop");
    th128::StageWorld world;th128::geometry526::Host host;
    require(host.load(bytes,error),error);
    require(world.load("geometry.ecl",data.data(),data.size(),error)&&world.start("geometry.ecl",1,0),error);
    require(world.setGeometryHost(
        [&](std::uint32_t owner,float radius,std::uint32_t color,std::string& reason){return host.create(owner,radius,color,reason);},
        [&](std::uint32_t owner,const std::array<float,3>& position,std::string& reason){return host.update(owner,position,1,reason);},
        [&](std::uint32_t owner,std::string& reason){return host.removeOwner(owner,reason);},error),error);
    require(world.enableLocalEnemyPasses(error),error);
    require(host.effectCount()==1&&host.nodeCount()==16&&host.effectState(1)->scale==18,"real creation is followed by the native geometry update in the same local pass");
    const auto first=host.snapshots(error);require(bool(first),error);const auto firstIdentity=first->front().id;
    require(world.tick()&&host.effectState(1)->scale==18,"constructor guard does not double-update geometry");
    bool replaced=false,retired=false;
    for(int tick=0;tick<14;++tick){
        require(world.tick(),world.error());
        if(host.effectState(1)&&host.effectState(1)->radius==120){
            replaced=true;require(host.snapshots(error)->front().id>firstIdentity,"replacement retires original rows before binding new text82 rows");
        }
        if(replaced&&host.effectCount()==0){retired=true;break;}
    }
    require(replaced&&retired&&host.nodeCount()==0,"zero radius actually retires the owning rows, not an empty successful opcode");
    require(world.randomCalls()==0,"geometry must not invent game RNG calls");
    world.reset();require(host.nodeCount()==0,"reset leaves no detached geometry rows");
    std::cout<<"owning526 ECL creation/update/replacement/zero-radius retirement and constructor guard passed\n";
}
