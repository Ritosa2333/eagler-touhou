#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <bit>
#include <iostream>
using namespace ecl_fixture;
int main(){
    try{
        auto spawn=stringWords("boss");spawn.insert(spawn.end(),{floatBits(0),floatBits(144),100,0,0});
        const auto data=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},100)})},
            {"boss",join({instruction(404,{floatBits(0),floatBits(144),floatBits(288),floatBits(64)}),
                instruction(312,{40,4,floatBits(2)}),instruction(0,{},100)})}});
        for(const bool playerContext:{false,true}){
            th128::StageWorld world;std::string error;
            require(world.load("random-move.ecl",data.data(),data.size(),error)&&world.start("random-move.ecl",1,0),error);
            if(playerContext)require(world.setPlayerAimPosition({-30,400,0},error),error);
            const bool started=world.enableLocalEnemyPasses(error);
            if(!playerContext){require(!started&&world.error().find("player")!=std::string::npos,"312 needs actual explicit player context");continue;}
            require(started,error);const auto born=world.entities()[1];
            require(born.x==0&&born.y==144&&born.angle==0&&born.speed==0,"312 pair setup must not move or replace polar values immediately");
            require(world.randomCalls()==3,"inside-quarter312 consumes original32-bit plus16-bit RNG calls");
            require(world.tick(),world.error());require(world.entities()[1].x==0,"duplicate constructor guard does not advance pair/movement");
            require(world.tick(),world.error());const auto moved=world.entities()[1];
            require(std::bit_cast<std::uint32_t>(moved.angle)==3203963541u,"actual selected direction matches native seed0/player-left sample");
            require(std::bit_cast<std::uint32_t>(moved.speed)==1072913449u,"actual first mode4 speed matches complete native pair helper");
            require(moved.x>0&&moved.y<144&&moved.y>=112,"actual movement uses chosen direction and404 bounds");
            require(world.randomCalls()==3,"pair ticks do not consume additional RNG");
        }
        std::cout<<"actual312 owning world uses explicit player, exact RNG/pair stores, guard and bounded motion\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
