#include "ReplayRecording.hpp"
#include "Lzss.hpp"
#include "ResourceCrypt.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
namespace th15 {
namespace {
u32 dword(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 word(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
template<class T>void put(u8* p,T value){std::memcpy(p,&value,sizeof value);}
void append(std::vector<u8>& out,const void* p,u32 n){auto* bytes=static_cast<const u8*>(p);out.insert(out.end(),bytes,bytes+n);}
void user_block(std::vector<u8>& out,u8 kind,const std::string& text){
 const u32 size=(12+u32(text.size())+1+3)&~3u;const u32 start=out.size();out.resize(start+size,0);
 put(out.data()+start,u32(0x52455355));put(out.data()+start+4,size);out[start+8]=kind;
 std::memcpy(out.data()+start+12,text.data(),text.size());
}
}
bool ReplayRecording::fail(const char* reason){if(failure.empty())failure=reason;return false;}
bool ReplayRecording::begin(const u8* header,u32 size){
 if(!failure.empty())return false;if(sealed)return fail("Recording has already been exported");
 if(!header||size!=0x238)return fail("Invalid recording stage snapshot");const u32 n=word(header);
 if(n<1||n>7)return fail("Invalid recording stage number");auto& s=stages[n];s={};s.present=true;
 std::copy(header,header+size,s.snapshot.begin());put(s.snapshot.data()+4,u32(0));put(s.snapshot.data()+8,u32(0));current=n;clock=-1;return true;
}
bool ReplayRecording::tick(ReplayInput input,float fps,bool suspended,PlayerTouch touch){
 if(!failure.empty())return false;
 // The original input callback remains valid after export. The exported flag
 // prevents another termination sentinel, but does not freeze clock or inputs.
 if(!current||clock<0)return true;
 if(!touch.valid())return fail("Invalid recording touch target");
 if(!suspended){auto& s=stages[current];if(s.inputs.size()>=2*1024*1024)return fail("Replay recording exceeds input limit");
  if(touch.mode)s.touches.push_back({u32(s.inputs.size()),touch});
  if(clock%30==0){const float rounded=fps+0.5f;i32 value=std::numeric_limits<i32>::min();
   if(rounded>=256)s.frame_rates.push_back(255);
   else {if(std::isfinite(rounded)&&double(rounded)>=-2147483648.0&&double(rounded)<2147483648.0)value=i32(rounded);s.frame_rates.push_back(u8(value));}
  }s.inputs.push_back({input.held,input.pressed,input.released});
 }clock=wrapping_add(clock,1);return true;
}
void ReplayRecording::finish(i64 timestamp,i32 number,bool cleared){put(metadata.data()+0xc,timestamp);put(metadata.data()+0x98,cleared?8:number);}
bool ReplayRecording::write(const char* name,const ReplayExportDetails& details,bool terminate){
 if(!failure.empty())return false;if(!name||std::strlen(name)>8)return fail("Replay name exceeds eight bytes");
 if(dword(metadata.data()+0x8c)>3||dword(metadata.data()+0x90)!=0||dword(metadata.data()+0x94)>4)return fail("Invalid recording selection");
 bool any=false;for(u32 i=1;i<8;i++)any|=stages[i].present;if(!any)return fail("No replay stages to export");
 if(!sealed&&terminate&&current)stages[current].inputs.push_back({65535,65535,65535});
 if(!serialize(name,details))return false;sealed=true;return true;
}
bool ReplayRecording::serialize(const char* name,const ReplayExportDetails& details){
 std::fill(metadata.begin(),metadata.begin()+9,0);std::memcpy(metadata.data(),name,std::strlen(name));std::fill(metadata.begin()+std::strlen(name),metadata.begin()+8,' ');
 u32 count=0,first=0,last=0,total=0xa4;for(u32 i=1;i<8;i++)if(stages[i].present){count++;if(!first)first=i;last=i;const auto& s=stages[i];const u64 size=u64(s.inputs.size())*6+s.frame_rates.size();
  if(size>64*1024*1024||u64(total)+0x238+size>64*1024*1024)return fail("Replay recording exceeds file limit");total+=0x238+u32(size);
 }
 const float fraction=float(details.elapsed/details.total),slow=100.0f-fraction*100.0f;
 put(metadata.data()+0x84,slow);put(metadata.data()+0x88,count);put(metadata.data()+0x14,details.score);
 std::vector<u8> decoded;decoded.reserve(total);append(decoded,metadata.data(),metadata.size());
 for(u32 i=1;i<8;i++)if(stages[i].present){auto& s=stages[i];put(s.snapshot.data()+4,u32(s.inputs.size()));put(s.snapshot.data()+8,u32(s.inputs.size()*6+s.frame_rates.size()));
  append(decoded,s.snapshot.data(),s.snapshot.size());for(const auto& input:s.inputs)for(auto v:input)append(decoded,&v,2);if(!s.frame_rates.empty())append(decoded,s.frame_rates.data(),s.frame_rates.size());
 }
 Lzss codec;const auto packed=codec.encode(decoded.data(),decoded.size());if(packed.empty())return fail("Replay compression failed");const u32 size=packed.size();std::vector<u8> a(size),b(size);
 if(!resource_crypt(packed.data(),a.data(),size,{0x7d,0x3a,0x100,size},true)||!resource_crypt(a.data(),b.data(),size,{0x5c,0xe1,0x400,size},true))return fail("Replay encryption failed");
 file.assign(0x24,0);put(file.data(),u32(0x72353174));put(file.data()+4,u16(3));put(file.data()+0xc,size+0x24);put(file.data()+0x10,u32(0x100));put(file.data()+0x1c,size);put(file.data()+0x20,u32(decoded.size()));append(file,b.data(),size);
 std::string info="\x93\x8c\x95\xfb\x8d\xae\x8e\xec\x93\x60 \x83\x8a\x83\x76\x83\x8c\x83\x43\x83\x74\x83\x40\x83\x43\x83\x8b\x8f\xee\x95\xf1\r\nVersion 1.00b\r\nName ";info.append(reinterpret_cast<const char*>(metadata.data()),8);info+="\r\n";
 char formatted[128];std::snprintf(formatted,sizeof formatted,"Date %.2d/%.2d/%.2d %.2d:%.2d\r\n",details.year%100,details.month,details.day,details.hour,details.minute);info+=formatted;
 static const char* characters[]={"Reimu  ","Marisa ","Sanae  ","Reisen "};static const char* ranks[]={"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};
 info+="Chara ";info+=characters[dword(metadata.data()+0x8c)];info+="\r\nRank ";info+=ranks[dword(metadata.data()+0x94)];info+="\r\n";
 if(signed_bits(dword(metadata.data()+0x98))>7)info+=first==7?"Extra Stage Clear\r\n":"Stage All Clear\r\n";
 else if(first==last){if(first==7)info+="Extra Stage\r\n";else{std::snprintf(formatted,sizeof formatted,"Stage %d\r\n",first);info+=formatted;}}
 else {std::snprintf(formatted,sizeof formatted,"Stage %d \x81\x60 %d\r\n",first,last);info+=formatted;}
 std::snprintf(formatted,sizeof formatted,"Score %d\r\nSlow Rate %2.2f\r\n",details.score,double(slow));info+=formatted;
 user_block(file,0,info);user_block(file,1,"\x83\x52\x83\x81\x83\x93\x83\x67\x82\xf0\x8f\x91\x82\xaf\x82\xdc\x82\xb7");
 u64 count_touches=0;for(const auto& s:stages)count_touches+=s.touches.size();
 if(count_touches){if(count_touches>(64*1024*1024-32)/16||file.size()+32+count_touches*16>128*1024*1024)return fail("Replay touch data exceeds file limit");const u32 start=file.size(),length=32+u32(count_touches)*16;file.resize(start+length,0);auto* out=file.data()+start;
  put(out,u32(0x52455355));put(out+4,length);put(out+8,u32(0x15));put(out+12,u32(0x54353154));put(out+16,u32(1));put(out+20,u32(count_touches));out+=32;
  for(u32 stage=1;stage<8;stage++)for(const auto& t:stages[stage].touches){put(out,u16(stage));put(out+2,u16(t.motion.mode));put(out+4,t.frame);put(out+8,t.motion.x);put(out+12,t.motion.y);out+=16;}
 }if(!practice_block.empty())append(file,practice_block.data(),u32(practice_block.size()));return true;
}
bool ReplayRecording::spell_timing(u32 number,u32 sequence,i32 value){
 if(!failure.empty())return false;
 if(number<1||number>7||!stages[number].present||sequence>=20)return fail("Recording spell timing outside stage snapshot");
 put(stages[number].snapshot.data()+0x1e4+sequence*4,value);return true;
}

}
