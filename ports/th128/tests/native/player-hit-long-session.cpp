#include "game/PlayerHitHost.hpp"
#include "game/PlayerHitRules.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

using namespace th128::player;
static unsigned checks=0;
static void require(bool condition,const char* message){
    ++checks;
    if(!condition){std::cerr<<"FAILED: "<<message<<'\n';std::exit(1);}
}
static hit::Timer timer(int value){return {value-1,value,float(value),0x4b6f58,1};}

int main(int argc,char** argv){
    require(argc==2,"original pl00 ANM path");
    std::ifstream stream(argv[1],std::ios::binary);
    const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(stream)),{});
    require(!bytes.empty(),"complete original player asset");
    const hit::Timer zero{-1,0,0,0x4b6f58,1};
    const HitHostSnapshot snapshot{{1,0,0},zero,zero,zero,{true,0},{0,0,0},{true,0}};
    PlayerHitHost host;std::string error;
    unsigned effects=0,sounds=0;
    require(host.configure(bytes.data(),bytes.size(),snapshot,
        [&](int script,hit::Position point,int layer,std::string&){
            require(script==83||script==84,"original authored hit effect");
            require(point.horizontal==320&&point.vertical==416&&layer==0,"original hit effect position");
            ++effects;return true;
        },[&](int sound,std::string&){require(sound==2,"original hit sound2");++sounds;return true;},error),
        "configure actual PlayerHitHost");
    for(int tick=0;tick<65754;++tick)require(host.advanceNormalTimersUnit(error),"legitimate normal-state timer advance");
    require(host.snapshot().stateTimer.current==65754,"observed558 timer reached through actual production increments");
    const auto overlap=host.confirmedOverlap({0,400,0},{false,0},error);
    if(!overlap)std::cerr<<"actual PlayerHitHost error: "<<error<<'\n';
    require(overlap&&*overlap==1,"confirmed overlap after65754 updates must finish the real hit path");
    require(error.empty(),"long-session hit has no native fault");
    require(effects==33&&sounds==1,"all33 effects and original sound executed exactly once");
    require(host.snapshot().gate.stateF78==4&&host.snapshot().stateTimer.current==0,"state4 and state timer reset");
    require(host.snapshot().protection.current==6,"original six-frame hit counter");
    require(host.animationState()&&host.animationState()->script==0,"real player ANM rebound");
    for(unsigned tick=0;tick<8;++tick)require(host.advanceWindowUnit({true,0,0,0},error),"actual death-window progression");
    require(host.snapshot().stateTimer.current==8,"death window reaches original boundary");
    require(!host.advanceWindowUnit({true,0,0,0},error)&&error.find("death commit")!=std::string::npos,
        "unsupported death callback remains explicit");

    for(const int elapsed:{65535,65536,65537,65754,71332,999999}){
        const auto t=timer(elapsed);
        const auto reset=hit::timerResetPrefix(t,{true,0});
        require(reset&&reset->updated.previous==-1&&reset->updated.current==0&&reset->updated.fractional==0,
            "long finite state timer resets to original zero tuple");
        require(reset->requiredCall&&reset->requiredCall->argument==2,"long timer preserves sound request");
        const auto initial=hit::normalInitPrefix(t,t,t,7);
        require(initial&&initial->stateTimer.current==0&&initial->focusTimer.current==0&&initial->rechargeTimer.current==-1,
            "long timers allow original normal-init writes");
        const auto counter=hit::counterResetPrefix(t);
        require(counter&&counter->updated.current==6,"long counter reset");
        const auto step=hit::counterStep(t,1.f);
        require(step&&step->previous==elapsed&&step->current==elapsed-1&&step->fractional==float(elapsed-1),
            "long positive protection counter decrements safely");
    }
    for(const float value:{std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),2147483648.f,-2147483904.f}){
        auto invalid=zero;invalid.fractional=value;
        require(!hit::timerResetPrefix(invalid,{true,0}),"nonrepresentable state timer remains rejected");
        require(!hit::counterResetPrefix(invalid),"nonrepresentable protection timer remains rejected");
        require(!hit::normalInitPrefix(invalid,zero,zero,0),"malformed normal init remains rejected");
        require(!hit::windowPrefix(4,zero,invalid,{false,0,0,0}),"malformed held timer remains rejected");
        require(!hit::counterStep(invalid,1.f),"invalid conversion cannot execute");
    }
    for(const float coordinate:{65537.f,-65537.f,std::numeric_limits<float>::infinity()})
        require(!hit::entryPrefix(1,{coordinate,0,0}),"coordinate range remains unchanged");
    auto lastSafe=zero;lastSafe.current=2147483520;lastSafe.previous=2147483519;lastSafe.fractional=2147483520.f;
    const auto largeStep=hit::counterStep(lastSafe,1.f);
    require(largeStep&&largeStep->current==2147483520,"binary32 rounding precedes safe int32 conversion");
    auto negative=zero;negative.current=1;negative.fractional=-2147483648.f;
    const auto lowStep=hit::counterStep(negative,4.f);
    require(lowStep&&lowStep->current==std::numeric_limits<int>::min(),"rounded lower int32 boundary is representable");
    std::cout<<"{\"passed\":true,\"normalTimerAdvances\":65754,\"actualHostHit\":true,\"checks\":"<<checks
        <<",\"WorldStarts\":0,\"originalPlayerANM\":true,\"syntheticEffectsAndSound\":true}"<<'\n';
}
