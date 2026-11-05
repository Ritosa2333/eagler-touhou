#include "AnimationEngine.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define ANIMATION_EXPORT(name) extern "C" __attribute__((export_name(name)))
ANIMATION_EXPORT("animation_engine_create") browser::AnimationEngine* animation_engine_create(browser::FileSystem* files,browser::GraphicsDevice* device,Rng* script,Rng* visual,float* rate){auto* bytes=std::malloc(sizeof(browser::AnimationEngine));return bytes?new(bytes)browser::AnimationEngine(*files,*device,*script,*visual,*rate):nullptr;}
ANIMATION_EXPORT("animation_engine_destroy") void animation_engine_destroy(browser::AnimationEngine* engine){if(engine){engine->~AnimationEngine();std::free(engine);}}
ANIMATION_EXPORT("animation_engine_manager") AnmManager* animation_engine_manager(browser::AnimationEngine* engine){return &engine->manager;}
ANIMATION_EXPORT("animation_engine_camera") Camera* animation_engine_camera(browser::AnimationEngine* engine,u32 ui){return ui?&engine->ui:&engine->world;}
ANIMATION_EXPORT("animation_engine_load") AnmFile* animation_engine_load(browser::AnimationEngine* engine,i32 slot,const char* name){return engine->manager.load(slot,name,engine->resources);}
ANIMATION_EXPORT("animation_engine_unload") void animation_engine_unload(browser::AnimationEngine* engine,i32 slot){engine->manager.unload(slot,engine->resources);}
ANIMATION_EXPORT("animation_engine_spawn") u32 animation_engine_spawn(browser::AnimationEngine* engine,AnmFile* file,i32 script,u32 tag,u32 placement){if(!file||script<0||script>=file->script_count||tag>=19||placement>3)return 0;return engine->manager.create(*file,script,tag,static_cast<AnimationPlacement>(placement),*engine,*engine);}
ANIMATION_EXPORT("animation_engine_find") AnmVm* animation_engine_find(browser::AnimationEngine* engine,u32 id){return engine->manager.registry.find(id);}
ANIMATION_EXPORT("animation_engine_update") i32 animation_engine_update(browser::AnimationEngine* engine){return engine->update_all();}
ANIMATION_EXPORT("animation_engine_draw") i32 animation_engine_draw(browser::AnimationEngine* engine){engine->begin_frame();const i32 result=engine->draw_all();engine->flush();return result;}
ANIMATION_EXPORT("animation_engine_interrupt") void animation_engine_interrupt(browser::AnimationEngine* engine,u32 id,i32 label){engine->manager.registry.interrupt(id,label);}
ANIMATION_EXPORT("animation_engine_remove_all") void animation_engine_remove_all(browser::AnimationEngine* engine){engine->manager.release(*engine);}
