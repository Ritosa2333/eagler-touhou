#include "EndingScene.hpp"
namespace th17 {
EndingScene::EndingScene(AnmManager& a,RecordStore& r,EndingScenePlatform& h,FrameScheduler& f,i32 text):animations(a),records(r),host(h),text_bank(text),script(*this),frame(script,h,f){}
EndingScene::~EndingScene(){host.cancel_animation_request();for(auto& h:script.state.text_handles)animations.retire(h);for(i32 bank=20;bank<24;bank++)animations.unload(bank);}
bool EndingScene::check(bool ok,const std::string& why){if(!ok&&error.empty())error=why.empty()?"Ending scene operation failed":why;return ok;}
const char* EndingScene::filename(i32 index)noexcept{static const char* names[]={"e01.msg","e02.msg","e03.msg","e04.msg","e05.msg","e06.msg","e07.msg","e08.msg","e09.msg","staff1.msg","staff2.msg","staff3.msg","staff4.msg","e10.msg","e11.msg","e12.msg"};return u32(index)<16?names[index]:nullptr;}
bool EndingScene::initialize(const GameRun& progress,i32 preview){
 const i32 index=preview>=0?preview:ending_index(progress.character,progress.animal,progress.scoring.continues);const char* name=filename(index);
 if(!check(name&&u32(progress.difficulty)<5&&u32(progress.character)<3&&u32(progress.animal)<3,"Ending selection outside original range"))return false;
 script.run={progress.difficulty,progress.scoring.continues,0,preview>=0};if(!check(host.loading_overlay(),"Ending initial loading overlay failed"))return false;
 if(preview<0){if(!records.endings[index])script.run.first_seen_flags|=1;if(!records.endings[9])script.run.first_seen_flags|=2;records.endings[index]|=u8(progress.difficulty>=1&&progress.difficulty<=3?1u|(1u<<progress.difficulty):1u);if(index<9)records.endings[9]=1;}
 if(!check(host.read_message(name,program),"Ending message load failed")||program.scripts.empty())return check(false,"Ending message script unavailable");
 if(!check(script.initialize(program.scripts[0]),script.error))return false;if(index>8)script.state.flags|=2;return true;
}
bool EndingScene::animation_ready(i32 bank,const u8* bytes,u32 size){if(bank<20||bank>=24||script.state.loading_bank!=bank-20||!(script.state.flags&4))return check(false,"Ending animation completion does not match pending bank");animations.unload(bank);if(!check(animations.load(bank,bytes,size),animations.error)||!check(host.prepare_animation(*animations.resource(bank)),"Ending graphics resource preparation failed"))return false;return check(script.animation_ready(bank-20),script.error);}
bool EndingScene::create_text(i32 index,u32& handle){handle=animations.create(text_bank,index,-1,0);auto* vm=animations.registry.find(handle);if(!vm)return check(false,animations.error);vm->visual.render_flags&=~0x1000u;return true;}
bool EndingScene::text(u32 h,const std::string& bytes,u32 color){auto* vm=animations.registry.find(h);if(!vm)return check(false,"Ending text animation unavailable");TextureText request;request.bytes=bytes;request.color=color;return check(host.text(*vm,request),"Ending text upload failed");}
bool EndingScene::interrupt(u32 h,i32 label){return check(animations.interrupt(h,label),animations.error);}bool EndingScene::retire(u32& h){return check(animations.retire(h),animations.error);}
bool EndingScene::load_animation(i32 bank,const std::string& name){animations.unload(bank);return check(host.request_animation(bank,name),"Ending animation request failed");}
bool EndingScene::create_animation(i32 bank,i32 index,u32& h){h=animations.create(bank,index,-1,0);return check(h!=0,animations.error);}
bool EndingScene::loading_overlay(){return host.loading_overlay();}bool EndingScene::end_loading_overlay(){return host.end_loading_overlay();}bool EndingScene::read_staff(const std::string& name,MessageResource& out){return host.read_message(name,out);}
bool EndingScene::prepare_music(const std::string& name){return host.prepare_music(name);}bool EndingScene::start_music(i32 track){return records.unlock_music(track)&&host.start_music(track);}bool EndingScene::fade_music(i32 seconds){return host.fade_music(seconds);}bool EndingScene::sound(i32 id){return host.sound(id);}bool EndingScene::shake(i32 kind,i32 amount){return host.shake(kind,amount);}bool EndingScene::reset_caption_state(){return host.reset_caption_state();}
}
