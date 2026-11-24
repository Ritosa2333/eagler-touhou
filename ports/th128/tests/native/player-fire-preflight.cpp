#include "game/PlayerFireHost.hpp"
#include <cassert>
#include <iostream>
using namespace th128::player;
using namespace th128::player::fire;
static hit::Timer timer(int value){return {value-1,value,float(value),0x4b6f58,1};}
int main(){
    auto run=[](Host::FreezePreflight preflight,int& freezes,bool& denied){
        Host host;std::string error;
        if(!host.configure(-1,timer(45),error)){std::cerr<<error;return false;}
        Input input;int power=3000;
        auto emit=[](int,std::string&){return true;};
        auto sound=[](int,int,std::string&){return true;};
        auto freeze=[&](int value,std::string&){++freezes;if(value<3000){std::cerr<<"bad freeze power";return false;}return true;};
        const bool ok=host.tick(1,input,0,0,power,emit,sound,freeze,error,preflight);
        denied=!ok?false:(power!=0&&host.state().held.current==45);
        if(!ok)std::cerr<<error;
        return ok;
    };
    int freezes=0;bool denied=false;
    bool ok=run({},freezes,denied);
    if(!ok||freezes!=1||denied){std::cerr<<"legacy path changed\n";return 1;}
    freezes=0;
    ok=run([](int,std::string&){return FreezePreflightDecision::Deny;},freezes,denied);
    if(!ok||freezes!=0||!denied){std::cerr<<"denied path changed power/timer or called freeze\n";return 1;}
    std::cout<<"PASS player fire preflight legacy/deny\n";
}
