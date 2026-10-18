// Purple THPracParam::GetJson/ReadJson + ReplaySaveParam USER/PRAC owner.
#include "PracticeConfig.hpp"
#include "PracticeSections.hpp"
#include "PracticeVersion.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>
#include <type_traits>
namespace th15 {
namespace {
u32 word(const u8* p){u32 value;std::memcpy(&value,p,4);return value;}
struct Value {char type=0;double number=0;bool boolean=false;std::string text;};
// THPrac metadata is a flat JSON object. Parse its actual schema, including
// duplicate/type/length checks; malformed replay data never becomes live state.
bool object(const char* bytes,u32 size,std::map<std::string,Value>& values){
 std::string data(bytes,size);const char* p=data.c_str();const char* end=p+size;
 auto space=[&](){while(p<end&&(*p==' '||*p=='\r'||*p=='\n'||*p=='\t'))++p;};
 auto take=[&](char c){space();if(p==end||*p!=c)return false;++p;return true;};
 auto string=[&](std::string& value){if(!take('"'))return false;while(p<end&&*p!='"'){if(u8(*p)<32||*p=='\\')return false;value+=*p++;}if(p==end)return false;++p;return true;};
 if(!take('{'))return false;space();if(p<end&&*p=='}'){++p;space();return p==end;}
 for(;;){std::string key;Value value;if(!string(key)||!take(':')||values.count(key))return false;space();if(p==end)return false;
  if(*p=='"'){value.type='s';if(!string(value.text))return false;}
  else if(end-p>=4&&std::memcmp(p,"true",4)==0){p+=4;value.type='b';value.boolean=true;}
  else if(end-p>=5&&std::memcmp(p,"false",5)==0){p+=5;value.type='b';}
  else {
   const char* begin=p;if(*p=='-')++p;if(p==end||*p<'0'||*p>'9')return false;
   if(*p=='0')++p;else while(p<end&&*p>='0'&&*p<='9')++p;
   if(p<end&&*p=='.'){++p;const char* fraction=p;while(p<end&&*p>='0'&&*p<='9')++p;if(p==fraction)return false;}
   if(p<end&&(*p=='e'||*p=='E')){++p;if(p<end&&(*p=='+'||*p=='-'))++p;const char* exponent=p;while(p<end&&*p>='0'&&*p<='9')++p;if(p==exponent)return false;}
   char* parsed=nullptr;value.number=std::strtod(begin,&parsed);if(parsed!=p||!std::isfinite(value.number))return false;value.type='n';
  }
  values.emplace(std::move(key),std::move(value));space();if(p<end&&*p=='}'){++p;space();return p==end;}if(!take(','))return false;
 }
}
}
std::string practice_replay_json(const PracticeConfig& p){
 if(!p.valid())return {};char buffer[1536];int n=0;
 auto append=[&](const char* fmt,auto... values){const int count=std::snprintf(buffer+n,sizeof(buffer)-n,fmt,values...);if(count<0||count>=int(sizeof(buffer))-n)return false;n+=count;return true;};
 if(!append("{\"version\":\"%s\",\"game\":\"th15\",\"mode\":%d,\"stage\":%d",practice_source_version,p.mode,p.stage))return {};
 if(p.section&&!append(",\"section\":%d",p.section))return {};
 if(p.phase&&!append(",\"phase\":%d",p.phase))return {};
 if(p.dlg&&!append("%s",",\"dlg\":true"))return {};
 if(p.section==TH15_ST3_BOSS1&&!append(",\"doremy_normal_1_phase\":%.9g",double(p.doremy_normal_1_phase)))return {};
 if(p.section==TH15_ST6_BOSS6&&!append(",\"enhanced_para\":%.9g",double(p.enhanced_para)))return {};
 if(!append(",\"score\":%lld,\"life\":%d,\"life_fragment\":%d,\"bomb\":%d,\"bomb_fragment\":%d,\"power\":%d,\"value\":%d,\"graze\":%d",static_cast<long long>(p.score),p.life,p.life_fragment,p.bomb,p.bomb_fragment,p.power,p.value,p.graze))return {};
 if(p.reisen_shield&&!append(",\"reisen_shield\":%d",p.reisen_shield))return {};
 if(!append("%s","}"))return {};return std::string(buffer,n);
}
bool practice_replay_parse(const char* bytes,u32 size,PracticeConfig& output){
 output.reset();if(!bytes||!size||size>4096)return false;std::map<std::string,Value> fields;
 if(!object(bytes,size,fields)||fields["game"].type!='s'||fields["game"].text!="th15")return false;
 PracticeConfig p;p.reset();bool valid=true;
 auto number=[&](const char* key,auto& destination){auto found=fields.find(key);if(found==fields.end())return;const auto& v=found->second;if(v.type!='n'){valid=false;return;}using T=std::decay_t<decltype(destination)>;if constexpr(std::is_integral_v<T>){if(std::trunc(v.number)!=v.number||v.number<0||v.number>(sizeof(T)==8?9999999990.:2147483647.)){valid=false;return;}}destination=T(v.number);};
 if(!fields.count("mode")||!fields.count("stage"))return false;
 number("mode",p.mode);number("stage",p.stage);number("section",p.section);number("phase",p.phase);number("score",p.score);
 number("life",p.life);number("life_fragment",p.life_fragment);number("bomb",p.bomb);number("bomb_fragment",p.bomb_fragment);number("power",p.power);number("value",p.value);number("graze",p.graze);number("reisen_shield",p.reisen_shield);number("doremy_normal_1_phase",p.doremy_normal_1_phase);number("enhanced_para",p.enhanced_para);
 // Purple writes bomb_fragment but ReadJson omits it. Preserve the recorded
 // field in the portable reader; old replays with no field retain zero.
 auto dlg=fields.find("dlg");if(dlg!=fields.end()){if(dlg->second.type!='b')return false;p.dlg=dlg->second.boolean;}
 if(!valid||!p.valid())return false;output=p;return true;
}
std::vector<u8> practice_replay_block(const PracticeConfig& config){
 const auto json=practice_replay_json(config);if(json.empty())return {};const u32 length=(u32(json.size())+16)&~3u;std::vector<u8> block(length,0);
 std::memcpy(block.data(),"USER",4);std::memcpy(block.data()+4,&length,4);std::memcpy(block.data()+8,"PRAC",4);std::memcpy(block.data()+12,json.data(),json.size());return block;
}
i32 practice_replay_status(const u8* bytes,u32 size,PracticeConfig& config){
 config.reset();if(!bytes||size<0x24||word(bytes)!=0x72353174u||(bytes[4]|u32(bytes[5])<<8)!=3)return -1;
 const u32 packed=word(bytes+0x1c);if(!packed||packed>size-0x24)return -1;
 bool found=false;PracticeConfig candidate;
 for(u32 offset=0x24+packed;size-offset>=12;){if(std::memcmp(bytes+offset,"USER",4))break;const u32 length=word(bytes+offset+4);if(length<12||length>size-offset)return -1;
  if(std::memcmp(bytes+offset+8,"PRAC",4)==0){if(found)return -1;const char* json=reinterpret_cast<const char*>(bytes+offset+12);u32 n=0;while(n<length-12&&json[n])++n;if(n==length-12||!practice_replay_parse(json,n,candidate))return -1;found=true;}
  offset+=length;
 }
 if(found)config=candidate;return found?1:0;
}
bool practice_replay_read(const u8* bytes,u32 size,PracticeConfig& config){return practice_replay_status(bytes,size,config)==1;}
}
