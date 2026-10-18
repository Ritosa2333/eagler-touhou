#include "EndingScript.hpp"
namespace th15 {
bool EndingScript::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool EndingScript::reset(const MessageScript& value,bool create_text){state=EndingState{};state.age.set(0);state.clock.set(0);state.wait.set(0);script=&value;instruction=0;finished=false;if(script->instructions.empty())return check(false,"Empty ending script");state.instruction_offset=script->instructions[0].offset;
 if(create_text)for(u32 i=0;i<5;i++)if(!check(host.create_text(i32(i)+46,state.text_handles[i]),"Ending text animation creation failed"))return false;return true;}
bool EndingScript::initialize(const MessageScript& value){error.clear();staff.scripts.clear();return reset(value,true);}
bool EndingScript::advance(){instruction++;if(!script||instruction>=script->instructions.size())return check(false,"Ending instruction outside script");state.instruction_offset=script->instructions[instruction].offset;return true;}
const char* EndingScript::staff_filename(const EndingRun& run)noexcept{if(run.player_mode&0x300)return "staff1.msg";if(run.deaths)return "staff2.msg";if(run.difficulty==1)return "staff4.msg";if(run.difficulty==2)return "staff5.msg";if(run.difficulty==3)return "staff6.msg";return "staff3.msg";}
bool EndingScript::animation_ready(i32 bank){if(!(state.flags&4)||bank!=state.loading_bank||bank<0||bank>=4)return check(false,"Unexpected ending animation completion");state.banks[u32(bank)]=bank+20;state.flags&=~4u;return check(host.end_loading_overlay(),"Ending loading overlay completion failed");}
bool EndingScript::step(const EndingInput& input){
 if(!error.empty()||!script)return false;if(finished||state.flags&4)return true;
 const bool skip=(input.held&0x200)||((input.held&1)&&input.held_frames>=20);
 for(u32 visited=0;visited<65536;visited++){
  if(instruction>=script->instructions.size())return check(false,"Ending instruction range invalid");const auto& c=script->instructions[instruction];if(state.clock.current<i32(c.time)){state.clock.tick(&input.rate);return true;}
  const i32 a=c.argument<i32>(0),b=c.argument<i32>(1),d=c.argument<i32>(2);
  switch(c.opcode){
  case 0:finished=true;return true;
  case 3:{if(state.line<0||state.line>=5)return check(false,"Ending text line outside range");if(!state.line){for(auto h:state.text_handles)if(!check(host.text(h," ",0xffffff),"Ending text clear failed")||!check(host.interrupt(h,3),"Ending text hide failed"))return false;}
   const auto h=state.text_handles[u32(state.line)];if(!check(host.text(h,c.text(),state.color),"Ending text draw failed")||!check(host.interrupt(h,2),"Ending text show failed"))return false;state.line=wrapping_add(state.line,1);if(state.line>=5)state.line=0;break;}
  case 4:for(auto h:state.text_handles)if(!check(host.interrupt(h,3),"Ending page hide failed"))return false;break;
  case 5:case 6:{if(state.wait.current<=0)state.wait.set(a);state.wait.decrement(&input.rate);if(c.opcode==5&&a<0)state.wait.set(999);
   const bool accepted=(input.pressed&0x80001)||state.wait.current<=0;
   if(!accepted){if(run.first_seen_flags&1||!skip||state.wait.current%6)return true;}
   else if(!check(host.sound(0),"Ending page sound failed"))return false;
   state.wait.set(0);if(c.opcode==6){state.line=0;if(!check(host.reset_caption_state(),"Ending caption reset failed"))return false;}break;}
  case 7:{if(a<0||a>=4)return check(false,"Ending animation bank outside range");if(!check(host.loading_overlay(),"Ending loading overlay failed"))return false;state.flags|=4;state.loading_bank=a;const auto first=c.payload.begin()+std::min<size_t>(4,c.payload.size());state.loading_name.assign(first,c.payload.end());const auto zero=state.loading_name.find(char(0));if(zero!=std::string::npos)state.loading_name.resize(zero);
   if(!check(host.load_animation(a+20,state.loading_name),"Ending animation loading failed")||!advance())return false;return true;}
  case 8:case 15:case 16:case 17:{if(c.opcode!=8&&run.difficulty!=i32(c.opcode)-14)break;if(a<0||a>=16||b<0||b>=4)return check(false,"Ending sprite resource selection outside range");auto& h=state.sprites[u32(a)];if(!check(host.retire(h),"Ending sprite retirement failed"))return false;if(state.banks[u32(b)]<0)return check(false,"Ending animation bank not prepared");if(!check(host.create_animation(state.banks[u32(b)],d,h),"Ending sprite creation failed"))return false;break;}
  case 9:state.color=u32(a);break;
  case 10:{std::string name(c.payload.begin(),c.payload.end());const auto zero=name.find(char(0));if(zero!=std::string::npos)name.resize(zero);if(!check(host.prepare_music(name),"Ending music preparation failed")||!check(host.start_music(name=="bgm/th15_14"?15:16),"Ending music start failed"))return false;break;}
  case 11:{float seconds=3;if(input.rate!=0&&!(input.rate>1.f))seconds=float(seconds/input.rate);if(!check(host.fade_music(truncate_int(seconds)),"Ending music fade failed"))return false;state.flags&=~1u;break;}
  case 12:{for(auto& h:state.text_handles)if(!check(host.retire(h),"Ending text retirement before staff failed"))return false;MessageProgram loaded;if(!check(host.read_staff(staff_filename(run),loaded),"Ending staff script load failed")||loaded.scripts.empty())return check(false,"Ending staff script unavailable");staff=std::move(loaded);if(!reset(staff.scripts[0],false))return false;state.flags=2;continue;}
  case 13:case 14:if(!check(host.shake(c.opcode==13?0:5,a),"Ending shake creation failed"))return false;break;
  default:break;
  }
  if(!advance())return false;
 }
 return check(false,"Ending command traversal exceeded resource bounds");
}
}
