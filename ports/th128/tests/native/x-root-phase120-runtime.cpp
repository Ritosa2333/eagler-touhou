#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

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
static unsigned effectDraws(const std::string& text){
    unsigned count=0;
    for(std::size_t at=0;(at=text.find("\"id\":",at))!=std::string::npos;at+=5){
        const auto id=std::stoll(text.substr(at+5));
        if(id>=13500000)++count;
    }
    return count;
}
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
const char* th128_world_bullet_draws();
const char* th128_world_bullet_draw_error();
unsigned th128_world_sound_count();
int th128_world_sound_id(unsigned);
float th128_world_sound_x(unsigned);
int th128_world_sound_operation(unsigned);
void th128_world_reset();
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
        th128_world_reset();
        auto load=[&](const std::string& name,const std::vector<std::uint8_t>& bytes){
            return th128_world_load(name.c_str(),unsigned(name.size()),bytes.data(),unsigned(bytes.size()));
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
        std::string state=th128_world_state();
        require(has(state,"\"phase\":119"),state);
        const std::string beforeDrawError=th128_world_bullet_draw_error();
        require(beforeDrawError.empty(),beforeDrawError);
        const char* draw119=th128_world_bullet_draws();
        require(effectDraws(draw119?draw119:"")==2,"phase119 must draw the two visible X root nodes");
        const unsigned before=th128_world_sound_count();
        require(th128_world_tick(),th128_world_error());
        state=th128_world_state();
        require(has(state,"\"phase\":120"),state);
        require(has(state,"\"protectionTicks\":10"),state);
        const std::string afterDrawError=th128_world_bullet_draw_error();
        require(afterDrawError.empty(),afterDrawError);
        require(effectDraws(th128_world_bullet_draws())==0,"phase120 must remove every X root draw node");
        const unsigned after=th128_world_sound_count();
        require(after>=before+3,state);
        const unsigned first=before;
        require(th128_world_sound_id(first)==47,state);
        require(th128_world_sound_id(first+1)==69,state);
        require(th128_world_sound_id(first+2)==51,state);
        const double playerX=number(state,"playerX");
        const double soundX=th128_world_sound_x(first);
        require(std::abs(soundX-playerX)<1e-6,"phase120 sound47 x does not match playerX");
        require(soundX!=320.0,"phase120 sound47 must not use screen center");
        require(th128_world_sound_operation(first)==0,"47 must be play");
        require(th128_world_sound_operation(first+1)==0,"69 must be play");
        require(th128_world_sound_operation(first+2)==1,"51 must be stop");
        require(th128_world_sound_x(first+1)==0.0,"69 must be non-positional");
        require(th128_world_sound_x(first+2)==0.0,"51 must be non-positional");
        int id47=0,id69=0,id51=0;
        for(unsigned index=before;index<after;++index){
            if(th128_world_sound_id(index)==47)++id47;
            if(th128_world_sound_id(index)==69)++id69;
            if(th128_world_sound_id(index)==51)++id51;
        }
        require(id47==1&&id69==1&&id51==1,"phase120 sounds must not duplicate in the newly emitted slice");
        require(!has(state,"\"faulted\":true"),state);
        require(th128_world_tick(),th128_world_error());
        require(effectDraws(th128_world_bullet_draws())==0,"phase121 must retain root removal");
        for(int tick=0;tick<59;++tick)require(th128_world_tick(),th128_world_error());
        state=th128_world_state();
        require(has(state,"\"phase\":180"),state);
        require(effectDraws(th128_world_bullet_draws())==0,"phase180 must not resurrect the root");
        th128_world_reset();
        require(effectDraws(th128_world_bullet_draws())==0,"reset must not leak the X root owner");
        std::cout<<"PASS phase120 root draw 2 visible nodes->0, sound 47/69/51, protection10, reset clean\n";
    }catch(const std::exception& error){
        std::cerr<<error.what()<<'\n';
        return 1;
    }
}
