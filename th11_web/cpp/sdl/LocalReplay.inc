
#include "../../../third_party/eagler-common/include/eagler/replay/Workspace.hpp"
#ifdef TH11_MULTIPLAYER
namespace {std::string local_description;unsigned local_start=1;}
extern "C" {
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_inspect"))) const char* eagler_replay_inspect(const th11::u8* data,unsigned size){
    th11::multiplayer::ReplayArchive r;if(!r.Load(data,size))return nullptr;const auto& setup=r.Setup();eagler::replay::Description d;
    d.encoded_name(r.Name().c_str(),unsigned(r.Name().size()));d.number("character",setup.selections[0]/3);d.number("shot",setup.selections[0]%3);d.number("difficulty",setup.difficulty);d.number("score",r.Score());d.number("timestamp",double(r.Timestamp()));
    bool seen[8]{};const auto& info=r.Info();for(unsigned i=0;i<info.chapterCount;i++){const auto stage=info.chapters[i].label;if(stage<1||stage>7||seen[stage])continue;seen[stage]=true;
        d.stage_without_score(stage,0,stage==7?"EXTRA":"STAGE "+std::to_string(stage));}
    local_description=d.finish();return local_description.c_str();
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_open"))) int eagler_replay_open(const char* path,unsigned stage){
    auto& a=th11::sdl::app;std::vector<th11::u8> bytes;if(!a.read_file(path,bytes,16*1024*1024))return 0;
    if(!a.multiplayer_load_replay(bytes.data(),unsigned(bytes.size()),stage))return 0;local_start=stage;return 1;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_state"))) const unsigned* eagler_replay_state(){
    static unsigned v[5];const auto& a=th11::sdl::app;v[0]=a.netplay.NextFrame();v[1]=a.multiplayer_replay.FrameCount();v[2]=!a.multiplayer_world||v[0]>=v[1];v[3]=a.session.state.stage;v[4]=local_start;return v;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_step"))) int eagler_replay_step(unsigned){return th11::sdl::app.multiplayer_tick();}
}
#endif
