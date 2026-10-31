#pragma once
#include "RecordStore.hpp"
namespace th17 {
struct RunClock {virtual ~RunClock()=default;virtual bool seconds(double&)=0;};
class RunPlayTime {
 RecordStore& records;GameRun& progress;RunClock& clock;bool fail(const char*);
public:
 double origin=0;std::string error;
 RunPlayTime(RecordStore& r,GameRun& p,RunClock& c):records(r),progress(p),clock(c){}
 bool rebase();bool account(i32 scene_destination);
};
}
