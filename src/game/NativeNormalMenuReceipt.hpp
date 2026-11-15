#pragma once
#include "NativePostrunMenu.hpp"
#include <algorithm>
#include <span>
namespace th128::postrun {
// Full current-menu receipt; the earlier NativeNormalRecord stats digest stays immutable.
class NativeNormalMenuReceipt {
    unsigned generation_=0,serial_=0,tick_=0,acknowledgedConfirmation_=0,pendingConfirmation_=0;
    std::vector<std::uint8_t> acknowledged_,pending_;
    static unsigned digest(std::span<const std::uint8_t> bytes){unsigned value=2166136261u;for(auto b:bytes){value^=b;value*=16777619u;}return value;}
public:
    bool bind(const Menu& menu,std::string& error){
        error.clear();if(generation_||!menu.sealedPostrun()||!menu.normalGeneration()||menu.profile().size()!=52232){error="ordinary menu receipt binding requires its sealed native menu";return false;}
        generation_=menu.normalGeneration();acknowledged_=menu.profile();acknowledgedConfirmation_=menu.normalNameConfirmationSerial();return true;
    }
    bool pending()const{return !pending_.empty();}
    bool settled(const Menu& menu)const{return generation_&&generation_==menu.normalGeneration()&&menu.sealedPostrun()&&!pending()&&acknowledged_==menu.profile()&&acknowledgedConfirmation_==menu.normalNameConfirmationSerial();}
    std::span<const std::uint8_t> packet()const{return pending_;}
    int prepare(const Menu& menu,std::string& error){
        error.clear();if(!generation_||generation_!=menu.normalGeneration()||!menu.sealedPostrun()||pending()){
            error="ordinary menu receipt generation or pending boundary changed";return -1;}
        const auto& profile=menu.profile();if(profile==acknowledged_&&menu.normalNameConfirmationSerial()==acknowledgedConfirmation_)return 0;
        pendingConfirmation_=menu.normalNameConfirmationSerial();
        if(profile.size()!=52232){error="ordinary full menu profile size changed";return -1;}
        const auto name=std::span<const std::uint8_t>(profile).subspan(record::NativeScoreProfile::StatsOffset+12,9);
        if(profile.size()!=52232||name[8]!=0||std::any_of(name.begin(),name.begin()+8,[](unsigned c){return c<32||c>126;})){
            error="ordinary full menu profile/name domain rejected";return -1;}
        const int position=menu.normalScorePosition();const unsigned route=menu.normalRoute(),rank=menu.normalRank();
        const unsigned rowName=position<0?0:route*1644+16+rank*320+unsigned(position)*32+6;
        if(menu.normalNameCommitted()&&position>=0&&!std::equal(name.begin(),name.end(),profile.begin()+rowName)){
            error="ordinary selected score row/ST+12/independent9B name differ";return -1;}
        if(serial_==UINT32_MAX){error="ordinary menu receipt serial exhausted";return -1;}++serial_;tick_=menu.ticks();
        const auto word=[this](unsigned value){for(unsigned i=0;i<4;++i)pending_.push_back(std::uint8_t(value>>(i*8)));};
        word(0x31504d4eu);word(1);word(80+52232+9);word(generation_);word(serial_);word(tick_);
        word(route);word(rank);word(unsigned(position+1));word(unsigned(menu.normalScore()));word(menu.normalContinues());
        word(52232);word(digest(profile));word(9);word(menu.normalPhase());word(menu.normalSubphase()|(menu.normalNameCommitted()?0x100u:0u)|(menu.normalReplayNameCommitted()?0x200u:0u));
        word(menu.normalFrames());word(record::NativeScoreProfile::StatsOffset+12);word(rowName);word(menu.extraSealed()?1:0);
        pending_.insert(pending_.end(),profile.begin(),profile.end());pending_.insert(pending_.end(),name.begin(),name.end());return 1;
    }
    bool acknowledge(const Menu& menu,std::span<const std::uint8_t> bytes,std::string& error){
        error.clear();if(!pending()||!menu.sealedPostrun()||generation_!=menu.normalGeneration()||tick_!=menu.ticks()
            ||pendingConfirmation_!=menu.normalNameConfirmationSerial()||bytes.size()!=pending_.size()||!std::equal(bytes.begin(),bytes.end(),pending_.begin())
            ||!std::equal(menu.profile().begin(),menu.profile().end(),pending_.begin()+80)){
            error="ordinary complete menu profile/name receipt identity changed before ACK";return false;}
        acknowledged_=menu.profile();acknowledgedConfirmation_=pendingConfirmation_;pending_.clear();return true;
    }
};
}
