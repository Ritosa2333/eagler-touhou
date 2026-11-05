#pragma once
#include "PauseMenu.hpp"
#include "TitleReplayMenu.hpp"
namespace th15 {
// Pause text uses the small original ASCII glyph set. The captured playfield
// is a renderer-owned sprite; its tint follows child script 57 of the overlay.
class PauseDraw {
 PauseState& state;SessionState& progress;PlayerLifeSession& player;RecordStore& records;AnmManager& animations;const std::array<std::shared_ptr<Replay>,25>& replays;
 HudTextDraw text;bool check(bool,const char*);bool emit(HudDrawServices&);bool name_grid(Vec3,HudDrawServices&);
 bool caption(i32,const u8*,ReplayCalendarServices&);bool slots(HudDrawServices&,ReplayCalendarServices&);bool replay_name(std::array<u8,0xa4>&,HudDrawServices&,ReplayCalendarServices&);bool ranking(HudDrawServices&,ReplayCalendarServices&);
public:
 std::string error;
 PauseDraw(PauseState& s,SessionState& p,PlayerLifeSession& v,RecordStore& r,AnmManager& a,const std::array<std::shared_ptr<Replay>,25>& files):state(s),progress(p),player(v),records(r),animations(a),replays(files){}
 bool draw(HudDrawServices&,ReplayCalendarServices&,std::array<u8,0xa4>* current_replay=nullptr,AnmVm* captured_sprite=nullptr);
};
}
