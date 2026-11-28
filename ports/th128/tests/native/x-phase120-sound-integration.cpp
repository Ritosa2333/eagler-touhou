#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "runtime/WorldExports.cpp"

static std::vector<std::uint8_t> read(const std::string& path){
    std::ifstream input(path,std::ios::binary);
    if(!input)throw std::runtime_error("missing "+path);
    return {std::istreambuf_iterator<char>(input),{}};
}
static void require(bool ok,const std::string& message){
    if(!ok){std::cerr<<message<<'\n';std::exit(1);}
}
static bool has(const std::string& text,const std::string& needle){
    return text.find(needle)!=std::string::npos;
}
static double number(const std::string& text,const std::string& key){
    const auto at=text.find('"'+key+"\":");if(at==std::string::npos)throw std::runtime_error("missing "+key);
    return std::stod(text.substr(at+key.size()+3));
}

int main(int argc,char**argv){
    try{
        require(argc==7,"usage: stage directory profile8 masks type13 type25 enemy.anm");
        const std::string directory=argv[1];
        auto path=[&](const char* name){return directory+"/raw/"+name;};
        const auto ecl=read(path("st_a1_1.ecl")),shared=read(path("default.ecl")),later=read(path("st_a1_1b.ecl"));
        const auto bullet=read(path("bullet.anm")),ice=read(path("ice.anm")),masks=read(argv[3]);
        const auto front=read(path("front.anm")),pl00=read(path("pl00.anm")),shot=read(path("pl00a.sht"));
        const auto profile8=read(argv[2]),type13=read(argv[4]),type25=read(argv[5]),enemy=read(argv[6]);
        const std::string name="st_a1_1.ecl";
        auto setup=[&](){
            th128_world_reset();
            auto load=[&](const std::string& resource,const std::vector<std::uint8_t>& bytes){
                return th128_world_load(resource.c_str(),unsigned(resource.size()),bytes.data(),unsigned(bytes.size()));
            };
            require(load(name,ecl)&&load("default.ecl",shared)&&load("st_a1_1b.ecl",later),th128_world_error());
            require(th128_world_start(name.c_str(),unsigned(name.size()),1,34772),th128_world_error());
            require(th128_world_load_enemy_anm(1,enemy.data(),unsigned(enemy.size())),th128_world_error());
            require(th128_world_enable_bullets(profile8.data(),unsigned(profile8.size()),bullet.data(),unsigned(bullet.size()),0,51200),th128_world_error());
            require(th128_world_add_bullet_profile(13,type13.data(),unsigned(type13.size())),th128_world_error());
            require(th128_world_add_bullet_profile(25,type25.data(),unsigned(type25.size())),th128_world_error());
            require(th128_world_enable_bullet_updates(shot.data(),unsigned(shot.size())),th128_world_error());
            require(th128_world_enable_graze(0,10000,0x1234),th128_world_error());
            require(th128_world_enable_player_context(pl00.data(),unsigned(pl00.size()),1,0,120,0,0,0,0,0,30000),th128_world_error());
            require(th128_world_enable_player_motion(23000,51200),th128_world_error());
            std::vector<std::uint8_t> sources(2000*4,0);
            require(th128_world_enable_player_shots(shot.data(),unsigned(shot.size()),pl00.data(),unsigned(pl00.size()),sources.data(),2000,0,0,0,0,0,0),th128_world_error());
            require(th128_world_enable_shot_hits(100000,0,0,0,0),th128_world_error());
            require(th128_world_enable_rapid_shot(-1),th128_world_error());
            require(th128_world_enable_player_death(-1),th128_world_error());
            require(th128_world_enable_perfect_hud(front.data(),unsigned(front.size())),th128_world_error());
            std::vector<std::uint8_t> targets(32,0);
            require(th128_world_enable_player_respawn(1,0,targets.data(),8),th128_world_error());
            require(th128_world_enable_z_fire(ice.data(),unsigned(ice.size()),masks.data(),unsigned(masks.size())),th128_world_error());
            require(has(th128_world_state(),"\"perfectFreeze\":{\"configured\":true"),th128_world_state());
            require(th128_world_input(2,2,0),th128_world_error());
            require(th128_world_tick(),th128_world_error());
            require(th128_world_input(2,0,0),th128_world_error());
            require(th128_world_tick(),th128_world_error());
            for(int tick=0;tick<118;++tick)require(th128_world_tick(),th128_world_error());
            require(has(th128_world_state(),"\"phase\":119"),th128_world_state());
        };
        auto scan=[](unsigned begin,unsigned end,int identifier){
            int count=0;
            for(unsigned index=begin;index<end;++index)if(th128_world_sound_id(index)==identifier)++count;
            return count;
        };
        setup();
        const unsigned before=th128_world_sound_count();
        require(th128_world_tick(),th128_world_error());
        std::string state=th128_world_state();
        require(has(state,"\"phase\":120"),state);
        const unsigned phase120=th128_world_sound_count();
        require(phase120>=before+3,state);
        require(th128_world_sound_id(before)==47,state);
        require(th128_world_sound_id(before+1)==69,state);
        require(th128_world_sound_id(before+2)==51,state);
        require(th128_world_sound_operation(before)==0,"47 must be play");
        require(th128_world_sound_operation(before+1)==0,"69 must be play");
        require(th128_world_sound_operation(before+2)==1,"51 must be stop");
        require(th128_world_sound_x(before+1)==0.0,"69 must be non-positional");
        require(th128_world_sound_x(before+2)==0.0,"51 must be non-positional");
        const double playerX=number(state,"playerX");
        require(std::abs(th128_world_sound_x(before)-playerX)<1e-6,"47 must use playerX");
        require(std::abs(playerX-320.0)>1e-6,"47 must not use center");
        require(scan(before,phase120,47)==1&&scan(before,phase120,69)==1&&scan(before,phase120,51)==1,
            "phase120 each sound once");
        require(th128_world_tick(),th128_world_error());
        state=th128_world_state();
        require(has(state,"\"phase\":121"),state);
        const unsigned phase121=th128_world_sound_count();
        require(scan(phase120,phase121,47)==0&&scan(phase120,phase121,69)==0&&scan(phase120,phase121,51)==0,
            "phase121 must not duplicate phase120 sounds");

        setup();
        const unsigned budgetBefore=th128_world_sound_count();
        grazeSounds.resize(65534);
        require(th128_world_tick()==0,"budget failure must reject the tick");
        const std::string budgetError=th128_world_error();
        require(has(budgetError,"perfect phase120 sound journal budget"),budgetError);
        state=th128_world_state();
        require(th128_world_sound_count()==65534,"budget failure must not append sounds");
        require(has(state,"\"failed\":true"),"budget failure must latch Host failure");
        require(has(state,"\"frozen\":0"),"budget failure must occur before freeze");
        require(budgetBefore<=65534,state);
        std::cout<<"PASS phase120/121 sound journal 47,69,51 playerX="<<playerX
                 <<" before="<<before<<" phase120="<<phase120<<" phase121="<<phase121
                 <<" budgetError="<<budgetError<<'\n';
    }catch(const std::exception& error){
        std::cerr<<error.what()<<'\n';
        return 1;
    }
}
