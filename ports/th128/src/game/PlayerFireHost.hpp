#pragma once
#include "PlayerFireRules.hpp"
#include <functional>
#include <string>
namespace th128::player::fire {
struct State {int shot=-1,savedPower=0;hit::Timer held{-1,0,0,0x4b6f58,1};};
enum class FreezePreflightDecision {Allow,Deny};
class Host {
public:
    using Emit=std::function<bool(int,std::string&)>;
    using Sound=std::function<bool(int,int,std::string&)>;
    using Freeze=std::function<bool(int,std::string&)>;
    using FreezePreflight=std::function<FreezePreflightDecision(int,std::string&)>;
    bool configure(int shot,hit::Timer held,std::string& error);
    bool tick(int playerState,Input input,int busy,int mode,int& power,const Emit& emit,const Sound& sound,const Freeze& freeze,std::string& error,
              FreezePreflight preflight={},bool keepPower=false);
    bool heldTimer(hit::Timer timer,std::string& error);
    bool suspend(const Sound& sound,std::string& error);
    const State& state() const{return state_;}
private:
    State state_;
    bool configured_=false,executing_=false;
    std::string failure_;
    bool fail(std::string reason,std::string& error);
};
}
