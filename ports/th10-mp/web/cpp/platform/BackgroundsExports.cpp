#include "Backgrounds.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define BACKGROUND_EXPORT(name) extern "C" __attribute__((export_name(name)))
BACKGROUND_EXPORT("backgrounds_create") browser::Backgrounds* backgrounds_create(browser::GameState* state,browser::AnimationEngine* engine,browser::ScreenEffects* effects,browser::FileSystem* files){auto* bytes=std::malloc(sizeof(browser::Backgrounds));return bytes?new(bytes)browser::Backgrounds(*state,*engine,*effects,*files):nullptr;}
BACKGROUND_EXPORT("backgrounds_destroy") void backgrounds_destroy(browser::Backgrounds* backgrounds){if(backgrounds){backgrounds->~Backgrounds();std::free(backgrounds);}}
BACKGROUND_EXPORT("backgrounds_load") Stage* backgrounds_load(browser::Backgrounds* backgrounds,const char* name,i32 offset){return backgrounds->create(name,offset);}
BACKGROUND_EXPORT("backgrounds_release") void backgrounds_release(browser::Backgrounds* backgrounds,Stage* stage){backgrounds->destroy(stage);}
BACKGROUND_EXPORT("backgrounds_activate") void backgrounds_activate(Stage* stage){stage->update_entry->flags|=2;stage->draw_entry->flags|=2;stage->foreground_entry->flags|=2;}
BACKGROUND_EXPORT("backgrounds_restart") void backgrounds_restart(browser::Backgrounds* backgrounds,Stage* stage){stage->restart(backgrounds->script);}
BACKGROUND_EXPORT("backgrounds_effect") void backgrounds_effect(browser::Backgrounds* backgrounds,Stage* stage,u32 operation){if(operation==0)stage->enable_effects();else if(operation==1)stage->disable_effects(backgrounds->engine.manager.registry);else if(operation==2)stage->fade_in(backgrounds->rate);else if(operation==3)stage->fade_to_black(backgrounds->effects);}
