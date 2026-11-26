// 完整65FEB0网格初始化；按原顺序存储binary32及六顶点UV/颜色。
#pragma once
#include "core/stages/StageResourceContext.hpp"
#include "core/math/BinaryFloat32.hpp"
#include "core/math/FiniteDivide.hpp"
namespace kaikeidou {
inline void initialize_background_mesh(BackgroundMeshPool& pool,const std::array<std::uint32_t,16>& args,unsigned precision){
    using namespace binary_float;
    const auto signed_word=[](std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);};
    if(signed_word(pool.count)>=10)return;
    const auto index=pool.count;pool.count+=1;auto& r=pool.meshes.at(index);
    for(const auto& f:std::array<std::array<unsigned,2>,6>{{{{4,0}},{{8,1}},{{12,2}},{{20,3}},{{16,4}},{{24,5}}}})r.word(f[0],args[f[1]]);
    const auto q=[](std::uint32_t v){return finite_binary32(v);};
    const auto sub=[&](Number a,Number b){b.negative=!b.negative;return add(a,b,precision);};
    r.word(28,to_binary32(q(args[6])));r.word(32,to_binary32(q(args[7])));r.word(0,args[8]);
    r.word(40,to_binary32(q(args[9])));r.word(48,to_binary32(q(args[10])));r.word(52,to_binary32(q(args[11])));r.word(44,to_binary32(q(args[12])));r.word(56,11);
    if(r.word(0)==0)r.word(56,args[15]);
    if(signed_word(r.word(56)-1u)<=0)return;
    r.word(36,to_binary32(divide(sub(q(r.word(40)),q(r.word(44))),integer(r.word(56)-1u),precision)));
    const auto u=to_binary32(divide(integer(r.word(12)),integer(r.word(8)),precision));
    const auto du=to_binary32(divide(integer(r.word(16)-r.word(12)),integer(r.word(8)),precision));
    const auto v=to_binary32(divide(integer(r.word(20)),integer(r.word(8)),precision));
    const auto dv=to_binary32(divide(integer(r.word(24)-r.word(20)),integer(r.word(8)),precision));
    if(signed_word(r.word(56))>11)throw std::out_of_range("Background mesh strips exceed recovered object storage");
    constexpr std::array<std::array<std::uint32_t,2>,6> uv{{{{0,0}},{{0x3f800000,0}},{{0,0x3f800000}},{{0x3f800000,0x3f800000}},{{0,0x3f800000}},{{0x3f800000,0}}}};
    for(std::uint32_t row=0;signed_word(row)<signed_word(r.word(56));++row){
        const auto offset=0x3c+row*0x9c;r.word(offset,to_binary32(q(args[13])));r.word(offset+4,to_binary32(q(args[14])));
        const auto base=sub(q(r.word(44)),q(r.word(36)));const auto shifted=multiply(integer(row),q(r.word(36)),precision);
        r.word(offset+8,to_binary32(add(base,shifted,precision)));
        for(unsigned vertex=0;vertex<6;++vertex){const auto at=offset+vertex*24;
            for(unsigned channel:{27u,24u,25u,26u})r.data[at+channel]=255;
            r.word(at+28,to_binary32(add(multiply(q(du),q(uv[vertex][0]),precision),q(u),precision)));
            r.word(at+32,to_binary32(add(multiply(q(dv),q(uv[vertex][1]),precision),q(v),precision)));
        }
    }
}
}
