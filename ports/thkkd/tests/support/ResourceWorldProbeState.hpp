// 连续对照的59区扩展及原平台返回输入；逐调用核对背景完整现场。
#pragma once
#include "core/stages/StageResourceContext.hpp"
#include <cstring>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
namespace kaikeidou {
struct ResourceWorldProbeState {
    static constexpr std::size_t byte_count=17916;
    StageResourceContext context;
    BackgroundMeshPool mesh;
    struct Call {
        std::uint32_t tick,kind,argument,flag,result,stage,part,character,frame;
        std::string path;
        std::array<std::uint8_t,byte_count> before{};
    };
    std::vector<Call> calls;
    std::size_t next_call=0;
    void read(std::istream& stream) {
        const auto raw=[&](auto& data){stream.read(reinterpret_cast<char*>(&data),sizeof(data));};
        raw(context.handles);raw(context.reals);raw(context.integers);raw(mesh.meshes);raw(mesh.count);
        if(!stream)throw std::runtime_error("Background state truncated");
    }
    void write(std::ostream& stream) const {
        const auto raw=[&](const auto& data){stream.write(reinterpret_cast<const char*>(&data),sizeof(data));};
        raw(context.handles);raw(context.reals);raw(context.integers);raw(mesh.meshes);raw(mesh.count);
    }
    std::array<std::uint8_t,byte_count> bytes() const {
        std::array<std::uint8_t,byte_count> result{};std::size_t offset=0;
        const auto raw=[&](const auto& data){std::memcpy(result.data()+offset,&data,sizeof(data));offset+=sizeof(data);};
        raw(context.handles);raw(context.reals);raw(context.integers);raw(mesh.meshes);raw(mesh.count);
        if(offset!=result.size())throw std::runtime_error("Background layout differs");return result;
    }
    void read_calls(std::istream& stream) {
        const auto word=[&](){std::uint32_t value=0;stream.read(reinterpret_cast<char*>(&value),4);if(!stream)throw std::runtime_error("Platform input truncated");return value;};
        const auto count=word();if(count>10000)throw std::runtime_error("Platform input count invalid");calls.resize(count);
        for(auto& call:calls){
            call.tick=word();call.kind=word();call.argument=word();call.flag=word();call.result=word();
            call.stage=word();call.part=word();call.character=word();call.frame=word();const auto size=word();
            if(size>128)throw std::runtime_error("Platform path too long");call.path.resize(size);
            stream.read(call.path.data(),size);stream.read(reinterpret_cast<char*>(call.before.data()),call.before.size());
            if(!stream)throw std::runtime_error("Platform scene truncated");
        }
    }
    std::uint32_t consume(std::uint32_t tick,std::uint32_t frame,std::uint32_t kind,std::uint32_t argument,
                          std::uint32_t flag,std::string_view path={}) {
        if(next_call==calls.size())throw std::runtime_error("Platform capture lacks this resource call");const auto& call=calls[next_call];
        if(call.tick!=tick||call.frame!=frame||call.kind!=kind||call.flag!=flag||call.path!=path||
           (kind==1&&call.argument!=argument)||call.stage!=context.stage||call.part!=context.part||call.character!=context.character)
            throw std::runtime_error("Resource call differs from original platform input");
        if(call.before!=bytes())throw std::runtime_error("Resource call background state differs from original");
        ++next_call;return call.result;
    }
};
}
