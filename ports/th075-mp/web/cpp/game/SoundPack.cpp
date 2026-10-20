#include "SoundPack.hpp"
#include <cmath>
namespace th075 {
namespace {void word(std::vector<u8>& out,u32 value,u32 n){for(u32 i=0;i<n;i++)out.push_back(u8(value>>(i*8)));}}
std::vector<u8> SoundSample::wave_file()const{
 if(!present)return {};std::vector<u8> out;out.reserve(44+pcm.size());const auto text=[&](const char* s){out.insert(out.end(),s,s+4);};
 text("RIFF");word(out,36+u32(pcm.size()),4);text("WAVE");text("fmt ");word(out,16,4);word(out,format.tag,2);word(out,format.channels,2);word(out,format.sampleRate,4);word(out,format.bytesPerSecond,4);word(out,format.blockAlign,2);word(out,format.bitsPerSample,2);text("data");word(out,u32(pcm.size()),4);out.insert(out.end(),pcm.begin(),pcm.end());return out;
}
bool SoundPack::load(const void* data,std::size_t size){
 samples.clear();error.clear();ByteReader in(data,size);const u32 count=in.dword();if(!in.valid||count>1000){error="Invalid sound count";return false;}std::vector<SoundSample> parsed(count);
 for(auto& sample:parsed){sample.present=in.byte()!=0;if(!in.valid){error="Truncated sound directory";return false;}if(!sample.present)continue;
  const u32 bytes=in.dword();auto& f=sample.format;f.tag=in.word();f.channels=in.word();f.sampleRate=in.dword();f.bytesPerSecond=in.dword();f.blockAlign=in.word();f.bitsPerSample=in.word();f.extraSize=in.word();const auto* p=in.take(bytes);
  if(!p||f.tag!=1||f.channels<1||f.channels>2||!f.sampleRate||(f.bitsPerSample!=8&&f.bitsPerSample!=16)||f.blockAlign!=f.channels*f.bitsPerSample/8||bytes%f.blockAlign!=0||f.extraSize){error="Invalid original PCM sample";return false;}sample.pcm.assign(p,p+bytes);
 }samples=std::move(parsed);return true;
}
MusicLoop original_music_loop(const void* source,std::size_t size,i16 cue){
 if(cue==59)return {0x60e8ac,0x5e0d90};MusicLoop result;const auto* data=static_cast<const u8*>(source);if(size<320)return result;data+=size-320;u32 start=0,length=0;
 for(u32 i=0;i+20<=320;i++){if(data[i]=='c'&&data[i+1]=='u'&&data[i+2]=='e')std::memcpy(&start,data+i+16,4);else if(data[i]=='a'&&data[i+1]=='d'&&data[i+2]=='t'&&data[i+3]=='l')std::memcpy(&length,data+i+16,4);}
 if(start&&length){result.endBytes=(start+length)*4;result.lengthBytes=length*4;}return result;
}
std::string music_resource_path(i16 cue){if(cue<0)return {};if(cue==40)return "wave/bgm/sys00_op.wav";if(cue==41)return "wave/bgm/sys99_ed.wav";const i32 n=cue<10?cue:cue<20?cue-10:cue<30?cue-20:cue;return "wave/bgm/"+std::string(n<10?"0":"")+std::to_string(n)+(cue<10?"a":cue<20?"b":cue<30?"c":"")+".wav";}
float sound_volume_gain(i32 volume){return std::pow(10.f,float((volume-100)*50)/2000.f);}
}
