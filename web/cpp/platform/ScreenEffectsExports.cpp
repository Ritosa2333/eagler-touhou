#include "ScreenEffects.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define EFFECT_EXPORT(name) extern "C" __attribute__((export_name(name)))
EFFECT_EXPORT("effects_create") browser::ScreenEffects* effects_create(browser::AnimationEngine* engine,const u32* quitting,const u32* controller){auto* bytes=std::malloc(sizeof(browser::ScreenEffects));return bytes?new(bytes)browser::ScreenEffects(*engine,*quitting,controller):nullptr;}
EFFECT_EXPORT("effects_destroy") void effects_destroy(browser::ScreenEffects* effects){if(effects){effects->~ScreenEffects();std::free(effects);}}
EFFECT_EXPORT("effects_start") ScreenEffect* effects_start(browser::ScreenEffects* effects,u32 kind,i32 duration,i32 a,i32 b,i32 c,i32 layer){return kind<9?ScreenEffect::create(static_cast<ScreenEffectKind>(kind),duration,a,b,c,layer,*effects):nullptr;}
EFFECT_EXPORT("effects_fade_out") void effects_fade_out(browser::ScreenEffects* effects,ScreenEffect* effect){effect->fade_out(effects->rate);}
EFFECT_EXPORT("effects_count") u32 effects_count(browser::ScreenEffects* effects){return effects->memory.count;}
EFFECT_EXPORT("effects_remove") void effects_remove(browser::ScreenEffects* effects,ScreenEffect* effect){if(effect){effect->release(*effects);effects->destroy(effect);}}
