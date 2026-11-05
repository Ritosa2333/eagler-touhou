
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include <eagler/replay/Workspace.hpp>
namespace th08 {
namespace {std::string local_description;unsigned local_start=0,local_lengths[9]{};bool local_started=false;
const char* local_labels[]={"STAGE 1","STAGE 2","STAGE 3","STAGE 4A","STAGE 4B","STAGE 5","FINAL A","FINAL B","EXTRA"};
}
extern "C" {
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_preparing"))) int eagler_replay_preparing(){return !local_started;}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_inspect"))) const char* eagler_replay_inspect(const u8* data,u32 size){
    ReplayPlayback r;if(!r.load(data,size))return nullptr;const auto& m=r.metadata();eagler::replay::Description d;
    d.encoded_name(m.player_name,8);d.text("date",std::string(m.date,strnlen(m.date,6)));d.number("character",m.shot_type);d.number("difficulty",m.difficulty);d.number("score",double(m.spell_score)*10);d.number("slowdown",m.lag);
    if(m.spell_number>=0){d.text("spell","Spell No."+std::to_string(m.spell_number+1));
        std::string bytes="[";for(unsigned i=0;i<48&&m.spell_name[i];i++){if(i)bytes+=',';bytes+=std::to_string(static_cast<unsigned char>(m.spell_name[i]));}d.field("spellNameBytes",bytes+"]");}
    for(u32 n=0;n<9;n++)if(r.has_stage(n)){ReplayStage st;r.snapshot(n,st);d.stage(n,double(m.spell_number>=0?m.spell_score:st.end_score)*10,0,m.spell_number>=0?"No."+std::to_string(m.spell_number+1):local_labels[n]);}
    local_description=d.finish();return local_description.c_str();
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_open"))) int eagler_replay_open(const char* path,u32 stage){
    if(!runtime||stage>8)return 0;const auto* relative=std::strstr(path,"/replay/");if(!relative)return 0;
    size_t size=0;auto* bytes=static_cast<u8*>(SDL_LoadFile(path,&size));if(!bytes)return 0;ReplayPlayback candidate;const bool valid=candidate.load(bytes,u32(size))&&candidate.has_stage(stage);if(valid)runtime->put(relative+1,bytes,u32(size));SDL_free(bytes);if(!valid)return 0;
    if(!runtime->app.begin_local_replay(relative+1,i32(stage)))return 0;local_start=stage;local_started=false;std::fill(local_lengths,local_lengths+9,0);return 1;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_state"))) const u32* eagler_replay_state(){
    static u32 v[5];std::fill(v,v+5,0);if(!runtime)return v;const auto& a=runtime->app;const auto stage=u32(a.game.globals.stage);v[3]=stage;v[4]=local_start;
    // Ordinary TH08 stores bounded input blocks, not a total tick count. The
    // native consumed cursor remains authoritative; no guessed seek endpoint.
    v[0]=local_started?u32(std::max(0,a.game.playback.stream.frame)):0;v[2]=local_started&&!a.in_game();return v;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_step"))) int eagler_replay_step(u32){
    if(!runtime)return 0;const bool ok=runtime->step(true);if(runtime->app.in_game())local_started=true;return ok;
}
}
}
#endif
