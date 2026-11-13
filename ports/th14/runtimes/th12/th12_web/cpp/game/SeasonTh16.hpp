#pragma once
#include "AnmVm.hpp"
namespace th11 {
struct Th16SeasonState {
    i32 count=0,maximum=1140;
    i32 costs[7]={0,100,130,160,200,250,300};
    i32 thresholds[7]={0,100,230,390,590,840,1140};
    i32 level()const noexcept;
    float progress()const noexcept;
    bool collect()noexcept;
};
struct Th16SeasonGaugeServices {
    virtual ~Th16SeasonGaugeServices()=default;
    virtual bool children(AnmVm&,bool)=0;
    virtual bool interrupt(AnmVm&,i32)=0;
    virtual bool update(AnmVm&)=0;
};
bool season_th16_gauge(const Th16SeasonState&,i32& active,AnmVm& bar,AnmVm& level,Th16SeasonGaugeServices&);
i32 item_th16_cancel_reward(i32 type,i32& point_value,i32 maximum,float& energy)noexcept;
}
