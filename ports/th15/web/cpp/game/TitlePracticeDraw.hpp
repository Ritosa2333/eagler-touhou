#pragma once
#include "TitleState.hpp"
#include "SessionState.hpp"
#include "RecordStore.hpp"
#include "HudDrawData.hpp"
namespace th15 {
class TitlePracticeDraw {
 TitleState& state;SessionState& progress;RecordStore& records;
public:
 std::string error;TitlePracticeDraw(TitleState& s,SessionState& p,RecordStore& r):state(s),progress(p),records(r){}
 bool draw(HudDrawServices&);
};
}
