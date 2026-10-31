#pragma once
#include "PauseNameEditor.hpp"
namespace th17 {
struct PauseScoreDetails {std::array<i32,2> timestamp{};double delivered=0,requested=1;};
struct PauseScoreServices {virtual ~PauseScoreServices()=default;virtual bool capture_score_details(PauseScoreDetails&)=0;};
class PauseScoreRegistration {
    PauseState& state;GameRun& progress;PlayerLifeSession& player;ItemScoreState& score;RecordStore& records;PauseNameEditor& editor;PauseScoreServices& host;
    bool check(bool,const char*);
public:
    std::string error;
    PauseScoreRegistration(PauseState& s,GameRun& p,PlayerLifeSession& v,ItemScoreState& points,RecordStore& r,PauseNameEditor& n,PauseScoreServices& h):state(s),progress(p),player(v),score(points),records(r),editor(n),host(h){}
    bool register_result();
};
}
