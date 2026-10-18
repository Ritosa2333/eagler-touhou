#include "../../th11_web/cpp/multiplayer/ReplayArchive.hpp"
#include "../../th11_web/cpp/multiplayer/ReplayStorage.hpp"
#include <cstdio>
#include <cstdlib>
#include <unordered_set>
using namespace th11::multiplayer;
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"Replay boundary check failed at %d: %s\n",__LINE__,#x);std::exit(1);}}while(false)
extern "C" {
__attribute__((export_name("capacity_allocate"))) void* capacity_allocate(unsigned bytes){return std::malloc(bytes);}
__attribute__((export_name("capacity_free"))) void capacity_free(void* p){std::free(p);}
__attribute__((export_name("capacity_validate"))) int capacity_validate(const unsigned char* bytes,unsigned size){return ReplayArchive::Validate(bytes,size);}
}
int main(int argc,char** argv){
    static_assert(ReplayArchive::MaxFrames==250000,"Browser metadata capacity must match pinned native storage");
    std::unordered_set<std::string> existing;
    auto exists=[&](const char* path){return existing.count(path)!=0;};
    CHECK(AutomaticReplayPath(exists)=="/save/replay/th11_01.rpy");
    for(unsigned slot=1;slot<=99;++slot){char path[80];std::snprintf(path,sizeof(path),"/save/replay/th11_%02u.rpy",slot);existing.insert(path);}
    CHECK(AutomaticReplayPath(exists)=="/save/replay/th11_ud0000.rpy");
    existing.insert("/save/replay/th11_ud0000.rpy");const auto before=existing;
    const auto selected=AutomaticReplayPath(exists);CHECK(selected=="/save/replay/th11_ud0001.rpy"&&existing==before);
    existing.insert(selected);CHECK(AutomaticReplayPath(exists)=="/save/replay/th11_ud0002.rpy");
    CHECK(AutomaticReplayPath([](const char* path){const std::string name=path;return name.find("_ud")==std::string::npos||name<="/save/replay/th11_ud0009.rpy";})=="/save/replay/th11_ud000a.rpy");
    CHECK(AutomaticReplayPath([](const char* path){const std::string name=path;return name.find("_ud")==std::string::npos||name<="/save/replay/th11_ud00zz.rpy";})=="/save/replay/th11_ud0100.rpy");
    CHECK(AutomaticReplayPath([](const char*){return true;}).empty());
    std::puts("PASS Replay paths: occupied 01-99 plus ud0000 -> ud0001, no replacement, base36 carry, exhausted namespace");

    SessionSetup setup;setup.session_id=0x1122334455667788ull;setup.player_count=3;setup.local_player=2;setup.seed=417;setup.difficulty=1;
    setup.requested_delay=setup.input_delay=2;setup.automatic=false;setup.selections={0,3,5};setup.build={0x12345678,0x90abcdef,0x87654321,0xfedcba09};
    ReplayArchive recording;CHECK(recording.Begin(setup,1723456789));CHECK(!recording.AtCapacity());
    std::array<Netplay::FrameInput,3> row{};
    for(unsigned frame=0;frame<ReplayArchive::MaxFrames;++frame){
        CHECK(!recording.AtCapacity());row[0].buttons=frame%2;row[1].buttons=frame%8?0:8;row[2].buttons=frame+1==ReplayArchive::MaxFrames?0x8001:1;
        CHECK(recording.Append(frame,frame<125000?1:2,row.data(),3));
    }
    CHECK(recording.AtCapacity()&&recording.FrameCount()==ReplayArchive::MaxFrames);
    CHECK(*recording.FrameAt(ReplayArchive::MaxFrames-1)==row);
    CHECK(!recording.Append(ReplayArchive::MaxFrames,2,row.data(),3));
    CHECK(recording.FrameCount()==ReplayArchive::MaxFrames&&!recording.FrameAt(ReplayArchive::MaxFrames));
    // Capacity is always partial even when a caller supplies a generic terminal
    // flag. The final legitimate confirmed input stays present in the archive.
    std::vector<unsigned char> bytes;CHECK(recording.Encode(bytes,"CAPACITY",1234567,true));
    ReplayArchive loaded;CHECK(loaded.Load(bytes.data(),bytes.size()));
    CHECK(loaded.AtCapacity()&&!loaded.Completed()&&loaded.Info().config.recordedPlayer==2);
    CHECK(loaded.FrameCount()==ReplayArchive::MaxFrames&&*loaded.FrameAt(ReplayArchive::MaxFrames-1)==row);
    CHECK(loaded.Info().chapterCount==2&&loaded.Info().chapters[1].firstFrame==125000);
    if(argc>1){auto* file=std::fopen(argv[1],"wb");CHECK(file);CHECK(std::fwrite(bytes.data(),1,bytes.size(),file)==bytes.size());CHECK(std::fclose(file)==0);}
    std::puts("PASS actual ReplayArchive: 250000 valid inputs, last input retained, extra append rejected, encoded terminal forced partial");
    std::puts("Scope: archive and filename boundary tests only; no 69-minute native gameplay run");
}
