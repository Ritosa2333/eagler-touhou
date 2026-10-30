#pragma once
#include "Types.hpp"
namespace th17 {
struct ReplayCalendar {i32 year=0,month=1,day=1,hour=0,minute=0;};
struct ReplayCalendarServices {virtual ~ReplayCalendarServices()=default;virtual bool calendar(i64 timestamp,ReplayCalendar&)=0;};
}
