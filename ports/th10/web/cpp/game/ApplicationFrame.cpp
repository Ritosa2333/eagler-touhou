#include "ApplicationFrame.hpp"
namespace th10 {
// 0x41ff80. Resource work and input precede the screen-transition gate.
i32 ApplicationFrame::update(){
    auto& app=application;auto& env=environment;
    if((app.engine_flags&0x80)&&!app.resource_loader.running)*env.pending_screen=3;
    env.update_audio();env.update_input();if(env.process_loading())return 4;
    if(app.frame_gate)return app.frame_gate==2?4:1;
    return env.transition(app);
}
// 0x420000. Deliberately retain cached slots 5/7 and the material-color cache.
i32 ApplicationFrame::begin_draw(){
    auto& app=application;auto& env=environment;auto& manager=**env.animations;
    manager.current_uv_sprite=nullptr;manager.current_texture=nullptr;
    manager.cached_draw_state[1]=255;manager.cached_draw_state[0]=3;
    manager.cached_draw_state[3]=manager.cached_draw_state[4]=255;
    manager.tint_enabled=0;manager.tint=0x80808080;manager.cached_draw_state[6]=255;
    manager.draw_offset={0,0};manager.cached_draw_state[2]=255;
    app.active_camera=&app.ui_camera;env.configure_camera(*app.active_camera);
    env.set_viewport(app.device,app.active_camera->viewport);app.screen_space=1;
    env.clear(*env.background_color);return 1;
}
i32 ApplicationFrame::finish_draw(){environment.flush();environment.world_camera->draw_offset={0,0};return 1;}
}
