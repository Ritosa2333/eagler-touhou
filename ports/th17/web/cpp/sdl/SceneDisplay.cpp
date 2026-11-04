#include "SceneDisplay.hpp"
#include <cstdio>
namespace th17::sdl {
SceneDisplay::SceneDisplay(StageGameplay& s,AnmManager& a,AnmEnvironment& e,GraphicsDevice& g,AnmRenderer& r):scene(s),animations(a),environment(e),graphics(g),renderer(r),views(r,e),captions(a,e),capture_device(g,r){}
SceneDisplay::~SceneDisplay(){renderer.flush();scene.frame_scheduler().remove(fps_callback);screenshots.reset();scene.scene_animations.fill(nullptr);targets.reset();game.reset();outgoing_background.reset();compositor.reset();letters.reset();layers.reset();graphics.select_target(nullptr,0);}
bool SceneDisplay::fail(const std::string& reason){if(error.empty())error=reason.empty()?"TH17 scene drawing failed":reason;return false;}
bool SceneDisplay::initialize(GameRun& run,GameRecords& records,SpellRecords& spells,StageDrawServices& host,i32 text_bank,i32 ascii_bank,StageGameplay* previous){
    if(layers)return fail("TH17 scene display already initialized");if(!captions.initialize(ascii_bank))return fail(captions.error);
    auto& scheduler=scene.frame_scheduler();
    layers=std::make_unique<AnmDrawSchedule>(scheduler,animations,renderer,graphics,views);
    letters=std::make_unique<AsciiDrawSchedule>(scheduler,captions,renderer,views);
    compositor=std::make_unique<ScreenCompositor>(scheduler,animations,environment,renderer,graphics,views);
    if(previous)outgoing_background=std::make_unique<StageDrawFrame>(scheduler,previous->background,previous->background_draw,animations,renderer,graphics,views,host);
    game=std::make_unique<GameDraw>(scene,run,records,spells,animations,renderer,graphics,views,captions,host);
    targets=std::make_unique<ScreenTargets>(animations,environment,views,*compositor,text_bank);
    if(!targets->prepare())return fail(targets->error);
    screenshots=std::make_unique<SceneCapture>(animations,environment,capture_device,text_bank);screenshots->pause_source=compositor->alternate;game->captured=&targets->captures[2];
    for(u32 i=0;i<scene.scene_animations.size();i++)scene.scene_animations[i]=&targets->captures[i];
    fps_callback.owner=this;fps_callback.enabled=true;fps_callback.run=[](void* owner)->i32{auto& self=*static_cast<SceneDisplay*>(owner);HudTextStyle style;style.font=1;style.color=self.presentation_fps<30?0xff5050ff:self.presentation_fps<40?0xffa0a0ff:0xffffffff;char value[32];std::snprintf(value,sizeof value,"%2.1ffps",double(self.presentation_fps)+.05);return self.captions.enqueue({{588,470,0},style,value})?1:5;};
    return scheduler.add(fps_callback,FramePass::Draw,72)>=0||fail("Frame-rate callback registration failed");
}
bool SceneDisplay::capture(u32& handle,bool results){return screenshots&&screenshots->capture(handle,results)||fail(screenshots?screenshots->error:"Screenshot outside an active playfield");}
bool SceneDisplay::draw(){
    if(!error.empty()||!compositor||!game)return false;if(!graphics.select_target(nullptr,0))return fail(graphics.error);
    if(!game->draw())return fail(game->error.empty()?game->background.error:game->error);
    if(outgoing_background&&!outgoing_background->error.empty())return fail(outgoing_background->error);
    if(!compositor->error.empty())return fail(compositor->error);if(!layers->error.empty())return fail(layers->error);if(!letters->error.empty())return fail(letters->error);
    renderer.flush();if(!screenshots->finish_frame())return fail(screenshots->error);graphics.present();if(!scene.presented(double(SDL_GetPerformanceCounter())/double(SDL_GetPerformanceFrequency())))return fail(scene.error);return graphics.error.empty()||fail(graphics.error);
}
}
