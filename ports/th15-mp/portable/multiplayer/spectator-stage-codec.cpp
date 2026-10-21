#include "../../web/cpp/multiplayer/SpectatorStageState.hpp"
#include <cassert>
#include <cstdio>
using namespace th15;using namespace th15::multiplayer;
int main(){
 for(u32 count:{2u,3u})for(i32 stage:{1,2,7}){
  SpectatorStageState state;state.frame=900;state.stage=stage;state.seats=count;
  for(u32 i=0;i<count;++i){
   SessionState progress;progress.stage=stage;progress.difficulty=stage==7?4:1;progress.new_run=false;
   PlayerLifeSession player;player.power=100+i*100;player.extra_lives=2;player.bombs=3;
   ItemScoreState score;score.point_value=1000000;EnemyWorldState enemies;PlayerMotion motion;
   motion.position_fixed={i32(i)*4096,51200};std::string music="stage.wav";
   ReplayRunState native{progress,player,score,enemies,motion,music};ReplayStageSnapshot header;
   Rng seed{417,0,0};assert(header.create(native,seed)&&header.refresh(native));state.headers[i]=header.bytes();
  }
  std::vector<u8> bytes;assert(EncodeSpectatorStageState(state,bytes));
  assert(bytes.size()==16+count*0x238&&bytes.size()<=Netplay::SpectatorStageMaxStateBytes);
  SpectatorStageState output;assert(DecodeSpectatorStageState(bytes,output));assert(output.headers==state.headers);
  for(std::size_t n=0;n<bytes.size();++n){auto shortData=std::vector<u8>(bytes.begin(),bytes.begin()+n);
   output.frame=123;assert(!DecodeSpectatorStageState(shortData,output)&&output.frame==123);
  }
  auto bad=bytes;bad[0]=1;assert(!DecodeSpectatorStageState(bad,output));
  bad=bytes;bad[16]=stage==7?1:7;assert(!DecodeSpectatorStageState(bad,output));
  bad=bytes;bad.push_back(0);assert(!DecodeSpectatorStageState(bad,output));
  state.headers[0][0]=0;assert(!EncodeSpectatorStageState(state,bytes));
 }
 std::puts("PASS streaming Replay: native stage headers, 2/3 seats, transactional bounds");
}
