#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cmath>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace ecl_fixture;

namespace {
std::vector<unsigned char> read(const char* path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("missing player profile");
    return {std::istreambuf_iterator<char>(input), {}};
}
Bytes simpleEmissionProgram() {
    return join({instruction(40,{16}),instruction(411,{7}),instruction(500,{0}),instruction(502,{0,8,10}),instruction(501,{0}),
        instruction(411,{8}),instruction(1)});
}
Bytes expressionProgram(bool integerRead=false) {
    if(integerRead) {
        const std::int32_t target=44;
        return join({
            instruction(40,{16}),
            instruction(411,{1}),
            instruction(42,{std::uint32_t(-9944)},0,1,255,1),
            instruction(42,{400},0,0,255,1),
            instruction(59),
            instruction(14,{static_cast<std::uint32_t>(target),0},0,0,255,2),
            instruction(411,{2}),
            instruction(1)
        });
    }
    return join({
        instruction(40,{16}),
        instruction(44,{floatBits(.5f)}),
        instruction(45,{floatBits(0)},0,1),
        instruction(44,{floatBits(-9944)},0,1),
        instruction(44,{floatBits(0)},0,1),
        instruction(55),
        instruction(45,{floatBits(8)},0,1),
        instruction(44,{floatBits(-9989)},0,1),
        instruction(45,{floatBits(4)},0,1),
        instruction(500,{0}),
        instruction(502,{0,8,10}),
        instruction(523,{0,floatBits(4),floatBits(8)},0,6,255,3),
        instruction(501,{0}),
        instruction(1)
    });
}
}

int main(int argc,char** argv) {
    try {
        require(argc==2,"usage: ecl-global-9944 profile8.bin");
        const auto playerProfile=read(argv[1]);
        std::vector<th128::bullet::Birth> births;
        std::string error;
        {
            th128::StageWorld world;
            const auto data=program({{"main",simpleEmissionProgram()}});
            require(world.load("global9944-simple.ecl",data.data(),data.size(),error),error);
            require(world.start("global9944-simple.ecl",1,0),error);
            require(world.configureBullets(playerProfile.data(),playerProfile.size(),{0,400,0},
                [](const th128::bullet::Birth&,std::string&){return true;},error),error);
            require(world.tick(),world.error());
            require(world.bullets().birthCount()==1,std::string("simple 502/501 emission failed: births=")
                +std::to_string(world.bullets().birthCount())+" life="
                +std::to_string(world.entities().empty()?-1:world.entities()[0].life)+" err="+world.error());
            require(world.entities().size()==1&&world.entities()[0].life==8,
                "simple ECL life marker must execute through 501");
        }
        {
            th128::StageWorld world;
            const auto data=program({{"main",expressionProgram(false)}});
            require(world.load("global9944-expression.ecl",data.data(),data.size(),error),error);
            require(world.start("global9944-expression.ecl",1,0),error);
            require(world.configureBullets(playerProfile.data(),playerProfile.size(),{0,400,0},
                [&](const th128::bullet::Birth& birth,std::string&){births.push_back(birth);return true;},error),error);
            require(world.tick(),world.error());
            require(births.size()==1,"the actual 523 expression must emit one type8 bullet; births="
                +std::to_string(world.bullets().birthCount())+" err="+world.error());
            require(std::abs(births[0].state.position.x)<1e-4f&&std::abs(births[0].state.position.y-200.f)<1e-3f,
                "actual 523 expression must use -9944 distance 400 times B .5 at angle -9989");
            require(world.entities().size()==1,"root survives the expression");
        }
        {
            th128::StageWorld world;
            const auto data=program({{"main",expressionProgram(true)}});
            require(world.load("global9944-int.ecl",data.data(),data.size(),error),error);
            require(world.start("global9944-int.ecl",1,0),error);
            require(world.configureBullets(playerProfile.data(),playerProfile.size(),{0,400,0},
                [](const th128::bullet::Birth&,std::string&){return true;},error),error);
            require(world.tick(),world.error());
            require(world.entities().size()==1&&world.entities()[0].life==1,
                "integer -9944 must truncate to 400 and take the equality branch");
        }
        {
            th128::StageWorld cold;
            const auto data=program({{"main",expressionProgram(false)}});
            require(cold.load("global9944-cold.ecl",data.data(),data.size(),error),error);
            require(cold.start("global9944-cold.ecl",1,0),error);
            require(!cold.tick()&&cold.error().find("-9944")!=std::string::npos,
                "cold context must reject -9944 instead of inventing a value");
        }
        {
            th128::StageWorld reset;
            const auto data=program({{"main",expressionProgram(false)}});
            require(reset.load("global9944-reset.ecl",data.data(),data.size(),error),error);
            require(reset.start("global9944-reset.ecl",1,0),error);
            require(reset.configureBullets(playerProfile.data(),playerProfile.size(),{0,400,0},
                [](const th128::bullet::Birth&,std::string&){return true;},error),error);
            reset.reset();
            require(reset.load("global9944-reset.ecl",data.data(),data.size(),error),error);
            require(reset.start("global9944-reset.ecl",1,0),error);
            require(!reset.tick()&&reset.error().find("-9944")!=std::string::npos,
                "reset must clear the player context and reject -9944");
        }
        std::puts("PASS actual ECL44 -9944 expression/523/int/cold/reset");
    }catch(const std::exception& error){
        std::fprintf(stderr,"%s\n",error.what());
        return 1;
    }
}
