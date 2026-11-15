#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace ecl_fixture;

static std::vector<std::uint32_t> spawn(const char* name) {
    auto words=stringWords(name);
    words.insert(words.end(),{floatBits(-32),floatBits(0),100,0,0});
    return words;
}

static Bytes mainProgram(bool write,float first,float second,bool lateWrite=false) {
    std::vector<Bytes> commands;
    if(write){
        commands.push_back(instruction(44,{floatBits(first)},0,0,255,1));
        commands.push_back(instruction(45,{floatBits(-9979)},0,1,255,1));
        commands.push_back(instruction(44,{floatBits(second)},0,0,255,1));
        commands.push_back(instruction(45,{floatBits(-9978)},0,1,255,1));
        commands.push_back(instruction(42,{std::uint32_t(-9979)},0,1,255,1));
        commands.push_back(instruction(42,{std::uint32_t(-9978)},0,1,255,1));
    }
    commands.push_back(instruction(257,spawn("GirlA2"),0,0,255,6));
    commands.push_back(instruction(257,spawn("GirlB2"),0,0,255,6));
    if(lateWrite){
        commands.push_back(instruction(44,{floatBits(1.25f)},0,0,255,1));
        commands.push_back(instruction(45,{floatBits(-9979)},0,1,255,1));
        commands.push_back(instruction(44,{floatBits(-1.25f)},0,0,255,1));
        commands.push_back(instruction(45,{floatBits(-9978)},0,1,255,1));
    }
    commands.push_back(instruction(0,{},200));
    return join(commands);
}

static void configure(th128::StageWorld& world,std::string& error) {
    assert(world.setAppearanceBoundsProvider([](int,int){return th128::enemy::Bounds{64,64};},error));
    if(!world.enableLocalEnemyPasses(error)){
        std::cerr<<world.error()<<' '<<error<<'\n';
        std::exit(1);
    }
}

static std::vector<th128::EnemySnapshot> run(bool write,float first,float second,bool lateWrite=false) {
    const auto data=program({{"main",mainProgram(write,first,second,lateWrite)},
        {"GirlA2",join({instruction(304,{floatBits(-9979),floatBits(2)},0,1,255,2),instruction(0,{},200)})},
        {"GirlB2",join({instruction(305,{100,0,floatBits(-9978),floatBits(3)},0,4,255,4),instruction(0,{},200)})}});
    th128::StageWorld world;
    std::string error;
    assert(world.load("global9979.ecl",data.data(),data.size(),error));
    assert(world.start("global9979.ecl",1,0));
    configure(world,error);
    for(int tick=0;tick<105;++tick){
        if(!world.tick()){
            std::cerr<<"tick "<<tick<<": "<<world.error()<<'\n';
            std::exit(1);
        }
    }
    return world.entities();
}

int main() {
    const auto first=run(true,-0.7853981852531433f,0.39269909262657166f);
    assert(first.size()==3);
    assert(first[1].speed==2.0f&&std::fabs(first[1].angle+0.7853981852531433f)<1e-5f);
    assert(first[2].speed==3.0f&&std::fabs(first[2].angle-0.39269909262657166f)<1e-5f);
    const auto reset=run(false,0,0);
    assert(reset.size()==3&&reset[1].speed==2&&reset[2].speed==3&&reset[1].angle==0&&reset[2].angle==0);
    const auto isolated=run(true,-0.7853981852531433f,0.39269909262657166f,true);
    assert(isolated.size()==3);
    assert(std::fabs(isolated[1].angle+0.7853981852531433f)<1e-5f);
    assert(std::fabs(isolated[2].angle-0.39269909262657166f)<1e-5f);
    std::cout<<"PASS actual opcode45 main writes both signed globals; inherited GirlA2/B2 reads304/305; reset zero; post-spawn parent changes isolated\n";
}
