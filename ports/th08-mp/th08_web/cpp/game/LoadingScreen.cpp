#include "LoadingScreen.hpp"
#include "Presentation.hpp"
namespace th08 {
bool LoadingScreen::show(const Vec3& position,bool capture){
    if(!phase){for(i32 i=0;i<3;i++)if(!library.start(2,i,vms[i],animations))return false;phase=1;for(auto& vm:vms)vm.pos=position;}
    if(capture)actions.capture_screen();return true;
}
void LoadingScreen::fade(){if(phase==1){for(auto& vm:vms)vm.pendingInterrupt=1;phase=0;}}
void LoadingScreen::hide(){if(phase==1){for(auto& vm:vms)vm.pendingInterrupt=2;phase=2;}}
void LoadingScreen::update(){for(auto& vm:vms)animations.execute(vm);}
void LoadingScreen::draw(){
    for(auto& source:vms){AnmVm copy;if(presentation::render_only)copy=source;auto& vm=presentation::render_only?copy:source;vm.pos={Scalar::add(vm.pos.x,vm.pos2.x),Scalar::add(vm.pos.y,vm.pos2.y),Scalar::add(vm.pos.z,vm.pos2.z)};renderer.draw_2d(vm);if(!presentation::render_only)vm.pos={Scalar::sub(vm.pos.x,vm.pos2.x),Scalar::sub(vm.pos.y,vm.pos2.y),Scalar::sub(vm.pos.z,vm.pos2.z)};}
}
void LoadingScreen::background(){
    const u32 saved_color=ascii.state.color;const float saved_scale_x=ascii.state.scale_x,saved_scale_y=ascii.state.scale_y;
    if(phase>=2){if(!presentation::render_only)phase=wrapping_add(phase,1);if(phase>=5){ascii.state.scale_x=ascii.state.scale_y=.5f;const i32 alpha=255-(phase<35?(phase-5)*128/30:(65-phase)*128/30);ascii.state.color=(ascii.state.color&0xffffff)|(u32(u8(alpha))<<24);ascii.add_string({288,454,0},"Press Shot Button",software_texturing);ascii.state.scale_x=ascii.state.scale_y=1;actions.draw_text();ascii.state.string_count=0;if(!presentation::render_only&&phase>=65)phase=5;}}
    if(phase)actions.draw_capture();else if(!presentation::render_only&&actions.has_capture())actions.release_capture();
    if(presentation::render_only){ascii.state.color=saved_color;ascii.state.scale_x=saved_scale_x;ascii.state.scale_y=saved_scale_y;}
}
}
