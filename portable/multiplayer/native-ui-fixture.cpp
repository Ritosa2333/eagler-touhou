#include "../../th11_web/cpp/sdl/Application.cpp"
// Diagnostic-only TU. Linked instead of production Application.o; never packaged.
static bool fixture_freeze_ghost=false;
static bool fixture_record_io=false;
extern "C" {
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_begin(int stage,int local){
 auto& a=th11::sdl::app;fixture_freeze_ghost=false;fixture_record_io=false;
 if(!a.initialize())return 0;
#ifdef TH11_MULTIPLAYER
 a.netplay.Clear();a.multiplayer_replay.Clear();a.multiplayer_viewer=false;a.multiplayer_auto_path.clear();a.multiplayer_saved=false;
 auto& core=a.session.resources.core;a.renderer.invalidate();
 for(th11::AnmResource* f:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1]})a.graphics.unload(*f);
 if(a.session.resources.stage)for(th11::AnmResource* f:{&a.session.resources.stage->background,&a.session.resources.stage->logo,&a.session.resources.stage->enemies})a.graphics.unload(*f);
 th11::MultiplayerOptions o;o.seat_count=3;o.local_seat=local;o.stage=stage;o.difficulty=stage==7?4:1;o.selections={0,3,5};o.seed=12345;
 if(!a.session.begin_multiplayer(a.resources,o)){a.error=a.session.error;return 0;}
 for(th11::AnmResource* f:{&core.text,&core.ascii,&core.bullet,&core.enemy,&core.front,&core.title,&core.title_variant,&core.players[0],&core.players[1],&a.session.resources.stage->background,&a.session.resources.stage->logo,&a.session.resources.stage->enemies})if(!a.graphics.preload(*f)){a.error=a.graphics.error;return 0;}
 a.multiplayer_world=true;
 return a.audio_events()&&a.text_events();
#else
 return a.restart(stage,0,0,stage==7?4:1);
