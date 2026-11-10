#include "RollbackDriver.hpp"
#include "../platform/BrowserRuntime.hpp"
#include <cstring>
namespace th08 {
extern "C" __attribute__((export_name("multiplayer_spectator_result")))
u32 multiplayer_spectator_result(BrowserRuntime* r,const u8* bytes,u32 size){
    multiplayer::ReplayArchive candidate;
    if(!r||!r->network_driver()||!candidate.Load(bytes,size)||!r->network_driver()->Network().EnterSpectatorResults())return 0;
    r->replay_archive=std::move(candidate);return r->app.enter_spectator_results();
}
extern "C" __attribute__((export_name("multiplayer_calibration_status")))
const u32* multiplayer_calibration_status(BrowserRuntime* r){static u32 empty[32]{};
    return r&&r->network_driver()?r->network_driver()->CalibrationStatus():empty;}
extern "C" __attribute__((export_name("multiplayer_local_player_visibility")))
u32 multiplayer_local_player_visibility(BrowserRuntime* r,u32 enabled){
    if(!r||enabled>1)return 0;
    r->app.game.enhance_local_player_visibility=enabled&&!r->app.session.netplay.ReadOnly();
    return !enabled||!r->app.session.netplay.ReadOnly();
}
extern "C" __attribute__((export_name("multiplayer_connect")))
u32 multiplayer_connect(BrowserRuntime* r,const char* relay){return r&&r->connect_network(relay);}
extern "C" __attribute__((export_name("multiplayer_spectator_connect")))
u32 multiplayer_spectator_connect(BrowserRuntime* r,const char* relay,const char* id){return r&&r->connect_spectator(relay,id);}
extern "C" __attribute__((export_name("multiplayer_spectator_status")))
const u32* multiplayer_spectator_status(BrowserRuntime* r){
    static u32 words[8]{};std::fill(words,words+8,0);words[0]=1;if(!r)return words;
    words[1]=r->app.session.netplay.Spectator();if(!r->network_driver())return words;
    const auto& n=r->network_driver()->Network();
    words[2]=n.SpectatorFinished();words[3]=u32(n.SpectatorBacklog());
    words[4]=n.SpectatorReceived();words[5]=n.SpectatorPublished();
    words[6]=n.SpectatorPublishFailed();words[7]=n.Enabled();return words;
}
extern "C" __attribute__((export_name("multiplayer_spectator_playback_state")))
const char* multiplayer_spectator_playback_state(BrowserRuntime* r){
    if(!r||!r->app.session.netplay.Spectator()||!r->network_driver())return nullptr;
    return r->network_driver()->Network().SpectatorState();
}
extern "C" __attribute__((export_name("multiplayer_spectator_playback_control")))
u32 multiplayer_spectator_playback_control(BrowserRuntime* r,const char* run,const char* action,u32 frame){
    if(!r||!run||!action||!r->app.session.netplay.Spectator()||!r->network_driver())return 0;
    return r->network_driver()->Network().SpectatorControl(run,action,frame);
}
extern "C" __attribute__((export_name("multiplayer_network_poll")))
u32 multiplayer_network_poll(BrowserRuntime* r){return r&&r->pump_network();}
extern "C" __attribute__((export_name("multiplayer_network_error")))
const char* multiplayer_network_error(BrowserRuntime* r){return r&&r->network_driver()?r->network_driver()->Error():"";}
extern "C" __attribute__((export_name("multiplayer_progress")))
const u32* multiplayer_progress(BrowserRuntime* r){
    // Keep the published forward frontier distinct from the transient replay
    // cursor. Hosts must not count historical recovery as new game progress.
    static u32 words[6]{};std::fill(words,words+6,0);words[5]=Netplay::INVALID_FRAME;
    if(!r)return words;const auto& net=r->app.session.netplay;
    words[0]=net.PredictionFrontier();words[1]=net.NextFrame();words[2]=net.Correcting();words[5]=net.CorrectionEnd();
    if(const auto* driver=r->network_driver()){words[3]=driver->CallbackYielded();words[4]=driver->CorrectionYields();}
    return words;
}
extern "C" __attribute__((export_name("multiplayer_reconcile")))
u32 multiplayer_reconcile(BrowserRuntime* r){return r&&r->network_driver()&&r->network_driver()->Reconcile();}
extern "C" __attribute__((export_name("multiplayer_driver_status")))
const u32* multiplayer_driver_status(BrowserRuntime* r){
    static u32 words[17]{};std::fill(words,words+17,0);words[0]=1;if(!r||!r->network_driver())return words;
    const auto& d=*r->network_driver();const auto& n=d.Network();
    words[1]=d.Initialized();words[2]=d.Failed();words[3]=d.Corrections();words[4]=d.Resimulated();words[5]=d.Predicted();
    words[6]=d.Audio().NextCommit();words[7]=d.Audio().CommittedEvents();words[8]=d.Audio().CommittedDigest();
    words[9]=d.Files().CommittedWrites();words[10]=d.MaxBytes();words[11]=n.Enabled();
    const auto mode=n.Mode();words[12]=mode&&!std::strcmp(mode,"rtc")?1:mode&&!std::strcmp(mode,"relay")?2:mode&&!std::strcmp(mode,"spectator")?3:0;
    words[13]=n.Channel().PacketsSent();words[14]=n.Channel().PacketsReceived();words[15]=u32(n.Channel().Error());
    words[16]=d.RollbackStorageAllocated();return words;
}
}
