#include "AnmLayers.hpp"
namespace th10 {
// Original registered layer callbacks switch cameras at 3, 15, 16 and 19.
// The HUD boundary at 14 clears the world shake without rebuilding matrices.
i32 AnmLayers::draw(AnmManager& manager,u32 layer){
    if(layer==3){
        active=&world;world.configure_flat(platform);platform.set_viewport(active->viewport);screen_space=0;
        if(fog_enabled){platform.flush();fog_enabled=0;RenderCommands(*platform.render_environment).SetFogEnabled(false);}
    }else if(layer==14){world.draw_offset={0,0};(*platform.animation_manager)->draw_offset={0,0};}
    else if(layer==15||layer==16||layer==19){active=&ui;ui.configure_flat(platform);platform.set_viewport(active->viewport);screen_space=1;}
    return manager.draw_layer(layer,frame);
}
}
