#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace ecl_fixture;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream file(path,std::ios::binary);
    if(!file){std::fprintf(stderr,"missing %s\n",path);std::exit(2);}
    return {std::istreambuf_iterator<char>(file),{}};
}
static void requireWorld(bool condition,const std::string& message) {
    if(!condition)throw std::runtime_error(message);
}
static std::string stateText() {
    return th128_world_state();
}
static std::string drawText() {
    const char* draws=th128_world_bullet_draws();
    return draws?draws:"";
}
static void loadWorld(const std::string& name,const std::vector<unsigned char>& data) {
    th128_world_reset();
    requireWorld(th128_world_load(name.c_str(),std::uint32_t(name.size()),data.data(),std::uint32_t(data.size())),
        th128_world_error());
    requireWorld(th128_world_start(name.c_str(),std::uint32_t(name.size()),1,1),th128_world_error());
}
static std::vector<unsigned char> stageProgram(int color) {
    return program({{"main",join({
        instruction(0,{},1),
        instruction(500,{0}),
        instruction(502,{0,30,static_cast<unsigned>(color)}),
        instruction(503,{0,floatBits(0),floatBits(0)}),
        instruction(505,{0,floatBits(0),floatBits(0)}),
        instruction(506,{0,1,1}),
        instruction(501,{0}),
        instruction(0,{},120)
    })}});
}
static void configureAssets(const std::vector<unsigned char>& enemy,
                            const std::vector<unsigned char>& profile8,
                            const std::vector<unsigned char>& bullet,
                            const std::vector<unsigned char>& type30,
                            const std::vector<unsigned char>& shot) {
    requireWorld(th128_world_load_enemy_anm(1,enemy.data(),std::uint32_t(enemy.size())),th128_world_error());
    requireWorld(th128_world_enable_bullets(profile8.data(),std::uint32_t(profile8.size()),
        bullet.data(),std::uint32_t(bullet.size()),0,400),th128_world_error());
    requireWorld(th128_world_add_bullet_profile(30,type30.data(),std::uint32_t(type30.size())),
        th128_world_error());
    requireWorld(th128_world_enable_bullet_updates(shot.data(),std::uint32_t(shot.size())),
        th128_world_error());
}
static void runColor(const std::string& name,const std::vector<unsigned char>& data,
                     const std::vector<unsigned char>& enemy,const std::vector<unsigned char>& profile8,
                     const std::vector<unsigned char>& bullet,const std::vector<unsigned char>& type30,
                     const std::vector<unsigned char>& shot) {
    loadWorld(name,data);
    configureAssets(enemy,profile8,bullet,type30,shot);
    requireWorld(th128_world_tick(),"authored pre-emission wait tick");
    requireWorld(th128_world_tick(),th128_world_error());
    requireWorld(th128_world_tick(),std::string("authored first post-birth Host tick: ")+th128_world_error());
    const auto state=stateText();
    requireWorld(state.find("\"type\":30")!=std::string::npos,"missing live type30 state");
    requireWorld(state.find("\"script\":285")!=std::string::npos,"missing primary285 state");
    const auto lifeAt=state.find("\"lifeClock\":");
    requireWorld(lifeAt!=std::string::npos,"missing lifeClock state");
    const int lifeClock=std::stoi(state.substr(lifeAt+12));
    requireWorld(lifeClock>0,"first authored Host tick must own nonzero lifeClock; state="+state);
    requireWorld(state.find("\"freezePolicy\":1")!=std::string::npos,"missing freezePolicy1");
    const auto draws=drawText();
    requireWorld(draws.find("\"script\":286")!=std::string::npos,"missing secondary286 draw; "+draws);
    requireWorld(draws.find("\"drawPass\":\"hostile-bullets\"")!=std::string::npos,"missing hostile draw pass");
    requireWorld(draws.find("\"drawPass\":\"hostile-bullets\"")!=draws.rfind("\"drawPass\":\"hostile-bullets\""),
        "expected primary and secondary hostile draws; "+draws);
}
int main(int argc,char** argv) {
    try {
        requireWorld(argc==6,"usage: type30-worldexports <type30> <profile8> <bullet.anm> <enemy.anm> <pl00a.sht>");
        const auto type30=readFile(argv[1]),profile8=readFile(argv[2]),bullet=readFile(argv[3]);
        const auto enemy=readFile(argv[4]),shot=readFile(argv[5]);
        requireWorld(type30.size()==288,"type30 profile bytes");
        requireWorld(!shot.empty(),"pl00a.sht must be present");
        const auto data=stageProgram(6);
        const auto dataColor0=stageProgram(0);
        const std::string name="st_a1_1.ecl";
        th128_world_reset();

        loadWorld(name,data);
        requireWorld(th128_world_load_enemy_anm(1,enemy.data(),std::uint32_t(enemy.size())),th128_world_error());
        requireWorld(th128_world_enable_bullets(profile8.data(),std::uint32_t(profile8.size()),
            bullet.data(),std::uint32_t(bullet.size()),0,400),th128_world_error());
        requireWorld(th128_world_enable_bullet_updates(shot.data(),std::uint32_t(shot.size())),th128_world_error());
        requireWorld(th128_world_tick(),"authored pre-emission wait tick");
        requireWorld(!th128_world_tick(),"missing type30 profile must reject before visual creation");
        requireWorld(std::string(th128_world_error()).find("profile")!=std::string::npos,
            std::string("missing type30 error: ")+th128_world_error());

        runColor(name,data,enemy,profile8,bullet,type30,shot);
        th128_world_reset();
        runColor(name,dataColor0,enemy,profile8,bullet,type30,shot);
        th128_world_reset();

        const auto bad=[](std::vector<unsigned char> bytes) {
            bytes[0x11c]=0;
            return bytes;
        };
        loadWorld(name,data);
        requireWorld(th128_world_load_enemy_anm(1,enemy.data(),std::uint32_t(enemy.size())),th128_world_error());
        requireWorld(th128_world_enable_bullets(profile8.data(),std::uint32_t(profile8.size()),
            bullet.data(),std::uint32_t(bullet.size()),0,400),th128_world_error());
        requireWorld(th128_world_enable_bullet_updates(shot.data(),std::uint32_t(shot.size())),th128_world_error());
        auto wrong=bad(type30);
        requireWorld(!th128_world_add_bullet_profile(30,wrong.data(),std::uint32_t(wrong.size())),
            "wrong type30 secondary must be rejected");
        requireWorld(th128_world_tick(),"wrong-pair pre-emission wait tick");
        requireWorld(!th128_world_tick(),"wrong type30 profile must not create a Visual");
        requireWorld(std::string(th128_world_error()).find("profile")!=std::string::npos,
            std::string("wrong pair error: ")+th128_world_error());
        th128_world_reset();
        std::puts("PASS actual WorldExports type30 constructor285/286 draw/state/life/reset/0+6/missing/wrongpair");
    }catch(const std::exception& error) {
        std::fprintf(stderr,"FAIL %s\n",error.what());return 1;
    }
}

