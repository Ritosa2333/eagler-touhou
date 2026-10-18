// Included only by build-spectator-stage.mjs into its private BrowserRuntime.
#include <map>
#include "../game/EclFile.hpp"
namespace th15::sdl {
namespace {
std::vector<u8> spectator_lab_capsule;
std::vector<u8> spectator_lab_logic_bytes;
unsigned spectator_lab_frame=0;
unsigned spectator_lab_saved_frame=0;
unsigned spectator_lab_hash(const std::vector<u8>& bytes){unsigned hash=2166136261u;for(auto byte:bytes)hash=(hash^byte)*16777619u;return hash;}
void spectator_lab_logic(std::vector<u8>& bytes,GameBattle& battle){
 Netplay::SpectatorScalars::Writer w;std::vector<u8> logic;
 auto flush=[&](){if(!w.Good())return false;logic.insert(logic.end(),w.Bytes().begin(),w.Bytes().end());w=Netplay::SpectatorScalars::Writer{};return true;};
 auto timer=[&](const Timer& t){return w.Values(t.previous,t.current,t.fractional,t.rate_index,t.flags);};
 auto motion=[&](const MotionState& m){return w.Values(m.position.x,m.position.y,m.position.z,m.origin.x,m.origin.y,m.origin.z,m.speed,m.angle,m.radius,m.angular_velocity,m.axis_angle,m.axis_scale,m.phase,m.velocity.x,m.velocity.y,m.velocity.z,m.flags);};
 w.Values(battle.enemies->next_identifier,battle.enemies->rate,battle.enemies->count());timer(battle.enemies->timer);flush();
 for(u32 n=0;n<battle.enemies->count();n++){
  auto& e=*battle.enemies->at(n);auto& s=e.state;
  w.Values(s.id,s.parent_id,s.flags,s.life,s.initial_life,s.phase_life,s.pending_damage,s.cumulative_damage,s.lifecycle_flags);motion(s.motion);motion(s.absolute);motion(s.relative);timer(s.age_timer);timer(s.lifetime_timer);timer(s.phase_timer);w.Array(s.integer_registers);w.Array(s.float_registers);
  w.Value(e.scripts.count());flush();for(u32 thread=0;thread<e.scripts.count();thread++){
   EclFile codec;EclFile::Block block{};if(!codec.write(*e.scripts.at(thread),block)){last_error=codec.error;block.fill(0xff);}logic.insert(logic.end(),block.begin(),block.end());
  }
 }
 for(unsigned seat=0;seat<battle.mp_options.seat_count;seat++){
  auto& player=*battle.pilots[seat]->player;w.Array(player.shots.reward_counters);w.Values(player.damage.cursor,player.damage.maximum_damage,player.damage.score);
  for(const auto& s:player.shots.shots){w.Values(s.flags,s.index,s.state,s.target,s.damage,s.specification,s.damage_source);if(s.flags){motion(s.motion);timer(s.age);timer(s.behavior);timer(s.auxiliary);}}
  for(const auto& d:player.damage.sources){w.Value(d.flags);if(d.flags){w.Values(d.radius,d.radius_growth,d.angle,d.angular_speed,d.damage,d.accumulated,d.damage_limit,d.interval,d.last_target,d.shot_index,d.callback_target,d.callback_kind);motion(d.motion);timer(d.lifetime);}}
  if(!flush())last_error="Spectator fixture canonical player state exceeded section limit";
 }
 spectator_lab_logic_bytes=std::move(logic);bytes.insert(bytes.end(),spectator_lab_logic_bytes.begin(),spectator_lab_logic_bytes.end());
}
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_begin(unsigned count,unsigned stage){
 using namespace th15::sdl;th15::multiplayer::SessionSetup setup;setup.session_id=0x12345678;setup.seed=417;setup.player_count=count;setup.local_player=0;setup.difficulty=stage==7?4:1;setup.automatic=false;setup.build={1,2,3,4};setup.selections=count==2?std::array<unsigned,3>{2,3,0}:std::array<unsigned,3>{0,1,2};
 if(!app||running||!setup.valid()||!network.Reset(setup))return 0;
 app->files.read_only=true;spectator_lab_frame=0;app->audio_device.music_enabled=false;app->loading_startup=false;
 if(!th15_mp_start())return 0;if(stage!=setup.initial_stage()){app->selected_stage=stage;app->mp_options.stage=stage;if(!app->begin_run())return 0;}
 return 1;
}
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_step(unsigned count,unsigned held){
 using namespace th15::sdl;for(unsigned n=0;n<count;n++){
  if(!app||!app->scene())return 0;std::array<th15::MultiplayerInput,3> lanes{};lanes[0].held=held;
  app->scene()->battle.mp_input(lanes);mp_menu.update(held);
  if(!app->step(mp_menu.held,mp_menu.pressed,mp_menu.repeated,60,false)){last_error=app->failure;return 0;}++spectator_lab_frame;
 }return 1;
}
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_boundary(){using namespace th15::sdl;return app&&th15::multiplayer::SpectatorStageCheckpoint::Boundary(*app);}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_spectator_lab_boundary_state(){using namespace th15::sdl;static std::array<unsigned,12> out{};out.fill(0);if(app&&app->scene()){auto& scene=*app->scene();auto& b=scene.battle;out={unsigned(app->progress.stage),unsigned(app->progress.stage_frame),scene.spectator_ready(),app->run&&app->run->previous_scene()?1u:0u,app->display&&app->display->previous_background?app->display->previous_background->state.flags:0u,app->pending_load,app->pause?unsigned(app->pause->state.screen):0u,b.spell.flags,b.mp_bomb_active()?1u:0u,b.laser_scene->manager.count(),scene.messages.active()?1u:0u,app->loading_waiting()?1u:0u};}return out.data();}
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_capture(){using namespace th15::sdl;spectator_lab_saved_frame=spectator_lab_frame;return app&&th15::multiplayer::SpectatorStageCheckpoint::Capture(*app,network.Setup(),mp_menu,spectator_lab_frame,spectator_lab_capsule,last_error);}
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_restore(){using namespace th15::sdl;spectator_lab_frame=spectator_lab_saved_frame;return app&&th15::multiplayer::SpectatorStageCheckpoint::Restore(*app,network.Setup(),mp_menu,spectator_lab_frame,spectator_lab_capsule,last_error);}
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_import(const unsigned char* bytes,unsigned size,unsigned frame){using namespace th15::sdl;if(!bytes||!size||size>Netplay::SpectatorStageMaxStateBytes)return 0;spectator_lab_capsule.assign(bytes,bytes+size);spectator_lab_saved_frame=frame;return th15_spectator_lab_restore();}
EMSCRIPTEN_KEEPALIVE unsigned th15_spectator_lab_frame(){return th15::sdl::spectator_lab_frame;}
EMSCRIPTEN_KEEPALIVE const unsigned char* th15_spectator_lab_logic_bytes(){return th15::sdl::spectator_lab_logic_bytes.data();}
EMSCRIPTEN_KEEPALIVE unsigned th15_spectator_lab_logic_size(){return th15::sdl::spectator_lab_logic_bytes.size();}
EMSCRIPTEN_KEEPALIVE const unsigned char* th15_spectator_lab_bytes(){return th15::sdl::spectator_lab_capsule.data();}
EMSCRIPTEN_KEEPALIVE unsigned th15_spectator_lab_size(){return th15::sdl::spectator_lab_capsule.size();}
EMSCRIPTEN_KEEPALIVE const char* th15_spectator_lab_error(){using namespace th15::sdl;return last_error.empty()?app?app->failure.c_str():"No application":last_error.c_str();}
EMSCRIPTEN_KEEPALIVE const char* th15_spectator_lab_publication(){using namespace th15::sdl;static std::string text;text="stage="+std::to_string(mp_spectator_stage)+" capture="+mp_spectator_error;return text.c_str();}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_spectator_lab_state(){
 using namespace th15::sdl;static std::array<unsigned,16> out{};out.fill(0);if(!app||!app->scene())return out.data();auto& b=app->scene()->battle;
 th15::multiplayer::SpectatorStageState state;state.setup=network.Setup();state.setup.local_player=0;state.progress=app->progress;state.score=b.score;state.enemy=b.enemy_world;state.game_random=app->environment.game_rng;state.visual_random=app->visual;state.menu=mp_menu;state.frame=spectator_lab_frame;state.application_frames=app->frames;state.fragments=b.mp_fragments;
 for(unsigned n=0;n<state.setup.player_count;n++)th15::multiplayer::CaptureSpectatorPilot(*b.pilots[n],state.pilots[n]);std::vector<th15::u8> bytes;
 out[0]=th15::multiplayer::EncodeSpectatorStageState(state,bytes)?(spectator_lab_logic(bytes,b),spectator_lab_hash(bytes)):0;out[1]=app->progress.stage;out[2]=app->progress.stage_frame;out[3]=app->frames;out[4]=app->environment.game_rng.seed;out[5]=app->environment.game_rng.calls;out[6]=app->visual.seed;out[7]=app->visual.calls;out[8]=b.enemies->count();out[9]=b.bullet_scene->manager.visible_count;out[10]=b.items->active_count;out[11]=app->session->driver()->runtime.age.current;out[12]=app->scene()->background.frames;out[13]=app->scene()->hud.flags;out[14]=app->scene()->hud.intro_age.current;
 for(bool alternate:{false,true})for(unsigned n=0;n<app->animations.registry.ordered_count(alternate);n++){const auto h=app->animations.registry.ordered_handle(alternate,n);if(auto* vm=app->animations.registry.find(h))if((vm->visual.render_flags>>5&3)!=1)++out[15];}
 return out.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_spectator_lab_network_state(unsigned frame){th15::sdl::spectator_lab_frame=frame;return th15_spectator_lab_state();}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_spectator_lab_animation_counts(){using namespace th15::sdl;static std::array<unsigned,3> out{};out.fill(0);if(!app)return out.data();out[0]=app->animations.registry.count();for(bool alternate:{false,true})for(unsigned n=0;n<app->animations.registry.ordered_count(alternate);n++){const auto h=app->animations.registry.ordered_handle(alternate,n);if(auto* vm=app->animations.registry.find(h))++out[(vm->visual.render_flags>>5&3)==1?2:1];}return out.data();}
EMSCRIPTEN_KEEPALIVE const char* th15_spectator_lab_animation_inventory(){using namespace th15::sdl;static std::string out;out="{";if(!app)return "{}";std::map<std::string,unsigned> counts;for(bool alternate:{false,true})for(unsigned n=0;n<app->animations.registry.ordered_count(alternate);n++){const auto h=app->animations.registry.ordered_handle(alternate,n);if(auto* vm=app->animations.registry.find(h)){const int bank=app->animations.resource_id(vm->resource);const auto at=app->animations.resource_names.find(bank);const std::string name=at==app->animations.resource_names.end()?"unknown":at->second;counts[name+"/"+std::to_string(vm->source_script)+"/"+std::to_string(vm->visual.render_flags>>5&3)]++;}}for(const auto& entry:counts){if(out.size()>1)out+=",";out+="\""+entry.first+"\":"+std::to_string(entry.second);}out+="}";return out.c_str();}
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_next(){using namespace th15::sdl;return app&&app->complete_stage();}
EMSCRIPTEN_KEEPALIVE int th15_spectator_lab_distinct_power(){using namespace th15::sdl;if(!app||!app->scene())return 0;auto& battle=app->scene()->battle;for(unsigned seat=0;seat<battle.mp_options.seat_count;seat++){auto& pilot=*battle.pilots[seat];pilot.session.power=100+int(seat)*100;if(!pilot.player->configure_options()){last_error=pilot.player->options.error;return 0;}}return 1;}
}
