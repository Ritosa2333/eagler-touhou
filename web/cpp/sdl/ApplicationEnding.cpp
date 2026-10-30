#include "ApplicationState.hpp"
namespace th17::sdl {
bool ApplicationState::read_message(const std::string& name,MessageResource& program){std::vector<u8> bytes;return read(name,bytes)&&program.open(bytes.data(),bytes.size(),true)||fail(program.error);}
bool ApplicationState::request_animation(i32 bank,const std::string& name){if(pending_bank>=0)return fail("Overlapping ending animation requests");if(!read(name,pending_animation))return false;pending_bank=bank;return true;}
bool ApplicationState::prepare_animation(AnmResource& resource){return graphics.preload(resource)||fail(graphics.error);}
bool ApplicationState::cancel_animation_request(){pending_bank=-1;pending_animation.clear();return true;}
bool ApplicationState::loading_overlay(){if(!loading_handle){
 // ascii.anm loading scripts use half-scale coordinates (opcode 313 = 2).
 // Native 0x41d3d0 doubles (480,392) before binding this same animation.
 loading_handle=animations.create(0,17,-1,0,{960,784,0});if(!loading_handle)return fail(animations.error);
 }return true;}
bool ApplicationState::end_loading_overlay(){if(!loading_handle)return true;const u32 handle=loading_handle;loading_handle=0;return animations.interrupt(handle,1)||fail(animations.error);}
bool ApplicationState::prepare_music(const std::string& name){return music(name);}
bool ApplicationState::start_music(i32){return music_command(2);}
bool ApplicationState::fade_music(i32 duration){return audio_device.queue_music(5,duration,"FadeOut")||fail(audio_device.error);}
bool ApplicationState::shake(i32 kind,i32 duration){if(kind!=0&&kind!=5)return fail("Unsupported ending screen effect");return screen_fade(duration,20,85,kind==5,true);}
bool ApplicationState::reset_caption_state(){captions.spacing=9;return true;}
bool ApplicationState::ending_destination(i32 next){flow.pending=AppScene(next);return true;}
}
