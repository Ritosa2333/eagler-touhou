#pragma once
#include "GameConfig.hpp"
#include "ReplayRecording.hpp"
#include "ReplayPlayback.hpp"
#include "ReplayStageSnapshot.hpp"
#include <memory>
namespace th15 {
// One recording/playback owner survives stage factories. Snapshots are taken
// before startup and refreshed after the original stage reset sequence.
class SessionReplay {
 GameConfig& settings;Rng& game;Rng& visual;Replay file;ReplayRecording recording;
 std::unique_ptr<ReplayPlayback> playback;std::array<ReplayStageSnapshot,8> snapshots;
 i32 selected=0;bool initialized=false;
 std::array<ReplayStageSnapshot,3> seat_entries{};u32 stream_stage=0,activation_count_=0;bool fail(const std::string&);
public:
 std::string error;
 SessionReplay(GameConfig& s,Rng& game,Rng& visual):settings(s),game(game),visual(visual){}
 bool initialize_live(const ReplayRunState&);
 bool open_stream(const std::array<ReplayStageSnapshot,3>&,ReplayRunState&);
 u32 activation_count()const noexcept{return activation_count_;}
 bool stream_entry(u32 stage)const noexcept{return stream_stage==stage;}
 void finish_stream()noexcept{stream_stage=0;}
 void seat_entry(u32 seat,const ReplayStageSnapshot& entry){if(seat<3)seat_entries[seat]=entry;}
 const ReplayStageSnapshot& seat_entry(u32 seat)const{return seat_entries[seat];}
 bool open(const u8*,u32,i32 selected_stage,ReplayRunState&);
 bool prepare_stage(ReplayRunState&);bool activate_stage(ReplayRunState&);
 bool spell_timing(i32 sequence,i32& encoded);
 ReplayRecording* live()noexcept{return initialized&&!playback?&recording:nullptr;}
 ReplayPlayback* replay()noexcept{return playback.get();}
 const Replay* replay_file()const noexcept{return playback?&file:nullptr;}
 const ReplayStageSnapshot* snapshot(u32 stage)const noexcept{return initialized&&stage<8?&snapshots[stage]:nullptr;}
};
}
