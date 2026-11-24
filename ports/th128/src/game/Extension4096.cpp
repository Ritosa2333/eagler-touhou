#include "Extension4096.hpp"
#include <bit>
#include <cmath>

namespace th128::bullet {
std::optional<Extension4096> initializeExtension4096(const ExtensionRaw& record) noexcept {
    if(record.type!=4096||(record.control!=0&&record.control!=1))return std::nullopt;
    return Extension4096{0,record.parameterA,std::bit_cast<std::uint32_t>(record.parameterB)};
}
std::optional<Extension4096Step> stepExtension4096(const Extension4096& state,Position position,float width,float height) noexcept {
    for(const float value:{position.x,position.y,position.z,width,height})
        if(!std::isfinite(value)||std::abs(value)>16777216.f)return std::nullopt;
    if(width<0||height<0)return std::nullopt;
    Extension4096Step result{state,position,false,false};
    const bool outside=double(position.x)+double(width)*.5<=-192.
        ||double(position.x)-double(width)*.5>=192.
        ||double(position.y)+double(height)*.5<=0.
        ||double(position.y)-double(height)*.5>=448.;
    if(!outside)return result;
    if((state.flags&1u)&&position.y<0){
        result.position.y=float(double(position.y)+(double(height)+448.));
    }else if((state.flags&2u)&&position.y>=448.f){
        result.position.y=float(double(position.y)-(double(height)+448.));
    }else if((state.flags&4u)&&position.x<-192.f){
        result.position.x=float(double(position.x)+(double(width)+384.));
    }else if((state.flags&8u)&&position.x>=192.f){
        result.position.x=float(double(position.x)-(double(width)+384.));
    }else return result;
    result.wrapped=true;
    result.next.counter=std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(state.counter)+1u);
    result.finished=result.next.counter>=state.limit;
    return result;
}
}
