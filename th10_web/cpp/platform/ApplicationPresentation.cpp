#include "Application.hpp"
#include <cstdlib>
namespace th10::browser {
namespace{u32 pointer(const void* value){return static_cast<u32>(reinterpret_cast<uintptr_t>(value));}}
AppPresentation::AppPresentation(Application& a):owner(a){application=&a.value;animations=&a.manager;pressed_keys=&a.input.player_profiles[0].input.raw_pressed;reset_frames=&a.reset_frames;presentation_parameters=&a.parameters;}
i32 AppPresentation::present(void*){return owner.engine.device.present_frame();}
void AppPresentation::reset_device(void*,void* params){if(owner.engine.device.reset_presentation(params)<0)owner.error=-4;}
void AppPresentation::release_surface(void* surface){owner.engine.device.release_resource(surface);}
#ifdef TH_NATIVE_PLATFORM
void AppPresentation::configure_graphics(){owner.engine.device.host.configure_game(1);}
#else
void AppPresentation::render_state(void*,u32 key,u32 value){owner.engine.device.render_state(key,value);}
void AppPresentation::texture_stage(void*,u32 stage,u32 key,u32 value){owner.engine.device.texture_stage(stage,key,value);}
void AppPresentation::sampler_state(void*,u32 sampler,u32 key,u32 value){owner.engine.device.sampler(sampler,key,value);}
#endif
void AppPresentation::capture_texture(AnmManager&,i32 file,u32 flags,CaptureRectangle source,CaptureRectangle target){owner.captures.pixels().capture_texture(file,flags,source,target);}
void AppPresentation::capture_screen(AnmManager&,i32 slot,CaptureRectangle source,CaptureRectangle target){owner.captures.pixels().capture_screen(slot,source,target);}
void AppPresentation::create_directory(const char*){} // FileStore paths have implicit directories.
bool AppPresentation::file_exists(const char* path){return ResourceFiles{owner.files}.exists(path);}
void AppPresentation::save_screenshot(ApplicationState& app,const char* name){Screenshot{owner.screenshots}.capture(app,owner.screenshot,owner.captures.surface_format,name);}
AppScreenshot::AppScreenshot(Application& a):owner(a){global=&a.screenshot;output_handle=&a.screenshot_handle;}
void AppScreenshot::sleep(u32){if(owner.writer_pending){owner.writer_pending=false;Screenshot{*this}.write();}}
void* AppScreenshot::back_buffer(void*){return owner.engine.device.back_surface();}
void* AppScreenshot::allocate(u32 bytes){return std::malloc(bytes);}void AppScreenshot::free_bytes(void* bytes){std::free(bytes);}
TextureLock AppScreenshot::lock_surface(void* surface){return owner.captures.textures.lock_surface(surface);}
void AppScreenshot::unlock_surface(void* surface){owner.captures.textures.unlock_surface(surface);}
void AppScreenshot::release_surface(void* surface){owner.captures.release_surface(surface);}
u32 AppScreenshot::begin_writer(){owner.writer_pending=true;return 1;}
void AppScreenshot::report(ScreenshotError error){owner.screenshot_error=static_cast<i32>(error)+1;}
void AppScreenshot::open_output(const char* name){owner.screenshot_handle=owner.files.host.open(name,true);}
void AppScreenshot::write_output(const u8* bytes,u32 length,u32& actual){actual=owner.screenshot_handle==0xffffffff?0:owner.files.host.write(owner.screenshot_handle,bytes,length);}
void AppScreenshot::close_output(){if(owner.screenshot_handle!=0xffffffff)owner.files.host.close(owner.screenshot_handle);owner.screenshot_handle=0xffffffff;}
AppConfiguration::AppConfiguration(Application& a):owner(a){global=&a.state.configuration;keys=reinterpret_cast<u16*>(a.input.player_profiles[0].bindings);disable_vsync=&a.disable_vsync;}
u8* AppConfiguration::load(const char* name,u32* length){
    auto* bytes=ResourceFiles{owner.files}.load(name,length,true);
    // The original copies all thirteen words before checking file length.
    // Pad a truncated file so that its invalid-size path cannot read outside
    // a browser allocation; validation still rejects the original length.
    if(bytes&&*length<sizeof(ApplicationConfig)){auto* padded=static_cast<u8*>(std::calloc(1,sizeof(ApplicationConfig)));if(padded)std::memcpy(padded,bytes,*length);std::free(bytes);bytes=padded;}return bytes;
}
void AppConfiguration::release(void* p){std::free(p);}
i32 AppConfiguration::save(const char* name,const ApplicationConfig& value){return ResourceFiles{owner.files}.save(name,reinterpret_cast<const u8*>(&value),sizeof(value));}
void AppConfiguration::notice(ConfigurationNotice code,const char*){owner.notice_code=static_cast<i32>(code);}
}
