// 连续探针的音乐/标题平台返回流。只回放合法平台返回值，原现场用于逐边界核对。
#pragma once
#include <cstdint>
#include <cstring>
#include <istream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
namespace kaikeidou {
struct PresentationWorldProbeState {
    using Bytes=std::vector<std::uint8_t>;
    struct Call {
        std::uint32_t tick=0,frame=0,owner=0,api=0,result=0,request=0;
        std::vector<std::uint32_t> arguments;
        std::string path;
        Bytes before,after;
    };
    struct Request {
        std::uint32_t tick=0,frame=0,owner=0,operation=0;
        std::vector<std::uint32_t> arguments,calls;
        Bytes before,after;
    };
    struct Active {std::size_t request=0,consumed=0;};
    std::vector<Call> calls;
    std::vector<Request> requests;
    std::vector<Active> active;
    std::size_t next_call=0,next_request=0;
    static std::size_t state_size(std::uint32_t owner){
        if(owner==1)return 88;
        if(owner==2)return 16;
        throw std::runtime_error("Presentation owner invalid");
    }
    static std::uint32_t read_word(std::istream& stream){
        std::uint32_t value=0;stream.read(reinterpret_cast<char*>(&value),4);
        if(!stream)throw std::runtime_error("Presentation input truncated");return value;
    }
    static std::vector<std::uint32_t> read_words(std::istream& stream,std::uint32_t maximum){
        const auto count=read_word(stream);if(count>maximum)throw std::runtime_error("Presentation word count invalid");
        std::vector<std::uint32_t> result;result.reserve(count);
        for(std::uint32_t i=0;i<count;++i)result.push_back(read_word(stream));return result;
    }
    static Bytes read_bytes(std::istream& stream,std::size_t expected){
        const auto count=read_word(stream);if(count!=expected)throw std::runtime_error("Presentation state size invalid");
        Bytes bytes(count);stream.read(reinterpret_cast<char*>(bytes.data()),count);
        if(!stream)throw std::runtime_error("Presentation state truncated");return bytes;
    }
    static void require_state(const Bytes& wanted,const void* actual,std::size_t size,const char* boundary){
        if(wanted.size()!=size||std::memcmp(wanted.data(),actual,size)!=0){
            std::size_t at=0;while(at<wanted.size()&&at<size&&wanted[at]==static_cast<const std::uint8_t*>(actual)[at])++at;
            throw std::runtime_error(std::string("Presentation ")+boundary+" state differs at byte "+std::to_string(at));
        }
    }
    void read(std::istream& stream){
        if(read_word(stream)!=0x49303750u)throw std::runtime_error("Presentation stream version differs");
        const auto count=read_word(stream);if(count>100000)throw std::runtime_error("Presentation call count invalid");calls.resize(count);
        constexpr std::uint32_t argument_counts[9]={0,1,1,2,3,2,3,2,2};
        for(auto& call:calls){
            call.tick=read_word(stream);call.frame=read_word(stream);call.owner=read_word(stream);call.api=read_word(stream);
            call.result=read_word(stream);call.request=read_word(stream);call.arguments=read_words(stream,8);
            if(call.owner==1&&call.request==0)throw std::runtime_error("Music platform call lacks request");
            if(call.api<1||call.api>8||call.arguments.size()!=argument_counts[call.api]||call.owner!=(call.api<=6?1u:2u))
                throw std::runtime_error("Presentation API contract differs");
            const auto path_size=read_word(stream);if(path_size>4096)throw std::runtime_error("Presentation path too long");
            call.path.resize(path_size);if(path_size)stream.read(call.path.data(),path_size);
            if(!stream)throw std::runtime_error("Presentation path truncated");
            if(call.api==4||call.api==7){
                if(call.path.empty()||call.path.back()!=0||call.arguments[0]!=0)throw std::runtime_error("Presentation load path invalid");
                call.path.pop_back();if(call.path.find('\0')!=std::string::npos)throw std::runtime_error("Presentation path contains embedded zero");
            }else if(!call.path.empty())throw std::runtime_error("Unexpected presentation path");
            call.before=read_bytes(stream,state_size(call.owner));call.after=read_bytes(stream,state_size(call.owner));
        }
        const auto request_count=read_word(stream);if(request_count>100000)throw std::runtime_error("Presentation request count invalid");requests.resize(request_count);
        for(auto& request:requests){
            request.tick=read_word(stream);request.frame=read_word(stream);request.owner=read_word(stream);request.operation=read_word(stream);request.arguments=read_words(stream,2);
            if(request.operation<1||request.operation>3||request.owner!=(request.operation==2?2u:1u)||request.arguments.size()!=(request.operation==1?2u:0u))
                throw std::runtime_error("Presentation request contract differs");
            request.before=read_bytes(stream,state_size(request.owner));request.after=read_bytes(stream,state_size(request.owner));
            request.calls=read_words(stream,count);
            for(auto sequence:request.calls)if(sequence==0||sequence>count)throw std::runtime_error("Presentation request call index invalid");
        }
        std::vector<unsigned> membership(calls.size(),0);
        for(std::size_t i=0;i<requests.size();++i)for(auto sequence:requests[i].calls){
            const auto index=sequence-1u;
            if(++membership[index]!=1||calls[index].request!=i+1u||calls[index].owner!=requests[i].owner)
                throw std::runtime_error("Presentation request membership differs");
        }
        for(std::size_t i=0;i<calls.size();++i)if((calls[i].request!=0)!=(membership[i]!=0))
            throw std::runtime_error("Presentation orphan request call");
    }
    void begin(std::uint32_t tick,std::uint32_t frame,std::uint32_t owner,std::uint32_t operation,const std::vector<std::uint32_t>& arguments,const void* state,std::size_t size){
        if(next_request==requests.size())throw std::runtime_error("Unexpected presentation request");
        const auto index=next_request++;const auto& request=requests[index];
        if(request.tick!=tick||request.frame!=frame||request.owner!=owner||request.operation!=operation||request.arguments!=arguments)
            throw std::runtime_error("Presentation request differs at sequence "+std::to_string(index+1)+", tick "+std::to_string(tick));
        require_state(request.before,state,size,"request entry");active.push_back({index,0});
    }
    void end(const void* state,std::size_t size){
        if(active.empty())throw std::runtime_error("Presentation request return without entry");
        const auto& current=active.back();const auto& request=requests[current.request];
        if(current.consumed!=request.calls.size())throw std::runtime_error("Presentation request calls unconsumed");
        require_state(request.after,state,size,"request return");active.pop_back();
    }
    std::uint32_t consume(std::uint32_t tick,std::uint32_t frame,std::uint32_t owner,std::uint32_t api,
        const std::vector<std::uint32_t>& arguments,const void* state,std::size_t size,std::string_view path={}){
        if(next_call==calls.size())throw std::runtime_error("Unexpected presentation platform call");
        const auto sequence=next_call+1;const auto& call=calls[next_call];
        if(call.tick!=tick||call.frame!=frame||call.owner!=owner||call.api!=api||call.arguments!=arguments||call.path!=path)
            throw std::runtime_error("Presentation platform call differs at sequence "+std::to_string(sequence)+", tick "+std::to_string(tick));
        if(call.request){
            if(active.empty()||active.back().request+1!=call.request)throw std::runtime_error("Presentation platform request nesting differs");
            auto& current=active.back();const auto& expected=requests[current.request].calls;
            if(current.consumed==expected.size()||expected[current.consumed]!=sequence)throw std::runtime_error("Presentation platform order differs");
            ++current.consumed;
        }else if(!active.empty())throw std::runtime_error("Unowned presentation platform call inside request");
        require_state(call.before,state,size,"platform entry");
        require_state(call.after,state,size,"platform return");
        ++next_call;return call.result;
    }
    void finish_tick(std::uint32_t tick)const{
        if(!active.empty()||(next_call<calls.size()&&calls[next_call].tick<=tick)||(next_request<requests.size()&&requests[next_request].tick<=tick))
            throw std::runtime_error("Captured presentation boundary skipped at tick "+std::to_string(tick));
    }
    void finish()const{
        if(next_call!=calls.size()||next_request!=requests.size()||!active.empty())throw std::runtime_error("Captured presentation input remains unconsumed");
    }
};
}
