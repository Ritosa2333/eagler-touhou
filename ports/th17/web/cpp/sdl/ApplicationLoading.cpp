#include "ApplicationState.hpp"
#include "../game/AnmOverlay.hpp"
#include <SDL3/SDL.h>
namespace th17::sdl {
bool ApplicationState::advance_flow(){
 bool loading=false;
 if(flow.pending!=flow.current)switch(flow.pending){
 case AppScene::Game:case AppScene::Restart:case AppScene::ReplayRestart:
 case AppScene::ReplayStart:case AppScene::ExtraGameOver:case AppScene::RetrySelection:loading=true;break;
 case AppScene::Title:case AppScene::Ending:case AppScene::Results:loading=scene()!=nullptr;break;
 default:break;
 }
 if(loading){
  // The native loader draws while a worker builds the next owner. Wasm's
  // synchronous constructors must run after the browser can present that frame.
  // Stage-to-stage transitions retain their existing scrolling background.
  auto* overlay=animations.registry.find(transition_overlay);
  // Native opening panels remain owned by the scene after they disappear.
  // Only reuse scripts 3..6, which are still closing; scripts 7..10 belong
  // to the preceding entrance and cannot close again on a restart.
  if(!overlay||!overlay->geometry.overlay||(overlay->visual.render_flags&0x60)||!overlay->geometry.overlay->panels[0]||overlay->geometry.overlay->panels[0]->source_script!=3){
   u32 handle=0;if(!begin_transition(handle)||!animations.interrupt(handle,7))return false;
   overlay=animations.registry.find(handle);
  }
  if(!loading_overlay())return false;
  loading_destination=flow.pending;loading_frames=0;
  // effect.anm scripts 3..6 finish closing at frame 45. Reuse a closing
  // transition that the title already started instead of restarting it.
  loading_duration=u32(std::max(2,46-overlay->geometry.overlay->frames));
  loading_visual=visual;loading_game=environment.game_rng;return true;
 }
 const i32 result=flow.update();if(result==4){exiting=true;return true;}return result==1||fail(flow.error);
}
bool ApplicationState::draw_loading(){
 if(!graphics.select_target(nullptr,0))return fail(graphics.error);
 renderer.invalidate();graphics.configure_game(1);graphics.clear(0xff000000);graphics.set_fog(false);
 environment.render_target_has_alpha=true;views.configure(environment);views.camera(DrawCamera::Fullscreen,false);renderer.offset={};
 if(auto* vm=animations.registry.find(transition_overlay))if(renderer.draw(*vm)==-2)return fail(renderer.error);
 // Only the original loading ANMs draw here. No outgoing title, pause menu,
 // stage counters, capture callbacks or replay input advance during loading.
 for(auto* vm:animations.registry.layer(42))if(vm->resource==animations.resource(0)&&vm->source_script>=14&&vm->source_script<=17)
  if(renderer.draw(*vm)==-2)return fail(renderer.error);
 renderer.flush();graphics.present();return graphics.error.empty()||fail(graphics.error);
}
bool ApplicationState::step_loading(){
 const auto game=environment.game_rng,cosmetic=visual;
 visual=loading_visual;environment.game_rng=loading_game;
 const bool updated=animations.update(true)&&animations.update(false);
 loading_visual=visual;loading_game=environment.game_rng;visual=cosmetic;environment.game_rng=game;
 if(!updated)return fail(animations.error);
 if(!draw_loading())return false;
 audio_device.update();audio_device.pump();if(!audio_device.error.empty())return fail(audio_device.error);
 ++frames;if(++loading_frames<loading_duration)return true;
 flow.pending=loading_destination;loading_destination=AppScene::Uninitialized;
 const i32 result=flow.update();
 if(result==4){exiting=true;return true;}if(result!=1)return fail(flow.error);
 if(!scene()&&!end_loading_overlay())return false;
 // Loading time is not a slow gameplay sample or replay slowdown statistic.
 performance.rebase(double(SDL_GetTicksNS())/1e9);return true;
}
}
