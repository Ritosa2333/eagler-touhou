#include "../../th11_web/cpp/sdl/Application.cpp"
// Diagnostic-only TU. Linked instead of production Application.o; never packaged.
static bool fixture_freeze_ghost=false;
extern "C" {
EMSCRIPTEN_KEEPALIVE int mp_fixture_ui_begin(int stage,int local){
 auto& a=th11::sdl::app;fixture_freeze_ghost=false;
 if(!a.initialize())return 0;
#ifdef TH11_MULTIPLAYER
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
  if(!a.session.update_multiplayer(in)){a.error=a.session.error;return 0;}
  if(!a.capture_pause_frame()||!a.audio_events()||!a.text_events())return 0;
  a.audio.update();a.audio.pump();
  if(!a.session.draw(a.renderer)){a.error=a.session.error;return 0;}a.graphics.present();
#else
  if(!a.tick(p1,pauses!=0))return 0;
#endif
 }
 return 1;
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
 std::snprintf(out,sizeof(out),"{\"phase\":\"%s\",\"frame\":%u,\"hash\":%u,\"pauseState\":%d,\"titleSubstate\":%d,\"lifeHold\":%u,\"powerTaps\":%u,\"p1Lives\":%d,\"p2Lives\":%d,\"p1Power\":%d,\"p2Power\":%d,\"p2Ghost\":%s,\"endingFrames\":%u,\"ghostPinned\":%s}",s.phase_name(),s.state.frame,s.multiplayer_hash(),s.pause_menu?s.pause_menu->state:-1,s.title?s.title->substate:-1,p?p->life_hold:0,p?p->power_taps:0,p?p->economy.lives:0,q?q->economy.lives:0,p?p->economy.power:0,q?q->economy.power:0,q&&q->ghost?"true":"false",s.ending?s.ending->frames:0,fixture_freeze_ghost?"true":"false");
#else
 std::snprintf(out,sizeof(out),"{\"phase\":\"%s\",\"frame\":%u,\"pauseState\":%d,\"titleSubstate\":%d}",s.phase_name(),s.state.frame,s.pause_menu?s.pause_menu->state:-1,s.title?s.title->substate:-1);
#endif
 return out;
}
}
