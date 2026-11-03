
// Shared local viewer contract, implemented over TH10's original Application.
#include <eagler/replay/Workspace.hpp>
#include <cstdio>
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/ReplayFiles.hpp"
#endif
namespace {
std::string local_description;u32 local_start=1,local_lengths[8]{},local_offset=0,local_total=0;bool local_started=false;
i32 local_result=0;
const char* local_filename(const char* path){const char* p=std::strstr(path,"/replay/");return p?p+8:nullptr;}
}
extern "C" {
i32 world_error(th10::browser::World*);
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_error"))) const char* eagler_replay_error(){
    if(!session||!session->app)return "Native replay session unavailable";
    const auto& a=*session->app;static char message[192];
    std::snprintf(message,sizeof(message),"Native replay failed (result %d | application %d | world %d | screen %d | pending %d | flags %u)",local_result,a.error,a.world?world_error(a.world):0,a.value.screen,a.state.pending_screen,a.state.game.flags);
    return message;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_preparing"))) int eagler_replay_preparing(){return !local_started;}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_ready"))) int eagler_replay_ready(){
    if(!session||!session->app)return 0;const auto& a=*session->app;
    // Attract playback has the same initialized resources as the title menu.
    // Errors are terminal too: let open report them instead of waiting forever.
    return a.error||a.stopped||(a.startup&&!a.startup->loading&&a.startup->shared&&a.startup->scores&&((a.value.screen==4&&a.title)||(a.value.screen==7&&a.world)));
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_relocate"))) int eagler_replay_relocate(const char* source,const char* target){
    if(!session||!session->app||!source||!target)return 0;
    const auto* from=local_filename(source);const auto* to=local_filename(target);auto& s=session->app->state;
    if(!from||!to||std::strcmp(s.replay_filename,from)||std::strlen(to)>=sizeof(s.replay_filename))return 0;
    std::strcpy(s.replay_filename,to);return 1;
}
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
    local_result=0;
    if(!eagler_replay_ready()||!path||stage<1||stage>7)return 0;auto& a=*session->app;if(a.error||a.stopped)return 0;auto& s=a.state;const auto* name=local_filename(path);if(!name)return 0;
    ReplayDocument doc(a.files,0);if(doc.load(name)||!doc.value.readers[stage].stage||std::strlen(name)>=sizeof(s.replay_filename))return 0;
    for(u32 n=0;n<8;n++)local_lengths[n]=doc.value.readers[n].stage?doc.value.readers[n].stage->frames:0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::vector<u8> bytes;multiplayer::ReplayArchive candidate;
    if(!multiplayer::ReadReplayFile(a.files,name,bytes)||!candidate.Load(bytes.data(),bytes.size()))return 0;
    local_offset=candidate.SeekFrame(stage);if(local_offset==Netplay::INVALID_FRAME)return 0;local_total=candidate.Frames();
    if(a.multiplayer_replay_scope)a.multiplayer_finish_replay();
    s.multiplayer_replay.Clear();
#endif
    if(a.value.screen!=4){
        // A regular tick may defer transitions behind a frame/loading gate.
        // Retire the current session at this explicit workspace boundary first.
        s.pending_screen=4;local_result=a.frames.transition(a.value);
        if(local_result!=1||a.value.screen!=4||!a.title)return 0;local_result=0;
    }
    // Workspace playback bypasses the title-menu frame that retires demo mode.
    // User files must resolve from the save store, not the built-in archive.
    s.game.flags&=~0x20u;s.inactive_frames=0;
    s.game.stage=stage;s.game.reserved_040=stage;s.game.character=doc.value.info->character;s.game.shot_type=doc.value.info->shot_type;s.game.difficulty=doc.value.info->difficulty;
    s.current_stage=menu_data(s.chinese).stages+stage;std::strcpy(s.replay_filename,name);s.return_screen=2;s.pending_screen=12;
    local_start=stage;local_started=false;return 1;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_state"))) const u32* eagler_replay_state(){
    static u32 v[6];std::fill(v,v+6,0);if(!session||!session->app)return v;const auto& a=*session->app;const auto& s=a.state;
    v[3]=s.game.stage;v[4]=local_start;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const auto& r=s.multiplayer_replay;v[0]=local_started?r.Cursor():0;v[1]=local_total;v[2]=local_started&&(r.Complete()||!a.multiplayer_replay_scope);v[5]=local_offset;
#else
    for(u32 n=local_start;n<8;n++){v[1]+=local_lengths[n];if(n<u32(s.game.stage))v[0]+=local_lengths[n];}
    if(s.replay&&s.replay->active_stage>=0&&s.replay->active_stage<8)v[0]+=s.replay->readers[s.replay->active_stage].frame;
    v[2]=local_started&&a.value.screen==4;
#endif
    return v;
}
EMSCRIPTEN_KEEPALIVE __attribute__((export_name("eagler_replay_step"))) int eagler_replay_step(u32){
    if(!session||!session->app)return 0;auto& a=*session->app;a.audio.advance(17);local_result=a.step(true);if(a.value.screen==7)local_started=true;a.audio.pump();return local_result==0;
}
}
