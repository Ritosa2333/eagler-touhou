#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>
namespace th128::record {
struct ReplayPlaybackInitial {
    unsigned stage=0,rank=0,seed=0,shot=0,continues=0,generation=0,worldFrame=0;
    std::int32_t score=0,pointValue=0,horizontal=0,vertical=0,motivation=0,perfect=0;
    float frozenArea=0;
    std::int32_t graze=0;
};
class OwnedReplayPlaybackInput {
    std::vector<std::uint8_t> input_;
    unsigned generation_=0,stage_=0,rank_=0,frames_=0,consumed_=0;
    bool active_=false,pending_=false;
    static std::uint32_t word(std::span<const std::uint8_t> bytes,unsigned offset){
        return std::uint32_t(bytes[offset])|(std::uint32_t(bytes[offset+1])<<8)
            |(std::uint32_t(bytes[offset+2])<<16)|(std::uint32_t(bytes[offset+3])<<24);
    }
    static unsigned half(std::span<const std::uint8_t> bytes,unsigned offset){
        return unsigned(bytes[offset])|(unsigned(bytes[offset+1])<<8);
    }
public:
    void bind(std::span<const std::uint8_t> bytes,const ReplayPlaybackInitial& actual){
        if(active_||!actual.generation||actual.worldFrame||bytes.size()<144||bytes.size()>6000144)
            throw std::runtime_error("replay playback requires fresh actual owner and bounded stage bytes");
        const unsigned stage=half(bytes,0),seed=half(bytes,2),frames=word(bytes,4);
        const bool exact=stage==actual.stage&&seed==actual.seed&&actual.rank<=4
            &&(actual.rank==4)==(stage==16)&&frames>0&&frames<=1000000&&bytes.size()==144+std::size_t(frames)*6
            &&word(bytes,8)>=frames*6&&half(bytes,16)==actual.shot&&word(bytes,32)==actual.continues
            &&std::bit_cast<std::int32_t>(word(bytes,12))==actual.score
            &&std::bit_cast<std::int32_t>(word(bytes,20))==actual.pointValue
            &&std::bit_cast<std::int32_t>(word(bytes,24))==actual.horizontal
            &&std::bit_cast<std::int32_t>(word(bytes,28))==actual.vertical
            &&std::bit_cast<std::int32_t>(word(bytes,128))==actual.motivation
            &&std::bit_cast<std::int32_t>(word(bytes,132))==actual.perfect
            &&word(bytes,136)==std::bit_cast<std::uint32_t>(actual.frozenArea)
            &&actual.graze>=0&&actual.graze<=1000000000
            &&std::bit_cast<std::int32_t>(word(bytes,0x28))==actual.graze;
        if(!exact)throw std::runtime_error("recorded replay header differs from actual stage seed resources or position");
        std::vector<std::uint8_t> candidate(bytes.begin()+144,bytes.end());
        input_=std::move(candidate);generation_=actual.generation;stage_=stage;rank_=actual.rank;
        frames_=frames;consumed_=0;active_=true;pending_=false;
    }
    bool prepare(unsigned generation,bool auxiliaryKnown,bool auxiliaryLive,std::array<std::uint16_t,3>& packet){
        if(!active_||generation!=generation_||pending_||!auxiliaryKnown)
            throw std::runtime_error("replay playback input ownership or message auxiliary boundary is unavailable");
        if(auxiliaryLive)return false;
        if(consumed_>=frames_)throw std::runtime_error("replay playback exhausted its actual recorded frames");
        const auto bytes=std::span<const std::uint8_t>(input_);
        for(unsigned index=0;index<3;++index)packet[index]=std::uint16_t(half(bytes,consumed_*6+index*2));
        pending_=true;return true;
    }
    void commit(unsigned generation,bool tickSucceeded){
        if(!active_||generation!=generation_)throw std::runtime_error("replay playback commit generation differs");
        if(!tickSucceeded)return;
        if(pending_){++consumed_;pending_=false;}
    }
    void cancel(){input_.clear();active_=pending_=false;generation_=stage_=rank_=frames_=consumed_=0;}
    bool active()const{return active_;}
    bool pending()const{return pending_;}
    unsigned stage()const{return stage_;}
    unsigned rank()const{return rank_;}
    unsigned frames()const{return frames_;}
    unsigned consumed()const{return consumed_;}
    unsigned generation()const{return generation_;}
};
}
