#include "../game/CallbackNames.hpp"
#include "../game/HighRefresh.hpp"
#include "ScreenEffects.hpp"
#include <algorithm>
namespace th10::browser {
ScreenEffects::ScreenEffects(AnimationEngine& e,const u32& quit,const u32* controller):engine(e),update_chain(&e.chain_value){
    rate=&e.speed;quitting=&quit;controller_flags=controller;random=&e.script_random;camera_offset=&e.world.draw_offset;chain=&update_chain;callbacks=&e.callback_environment;
    const CallbackToken updates[]={callback_id::ScreenFadeUpdate,callback_id::ScreenShakeUpdate,callback_id::ScreenFlashUpdate,callback_id::ScreenFadeUpdate,callback_id::ScreenCircleUpdate,callback_id::ScreenFlashUpdate,callback_id::ScreenArcadeUpdate,callback_id::ScreenArcadeUpdate,callback_id::ScreenViewShakeUpdate},draws[]={callback_id::ScreenFadeDraw,0,callback_id::ScreenFlashDraw,callback_id::ScreenFlashDraw,callback_id::ScreenCircleDraw,callback_id::ScreenFadeDraw,callback_id::ScreenArcadeFadeDraw,callback_id::ScreenArcadeFlashDraw,0};
    std::memcpy(update_callbacks,updates,sizeof(updates));std::memcpy(draw_callbacks,draws,sizeof(draws));delete_callback=callback_id::ScreenEffectDelete;engine.register_receiver(*this);
}
ScreenEffects::~ScreenEffects(){while(memory.first){auto* effect=static_cast<ScreenEffect*>(memory.first->bytes);effect->release(*this);destroy(effect);}engine.unregister_receiver(*this);}
#ifndef TH_NATIVE_PLATFORM
bool ScreenEffects::invoke(CallbackToken token,void* object,i32& result){
    auto* effect=static_cast<ScreenEffect*>(object);
    if(token==delete_callback){effect->release(*this);destroy(effect);result=0;return true;}
    for(u32 i=0;i<9;++i){if(token==update_callbacks[i]){result=update_effect(*effect);return true;}if(draw_callbacks[i]&&token==draw_callbacks[i]){result=effect->draw(*this);return true;}}
    return false;
}
#endif
ScreenEffect* ScreenEffects::allocate(){return reinterpret_cast<ScreenEffect*>(memory.allocate(sizeof(ScreenEffect)));}
void ScreenEffects::destroy(ScreenEffect* effect){previous_alpha.erase(effect);memory.release(effect);}
i32 ScreenEffects::update_effect(ScreenEffect& effect){previous_alpha[&effect]=effect.alpha;return effect.update(*this);}
i32 ScreenEffects::presentation_alpha(const ScreenEffect& effect){
    if(!high_refresh::render_only||!high_refresh::active)return effect.alpha;
    const auto found=previous_alpha.find(&effect);if(found==previous_alpha.end())return effect.alpha;
    return static_cast<i32>(std::clamp(high_refresh::lerp(float(found->second),float(effect.alpha)),0.0f,255.0f));
}
void ScreenEffects::fullscreen_viewport(){engine.flush();auto viewport=engine.world.viewport;viewport.x=viewport.y=0;viewport.width=640;viewport.height=480;engine.device.viewport(viewport);}
void ScreenEffects::rectangle(const ScreenRect& bounds,u32 color){auto renderer=engine.renderer();auto* manager=&engine.manager;const u32 colors[]={color,color,color,color};draw_screen_rectangle(bounds,colors,&manager,renderer);}
}

namespace th10::browser {
void ScreenEffects::bind_callbacks(Callbacks& b){callback_context=this;
 for(u32 i=0;i<9;i++){if(update_callbacks[i])b.bind(update_callbacks[i],this,[](void* p,void* o,i32){return static_cast<ScreenEffects*>(p)->update_effect(*static_cast<ScreenEffect*>(o));});if(draw_callbacks[i])b.bind(draw_callbacks[i],this,[](void* p,void* o,i32){return static_cast<ScreenEffect*>(o)->draw(*static_cast<ScreenEffects*>(p));});}
 b.bind(delete_callback,this,[](void* p,void* o,i32){auto& s=*static_cast<ScreenEffects*>(p);auto* e=static_cast<ScreenEffect*>(o);e->release(s);s.destroy(e);return 0;});
}
}
