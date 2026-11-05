#include "../game/CallbackNames.hpp"
#include "Common.hpp"
#include <cstdlib>
namespace th10::browser {
void CommonTextRenderer::select_camera(bool world){
    AnmRenderer{engine.manager,graphics}.flush();engine.active=world?&engine.world:&engine.ui;
    GraphicsCamera camera(graphics);engine.active->configure_world(camera);camera.set_viewport(engine.active->viewport);engine.screen_space=world?0:1;
    graphics.viewport=reinterpret_cast<const RenderViewport*>(&engine.active->viewport);
}
void CommonTextRenderer::draw_character(AnmVm& vm,bool pixel){const auto flags=AnmRenderer::axis_geometry(vm,graphics.quad,pixel);AnmRenderer{engine.manager,graphics}.submit(vm,flags);}
Common::Common(AnimationEngine& e):engine(e),update_chain(&e.chain_value){
    current=&value;chain=&update_chain;callbacks=&e.callback_environment;virtual_table=1;update_callback=callback_id::CommonUpdate;draw_callback=callback_id::CommonDraw;early_draw_callback=callback_id::CommonEarlyDraw;
    slots=e.manager.files;effects_name="ascii.anm";text_name="text.anm";capture_name="capture.anm";e.register_receiver(*this);
}
Common::~Common(){if(value){auto* old=value;old->shutdown(*this);delete_object(old);}engine.unregister_receiver(*this);}
bool Common::initialize(){return CommonResources::create(*this)!=nullptr;}
#ifndef TH_NATIVE_PLATFORM
bool Common::invoke(CallbackToken token,void* object,i32& result){
    if(token==update_callback){result=static_cast<CommonResources*>(object)->update();return true;}
    if(token==draw_callback||token==early_draw_callback){CommonTextRenderer renderer(engine);result=static_cast<CommonResources*>(object)->draw(token==early_draw_callback,renderer);return true;}
    return false;
}
#endif
CommonResources* Common::allocate(){return static_cast<CommonResources*>(std::malloc(sizeof(CommonResources)));}
void Common::delete_object(void* p){std::free(p);}
void Common::free_geometry(void* p){std::free(p);}
AnmFile* Common::load_animations(i32 slot,const char* name){return engine.manager.load(slot,name,engine.resources);}
void Common::release_animations(AnmFile& file){file.release(engine.resources);}
void Common::bind_sprite(AnmFile& file,AnmVm& vm,i32 sprite){file.bind_sprite(vm,sprite);}
i32 Common::draw(bool early){CommonTextRenderer renderer(engine);return value->draw(early,renderer);}
}

namespace th10::browser {
void Common::bind_callbacks(Callbacks& b){callback_context=this;
 b.bind(update_callback,this,[](void*,void* o,i32){return static_cast<CommonResources*>(o)->update();});
 for(int early=0;early<2;early++)b.bind(early?early_draw_callback:draw_callback,this,[](void* p,void* o,i32 early){CommonTextRenderer r(static_cast<Common*>(p)->engine);return static_cast<CommonResources*>(o)->draw(early,r);},early);
}
}
