#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

std::vector<unsigned char> readFile(const std::string& path) {
    std::ifstream stream(path, std::ios::binary);
    assert(stream);
    return {(std::istreambuf_iterator<char>(stream)), {}};
}

int main(int argc, char** argv) {
    assert(argc > 3);
    th128::StageWorld world;
    std::string error;
    for (int index = 1; index < argc - 1; ++index) {
        const std::string path = argv[index];
        const auto bytes = readFile(path);
        assert(world.load(path.substr(path.find_last_of("/\\") + 1), bytes.data(), bytes.size(), error));
    }
    const auto profile = readFile(argv[argc - 1]);
    {
        using namespace ecl_fixture;
        const auto script=program({{"main",join({instruction(304,{floatBits(-9989),floatBits(4)},0,1),instruction(0,{},100)})}});
        th128::StageWorld aim;
        assert(aim.load("aim.ecl",script.data(),script.size(),error)&&aim.start("aim.ecl",1,34772));
        assert(aim.configureBullets(profile.data(),profile.size(),{0,400,0},[](const th128::bullet::Birth&,std::string&){return true;},error));
        assert(aim.enableLocalEnemyPasses(error));
        assert(aim.entities()[0].angle==1.5707963705062866f&&aim.entities()[0].speed==4&&aim.randomCalls()==0);
        th128::StageWorld absentPlayer;
        assert(absentPlayer.load("aim.ecl",script.data(),script.size(),error)&&absentPlayer.start("aim.ecl",1,34772));
        assert(!absentPlayer.enableLocalEnemyPasses(error)&&error.find("-9989")!=std::string::npos);
    }
    assert(world.start("st_a1_1.ecl", 1, 34772));
    unsigned constructors = 0;
    assert(world.configureBullets(profile.data(), profile.size(), {0, 400, 0},
        [&constructors](const th128::bullet::Birth& birth, std::string&) {
            ++constructors;
            assert(birth.script == 43 && birth.type == 8 && birth.color == 10);
            assert(birth.state.motionState == 2 && birth.state.animationInterrupt == 8);
            return true;
        }, error));
    for (unsigned step = 0; step < 300 && world.error().empty(); ++step) world.tick();
    assert(constructors == 1);
    assert(world.bullets().birthCount() == 1);
    const auto birth = world.bullets().snapshots().at(0);
    assert(birth.state.position.z == .1f);
    assert(world.error().find("bullet update") != std::string::npos);
    assert(world.error().find("opcode=501") == std::string::npos);
    std::cout << "ACTUAL first emission script=" << birth.script << " type=" << birth.type << " color=" << birth.color
              << " position=" << birth.state.position.x << ',' << birth.state.position.y
              << " frame=" << world.frame() << " stop=" << world.error() << '\n';
    assert(world.start("st_a1_1.ecl", 1, 34772));
    assert(world.configureBullets(profile.data(), profile.size(), {0, 400, 0},
        [&world](const th128::bullet::Birth&, std::string&) { world.reset(); return true; }, error));
    for (unsigned step = 0; step < 300 && world.error().empty(); ++step) world.tick();
    assert(world.error().find("world reset is not reentrant") != std::string::npos);
    assert(world.entities().size() == 2);
    assert(world.start("st_a1_1.ecl", 1, 34772));
    assert(world.configureBullets(profile.data(), profile.size(), {0, 400, 0},
        [](const th128::bullet::Birth&, std::string&) { return true; }, error));
    assert(world.setAppearanceBoundsProvider([](int bank,int script)->std::optional<th128::enemy::Bounds>{
        if(bank==1&&script==0)return th128::enemy::Bounds{32,32};
        return std::nullopt;
    },error));
    assert(world.enableLocalEnemyPasses(error));
    assert(world.positionIntegrated());
    for (unsigned step = 0; step < 300 && world.error().empty(); ++step) world.tick();
    assert(world.bullets().birthCount() == 1);
    assert(world.error().find("bullet update") != std::string::npos);
    const auto moved = world.entities().at(1);
    assert(moved.x < -128 && moved.y > -32);
    assert(world.bullets().snapshots().at(0).state.position.y > -32);
    std::cout << "MOTION first emission enemy=" << moved.x << ',' << moved.y << " frame=" << world.frame() << '\n';
    world.reset();
    using namespace ecl_fixture;
    const auto bounded=program({{"main",join({instruction(402,{0x20000}),instruction(304,{floatBits(0),floatBits(1000)}),
        instruction(402,{0x1000},1),instruction(1,{},100)})}});
    assert(world.load("bounds.ecl",bounded.data(),bounded.size(),error));
    assert(world.start("bounds.ecl",1,34772)&&world.enableLocalEnemyPasses(error));
    assert(world.tick());
    assert(!world.tick());
    assert(world.error().find("stage controller transition")!=std::string::npos);
    assert(!world.entities().at(0).active);
    assert((world.entities().at(0).flags&0x1000U)==0);
    assert(world.start("st_a1_1.ecl",1,34772));
    assert(world.configureBullets(profile.data(),profile.size(),{0,400,0},
        [](const th128::bullet::Birth&,std::string&){return true;},error));
    assert(world.setAppearanceBoundsProvider([](int bank,int script)->std::optional<th128::enemy::Bounds>{
        if(bank==1&&script==0)return th128::enemy::Bounds{32,32};return std::nullopt;
    },error));
    unsigned bodyUpdates=0;
    assert(world.configureBulletUpdater([&bodyUpdates](th128::bullet::Birth&,bool&,std::string& reason){
        ++bodyUpdates;reason="explicit first bullet update boundary";return false;
    },error));
    assert(world.enableLocalEnemyPasses(error));
    for(unsigned step=0;step<300&&world.error().empty();++step)world.tick();
    assert(bodyUpdates==1&&world.frame()==115);
    assert(world.error()=="explicit first bullet update boundary");
    world.reset();
    const auto exempt=program({{"main",join({instruction(402,{0x20004}),instruction(304,{floatBits(th128::enemy::Pi/4),floatBits(1414)}),
        instruction(402,{0x1000},1),instruction(1,{},100)})}});
    assert(world.load("exempt.ecl",exempt.data(),exempt.size(),error));
    assert(world.start("exempt.ecl",1,34772)&&world.enableLocalEnemyPasses(error));
    assert(world.tick()&&world.tick());
    assert((world.entities().at(0).flags&0x1000U)!=0);
    world.reset();
    const auto unknownAppearance=program({{"main",join({instruction(258,{1}),instruction(262,{0,0}),
        instruction(402,{0x1000},1),instruction(1,{},100)})}});
    assert(world.load("unknown-appearance.ecl",unknownAppearance.data(),unknownAppearance.size(),error));
    assert(world.start("unknown-appearance.ecl",1,34772)&&world.enableLocalEnemyPasses(error));
    assert(world.tick()&&!world.tick());
    assert(world.error().find("appearance bounds")!=std::string::npos);
    assert((world.entities().at(0).flags&0x1000U)==0);
    world.reset();
    const auto removal=program({{"main",join({instruction(402,{0x4000000}),instruction(1,{},100)})}});
    assert(world.load("removal.ecl",removal.data(),removal.size(),error));
    assert(world.start("removal.ecl",1,34772)&&world.enableLocalEnemyPasses(error));
    assert(!world.tick()&&!world.entities().at(0).active);
    assert(world.error().find("stage controller transition")!=std::string::npos);
    return 0;
}
