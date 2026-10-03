#include "../platform/BrowserRuntime.hpp"
#include "RollbackDriver.hpp"
#include "../platform/PlatformDevices.hpp"
#include <cstdio>

namespace th08 {
extern "C" u32 browser_multiplayer_configure(BrowserRuntime*,const u32*,u32);
bool BrowserRuntime::set_replay_viewer(){
    if(prepared||app.session.netplay.Configured())return false;
    replay_viewer=true;return true;
}
bool BrowserRuntime::prepare_replay(const u8* bytes,u32 size,u32 stage){
    if(prepared||app.session.netplay.Configured()||stage>8)return false;
    multiplayer::ReplayArchive candidate;if(!candidate.Load(bytes,size))return false;
    const auto target=candidate.StageFrame(stage);if(target==Netplay::INVALID_FRAME)return false;
    auto setup=candidate.Description().setup;
    const u32 words[]{setup.version,setup.player_count,setup.local_player,setup.difficulty,setup.seed,
        u32(setup.session_id),u32(setup.session_id>>32),setup.build[0],setup.build[1],setup.build[2],setup.build[3],
        setup.characters[0],0,setup.characters[1],0,setup.characters[2],0,
        setup.input_delay_auto?0:setup.input_delay,setup.prediction_limit,setup.adonis_mode,
        u32(setup.input_delay_auto),setup.prediction_reserve};
    if(!browser_multiplayer_configure(this,words,setup.version>=6?22:setup.version==5?20:setup.version==4?19:17))return false;
    if(setup.version>=6){
        if(!app.session.netplay.Reset(setup))return false;
        app.session.multiplayer_session=setup;
    }
    if(!app.session.netplay.BeginPlayback())return false;
    replay_archive=std::move(candidate);replay_viewer=true;replay_finished=false;replay_seek_target=target;
    replay_shadow_files["score.dat"]=replay_archive.BootScore();
    const auto& config=replay_archive.Description().configuration;
    const auto* data=reinterpret_cast<const u8*>(&config);
    replay_shadow_files["th08.cfg"]={data,data+sizeof(config)};
    app.library.force_16bit=(config.options&4)!=0;
    return true;
}
bool BrowserRuntime::begin_replay_recording(){
    if(replay_archive.Playing()||app.session.netplay.ReadOnly())return true;
    if(replay_archive.Recording())return true;
    auto setup=app.session.netplay.Setup();setup.started=false;
    multiplayer::ReplayDescription description;
    description.setup=setup;description.configuration=replay_boot_configuration;
    std::memcpy(description.name,"Replay",6);char timestamp[20]{};calendar(description.date,timestamp);
    return replay_archive.Begin(description,replay_boot_score);
}
bool BrowserRuntime::request_multiplayer_replay(const char* name,u32 stage){
    if(!replay_viewer||app.session.netplay.Configured()||!name||stage>8)return false;
    const auto p=path(name);
    if(p.rfind("replay/th8_",0)||p.size()>32||p.size()<17||p.substr(p.size()-5)!=".rpyx")return false;
    for(char c:p.substr(11,p.size()-16))if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')))return false;
    const auto bytes=read(p.c_str());Netplay::InputReplayInfo info;multiplayer::ReplayDescription d;
    if(!multiplayer::ReplayArchive::Inspect(bytes.data(),bytes.size(),info,d))return false;
    bool present=false;for(u32 i=0;i<info.chapterCount;++i)present|=(info.chapters[i].label&15)==stage+1;
    if(!present)return false;
    replay_request=p;replay_requested_stage=stage;return true;
}
bool BrowserRuntime::save_multiplayer_replay(i32 slot,const char* name){
    if(app.session.netplay.ReadOnly())return true;
    if(!replay_archive.Recording()||!multiplayer_driver||!multiplayer_driver->FrameOpen())return false;
    char date[6]{},stamp[20]{};calendar(date,stamp);
    return replay_archive.RequestSave(app.session.netplay.NextFrame(),slot,name,date);
}
bool BrowserRuntime::save_confirmed_multiplayer_replay(i32 slot,const char* name){
    if(replay_viewer||app.session.netplay.ReadOnly()||slot<1||slot>15||!name||!multiplayer_driver||
       multiplayer_driver->FrameOpen()||!multiplayer_driver->Reconcile()||!replay_archive.Recording())return false;
    auto d=replay_archive.Description();std::memset(d.name,0,8);
    for(u32 i=0;i<8&&name[i];++i)d.name[i]=name[i];char stamp[20]{};calendar(d.date,stamp);
    std::vector<u8> bytes;if(!replay_archive.Encode(bytes,&d))return false;
    char filename[48];std::snprintf(filename,sizeof(filename),"replay/th8_%02d.rpyx",slot);
    return resources_.put(filename,bytes.data(),u32(bytes.size()))&&file_device().save(filename,bytes.data(),u32(bytes.size()));
}
}
