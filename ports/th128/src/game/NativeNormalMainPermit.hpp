#pragma once
#include "animation/AnmScene.hpp"
#include "engine/Random.hpp"
#include <memory>
#include <vector>
#include <string>
namespace th128::postrun {
class NativeNormalMainOwner;
class NativeNormalMainPermit;
bool adoptOrdinaryMain(std::unique_ptr<NativeNormalMainPermit>& permit,std::string& error);
struct NativeNormalMainRandom {
    unsigned generation=0,worldFrame=0,nativeFrames=0;
    th128::Random game{},display{};
    bool same(const NativeNormalMainRandom& other)const{return generation==other.generation&&worldFrame==other.worldFrame&&nativeFrames==other.nativeFrames
        &&game.seed==other.game.seed&&game.calls==other.game.calls&&display.seed==other.display.seed&&display.calls==other.display.calls;}
};
// Only the actual Menu/Media/profile-receipt completion owner can mint this.
class NativeNormalMainPermit {
    friend class NativeNormalMainOwner;
    friend bool adoptOrdinaryMain(std::unique_ptr<NativeNormalMainPermit>&,std::string&);
    std::unique_ptr<anm::Scene> scene_;
    std::vector<std::uint8_t> profile_;
    NativeNormalMainRandom random_;
    unsigned tick_=0,serial_=0,route_=0,rank_=0,continues_=0,digest_=0;
    bool consumed_=false,extra_=false;
    NativeNormalMainPermit(std::unique_ptr<anm::Scene> scene,std::vector<std::uint8_t> profile,NativeNormalMainRandom random,
        unsigned tick,unsigned serial,unsigned route,unsigned rank,unsigned continues,unsigned digest,bool extra=false)
        :scene_(std::move(scene)),profile_(std::move(profile)),random_(random),tick_(tick),serial_(serial),route_(route),rank_(rank),continues_(continues),digest_(digest),extra_(extra){}
public:
    NativeNormalMainPermit(const NativeNormalMainPermit&)=delete;
    NativeNormalMainPermit& operator=(const NativeNormalMainPermit&)=delete;
    NativeNormalMainPermit(NativeNormalMainPermit&&)=default;
    NativeNormalMainPermit& operator=(NativeNormalMainPermit&&)=default;
    bool ready()const{return !consumed_&&scene_&&profile_.size()==52232&&random_.generation&&random_.worldFrame&&random_.nativeFrames&&tick_&&serial_&&(extra_?route_==6&&rank_==4&&continues_==0:route_<6&&rank_<4&&continues_<=9);}
    unsigned generation()const{return random_.generation;}
};
}
