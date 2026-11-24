#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <iostream>
#include <limits>
using namespace ecl_fixture;
int main(){
    try{
    for(const std::int32_t value:{0,-1,1,2,8,17,std::numeric_limits<std::int32_t>::min(),std::numeric_limits<std::int32_t>::max()}){
        const auto data=program({{"main",join({instruction(407,{0,123}),instruction(407,{2,8}),instruction(408,{floatBits(48),floatBits(48)}),
            instruction(410,{std::uint32_t(value)}),instruction(0,{},100)})}});
        th128::StageWorld drops;std::string error;
        require(drops.load("base-drop.ecl",data.data(),data.size(),error)&&drops.start("base-drop.ecl",1,0),error);
        require(drops.enableLocalEnemyPasses(error),error);const auto state=drops.entities()[0];
        require(state.drop==value&&state.drops.words[0]==value&&state.drops.words[2]==8&&state.drops.width==48&&state.drops.height==48&&state.score==0,
            "410 only changes actual base drop, preserving additional drops and spread");
    }
    for(const int slot:{0,1,2,3,7}){
        auto spawn=stringWords("boss");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),100,0,0});
        const auto data=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(420),instruction(402,{0x1000}),instruction(0,{},100)})},
            {"boss",join({instruction(412,{std::uint32_t(slot)}),instruction(304,{floatBits(0),floatBits(1)}),instruction(83,{3}),
                instruction(412,{0xffffffffu}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string error;
        require(world.load("wait.ecl",data.data(),data.size(),error)&&world.start("wait.ecl",1,0),error);
        require(world.enableLocalEnemyPasses(error),error);
        if(slot<3){
            require(!(world.entities()[0].flags&0x1000),"registered first-three boss holds main continuation");
            bool movedWhileHeld=false,resumed=false;
            for(int tick=0;tick<12;++tick){
                require(world.tick(),world.error());const auto states=world.entities();
                if(!(states[0].flags&0x1000)&&states[1].x>0)movedWhileHeld=true;
                if(states[0].flags&0x1000){resumed=true;break;}
            }
            require(movedWhileHeld&&resumed,"real412 owner release resumes main while boss motion continues independently");
        }else require(world.entities()[0].flags&0x1000,"original420 ignores slots3 through7");
        require(world.randomCalls()==0,"boss wait consumes no game RNG");
    }
    {
        const auto data=program({{"main",join({instruction(411,{100}),instruction(427,{0,floatBits(50),0xffff8080u}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string error;
        require(world.load("hud-clear.ecl",data.data(),data.size(),error)&&world.start("hud-clear.ecl",1,0),error);
        require(world.enableLocalEnemyPasses(error),world.error());require(world.bossGauges().values[0][0].ratio==.5f,"authored marker must reach real HUD fields");
        require(world.prepareBossHudPass(error),error);require(world.bossGauges().values[0][0].ratio==0,"HUD pass clears absent-Boss markers before another encounter reuses slot0");
    }
    {
        const auto data=program({{"main",join({instruction(440,{0xffffffffu},0,1),instruction(411,{123}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string error;
        require(world.load("original440.ecl",data.data(),data.size(),error)&&world.start("original440.ecl",1,0),error);
        require(world.enableLocalEnemyPasses(error),world.error());require(world.entities()[0].life==123&&world.randomCalls()==0,"original440 dispatch is a genuine ignored instruction, not TH12 remaining-spells state");
    }
    {
        const auto data=program({{"main",join({instruction(420,{1}),instruction(402,{0x1000}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string error;
        require(world.load("bad.ecl",data.data(),data.size(),error)&&world.start("bad.ecl",1,0),error);
        require(!world.enableLocalEnemyPasses(error)&&world.faulted(),"malformed420 cannot be a silent successful wait");
        require(!(world.entities()[0].flags&0x1000),"bad420 does not execute continuation");
    }
    std::cout<<"actual412 boss ownership gates420, other motion continues, cancellation releases main\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
