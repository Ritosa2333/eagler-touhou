#include "playtime.hpp"
#include "../runtime_core/runtime_core.hpp"
namespace th20::source::gameplay {
void accumulate_playtime(game_session::Session& session,progress::SaveManager& saves,int replay_mode,int scene,const std::function<double()>& clock){
 if(replay_mode!=0||scene==-1||scene==3)return;
 const double previous=progress::read<double>(&session,0x2b0),elapsed=clock()-previous;
 if(elapsed>=0){
  // The original C++ unsigned conversion is supplied by the same x86 CRT;
  // ordinary clock deltas are finite and nonnegative. The unit is 1/100 s.
  const auto ticks=static_cast<std::uint64_t>(elapsed*100.0);
  auto& metadata=saves.current.metadata;progress::write(metadata.bytes,0x60,progress::read<std::uint64_t>(metadata.bytes,0x60)+ticks);progress::update_metadata_checksum(metadata,state::random_streams[1]);
  auto& profile=*progress::current_profile(saves);progress::write(profile.bytes,0x76b0,progress::read<std::uint64_t>(profile.bytes,0x76b0)+static_cast<std::uint64_t>(elapsed*100.0));
 }
 const double now=clock();progress::write(&session,0x2b0,now);
}
}
