#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
static std::vector<std::uint8_t> read(const std::string& path){std::ifstream file(path,std::ios::binary);assert(file);return {std::istreambuf_iterator<char>(file),{}};}
static bool has(const std::string& text,const std::string& needle){return text.find(needle)!=std::string::npos;}
static void require(bool ok,const std::string& message){if(!ok){std::cerr<<message<<'\n';std::exit(1);}}
static void configurePlayer(const std::vector<std::uint8_t>& pl00){
    const th128::player::hit::Timer zero{-1,0,0,0x4b6f58,1};
    const th128::player::hit::Timer state{7,8,8,0x4b6f58,1};
    const th128::player::hit::Timer protection{179,180,180,0x4b6f58,1};
    th128::player::HitHostSnapshot snapshot{{4,0,180},state,zero,protection,{true,0},{0,0,0},{true,0}};
    playerHit=std::make_unique<th128::player::PlayerHitHost>();
    std::string error;
    require(playerHit->configure(pl00.data(),pl00.size(),snapshot,
        [](int,th128::player::hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error),error);
    playerConfigured=true;playerMotion=false;playerWindowControl={true,0,0,0};playerHitControl={true,0};
    playerDeathConfigured=true;deathAwardSelector=-1;playerRespawnConfigured=true;
    respawnControl=th128::player::respawn::ControlState{};respawnControl.mode04=0;respawnControl.value08=0;
    motivationRaw=0;motivationFlag=0;playerInput={};
}
static void prepare(const std::string& directory,const std::vector<std::uint8_t>& pl00,
                    const std::vector<std::uint8_t>& profile){
    (void)directory;
    th128_world_reset();
    std::string error;
    const auto script=ecl_fixture::program({{"main",ecl_fixture::join({ecl_fixture::instruction(83,{100},0,0,255,1),ecl_fixture::instruction(12,{0xffffffecU,0},0,0,255,2)})}});
    require(th128_world_load("st_a1_1.ecl",11,script.data(),unsigned(script.size())),th128_world_error());
    require(th128_world_start("st_a1_1.ecl",11,1,34772),th128_world_error());
    require(world->configureBullets(profile.data(),profile.size(),{0,0,0},
        [](const th128::bullet::Birth&,std::string&){return true;},error),error);
    std::vector<th128::damagefield::Image> empty(th128::damagefield::Capacity);
    require(damageFields->configure(empty,0,error),error);
    configurePlayer(pl00);
}
static bool tickDeathAndRespawn(bool enabled,const std::string& directory,
                                const std::vector<std::uint8_t>& pl00,const std::vector<std::uint8_t>& profile){
    prepare(directory,pl00,profile);
    require(th128_world_set_debug_infinite_respawn(enabled?1:0),th128_world_error());
    for(int tick=0;tick<9;++tick){if(!th128_world_tick()){if(enabled)std::cerr<<"first death tick "<<tick<<": "<<th128_world_error()<<"\n";return false;}}
    if(playerHit->snapshot().gate.stateF78!=2)return false;
    for(int tick=0;tick<31;++tick){if(!th128_world_tick()){if(enabled)std::cerr<<"first respawn tick "<<tick<<": "<<th128_world_error()<<" debug="<<debugInfiniteRespawnRun<<" motivation="<<motivationRaw<<"\n";return false;}}
    if(enabled){
        for(int tick=0;tick<60;++tick){if(!th128_world_tick()){std::cerr<<"first entry tick "<<tick<<": "<<th128_world_error()<<"\n";return false;}}
        require(playerHit->snapshot().gate.stateF78==1,"debug respawn did not reach player1");
        require(respawnsCommitted==1,"debug respawn did not commit");
        require(deathsCommitted==1,"debug death did not commit");
        require(debugReviveRefills==1,"debug refill count");
        require(has(th128_world_state(),"\"lastRefillBefore\":-10000"),th128_world_state());
        require(has(th128_world_state(),"\"lastRefillAfter\":10000"),th128_world_state());
        configurePlayer(pl00);
        for(int tick=0;tick<9;++tick){if(!th128_world_tick()){std::cerr<<"second death tick "<<tick<<": "<<th128_world_error()<<"\n";return false;}}
        for(int tick=0;tick<31;++tick){if(!th128_world_tick()){std::cerr<<"second respawn tick "<<tick<<": "<<th128_world_error()<<"\n";return false;}}
        for(int tick=0;tick<60;++tick){if(!th128_world_tick()){std::cerr<<"second entry tick "<<tick<<": "<<th128_world_error()<<"\n";return false;}}
        require(playerHit->snapshot().gate.stateF78==1,"second debug respawn did not reach player1");
        require(respawnsCommitted==2,"second debug respawn did not commit");
        require(deathsCommitted==2,"second debug death did not commit");
        require(debugReviveRefills==2,"second debug refill count");
        return true;
    }
    require(th128_world_error()[0]!=0,"default-off should hit GameOver error");
    return false;
}
int main(int argc,char** argv){
    try{
        require(argc==3,"stage directory and bullet runtime profile required");
        const std::string directory=argv[1];
        const auto pl00=read(directory+"/raw/pl00.anm"),profile=read(argv[2]);
        worldTickExecuting=true;
        require(th128_world_set_debug_infinite_respawn(1)==0,"setter must reject during world tick");
        worldTickExecuting=false;
        require(!tickDeathAndRespawn(false,directory,pl00,profile),"default-off unexpectedly respawned");
        require(tickDeathAndRespawn(true,directory,pl00,profile),"enabled respawn failed");
        auto st=th128_world_state();
        require(has(st,"\"debug\":{\"infiniteRespawn\":true,\"runTainted\":true,\"reviveRefills\":2"),st);
        const std::string beforeInvalid=st;
        require(th128_world_set_debug_infinite_respawn(2)==0,"invalid 2 must fail");
        require(th128_world_set_debug_infinite_respawn(-1)==0,"invalid -1 must fail");
        require(th128_world_state()==beforeInvalid,"invalid toggles changed debug state");
        debugSetterExecuting=true;
        require(th128_world_set_debug_infinite_respawn(1)==0,"self-reentrant setter must fail");
        debugSetterExecuting=false;
        StageCarryRecord carry{};
        captureDebugCarry(carry);
        require(carry.debugInfiniteRespawn==1&&carry.debugRunTainted&&carry.debugReviveRefills==2,"capture debug carry fields");
        debugInfiniteRespawnRun=false;debugRunTainted=false;debugReviveRefills=0;
        restoreDebugCarry(carry);
        require(debugInfiniteRespawnRun&&debugRunTainted&&debugReviveRefills==2,"restore debug carry fields");
        carry.debugInfiniteRespawn=0;
        restoreDebugCarry(carry);
        require(!debugInfiniteRespawnRun&&debugRunTainted,"disabled carry must preserve taint");
        require(th128_world_set_debug_infinite_respawn(0),th128_world_error());
        st=th128_world_state();
        require(has(st,"\"infiniteRespawn\":false,\"runTainted\":true"),st);
        require(!th128_world_set_debug_infinite_respawn(2)&&has(st,"\"infiniteRespawn\":false"),"invalid toggle changed state");
        th128_world_reset();
        st=th128_world_state();
        require(has(st,"\"infiniteRespawn\":false,\"runTainted\":false"),st);
        std::cout<<"PASS debug world tick: default-off GameOver, refill, respawn, taint/reset\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
