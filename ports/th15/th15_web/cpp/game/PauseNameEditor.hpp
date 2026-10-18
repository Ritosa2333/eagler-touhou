#pragma once
#include "PauseState.hpp"
#include "RecordStore.hpp"
namespace th15 {
struct PauseNameServices {
 virtual ~PauseNameServices()=default;
 virtual bool sound(i32)=0;
 virtual bool write_replay(i32 slot,const std::array<char,9>&)=0;
 virtual bool prepare_results_menu()=0;
};
// Shared original 13-column name grid for score registration and replay files.
// A saved name is eight bytes, padded with spaces, followed by a terminator.
class PauseNameEditor {
 PauseState& state;SessionState& progress;PlayerLifeSession& player;RecordStore& records;PauseNameServices& host;
 bool check(bool,const char*);bool sound(i32);bool erase();bool append(char);
public:
 std::string error;
 PauseNameEditor(PauseState& s,SessionState& p,PlayerLifeSession& v,RecordStore& r,PauseNameServices& h):state(s),progress(p),player(v),records(r),host(h){}
 bool prepare();bool update(u32 pressed,u32 repeated);
};
}
