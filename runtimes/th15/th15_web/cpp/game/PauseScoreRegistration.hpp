#pragma once
#include "PauseNameEditor.hpp"
namespace th15 {
struct PauseScoreDetails {std::array<i32,2> timestamp{};double delivered=0,requested=1;};
struct PauseScoreServices {virtual ~PauseScoreServices()=default;virtual bool capture_score_details(PauseScoreDetails&)=0;};
// Registration qualifies a score before asking the platform for its timestamp.
// Extra's completed-stage marker belongs to the score row, not the live scene.
class PauseScoreRegistration {
 PauseState& state;SessionState& progress;PlayerLifeSession& player;ItemScoreState& score;RecordStore& records;PauseNameEditor& editor;PauseScoreServices& host;
 bool check(bool,const char*);
public:
 std::string error;
 PauseScoreRegistration(PauseState& s,SessionState& p,PlayerLifeSession& v,ItemScoreState& points,RecordStore& r,PauseNameEditor& n,PauseScoreServices& h):state(s),progress(p),player(v),score(points),records(r),editor(n),host(h){}
 bool register_result();
};
}
