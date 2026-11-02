#pragma once
#include "../../cpp/game/ScreenShake.hpp"
struct ScreenShakeFixture {Rng random;ScreenShakeContext context;std::unique_ptr<ScreenShake> shake;std::unique_ptr<ScreenNudge> nudge;};
API(screen_shake_create) ScreenShakeFixture* screen_shake_create(i32 kind,i32 a,i32 b,i32 c,i32 d){auto* p=new ScreenShakeFixture;if(kind==8)p->shake=std::make_unique<ScreenShake>(p->random,ScreenShakeSpec{a,b,c,d});else p->nudge=std::make_unique<ScreenNudge>(p->random,ScreenNudgeSpec{a,b,c});return p;}
API(screen_shake_delete) void screen_shake_delete(ScreenShakeFixture* p){delete p;}
API(screen_shake_step) i32 screen_shake_step(ScreenShakeFixture* p,float rate,float scale,u32 flags,u32 available,u32 shutdown){p->context={shutdown!=0,available!=0,flags,rate,scale};return p->shake?p->shake->update(p->context):p->nudge->update(p->context);}
API(screen_shake_field) void* screen_shake_field(ScreenShakeFixture* p,u32 kind){if(kind==0)return &p->random;if(kind==1)return p->shake?&p->shake->age:&p->nudge->age;if(kind==2)return p->shake?&p->shake->world_offset:&p->nudge->world_offset;return p->shake?&p->shake->screen_offset:&p->nudge->screen_offset;}

#include "../../cpp/game/ScreenEffects.hpp"
struct ScreenEffectsFixture {FrameScheduler scheduler;Rng game,visual;ScreenEffects effects{scheduler,game,visual};};
API(screen_effects_create) ScreenEffectsFixture* screen_effects_create(){return new ScreenEffectsFixture;}
API(screen_effects_delete) void screen_effects_delete(ScreenEffectsFixture* p){delete p;}
API(screen_effects_request) u32 screen_effects_request(ScreenEffectsFixture* p,i32 kind,i32 a,i32 b,i32 c,i32 d){return kind==8?p->effects.shake({a,b,c,d}):p->effects.nudge({a,b,c});}
API(screen_effects_step) i32 screen_effects_step(ScreenEffectsFixture* p,float rate,float scale,u32 flags,u32 available,u32 shutdown){p->effects.collect();p->effects.context={shutdown!=0,available!=0,flags,rate,scale};return p->scheduler.update();}
API(screen_effects_field) const void* screen_effects_field(ScreenEffectsFixture* p,u32 field){switch(field){case 0:return &p->game;case 1:return &p->visual;case 2:return &p->effects.world_offset;default:return &p->effects.screen_offset;}}
API(screen_effects_finish_draw) void screen_effects_finish_draw(ScreenEffectsFixture* p){p->effects.reset_offsets();}
API(screen_effects_count) u32 screen_effects_count(ScreenEffectsFixture* p){return p->effects.count();}
API(screen_effects_clear) void screen_effects_clear(ScreenEffectsFixture* p){p->effects.clear();}
