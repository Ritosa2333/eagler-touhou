#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cstdio>
#include <optional>
#include <string>
#include <vector>

using namespace ecl_fixture;

static std::vector<std::uint32_t> spawnWords(const std::string& subroutine,float x,float y,int life){
    auto words=stringWords(subroutine);
    words.insert(words.end(),{floatBits(x),floatBits(y),std::uint32_t(life),0,0});return words;
}

int main(){
    try{
    {
        const auto data=program({{"main",join({instruction(452,{2}),instruction(258,{1}),
            instruction(262,{0,99}),instruction(259,{1,100}),instruction(269,{2}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("layer.ecl",data.data(),data.size(),reason)&&world.start("layer.ecl",1,0),reason);
        std::vector<th128::AnimationBirth> births;
        require(world.setAnimationHost([&](const th128::AnimationBirth& birth,std::string&){births.push_back(birth);return true;},
            [](std::uint32_t){},reason),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        require(world.entities()[0].animationLayerOffset==2,"452 stores its integer in the entity snapshot");
        require(births.size()==3,"three authored animation births reached");
        require(births[0].request.layer==9&&births[0].request.script==99,
            "262 uses the stored 452 offset plus seven as the birth layer");
        require(births[1].request.layer==9&&births[1].request.script==100,
            "259 uses the same stored layer offset plus seven");
        require(births[2].request.layer==8&&births[2].request.script==104,
            "269 keeps its original fixed layer eight and base script plus five");
    }
    {
        const auto data=program({{"main",join({instruction(42,{3}),instruction(452,{0xffffffffu},0,1,255,1),
            instruction(258,{1}),instruction(262,{0,50}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;std::vector<th128::AnimationBirth> births;
        require(world.load("masked.ecl",data.data(),data.size(),reason)&&world.start("masked.ecl",1,0),reason);
        require(world.setAnimationHost([&](const th128::AnimationBirth& birth,std::string&){births.push_back(birth);return true;},
            [](std::uint32_t){},reason),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        require(world.entities()[0].animationLayerOffset==3,"452 resolves a reference-masked integer operand");
        require(births.size()==1&&births[0].request.layer==10,"the resolved layer offset reaches the next birth");
    }
    {
        const auto data=program({{"main",join({instruction(452,{},0,0,255,0),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("shape.ecl",data.data(),data.size(),reason)&&world.start("shape.ecl",1,0),reason);
        require(!world.enableLocalEnemyPasses(reason),"452 without exactly one integer operand is rejected");
        require(world.faulted()&&world.error().find("opcode 452")!=std::string::npos,world.error());
        require(world.entities()[0].animationLayerOffset==0,"rejected 452 does not pollute the layer offset");
    }
    {
        const auto data=program({{"main",join({instruction(529,{0}),instruction(257,spawnWords("child",0,100,20),0,0,255,6),
            instruction(0,{},100)})},{"child",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("clear.ecl",data.data(),data.size(),reason)&&world.start("clear.ecl",1,0),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        require(world.spawnCount()==1&&world.entities().size()==2,
            "529 selector zero is the original no-callback clear and advances");
    }
    {
        const auto data=program({{"main",join({instruction(42,{0}),instruction(529,{0xffffffffu},0,1,255,1),
            instruction(257,spawnWords("child",0,100,20),0,0,255,6),instruction(0,{},100)})},
            {"child",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("maskedclear.ecl",data.data(),data.size(),reason)&&world.start("maskedclear.ecl",1,0),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        require(world.spawnCount()==1,"529 resolves the reference-masked zero selector");
    }
    for(const std::int32_t selector:{2,3,-1,1000}){
        const auto data=program({{"main",join({instruction(529,{std::uint32_t(selector)}),
            instruction(257,spawnWords("child",0,100,20),0,0,255,6),instruction(0,{},100)})},
            {"child",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("callback.ecl",data.data(),data.size(),reason)&&world.start("callback.ecl",1,0),reason);
        require(!world.enableLocalEnemyPasses(reason),"unported 529 callback selectors are rejected");
        require(world.faulted()&&world.error().find("ECL529 native enemy-update callback selector is not ported")!=std::string::npos,
            world.error());
        require(world.spawnCount()==0,"a rejected 529 callback never executes the following spawn");
    }
    {
        const auto data=program({{"main",join({instruction(529,{}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("shape529.ecl",data.data(),data.size(),reason)&&world.start("shape529.ecl",1,0),reason);
        require(!world.enableLocalEnemyPasses(reason),"529 without its selector operand is rejected");
    }
    std::puts("PASS 452 layer offset consumer and bounded 529 clear selector");
    return 0;
    }catch(const std::exception& error){std::fprintf(stderr,"FAIL enemy stage fields: %s\n",error.what());return 1;}
}
