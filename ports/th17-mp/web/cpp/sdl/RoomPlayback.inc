// Included beside the native RoomRuntime so playback shares its exact tick.
bool RoomRuntime::save_replay(i32 slot,const char* name,bool completed){
 if(playing_replay||!replay.frames())return true;
 if(name&&std::strlen(name)>8)return false;
 if(slot>=25||slot< -1)return false;
 if(name)saved_name=name;
 std::string path=saved_path;
 if(slot>=0){
  char filename[64];std::snprintf(filename,sizeof filename,"/save/replay/th17mp_%02d.rpy",slot+1);
  path=filename;
 }else if(path.empty()){
  for(unsigned n=1;n<=99;++n){
   char filename[64];std::snprintf(filename,sizeof filename,"/save/replay/th17mp_%02u.rpy",n);
   if(auto* existing=SDL_IOFromFile(filename,"rb"))SDL_CloseIO(existing);
   else {path=filename;break;}
  }
  if(path.empty())path="/save/replay/th17mp_01.rpy";
 }
 std::vector<u8> bytes;
 if(!replay.encode(bytes,saved_name.c_str(),confirmed_score,completed))return app->fail("TH17 multiplayer Replay encoding failed");
 const auto temporary=path+".tmp";
 auto* file=SDL_IOFromFile(temporary.c_str(),"wb");
 if(!file)return app->fail("TH17 multiplayer Replay file could not be opened");
 const bool written=SDL_WriteIO(file,bytes.data(),bytes.size())==bytes.size();
 const bool closed=SDL_CloseIO(file);
 if(!written||!closed||std::rename(temporary.c_str(),path.c_str()))return app->fail("TH17 multiplayer Replay could not be saved");
 saved_path=path;
 if(local_results&&!spectator&&!setup.local)
  Netplay::BrowserPeerTransport::ShareSpectatorReplay(bytes.data(),bytes.size(),true);
 return true;
}
bool RoomRuntime::publish(bool finish){
 if(spectator||playing_replay||setup.local||publisher_failed||!publication_started||ended)return true;
 const auto begin=emscripten_get_now();
 Netplay::SpectatorStagePublisher::PublishFailure failure{};
 const auto send=[&](const auto& bytes){
  return emscripten_get_now()-begin<2&&transport.SendSpectator(bytes.data(),bytes.size());
 };
 const bool ok=publisher.Publish(published_through,published_through,
  [&](u32 frame,auto& inputs){
   const auto* value=replay.frame(frame);if(!value)return false;inputs=*value;return true;
  },send,&failure);
 if(!ok&&!Netplay::SpectatorStagePublisher::Retryable(failure)){
  publisher_failed=true;transport.StopSpectators();return true;
 }
 if(ok&&finish&&publisher.Published()==replay.frames()&&publisher.Finish(replay.frames(),send)){
  transport.StopSpectators(true);ended=true;
 }
 return true;
}
bool RoomRuntime::receive_spectator(){
 u64 session_id=0;u32 seed=0;
 if(transport.TakeSpectatorSession(&session_id,&seed)){
  playback.Clear();spectator_view.Clear();entered=false;entry_stage=0;
  setup.room=session_id;setup.seed=u16(seed);
  spectator_deadline=now_ms+90000;
 }
 std::vector<u8> bytes;
 const auto begin=emscripten_get_now();
 for(unsigned n=0;n<128&&transport.Poll(&bytes);++n){
  if(Netplay::IsSpectatorStageEvent(bytes.data(),bytes.size())){
   Netplay::SpectatorStageEvent event;
   if(!Netplay::DecodeSpectatorStageEvent(bytes.data(),bytes.size(),&event)||event.game!='G'||
      event.playerCount!=setup.seats||event.sessionId!=setup.room||event.gameplayAbi!=setup.gameplay_abi()||
      (event.kind==Netplay::SpectatorEventKind::Checkpoint&&event.schema!=multiplayer::StageEntry::transport_schema)||
      !playback.Receive(event))return app->fail("TH17 spectator stage contract mismatch");
  }else{
   Netplay::SpectatorFramePacket frame;
   if(!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&frame)||!playback.Receive(frame))
    return app->fail("TH17 spectator input stream is invalid");
  }
  if(emscripten_get_now()-begin>=2)break;
 }
 if(playback.History().Ended())return true;
 if(!playback.Ready()&&now_ms>=spectator_deadline)return app->fail("TH17 spectator bootstrap timed out");
 return !transport.Failed()||app->fail(transport.LastError());
}
bool RoomRuntime::advance_playback(bool advance){
 if(!advance)return true;
 if(spectator){
  const auto begin=emscripten_get_now();
  const bool fast=playback.FastForwarding();
  app->audio_device.suspend(playback.Paused()||playback.Seeking());
  return playback.Step([&](const auto& checkpoint){
   if(checkpoint.schema!=multiplayer::StageEntry::transport_schema||
      !multiplayer::restore_stage_entry(*app,setup,main_input,checkpoint.frame,checkpoint.state))
    return false;
   entered=true;app->renderer.hud_viewer=1;return true;
  },[&](const auto& packet,bool seeking){
   Netplay::FrameDecision decision;decision.canAdvance=true;decision.inputs=packet.inputs;
   app->graphics.logical_drawing=seeking&&packet.frame+1<playback.Target();
   app->room_silent=seeking;
   const bool ok=step(packet.frame,decision);
   app->graphics.logical_drawing=false;app->room_silent=false;
   auto& output=state(packet.frame);
   if(!seeking)for(const auto& sound:output.sounds)
    if(!app->audio(sound.id,sound.pan,sound.action,sound.queued))return false;
   output.sounds.clear();
   app->audio_device.update();
   return ok;
  },[&]{return emscripten_get_now()-begin>=6;},fast?120:1)||app->fail("TH17 spectator stage restoration failed");
 }
 if(playback_frame>=replay.frames())return true;
 Netplay::FrameDecision decision;decision.canAdvance=true;decision.inputs=*replay.frame(playback_frame);
 if(!step(playback_frame,decision))return false;
 auto& output=state(playback_frame++);
 if(!app->room_silent)for(const auto& sound:output.sounds)
  if(!app->audio(sound.id,sound.pan,sound.action,sound.queued))return false;
 output.sounds.clear();app->audio_device.update();return true;
}
