#include "Application.hpp"
#if TH075_MULTIPLAYER
#include "Multiplayer.hpp"
#include "../multiplayer/InputLatch.hpp"
#include <eagler/netplay/FrameBudget.hpp>
#endif
#include "../../../portable/sdl/FrameCadence.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <algorithm>
#include <cmath>
namespace th075::sdl {
std::unique_ptr<Application> app;
struct Key{const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
std::array<ControlCounts,2> controlCounts;std::array<i32,2> cancelCounts{};
touhou::sdl::FrameCadence cadence;bool suspended=false;double previous=-1;u8 touchButtons=0;float stickX=0,stickY=0;bool touchEnabled=true,touchEscape=false;SDL_Gamepad* joystick=nullptr;bool musicEnabled=true;
#if TH075_MULTIPLAYER
th075::multiplayer::InputLatch mpInputLatch;
#endif
void clear_inputs(){
#if TH075_MULTIPLAYER
mpInputLatch.Clear();
#endif
for(auto& k:keyboard_map)k.hosted=false;for(auto& c:controlCounts)c.clear();cancelCounts.fill(0);SDL_ResetKeyboard();touchButtons=0;stickX=stickY=0;touchEscape=false;}
u8 stick_held(float x,float y){if(x*x+y*y<.04f)return 0;const double angle=std::atan2(double(y),double(x));const int sector=int(std::floor((angle+3.141592653589793/8)/(3.141592653589793/4)))&7;constexpr u8 dirs[8]{2,10,8,9,1,5,4,6};return dirs[sector];}
BattleSystemInput sample(){BattleSystemInput input;SDL_Event e;while(SDL_PollEvent(&e)){if(e.type==SDL_EVENT_WINDOW_FOCUS_LOST)clear_inputs();if(e.type==SDL_EVENT_GAMEPAD_ADDED&&!joystick)joystick=SDL_OpenGamepad(e.jdevice.which);if(e.type==SDL_EVENT_GAMEPAD_REMOVED&&joystick&&SDL_GetGamepadID(joystick)==e.jdevice.which){SDL_CloseGamepad(joystick);joystick=nullptr;}}const auto* keys=SDL_GetKeyboardState(nullptr);for(const auto& k:keyboard_map)if(k.scan<256&&(k.hosted||(k.native!=SDL_SCANCODE_UNKNOWN&&keys[k.native]&&!EM_ASM_INT({return !!Module.resetBrowserKeyboard;}))))input.keys[k.scan]=128;
#if TH075_MULTIPLAYER
 if(multiplayer::active()){for(const auto& key:keyboard_map)if(mpInputLatch.TakeKey(key.scan)){for(unsigned button=4;button<8;++button)if(app->settings.mappings[0].bindings[button]==key.scan)input.keys[key.scan]=128;}}
 const u8 capturedTouch=multiplayer::active()?mpInputLatch.TakeTouch():0;
#else
 const u8 capturedTouch=0;
#endif
 for(u8 side=0;side<2;side++){u8 held=0;const auto& mapping=app->settings.mappings[side];for(u8 i=0;i<8;i++)if(mapping.bindings[i]<256&&input.keys[mapping.bindings[i]])held|=u8(1u<<i);const u8 active=app->selection.activePlayer<2?app->selection.activePlayer:0;if(touchEnabled&&side==active)held|=touchButtons|capturedTouch|stick_held(stickX,stickY);if(joystick&&side==active){float x=SDL_GetGamepadAxis(joystick,SDL_GAMEPAD_AXIS_LEFTX)/32767.f,y=SDL_GetGamepadAxis(joystick,SDL_GAMEPAD_AXIS_LEFTY)/32767.f;held|=stick_held(x,y);if(SDL_GetGamepadButton(joystick,SDL_GAMEPAD_BUTTON_DPAD_LEFT))held|=1;if(SDL_GetGamepadButton(joystick,SDL_GAMEPAD_BUTTON_DPAD_RIGHT))held|=2;if(SDL_GetGamepadButton(joystick,SDL_GAMEPAD_BUTTON_DPAD_UP))held|=4;if(SDL_GetGamepadButton(joystick,SDL_GAMEPAD_BUTTON_DPAD_DOWN))held|=8;constexpr SDL_GamepadButton face[]{SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH};for(u8 b=0;b<4;b++)if(SDL_GetGamepadButton(joystick,face[b]))held|=u8(16<<b);if(SDL_GetGamepadButton(joystick,SDL_GAMEPAD_BUTTON_START))input.keys[1]=128;}controlCounts[side].from_held(held);input.players[side]=controlCounts[side];const auto cancel=mapping.bindings[8]<256&&input.keys[mapping.bindings[8]];cancelCounts[side]=cancel?increment_wrapped(cancelCounts[side],1):0;input.cancel[side]=cancelCounts[side];}if(touchEnabled&&touchEscape)input.keys[1]=128;return input;}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th075_initialize(unsigned scale){using namespace th075::sdl;if(!app)app=std::make_unique<Application>();app->audio.musicEnabled=musicEnabled;if(!app->initialize(scale))return 0;for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);SDL_InitSubSystem(SDL_INIT_GAMEPAD);int count=0;auto* ids=SDL_GetGamepads(&count);if(count)joystick=SDL_OpenGamepad(ids[0]);SDL_free(ids);return 1;}
EMSCRIPTEN_KEEPALIVE const char* th075_error(){return th075::sdl::app?th075::sdl::app->error.c_str():"Application not initialized";}
EMSCRIPTEN_KEEPALIVE unsigned th075_frame(){return th075::sdl::app?th075::sdl::app->frame:0;}
EMSCRIPTEN_KEEPALIVE unsigned th075_phase(){return th075::sdl::app?th075::sdl::app->phase():0;}
EMSCRIPTEN_KEEPALIVE int th075_room_preview(unsigned on){using namespace th075::sdl;clear_inputs();return app&&app->preview_room(on!=0);}
EMSCRIPTEN_KEEPALIVE unsigned th075_scene(){return th075::sdl::app?th075::sdl::app->current_scene():255;}
EMSCRIPTEN_KEEPALIVE int th075_tick(double timestamp){using namespace th075::sdl;if(!app||suspended)return 1;const double delta=previous<0?1./60.:(timestamp-previous)/1000.;previous=timestamp;double elapsed=delta;
#if TH075_MULTIPLAYER
 if(multiplayer::active()){if(!multiplayer::pump(emscripten_get_now()*1000))return 0;elapsed=multiplayer::pace(delta*1000)/1000;}
#endif
 app->graphics.renderer.defer=true;bool ok=true;
#if TH075_MULTIPLAYER
 if(multiplayer::active()){
  cadence.accumulate(elapsed);const auto ticks=cadence.due();const double started=emscripten_get_now();
  for(unsigned n=0;n<ticks&&ok;++n){
   if(!Netplay::FrameBudget::CanStartTick(n,std::uint64_t((emscripten_get_now()-started)*1'000'000)))break;
   bool advanced=false;ok=multiplayer::step({},emscripten_get_now()*1000,&advanced);
   if(!advanced)break;cadence.consume();
  }
 }else
#endif
 {const auto ticks=cadence.advance(elapsed);for(unsigned n=0;n<ticks&&ok;++n){auto input=sample();if(app->room_preview)app->preview_title_tick(input);ok=app->tick(input,unsigned(timestamp));}}
 app->graphics.renderer.commit();app->graphics.renderer.defer=false;app->audio.pump();return ok?1:0;}
EMSCRIPTEN_KEEPALIVE void th075_key(unsigned scan,unsigned down){
#if TH075_MULTIPLAYER
if(th075::sdl::multiplayer::active())th075::sdl::mpInputLatch.Key(scan,down!=0);
#endif
for(auto& k:th075::sdl::keyboard_map)if(k.scan==scan)k.hosted=down!=0;}
EMSCRIPTEN_KEEPALIVE void th075_keys_clear(){th075::sdl::clear_inputs();}
EMSCRIPTEN_KEEPALIVE void th075_loop_pause(unsigned on){using namespace th075::sdl;suspended=on!=0;previous=-1;cadence.reset();clear_inputs();if(app)app->audio.suspend(suspended);}
EMSCRIPTEN_KEEPALIVE void th075_touch_controls(unsigned enabled,unsigned a,unsigned b,unsigned c,unsigned d,unsigned pause){using namespace th075::sdl;touchEnabled=enabled!=0;touchButtons=unsigned(a!=0)*16|unsigned(b!=0)*32|unsigned(c!=0)*64|unsigned(d!=0)*128;touchEscape=pause!=0;
#if TH075_MULTIPLAYER
 if(multiplayer::active()&&touchEnabled)mpInputLatch.Touch(touchButtons);
#endif
}
EMSCRIPTEN_KEEPALIVE void th075_touch_stick(float x,float y){using namespace th075::sdl;stickX=std::isfinite(x)?std::clamp(x/32767.f,-1.f,1.f):0;stickY=std::isfinite(y)?std::clamp(y/32767.f,-1.f,1.f):0;}
EMSCRIPTEN_KEEPALIVE int th075_validate_file(unsigned kind,const unsigned char* data,unsigned size){if(kind==0){th075::GameProfile p;return p.load(data,size);}if(kind==1){th075::ReplayFile r;return r.load(data,size)&&!r.rounds.empty();}if(kind==2){th075::GameSettings s;return s.load(data,size);}return 0;}
EMSCRIPTEN_KEEPALIVE int th075_save_scores(){using namespace th075::sdl;return app&&app->save_scores();}
EMSCRIPTEN_KEEPALIVE int th075_save_replay(unsigned slot,const char* name){using namespace th075::sdl;return app&&app->save_replay(slot,name);}
EMSCRIPTEN_KEEPALIVE int th075_return_title(){using namespace th075::sdl;return app&&app->return_title();}
EMSCRIPTEN_KEEPALIVE int th075_pause(){using namespace th075::sdl;if(!app||!app->battle)return 0;app->battle->progress.paused=true;return 1;}
EMSCRIPTEN_KEEPALIVE int th075_resume(){using namespace th075::sdl;if(!app||!app->battle)return 0;app->battle->progress.paused=false;return 1;}
EMSCRIPTEN_KEEPALIVE int th075_restart(){using namespace th075::sdl;return app&&app->restart();}
EMSCRIPTEN_KEEPALIVE void th075_music_enabled(unsigned on){using namespace th075::sdl;musicEnabled=on!=0;if(app)app->audio.musicEnabled=musicEnabled;}
EMSCRIPTEN_KEEPALIVE void th075_touch_cancel(){using namespace th075::sdl;touchButtons=0;stickX=stickY=0;touchEscape=false;
#if TH075_MULTIPLAYER
mpInputLatch.Clear();
#endif
}
EMSCRIPTEN_KEEPALIVE const unsigned* th075_audio_statistics(){using namespace th075::sdl;return app?app->audio.statistics():nullptr;}
EMSCRIPTEN_KEEPALIVE void th075_audio_close(){using namespace th075::sdl;if(app)app->audio.close();}
#if TH075_DEVELOPMENT_HARNESS
EMSCRIPTEN_KEEPALIVE unsigned th075_probe_physical_held(){return th075::sdl::sample().players[0].held();}
// Test-only source/control and decode checks for every compiled replacement.
EMSCRIPTEN_KEEPALIVE int th075_probe_language_resource(const char* path,unsigned edition,unsigned kind){
 using namespace th075;using namespace th075::sdl;if(!app||!path||edition>1||kind>2)return -1;
 std::vector<u8> translated,original;FileResources fresh;
 if(!app->resources.read(ResourceEdition(edition),path,translated))return -2;
 EM_ASM({Module.probePreviousLanguage=Module.th075Language;Module.th075Language=null;});
 const bool ok=fresh.read(ResourceEdition(edition),path,original);
 EM_ASM({Module.th075Language=Module.probePreviousLanguage;delete Module.probePreviousLanguage;});
 if(!ok)return -3;
 if(kind==0){TexturePack a,b;if(!a.load(original.data(),original.size())||!b.load(translated.data(),translated.size())||a.images.size()!=b.images.size())return -4;std::vector<u8> pixels;for(u32 i=0;i<a.images.size();i++)if(a.images[i].width!=b.images[i].width||a.images[i].height!=b.images[i].height||!b.images[i].decode(pixels))return -5;return int(b.images.size());}
 if(kind==2){MusicCatalog a,b;if(!a.load(original.data(),original.size())||!b.load(translated.data(),translated.size())||a.tracks.size()!=b.tracks.size())return -6;for(u32 i=0;i<a.tracks.size();i++)if(a.tracks[i].id!=b.tracks[i].id||a.tracks[i].author!=b.tracks[i].author)return -7;return int(b.tracks.size());}
 FighterScript a,b;if(!a.load(original.data(),original.size())||!b.load(translated.data(),translated.size())||a.labels!=b.labels||a.blocks.size()!=b.blocks.size())return -8;unsigned changed=0;
 for(u32 i=0;i<a.blocks.size();i++){if(a.blocks[i].size()!=b.blocks[i].size())return -9;for(u32 j=0;j<a.blocks[i].size();j++){const auto& x=a.blocks[i][j];const auto& y=b.blocks[i][j];if(x.kind!=y.kind||x.argument!=y.argument)return -10;if(x.text!=y.text){changed++;TextRasterOptions o;o.width=1024;o.height=1024;o.lineLimit=40;const auto texture=app->fonts.create(y.text,o);if(!texture)return -11;app->graphics.destroy(texture);}}}
 return int(changed);
}
EMSCRIPTEN_KEEPALIVE const float* th075_probe_camera(){using namespace th075::sdl;static float v[8]{};if(app){const auto& loaded=app->loading.story.camera;v[0]=loaded.fixed;v[1]=loaded.fixedY;if(app->battle){const auto& c=app->battle->simulation.story.camera;v[2]=c.fixed;v[3]=c.x;v[4]=c.y;v[5]=c.span;v[6]=c.fixedY;v[7]=c.smoothY;}}return v;}
// Verify the real shared renderer, including texture alpha times vertex alpha.
EMSCRIPTEN_KEEPALIVE unsigned th075_probe_alpha(unsigned color){using namespace th075;using namespace th075::sdl;if(!app)return 0;auto& d=app->graphics;auto& g=app->zun;const auto image=d.create_surface(1,1);const u32 white=0xffffffff;d.update_pixels(image,&white,4);d.clear(0xffff0000);g.blend(SpriteBlend::Alpha);g.depth(DepthMode::Disabled);g.textured_quad(rectangle_quad(0,0,64,64),{0,0,1,1},color,image);const auto* pixels=d.capture_pixels();u32 result=0;std::memcpy(&result,pixels+(32*640+32)*4,4);d.destroy(image);return result;}
EMSCRIPTEN_KEEPALIVE const unsigned char* th075_probe_pixels(){using namespace th075::sdl;return app?app->graphics.capture_pixels():nullptr;}
EMSCRIPTEN_KEEPALIVE const unsigned* th075_probe_replay_cursors(){using namespace th075::sdl;static unsigned state[6];if(app&&app->battle){const auto& r=app->battle->simulation.replay;state[0]=r.inputCursor;state[1]=r.randomCursor;state[2]=r.auxiliaryCursor;state[3]=r.eventCursor;state[4]=r.exhausted;state[5]=app->battle->session.replayExhausted;}return state;}

EMSCRIPTEN_KEEPALIVE const int* th075_probe_state(){using namespace th075::sdl;static std::array<int,48> v;v.fill(0);if(!app)return v.data();v[0]=app->current_scene();v[1]=app->selection.storyStage;if(auto* b=app->battle.get()){v[2]=int(b->simulation.rules.phase);v[3]=b->simulation.rules.countdown;v[4]=b->session.replayMode;v[5]=b->session.roundIndex;v[6]=b->session.replayRound;v[7]=b->simulation.rules.remainingLives;v[8]=int(b->simulation.replay.random.state);v[9]=b->timing.replayFrame;for(int s=0;s<2;s++){auto& a=b->simulation.characters[s]->actor;const auto o=10+s*16;v[o]=int(a.motion.x*1000);v[o+1]=int(a.motion.y*1000);v[o+2]=a.combat.health;v[o+3]=a.animation.cursor.action;v[o+4]=a.combat.spellPhase;v[o+5]=a.score;v[o+6]=int(b->simulation.objects[s].size(0)+b->simulation.objects[s].size(1)+b->simulation.objects[s].size(2)+b->simulation.objects[s].size(3));v[o+7]=a.confirmCount;v[o+8]=a.controls.current.held();v[o+9]=a.dialogue.label;v[o+10]=a.dialogue.record;v[o+11]=b->simulation.characters[s]->story_sequence().step;v[o+12]=int(a.animation.frame!=nullptr);v[o+13]=a.damageDisabled;v[o+14]=a.transitionReady;v[o+15]=a.combat.freeze;}v[42]=b->progress.paused;v[43]=b->progress.pauseFade;v[44]=b->progress.pauseChoice;}v[45]=int(app->graphics.pipeline().minFilter);v[46]=int(app->graphics.pipeline().magFilter);v[47]=int(app->graphics.render_scale());return v.data();}
EMSCRIPTEN_KEEPALIVE int th075_probe_knockout(unsigned side,unsigned phases){using namespace th075::sdl;if(!app||!app->battle||side>1)return 0;auto& b=*app->battle;b.simulation.characters[side]->actor.combat.health=0;b.simulation.characters[side]->actor.combat.spellPhase=th075::u8(std::clamp(phases,1u,12u));b.simulation.rules.detect_knockout(b.simulation.characters[0]->actor,b.simulation.characters[1]->actor,b.session.mode,b.session.replayMode);auto& a=b.simulation.characters[side]->actor;a.select_action(phases>1?97:99);if(phases>1){a.motion.y=40;a.motion.velocityX=0;a.motion.velocityY=-2;}return 1;}
EMSCRIPTEN_KEEPALIVE int th075_probe_auto(unsigned on){using namespace th075::sdl;if(!app||!app->battle)return 0;app->battle->controls.recordingFastForward=on!=0;return 1;}
EMSCRIPTEN_KEEPALIVE const float* th075_probe_audio(){using namespace th075::sdl;static float pcm[2048];return app&&app->audio.mix(pcm,1024)?pcm:nullptr;}
EMSCRIPTEN_KEEPALIVE int th075_probe_sound(unsigned bank,unsigned id){using namespace th075::sdl;if(!app)return 0;if(!bank)app->audio.sound(id);else app->audio.character_sound(bank-1,id);return 1;}

EMSCRIPTEN_KEEPALIVE int th075_probe_scene(unsigned scene){using namespace th075::sdl;return app&&app->probe_scene(scene);}
EMSCRIPTEN_KEEPALIVE int th075_probe_battle(unsigned mode,unsigned first,unsigned other,unsigned background){using namespace th075::sdl;return app&&app->probe_battle(mode,first,other,background);}
#endif
}
