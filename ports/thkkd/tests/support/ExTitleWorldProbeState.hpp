// 标题初态来自共同游戏初态；请求中的前后字节只用于比较，平台随机返回独立供给。
#pragma once
#include "core/frontend/MenuStartContext.hpp"
#include "core/frontend/MenuTitleReturnContext.hpp"
#include <array>
#include <cstring>
#include <istream>
#include <stdexcept>
#include <vector>
namespace kaikeidou {
struct ExTitleWorldProbeState {
    static constexpr std::uint32_t initial_magic=0x31495845u,request_magic=0x31525845u;
    static constexpr std::size_t state_bytes=2488;
    using Bytes=std::array<std::uint8_t,state_bytes>;
    struct Request {
        std::uint32_t tick=0,frame=0,precision=0,site=0;
        Bytes before{},after{};
        std::array<std::array<std::uint32_t,2>,235> random{};
    };
    MenuStartContext menu;
    MenuTitleReturnContext title;
    std::uint32_t initial_volume=0;
    std::vector<Request> requests;
    std::size_t next=0,random_index=0;
    bool active=false;
    static std::uint32_t word(std::istream& stream){
        std::uint32_t value=0;stream.read(reinterpret_cast<char*>(&value),4);
        if(!stream)throw std::runtime_error("EX title input truncated");return value;
    }
    template<class Value> static void read_raw(std::istream& stream,Value& value){
        stream.read(reinterpret_cast<char*>(&value),sizeof(value));
        if(!stream)throw std::runtime_error("EX title state truncated");
    }
    void read_initial(std::istream& stream){
        if(word(stream)!=initial_magic||word(stream)!=1||word(stream)!=state_bytes)
            throw std::runtime_error("EX title initial-state version differs");
        menu.outer_scene=word(stream);menu.phase=word(stream);menu.timer=word(stream);initial_volume=word(stream);
        title.replay_mode=word(stream);title.save_disabled=word(stream);menu.replay_submode=word(stream);title.save_choice=word(stream);
        read_raw(stream,title.prompt);title.eof_flags=word(stream);title.accumulator=word(stream);
        read_raw(stream,title.strips);read_raw(stream,title.particles);read_raw(stream,title.objects);
    }
    void require_initial_aliases(std::uint32_t scene,std::uint32_t phase,std::uint32_t timer,std::uint32_t volume)const{
        if(menu.outer_scene!=scene||menu.phase!=phase||menu.timer!=timer||initial_volume!=volume||title.replay_mode!=1)
            throw std::runtime_error("EX title common initial aliases differ");
    }
    Bytes bytes(std::uint32_t volume)const{
        Bytes result{};std::size_t offset=0;
        const auto append=[&](const auto& value){std::memcpy(result.data()+offset,&value,sizeof(value));offset+=sizeof(value);};
        append(menu.outer_scene);append(menu.phase);append(menu.timer);append(volume);
        append(title.replay_mode);append(title.save_disabled);append(menu.replay_submode);append(title.save_choice);
        append(title.prompt);append(title.eof_flags);append(title.accumulator);
        append(title.strips);append(title.particles);append(title.objects);
        if(offset!=state_bytes)throw std::runtime_error("EX title field layout differs");return result;
    }
    void read_requests(std::istream& stream){
        if(word(stream)!=request_magic||word(stream)!=1)throw std::runtime_error("EX title request version differs");
        const auto count=word(stream);if(count>1000)throw std::runtime_error("EX title request count invalid");requests.resize(count);
        std::uint32_t previous=0;
        for(auto& request:requests){
            request.tick=word(stream);request.frame=word(stream);request.precision=word(stream);request.site=word(stream);
            if(request.tick<previous||(request.precision!=24&&request.precision!=53&&request.precision!=64)||word(stream)!=235)
                throw std::runtime_error("EX title request metadata invalid");
            previous=request.tick;read_raw(stream,request.before);read_raw(stream,request.after);read_raw(stream,request.random);
        }
    }
    void begin(std::uint32_t tick,std::uint32_t frame,std::uint32_t site,std::uint32_t volume,unsigned precision){
        if(active||next>=requests.size())throw std::runtime_error("Unexpected EX title request");
        const auto& request=requests[next];
        if(request.tick!=tick||request.frame!=frame||request.site!=site||request.precision!=precision||request.before!=bytes(volume))
            throw std::runtime_error("EX title request entry differs");
        active=true;random_index=0;
    }
    std::uint32_t random(std::uint32_t maximum){
        if(!active||random_index>=235)throw std::runtime_error("Unexpected EX title random call");
        const auto& call=requests[next].random[random_index++];
        if(call[0]!=maximum)throw std::runtime_error("EX title random argument differs");return call[1];
    }
    void end(std::uint32_t volume){
        if(!active||random_index!=235||requests[next].after!=bytes(volume))
            throw std::runtime_error("EX title request return differs");
        active=false;++next;
    }
    void finish_tick(std::uint32_t tick)const{
        if(active||(next<requests.size()&&requests[next].tick<=tick))throw std::runtime_error("EX title request skipped");
    }
    void finish()const{
        if(active||next!=requests.size())throw std::runtime_error("EX title requests remain unconsumed");
    }
};
}
