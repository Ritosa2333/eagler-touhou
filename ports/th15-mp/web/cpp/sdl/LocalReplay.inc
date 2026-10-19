
#include <eagler/replay/Workspace.hpp>
namespace {std::string local_description;unsigned local_start=1,local_target=0,local_lengths[8]{};bool local_started=false;
unsigned local_word(const th15::u8* p){unsigned n;std::memcpy(&n,p,4);return n;}}
extern "C" {
EMSCRIPTEN_KEEPALIVE const char* eagler_replay_inspect(const th15::u8* data,unsigned size){
    eagler::replay::Description d;
#if TH15_MULTIPLAYER
    th15::multiplayer::ReplayArchive r;if(!r.Load(data,size))return nullptr;const auto& setup=r.Setup();d.encoded_name(r.Name().c_str(),unsigned(r.Name().size()));d.number("character",setup.selections[0]);d.number("difficulty",setup.difficulty);d.number("score",r.Score());d.number("timestamp",double(r.Timestamp()));
    bool seen[8]{};const auto& info=r.Info();for(unsigned i=0;i<info.chapterCount;i++){const auto n=info.chapters[i].label;if(n<1||n>7||seen[n])continue;seen[n]=true;d.stage_without_score(n,0,n==7?"EXTRA":"STAGE "+std::to_string(n));}
#else
    th15::Replay r;if(!r.open(data,size))return nullptr;const auto* h=r.decoded().data();d.encoded_name(reinterpret_cast<const char*>(h),8);d.number("character",r.character());d.number("difficulty",r.difficulty());d.number("score",double(local_word(h+20))*10+local_word(h+0x9c));d.number("lastStage",local_word(h+0x98));float slow;std::memcpy(&slow,h+0x84,4);d.number("slowdown",slow);d.number("timestamp",double(local_word(h+12))+double(local_word(h+16))*4294967296.);d.text("mode",r.mode_flags()&1?"Legacy":"Pointdevice");if(r.mode_flags()&2)d.text("spell","No. "+std::to_string(local_word(h+0xa0)+1));
    for(unsigned n=1;n<=7;n++)if(const auto* st=r.stage(n)){const auto* next=n<6?r.header(n+1):nullptr;d.stage(n,double(local_word(next?next+0x30:h+20))*10+local_word(next?next+0x38:h+0x9c),st->frames,n==7?"EXTRA":"STAGE "+std::to_string(n));}
#endif
    local_description=d.finish();return local_description.c_str();
}
EMSCRIPTEN_KEEPALIVE int eagler_replay_select_stage(unsigned stage){
#if TH15_MULTIPLAYER
    using namespace th15::sdl;const auto& info=mp_replay.Info();for(unsigned i=0;i<info.chapterCount;i++)if(info.chapters[i].label==stage){local_target=info.chapters[i].firstFrame;local_start=stage;return 1;}
#endif
    return 0;
}
EMSCRIPTEN_KEEPALIVE int eagler_replay_open(const char* path,unsigned stage){
#if TH15_MULTIPLAYER
    return 0;
#else
    using namespace th15::sdl;if(!app||stage<1||stage>7)return 0;size_t size=0;auto* data=static_cast<th15::u8*>(SDL_LoadFile(path,&size));if(!data)return 0;
    th15::Replay r;const bool valid=r.open(data,unsigned(size));SDL_free(data);if(!valid||!r.stage(stage))return 0;const auto* h=r.decoded().data();th15::ReplayStartRequest request;
    request.filename=path;request.stage=stage;request.character=local_word(h+0x8c);request.subcharacter=local_word(h+0x90);request.difficulty=r.difficulty();request.spell=th15::i32(local_word(h+0xa0));request.spell_practice=(r.mode_flags()&2)!=0;
    if(!app->start_replay(request))return 0;for(unsigned n=0;n<8;n++)local_lengths[n]=r.stage(n)?r.stage(n)->frames:0;local_start=stage;local_target=0;local_started=false;return 1;
#endif
}
EMSCRIPTEN_KEEPALIVE const unsigned* eagler_replay_state(){
    using namespace th15::sdl;static unsigned v[6];std::fill(v,v+6,0);if(!app)return v;v[3]=app->progress.stage;v[4]=local_start;
#if TH15_MULTIPLAYER
    v[0]=network.NextFrame();v[1]=mp_replay.FrameCount();v[2]=v[0]>=v[1]||app->exiting;v[5]=local_target;
#else
    for(unsigned n=local_start;n<8;n++){v[1]+=local_lengths[n];if(n<unsigned(app->progress.stage))v[0]+=local_lengths[n];}
    if(local_started&&app->replay&&app->replay->replay_file())v[0]+=app->replay->replay_file()->frame();v[2]=local_started&&(app->title||app->exiting);
#endif
    return v;
}
EMSCRIPTEN_KEEPALIVE int eagler_replay_step(unsigned){
    using namespace th15::sdl;if(!app)return 0;
#if TH15_MULTIPLAYER
    return mp_tick(0,false);
#else
    const bool ok=app->step(0,0,0,60,false);if(app->scene()&&app->pending_destination<0&&!app->pending_load)local_started=true;return ok;
#endif
}
}
