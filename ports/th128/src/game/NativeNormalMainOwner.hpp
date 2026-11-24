#pragma once
#include "NativeNormalMainPermit.hpp"
#include "NativePostrunMenu.hpp"
#include "NativeNormalMenuReceipt.hpp"
#include "NativePostrunMedia.hpp"
#include "StageControl.hpp"
namespace th128::postrun {
class NativeNormalMainOwner {
    const Menu* menu_=nullptr;
    NativeNormalMainRandom random_{};
    std::vector<std::uint8_t> packet_;
    bool acknowledged_=false,released_=false;
    unsigned tick_=0,digest_=0;
    bool boundary(const Menu& menu,const Media& media,const NativeNormalMenuReceipt& receipt,const stage::Control& control,
        unsigned serial,bool awaitingPresent,const NativeNormalMainRandom& actual,std::string& error)const{
        if(menu_!=&menu||released_||!menu.sealedPostrun()||menu.normalGeneration()!=random_.generation||!random_.same(actual)
            ||actual.nativeFrames!=menu.normalFrames()||serial!=menu.ticks()||awaitingPresent||!media.empty()||!receipt.settled(menu)
            ||control.current()!=4||control.pending()!=4||control.menuMode()!=3||!menu.mainHandoffReady()||menu.replayPresent()||menu.ordinaryReplaySavePending()){
            error="ordinary Main requires its actual same-owner media/profile/replay/presentation/RNG boundary";return false;}return true;
    }
public:
    bool bind(const Menu& menu,NativeNormalMainRandom random,std::string& error){
        error.clear();if(menu_||!menu.sealedPostrun()||random.generation!=menu.normalGeneration()||!random.worldFrame||random.nativeFrames!=menu.normalFrames()||!random.nativeFrames){error="ordinary Main owner binding rejected";return false;}
        menu_=&menu;random_=random;return true;
    }
    bool pending()const{return !packet_.empty()&&!released_;}
    std::span<const std::uint8_t> packet()const{return packet_;}
    bool prepare(const Menu& menu,const Media& media,const NativeNormalMenuReceipt& receipt,const stage::Control& control,
        unsigned serial,bool awaitingPresent,const NativeNormalMainRandom& actual,std::string& error){
        error.clear();if(pending()||acknowledged_||!boundary(menu,media,receipt,control,serial,awaitingPresent,actual,error))return false;
        tick_=menu.ticks();digest_=NativeOrdinaryReplaySave::checksum(menu.profile());const auto word=[this](unsigned v){for(unsigned i=0;i<4;++i)packet_.push_back(std::uint8_t(v>>(8*i)));};
        word(0x314d4e4fu);word(1);word(128+52232);word(random_.generation);word(1);word(tick_);word(random_.worldFrame);word(random_.nativeFrames);
        word(menu.normalRoute());word(menu.normalRank());word(menu.normalContinues());word(52232);word(digest_);word(menu.normalNameConfirmationSerial());word(menu.sceneNodeCount());word(3);
        word(random_.game.seed);word(random_.game.calls);word(random_.display.seed);word(random_.display.calls);word(1);word(menu.extraSealed()?1:0);unsigned seen=0;
        for(unsigned i=0;i<10;++i)if(menu.profile()[record::NativeScoreProfile::StatsOffset+0x26+i])seen|=1u<<i;word(seen);word(record::NativeScoreProfile::StatsOffset+12);
        for(unsigned i=0;i<8;++i)word(0);packet_.insert(packet_.end(),menu.profile().begin(),menu.profile().end());return true;
    }
    bool acknowledge(const Menu& menu,const Media& media,const NativeNormalMenuReceipt& receipt,const stage::Control& control,
        unsigned serial,bool awaitingPresent,const NativeNormalMainRandom& actual,std::span<const std::uint8_t> bytes,std::string& error){
        error.clear();if(!pending()||acknowledged_||!boundary(menu,media,receipt,control,serial,awaitingPresent,actual,error)||tick_!=menu.ticks()
            ||bytes.size()!=packet_.size()||!std::equal(bytes.begin(),bytes.end(),packet_.begin())||!std::equal(menu.profile().begin(),menu.profile().end(),packet_.begin()+128)){
            if(error.empty())error="ordinary Main full descriptor acknowledgement is stale, duplicate or changed";return false;}acknowledged_=true;return true;
    }
    std::unique_ptr<NativeNormalMainPermit> release(Menu& menu,const Media& media,const NativeNormalMenuReceipt& receipt,const stage::Control& control,
        unsigned serial,bool awaitingPresent,const NativeNormalMainRandom& actual,std::string& error){
        error.clear();if(!pending()||!acknowledged_||tick_!=menu.ticks()||!boundary(menu,media,receipt,control,serial,awaitingPresent,actual,error)
            ||!std::equal(menu.profile().begin(),menu.profile().end(),packet_.begin()+128))return {};
        auto permit=std::unique_ptr<NativeNormalMainPermit>(new NativeNormalMainPermit(menu.releaseMainScene(),menu.profile(),random_,tick_,1,
            menu.normalRoute(),menu.normalRank(),menu.normalContinues(),digest_,menu.extraSealed()));if(!permit->ready()){error="ordinary Main native sealed permit incomplete";return {};}
        released_=true;return permit;
    }
};
}
