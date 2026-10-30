#include "RunInput.hpp"
namespace th17 {
RunInput::RunInput(StageGameplay& s,GameRun& p,RunInputServices& h,Replay* r,ReplayRecording* w,u16 seed,bool fresh):scene(s),progress(p),host(h),replay(r),recording(w){
 snapshot.stage=u16(p.stage);snapshot.seed=seed;snapshot.frame_gate=fresh?1:0;snapshot.run=scene.capture_run();snapshot.run.counters={};for(u32 i=0;i<20;i++)snapshot.reserved[i]=i*0xdeaddedadu;
 // 0x4310a2 retains priority-17 input. The outgoing stream records/plays the
 // HUD wait and fade; 0x44f5a0 switches streams only when main activates.
 active=!fresh&&!progress.historical_web_replay;
 callback.owner=this;callback.enabled=true;callback.run=[](void* p)->i32{return static_cast<RunInput*>(p)->scheduled();};if(scene.frame_scheduler().add(callback,FramePass::Update,17)<0)fail("Run input callback registration failed");
 acceleration_callback.owner=this;acceleration_callback.enabled=true;acceleration_callback.run=[](void* p)->i32{return static_cast<RunInput*>(p)->accelerate_replay();};if(scene.frame_scheduler().add(acceleration_callback,FramePass::Update,36)<0)fail("Replay acceleration callback registration failed");
}
RunInput::~RunInput(){scene.frame_scheduler().remove(acceleration_callback);scene.frame_scheduler().remove(callback);}
bool RunInput::fail(const std::string& why){if(error.empty())error=why;return false;}
bool RunInput::activate(){
 input={};if(replay){if(!replay->select(progress.stage))return fail("Replay stage unavailable at input activation");}
 else if(recording){if(!snapshot.frame_gate){snapshot.run=scene.capture_run();snapshot.run.counters={};}snapshot.player=scene.battle.player->motion.position_fixed;snapshot.focus=scene.battle.player->motion.focus;if(!recording->begin(reinterpret_cast<const u8*>(&snapshot),sizeof snapshot))return fail(recording->error());recording->activate();}
 active=entered=true;return true;
}
i32 RunInput::scheduled(){
 if(!error.empty())return i32(FrameAction::Error);if(!active)return i32(FrameAction::Continue);
 PlayerTouch movement=touch;if(replay){const auto value=replay->tick();recorded_fps=value.fps;if(value.end){input={};auto& frame=scene.battle.active_frame();frame.held=frame.pressed=0;frame.shoot_frames=frame.skip_frames=0;scene.battle.player->motion.touch={};if(!host.replay_finished())return i32(FrameAction::Error);frame.game_flags=progress.scene_flags;return i32(FrameAction::Continue);}input.update(value.held);movement=value.touch;}
 else {input.recording_update(u16(physical),auto_focus);if(recording&&!recording->tick({u16(input.held),u16(input.pressed),u16(input.released)},fps,false,touch)){fail(recording->error());return i32(FrameAction::Error);}}
 auto& frame=scene.battle.active_frame();frame.held=input.held;frame.pressed=input.pressed;frame.shoot_frames=i32(input.duration[0]);frame.skip_frames=i32(input.duration[9]);scene.battle.player->motion.touch=movement;return i32(FrameAction::Continue);
}
i32 RunInput::accelerate_replay(){
 // Native 44f4b0/44e610 runs at priority 36, after the alternate ANM pass.
 // Physical Z/Ctrl repeats updates to the next eight-input boundary; recorded
 // keys remain untouched and the pause callback can stop before consumption.
 if(!entered||!replay||(progress.scene_flags&4)||!(physical&0x201))return i32(FrameAction::Continue);
 return replay->frame()%8?i32(FrameAction::Restart):i32(FrameAction::Continue);
}
}
