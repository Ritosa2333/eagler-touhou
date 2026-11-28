#include "game/PlayerHitHost.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
using Bytes=std::vector<std::uint8_t>;
static Bytes read(const char* path){std::ifstream file(path,std::ios::binary);assert(file);return {std::istreambuf_iterator<char>(file),{}};}
static bool run(Bytes animation,int motivation,bool& gameOver){
    th128::player::PlayerHitHost host;std::string error;
    const th128::player::hit::Timer zero{-1,0,0,0x4b6f58,1};
    const th128::player::hit::Timer state{29,30,30,0x4b6f58,1};
    const th128::player::hit::Timer protection{199,200,200,0x4b6f58,1};
    th128::player::HitHostSnapshot snapshot{{2,0,30},state,zero,protection,{true,0},{0,0,0},{true,0}};
    if(!host.configure(animation.data(),animation.size(),snapshot,
        [](int,th128::player::hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error)){std::cerr<<error;return false;}
    const auto result=host.beginRespawn({0,0,0},motivation,0,
        [](const th128::player::death::CallRequest& call,std::string&){return call.kind==th128::player::death::CallKind::RespawnField43df90||
            call.kind==th128::player::death::CallKind::PostRespawn427190;},error);
    gameOver=!result;
    return result.has_value();
}
int main(int argc,char** argv){
    assert(argc==2);const auto animation=read(argv[1]);
    bool lowGameOver=false;
    if(run(animation,-10000,lowGameOver)){std::cerr<<"low motivation unexpectedly respawned";return 1;}
    if(!lowGameOver){std::cerr<<"low motivation did not hit game-over gate";return 1;}
    bool highGameOver=true;
    if(!run(animation,10000,highGameOver)||highGameOver){std::cerr<<"legal motivation did not begin respawn";return 1;}
    std::cout<<"PASS debug respawn gate boundary\n";
}
