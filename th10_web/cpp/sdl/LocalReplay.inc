
// Shared local viewer contract, implemented over TH10's original Application.
#include "../../../third_party/eagler-common/include/eagler/replay/Workspace.hpp"
namespace {
std::string local_description;u32 local_start=1,local_lengths[8]{};bool local_started=false,local_ready=false;
const char* local_filename(const char* path){const char* p=std::strstr(path,"/replay/");return p?p+8:nullptr;}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_preparing"))) int eagler_replay_preparing(){return !local_started;}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_ready"))) int eagler_replay_ready(){return local_ready||(session&&session->app&&session->app->value.screen==4&&session->app->title);}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_inspect"))) const char* eagler_replay_inspect(const u8*,u32,const char* path){
    if(!session||!session->files||!path)return nullptr;const auto* name=local_filename(path);if(!name)return nullptr;
    ReplayDocument doc(*session->files,0);if(doc.load(name)||!doc.value.info)return nullptr;
    const auto& r=doc.value;const auto& h=*r.info;eagler::replay::Description d;d.encoded_name(h.name,12);
    d.number("character",h.character);d.number("shot",h.shot_type);d.number("difficulty",h.difficulty);d.number("score",double(h.score)*10+h.score_units);d.number("timestamp",h.timestamp);d.number("lastStage",h.last_stage);
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    d.number("slowdown",h.slow_rate);
#endif
    for(u32 n=1;n<8;n++)if(const auto* st=r.readers[n].stage){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        d.stage_without_score(n,0,n==7?"EXTRA":"STAGE "+std::to_string(n));
#else
        const auto* next=n<6?r.readers[n+1].stage:nullptr;
        d.stage(n,double(next?next->score:h.score)*10+(next?next->score_units:h.score_units),st->frames,n==7?"EXTRA":"STAGE "+std::to_string(n));
#endif
    }
    local_description=d.finish();return local_description.c_str();
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_open"))) int eagler_replay_open(const char* path,u32 stage){
    if(!session||!session->app||!path||stage<1||stage>7)return 0;auto& a=*session->app;auto& s=a.state;const auto* name=local_filename(path);if(!name)return 0;
    ReplayDocument doc(a.files,0);if(doc.load(name)||!doc.value.readers[stage].stage||std::strlen(name)>=sizeof(s.replay_filename))return 0;
    for(u32 n=0;n<8;n++)local_lengths[n]=doc.value.readers[n].stage?doc.value.readers[n].stage->frames:0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(a.multiplayer_replay_scope)a.multiplayer_finish_replay();
#endif
    if(a.value.screen!=4){s.pending_screen=4;if(a.step(true)!=0)return 0;}
    s.game.stage=stage;s.game.reserved_040=stage;s.game.character=doc.value.info->character;s.game.shot_type=doc.value.info->shot_type;s.game.difficulty=doc.value.info->difficulty;
    s.current_stage=menu_data(s.chinese).stages+stage;std::strcpy(s.replay_filename,name);s.return_screen=2;s.pending_screen=12;
    local_start=stage;local_started=false;local_ready=true;return 1;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_state"))) const u32* eagler_replay_state(){
    static u32 v[5];std::fill(v,v+5,0);if(!session||!session->app)return v;const auto& a=*session->app;const auto& s=a.state;
    v[3]=s.game.stage;v[4]=local_start;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const auto& r=s.multiplayer_replay;v[0]=r.Cursor();v[1]=r.Frames();v[2]=local_started&&(r.Complete()||!a.multiplayer_replay_scope);
#else
    for(u32 n=local_start;n<8;n++){v[1]+=local_lengths[n];if(n<u32(s.game.stage))v[0]+=local_lengths[n];}
    if(s.replay&&s.replay->active_stage>=0&&s.replay->active_stage<8)v[0]+=s.replay->readers[s.replay->active_stage].frame;
    v[2]=local_started&&a.value.screen==4;
#endif
    return v;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_step"))) int eagler_replay_step(u32){
    if(!session||!session->app)return 0;auto& a=*session->app;a.audio.advance(17);const auto result=a.step(true);if(a.value.screen==7)local_started=true;a.audio.pump();return result==0;
}
}
