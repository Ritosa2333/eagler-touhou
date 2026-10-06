// Development-only probes for scene lifecycle, drawing and controlled death regression.
#include "../../cpp/sdl/ApplicationState.hpp"
#include <memory>
#include <emscripten.h>
using namespace th15;
namespace {std::unique_ptr<sdl::ApplicationState> app;std::string last_error,music_name;}
extern "C" {
i32 application_initialize(i32 device){if(!app)app=std::make_unique<sdl::ApplicationState>();if(!app->initialize(device!=0)){last_error=app->failure;app.reset();return 0;}return 1;}
void application_render_scale(u32 scale){if(!app)app=std::make_unique<sdl::ApplicationState>();app->graphics.render_scale=scale>=2?2:1;}
i32 application_step(u32 held,u32 pressed,u32 repeated,float fps,i32 focus){return app&&app->step(held,pressed,repeated,fps,focus!=0);}
const char* application_error(){return app?app->failure.c_str():last_error.c_str();}
const i32* application_state(){return app?app->projection():nullptr;}
const i32* application_manual_state(){static std::array<i32,5> state;state={-1,-1,-1,-1,-1};if(app&&app->pause_manual){const auto& m=*app->pause_manual;state={m.mode,m.phase,m.age.current,m.menu.cursor,truncate_int(m.offset_x)};}return state.data();}
void application_title_select(i32 cursor){if(app&&app->title)app->title->state.menu.select(cursor);}
i32 application_complete_stage(){return app&&app->complete_stage();}
const i32* application_transition_state(){static std::array<i32,9> state;state.fill(-1);if(app){auto* display=app->display.get();if(display&&display->game){const auto& d=display->game->background;state[0]=d.state.flags;state[1]=d.state.transition.current;state[2]=app->scene()->background.frames;}if(display&&display->previous_background){const auto& d=*display->previous_background;state[3]=d.state.flags;state[4]=d.state.transition.current;}if(app->run)state[5]=app->run->previous_scene()?1:0;if(app->scene()){state[6]=app->scene()->battle.enemies->count();state[7]=app->scene()->battle.bullet_scene->manager.visible_count;}state[8]=app->fades.size();}return state.data();}
const char* application_music(){if(!app)return "";double seconds=0;app->audio_device.current_music(music_name,seconds);return music_name.c_str();}
const u8* application_pixels(){if(!app)return nullptr;app->graphics.backend.read(sdl::GraphicsDevice::screen);return app->graphics.pixels(sdl::GraphicsDevice::screen)->pixels.data();}
const touhou::sdl::Statistics* application_graphics_stats(){return app?&app->graphics.backend.stats:nullptr;}
void application_keyboard_key(u32 key,u32 down){if(app&&key<256){app->keyboard_state.format=2;app->keyboard_state.keys[key]=down?128:0;}}
void application_test_protection(i32 frames){if(app&&app->scene())app->scene()->battle.player->life.invulnerability.set(frames);}
i32 application_test_game_over(){if(!app||!app->scene()||!app->pause)return 0;app->scene()->battle.session.extra_lives=-1;return app->pause->open(PauseEntrance::GameOver,false);}
i32 application_test_death(){return app&&app->scene()&&app->scene()->battle.player->life.commit_death();}
const i32* application_pause_state(){static std::array<i32,6> value;value.fill(-1);if(app&&app->pause){auto& p=app->pause->state;value={i32(p.screen),p.phase,p.menu.cursor,p.names.cursor,p.age.current,i32(app->audio_device.sound_playing(14))};}return value.data();}
void application_test_finish_name(){if(app&&app->pause)app->pause->state.names.select(90);}
void application_close(){app.reset();}
}

extern "C" const i32* application_checkpoint_state(){static std::array<i32,9> state{};state.fill(0);if(app&&app->scene()){const auto& c=app->scene()->checkpoint;const auto& s=c.state().state();state={c.ready()?1:0,s.stage,s.character,s.difficulty,s.chapter,s.stage_frame,s.chapter_deaths,i32(app->checkpoint_bytes.size()),i32(c.animation_count())};}return state.data();}

// Exercise actual sprite material with the application's inherited pipeline.
// Deliberately do not enable blending or change alpha comparison here.
extern "C" const u8* application_test_alpha_pixels(){
 if(!app)return nullptr;auto& g=app->graphics;app->renderer.flush();const auto saved=g.backend.state;
 AnmResource file;AnmTexture texture;texture.name="application-alpha-regression";texture.format=texture.pixel_format=1;texture.width=texture.pixel_width=3;texture.height=texture.pixel_height=1;texture.pixels={40,80,120,0,40,80,120,128,40,80,120,255};file.textures.push_back(std::move(texture));file.sprites.push_back({0,0,0,0,3,1,0,0,1,1});
 if(!g.preload(file)||!g.select_target(nullptr,0)||!g.clear_target(0xff203040,nullptr))return nullptr;
 AnmVm vm;vm.resource=&file;if(!vm.select_sprite(0))return nullptr;vm.visual.render_flags|=0x800;vm.visual.flags|=3;
 auto& pipeline=g.pipeline();pipeline.depthTest=false;pipeline.depthWrite=false;pipeline.fog=false;g.set_layout(touhou::graphics::VertexLayout::ScreenColorUv);
 for(u32 i=0;i<3;i++){const float x=10+40*i,u=(float(i)+.5f)/3;const AnmGeometryVertex vertices[]={{{x,10,0},1,0xffffffff,{u,.5f}},{{x+32,10,0},1,0xffffffff,{u,.5f}},{{x,42,0},1,0xffffffff,{u,.5f}},{{x+32,42,0},1,0xffffffff,{u,.5f}}};if(app->renderer.draw_screen_strip(vm,vertices,4)==-2)return nullptr;}
 app->renderer.flush();g.backend.read(sdl::GraphicsDevice::screen);g.unload(file);g.backend.state=saved;app->renderer.invalidate();return g.pixels(sdl::GraphicsDevice::screen)->pixels.data();
}

// Keep asset and lifecycle regression tests independent of private music inputs.
extern "C" EMSCRIPTEN_KEEPALIVE void application_test_music_enabled(int enabled){if(!app)app=std::make_unique<sdl::ApplicationState>();app->audio_device.music_enabled=enabled!=0;}
extern "C" EMSCRIPTEN_KEEPALIVE const float* application_test_lifecycle(){static std::array<float,6> value{};value.fill(-1);if(app){value[4]=float(app->pending_load);value[5]=app->checkpoint_encoder?1:0;if(app->scene()){const auto& s=*app->scene();value[0]=s.battle.player->motion.position.x;value[1]=s.battle.player->motion.position.y;value[2]=float(s.hud.flags);value[3]=float(s.hud.intro_age.current);}}return value.data();}

extern "C" EMSCRIPTEN_KEEPALIVE int application_test_capture(){return app&&app->scene()&&app->scene()->capture(app->progress.chapter);}
