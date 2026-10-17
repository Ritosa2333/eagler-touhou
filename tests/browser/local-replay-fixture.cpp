// Software-generated input corpus, encoded by each title's real native codec.
// This is a partial Stage 1 recording, not a live-room or retail golden trace.
#include "ReplayArchive.hpp"
#include <array>
#include <cstdlib>
#include <fstream>
#if defined(TH11_MULTIPLAYER)
namespace replay_title=th11::multiplayer;
#else
namespace replay_title=th15::multiplayer;
#endif
int main(int argc,char** argv){
    if(argc!=7)return 2;
    replay_title::SessionSetup setup;setup.session_id=123;setup.seed=123;setup.automatic=false;
    setup.player_count=std::strtoul(argv[6],nullptr,10);setup.local_player=setup.player_count-1;
    for(unsigned i=0;i<4;i++)setup.build[i]=std::strtoul(argv[2+i],nullptr,16);
    for(unsigned i=0;i<setup.player_count;i++)setup.selections[i]=i;
    replay_title::ReplayArchive archive;if(!archive.Begin(setup,1792144800))return 3;
    for(unsigned frame=0;frame<500;frame++){
        std::array<Netplay::FrameInput,3> inputs{};
        if(frame>=180&&frame<300)for(unsigned seat=0;seat<setup.player_count;seat++)inputs[seat].buttons=seat%2?64:128;
        if(!archive.Append(frame,1,inputs.data(),setup.player_count))return 4;
    }
    std::vector<std::uint8_t> bytes;if(!archive.Encode(bytes,"LOCAL",0,false))return 5;
    std::ofstream file(argv[1],std::ios::binary);file.write(reinterpret_cast<const char*>(bytes.data()),bytes.size());return file?0:6;
}
