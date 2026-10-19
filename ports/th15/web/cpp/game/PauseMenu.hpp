#pragma once
#include "PauseScoreRegistration.hpp"
#include "AnmManager.hpp"
#include "ReplayCatalog.hpp"
namespace th15 {
struct PauseMenuServices:PauseScoreServices {
 virtual bool replay_save_available()const{return true;}
 virtual bool sound(i32)=0;
 virtual bool synchronize_result_score()=0;
 virtual bool read_replay_slot(i32,std::shared_ptr<Replay>&)=0;
 virtual bool save_named_replay(i32,const std::array<char,9>&)=0;
 virtual bool prepare_replay_save(bool completed)=0;
 virtual bool open_options(float offset)=0;
 virtual bool options_finished()const=0;
 virtual bool close_options()=0;
 virtual bool finish_pause_selection()=0;
};
// Original menu delays and history remain gameplay state. ANM trees belong to
// the same portable manager as the title and HUD; platform work is explicit.
class PauseMenu final:private PauseNameServices {
 PauseState& state;SessionState& progress;PlayerLifeSession& player;AnmManager& animations;PauseMenuServices& host;
 PauseNameEditor names;PauseScoreRegistration registration;
 bool check(bool,const char*);bool interrupt(u32,i32,bool immediate=false);bool child(i32,i32);bool freeze(bool);bool navigate(u32,u32,i32,bool visual=true);bool disable(i32);
 bool sound(i32)override;bool write_replay(i32,const std::array<char,9>&)override;bool prepare_results_menu()override;
 bool replay_slots();bool restrict_replay_save();
public:
 std::array<std::shared_ptr<Replay>,25> replays{};std::string error;
 PauseMenu(PauseState& s,SessionState& p,PlayerLifeSession& v,ItemScoreState& score,RecordStore& records,AnmManager& a,PauseMenuServices& h):state(s),progress(p),player(v),animations(a),host(h),names(s,p,v,records,*this),registration(s,p,v,score,records,names,h){}
 bool update(u32 pressed,u32 repeated);
};
}
