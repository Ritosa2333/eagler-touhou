#include "SessionGameplay.hpp"
namespace th15 {
SessionGameplay::SessionGameplay(StageGameplay& scene,SessionState& progress,SessionGameplayServices& services,ReplayRecording* recording,ReplayPlayback* playback):scene(scene),progress(progress),services(services),recording(recording),playback(playback),runtime(progress,scene.battle.session,*this){
 const FrameCallback::Function functions[]={[](void* p)->i32{return static_cast<SessionGameplay*>(p)->update_runtime();},[](void* p)->i32{return static_cast<SessionGameplay*>(p)->update_input();},[](void* p)->i32{return static_cast<SessionGameplay*>(p)->accelerate_replay();}};
 constexpr i32 priorities[]={15,16,35};
 for(u32 i=0;i<3;i++){callbacks[i].owner=this;callbacks[i].run=functions[i];callbacks[i].enabled=true;if(scene.frame_scheduler().add(callbacks[i],FramePass::Update,priorities[i])<0){fail("Session callback registration failed");for(auto& c:callbacks)scene.frame_scheduler().remove(c);return;}}
 previous_chapter_owner=scene.battle.enemy_world.chapter_request_owner;scene.battle.enemy_world.chapter_request_owner=&runtime.requested_chapter;attached=true;
}
SessionGameplay::~SessionGameplay(){for(auto& c:callbacks)scene.frame_scheduler().remove(c);if(scene.battle.enemy_world.chapter_request_owner==&runtime.requested_chapter)scene.battle.enemy_world.chapter_request_owner=previous_chapter_owner;}
bool SessionGameplay::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Session gameplay operation failed":reason;return false;}
bool SessionGameplay::step(const SessionGameplayInput& input){if(!error.empty()||!attached)return false;frame=input;if(!scene.step(frame.scene))return fail(error.empty()?scene.error:error);return error.empty();}
i32 SessionGameplay::update_runtime(){
 const SessionFrameState input{scene.hud.flags,scene.hud.intro_age.current,scene.battle.player->life.state,frame.background_finished,frame.return_to_selection,frame.physical_pressed};
 const i32 action=runtime.update(input);scene.battle.active_frame().game_flags=progress.scene_flags;if(action==i32(FrameAction::Error))fail(scene.error.empty()?runtime.error:scene.error);return action;
}
i32 SessionGameplay::update_input(){
 scene.battle.player->motion.touch={};
 if(!input_enabled)return i32(FrameAction::Continue);
 if((recording&&recording->frame_clock()<0)||(playback&&playback->frame_clock()<0))return i32(FrameAction::Continue);
 if(progress.replay&&!playback){fail("Replay input controller unavailable");return i32(FrameAction::Error);}
 if(playback){const bool before=playback->finished;if(!playback->tick()){fail(playback->error());return i32(FrameAction::Error);}if(!before&&playback->finished&&!services.finish_replay()){fail("Replay completion failed");return i32(FrameAction::Error);}}
 else {live.recording_update(u16(frame.scene.battle.held),auto_focus);if(recording&&!recording->tick({u16(live.held),u16(live.pressed),u16(live.released)},frame.fps,(scene.battle.session.mode_flags&0x300)!=0,frame.touch)){fail(recording->error());return i32(FrameAction::Error);}}
 scene.battle.player->motion.touch=playback?playback->touch:frame.touch;scene.apply_input(controls());return i32(FrameAction::Continue);
}
// Original 45ceb0 runs at update priority 35: repeat updates until the
// playback clock reaches an eight-frame boundary, then draw once. Live
// input stays separate from the replay stream and never changes its keys.
i32 SessionGameplay::accelerate_replay(){
 if(!playback||!input_enabled||playback->finished||(progress.scene_flags&4)||!(frame.scene.battle.held&0x201)||playback->frame_clock()<0)return i32(FrameAction::Continue);
 return playback->frame_clock()%8?i32(FrameAction::Restart):i32(FrameAction::Continue);
}
bool SessionGameplay::ending_fade(){return services.ending_fade();}
bool SessionGameplay::finish_replay(){return services.finish_replay();}
bool SessionGameplay::destination(SessionDestination d){return services.destination(d);}
bool SessionGameplay::load_scene(bool& waiting){return services.load_scene(waiting);}
bool SessionGameplay::activate_scene(){return services.activate_scene();}
bool SessionGameplay::release_background(){return services.release_background();}
bool SessionGameplay::load_checkpoint_file(bool& restored){return services.load_checkpoint_file(restored);}
bool SessionGameplay::restart_overlay(i32 script){return services.restart_overlay(script);}
bool SessionGameplay::restart_effect(i32 label){return services.restart_effect(label);}
bool SessionGameplay::prepare_stage_music(){return services.prepare_stage_music();}
bool SessionGameplay::start_stage_music(){return services.start_stage_music();}
bool SessionGameplay::start_boss_music(){return services.start_boss_music();}
bool SessionGameplay::seek_stage_music(double seconds){return services.seek_stage_music(seconds);}
bool SessionGameplay::demo_fade(){return services.demo_fade();}
bool SessionGameplay::update_score(){scene.hud.update_score();return true;}
bool SessionGameplay::chapter_reward(bool boss){return scene.chapter_reward(boss);}
bool SessionGameplay::skip_chapter_reward(){return scene.skip_chapter_reward();}
bool SessionGameplay::chapter_checkpoint(i32 chapter){return scene.capture(chapter);}
bool SessionGameplay::update_overlays(){return services.update_overlays();}
}
