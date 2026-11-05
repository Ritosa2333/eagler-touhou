#include "MusicCommands.hpp"
#include "Timer.hpp"
namespace th15 {
bool MusicCommands::fail(const char* reason){if(error.empty())error=reason;return false;}
bool MusicCommands::enqueue(i32 code,i32 value,const std::string& text){if(text.size()>=256)return fail("Music command string exceeds original capacity");for(u32 n=0;n<31;n++)if(requests[n].code==0){auto& command=requests[n];command.code=code;command.value=value;std::memcpy(command.text.data(),text.c_str(),text.size()+1);command.step=0;break;}return true;}
void MusicCommands::pop(){for(u32 n=0;n<31&&requests[n].code;n++)requests[n]=requests[n+1];}
bool MusicCommands::process(){
 if(!error.empty())return false;auto& command=requests[0];bool remove=false;const bool ready=output.music_ready();switch(command.code){
 case 1:
  if(alternate&&command.step!=0){command.step=wrapping_add(command.step,1);break;}if(alternate)output.preload_reset();
  if(command.value<0||command.value>=i32(prepared.size()))return fail("Music preload slot outside original range");prepared[command.value]=command.text.data();if(!output.prepare_music(command.value,prepared[command.value]))return fail("Music track preparation failed");remove=true;break;
 case 2:
  if(alternate&&command.value>=0){switch(command.step){case 0:if(output.cached_music(command.value))remove=true;break;case 2:if(ready)output.rewind_music_stream();break;case 5:command.value=output.music_has_intro()?1:0;if(output.fill_music(true,false)<0)remove=true;break;case 7:output.start_music_stream();break;default:if(command.step>=20)remove=true;break;}}
  else {if(!ready){remove=true;break;}switch(command.step){
   case 0:output.stop_music_stream(false);break;
   case 1:{if(output.music_busy())return true;std::string wave;if(command.value>=0){if(command.value>=i32(prepared.size()))return fail("Music play slot outside original range");wave=prepared[command.value];}else wave=command.text.data();current_wave=wave;command.value=layout.original_index(wave);if(!output.load_music(wave,command.value))return fail("Music stream load failed");break;}
   case 2:output.select_music(command.value);break;
   case 3:{const auto* track=layout.track(u32(command.value));if(!track)return fail("Music stream track unavailable");command.value=track->loop_start?1:0;if(output.fill_music(false,true)<0)remove=true;break;}
   case 4:output.start_music_stream();break;
   default:if(command.step>=7)remove=true;break;
  }}break;
 case 3:case 4:
  if(!ready){remove=true;break;}if(command.step==0)output.stop_music_stream(true);
  else if(command.code==3){if(command.step==1)remove=true;}
  else {if(command.step==1){if(!output.release_pending())remove=true;else output.signal_music_release();}else if(command.step==2){if(output.release_waiting()){output.signal_music_release();command.step=wrapping_add(command.step,-1);}}else if(command.step==3)output.close_music_stream();else if(command.step==10)remove=true;}
  break;
 case 5:if(ready)output.fade_music_stream(truncate_int(float(float(command.value)*60.f)));remove=true;break;
 case 6:case 7:if(music_mode==1){if(output.music_busy())return true;if(ready)output.pause_music_stream(command.code==6);}remove=true;break;
 case 8:if(ready)output.refresh_music_volume();remove=true;break;
 case 9:output.switch_music_wave(layout.original_index(command.text.data()));remove=true;break;
 default:return true;
 }
 if(remove)pop();else command.step=wrapping_add(command.step,1);
 // Original compaction advances its scan pointer to the empty tail. Its
 // preload re-entry therefore does not consume the new head in this pass.
 return true;
}
}
