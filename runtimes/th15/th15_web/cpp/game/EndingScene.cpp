#include "EndingScene.hpp"
#include <cstdio>
namespace th15 {
EndingScene::EndingScene(AnmManager& a,RecordStore& r,EndingScenePlatform& platform,FrameScheduler& schedule,i32 text):animations(a),records(r),host(platform),text_bank(text),script(*this),frame(script,platform,schedule){}
EndingScene::~EndingScene(){host.cancel_animation_request();for(auto& h:script.state.text_handles)animations.retire(h);for(i32 bank=20;bank<24;bank++)animations.unload(bank);}
bool EndingScene::check(bool ok,const std::string& why){if(!ok&&error.empty())error=why.empty()?"Ending scene operation failed":why;return ok;}
bool EndingScene::initialize(const SessionState& progress,u32 player_mode,i32 deaths){const i32 index=ending_index(progress.character,progress.subcharacter,player_mode,deaths);script.run={progress.difficulty,progress.continues,player_mode,0,deaths};if(!check(host.loading_overlay(),"Ending initial loading overlay failed")||!check(records.begin_ending(index,progress.difficulty,script.run.first_seen_flags),records.error))return false;char name[16];std::snprintf(name,sizeof name,"e%02d.msg",index+1);if(!check(host.read_message(name,program),"Ending message load failed")||program.scripts.empty())return check(false,"Ending message script unavailable");return check(script.initialize(program.scripts[0]),script.error);}
bool EndingScene::animation_ready(i32 bank,const u8* bytes,u32 size){if(bank<20||bank>=24||script.state.loading_bank!=bank-20||!(script.state.flags&4))return check(false,"Ending animation completion does not match pending bank");animations.unload(bank);if(!check(animations.load(bank,bytes,size),animations.error)||!check(host.prepare_animation(*animations.resource(bank)),"Ending graphics resource preparation failed"))return false;return check(script.animation_ready(bank-20),script.error);}
bool EndingScene::create_text(i32 index,u32& handle){handle=animations.create(text_bank,index,-1,0);auto* vm=animations.registry.find(handle);if(!vm)return check(false,animations.error);vm->visual.render_flags&=~0x1000u;return true;}
bool EndingScene::text(u32 handle,const std::string& bytes,u32 color){auto* vm=animations.registry.find(handle);if(!vm)return check(false,"Ending text animation unavailable");return check(host.text(*vm,{handle,bytes,0,0,color}),"Ending text upload failed");}
bool EndingScene::interrupt(u32 h,i32 label){return check(animations.interrupt(h,label),animations.error);}
bool EndingScene::retire(u32& h){return check(animations.retire(h),animations.error);}
bool EndingScene::load_animation(i32 bank,const std::string& name){animations.unload(bank);return check(host.request_animation(bank,name),"Ending animation request failed");}
bool EndingScene::create_animation(i32 bank,i32 index,u32& handle){handle=animations.create(bank,index,-1,0);return check(handle!=0,animations.error);}
bool EndingScene::loading_overlay(){return host.loading_overlay();}bool EndingScene::end_loading_overlay(){return host.end_loading_overlay();}
bool EndingScene::read_staff(const std::string& name,MessageProgram& out){return host.read_message(name,out);}
bool EndingScene::prepare_music(const std::string& stem){return host.prepare_music(stem);}bool EndingScene::start_music(i32 track){return records.unlock_music(track)&&host.start_music(track);}
bool EndingScene::fade_music(i32 seconds){return host.fade_music(seconds);}bool EndingScene::sound(i32 id){return host.sound(id);}
bool EndingScene::shake(i32 kind,i32 amount){return host.shake(kind,amount);}bool EndingScene::reset_caption_state(){return host.reset_caption_state();}
}
