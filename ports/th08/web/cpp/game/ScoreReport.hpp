#pragma once
#include "ScoreStore.hpp"
#include <string>
namespace th08 {
// Original D-key score.txt report. Timestamp is supplied in local time as
// yy/mm/dd hh:mm:ss; the output remains the original CP932 text with CRLF.
std::string score_report(GameplaySession&,ScoreStore&,const char* timestamp,u32 now);
}
