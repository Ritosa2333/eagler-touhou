#include "FreezeAccounting.hpp"
#include <algorithm>
namespace th128::freeze {
std::optional<Accounting> account(std::uint32_t pixels,int score) noexcept {
    if(pixels>172032||score<0||score>999999999)return std::nullopt;
    Accounting result;result.percent=float(double(pixels)/172032.*100.);
    const int area=int(double(result.percent)*100.);
    if(area<1000)result.bonus=area*10;
    else if(area<2000)result.bonus=area*30-20000;
    else if(area<4000)result.bonus=area*60-80000;
    else if(area<6000)result.bonus=(area-2400)*100;
    else result.bonus=area*160-600000;
    result.score=std::min(score+result.bonus/10,999999999);return result;
}
}
