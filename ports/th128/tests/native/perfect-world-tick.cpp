#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
static std::vector<std::uint8_t> read(const std::string& path){std::ifstream f(path,std::ios::binary);if(!f)throw std::runtime_error("missing "+path);return {std::istreambuf_iterator<char>(f),{}};}
static void require(bool ok,const std::string& message){if(!ok){std::cerr<<message<<'\n';std::exit(1);}}
extern "C" {
int th128_world_load(const char*,std::uint32_t,const void*,std::uint32_t);
int th128_world_start(const char*,std::uint32_t,int,unsigned);
int th128_world_enable_bullets(const void*,std::uint32_t,const void*,std::uint32_t,float,float);
int th128_world_load_enemy_anm(int,const void*,std::uint32_t);
int th128_world_add_bullet_profile(int,const void*,unsigned);
int th128_world_enable_bullet_updates(const void*,std::uint32_t);
int th128_world_enable_graze(int,int,unsigned);
int th128_world_enable_player_context(const void*,std::uint32_t,int,unsigned,int,unsigned,unsigned,int,int,int,int);
int th128_world_enable_player_motion(int,int);
int th128_world_enable_player_shots(const void*,std::uint32_t,const void*,std::uint32_t,const void*,std::uint32_t,float,int,unsigned,unsigned,unsigned,unsigned);
int th128_world_enable_shot_hits(int,int,int,int,unsigned);
int th128_world_enable_rapid_shot(int);
int th128_world_enable_player_death(int);
int th128_world_enable_perfect_hud(const void*,unsigned);
int th128_world_enable_player_respawn(int,int,const void*,unsigned);
int th128_world_enable_z_fire(const void*,unsigned,const void*,unsigned);
int th128_world_input(unsigned,unsigned,unsigned);
int th128_world_tick();
const char* th128_world_state();
const char* th128_world_error();
void th128_world_reset();
}
static std::string state(){return th128_world_state();}
static bool has(const std::string& text,const std::string& needle){return text.find(needle)!=std::string::npos;}
static int integer(const std::string& text,const std::string& key){const auto at=text.find("\""+key+"\":");if(at==std::string::npos)throw std::runtime_error("missing "+key);return std::stoi(text.substr(at+key.size()+3));}
int main(int argc,char** argv){
    try{
        require(argc==7,"usage: stage directory profile8 masks type13 type25 enemy.anm");
        const std::string dir=argv[1],profile8Path=argv[2];
        auto name=[&](const char* file){return std::string(dir)+"/raw/"+file;};
        const auto ecl=read(name("st_a1_1.ecl")),shared=read(name("default.ecl")),later=read(name("st_a1_1b.ecl"));
        const auto bullet=read(name("bullet.anm")),ice=read(name("ice.anm")),masks=read(argv[3]);
        const auto front=read(name("front.anm")),pl00=read(name("pl00.anm")),shot=read(name("pl00a.sht"));
        const auto profile8=read(profile8Path),type13=read(argv[4]),type25=read(argv[5]),enemy=read(argv[6]);
        const std::string nameA="st_a1_1.ecl",nameShared="default.ecl",nameLater="st_a1_1b.ecl";
        th128_world_reset();
        auto load=[&](const std::string& n,const std::vector<std::uint8_t>& bytes){return th128_world_load(n.c_str(),unsigned(n.size()),bytes.data(),unsigned(bytes.size()));};
        require(load(nameA,ecl)&&load(nameShared,shared)&&load(nameLater,later),th128_world_error());
        require(th128_world_start(nameA.c_str(),unsigned(nameA.size()),1,34772),th128_world_error());
        require(th128_world_load_enemy_anm(1,enemy.data(),unsigned(enemy.size())),th128_world_error());
        require(th128_world_enable_bullets(profile8.data(),unsigned(profile8.size()),bullet.data(),unsigned(bullet.size()),0,51200),th128_world_error());
        require(th128_world_add_bullet_profile(13,type13.data(),unsigned(type13.size())),th128_world_error());
        require(th128_world_add_bullet_profile(25,type25.data(),unsigned(type25.size())),th128_world_error());
        require(th128_world_enable_bullet_updates(shot.data(),unsigned(shot.size())),th128_world_error());
        require(th128_world_enable_graze(0,10000,0x1234),th128_world_error());
        require(th128_world_enable_player_context(pl00.data(),unsigned(pl00.size()),1,0,120,0,0,0,0,0,30000),th128_world_error());
        require(th128_world_enable_player_motion(0,51200),th128_world_error());
        std::vector<std::uint8_t> sources(2000*4,0);
        require(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),pl00.data(),unsigned(pl00.size()),sources.data(),2000,0,0,0,0,0,0),th128_world_error());
        require(th128_world_enable_shot_hits(100000,0,0,0,0),th128_world_error());
        require(th128_world_enable_rapid_shot(-1),th128_world_error());
        require(th128_world_enable_player_death(-1),th128_world_error());
        require(th128_world_enable_perfect_hud(front.data(),unsigned(front.size())),th128_world_error());
        std::vector<std::uint8_t> targets(32,0);
        require(th128_world_enable_player_respawn(1,0,targets.data(),8),th128_world_error());
        require(th128_world_enable_z_fire(ice.data(),unsigned(ice.size()),masks.data(),unsigned(masks.size())),th128_world_error());
        auto worldState=state();
        require(has(worldState,"\"perfectFreeze\":{\"configured\":true"),worldState);
        require(has(worldState,"\"stock\":30000"),worldState);
        require(th128_world_input(2,2,0),th128_world_error());
        require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(has(worldState,"\"perfectFreeze\":{\"configured\":true"),worldState);
        require(has(worldState,"\"active\":true"),worldState);
        require(has(worldState,"\"phase\":0"),worldState);
        require(has(worldState,"\"systemMode3c\":1"),worldState);
        require(has(worldState,"\"protectionTicks\":200"),worldState);
        require(has(worldState,"\"stock\":20000"),worldState);
        require(has(worldState,"\"charge\":20000"),worldState);
        require(has(worldState,"\"playerProtection\":200"),worldState);
        require(th128_world_input(2,0,0),th128_world_error());
        require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(has(worldState,"\"phase\":1"),worldState);
        require(has(worldState,"\"protectionTicks\":199"),worldState);
        require(has(worldState,"\"playerProtection\":199"),worldState);
        for(int tick=0;tick<119;++tick)require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(has(worldState,"\"phase\":120"),worldState);
        require(has(worldState,"\"active\":true"),worldState);
        const bool frozen=has(worldState,"\"frozen\":0")==false;
        std::cout<<"WORLD tick state phase120 frozen="<<frozen<<" state="<<worldState.substr(0,std::min<size_t>(worldState.size(),400))<<"\n";
        for(int tick=0;tick<60;++tick)require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(has(worldState,"\"phase\":180"),worldState);
        require(has(worldState,"\"systemMode3c\":0"),worldState);
        require(has(worldState,"\"active\":true"),worldState);
        require(th128_world_input(1,1,0),th128_world_error());
        for(int tick=0;tick<70&&integer(state(),"heldZ")<45;++tick){
            require(th128_world_tick(),th128_world_error());
            require(th128_world_input(1,0,0),th128_world_error());
        }
        worldState=state();
        const int powerBeforeRelease=integer(worldState,"powerRaw");
        const int heldBeforeRelease=integer(worldState,"heldZ");
        require(heldBeforeRelease>=45,worldState);
        require(th128_world_input(0,0,1),th128_world_error());
        require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(has(worldState,"\"zDenied\":\"ordinary Z freeze during a live Perfect ice batch is unsupported\""),worldState);
        require(integer(worldState,"powerRaw")==powerBeforeRelease,worldState);
        require(integer(worldState,"heldZ")==heldBeforeRelease,worldState);
        for(int tick=0;tick<320&&integer(state(),"powerRaw")!=0;++tick)require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(integer(worldState,"powerRaw")==0,worldState);
        require(has(worldState,"\"busy\":1"),worldState);
        for(int tick=0;tick<20;++tick)require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(has(worldState,"\"active\":false"),worldState);
        require(has(worldState,"\"stock\":20000"),worldState);
        require(th128_world_input(0,0,2),th128_world_error());
        require(th128_world_tick(),th128_world_error());
        require(th128_world_input(2,2,0),th128_world_error());
        require(th128_world_tick(),th128_world_error());
        worldState=state();
        require(has(worldState,"\"active\":false"),worldState);
        require(has(worldState,"\"stock\":20000"),worldState);
        require(has(worldState,"\"lastDenied\":\"ordinary ice batch or live Perfect objects are nonempty\""),worldState);
        th128_world_reset();
        require(has(state(),"\"perfectFreeze\":{\"configured\":false"),state());
        std::cout<<"PASS perfect world tick: stock/protection/phase/restart, frozen="<<frozen<<"\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
