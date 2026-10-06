#include "ApplicationState.hpp"
namespace th15::sdl {
bool ApplicationState::read_message(const std::string& name,MessageProgram& program){std::vector<u8> bytes;return read(name,bytes)&&program.open(bytes.data(),bytes.size())||fail(program.error);}
bool ApplicationState::request_animation(i32 bank,const std::string& name){if(pending_bank>=0)return fail("Overlapping ending resource requests");pending_animation_name=name;pending_bank=bank;return true;}
bool ApplicationState::prepare_animation(AnmResource& resource){return graphics.preload(resource)||fail(graphics.error);}
bool ApplicationState::cancel_animation_request(){pending_bank=-1;pending_animation.clear();pending_animation_name.clear();return true;}
bool ApplicationState::loading_overlay(){if(!transition_overlay){transition_overlay=animations.create(2,17,-1,0,{960,784,0});if(!transition_overlay)return fail(animations.error);}return true;}
bool ApplicationState::end_loading_overlay(){if(!transition_overlay)return true;const u32 handle=transition_overlay;transition_overlay=0;return animations.interrupt(handle,1)||fail(animations.error);}
bool ApplicationState::prepare_music(const std::string& name){return music(name);}
bool ApplicationState::start_music(i32){return music_command(2);}
bool ApplicationState::fade_music(i32 duration){return audio_device.queue_music(5,duration,"FadeOut")||fail(audio_device.error);}
bool ApplicationState::shake(i32 kind,i32 duration){if(kind!=0&&kind!=5)return fail("Ending requested an unsupported screen effect");return screen_fade(duration,20,81,kind==5,true);}
bool ApplicationState::reset_caption_state(){captions.spacing=9;return true;}
bool ApplicationState::ending_destination(i32 next){pending_destination=next;return true;}
}
