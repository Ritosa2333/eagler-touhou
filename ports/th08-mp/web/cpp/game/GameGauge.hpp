#pragma once
#include "GameValues.hpp"
namespace th08 {
struct GaugeThresholds {
    i16 minimum=-10000,maximum=10000,human_bonus=-8000,youkai_bonus=8000,human=-2000,youkai=2000;
    void configure(u8 character)noexcept;
};
class GameGauge {
public:
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    GameGauge(GameGlobals& values,GaugeThresholds& thresholds):values{values.gauge,values.gauge_copy},thresholds(thresholds){}
    GameGauge(i16& gauge,i16& previous,GaugeThresholds& thresholds):values{gauge,previous},thresholds(thresholds){}
#else
    GameGauge(GameGlobals& values,GaugeThresholds& thresholds):values(values),thresholds(thresholds){}
#endif
    i16 value()const noexcept{return values.gauge;}
    bool human_bonus()const noexcept{return value()<=thresholds.human_bonus;}
    bool human()const noexcept{return value()<=thresholds.human;}
    bool youkai_bonus()const noexcept{return value()>=thresholds.youkai_bonus;}
    bool youkai()const noexcept{return value()>=thresholds.youkai;}
    void set(i16 value)noexcept{values.gauge=value;}
    void add(i16 amount,bool bomb,bool force)noexcept;
private:
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    struct GaugeValues {i16& gauge; i16& gauge_copy;} values;
#else
    GameGlobals& values;
#endif
    GaugeThresholds& thresholds;
};
// Original rank changes retain the fractional counter at a clamped limit.
struct GameRank {
    i32 value=0,maximum=0,minimum=0,fraction=0;
    void add(i32 amount)noexcept;
    void subtract(i32 amount)noexcept;
};
}
