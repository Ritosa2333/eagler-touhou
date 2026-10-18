#pragma once
#include "SessionState.hpp"
#include "PlayerMotion.hpp"
#include "Rng.hpp"
#include <array>
namespace th15 {
struct ReplayRunState {SessionState& progress;PlayerLifeSession& player;ItemScoreState& score;EnemyWorldState& enemies;PlayerMotion& motion;std::string& music;};
// Bytes exist only at the original file boundary. Gameplay uses named state;
// untouched padding from imported files is retained for lossless re-export.
class ReplayStageSnapshot {
 std::array<u8,0x238> data{};bool available=false;
 void capture_progress(const ReplayRunState&);
public:
 std::string error;
 bool create(const ReplayRunState&,const Rng& game,bool initial_recording=true);
 bool read(const u8*,u32);
 bool refresh(const ReplayRunState&);
 bool restore_progress(ReplayRunState&);
 bool spell_timing(u32 sequence,i32& value,bool write);
 bool restore_player(PlayerMotion&)const;
 bool restore_seed(Rng& game,Rng* visual=nullptr)const noexcept;
 const std::array<u8,0x238>& bytes()const noexcept{return data;}
};
}
