#include "RingGeometry.hpp"

namespace th128::geometry {
std::optional<std::vector<RingVertex>> ring(const anm::State& state,const anm::SpriteInfo& sprite,float x,float y,float z){
    const std::array values{x,y,z,state.sx,state.sy,state.rx,state.rotation,state.u,state.v,sprite.u0,sprite.u1};
    if((state.mode!=9&&state.mode!=14)||state.geometryCount<2||state.geometryCount>4096
        ||!std::all_of(values.begin(),values.end(),[](float value){return std::isfinite(value)&&std::abs(value)<=65536;}))return std::nullopt;
    const auto count=state.geometryCount;
    float angle=state.mode==14?anm::normalized(state.rotation):state.rotation;
    const float step=float(double(state.mode==14?state.rx:anm::Tau)/(count-1));
    const float textureStep=float(double(state.integers[1])/(count-1));
    const float outer=float(double(state.sx)*.5+state.sy),inner=float(double(state.sy)-double(state.sx)*.5);
    const auto color=(state.flags&0x30000u)?state.secondary:state.primary;
    std::vector<RingVertex> result;result.reserve(std::size_t(count)*2);float textureV=0;
    const auto polar=[&](float radius,float u){
        return RingVertex{float(double(float(std::cos(double(angle))*radius))+x),float(double(float(std::sin(double(angle))*radius))+y),z,1,color,
            float(double(u)+state.u),float(double(textureV)+state.v)};
    };
    for(int index=0;index<count;++index){
        if(state.mode==9&&index==count-1){
            auto first=result[0],second=result[1];first.v=second.v=float(double(textureV)+state.v);result.push_back(first);result.push_back(second);
        }else{result.push_back(polar(outer,sprite.u0));result.push_back(polar(inner,sprite.u1));}
        textureV=float(double(textureV)+textureStep);angle=anm::normalized(float(double(angle)+step));
    }
    return result;
}
}
