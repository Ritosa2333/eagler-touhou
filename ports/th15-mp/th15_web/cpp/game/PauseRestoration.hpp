#pragma once
#include "PauseState.hpp"
#include "SessionState.hpp"
namespace th15 {
struct PauseRestorationServices {
 virtual ~PauseRestorationServices()=default;
 virtual bool restart_elapsed_play_time()=0;virtual bool interrupt_pause_visual(u32,i32)=0;
 virtual bool resume_dialogue()=0;virtual bool resume_chapter_result()=0;
};
// The close presentation step precedes the selected retry/continue/title action.
// Only normal pause restores chapter/dialogue freezes and saved frame skipping.
class PauseRestoration {
 PauseState& state;SessionState& progress;i32& frame_skip;PauseRestorationServices& host;bool check(bool,const char*);
public:
 std::string error;
 PauseRestoration(PauseState& s,SessionState& p,i32& skip,PauseRestorationServices& h):state(s),progress(p),frame_skip(skip),host(h){}
 bool restore();
};
}
