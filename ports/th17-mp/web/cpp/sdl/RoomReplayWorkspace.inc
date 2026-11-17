namespace {
std::string replay_description;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE const char* eagler_replay_inspect(const th17::u8* bytes,unsigned size){
 th17::multiplayer::ReplayArchive replay;
 if(!replay.load(bytes,size))return nullptr;
 eagler::replay::Description description;
 description.encoded_name(replay.name().c_str(),unsigned(replay.name().size()));
 description.number("character",replay.setup().loadouts[0].character);
 description.number("difficulty",replay.setup().difficulty);
 description.number("score",double(replay.score())*10);
 description.number("timestamp",double(replay.timestamp()));
 const auto& info=replay.info();
 for(unsigned n=0;n<info.chapterCount;++n){
  const auto stage=info.chapters[n].label;
  const auto number=stage%8;
  description.stage_without_score(stage,0,number==7?"EXTRA":"STAGE "+std::to_string(number));
 }
 replay_description=description.finish();return replay_description.c_str();
}
EMSCRIPTEN_KEEPALIVE int th17_mp_replay_load(const th17::u8* bytes,unsigned size,const unsigned* build,unsigned stage){
 using namespace th17::sdl;
 if(running||!app||!app->title||!build)return 0;
 auto candidate=std::make_unique<RoomRuntime>();
 if(!candidate->replay.load(bytes,size))return 0;
 candidate->setup=candidate->replay.setup();
 if(!std::equal(candidate->setup.build.begin(),candidate->setup.build.end(),build))
  return app->fail("TH17 multiplayer Replay requires its recorded Runtime build");
 const auto& info=candidate->replay.info();
 bool found=false;
 for(unsigned n=0;n<info.chapterCount;++n)if(info.chapters[n].label==stage){
  candidate->bootstrap_target=info.chapters[n].firstFrame;found=true;break;
 }
 if(!found)return 0;
 candidate->start_stage=stage;candidate->playing_replay=true;
 room=std::move(candidate);app->room_preview=false;app->title->state.room_preview=false;
 app->room_replay_save=[](th17::i32,const std::array<char,9>&,bool){return true;};
 app->frames=0;return 1;
}
EMSCRIPTEN_KEEPALIVE const unsigned* eagler_replay_state(){
 using namespace th17::sdl;
 static std::array<unsigned,6> result{};
 result={};
 if(room&&room->playing_replay)result={room->playback_frame,room->replay.frames(),
  unsigned(room->playback_frame>=room->replay.frames()),app?unsigned(app->progress.stage):0,
  room->start_stage,room->bootstrap_target};
 return result.data();
}
EMSCRIPTEN_KEEPALIVE int eagler_replay_step(unsigned render){
 using namespace th17::sdl;
 if(!room||!room->playing_replay||!app)return 0;
 app->graphics.logical_drawing=!render;
 app->room_silent=!render;
 app->graphics.backend.defer=true;
 const bool ok=room->advance_playback(true);
 app->graphics.logical_drawing=false;app->room_silent=false;
 if(render){app->graphics.backend.commit();app->graphics.backend.defer=false;}
 return ok;
}
EMSCRIPTEN_KEEPALIVE void eagler_replay_present(){
 using namespace th17::sdl;
 if(app){app->graphics.backend.commit();app->graphics.backend.defer=false;}
}
EMSCRIPTEN_KEEPALIVE void eagler_replay_audio_pump(){using namespace th17::sdl;if(app)app->audio_device.pump();}
EMSCRIPTEN_KEEPALIVE const char* eagler_replay_error(){return th17_error();}
}
