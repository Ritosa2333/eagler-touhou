#include "TitleInformation.hpp"
namespace th08 {
void TitleInformation::start(const char* path){frame=0;active=true;actions.load_image(path);}
bool TitleInformation::step(){
    if(!active)return false;
    renderer.current_shader=renderer.current_color_op=renderer.disable_z_write=renderer.camera_mode=0xff;renderer.current_blend=3;renderer.current_sprite=nullptr;renderer.current_texture=0;renderer.state_changes=renderer.flushes=0;renderer.mix_enabled=false;renderer.mix_color=0x80808080;
    actions.begin_frame();actions.background();
    // The original intentionally narrows the arithmetic to an 8-bit alpha,
    // including its unusual 60-frame numerator in a 20-frame fade-in.
    if(frame<20||frame>5940){const u8 alpha=u8(((frame<20?60-frame:frame-5980)*255)/20);const u32 colors[4]={u32(alpha)<<24,u32(alpha)<<24,u32(alpha)<<24,u32(alpha)<<24};renderer.draw_rectangle(0,0,639,479,colors);}
    input.current=actions.poll_input();if(!actions.present())actions.reset_device();
    const u16 mask=4099;if(frame>=180&&frame<5980&&(input.current&mask)&&(input.current&mask)!=(input.previous&mask)){frame=5980;actions.confirm_sound();}
    ++frame;actions.process_sounds();if(frame>=6000){actions.release_image();active=false;}return active;
}
}
