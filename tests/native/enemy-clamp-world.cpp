#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <iostream>
using namespace ecl_fixture;
int main(){
    try{
        const auto data=program({{"main",join({instruction(404,{floatBits(0),floatBits(144),floatBits(288),floatBits(64)}),
            instruction(304,{floatBits(0),floatBits(100)}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string error;
        require(world.load("clamp.ecl",data.data(),data.size(),error)&&world.start("clamp.ecl",1,0),error);
        require(world.enableLocalEnemyPasses(error),error);require(world.entities()[0].y==0,"404 stores its area without moving during its ECL command");
        require(world.tick()&&world.entities()[0].y==0,"constructor guard does not apply an extra clamp pass");
        require(world.tick(),world.error());require(world.entities()[0].x==100&&world.entities()[0].y==112,"404 uses center plus half extents after combine");
        require(world.tick(),world.error());require(world.entities()[0].x==144&&world.entities()[0].y==112,"clamp updates the actual enemy position, not just drawing");
        require(world.tick(),world.error());require(world.entities()[0].x==144,"absolute rebuild keeps later motion clamped");
        require(world.randomCalls()==0,"404 movement area consumes no game RNG");
        const auto unsupported=program({{"main",join({instruction(402,{0x8000000}),instruction(0,{},100)})}});
        th128::StageWorld transform;
        require(transform.load("transform.ecl",unsupported.data(),unsupported.size(),error)&&transform.start("transform.ecl",1,0),error);
        require(transform.enableLocalEnemyPasses(error)&&transform.tick(),error);
        require(!transform.tick()&&transform.error().find("transform")!=std::string::npos,"unrelated transform flag must remain an explicit stop");
        std::cout<<"actual404 registration, post-combine center/half-size clamp and persistent absolute rebuild passed\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
