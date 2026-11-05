#include "../platform/BrowserRuntime.hpp"

namespace th08::multiplayer {
extern "C" __attribute__((export_name("multiplayer_replay_validate")))
u32 replay_validate(const u8* bytes,u32 size){Netplay::InputReplayInfo info;ReplayDescription d;return ReplayArchive::Inspect(bytes,size,info,d);}
extern "C" __attribute__((export_name("multiplayer_replay_seed")))
u32 replay_seed(const u8* bytes,u32 size){Netplay::InputReplayInfo info;ReplayDescription d;return ReplayArchive::Inspect(bytes,size,info,d)?d.setup.seed:Netplay::INVALID_FRAME;}
extern "C" __attribute__((export_name("multiplayer_replay_viewer")))
u32 replay_viewer(BrowserRuntime* r){return r&&r->set_replay_viewer();}
extern "C" __attribute__((export_name("multiplayer_replay_load")))
u32 replay_load(BrowserRuntime* r,const u8* bytes,u32 size,u32 stage){return r&&r->prepare_replay(bytes,size,stage);}
extern "C" __attribute__((export_name("multiplayer_replay_request")))
const char* replay_request(BrowserRuntime* r){return r?r->ReplayRequest():nullptr;}
extern "C" __attribute__((export_name("multiplayer_replay_exit")))
void replay_exit(BrowserRuntime* r){if(r)r->exit_replay();}
extern "C" __attribute__((export_name("multiplayer_replay_status")))
const u32* replay_status(BrowserRuntime* r){
    static u32 words[12]{};std::fill(words,words+12,0);words[0]=1;if(!r)return words;
    const auto& a=r->replay_archive;
    words[1]=a.Recording();words[2]=a.Playing();words[3]=r->ReplayViewer();words[4]=a.Cursor();
    words[5]=a.Info().frameCount;words[6]=a.Generation();words[7]=a.Base();words[8]=r->ReplaySeekTarget();
    words[9]=r->ReplayFinished();words[10]=r->ReplayRequestedStage();words[11]=a.Complete();return words;
}
namespace {std::vector<u8> exported;}
extern "C" __attribute__((export_name("multiplayer_replay_export")))
const u8* replay_export(BrowserRuntime* r){exported.clear();return r&&r->export_multiplayer_replay(exported)?exported.data():nullptr;}
extern "C" __attribute__((export_name("multiplayer_replay_export_size")))
u32 replay_export_size(){return u32(exported.size());}
}

#include <eagler/replay/Workspace.hpp>
namespace {std::string local_replay_description;}
extern "C" __attribute__((export_name("eagler_replay_inspect"))) const char* eagler_replay_inspect(const th08::u8* data,th08::u32 size){
    Netplay::InputReplayInfo info;th08::multiplayer::ReplayDescription m;
    if(!th08::multiplayer::ReplayArchive::Inspect(data,size,info,m))return nullptr;
    eagler::replay::Description d;d.encoded_name(m.name,8);d.number("character",m.setup.characters[0]);d.number("difficulty",m.setup.difficulty);d.number("score",double(m.score)*10);d.text("date",std::string(m.date,6));d.number("lastStage",m.last_stage);
    bool seen[9]{};const char* labels[]={"STAGE 1","STAGE 2","STAGE 3","STAGE 4A","STAGE 4B","STAGE 5","FINAL A","FINAL B","EXTRA"};
    for(unsigned i=0;i<info.chapterCount;i++){const unsigned stage=(info.chapters[i].label&15)-1;if(stage>8||seen[stage])continue;seen[stage]=true;
        d.stage(stage,double(m.stage_scores[stage])*10,0,labels[stage]);}
    local_replay_description=d.finish();return local_replay_description.c_str();
}
