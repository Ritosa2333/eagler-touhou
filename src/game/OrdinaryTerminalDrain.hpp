#pragma once
#include "game/NativeOrdinaryTerminalOwner.hpp"
#include <functional>
#include <string>

namespace th128::stage {
class OrdinaryTerminalDrain {
public:
    using Validator=std::function<bool(unsigned,unsigned,int,std::string&)>;
private:
    const th128::record::NativeOrdinaryTerminalOwner* owner_=nullptr;
    unsigned generation_=0,stage_=0;
    int progress_=0;
    Validator validator_;
public:
    bool bind(const th128::record::NativeOrdinaryTerminalOwner& owner,int stage,int progress,
              Validator validator,std::string& error){
        error.clear();
        if(owner_||!owner.bound()||owner.claimed()||owner.recorded()||stage<1||progress<0||!validator){
            error="ordinary terminal drain requires a fresh unclaimed native owner";return false;
        }
        const auto& identity=owner.identity();
        if(identity.stage!=unsigned(stage)||!identity.generation){
            error="ordinary terminal drain differs from its bound native stage";return false;
        }
        owner_=&owner;generation_=identity.generation;stage_=identity.stage;progress_=progress;
        validator_=std::move(validator);return true;
    }
    bool ready(std::string& error)const{
        error.clear();
        if(!owner_||!validator_||!owner_->bound()||!owner_->claimed()||!owner_->recorded()){
            error="ordinary terminal drain lacks a claimed recorded native owner";return false;
        }
        const auto& identity=owner_->identity();
        if(identity.generation!=generation_||identity.stage!=stage_){
            error="ordinary terminal drain native identity changed";return false;
        }
        try{
            if(!validator_(generation_,stage_,progress_,error)||!error.empty()){
                if(error.empty())error="ordinary terminal drain native session validator declined";
                return false;
            }
        }catch(const std::exception& failure){error=failure.what();return false;}
        return true;
    }
    bool bound()const noexcept{return owner_!=nullptr;}
    void reset()noexcept{owner_=nullptr;generation_=stage_=0;progress_=0;validator_={};}
};
}
