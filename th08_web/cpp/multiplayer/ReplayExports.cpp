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