#endif
}
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_step(int count,unsigned p1,unsigned p2,unsigned p3,unsigned pauses){
 auto& a=th11::sdl::app;
 for(int n=0;n<count;++n){
#ifdef TH11_MULTIPLAYER
  if(fixture_freeze_ghost&&a.session.battle)for(auto& p:a.session.battle->pilots)if(p&&p->ghost){p->ghost_dx=p->ghost_dy=0;p->ghost_clock=1;}
  std::array<th11::MultiplayerInput,3> in{};const unsigned held[]={p1,p2,p3};for(unsigned i=0;i<3;++i){in[i].held=held[i];in[i].pause=(pauses&(1u<<i))!=0;}
  const auto stage=a.session.state.stage;
  if(!a.session.update_multiplayer(in)){a.error=a.session.error;return 0;}
  if(fixture_record_io&&!a.multiplayer_menu_metadata())return 0;
  if(!a.capture_pause_frame()||!a.audio_events()||!a.text_events())return 0;
  a.audio.update();a.audio.pump();
  if(!a.session.draw(a.renderer)){a.error=a.session.error;return 0;}a.graphics.present();
  if(fixture_record_io){
   std::array<Netplay::FrameInput,3> frames;
   for(unsigned seat=0;seat<3;++seat)if(!th11::multiplayer::InputLanes::Capture(in[seat].held,in[seat].pause,0,0,0,frames[seat]))return 0;
   if(!a.multiplayer_replay.Append(a.multiplayer_replay.FrameCount(),stage,frames.data(),3)||!a.multiplayer_menu_save())return 0;
  }
#else
  if(!a.tick(p1,pauses!=0))return 0;
#endif
 }
 return 1;
}
// App storage seam only: no transport, measured startup, or full-run claim.
// The fixed non-production fingerprint makes every file unmistakably diagnostic.
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_record_io(unsigned read_only){
#ifdef TH11_MULTIPLAYER
 auto& a=th11::sdl::app;if(!a.session.battle||read_only>1)return 0;
 const auto& o=a.session.multiplayer_options;th11::multiplayer::SessionSetup setup;
 setup.session_id=0x5549465854483131ull;setup.player_count=o.seat_count;setup.local_player=o.local_seat;
 setup.difficulty=o.difficulty;setup.seed=o.seed;setup.automatic=false;
 for(unsigned i=0;i<3;++i)setup.selections[i]=unsigned(o.selections[i]);
 setup.build={0x55494658u,0x54483131u,2u,1u};
 if(!(read_only?a.netplay.BeginPlayback(setup):a.netplay.Reset(setup)))return 0;
 a.multiplayer_build=setup.build;
 if(!a.multiplayer_replay.Begin(setup,1791504000ull))return 0;
 a.session.recording_timestamp=a.multiplayer_replay.Timestamp();fixture_record_io=true;return 1;
#else
 return 0;
#endif
}
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_result_score(unsigned units){
#ifdef TH11_MULTIPLAYER
 auto& session=th11::sdl::app.session;if(units>999999999u||!session.battle)return 0;
 session.economy.score_units=int(units);auto& score=session.battle->hud.score;
 // This controlled terminal fixture represents a score whose native rolling
 // display has already caught up, not a one-frame artificial score award.
 score.displayed=int(units);score.speed=0;score.high=std::max(score.high,int(units));
 score.continues=score.high_continues=0;return 1;
#else
 return 0;
#endif
}
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_flush(){
#ifdef TH11_MULTIPLAYER
 return fixture_record_io&&th11::sdl::app.multiplayer_save(0,"PLAYER");
#else
 return 0;
#endif
}
EMSCRIPTEN_KEEPALIVE const char* mp_fixture_ui_saved(unsigned slot){
 static char out[1024];
#ifdef TH11_MULTIPLAYER
 auto& a=th11::sdl::app;char path[80];if(slot<1||slot>99)return "{\"valid\":false}";
 std::snprintf(path,sizeof(path),"/save/replay/th11_%.2u.rpy",slot);
 std::vector<th11::u8> bytes;th11::multiplayer::ReplayArchive replay;
 if(!a.read_file(path,bytes,16*1024*1024)||!replay.Load(bytes.data(),bytes.size()))return "{\"valid\":false}";
 std::string name;for(unsigned char c:replay.Name()){if(c=='\\'||c=='\"')name.push_back('\\');if(c>=32&&c<127)name.push_back(char(c));else name.push_back('?');}
 const auto& s=replay.Setup();
 std::snprintf(out,sizeof(out),"{\"valid\":true,\"slot\":%u,\"name\":\"%s\",\"scoreUnits\":%u,\"frames\":%u,\"bytes\":%u,\"playerCount\":%u,\"recordedPlayer\":%u,\"completed\":%s,\"diagnosticBuild\":[%u,%u,%u,%u]}",slot,name.c_str(),replay.Score(),replay.FrameCount(),unsigned(bytes.size()),s.player_count,s.local_player,replay.Completed()?"true":"false",s.build[0],s.build[1],s.build[2],s.build[3]);
#else
 std::snprintf(out,sizeof(out),"{\"valid\":false}");
#endif
 return out;
}
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_fixture(int kind){
#ifdef TH11_MULTIPLAYER
 auto& a=th11::sdl::app;auto* b=a.session.battle.get();if(!b)return 0;
 const float x[]={0,10,100};for(unsigned i=0;i<3;++i){auto& p=*b->pilots[i];auto& s=p.player->motion.state;s.x=int(x[i]*128);s.y=400*128;s.position={x[i],400,0};p.player->state.invincibility.set(100000,&b->animations.rate);p.economy.lives=i==1?1:3;p.economy.power=i==0?40:i==1?0:p.economy.max_power;p.power_changed();}
 if(kind==2){auto& q=*b->pilots[1];q.economy.lives=-1;if(!q.game_over(false))return 0;fixture_freeze_ghost=true;}
 return 1;
#else
 return 0;
#endif
}
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_terminal(int kind){
 auto& a=th11::sdl::app;if(!a.session.battle)return 0;
 a.session.battle->completion.state.exit=kind==0?th11::StageExit::Title:kind==1?th11::StageExit::Results:th11::StageExit::Ending;return 1;
}
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_replays(){
 auto& a=th11::sdl::app;
 if(!a.session.open_title(a.resources,false,th11::TitleScreen::Replays)){a.error=a.session.error;return 0;}
 a.scan_replays();return 1;
}
EMSCRIPTEN_KEEPALIVE const char* mp_fixture_ui_state(){
 static char out[2048];auto& a=th11::sdl::app;auto& s=a.session;
#ifdef TH11_MULTIPLAYER
 auto* b=s.battle.get();auto* p=b?b->pilots[0].get():nullptr;auto* q=b?b->pilots[1].get():nullptr;
 std::snprintf(out,sizeof(out),"{\"phase\":\"%s\",\"frame\":%u,\"hash\":%u,\"pauseState\":%d,\"titleSubstate\":%d,\"lifeHold\":%u,\"powerTaps\":%u,\"p1Lives\":%d,\"p2Lives\":%d,\"p1Power\":%d,\"p2Power\":%d,\"p2Ghost\":%s,\"endingFrames\":%u,\"ghostPinned\":%s,\"titleScreen\":%d,\"titleCursor\":%d,\"titleNameCursor\":%d,\"titleNameLength\":%d,\"pauseCursor\":%d,\"pauseNameCursor\":%d,\"pauseNameLength\":%d,\"sharedScoreUnits\":%d,\"displayedScoreUnits\":%d,\"recordIo\":%s,\"readOnly\":%s,\"archiveFrames\":%u,\"autoPath\":\"%s\"}",s.phase_name(),s.state.frame,s.multiplayer_hash(),s.pause_menu?s.pause_menu->state:-1,s.title?s.title->substate:-1,p?p->life_hold:0,p?p->power_taps:0,p?p->economy.lives:0,q?q->economy.lives:0,p?p->economy.power:0,q?q->economy.power:0,q&&q->ghost?"true":"false",s.ending?s.ending->frames:0,fixture_freeze_ghost?"true":"false",s.title?int(s.title->screen):-1,s.title?s.title->cursor.selected:-1,s.title?s.title->name_cursor.selected:-1,s.title?s.title->name_length:-1,s.pause_menu?s.pause_menu->cursor.selected:-1,s.pause_menu?s.pause_menu->name_cursor.selected:-1,s.pause_menu?s.pause_menu->name_length:-1,s.economy.score_units,b?b->hud.score.displayed:-1,fixture_record_io?"true":"false",a.netplay.ReadOnly()?"true":"false",a.multiplayer_replay.FrameCount(),a.multiplayer_auto_path.c_str());
#else
 std::snprintf(out,sizeof(out),"{\"phase\":\"%s\",\"frame\":%u,\"pauseState\":%d,\"titleSubstate\":%d}",s.phase_name(),s.state.frame,s.pause_menu?s.pause_menu->state:-1,s.title?s.title->substate:-1);
#endif
 return out;
}
}
