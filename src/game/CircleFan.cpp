#include "CircleFan.hpp"
#include <cmath>
namespace th128::geometry {
std::optional<std::vector<ColorVertex>> circleLine(float horizontal,float vertical,float radius,float angle,int count,std::uint32_t color) noexcept {
    if(!std::isfinite(horizontal)||!std::isfinite(vertical)||!std::isfinite(radius)||!std::isfinite(angle)||count<1||count>4096||std::abs(radius)>65536||std::abs(angle)>2048)return std::nullopt;
    std::vector<ColorVertex> result;result.reserve(std::size_t(count)+1);const float step=float(6.2831854820251465/count);
    for(int index=0;index<=count;++index){result.push_back({float(double(horizontal)+float(std::cos(double(angle))*radius)),float(double(vertical)+float(std::sin(double(angle))*radius)),0,1,color});angle=float(double(angle)+step);while(angle>3.1415927410125732f)angle=float(double(angle)-6.2831854820251465);while(angle< -3.1415927410125732f)angle=float(double(angle)+6.2831854820251465);}
    return result;
}
std::optional<std::vector<ColorVertex>> circleFan(float horizontal,float vertical,float radius,float angle,int count,std::uint32_t primary,std::uint32_t edge) noexcept {
    if(!std::isfinite(horizontal)||!std::isfinite(vertical)||!std::isfinite(radius)||!std::isfinite(angle)
        ||std::abs(horizontal)>65536||std::abs(vertical)>65536||std::abs(radius)>65536||std::abs(angle)>2048||count<1||count>4096)return std::nullopt;
    std::vector<ColorVertex> result;result.reserve(std::size_t(count)+2);result.push_back({horizontal,vertical,0,1,primary});
    const float increment=float(6.283185482025146484375/double(count));
    for(int index=0;index<=count;++index){
        const float offsetX=float(std::cos(double(angle))*radius),offsetY=float(std::sin(double(angle))*radius);
        result.push_back({float(double(horizontal)+offsetX),float(double(vertical)+offsetY),0,1,edge});
        angle=float(double(angle)+increment);
        while(angle>3.1415927410125732f)angle=float(double(angle)-6.2831854820251465);
        while(angle< -3.1415927410125732f)angle=float(double(angle)+6.2831854820251465);
    }
    return result;
}
}
