#pragma once
#include "RecordStore.hpp"
namespace th15 {
struct RunClock {virtual ~RunClock()=default;virtual bool seconds(double&)=0;};
class RunPlayTime {
 RecordStore& records;SessionState& progress;PlayerLifeSession& player;RunClock& clock;bool fail(const char*);
public:
 double origin=0;std::string error;
 RunPlayTime(RecordStore& r,SessionState& p,PlayerLifeSession& v,RunClock& c):records(r),progress(p),player(v),clock(c){}
 bool rebase();bool account(i32 scene_destination);
};
}
