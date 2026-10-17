#include "../../th15_web/cpp/multiplayer/SpectatorStageState.hpp"
#include <cassert>
#include <cstdio>
using namespace th15;
using namespace th15::multiplayer;
int main(){
 for(unsigned count:{2u,3u})for(unsigned difficulty:{1u,4u})for(unsigned delay:{0u,3u,9u}){
  SpectatorStageState state;state.setup.session_id=0x123456789abcdef0ull;state.setup.player_count=count;
  state.setup.seed=417;state.setup.build={1,2,3,4};state.setup.input_delay=delay;
  state.setup.difficulty=difficulty;state.setup.selections={0,1,count==3?3u:0u};
  state.progress.stage=difficulty==4?7:2;state.progress.starting_stage=difficulty==4?7:1;
  state.progress.difficulty=difficulty;state.progress.character=0;state.progress.new_run=false;
  state.frame=900;state.application_frames=1000;state.enemy.difficulty=difficulty;
  state.game_random={421,0,100};state.visual_random={811,0,200};state.fragments=2;
  for(unsigned seat=0;seat<count;++seat){auto& p=state.pilots[seat];
   p.resources.power=100+int(seat)*100;p.resources.power_step=100;p.resources.bombs=2;
   p.motion.position_fixed={int(seat)*4096,51200};p.motion.position={float(seat)*32.f,400,0};
   p.motion.options[0].position={int(seat)*128,1024};p.keys.held=1;p.keys.duration[0]=120+seat;
   p.life_state=1;p.ghost_random=seat+80;p.life_piece_tier=seat;
  }
  std::vector<u8> bytes;assert(EncodeSpectatorStageState(state,bytes));assert(bytes.size()<8192);
  SpectatorStageState decoded;assert(DecodeSpectatorStageState(bytes,decoded));
  assert(decoded.setup.input_delay==delay&&decoded.setup.session_id==state.setup.session_id);
  assert(decoded.frame==900&&decoded.fragments==2&&decoded.progress.stage==state.progress.stage);
  for(unsigned seat=0;seat<count;++seat){const auto& p=decoded.pilots[seat];
   assert(p.resources.power==state.pilots[seat].resources.power&&p.keys.duration[0]==120+seat);
   assert(p.motion.position_fixed.x==int(seat)*4096&&p.motion.options[0].position.x==int(seat)*128);
   assert(!p.motion.options[0].movement_callback);
  }
  if(count==2&&difficulty==1&&delay==0){
   for(std::size_t size=0;size<bytes.size();++size){
    std::vector<u8> truncated(bytes.begin(),bytes.begin()+size);decoded.frame=123;
    assert(!DecodeSpectatorStageState(truncated,decoded)&&decoded.frame==123);
   }
   auto wrong=bytes;wrong[0]=2;assert(!DecodeSpectatorStageState(wrong,decoded));
  }
  state.pilots[1].resources.power=401;assert(!EncodeSpectatorStageState(state,bytes));
 }
 std::puts("PASS TH15 named stage scalar codec: 2/3 seats, Normal/Extra, measured delay, input edges, transactional decode");
}
