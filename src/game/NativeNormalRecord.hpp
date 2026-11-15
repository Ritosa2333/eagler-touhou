#pragma once
#include "NativeRecordAccounting.hpp"
#include "NativeRecordBootstrap.hpp"
#include "NativeRecordTiming.hpp"
#include "NativeScoreProfile.hpp"
#include <array>
#include <span>
#include <vector>
#include <cstdint>
#include <stdexcept>

namespace th128::record {

// Ordinary scores own a separate profile; diagnostic postrun owners remain tainted.
class NativeNormalRecord {
    std::vector<std::uint8_t> profile_, pending_;
    NativeRecordTiming timing_;
    unsigned route_=0,rank_=0,stage_=0,frames_=0,presented_=0,digest_=0;
    bool terminalStatsPrepared_=false;
    std::int32_t terminalScore_=0;
    unsigned terminalContinues_=0;
    double startedSeconds_=0,lastSeconds_=0;
    bool started_=false,prepared_=false,acknowledged_=false,terminalClearRecorded_=false;

    static std::uint32_t read32(std::span<const std::uint8_t> b,std::size_t p){
        return std::uint32_t(b[p])|(std::uint32_t(b[p+1])<<8)|(std::uint32_t(b[p+2])<<16)|(std::uint32_t(b[p+3])<<24);
    }
    static void write32(std::span<std::uint8_t> b,std::size_t p,std::uint32_t value){
        for(unsigned byte=0;byte<4;++byte)b[p+byte]=std::uint8_t(value>>(byte*8));
    }
    void incrementCD(std::size_t offset){
        const auto value=read32(profile_,offset);
        if(std::bit_cast<std::int32_t>(value)<99999)write32(profile_,offset,value+1u);
    }
    std::size_t spellRow(int id)const{
        if(!started_||prepared_||id<0||id>=250)throw std::runtime_error("ordinary spell record lacks its actual run/card owner");
        return NativeScoreProfile::ClearOffset+12+std::size_t(id)*152;
    }
    static bool next(unsigned a,unsigned b){
        return (a==1&&(b==2||b==4))||(a==2&&b==3)||(a==4&&b==5)
            ||(a==6&&(b==7||b==9))||(a==7&&b==8)||(a==9&&b==10)
            ||(a==11&&(b==12||b==14))||(a==12&&b==13)||(a==14&&b==15);
    }
    friend class NativeOrdinaryTerminalOwner;
    void commitOrdinaryClear(unsigned stage,unsigned route,unsigned rank){
        if(!started_||prepared_||acknowledged_||terminalClearRecorded_||stage_!=stage||route_!=route||rank_!=rank)
            throw std::runtime_error("ordinary clear record lacks its uncommitted terminal owner");
        NativeRecordAccounting::recordClear(profile_,route_,rank_,0);
        terminalClearRecorded_=true;
    }
public:
    explicit NativeNormalRecord(std::span<const std::uint8_t> profile){
        if(profile.empty()){
            std::array<std::uint8_t,250> ranks{};for(unsigned i=0;i<250;++i)ranks[i]=i<240?i%4:4;
            constexpr std::array<std::uint8_t,9> routeName{45,45,45,45,45,45,45,45,0};
            constexpr std::array<std::uint8_t,9> lastName{32,32,32,32,32,32,32,32,0};
            Random metadataRandom{};
            profile_=NativeRecordBootstrap::create(metadataRandom,ranks,routeName,lastName);
        }else{
            NativeScoreProfile::assemble(profile);profile_.assign(profile.begin(),profile.end());
        }
    }
    void beginStage(unsigned stage,unsigned rank,double seconds){
        if(prepared_||!std::isfinite(seconds)||seconds<lastSeconds_||rank>=4)throw std::runtime_error("ordinary record stage clock rejected");
        if(!started_){
            if(stage!=1&&stage!=6&&stage!=11)throw std::runtime_error("ordinary records require a natural route origin");
            route_=stage==1?0:stage==6?2:4;rank_=rank;startedSeconds_=seconds;
            NativeRecordAccounting::beginFresh(profile_,route_,rank_,0);started_=true;
        }else{
            if(rank!=rank_||!next(stage_,stage))throw std::runtime_error("ordinary record natural handoff rejected");
            if(stage==4||stage==9||stage==14)++route_;
        }
        stage_=stage;lastSeconds_=seconds;
    }
    void activeFrame(std::uint32_t flags,bool held){
        if(!started_||prepared_)throw std::runtime_error("ordinary record active frame lacks its run owner");
        NativeRecordAccounting::recordActiveFrame(profile_,route_,0,flags,held);++frames_;
    }
    void present(double seconds,unsigned replayFrames,bool paused){
        if(!started_||prepared_||!std::isfinite(seconds)||seconds<lastSeconds_||replayFrames!=frames_)throw std::runtime_error("ordinary record presentation clock rejected");
        lastSeconds_=seconds;
        // A repeated display of one logic frame is not an additional original driver sample.
        if(replayFrames==presented_)return;
        std::uint32_t flags=paused?0x10u:0u;timing_.sample(seconds-startedSeconds_,true,flags,0);presented_=replayFrames;
    }
    void beginSpell(int id,std::span<const std::uint8_t> name){
        const auto row=spellRow(id);
        if(name.empty()||name.size()>=128||std::find(name.begin(),name.end(),std::uint8_t{0})!=name.end())
            throw std::runtime_error("ordinary spell name is outside the original NUL-terminated field");
        std::copy(name.begin(),name.end(),profile_.begin()+row);profile_[row+name.size()]=0;
        incrementCD(row+140);
    }
    void endSpell(int id,std::uint32_t flags){
        if(!(flags&1u))return;
        const auto row=spellRow(id);
        incrementCD(row+136);
        if(flags&2u){incrementCD(row+128);if(flags&4u)incrementCD(row+132);}
    }
private:
    void prepareOwned(std::int32_t score,unsigned continues,std::uint64_t timestamp,double seconds,unsigned replayFrames,bool insertScore){
        if(!started_||prepared_||!frames_||replayFrames!=frames_||score<0||score>999999999||continues>9
            ||timestamp>0x793406fffull||!std::isfinite(seconds)||seconds<lastSeconds_)throw std::runtime_error("ordinary record final native identity rejected");
        pending_=profile_;const auto first=std::size_t(route_)*1644+16+rank_*320;
        if(insertScore&&score>=std::int32_t(read32(profile_,first+9*32))){
            const auto& counters=timing_.state();
            if(counters.denominator<=0)throw std::runtime_error("ordinary score slowdown has no completed real clock window");
            std::array<std::uint8_t,9> name{};std::copy_n(profile_.begin()+NativeScoreProfile::StatsOffset+12,9,name.begin());
            NativeScoreProfile::insert(pending_,{route_,rank_,score,stage_,continues,timestamp,counters.numerator,counters.denominator,name});
        }
        NativeRecordAccounting::addPlaytime(pending_,seconds-startedSeconds_,0,16);
        digest_=2166136261u;for(auto byte:pending_){digest_^=byte;digest_*=16777619u;}
        prepared_=true;lastSeconds_=seconds;
    }
    void prepareOrdinaryTerminalStats(std::int32_t score,unsigned continues,std::uint64_t timestamp,double seconds,unsigned replayFrames){
        if(!terminalClearRecorded_)throw std::runtime_error("ordinary terminal statistics require the recorded native clear owner");
        prepareOwned(score,continues,timestamp,seconds,replayFrames,false);
        terminalScore_=score;terminalContinues_=continues;terminalStatsPrepared_=true;
    }
public:
    void prepare(std::int32_t score,unsigned continues,std::uint64_t timestamp,double seconds,unsigned replayFrames){
        prepareOwned(score,continues,timestamp,seconds,replayFrames,true);
    }
    void acknowledge(unsigned serial,unsigned bytes,unsigned digest){
        if(!prepared_||acknowledged_||serial!=1||bytes!=pending_.size()||digest!=digest_)throw std::runtime_error("ordinary record full durable identity rejected");
        acknowledged_=true;
    }
    bool terminalStatsPrepared()const noexcept{return terminalStatsPrepared_;}
    std::int32_t terminalScore()const noexcept{return terminalScore_;}
    unsigned terminalContinues()const noexcept{return terminalContinues_;}
    const NativeFrameStatisticsState& statistics()const noexcept{return timing_.state();}
    unsigned highScoreRaw()const{const auto v=read32(profile_,std::size_t(route_)*1644+16+rank_*320);if(v>999999999)throw std::runtime_error("ordinary highscore raw value rejected");return v;}
    unsigned highScoreContinue()const{return profile_[std::size_t(route_)*1644+16+rank_*320+5];}
    bool started()const{return started_;}bool prepared()const{return prepared_;}bool acknowledged()const{return acknowledged_;}
    unsigned route()const{return route_;}unsigned rank()const{return rank_;}unsigned frames()const{return frames_;}unsigned digest()const{return digest_;}
    unsigned stage()const{return stage_;}
    const std::vector<std::uint8_t>& pending()const{return pending_;}
};
}
