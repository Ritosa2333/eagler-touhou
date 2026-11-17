#pragma once
#include "game/NativeReplayInput.hpp"
#include "game/TouchMotion.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace th128::record {
class NativeRunReplay {
    struct Stage {
        std::array<std::uint8_t,144> header{};
        NativeReplayInput input;
        unsigned worldFrames=0;
        th128::player::TouchMotion touch{};
        std::vector<std::array<std::int32_t,4>> touchEvents;
        std::vector<std::array<unsigned,4>> auxiliary;
    };
    std::vector<Stage> stages_;
    unsigned route_=0,rank_=0,lastStage_=0;
    bool practice_=false,assistedMovement_=false;
    double seconds_=0,activeSeconds_=0;
    static void store16(std::span<std::uint8_t> bytes,std::size_t offset,std::uint16_t value){bytes[offset]=std::uint8_t(value);bytes[offset+1]=std::uint8_t(value>>8);}
    static void store32(std::span<std::uint8_t> bytes,std::size_t offset,std::uint32_t value){for(unsigned index=0;index<4;++index)bytes[offset+index]=std::uint8_t(value>>(index*8));}
    static std::array<std::uint8_t,60> configuration(){
        std::array<std::uint8_t,60> bytes{};store32(bytes,0,0x128001);
        constexpr std::array<std::uint16_t,9> mapping{0,1,3,2,4,65535,65535,65535,65535};
        for(std::size_t index=0;index<mapping.size();++index)store16(bytes,4+index*2,mapping[index]);
        store32(bytes,0x16,0x2580258);store32(bytes,0x1a,0x10100);store16(bytes,0x1f,0x6402);bytes[0x21]=0x50;store16(bytes,0x22,0x200);
        store32(bytes,0x28,0x80000000);store32(bytes,0x2c,0x80000000);store32(bytes,0x38,0x100);return bytes;
    }
    static bool nextStage(unsigned source,unsigned target){
        constexpr std::array<unsigned,16> first{0,2,3,0,5,0,7,8,0,10,0,12,13,0,15,0};
        return source<first.size()&&(first[source]==target||(source==1&&target==4)||(source==6&&target==9)||(source==11&&target==14));
    }
public:
    static NativeRunReplay practice(){NativeRunReplay result;result.practice_=true;return result;}
    void markAssistedMovement(){if(stages_.empty())throw std::runtime_error("assisted movement lacks actual native run history");assistedMovement_=true;}
    bool assistedMovement()const noexcept{return assistedMovement_;}
    bool empty()const{return stages_.empty();}
    unsigned frames()const{unsigned count=0;for(const auto& stage:stages_)count+=unsigned(stage.input.frame());return count;}
    unsigned stages()const{return unsigned(stages_.size());}
    unsigned stageFrames()const{return stages_.empty()?0:unsigned(stages_.back().input.frame());}
    unsigned stage()const{return lastStage_;}
    unsigned route()const{return route_;}
    unsigned rank()const{return rank_;}
    bool configuredFor(unsigned id)const{return !stages_.empty()&&lastStage_==id;}
    void begin(unsigned stage,unsigned rank,unsigned seed,unsigned shot,unsigned continues,std::int32_t score,
               std::int32_t pointValue,std::int32_t motivation,std::int32_t perfect,float frozenArea,
               std::int32_t horizontal,std::int32_t vertical,std::int32_t graze=0){
        if(stage<1||stage>16||rank>4||(rank==4)!=(stage==16)||seed>65535||shot>15||continues>9||score<0||score>999999999
           ||pointValue<0||motivation<0||motivation>100000||perfect<0||perfect>30000||!std::isfinite(frozenArea)||frozenArea<0
           ||graze<0||graze>1000000000||stages_.size()>=3)throw std::runtime_error("normal replay initial native stage fields rejected");
        if(stages_.empty()){
            if(!practice_&&stage!=1&&stage!=6&&stage!=11&&stage!=16)throw std::runtime_error("normal replay must start at original route beginning");
        }else if(rank!=rank_||!nextStage(lastStage_,stage))throw std::runtime_error("normal replay stage carry identity rejected");
        Stage candidate;candidate.input.beginStageBuffers(stage);
        store16(candidate.header,0,std::uint16_t(stage));store16(candidate.header,2,std::uint16_t(seed));
        store32(candidate.header,0xc,std::bit_cast<std::uint32_t>(score));store16(candidate.header,0x10,std::uint16_t(shot));
        store32(candidate.header,0x14,std::bit_cast<std::uint32_t>(pointValue));store32(candidate.header,0x18,std::bit_cast<std::uint32_t>(horizontal));
        store32(candidate.header,0x1c,std::bit_cast<std::uint32_t>(vertical));store32(candidate.header,0x20,continues);
        store32(candidate.header,0x28,std::bit_cast<std::uint32_t>(graze));
        std::uint32_t value=0;for(unsigned index=0;index<20;++index){store32(candidate.header,0x30+index*4,value);value-=0x21522153u;}
        store32(candidate.header,0x80,std::bit_cast<std::uint32_t>(motivation));store32(candidate.header,0x84,std::bit_cast<std::uint32_t>(perfect));
        store32(candidate.header,0x88,std::bit_cast<std::uint32_t>(frozenArea));store32(candidate.header,0x8c,1);
        if(stages_.empty())route_=practice_?(stage==16?6:stage<=5?(stage>=4?1:0):stage<=10?(stage>=9?3:2):(stage>=14?5:4)):(stage==16?6:stage==11?4:stage==6?2:0);
        else if(stage==4||stage==9||stage==14)++route_;
        stages_.push_back(std::move(candidate));lastStage_=stage;rank_=rank;
    }
    bool append(std::array<std::uint16_t,3> input,bool messageAuxiliary,float actualFPS,double actualSeconds,
                th128::player::TouchMotion touch={}){
        if(stages_.empty()||!std::isfinite(actualSeconds)||actualSeconds<seconds_||!std::isfinite(actualFPS)||actualFPS<0||frames()>=1000000)
            throw std::runtime_error("normal native replay input/clock boundary rejected");
        if(!th128::player::validTouch(touch))throw std::runtime_error("replay touch command rejected");
        auto& stage=stages_.back();
        if(stage.worldFrames>=1000000||stage.touchEvents.size()>=100000)throw std::runtime_error("replay touch history budget");
        if(touch!=stage.touch){stage.touchEvents.push_back({std::int32_t(stage.worldFrames),std::int32_t(touch.mode),touch.x,touch.y});stage.touch=touch;}
        if(messageAuxiliary){
            if(stage.auxiliary.size()>=12000)throw std::runtime_error("replay auxiliary input budget");
            stage.auxiliary.push_back({stage.worldFrames,input[0],input[1],input[2]});
        }
        ++stage.worldFrames;
        const auto delta=actualSeconds-seconds_;seconds_=actualSeconds;
        if(stages_.back().input.append({true,true,messageAuxiliary,actualFPS,input})){activeSeconds_+=delta;return true;}
        return false;
    }
    std::vector<std::uint8_t> finishOrdinary(std::uint64_t timestamp,std::int32_t score,
            const std::array<std::uint8_t,8>& name,double numerator,double denominator)const{
        static constexpr std::array<unsigned,6> terminals{3,5,8,10,13,15};
        if(route_>=6||rank_>=4||stages_.size()!=3||lastStage_!=terminals[route_]
            ||!std::isfinite(numerator)||numerator<0||!std::isfinite(denominator)||denominator<=0)
            throw std::runtime_error("ordinary completed replay lacks its three-stage native history/statistics");
        auto result=snapshot(timestamp,score,route_+17,0,name);
        const auto ratio=static_cast<float>(numerator/denominator);
        const auto slowdown=assistedMovement_?100.f:static_cast<float>(100.-100.*static_cast<double>(ratio));
        if(!std::isfinite(slowdown))throw std::runtime_error("ordinary completed replay slowdown is not finite");
        store32(result,36+84,std::bit_cast<std::uint32_t>(slowdown));return result;
    }
    std::vector<std::uint8_t> finishExtra(std::uint64_t timestamp,std::int32_t score,const std::array<std::uint8_t,8>& name,double numerator,double denominator)const{
        if(route_!=6||rank_!=4||stages_.size()!=1||lastStage_!=16||!frames()||!std::isfinite(numerator)||numerator<0||!std::isfinite(denominator)||denominator<=0)
            throw std::runtime_error("completed EX replay lacks its actual one-stage history/statistics");
        auto result=snapshot(timestamp,score,23,0,name);const auto ratio=static_cast<float>(numerator/denominator),slowdown=assistedMovement_?100.f:static_cast<float>(100.-100.*double(ratio));
        if(!std::isfinite(slowdown))throw std::runtime_error("EX replay slowdown is not finite");store32(result,36+84,std::bit_cast<std::uint32_t>(slowdown));return result;
    }
    std::vector<std::uint8_t> snapshot(std::uint64_t timestamp,std::int32_t score,unsigned currentStage,unsigned continues,
                                      const std::array<std::uint8_t,8>& name)const{
        if(stages_.empty()||!frames()||timestamp>0x793406fffull||score<0||score>999999999||currentStage>23||continues>9)
            throw std::runtime_error("normal native replay snapshot has incomplete owner");
        std::vector<std::uint8_t> result(148);store32(result,0,0x72383231);store32(result,4,2);store32(result,0x10,0x100);
        std::copy(name.begin(),name.end(),result.begin()+36);
        for(unsigned index=0;index<8;++index)result[36+12+index]=std::uint8_t(timestamp>>(index*8));
        store32(result,36+20,std::uint32_t(score));const auto config=configuration();std::copy(config.begin(),config.end(),result.begin()+36+24);
        const float ratio=seconds_>0?float(activeSeconds_/seconds_):1.f;
        const float slowdown=assistedMovement_?100.f:float(100.-100.*double(ratio));store32(result,36+84,std::bit_cast<std::uint32_t>(slowdown));
        store32(result,36+88,unsigned(stages_.size()));store32(result,36+92,route_);store32(result,36+100,rank_);store32(result,36+104,currentStage);
        for(const auto& stage:stages_){
            auto header=stage.header;std::vector<std::uint8_t> inputs,fps;
            for(const auto& chunk:stage.input.chunks()){
                inputs.insert(inputs.end(),chunk.input.begin(),chunk.input.begin()+chunk.packets*6);
                fps.insert(fps.end(),chunk.fps.begin(),chunk.fps.begin()+chunk.samples);
            }
            store32(header,4,unsigned(stage.input.frame()));store32(header,8,unsigned(inputs.size()+fps.size()));
            result.insert(result.end(),header.begin(),header.end());result.insert(result.end(),inputs.begin(),inputs.end());result.insert(result.end(),fps.begin(),fps.end());
        }
        const bool auxiliary=std::any_of(stages_.begin(),stages_.end(),[](const Stage& stage){return !stage.auxiliary.empty();});
        if(auxiliary||std::any_of(stages_.begin(),stages_.end(),[](const Stage& stage){return !stage.touchEvents.empty();})){
            const auto appendWord=[&](unsigned value){const auto at=result.size();result.resize(at+4);store32(result,at,value);};
            appendWord(0x31484354u);appendWord(auxiliary?2:1);appendWord(unsigned(stages_.size()));
            for(const auto& stage:stages_){appendWord(stage.input.stage());appendWord(stage.worldFrames);appendWord(unsigned(stage.touchEvents.size()));
                if(auxiliary)appendWord(unsigned(stage.auxiliary.size()));
                for(const auto& event:stage.touchEvents)for(const auto value:event)appendWord(std::bit_cast<unsigned>(value));
                if(auxiliary)for(const auto& event:stage.auxiliary)for(const auto value:event)appendWord(value);}
        }
        return result;
    }
};
}
