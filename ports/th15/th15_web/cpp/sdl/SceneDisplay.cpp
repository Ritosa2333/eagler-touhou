#include "SceneDisplay.hpp"
namespace th15::sdl {
SceneDisplay::SceneDisplay(FrameScheduler& f,AnmManager& a,AnmEnvironment& e,GraphicsDevice& g,AnmRenderer& r,ScreenViews& v,AsciiText& t,i32 text,i32 ascii):scheduler(f),animations(a),environment(e),graphics(g),renderer(r),views(v),captions(t),text_bank(text),ascii_bank(ascii),capture_device(g,r),screenshots(a,e,capture_device,text){}
SceneDisplay::~SceneDisplay(){detach();targets.reset();compositor.reset();caption_drawing.reset();animation_drawing.reset();}
bool SceneDisplay::fail(const std::string& why){if(error.empty())error=why.empty()?"Scene display failed":why;return false;}
bool SceneDisplay::initialize(){
 if(animation_drawing)return fail("Scene display already initialized");
 for(const auto bank:{text_bank,ascii_bank}){auto* file=animations.resource(bank);if(!file||!graphics.preload(*file))return fail(graphics.error.empty()?"Common scene animation unavailable":graphics.error);}
 views.configure(environment);if(!captions.initialize(ascii_bank))return fail(captions.error);
 animation_drawing=std::make_unique<AnmDrawSchedule>(scheduler,animations,renderer,graphics,views);caption_drawing=std::make_unique<AsciiFrame>(scheduler,captions,renderer,views);compositor=std::make_unique<ScreenCompositor>(scheduler,animations,environment,renderer,graphics,views);targets=std::make_unique<ScreenTargets>(animations,environment,views,*compositor,text_bank);
 return animation_drawing->error.empty()&&compositor->error.empty()||fail("Common scene draw callbacks unavailable");
}
bool SceneDisplay::attach(StageGameplay& scene,SessionState& progress,RecordStore& records,StageDrawServices& background,ReplayCalendarServices& dates,RunPause* pause,std::array<u8,0xa4>* replay){
 if(!targets||game)return fail("Scene display cannot attach this game");game=std::make_unique<GameDraw>(scene,progress,records,animations,renderer,graphics,views,captions,background,dates);
 // The native background constructor snapshots camera 3 before 44f300
 // refreshes that shared camera for the already active screen targets.
 if(!targets->prepare()){game.reset();return fail(targets->error);}
 screenshots.pause_source=compositor->alternate;game->pause=pause;game->captured=&targets->captures[2];game->replay_header=replay;return game->error.empty()||fail(game->error);
}
bool SceneDisplay::retain_background(StageGameplay& scene,StageDrawServices& services){
 if(!game||previous_background)return fail("Departing background cannot be retained");
 auto carried=std::make_unique<StageDrawFrame>(scheduler,scene.background,animations,renderer,graphics,views,services);carried->state=game->background.state;carried->viewport=game->background.viewport;carried->playfield_origin=game->background.playfield_origin;carried->rate=game->background.rate;
 if(!carried->error.empty())return fail(carried->error);previous_scene=&scene.background;previous_background=std::move(carried);previous_update.owner=this;previous_update.enabled=true;previous_update.run=[](void* p)->i32{auto& display=*static_cast<SceneDisplay*>(p);if(display.previous_scene&&display.previous_background&&display.previous_scene->update(display.previous_background->update_frame()))return 1;display.fail(display.previous_scene?display.previous_scene->error:"Retained background update has no owner");return 5;};if(scheduler.add(previous_update,FramePass::Update,17)<0)return fail("Retained background callback registration failed");renderer.flush();game.reset();screenshots.discard();return true;
}
void SceneDisplay::release_previous()noexcept{scheduler.remove(previous_update);previous_background.reset();previous_scene=nullptr;}
void SceneDisplay::detach(){renderer.flush();release_previous();game.reset();screenshots.discard();if(targets)targets->deactivate();graphics.select_target(nullptr,0);}
bool SceneDisplay::capture(u32& handle,bool results){if(!targets||!compositor->active)return fail("Screenshot outside an active playfield");return screenshots.capture(handle,results)||fail(screenshots.error);}
bool SceneDisplay::draw(){
 if(!error.empty()||!compositor)return false;graphics.presentation.begin();if(game)game->prepare_frame();if(!graphics.select_target(nullptr,0))return fail(graphics.error);
 if(scheduler.draw()<0){if(game&&!game->error.empty())return fail(game->error);if(game&&!game->background.error.empty())return fail(game->background.error);if(!animation_drawing->error.empty())return fail(animation_drawing->error);if(!captions.error.empty())return fail(captions.error);return fail(compositor->error);}
 renderer.flush();graphics.presentation.finish();if(!screenshots.finish_frame())return fail(screenshots.error);graphics.present();return graphics.error.empty()||fail(graphics.error);
}
}
